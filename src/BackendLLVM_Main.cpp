#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "Context.h"
#include "Workspace.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool BackendLLVM::emitOS(const BuildParameters& buildParameters)
{
    if (g_CommandLine->target.os == SwagTargetOs::Windows)
    {
        int   ct              = buildParameters.compileType;
        int   precompileIndex = buildParameters.precompileIndex;
        auto& pp              = *perThread[ct][precompileIndex];
        auto& context         = *pp.context;
        auto& builder         = *pp.builder;
        auto& modu            = *pp.module;

        // int _DllMainCRTStartup(void*, int, void*)
        {
            VectorNative<llvm::Type*> params;
            params.push_back(builder.getInt8PtrTy());
            params.push_back(builder.getInt32Ty());
            params.push_back(builder.getInt8PtrTy());
            llvm::FunctionType* FT = llvm::FunctionType::get(builder.getInt32Ty(), {params.begin(), params.end()}, false);
            llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "_DllMainCRTStartup", modu);
            llvm::BasicBlock*   BB = llvm::BasicBlock::Create(context, "entry", F);
            builder.SetInsertPoint(BB);
            builder.CreateRet(builder.getInt32(1));
        }

        // Stack probing. Must do it by hand to avoid linking with vc runtime on windows.
        // void __chkstk()
        if (g_CommandLine->target.arch == SwagTargetArch::X86_64)
        {
            Utf8 in;
            in.append(R"(
            __chkstk:
            push    %rcx
            push    %rax
            cmp     $0x1000, %rax
            lea     24(%rsp), %rcx
            jb      1f
            2:
            sub     $0x1000, %rcx
            test    %rcx, (%rcx)
            sub     $0x1000, %rax
            cmp     $0x1000, %rax
            ja      2b
            1:
            sub     %rax, %rcx
            test    %rcx, (%rcx)
            pop     %rax
            pop     %rcx
            ret)");

            in.append("\n");
            in.append(".global __chkstk\n");
            modu.setModuleInlineAsm(in.c_str());
        }

        // int _fltused = 0;
        new llvm::GlobalVariable(modu, builder.getInt32Ty(), false, llvm::GlobalValue::ExternalLinkage, builder.getInt32(0), "_fltused");
    }

    return true;
}

bool BackendLLVM::emitMain(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    SWAG_CHECK(emitOS(buildParameters));

    const char* entryPoint = nullptr;
    switch (g_CommandLine->target.os)
    {
    case SwagTargetOs::Windows:
        entryPoint = "mainCRTStartup";
        break;
    default:
        entryPoint = "main";
        break;
    }

    // void mainCRTStartup()
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {}, false);
    llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, entryPoint, modu);
    llvm::BasicBlock*   BB = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);

    // Reserve room to pass parameters to embedded intrinsics
    auto allocT = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(2));

    // Set default system allocator function
    SWAG_ASSERT(g_DefaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) ByteCode::undoByteCodeLambda(((void**) g_DefaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    auto allocFct = modu.getOrInsertFunction(bcAlloc->getCallName().c_str(), pp.allocatorTy);
    builder.CreateStore(allocFct.getCallee(), pp.defaultAllocTable);

    // mainContext.allocator.itable = &defaultAllocTable
    {
        auto toTable   = builder.CreateInBoundsGEP(pp.mainContext, {pp.cst0_i32, pp.cst0_i32, pp.cst1_i32});
        auto fromTable = builder.CreatePointerCast(pp.defaultAllocTable, llvm::Type::getInt8PtrTy(context));
        builder.CreateStore(fromTable, toTable);
    }

    // mainContext.flags = 0
    {
        auto     toFlags      = builder.CreateInBoundsGEP(pp.mainContext, {pp.cst0_i32, pp.cst1_i32});
        uint64_t contextFlags = getDefaultContextFlags(module);
        builder.CreateStore(builder.getInt64(contextFlags), toFlags);
    }

    // __process_infos.modules
    {
        auto v0 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.constantSeg), builder.getInt32(module->modulesSliceOffset));
        auto r0 = TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, builder.getInt32(0)));
        builder.CreateStore(v0, r0);

        auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(TO_PTR_I8(pp.processInfos), builder.getInt32(8)));
        builder.CreateStore(builder.getInt64(module->moduleDependencies.count + 1), r1);
    }

    // __process_infos.args
    {
        auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(TO_PTR_I8(pp.processInfos), builder.getInt32(16)));
        builder.CreateStore(pp.cst0_i64, r0);
        auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(TO_PTR_I8(pp.processInfos), builder.getInt32(24)));
        builder.CreateStore(pp.cst0_i64, r1);
    }

    // __process_infos.contextTlsId = swag_runtime_tlsAlloc()
    {
        auto result  = emitCall(buildParameters, module, g_LangSpec->name__tlsAlloc, nullptr, allocT, {}, {});
        auto toTlsId = TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst2_i32}));
        builder.CreateStore(result, toTlsId);
    }

    // Set main context
    {
        auto toContext = builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst3_i32});
        builder.CreateStore(pp.mainContext, toContext);
    }

    // Set current backend as LLVM
    {
        auto toBackendKind = TO_PTR_I32(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst6_i32}));
        builder.CreateStore(builder.getInt32((uint32_t) SwagBackendGenType::LLVM), toBackendKind);
    }

    // Set default context in TLS
    {
        auto toTlsId   = builder.CreateLoad(TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst2_i32})));
        auto toContext = builder.CreatePointerCast(pp.mainContext, llvm::Type::getInt8PtrTy(context));
        emitCall(buildParameters, module, g_LangSpec->name__tlsSetValue, nullptr, allocT, {UINT32_MAX, UINT32_MAX}, {toTlsId, toContext});
    }

    {
        emitCall(buildParameters, module, g_LangSpec->name__setupRuntime, nullptr, allocT, {}, {});
    }

    // Load all dependencies
    VectorNative<ModuleDependency*> moduleDependencies;
    module->sortDependenciesByInitOrder(moduleDependencies);
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep      = moduleDependencies[i];
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto nameLib = nameDown + Backend::getOutputFileExtension(g_CommandLine->target, BuildCfgBackendKind::DynamicLib);
        auto ptrStr  = builder.CreateGlobalStringPtr(nameLib.c_str());
        emitCall(buildParameters, module, g_LangSpec->name__loaddll, nullptr, allocT, {UINT32_MAX, UINT32_MAX}, {ptrStr, builder.getInt64(nameLib.length())});
    }

    // Call to global init of all dependencies
    auto funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {pp.processInfosTy->getPointerTo()}, false);
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameFct  = dep->module->getGlobalPrivFct(g_LangSpec->name_globalInit);
        auto funcInit = modu.getOrInsertFunction(nameFct.c_str(), funcType);
        builder.CreateCall(funcInit, pp.processInfos);
    }

    // Call to global init of this module
    {
        auto nameFct  = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
        auto funcInit = modu.getOrInsertFunction(nameFct.c_str(), funcType);
        builder.CreateCall(funcInit, pp.processInfos);
    }

    // Call to global premain of all dependencies
    for (int i = 0; i < moduleDependencies.size(); i++)
    {
        auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        auto nameFct  = dep->module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
        auto funcInit = modu.getOrInsertFunction(nameFct.c_str(), funcType);
        builder.CreateCall(funcInit, pp.processInfos);
    }

    // Call to global premain of this module
    {
        auto nameFct  = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
        auto funcInit = modu.getOrInsertFunction(nameFct.c_str(), funcType);
        builder.CreateCall(funcInit, pp.processInfos);
    }

    auto funcTypeVoid = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);

    // Call to test functions
    if (buildParameters.compileType == BackendCompileType::Test && !module->byteCodeTestFunc.empty())
    {
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;

            auto fcc = modu.getOrInsertFunction(bc->getCallName().c_str(), funcTypeVoid);
            builder.CreateCall(fcc);
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        auto fncMain = modu.getOrInsertFunction(module->byteCodeMainFunc->getCallName().c_str(), funcTypeVoid);
        builder.CreateCall(fncMain);
    }

    // Call to global drop of this module
    auto nameFct  = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    auto funcDrop = modu.getOrInsertFunction(nameFct.c_str(), funcTypeVoid);
    builder.CreateCall(funcDrop);

    // Call to global drop of all dependencies
    for (int i = (int) moduleDependencies.size() - 1; i >= 0; i--)
    {
        auto dep = moduleDependencies[i];
        if (!dep->module->isSwag)
            continue;
        nameFct  = dep->module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
        funcDrop = modu.getOrInsertFunction(nameFct.c_str(), funcTypeVoid);
        builder.CreateCall(funcDrop);
    }

    // Call exit
    emitCall(buildParameters, module, g_LangSpec->name__exit, nullptr, allocT, {}, {});

    builder.CreateRetVoid();
    return true;
}

bool BackendLLVM::emitGetTypeTable(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType  = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), {}, false);
    auto            funcName = module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
    llvm::Function* fct      = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, funcName.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.constantSeg), builder.getInt32(module->typesSliceOffset));
    builder.CreateRet(TO_PTR_I8(r1));

    return true;
}

bool BackendLLVM::emitGlobalPreMain(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {pp.processInfosTy->getPointerTo()}, false);
    auto            nameFct = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, nameFct.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // __process_infos = *processInfos;
    {
        auto p0 = TO_PTR_I8(pp.processInfos);
        auto p1 = TO_PTR_I8(fct->getArg(0));
        builder.CreateMemCpy(p0, llvm::Align{}, p1, llvm::Align{}, builder.getInt64(sizeof(SwagProcessInfos)));
    }

    // Call to #premain functions
    auto fctType1 = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {}, false);
    for (auto bc : module->byteCodePreMainFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto func = modu.getOrInsertFunction(bc->getCallName().c_str(), fctType1);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
    return true;
}

bool BackendLLVM::emitGlobalInit(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {pp.processInfosTy->getPointerTo()}, false);
    auto            nameFct = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, nameFct.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // __process_infos = *processInfos;
    {
        auto p0 = TO_PTR_I8(pp.processInfos);
        auto p1 = TO_PTR_I8(fct->getArg(0));
        builder.CreateMemCpy(p0, llvm::Align{}, p1, llvm::Align{}, builder.getInt64(sizeof(SwagProcessInfos)));
    }

    // Init thread local storage id
    {
        auto allocT = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(1));
        emitCall(buildParameters, module, g_LangSpec->name__tlsAlloc, nullptr, allocT, {UINT32_MAX}, {pp.symTls_threadLocalId});
    }

    // Initialize data segments
    builder.CreateCall(modu.getFunction("__initMutableSeg"));
    builder.CreateCall(modu.getFunction("__initConstantSeg"));
    builder.CreateCall(modu.getFunction("__initTlsSeg"));

    // Init type table slice for each dependency (by calling ???_getTypeTable)
    auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.constantSeg), builder.getInt32(module->modulesSliceOffset + sizeof(SwagModule) + offsetof(SwagModule, types)));
    for (auto& dep : module->moduleDependencies)
    {
        if (!dep->module->isSwag)
        {
            r1 = builder.CreateInBoundsGEP(r1, builder.getInt64(sizeof(SwagModule)));
            continue;
        }

        auto callTable = dep->module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
        auto callType  = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), {}, false);
        auto func      = modu.getOrInsertFunction(callTable.c_str(), callType);
        auto r0        = builder.CreateCall(func);

        // Count types is stored as a uint64_t at the start of the address
        auto numTypes = builder.CreateLoad(TO_PTR_I64(r0));
        auto ptrTypes = builder.CreateInBoundsGEP(r0, builder.getInt64(sizeof(uint64_t)));

        builder.CreateStore(ptrTypes, TO_PTR_PTR_I8(r1));
        auto r2 = builder.CreateInBoundsGEP(r1, builder.getInt64(sizeof(void*)));
        builder.CreateStore(numTypes, TO_PTR_I64(r2));

        r1 = builder.CreateInBoundsGEP(r1, builder.getInt64(sizeof(SwagModule)));
    }

    // Call to #init functions
    auto initFctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto func = modu.getOrInsertFunction(bc->getCallName().c_str(), initFctType);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
    return true;
}

bool BackendLLVM::emitGlobalDrop(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto  modu    = pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    auto            nameFct = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, nameFct.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // Call to #drop functions
    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto func = modu->getOrInsertFunction(bc->getCallName().c_str(), fctType);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
    return true;
}
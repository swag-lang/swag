#include "pch.h"
#include "ByteCode.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "LLVM.h"
#include "LLVM_Macros.h"
#include "Module.h"

void LLVM::emitOS(const BuildParameters& buildParameters) const
{
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        const int   ct              = buildParameters.compileType;
        const int   precompileIndex = buildParameters.precompileIndex;
        const auto& pp              = *(LLVMPerObj*) perThread[ct][precompileIndex];
        auto&       context         = *pp.context;
        auto&       builder         = *pp.builder;
        auto&       modu            = *pp.module;

        // int _DllMainCRTStartup(void*, int, void*)
        {
            VectorNative<llvm::Type*> params;
            params.push_back(PTR_I8_TY());
            params.push_back(I32_TY());
            params.push_back(PTR_I8_TY());
            llvm::FunctionType* FT = llvm::FunctionType::get(I32_TY(), {params.begin(), params.end()}, false);
            llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "_DllMainCRTStartup", modu);
            llvm::BasicBlock*   BB = llvm::BasicBlock::Create(context, "entry", F);
            builder.SetInsertPoint(BB);
            builder.CreateRet(builder.getInt32(1));
        }

        // :ChkStk
        // See SWAG_LIMIT_PAGE_STACK
        // Stack probing. Must do it by hand to avoid linking with vc runtime on windows.
        // void __chkstk()
        if (g_CommandLine.target.arch == SwagTargetArch::X86_64)
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
        new llvm::GlobalVariable(modu, I32_TY(), false, llvm::GlobalValue::ExternalLinkage, builder.getInt32(0), "_fltused");
    }
}

void LLVM::emitMain(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *(LLVMPerObj*) perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    emitOS(buildParameters);

    const char* entryPoint = nullptr;
    switch (g_CommandLine.target.os)
    {
    case SwagTargetOs::Windows:
        if (buildParameters.buildCfg->backendSubKind == BuildCfgBackendSubKind::Console)
            entryPoint = "mainCRTStartup";
        else
            entryPoint = "WinMainCRTStartup";
        break;
    default:
        entryPoint = "main";
        break;
    }

    // void mainCRTStartup()
    llvm::FunctionType* FT = llvm::FunctionType::get(VOID_TY(), {}, false);
    llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, entryPoint, modu);
    llvm::BasicBlock*   BB = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);

    // Reserve room to pass parameters to embedded intrinsics
    auto allocT = builder.CreateAlloca(I64_TY(), builder.getInt64(2));
    allocT->setAlignment(llvm::Align{16});

    // Set default system allocator function
    SWAG_ASSERT(g_SystemAllocatorTable);
    auto bcAlloc = (ByteCode*) ByteCode::undoByteCodeLambda(((void**) g_SystemAllocatorTable)[0]);
    SWAG_ASSERT(bcAlloc);
    auto allocFct = modu.getOrInsertFunction(bcAlloc->getCallName().c_str(), pp.allocatorTy);
    builder.CreateStore(allocFct.getCallee(), pp.defaultAllocTable);

    // mainContext.allocator.itable = &defaultAllocTable
    {
        auto toTable   = builder.CreateInBoundsGEP(pp.contextTy, pp.mainContext, {pp.cst0_i32, pp.cst0_i32, pp.cst1_i32});
        auto fromTable = builder.CreatePointerCast(pp.defaultAllocTable, PTR_I8_TY());
        builder.CreateStore(fromTable, toTable);
    }

    // mainContext.flags = 0
    {
        auto     toFlags      = builder.CreateInBoundsGEP(pp.contextTy, pp.mainContext, {pp.cst0_i32, pp.cst1_i32});
        uint64_t contextFlags = getDefaultContextFlags(module);
        builder.CreateStore(builder.getInt64(contextFlags), toFlags);
    }

    // __process_infos.modules
    {
        auto v0 = builder.CreateInBoundsGEP(I8_TY(), pp.constantSeg, builder.getInt32(module->modulesSliceOffset));
        auto r0 = TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, builder.getInt32(0)));
        builder.CreateStore(v0, r0);

        auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(I8_TY(), pp.processInfos, builder.getInt32(8)));
        builder.CreateStore(builder.getInt64(module->moduleDependencies.count + 1), r1);
    }

    // __process_infos.args
    {
        auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(I8_TY(), pp.processInfos, builder.getInt32(16)));
        builder.CreateStore(pp.cst0_i64, r0);
        auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(I8_TY(), pp.processInfos, builder.getInt32(24)));
        builder.CreateStore(pp.cst0_i64, r1);
    }

    // __process_infos.contextTlsId = swag_runtime_tlsAlloc()
    {
        auto result  = emitCall(buildParameters, module, g_LangSpec->name__tlsAlloc, nullptr, allocT, {}, {});
        auto toTlsId = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cst0_i32, pp.cst2_i32});
        builder.CreateStore(result, TO_PTR_I64(toTlsId));
    }

    // Set main context
    {
        auto toContext = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cst0_i32, pp.cst3_i32});
        builder.CreateStore(pp.mainContext, toContext);
    }

    // Set current backend as LLVM
    {
        auto toBackendKind = TO_PTR_I32(builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cst0_i32, pp.cst6_i32}));
        builder.CreateStore(builder.getInt32((uint32_t) SwagBackendGenType::LLVM), toBackendKind);
    }

    // Set default context in TLS
    {
        auto v0        = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {pp.cst0_i32, pp.cst2_i32});
        auto toTlsId   = builder.CreateLoad(I64_TY(), v0);
        auto toContext = builder.CreatePointerCast(pp.mainContext, PTR_I8_TY());
        emitCall(buildParameters, module, g_LangSpec->name__tlsSetValue, nullptr, allocT, {UINT32_MAX, UINT32_MAX}, {toTlsId, toContext});
    }

    // Initialize constant segment because we need to have correct pointers to DebugAllocator...
    // This is different with x64 because in x64 patching is not done by hand...
    builder.CreateCall(modu.getFunction("__initConstantSeg"));

    // __setupRuntime
    {
        auto rtFlags = builder.getInt64(getRuntimeFlags(module));
        emitCall(buildParameters, module, g_LangSpec->name__setupRuntime, nullptr, allocT, {UINT32_MAX}, {rtFlags});
    }

    // Load all dependencies
    VectorNative<ModuleDependency*> moduleDependencies;
    module->sortDependenciesByInitOrder(moduleDependencies);
    for (auto dep : moduleDependencies)
    {
        auto nameLib = getOutputFileName(g_CommandLine.target, dep->module->name, BuildCfgOutputKind::DynamicLib);

        error_code err;
        if (exists(nameLib, err))
        {
            nameLib     = nameLib.filename();
            auto ptrStr = builder.CreateGlobalStringPtr(nameLib.string());
            emitCall(buildParameters, module, g_LangSpec->name__loaddll, nullptr, allocT, {UINT32_MAX, UINT32_MAX}, {ptrStr, builder.getInt64(nameLib.string().length())});
        }
    }

    // Call to global init of all dependencies
    auto funcType = llvm::FunctionType::get(VOID_TY(), {pp.processInfosTy->getPointerTo()}, false);
    for (auto dep : moduleDependencies)
    {
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
    for (auto dep : moduleDependencies)
    {
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

    auto funcTypeVoid = llvm::FunctionType::get(VOID_TY(), false);

    // Call to test functions
    if (buildParameters.compileType == Test)
    {
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
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

    // __closeRuntime
    emitCall(buildParameters, module, g_LangSpec->name__closeRuntime, nullptr, allocT, {}, {});

    builder.CreateRetVoid();
}

void LLVM::emitGetTypeTable(const BuildParameters& buildParameters) const
{
    const int ct              = buildParameters.compileType;
    const int precompileIndex = buildParameters.precompileIndex;

    const auto& pp      = *(LLVMPerObj*) perThread[ct][precompileIndex];
    auto&       context = *pp.context;
    auto&       builder = *pp.builder;
    auto&       modu    = *pp.module;

    const auto      fctType  = llvm::FunctionType::get(PTR_I8_TY(), {}, false);
    const auto      funcName = module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
    llvm::Function* fct      = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, funcName.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    const auto r1 = builder.CreateInBoundsGEP(I8_TY(), pp.constantSeg, builder.getInt32(module->typesSliceOffset));
    builder.CreateRet(TO_PTR_I8(r1));
}

void LLVM::emitGlobalPreMain(const BuildParameters& buildParameters) const
{
    const int ct              = buildParameters.compileType;
    const int precompileIndex = buildParameters.precompileIndex;

    const auto& pp      = *(LLVMPerObj*) perThread[ct][precompileIndex];
    auto&       context = *pp.context;
    auto&       builder = *pp.builder;
    auto&       modu    = *pp.module;

    const auto      fctType = llvm::FunctionType::get(VOID_TY(), {pp.processInfosTy->getPointerTo()}, false);
    const auto      nameFct = module->getGlobalPrivFct(g_LangSpec->name_globalPreMain);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, nameFct.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // __process_infos = *processInfos;
    {
        const auto p0 = TO_PTR_I8(pp.processInfos);
        const auto p1 = TO_PTR_I8(fct->getArg(0));
        builder.CreateMemCpy(p0, llvm::Align{}, p1, llvm::Align{}, builder.getInt64(sizeof(SwagProcessInfos)));
    }

    // Call to #premain functions
    const auto fctType1 = llvm::FunctionType::get(VOID_TY(), {}, false);
    for (const auto bc : module->byteCodePreMainFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        const auto func = modu.getOrInsertFunction(bc->getCallName().c_str(), fctType1);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
}

void LLVM::emitGlobalInit(const BuildParameters& buildParameters)
{
    const int ct              = buildParameters.compileType;
    const int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = *(LLVMPerObj*) perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    const auto      fctType = llvm::FunctionType::get(VOID_TY(), {pp.processInfosTy->getPointerTo()}, false);
    const auto      nameFct = module->getGlobalPrivFct(g_LangSpec->name_globalInit);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, nameFct.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // __process_infos = *processInfos;
    {
        const auto p0 = TO_PTR_I8(pp.processInfos);
        const auto p1 = TO_PTR_I8(fct->getArg(0));
        builder.CreateMemCpy(p0, llvm::Align{}, p1, llvm::Align{}, builder.getInt64(sizeof(SwagProcessInfos)));
    }

    // Init thread local storage id
    {
        const auto allocT = builder.CreateAlloca(I64_TY(), builder.getInt64(1));
        allocT->setAlignment(llvm::Align{16});
        emitCall(buildParameters, module, g_LangSpec->name__tlsAlloc, nullptr, allocT, {UINT32_MAX}, {pp.symTls_threadLocalId});
    }

    // Initialize data segments
    builder.CreateCall(modu.getFunction("__initMutableSeg"));
    builder.CreateCall(modu.getFunction("__initConstantSeg"));
    builder.CreateCall(modu.getFunction("__initTlsSeg"));

    // Init type table slice for each dependency (by calling ???_getTypeTable)
    auto r1 = builder.CreateInBoundsGEP(I8_TY(), pp.constantSeg, builder.getInt32(module->modulesSliceOffset + sizeof(SwagModule) + offsetof(SwagModule, types)));
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep->module->isSwag)
        {
            r1 = builder.CreateInBoundsGEP(I8_TY(), r1, builder.getInt64(sizeof(SwagModule)));
            continue;
        }

        auto       callTable = dep->module->getGlobalPrivFct(g_LangSpec->name_getTypeTable);
        const auto callType  = llvm::FunctionType::get(PTR_I8_TY(), {}, false);
        const auto func      = modu.getOrInsertFunction(callTable.c_str(), callType);
        const auto r0        = builder.CreateCall(func);

        // Count types is stored as a uint64_t at the start of the address
        const auto numTypes = builder.CreateLoad(I64_TY(), r0);
        const auto ptrTypes = builder.CreateInBoundsGEP(I8_TY(), r0, builder.getInt64(sizeof(uint64_t)));

        builder.CreateStore(ptrTypes, TO_PTR_PTR_I8(r1));
        const auto r2 = builder.CreateInBoundsGEP(I8_TY(), r1, builder.getInt64(sizeof(void*)));
        builder.CreateStore(numTypes, TO_PTR_I64(r2));

        r1 = builder.CreateInBoundsGEP(I8_TY(), r1, builder.getInt64(sizeof(SwagModule)));
    }

    // Call to #init functions
    const auto initFctType = llvm::FunctionType::get(VOID_TY(), false);
    for (const auto bc : module->byteCodeInitFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        const auto func = modu.getOrInsertFunction(bc->getCallName().c_str(), initFctType);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
}

void LLVM::emitGlobalDrop(const BuildParameters& buildParameters)
{
    const int ct              = buildParameters.compileType;
    const int precompileIndex = buildParameters.precompileIndex;

    const auto& pp      = *(LLVMPerObj*) perThread[ct][precompileIndex];
    auto&       context = *pp.context;
    auto&       builder = *pp.builder;
    auto&       modu    = *pp.module;

    const auto      fctType = llvm::FunctionType::get(VOID_TY(), false);
    const auto      nameFct = module->getGlobalPrivFct(g_LangSpec->name_globalDrop);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, nameFct.c_str(), modu);
    if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Library)
        fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // Call to #drop functions
    for (const auto bc : module->byteCodeDropFunc)
    {
        const auto node = bc->node;
        if (node && node->hasAttribute(ATTRIBUTE_COMPILER))
            continue;
        const auto func = modu.getOrInsertFunction(bc->getCallName().c_str(), fctType);
        builder.CreateCall(func);
    }

    // __dropGlobalVariables
    {
        emitCall(buildParameters, module, g_LangSpec->name__dropGlobalVariables, nullptr, nullptr, {}, {});
    }

    builder.CreateRetVoid();
}

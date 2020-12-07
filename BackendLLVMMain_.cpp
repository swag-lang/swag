#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "Context.h"
#include "Workspace.h"

bool BackendLLVM::emitOS(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    if (g_CommandLine.os == BackendOs::Windows)
    {
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

        // void __chkstk()
        {
            llvm::FunctionType* FT = llvm::FunctionType::get(builder.getVoidTy(), {}, false);
            llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "__chkstk", modu);
            llvm::BasicBlock*   BB = llvm::BasicBlock::Create(context, "entry", F);
            builder.SetInsertPoint(BB);
            builder.CreateRetVoid();
        }

        // int _fltused = 0;
        new llvm::GlobalVariable(modu, builder.getInt32Ty(), false, llvm::GlobalValue::ExternalLinkage, builder.getInt32(0), "_fltused");

        return true;
    }
    else
    {
        module->error(format("llvm backend unsupported os '%s'", g_Workspace.GetOsName().c_str()));
        return false;
    }
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
    switch (g_CommandLine.os)
    {
    case BackendOs::Windows:
        entryPoint = "mainCRTStartup";
        break;
    default:
        module->error(format("llvm backend unsupported os '%s'", g_Workspace.GetOsName().c_str()));
        return false;
    }

    // void mainCRTStartup()
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {}, false);
    llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, entryPoint, modu);
    llvm::BasicBlock*   BB = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);

    // Reserve room to pass parameters to embedded intrinsics
    auto allocT = TO_PTR_I64(builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(2)));

    // Main context
    SWAG_ASSERT(g_defaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) undoByteCodeLambda(((void**) g_defaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    auto allocFct = modu.getOrInsertFunction(bcAlloc->callName().c_str(), pp.allocatorTy);
    builder.CreateStore(allocFct.getCallee(), pp.defaultAllocTable);

    //mainContext.allocator.itable = &defaultAllocTable
    {
        auto toTable   = builder.CreateInBoundsGEP(pp.mainContext, {pp.cst0_i32, pp.cst0_i32, pp.cst1_i32});
        auto fromTable = builder.CreatePointerCast(pp.defaultAllocTable, llvm::Type::getInt8PtrTy(context));
        builder.CreateStore(fromTable, toTable);
    }

    //mainContext.flags = 0
    {
        auto     toFlags      = builder.CreateInBoundsGEP(pp.mainContext, {pp.cst0_i32, pp.cst1_i32});
        uint64_t contextFlags = getDefaultContextFlags(module);
        builder.CreateStore(builder.getInt64(contextFlags), toFlags);
    }

    // __process_infos.contextTlsId = swag_runtime_tlsAlloc()
    {
        auto typeF   = createFunctionTypeInternal(buildParameters, 1);
        auto toTlsId = builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32});
        builder.CreateCall(modu.getOrInsertFunction("__tlsAlloc", typeF), {toTlsId});
    }

    // __process_infos.defaultContext = &mainContext
    {
        auto toContext = builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst2_i32});
        builder.CreateStore(pp.mainContext, toContext);
    }

    // swag_runtime_tlsSetValue(__process_infos.contextTlsId, __process_infos.defaultContext)
    {
        auto typeF     = createFunctionTypeInternal(buildParameters, 2);
        auto toTlsId   = builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32});
        auto toContext = builder.CreatePtrToInt(builder.CreatePointerCast(pp.mainContext, llvm::Type::getInt64PtrTy(context)), builder.getInt64Ty());
        builder.CreateStore(toContext, GEP_I32(allocT, 0));
        auto p0 = GEP_I32(allocT, 0);
        builder.CreateCall(modu.getOrInsertFunction("__tlsSetValue", typeF), {toTlsId, p0});
    }

    // Load all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto nameLib = nameDown + OS::getDllFileExtension();
        auto ptrStr  = builder.CreateGlobalStringPtr(nameLib.c_str());

        builder.CreateStore(builder.CreatePtrToInt(ptrStr, builder.getInt64Ty()), GEP_I32(allocT, 0));
        builder.CreateStore(builder.getInt64(nameLib.length()), GEP_I32(allocT, 1));
        auto typeF = createFunctionTypeInternal(buildParameters, 2);
        auto p0    = GEP_I32(allocT, 0);
        auto p1    = GEP_I32(allocT, 1);
        builder.CreateCall(modu.getOrInsertFunction("__loaddll", typeF), {p0, p1});
    }

    // Call to global init of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep->module->isSwag)
            continue;
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        auto funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {pp.processInfosTy->getPointerTo()}, false);
        auto funcInit = modu.getOrInsertFunction(format("%s_globalInit", nameDown.c_str()).c_str(), funcType);
        builder.CreateCall(funcInit, pp.processInfos);
    }

    // Call to global init of this module
    {
        auto funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {pp.processInfosTy->getPointerTo()}, false);
        auto funcInit = modu.getOrInsertFunction(format("%s_globalInit", module->nameDown.c_str()).c_str(), funcType);
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

            auto fcc = modu.getOrInsertFunction(bc->callName().c_str(), funcTypeVoid);
            builder.CreateCall(fcc);
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        auto fncMain = modu.getOrInsertFunction(module->byteCodeMainFunc->callName().c_str(), funcTypeVoid);
        builder.CreateCall(fncMain);
    }

    // Call to global drop of this module
    auto funcDrop = modu.getOrInsertFunction(format("%s_globalDrop", module->nameDown.c_str()).c_str(), funcTypeVoid);
    builder.CreateCall(funcDrop);

    // Call to global drop of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep->module->isSwag)
            continue;
        auto nameDown = dep->name;
        Ast::normalizeIdentifierName(nameDown);
        funcDrop = modu.getOrInsertFunction(format("%s_globalDrop", nameDown.c_str()).c_str(), funcTypeVoid);
        builder.CreateCall(funcDrop);
    }

    // Call exit
    auto typeF = createFunctionTypeInternal(buildParameters, 0);
    builder.CreateCall(modu.getOrInsertFunction("__exit", typeF), {});

    uint32_t value  = 0;
    auto     retVal = llvm::ConstantInt::get(context, llvm::APInt(32, value));
    builder.CreateRet(retVal);

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
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, format("%s_globalInit", module->nameDown.c_str()).c_str(), modu);
    fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // __process_infos = *processInfos;
    {
        auto typeF  = createFunctionTypeInternal(buildParameters, 3);
        auto allocT = TO_PTR_I64(builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(3)));
        auto r0     = builder.CreatePtrToInt(pp.processInfos, builder.getInt64Ty());
        auto r1     = builder.CreatePtrToInt(fct->getArg(0), builder.getInt64Ty());
        auto r2     = builder.getInt64(sizeof(SwagProcessInfos));
        auto p0     = GEP_I32(allocT, 0);
        auto p1     = GEP_I32(allocT, 1);
        auto p2     = GEP_I32(allocT, 2);
        builder.CreateStore(r0, p0);
        builder.CreateStore(r1, p1);
        builder.CreateStore(r2, p2);
        builder.CreateCall(modu.getOrInsertFunction("@memcpy", typeF), {p0, p1, p2});
    }

    // Initialize data segments
    builder.CreateCall(modu.getFunction("initMutableSeg"));
    builder.CreateCall(modu.getFunction("initTypeSeg"));
    builder.CreateCall(modu.getFunction("initConstantSeg"));

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto func = modu.getOrInsertFunction(bc->callName().c_str(), fctType);
        builder.CreateCall(func, {fct->getArg(0)});
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
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, format("%s_globalDrop", module->nameDown.c_str()).c_str(), modu);
    fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // Call to #drop functions
    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto func = modu->getOrInsertFunction(bc->callName().c_str(), fctType);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
    return true;
}
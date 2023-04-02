#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "Workspace.h"
#include "BackendLinker.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"

bool BackendLLVM::createRuntime(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (!perThread[ct][precompileIndex])
        perThread[ct][precompileIndex] = new LLVMPerThread;

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& modu    = *pp.module;

    // swag_interface_t
    {
        llvm::Type* members[] = {
            llvm::Type::getInt8PtrTy(context),
            llvm::Type::getInt8PtrTy(context)};
        pp.interfaceTy = llvm::StructType::create(context, members, "swag_interface_t");
    }

    // swag_context_t
    {
        llvm::Type* members[] = {
            pp.interfaceTy,                                                                       // allocator
            llvm::Type::getInt64Ty(context),                                                      // flags
            pp.interfaceTy,                                                                       // ScratchAllocator allocator
            llvm::Type::getInt8PtrTy(context),                                                    // ScratchAllocator block
            llvm::Type::getInt64Ty(context),                                                      // ScratchAllocator capacity
            llvm::Type::getInt64Ty(context),                                                      // ScratchAllocator used
            llvm::Type::getInt64Ty(context),                                                      // ScratchAllocator maxUsed
            llvm::Type::getInt8PtrTy(context),                                                    // ScratchAllocator firstLeak
            llvm::Type::getInt64Ty(context),                                                      // ScratchAllocator totalLeak
            llvm::Type::getInt64Ty(context),                                                      // ScratchAllocator maxLeak
            llvm::ArrayType::get(llvm::Type::getInt8Ty(context), MAX_LEN_ERROR_MSG),              // errorMsg
            llvm::Type::getInt32Ty(context),                                                      // errorMsgStart
            llvm::Type::getInt32Ty(context),                                                      // errorMsgLen
            llvm::Type::getInt32Ty(context),                                                      // traceIndex
            llvm::ArrayType::get(llvm::Type::getInt8Ty(context), MAX_TRACE * sizeof(void*)),      // trace
            llvm::ArrayType::get(llvm::Type::getInt8Ty(context), sizeof(SwagSourceCodeLocation)), // exceptionLoc
            llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 4 * sizeof(void*)),              // exceptionParams
            llvm::Type::getInt8PtrTy(context),                                                    // panic
        };

        static_assert(sizeof(SwagContext) == 568);
        pp.contextTy = llvm::StructType::create(context, members, "swag_context_t");
        SWAG_ASSERT(pp.contextTy->isSized());
    }

    // swag_slice_t
    {
        llvm::Type* members[] = {
            llvm::Type::getInt8PtrTy(context),
            llvm::Type::getInt64Ty(context),
        };
        pp.sliceTy = llvm::StructType::create(context, members, "swag_slice_t");
        SWAG_ASSERT(pp.sliceTy->isSized());
    }

    // swag_alloctor_t
    {
        llvm::Type* params[] = {
            llvm::Type::getInt64PtrTy(context),
            llvm::Type::getInt64PtrTy(context),
        };
        pp.allocatorTy = llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false);
    }

    // byteCodeRun
    {
        llvm::Type* params[] = {
            llvm::Type::getInt8PtrTy(context),
        };
        pp.bytecodeRunTy = llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, true);
    }

    // makeCallback
    {
        llvm::Type* params[] = {
            llvm::Type::getInt8PtrTy(context),
        };
        pp.makeCallbackTy = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), params, false);
    }

    // SwagProcessInfo
    {
        llvm::Type* members[] = {
            pp.sliceTy,
            pp.sliceTy,
            llvm::Type::getInt64Ty(context),
            pp.contextTy->getPointerTo(),
            pp.bytecodeRunTy->getPointerTo(),
            pp.makeCallbackTy->getPointerTo(),
            llvm::Type::getInt32Ty(context),
        };
        pp.processInfosTy = llvm::StructType::create(context, members, "SwagProcessInfo");
        SWAG_ASSERT(pp.processInfosTy->isSized());
    }

    // mainContext
    if (precompileIndex == 0)
    {
        pp.mainContext          = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantAggregateZero::get(pp.contextTy), "swag_mainContext");
        pp.defaultAllocTable    = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantPointerNull::get(pp.allocatorTy->getPointerTo()), "swag_defaultAllocTable");
        pp.processInfos         = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantAggregateZero::get(pp.processInfosTy), "swag_processInfos");
        pp.symTls_threadLocalId = new llvm::GlobalVariable(modu, llvm::Type::getInt64Ty(context), false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0), "swag_tls_threadLocalId");
    }
    else
    {
        pp.mainContext          = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::InternalLinkage, nullptr, "swag_mainContext");
        pp.defaultAllocTable    = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::InternalLinkage, nullptr, "swag_defaultAllocTable");
        pp.processInfos         = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::InternalLinkage, nullptr, "swag_processInfos");
        pp.symTls_threadLocalId = new llvm::GlobalVariable(modu, llvm::Type::getInt64Ty(context), false, llvm::GlobalValue::InternalLinkage, nullptr, "swag_tls_threadLocalId");
    }

    // LIBC functions without llvm intrinsics
    {
        pp.fn_acosf32  = modu.getOrInsertFunction(g_LangSpec->name_acosf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_acosf64  = modu.getOrInsertFunction(g_LangSpec->name_acos.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_asinf32  = modu.getOrInsertFunction(g_LangSpec->name_asinf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_asinf64  = modu.getOrInsertFunction(g_LangSpec->name_asin.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_tanf32   = modu.getOrInsertFunction(g_LangSpec->name_tanf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_tanf64   = modu.getOrInsertFunction(g_LangSpec->name_tan.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_atanf32  = modu.getOrInsertFunction(g_LangSpec->name_atanf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_atanf64  = modu.getOrInsertFunction(g_LangSpec->name_atan.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_sinhf32  = modu.getOrInsertFunction(g_LangSpec->name_sinhf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_sinhf64  = modu.getOrInsertFunction(g_LangSpec->name_sinh.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_coshf32  = modu.getOrInsertFunction(g_LangSpec->name_coshf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_coshf64  = modu.getOrInsertFunction(g_LangSpec->name_cosh.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_tanhf32  = modu.getOrInsertFunction(g_LangSpec->name_tanhf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_tanhf64  = modu.getOrInsertFunction(g_LangSpec->name_tanh.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_powf32   = modu.getOrInsertFunction(g_LangSpec->name_powf.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), {llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context)}, false));
        pp.fn_powf64   = modu.getOrInsertFunction(g_LangSpec->name_pow.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)}, false));
        pp.fn_atan2f32 = modu.getOrInsertFunction(g_LangSpec->name_atan2f.c_str(), ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), {llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context)}, false));
        pp.fn_atan2f64 = modu.getOrInsertFunction(g_LangSpec->name_atan2.c_str(), ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)}, false));
        pp.fn_memcmp   = modu.getOrInsertFunction(g_LangSpec->name_memcmp.c_str(), ::llvm::FunctionType::get(llvm::Type::getInt32Ty(context), {llvm::Type::getInt8PtrTy(context), llvm::Type::getInt8PtrTy(context), llvm::Type::getInt64Ty(context)}, false));
        pp.fn_strlen   = modu.getOrInsertFunction(g_LangSpec->name_strlen.c_str(), ::llvm::FunctionType::get(llvm::Type::getInt64Ty(context), {llvm::Type::getInt8PtrTy(context)}, false));
        pp.fn_strcmp   = modu.getOrInsertFunction(g_LangSpec->name_strcmp.c_str(), ::llvm::FunctionType::get(llvm::Type::getInt64Ty(context), {llvm::Type::getInt8PtrTy(context), llvm::Type::getInt8PtrTy(context)}, false));
        pp.fn_malloc   = modu.getOrInsertFunction(g_LangSpec->name_malloc.c_str(), ::llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), {llvm::Type::getInt64Ty(context)}, false));
        pp.fn_realloc  = modu.getOrInsertFunction(g_LangSpec->name_realloc.c_str(), ::llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), {llvm::Type::getInt8PtrTy(context), llvm::Type::getInt64Ty(context)}, false));
        pp.fn_free     = modu.getOrInsertFunction(g_LangSpec->name_free.c_str(), ::llvm::FunctionType::get(llvm::Type::getVoidTy(context), {llvm::Type::getInt8PtrTy(context)}, false));
    }

    // Cache things
    pp.cst0_i1  = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0);
    pp.cst1_i1  = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 1);
    pp.cst0_i8  = llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0);
    pp.cst1_i8  = llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 1);
    pp.cst0_i16 = llvm::ConstantInt::get(llvm::Type::getInt16Ty(context), 0);
    pp.cst0_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    pp.cst1_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 1);
    pp.cst2_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 2);
    pp.cst3_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 3);
    pp.cst4_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 4);
    pp.cst5_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 5);
    pp.cst6_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 6);
    pp.cst0_i64 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
    pp.cst1_i64 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 1);
    pp.cst0_f32 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 0);
    pp.cst0_f64 = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0);
    pp.cst_null = llvm::ConstantPointerNull::get(llvm::Type::getInt64PtrTy(context));

    return true;
}

JobResult BackendLLVM::prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (!perThread[ct][precompileIndex])
        perThread[ct][precompileIndex] = new LLVMPerThread;
    auto& pp = *perThread[ct][precompileIndex];

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->printStartBuilding(buildParameters);

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        pp.filename = Fmt("%s%d", buildParameters.outputFileName.c_str(), precompileIndex);
        pp.filename += Backend::getObjectFileExtension(g_CommandLine.target);

        pp.context = new llvm::LLVMContext();
        pp.module  = new llvm::Module(pp.filename.c_str(), *pp.context);
        pp.builder = new llvm::IRBuilder<>(*pp.context);

        if (buildParameters.buildCfg->backendDebugInformations)
        {
            pp.dbg = new BackendLLVMDbg;
            pp.dbg->setup(this, pp.module);
        }

        createRuntime(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
        emitDataSegment(buildParameters, &module->tlsSegment);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);

        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        if (g_Workspace->bootstrapModule->numErrors || g_Workspace->runtimeModule->numErrors)
            module->numErrors++;
        if (module->numErrors)
            return JobResult::ReleaseJob;
        if (stage == 1)
            return JobResult::ReleaseJob;

        if (precompileIndex == 0)
        {
            emitInitSeg(buildParameters, &module->mutableSegment, SegmentKind::Data);
            emitInitSeg(buildParameters, &module->constantSegment, SegmentKind::Constant);
            emitInitSeg(buildParameters, &module->tlsSegment, SegmentKind::Tls);
            emitGetTypeTable(buildParameters);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitGlobalPreMain(buildParameters);
            emitMain(buildParameters);
        }

        // Output file
        generateObjFile(buildParameters);

        delete pp.module;
        delete pp.context;
        delete pp.builder;
        delete pp.dbg;
    }

    return JobResult::ReleaseJob;
}

bool BackendLLVM::generateObjFile(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& modu            = *pp.module;

    // Debug infos
    if (pp.dbg)
        pp.dbg->finalize();

    // Target triple
    Utf8 archName;
    switch (g_CommandLine.target.arch)
    {
    case SwagTargetArch::X86_64:
        archName = (const char*) llvm::Triple::getArchTypeName(llvm::Triple::x86_64).bytes_begin();
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    Utf8 vendorName, osName, abiName;
    switch (g_CommandLine.target.os)
    {
    case SwagTargetOs::Windows:
        osName     = (const char*) llvm::Triple::getOSTypeName(llvm::Triple::Win32).bytes_begin();
        vendorName = (const char*) llvm::Triple::getVendorTypeName(llvm::Triple::PC).bytes_begin();
        abiName    = (const char*) llvm::Triple::getEnvironmentTypeName(llvm::Triple::MSVC).bytes_begin();
        break;
    case SwagTargetOs::Linux:
        osName = (const char*) llvm::Triple::getOSTypeName(llvm::Triple::Linux).bytes_begin();
        break;
    case SwagTargetOs::MacOSX:
        osName = (const char*) llvm::Triple::getOSTypeName(llvm::Triple::MacOSX).bytes_begin();
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    Utf8 targetTriple = Fmt("%s-%s-%s-%s", archName.c_str(), vendorName.c_str(), osName.c_str(), abiName.c_str()).c_str();
    bool isDebug      = !buildParameters.buildCfg->backendOptimizeSpeed && !buildParameters.buildCfg->backendOptimizeSize;

    // Setup target
    modu.setTargetTriple(targetTriple.c_str());
    std::string error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple.c_str(), error);
    if (!target)
    {
        Report::error(Fmt(Err(Err0558), targetTriple.c_str()));
        return false;
    }

    // Create target machine
    auto                cpu      = "generic";
    auto                features = "";
    llvm::TargetOptions opt;
    auto                rm            = llvm::Optional<llvm::Reloc::Model>();
    auto                targetMachine = target->createTargetMachine(targetTriple.c_str(), cpu, features, opt, rm);
    if (isDebug)
        targetMachine->setOptLevel(llvm::CodeGenOpt::None);
    else
        targetMachine->setOptLevel(llvm::CodeGenOpt::Aggressive);
    targetMachine->setO0WantsFastISel(true);
    modu.setDataLayout(targetMachine->createDataLayout());

    // Output file
    auto targetPath = Backend::getCacheFolder(buildParameters);
    auto path       = targetPath;
    path.append(pp.filename.c_str());
    error_code                err;
    llvm::raw_fd_ostream      dest(path.string(), err, llvm::sys::fs::OF_None);
    llvm::legacy::PassManager llvmPass;

    // Pipeline configurations
    llvm::PipelineTuningOptions pipelineOptions;
    pipelineOptions.LoopUnrolling     = !isDebug;
    pipelineOptions.SLPVectorization  = !isDebug;
    pipelineOptions.LoopVectorization = !isDebug;
    pipelineOptions.LoopInterleaving  = !isDebug;
    pipelineOptions.MergeFunctions    = !isDebug;

    llvm::PassBuilder             passBuilder(false, targetMachine, pipelineOptions);
    llvm::LoopAnalysisManager     loopMgr;
    llvm::FunctionAnalysisManager functionMgr;
    llvm::CGSCCAnalysisManager    cgsccMgr;
    llvm::ModuleAnalysisManager   moduleMgr;

    passBuilder.registerModuleAnalyses(moduleMgr);
    passBuilder.registerCGSCCAnalyses(cgsccMgr);
    passBuilder.registerFunctionAnalyses(functionMgr);
    passBuilder.registerLoopAnalyses(loopMgr);
    passBuilder.crossRegisterProxies(loopMgr, functionMgr, cgsccMgr, moduleMgr);

    llvm::PassBuilder::OptimizationLevel optLevel;
    if (isDebug)
        optLevel = llvm::PassBuilder::OptimizationLevel::O0;
    else if (buildParameters.buildCfg->backendOptimizeSize)
        optLevel = llvm::PassBuilder::OptimizationLevel::Oz;
    else
        optLevel = llvm::PassBuilder::OptimizationLevel::O3;

    llvm::ModulePassManager modulePassMgr;
    if (optLevel == llvm::PassBuilder::OptimizationLevel::O0)
        modulePassMgr = passBuilder.buildO0DefaultPipeline(optLevel);
    else
        modulePassMgr = passBuilder.buildPerModuleDefaultPipeline(optLevel);

    llvm::legacy::PassManager codegenPassMgr;
    codegenPassMgr.add(new llvm::TargetTransformInfoWrapperPass(targetMachine->getTargetIRAnalysis()));
    targetMachine->addPassesToEmitFile(codegenPassMgr, dest, nullptr, llvm::CGFT_ObjectFile);

    modulePassMgr.run(modu, moduleMgr);
    codegenPassMgr.run(modu);

    dest.flush();
    dest.close();

    // Output IR code
    if (buildParameters.buildCfg->backendLLVM.outputIR)
    {
        Utf8 irName = path.string();
        irName.append(".ir");
        llvm::raw_fd_ostream destFileIR(irName.c_str(), err, llvm::sys::fs::OF_None);
        modu.print(destFileIR, nullptr);
        destFileIR.flush();
        destFileIR.close();
        g_Log.messageVerbose(Fmt("generating llvm %s", irName.c_str()));
    }

    delete targetMachine;
    return true;
}

bool BackendLLVM::generateOutput(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    Vector<Path> files;
    files.reserve(numPreCompileBuffers);
    for (auto i = 0; i < numPreCompileBuffers; i++)
        files.push_back(perThread[buildParameters.compileType][i]->filename);
    return BackendLinker::link(buildParameters, module, files);
}

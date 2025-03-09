#include "pch.h"
#include "Backend/LLVM/Main/Llvm.h"
#include "Backend/LLVM/Debug/LlvmDebug.h"
#include "Backend/LLVM/Main/Llvm_Macros.h"
#include "Os/Os.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

Llvm::Llvm(Module* mdl) :
    Backend{mdl}
{
}

void Llvm::createRuntime(LlvmEncoder& pp)
{
    const auto& buildParameters = pp.buildParams;
    const auto  precompileIndex = buildParameters.precompileIndex;
    auto&       context         = *pp.llvmContext;
    auto&       modu            = *pp.llvmModule;

    // SwagInterface
    {
        llvm::Type* members[] = {PTR_I8_TY(), PTR_I8_TY()};
        pp.interfaceTy        = llvm::StructType::create(context, members, "SwagInterface");
    }

    // SwagErrorValue
    {
        llvm::Type* members[] = {
            PTR_I8_TY(), // value
            PTR_I8_TY(), // value
            I32_TY(),    // pushUsedAlloc
            I32_TY(),    // pushTraceIndex
            I32_TY(),    // pushHasError
            I32_TY()};   // padding
        pp.errorTy = llvm::StructType::create(context, members, "SwagErrorValue");
    }

    // SwagScratchAllocator
    {
        llvm::Type* members[] = {
            pp.interfaceTy, // ScratchAllocator allocator
            PTR_I8_TY(),    // ScratchAllocator block
            I64_TY(),       // ScratchAllocator capacity
            I64_TY(),       // ScratchAllocator used
            I64_TY(),       // ScratchAllocator maxUsed
            PTR_I8_TY(),    // ScratchAllocator firstLeak
            I64_TY(),       // ScratchAllocator totalLeak
            I64_TY()};      // ScratchAllocator maxLeak
        pp.scratchTy = llvm::StructType::create(context, members, "SwagScratchAllocator");
    }

    // SwagContext
    {
        llvm::Type* members[] = {
            pp.interfaceTy,                                                 // allocator
            I64_TY(),                                                       // flags
            pp.scratchTy,                                                   // tempAllocator
            pp.scratchTy,                                                   // errorAllocator
            PTR_I8_TY(),                                                    // debugAllocator
            I64_TY(),                                                       // runtimeFlags
            I64_TY(),                                                       // user0
            I64_TY(),                                                       // user1
            I64_TY(),                                                       // user2
            I64_TY(),                                                       // user3
            llvm::ArrayType::get(I8_TY(), SWAG_MAX_TRACES * sizeof(void*)), // traces
            llvm::ArrayType::get(pp.errorTy, SWAG_MAX_ERRORS),              // errors
            llvm::ArrayType::get(I8_TY(), sizeof(SwagSourceCodeLocation)),  // exceptionLoc
            llvm::ArrayType::get(I8_TY(), 4 * sizeof(void*)),               // exceptionParams
            PTR_I8_TY(),                                                    // panic
            PTR_I8_TY(),                                                    // curError
            PTR_I8_TY(),                                                    // curError
            I32_TY(),                                                       // errorIndex
            I32_TY(),                                                       // traceIndex
            I32_TY(),                                                       // hasError
        };

        static_assert(sizeof(SwagContext) == 1600);
        pp.contextTy = llvm::StructType::create(context, members, "SwagContext");
        SWAG_ASSERT(pp.contextTy->isSized());
    }

    // SwagSlice
    {
        llvm::Type* members[] = {
            PTR_I8_TY(),
            I64_TY(),
        };
        pp.sliceTy = llvm::StructType::create(context, members, "SwagSlice");
        SWAG_ASSERT(pp.sliceTy->isSized());
    }

    // swag_alloctor_t
    {
        llvm::Type* params[] = {
            PTR_I64_TY(),
            PTR_I64_TY(),
        };
        pp.allocatorTy = llvm::FunctionType::get(VOID_TY(), params, false);
    }

    // byteCodeRun
    {
        llvm::Type* params[] = {
            PTR_I8_TY(),
        };
        pp.bytecodeRunTy = llvm::FunctionType::get(VOID_TY(), params, true);
    }

    // makeCallback
    {
        llvm::Type* params[] = {
            PTR_I8_TY(),
        };
        pp.makeCallbackTy = llvm::FunctionType::get(PTR_I8_TY(), params, false);
    }

    // SwagProcessInfo
    {
        llvm::Type* members[] = {
            pp.sliceTy,
            pp.sliceTy,
            I64_TY(),
            pp.contextTy->getPointerTo(),
            pp.bytecodeRunTy->getPointerTo(),
            pp.makeCallbackTy->getPointerTo(),
            I32_TY(),
        };
        pp.processInfosTy = llvm::StructType::create(context, members, "SwagProcessInfo");
        SWAG_ASSERT(pp.processInfosTy->isSized());
    }

    // mainContext
    if (precompileIndex == 0)
    {
        pp.mainContext       = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::WeakAnyLinkage, llvm::ConstantAggregateZero::get(pp.contextTy), "swag_mainContext");
        pp.defaultAllocTable = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::WeakAnyLinkage, llvm::ConstantPointerNull::get(pp.allocatorTy->getPointerTo()), "swag_defaultAllocTable");
        pp.processInfos =
        new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::WeakAnyLinkage, llvm::ConstantAggregateZero::get(pp.processInfosTy), "swag_processInfos");
        pp.symTlsThreadLocalId = new llvm::GlobalVariable(modu, I64_TY(), false, llvm::GlobalValue::WeakAnyLinkage, llvm::ConstantInt::get(I64_TY(), 0), "swag_tls_threadLocalId");
    }
    else
    {
        pp.mainContext         = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::ExternalLinkage, nullptr, "swag_mainContext");
        pp.defaultAllocTable   = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::ExternalLinkage, nullptr, "swag_defaultAllocTable");
        pp.processInfos        = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::ExternalLinkage, nullptr, "swag_processInfos");
        pp.symTlsThreadLocalId = new llvm::GlobalVariable(modu, I64_TY(), false, llvm::GlobalValue::ExternalLinkage, nullptr, "swag_tls_threadLocalId");
    }

    // LIBC functions without llvm intrinsics
    {
        pp.fnAcosF32  = modu.getOrInsertFunction(g_LangSpec->name_acosf.cstr(), llvm::FunctionType::get(F32_TY(), F32_TY(), false));
        pp.fnAcosF64  = modu.getOrInsertFunction(g_LangSpec->name_acos.cstr(), llvm::FunctionType::get(F64_TY(), F64_TY(), false));
        pp.fnAsinF32  = modu.getOrInsertFunction(g_LangSpec->name_asinf.cstr(), llvm::FunctionType::get(F32_TY(), F32_TY(), false));
        pp.fnAsinF64  = modu.getOrInsertFunction(g_LangSpec->name_asin.cstr(), llvm::FunctionType::get(F64_TY(), F64_TY(), false));
        pp.fnTanF32   = modu.getOrInsertFunction(g_LangSpec->name_tanf.cstr(), llvm::FunctionType::get(F32_TY(), F32_TY(), false));
        pp.fnTanF64   = modu.getOrInsertFunction(g_LangSpec->name_tan.cstr(), llvm::FunctionType::get(F64_TY(), F64_TY(), false));
        pp.fnAtanF32  = modu.getOrInsertFunction(g_LangSpec->name_atanf.cstr(), llvm::FunctionType::get(F32_TY(), F32_TY(), false));
        pp.fnAtanF64  = modu.getOrInsertFunction(g_LangSpec->name_atan.cstr(), llvm::FunctionType::get(F64_TY(), F64_TY(), false));
        pp.fnSinhF32  = modu.getOrInsertFunction(g_LangSpec->name_sinhf.cstr(), llvm::FunctionType::get(F32_TY(), F32_TY(), false));
        pp.fnSinhF64  = modu.getOrInsertFunction(g_LangSpec->name_sinh.cstr(), llvm::FunctionType::get(F64_TY(), F64_TY(), false));
        pp.fnCoshF32  = modu.getOrInsertFunction(g_LangSpec->name_coshf.cstr(), llvm::FunctionType::get(F32_TY(), F32_TY(), false));
        pp.fnCoshF64  = modu.getOrInsertFunction(g_LangSpec->name_cosh.cstr(), llvm::FunctionType::get(F64_TY(), F64_TY(), false));
        pp.fnTanhF32  = modu.getOrInsertFunction(g_LangSpec->name_tanhf.cstr(), llvm::FunctionType::get(F32_TY(), F32_TY(), false));
        pp.fnTanhF64  = modu.getOrInsertFunction(g_LangSpec->name_tanh.cstr(), llvm::FunctionType::get(F64_TY(), F64_TY(), false));
        pp.fnPowF32   = modu.getOrInsertFunction(g_LangSpec->name_powf.cstr(), llvm::FunctionType::get(F32_TY(), {F32_TY(), F32_TY()}, false));
        pp.fnPowF64   = modu.getOrInsertFunction(g_LangSpec->name_pow.cstr(), llvm::FunctionType::get(F64_TY(), {F64_TY(), F64_TY()}, false));
        pp.fnAtan2F32 = modu.getOrInsertFunction(g_LangSpec->name_atan2f.cstr(), llvm::FunctionType::get(F32_TY(), {F32_TY(), F32_TY()}, false));
        pp.fnAtan2F64 = modu.getOrInsertFunction(g_LangSpec->name_atan2.cstr(), llvm::FunctionType::get(F64_TY(), {F64_TY(), F64_TY()}, false));
        pp.fnMemCmp   = modu.getOrInsertFunction(g_LangSpec->name_memcmp.cstr(), llvm::FunctionType::get(I32_TY(), {PTR_I8_TY(), PTR_I8_TY(), I64_TY()}, false));
        pp.fnStrlen   = modu.getOrInsertFunction(g_LangSpec->name_strlen.cstr(), llvm::FunctionType::get(I64_TY(), {PTR_I8_TY()}, false));
        pp.fnStrcmp   = modu.getOrInsertFunction(g_LangSpec->name_strcmp.cstr(), llvm::FunctionType::get(I64_TY(), {PTR_I8_TY(), PTR_I8_TY()}, false));
        pp.fnMalloc   = modu.getOrInsertFunction(g_LangSpec->name_malloc.cstr(), llvm::FunctionType::get(PTR_I8_TY(), {I64_TY()}, false));
        pp.fnReAlloc  = modu.getOrInsertFunction(g_LangSpec->name_realloc.cstr(), llvm::FunctionType::get(PTR_I8_TY(), {PTR_I8_TY(), I64_TY()}, false));
        pp.fnFree     = modu.getOrInsertFunction(g_LangSpec->name_free.cstr(), llvm::FunctionType::get(VOID_TY(), {PTR_I8_TY()}, false));
    }
}

JobResult Llvm::prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob)
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;

    allocatePerObj<LlvmEncoder>(buildParameters);
    auto& pp = encoder<LlvmEncoder>(ct, precompileIndex);
    pp.init(buildParameters);

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->startBuilding();

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        pp.filename = form("%s%d", buildParameters.module->name.cstr(), precompileIndex);
        pp.filename += getObjectFileExtension(g_CommandLine.target);

        pp.llvmContext = new llvm::LLVMContext();
        pp.llvmContext->setDiscardValueNames(true);

        pp.llvmModule = new llvm::Module(pp.filename.cstr(), *pp.llvmContext);
        pp.builder    = new llvm::IRBuilder(*pp.llvmContext);

        if (buildParameters.buildCfg->backendDebugInfos)
        {
            pp.dbg = new LlvmDebug;
            pp.dbg->setup(this, pp.llvmModule);
        }

        createRuntime(pp);
        emitDataSegment(pp, &module->bssSegment);
        emitDataSegment(pp, &module->mutableSegment);
        emitDataSegment(pp, &module->constantSegment);
        emitDataSegment(pp, &module->tlsSegment);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;
        emitAllFunctionBodies(pp.buildParams, ownerJob);

        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        if (g_Workspace->bootstrapModule->numErrors || g_Workspace->runtimeModule->numErrors)
            ++module->numErrors;
        if (module->numErrors)
            return JobResult::ReleaseJob;
        if (stage == 1)
            return JobResult::ReleaseJob;

        if (precompileIndex == 0)
        {
            emitInitSeg(pp, &module->mutableSegment, SegmentKind::Data);
            emitInitSeg(pp, &module->constantSegment, SegmentKind::Constant);
            emitInitSeg(pp, &module->tlsSegment, SegmentKind::Tls);
            emitGetTypeTable(pp);
            emitGlobalInit(pp);
            emitGlobalDrop(pp);
            emitGlobalPreMain(pp);
            emitMain(pp);
        }

        // Output file
        generateObjFile(pp);

        delete pp.llvmModule;
        delete pp.llvmContext;
        delete pp.builder;
        delete pp.dbg;
    }

    return JobResult::ReleaseJob;
}

void Llvm::generateObjFile(LlvmEncoder& pp)
{
    auto& modu = *pp.llvmModule;

    // Debug infos
    if (pp.dbg)
        pp.dbg->finalize();

    // Target triple
    llvm::StringRef archName;
    switch (g_CommandLine.target.arch)
    {
        case SwagTargetArch::X86_64:
            archName = llvm::Triple::getArchTypeName(llvm::Triple::x86_64);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    llvm::StringRef vendorName, osName, abiName;
    switch (g_CommandLine.target.os)
    {
        case SwagTargetOs::Windows:
            osName     = llvm::Triple::getOSTypeName(llvm::Triple::Win32);
            vendorName = llvm::Triple::getVendorTypeName(llvm::Triple::PC);
            abiName    = llvm::Triple::getEnvironmentTypeName(llvm::Triple::MSVC);
            break;
        case SwagTargetOs::Linux:
            osName = llvm::Triple::getOSTypeName(llvm::Triple::Linux);
            break;
        case SwagTargetOs::MacOsX:
            osName = llvm::Triple::getOSTypeName(llvm::Triple::MacOSX);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    Utf8 targetTriple = form("%s-%s-%s-%s", archName.bytes_begin(), vendorName.bytes_begin(), osName.bytes_begin(), abiName.bytes_begin());

    // Setup target
    modu.setTargetTriple(targetTriple.cstr());
    std::string error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple.cstr(), error);
    if (!target)
        Report::internalError(pp.module, form("the LLVM backend failed to create the target [[%s]]", targetTriple.cstr()));

    // Create target machine
    Utf8 cpu = "generic";
    Utf8 features;

    // CPU. If we are compiling for the native env, then get cpu features.
    if (g_CommandLine.target.os == OS::getNativeTarget().os && g_CommandLine.target.arch == OS::getNativeTarget().arch &&
        (g_CommandLine.target.cpu.empty() || g_CommandLine.target.cpu == OS::getNativeTarget().cpu))
    {
        cpu = OS::getNativeTarget().cpu;

        llvm::StringMap<bool> feat;
        llvm::sys::getHostCPUFeatures(feat);
        bool first = true;
        for (auto& it : feat)
        {
            if (!it.getValue())
                continue;

            if (!first)
                features += ",";
            first = false;
            features += "+";
            features += it.getKey().str().c_str();
        }
    }
    else if (!g_CommandLine.target.cpu.empty())
    {
        cpu = g_CommandLine.target.cpu.length();
    }

    llvm::TargetOptions opt;
    auto                buildCfg     = pp.buildParams.buildCfg;
    const auto&         buildCfgLLVM = buildCfg->backendLLVM;

    opt.AllowFPOpFusion     = buildCfgLLVM.fpMathFma ? llvm::FPOpFusion::Fast : llvm::FPOpFusion::Standard;
    opt.NoNaNsFPMath        = buildCfgLLVM.fpMathNoNaN;
    opt.NoInfsFPMath        = buildCfgLLVM.fpMathNoInf;
    opt.UnsafeFPMath        = buildCfgLLVM.fpMathUnsafe;
    opt.NoSignedZerosFPMath = buildCfgLLVM.fpMathNoSignedZero;
    opt.ApproxFuncFPMath    = buildCfgLLVM.fpMathApproxFunc;

    auto rm            = std::optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple.cstr(), cpu.cstr(), features.cstr(), opt, rm);

    switch (buildCfg->backendOptimize)
    {
        case BuildCfgBackendOptim::O0:
            targetMachine->setOptLevel(llvm::CodeGenOpt::None);
            break;
        case BuildCfgBackendOptim::O1:
            targetMachine->setOptLevel(llvm::CodeGenOpt::Less);
            break;
        case BuildCfgBackendOptim::O2:
        case BuildCfgBackendOptim::Os:
        case BuildCfgBackendOptim::Oz:
            targetMachine->setOptLevel(llvm::CodeGenOpt::Default);
            break;
        case BuildCfgBackendOptim::O3:
            targetMachine->setOptLevel(llvm::CodeGenOpt::Aggressive);
            break;
    }

    targetMachine->setO0WantsFastISel(true);
    modu.setDataLayout(targetMachine->createDataLayout());

    // Output file
    auto targetPath = getCacheFolder();
    auto path       = targetPath;
    path.append(pp.filename);
    std::error_code           err;
    llvm::raw_fd_ostream      dest(path.cstr(), err, llvm::sys::fs::OF_None);
    llvm::legacy::PassManager llvmPass;

    // Pipeline configurations
    llvm::PipelineTuningOptions pipelineOptions;
    pipelineOptions.LoopUnrolling     = buildCfg->backendOptimize > BuildCfgBackendOptim::O1;
    pipelineOptions.SLPVectorization  = buildCfg->backendOptimize > BuildCfgBackendOptim::O1;
    pipelineOptions.LoopVectorization = buildCfg->backendOptimize > BuildCfgBackendOptim::O1;
    pipelineOptions.LoopInterleaving  = buildCfg->backendOptimize > BuildCfgBackendOptim::O1;
    pipelineOptions.MergeFunctions    = buildCfg->backendOptimize > BuildCfgBackendOptim::O1;

    llvm::PassBuilder             passBuilder(targetMachine, pipelineOptions);
    llvm::LoopAnalysisManager     loopMgr;
    llvm::FunctionAnalysisManager functionMgr;
    llvm::CGSCCAnalysisManager    cgsccMgr;
    llvm::ModuleAnalysisManager   moduleMgr;

    passBuilder.registerModuleAnalyses(moduleMgr);
    passBuilder.registerCGSCCAnalyses(cgsccMgr);
    passBuilder.registerFunctionAnalyses(functionMgr);
    passBuilder.registerLoopAnalyses(loopMgr);
    passBuilder.crossRegisterProxies(loopMgr, functionMgr, cgsccMgr, moduleMgr);

    llvm::OptimizationLevel optLevel;
    switch (buildCfg->backendOptimize)
    {
        case BuildCfgBackendOptim::O0:
            optLevel = llvm::OptimizationLevel::O0;
            break;
        case BuildCfgBackendOptim::O1:
            optLevel = llvm::OptimizationLevel::O1;
            break;
        case BuildCfgBackendOptim::O2:
            optLevel = llvm::OptimizationLevel::O2;
            break;
        case BuildCfgBackendOptim::O3:
            optLevel = llvm::OptimizationLevel::O3;
            break;
        case BuildCfgBackendOptim::Os:
            optLevel = llvm::OptimizationLevel::Os;
            break;
        case BuildCfgBackendOptim::Oz:
            optLevel = llvm::OptimizationLevel::Oz;
            break;
    }

    llvm::ModulePassManager modulePassMgr;
    if (optLevel == llvm::OptimizationLevel::O0)
        modulePassMgr = passBuilder.buildO0DefaultPipeline(optLevel);
    else
        modulePassMgr = passBuilder.buildPerModuleDefaultPipeline(optLevel);

#ifdef SWAG_DEV_MODE
    modulePassMgr.addPass(llvm::VerifierPass());
#endif

    llvm::legacy::PassManager codegenPassMgr;
    codegenPassMgr.add(new llvm::TargetTransformInfoWrapperPass(targetMachine->getTargetIRAnalysis()));
    targetMachine->addPassesToEmitFile(codegenPassMgr, dest, nullptr, llvm::CGFT_ObjectFile);

    modulePassMgr.run(modu, moduleMgr);
    codegenPassMgr.run(modu);

    dest.flush();
    dest.close();

    // Output IR code
    if (buildCfgLLVM.outputIR)
    {
        Utf8 irName = path;
        irName.append(".ir");
        llvm::raw_fd_ostream destFileIR(irName.cstr(), err, llvm::sys::fs::OF_None);
        modu.print(destFileIR, nullptr);
        destFileIR.flush();
        destFileIR.close();
        g_Log.messageVerbose(form("generating llvm %s", irName.cstr()));
    }

    delete targetMachine;
}

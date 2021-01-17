#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "Workspace.h"
#include "BackendLinker.h"
#include "Module.h"

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
            pp.interfaceTy,
            llvm::Type::getInt64Ty(context),
            llvm::ArrayType::get(llvm::Type::getInt8Ty(context), MAX_LEN_ERROR_MSG),
            llvm::Type::getInt32Ty(context),
            llvm::Type::getInt32Ty(context), // padding
        };

        static_assert(sizeof(SwagContext) == 16 + 8 + MAX_LEN_ERROR_MSG + 4 + 4);
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

    // swag_process_infos_t
    {
        llvm::Type* members[] = {
            pp.sliceTy,
            llvm::Type::getInt64Ty(context),
            pp.contextTy->getPointerTo(),
            pp.bytecodeRunTy->getPointerTo(),
            pp.makeCallbackTy->getPointerTo(),
        };
        pp.processInfosTy = llvm::StructType::create(context, members, "swag_process_infos_t");
        SWAG_ASSERT(pp.processInfosTy->isSized());
    }

    // mainContext
    if (precompileIndex == 0)
    {
        pp.mainContext       = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantAggregateZero::get(pp.contextTy), "swag_mainContext");
        pp.defaultAllocTable = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantPointerNull::get(pp.allocatorTy->getPointerTo()), "swag_defaultAllocTable");
        pp.processInfos      = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantAggregateZero::get(pp.processInfosTy), "swag_processInfos");
    }
    else
    {
        pp.mainContext       = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::InternalLinkage, nullptr, "swag_mainContext");
        pp.defaultAllocTable = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::InternalLinkage, nullptr, "swag_defaultAllocTable");
        pp.processInfos      = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::InternalLinkage, nullptr, "swag_processInfos");
    }

    // LIBC functions
    {

        pp.fn_acosf32 = modu.getOrInsertFunction("acosf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_acosf64 = modu.getOrInsertFunction("acos", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_asinf32 = modu.getOrInsertFunction("asinf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_asinf64 = modu.getOrInsertFunction("asin", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_tanf32  = modu.getOrInsertFunction("tanf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_tanf64  = modu.getOrInsertFunction("tan", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_atanf32 = modu.getOrInsertFunction("atanf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_atanf64 = modu.getOrInsertFunction("atan", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_sinhf32 = modu.getOrInsertFunction("sinhf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_sinhf64 = modu.getOrInsertFunction("sinh", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_coshf32 = modu.getOrInsertFunction("coshf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_coshf64 = modu.getOrInsertFunction("cosh", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_tanhf32 = modu.getOrInsertFunction("tanhf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context), false));
        pp.fn_tanhf64 = modu.getOrInsertFunction("tanh", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), false));
        pp.fn_powf32  = modu.getOrInsertFunction("powf", ::llvm::FunctionType::get(llvm::Type::getFloatTy(context), {llvm::Type::getFloatTy(context), llvm::Type::getFloatTy(context)}, false));
        pp.fn_powf64  = modu.getOrInsertFunction("pow", ::llvm::FunctionType::get(llvm::Type::getDoubleTy(context), {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)}, false));
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
    pp.cst0_i64 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
    pp.cst1_i64 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 1);
    pp.cst0_f32 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 0);
    pp.cst0_f64 = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0);
    pp.cst_null = llvm::ConstantPointerNull::get(llvm::Type::getInt64PtrTy(context));

    return true;
}

JobResult BackendLLVM::prepareOutput(const BuildParameters& buildParameters, Job* ownerJob)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (!perThread[ct][precompileIndex])
        perThread[ct][precompileIndex] = new LLVMPerThread;
    auto& pp = *perThread[ct][precompileIndex];

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->printUserMessage(buildParameters);

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        pp.filename = format("%s%d", buildParameters.outputFileName.c_str(), precompileIndex);
        pp.filename += ".obj";

        pp.context = new llvm::LLVMContext();
        pp.module  = new llvm::Module(pp.filename.c_str(), *pp.context);
        pp.builder = new llvm::IRBuilder<>(*pp.context);

        if (buildParameters.buildCfg->backendDebugInformations)
        {
            pp.dbg = new BackendLLVMDbg;
            pp.dbg->setup(this, pp.module);
        }

        if (g_CommandLine.verbose)
            g_Log.verbosePass(LogPassType::Info, "LLVM precompile", pp.filename);

        createRuntime(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->typeSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        if (module->numErrors)
            return JobResult::ReleaseJob;

        if (precompileIndex == 0)
        {
            emitInitMutableSeg(buildParameters);
            emitInitTypeSeg(buildParameters);
            emitInitConstantSeg(buildParameters);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
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
    string archName;
    switch (g_CommandLine.arch)
    {
    case BackendArch::X86_64:
        archName = (const char*) llvm::Triple::getArchTypeName(llvm::Triple::x86_64).bytes_begin();
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    string vendorName;
    switch (g_CommandLine.vendor)
    {
    case BackendVendor::Pc:
        vendorName = (const char*) llvm::Triple::getVendorTypeName(llvm::Triple::PC).bytes_begin();
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    string osName;
    switch (g_CommandLine.os)
    {
    case BackendOs::Windows:
        osName = (const char*) llvm::Triple::getOSTypeName(llvm::Triple::Win32).bytes_begin();
        break;
    case BackendOs::Linux:
        osName = (const char*) llvm::Triple::getOSTypeName(llvm::Triple::Linux).bytes_begin();
        break;
    case BackendOs::MacOSX:
        osName = (const char*) llvm::Triple::getOSTypeName(llvm::Triple::MacOSX).bytes_begin();
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    string abiName;
    switch (g_CommandLine.abi)
    {
    case BackendAbi::Msvc:
        abiName = (const char*) llvm::Triple::getEnvironmentTypeName(llvm::Triple::MSVC).bytes_begin();
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    string targetTriple = format("%s-%s-%s-%s", archName.c_str(), vendorName.c_str(), osName.c_str(), abiName.c_str()).c_str();

    // Setup target
    modu.setTargetTriple(targetTriple);
    std::string error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target)
    {
        g_Log.error(format("llvm backend cannot create target '%s'", targetTriple.c_str()));
        return false;
    }

    auto                cpu      = "generic";
    auto                features = "";
    llvm::TargetOptions opt;
    auto                rm            = llvm::Optional<llvm::Reloc::Model>();
    auto                targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);
    modu.setDataLayout(targetMachine->createDataLayout());

    // Output file
    auto                      targetPath = Backend::getCacheFolder(buildParameters);
    auto                      path       = targetPath + "/" + pp.filename;
    auto                      filename   = path;
    std::error_code           EC;
    llvm::raw_fd_ostream      dest(filename, EC, llvm::sys::fs::OF_None);
    llvm::legacy::PassManager llvmPass;

    // Optimize passes
    if (buildParameters.buildCfg->backendOptimizeSpeed)
        targetMachine->setOptLevel(llvm::CodeGenOpt::Aggressive);
    else
        targetMachine->setOptLevel(llvm::CodeGenOpt::None);
    targetMachine->setO0WantsFastISel(true);

    bool isDebug = !buildParameters.buildCfg->backendOptimizeSpeed && !buildParameters.buildCfg->backendOptimizeSize;

    llvm::PassManagerBuilder pmb;
    pmb.OptLevel           = buildParameters.buildCfg->backendOptimizeSpeed ? 3 : 0;
    pmb.SizeLevel          = buildParameters.buildCfg->backendOptimizeSize ? 2 : 0;
    pmb.VerifyInput        = g_CommandLine.devMode;
    pmb.VerifyOutput       = g_CommandLine.devMode;
    pmb.DisableTailCalls   = isDebug;
    pmb.DisableUnrollLoops = isDebug;
    pmb.DisableGVNLoadPRE  = isDebug;
    pmb.SLPVectorize       = !isDebug;
    pmb.LoopVectorize      = !isDebug;
    pmb.RerollLoops        = !isDebug;
    pmb.NewGVN             = !isDebug;
    pmb.MergeFunctions     = !isDebug;
    pmb.PrepareForLTO      = false;
    pmb.PrepareForThinLTO  = false;
    pmb.PerformThinLTO     = false;
    pmb.Inliner            = isDebug ? nullptr : llvm::createFunctionInliningPass(pmb.OptLevel, pmb.SizeLevel, true);
    pmb.LibraryInfo        = new llvm::TargetLibraryInfoImpl(llvm::Triple(modu.getTargetTriple()));

    if (!isDebug)
        llvmPass.add(new llvm::TargetTransformInfoWrapperPass(targetMachine->getTargetIRAnalysis()));
    pmb.populateModulePassManager(llvmPass);

    // Generate obj file pass
    targetMachine->addPassesToEmitFile(llvmPass, dest, nullptr, llvm::CGFT_ObjectFile);

    llvmPass.run(modu);
    dest.flush();
    dest.close();

    // Output IR code
    if (buildParameters.buildCfg->backendLLVM.outputIR || g_CommandLine.devMode)
    {
        auto                 filenameIR = path;
        llvm::raw_fd_ostream destFileIR(filename + ".ir", EC, llvm::sys::fs::OF_None);
        modu.print(destFileIR, nullptr);
        destFileIR.flush();
        destFileIR.close();
    }

    delete targetMachine;
    return true;
}

bool BackendLLVM::generateOutput(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    vector<string> files;
    files.reserve(numPreCompileBuffers);
    for (auto i = 0; i < numPreCompileBuffers; i++)
        files.push_back(perThread[buildParameters.compileType][i]->filename);
    return BackendLinker::link(buildParameters, module, files);
}

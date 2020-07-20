#include "pch.h"
#include "BackendLLVM.h"
#include "Job.h"
#include "Module.h"
#include "Workspace.h"
#include "OS.h"
#include "BackendSetupWin32.h"
#include "BackendLinkerWin32.h"
#include "DataSegment.h"

void BackendLLVM::setup()
{
}

bool BackendLLVM::createRuntime(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = perThread[ct][precompileIndex];
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
            pp.interfaceTy};
        pp.contextTy = llvm::StructType::create(context, members, "swag_context_t");
    }

    // swag_slice_t
    {
        llvm::Type* members[] = {
            llvm::Type::getInt8PtrTy(context),
            llvm::Type::getInt64Ty(context),
        };
        pp.sliceTy = llvm::StructType::create(context, members, "swag_slice_t");
    }

    // swag_alloctor_t
    {
        llvm::Type* params[] = {
            llvm::Type::getInt64PtrTy(context),
            llvm::Type::getInt64PtrTy(context),
        };
        pp.allocatorTy = llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false);
    }

    // swag_alloctor_t
    {
        llvm::Type* params[] = {
            llvm::Type::getInt8PtrTy(context),
        };
        pp.bytecodeRunTy = llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, true);
    }

    // swag_process_infos_t
    {
        llvm::Type* members[] = {
            pp.sliceTy,
            llvm::Type::getInt32Ty(context),
            pp.contextTy->getPointerTo(),
            pp.bytecodeRunTy,
        };
        pp.processinfosTy = llvm::StructType::create(context, members, "swag_process_infos_t");
    }

    // mainContext
    if (precompileIndex == 0)
    {
        pp.mainContext = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::InternalLinkage, nullptr, "mainContext");
    }

    return true;
}

JobResult BackendLLVM::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (perThread[ct][precompileIndex].pass == BackendPreCompilePass::Init)
    {
        perThread[ct][precompileIndex].pass = BackendPreCompilePass::FunctionBodies;

        SWAG_ASSERT(!module->name.empty());
        perThread[ct][precompileIndex].filename = format("%s%d", module->name.c_str(), precompileIndex);
        perThread[ct][precompileIndex].filename += buildParameters.postFix;
        perThread[ct][precompileIndex].filename += ".obj";

        perThread[ct][precompileIndex].context = new llvm::LLVMContext();
        perThread[ct][precompileIndex].module  = new llvm::Module(perThread[ct][precompileIndex].filename.c_str(), *perThread[ct][precompileIndex].context);
        perThread[ct][precompileIndex].builder = new llvm::IRBuilder<>(*perThread[ct][precompileIndex].context);

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', llvm backend, generating files", perThread[ct][precompileIndex].filename.c_str(), module->byteCodeTestFunc.size()));

        createRuntime(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (perThread[ct][precompileIndex].pass == BackendPreCompilePass::FunctionBodies)
    {
        perThread[ct][precompileIndex].pass = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlivePending;
    }

    if (perThread[ct][precompileIndex].pass == BackendPreCompilePass::End)
    {
        if (precompileIndex == 0)
        {
            emitInitDataSeg(buildParameters);
            emitInitConstantSeg(buildParameters);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitMain(buildParameters);
        }

        // Output file
        generateObjFile(buildParameters);
    }

    return JobResult::ReleaseJob;
}

bool BackendLLVM::generateObjFile(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    perThread[ct][precompileIndex].module->setTargetTriple(targetTriple);

    std::string Error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    auto CPU      = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto                RM               = llvm::Optional<llvm::Reloc::Model>();
    auto                theTargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);
    perThread[ct][precompileIndex].module->setDataLayout(theTargetMachine->createDataLayout());

    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + perThread[ct][precompileIndex].filename;

    auto                 filename = path;
    std::error_code      EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager llvmPass;

    //llvmPass.add(llvm::createPromoteMemoryToRegisterPass());
    //llvmPass.add(llvm::createInstructionCombiningPass());
    //llvmPass.add(llvm::createReassociatePass());
    //llvmPass.add(llvm::createGVNPass());
    //llvmPass.add(llvm::createCFGSimplificationPass());

    //llvm::PassManagerBuilder pmb;
    //pmb.OptLevel           = 3;
    //pmb.SizeLevel          = 2;
    //pmb.Inliner            = llvm::createFunctionInliningPass(pmb.OptLevel, pmb.SizeLevel, true);
    //pmb.DisableUnrollLoops = false;
    //pmb.LoopVectorize      = true;
    //pmb.SLPVectorize       = true;
    //pmb.populateModulePassManager(llvmPass);

    if (theTargetMachine->addPassesToEmitFile(llvmPass, dest, nullptr, llvm::CGFT_ObjectFile))
    {
        //errs() << "TheTargetMachine can't emit a file of this type";
        SWAG_ASSERT(false);
        return false;
    }

    llvmPass.run(*perThread[ct][precompileIndex].module);
    dest.flush();
    dest.close();

    // Output IR code
    if (buildParameters.target.backendLLVM.outputIR)
    {
        auto                 filenameIR = path;
        llvm::raw_fd_ostream destFileIR(filename + ".ir", EC, llvm::sys::fs::OF_None);
        perThread[ct][precompileIndex].module->print(destFileIR, nullptr);
        destFileIR.flush();
        destFileIR.close();
    }

    return true;
}

bool BackendLLVM::compile(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    int ct = buildParameters.compileType;

    Utf8 linkArguments;
    BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

    // Add all object files
    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    for (auto i = 0; i < numPreCompileBuffers; i++)
    {
        SWAG_ASSERT(!perThread[ct][i].filename.empty());
        auto path = targetPath + "/" + perThread[ct][i].filename.c_str();
        linkArguments += path + " ";
    }

    bool     verbose   = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;
    uint32_t numErrors = 0;
    auto     cmdLine   = "\"" + BackendSetupWin32::linkerPath + BackendSetupWin32::linkerExe + "\" " + linkArguments;
    SWAG_CHECK(OS::doProcess(cmdLine, BackendSetupWin32::linkerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));

    g_Workspace.numErrors += numErrors;
    module->numErrors += numErrors;
    return numErrors == 0;
}

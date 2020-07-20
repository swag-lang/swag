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

JobResult BackendLLVM::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (perType[ct][precompileIndex].pass == BackendPreCompilePass::Init)
    {
        perType[ct][precompileIndex].pass = BackendPreCompilePass::FunctionBodies;

        SWAG_ASSERT(!module->name.empty());
        perType[ct][precompileIndex].bufferFiles = format("%s%d", module->name.c_str(), precompileIndex);
        perType[ct][precompileIndex].bufferFiles += buildParameters.postFix;
        perType[ct][precompileIndex].bufferFiles += ".obj";

        perType[ct][precompileIndex].llvmContext = new llvm::LLVMContext();
        perType[ct][precompileIndex].llvmModule  = new llvm::Module(perType[ct][precompileIndex].bufferFiles.c_str(), *perType[ct][precompileIndex].llvmContext);
        perType[ct][precompileIndex].llvmBuilder = new llvm::IRBuilder<>(*perType[ct][precompileIndex].llvmContext);

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', llvm backend, generating files", perType[ct][precompileIndex].bufferFiles.c_str(), module->byteCodeTestFunc.size()));

        createRuntime(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (perType[ct][precompileIndex].pass == BackendPreCompilePass::FunctionBodies)
    {
        perType[ct][precompileIndex].pass = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlivePending;
    }

    if (perType[ct][precompileIndex].pass == BackendPreCompilePass::End)
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

bool BackendLLVM::createRuntime(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& context = *perType[ct][precompileIndex].llvmContext;

    // swag_interface_t
    llvm::Type* members[] = {
        llvm::Type::getInt8PtrTy(context),
        llvm::Type::getInt8PtrTy(context)};
    llvm::StructType::create(context, members, "swag_interface_t");
    return true;
}

bool BackendLLVM::generateObjFile(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    perType[ct][precompileIndex].llvmModule->setTargetTriple(targetTriple);

    std::string Error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    auto CPU      = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto                RM               = llvm::Optional<llvm::Reloc::Model>();
    auto                theTargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);
    perType[ct][precompileIndex].llvmModule->setDataLayout(theTargetMachine->createDataLayout());

    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + perType[ct][precompileIndex].bufferFiles;

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

    llvmPass.run(*perType[ct][precompileIndex].llvmModule);
    dest.flush();
    dest.close();

    // Output IR code
    if (buildParameters.target.backendLLVM.outputIR)
    {
        auto                 filenameIR = path;
        llvm::raw_fd_ostream destFileIR(filename + ".ir", EC, llvm::sys::fs::OF_None);
        perType[ct][precompileIndex].llvmModule->print(destFileIR, nullptr);
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
        SWAG_ASSERT(!perType[ct][i].bufferFiles.empty());
        auto path = targetPath + "/" + perType[ct][i].bufferFiles.c_str();
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

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
    if (pass[ct][precompileIndex] == BackendPreCompilePass::Init)
    {
        pass[ct][precompileIndex] = BackendPreCompilePass::FunctionBodies;

        SWAG_ASSERT(!module->name.empty());
        bufferFiles[ct][precompileIndex] = format("%s%d", module->name.c_str(), precompileIndex);
        bufferFiles[ct][precompileIndex] += buildParameters.postFix;
        bufferFiles[ct][precompileIndex] += ".obj";

        llvmContext[ct][precompileIndex] = new llvm::LLVMContext();
        llvmModule[ct][precompileIndex]  = new llvm::Module(bufferFiles[ct][precompileIndex].c_str(), *llvmContext[ct][precompileIndex]);
        llvmBuilder[ct][precompileIndex] = new llvm::IRBuilder<>(*llvmContext[ct][precompileIndex]);

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', llvm backend, generating files", bufferFiles[ct][precompileIndex].c_str(), module->byteCodeTestFunc.size()));

        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (pass[ct][precompileIndex] == BackendPreCompilePass::FunctionBodies)
    {
        pass[ct][precompileIndex] = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlivePending;
    }

    if (pass[ct][precompileIndex] == BackendPreCompilePass::End)
    {
        if (precompileIndex == 0)
        {
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
    llvmModule[ct][precompileIndex]->setTargetTriple(targetTriple);

    std::string Error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    auto CPU      = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto                RM               = llvm::Optional<llvm::Reloc::Model>();
    auto                theTargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);
    llvmModule[ct][precompileIndex]->setDataLayout(theTargetMachine->createDataLayout());

    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + bufferFiles[ct][precompileIndex];

    auto                 filename = path;
    std::error_code      EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager llvmPass;

    llvmPass.add(llvm::createPromoteMemoryToRegisterPass());
    llvmPass.add(llvm::createInstructionCombiningPass());
    llvmPass.add(llvm::createReassociatePass());
    llvmPass.add(llvm::createGVNPass());
    llvmPass.add(llvm::createCFGSimplificationPass());

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

    llvmPass.run(*llvmModule[ct][precompileIndex]);
    dest.flush();
    dest.close();

    // Output IR code
    if (buildParameters.target.backendLLVM.outputIR)
    {
        auto                 filenameIR = path;
        llvm::raw_fd_ostream destFileIR(filename + ".ir", EC, llvm::sys::fs::OF_None);
        llvmModule[ct][precompileIndex]->print(destFileIR, nullptr);
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
        SWAG_ASSERT(!bufferFiles[ct][i].empty());
        auto path = targetPath + "/" + bufferFiles[ct][i].c_str();
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

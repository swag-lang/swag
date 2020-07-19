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

JobResult BackendLLVM::preCompile(const BuildParameters& buildParameters, Job* ownerJob, int precompileIndex)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pass[precompileIndex] == BackendPreCompilePass::Init)
    {
        pass[precompileIndex] = BackendPreCompilePass::FunctionBodies;

        SWAG_ASSERT(!module->name.empty());
        bufferFiles[precompileIndex] = format("%s%d", module->name.c_str(), precompileIndex);
        bufferFiles[precompileIndex] += buildParameters.postFix;
        bufferFiles[precompileIndex] += ".obj";

        llvmContext[precompileIndex] = new llvm::LLVMContext();
        llvmModule[precompileIndex]  = new llvm::Module(bufferFiles[precompileIndex].c_str(), *llvmContext[precompileIndex]);
        llvmBuilder[precompileIndex] = new llvm::IRBuilder<>(*llvmContext[precompileIndex]);

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', llvm backend, generating files", bufferFiles[precompileIndex].c_str(), module->byteCodeTestFunc.size()));

        emitDataSegment(&module->bssSegment, precompileIndex);
        emitDataSegment(&module->mutableSegment, precompileIndex);
        emitDataSegment(&module->constantSegment, precompileIndex);
    }

    if (pass[precompileIndex] == BackendPreCompilePass::FunctionBodies)
    {
        pass[precompileIndex] = BackendPreCompilePass::End;
        emitAllFunctionBody(module, ownerJob, precompileIndex);
        return JobResult::KeepJobAlivePending;
    }

    if (pass[precompileIndex] == BackendPreCompilePass::End)
    {
        if (precompileIndex == 0)
        {
            emitGlobalDrop(buildParameters, precompileIndex);
            emitMain(buildParameters, precompileIndex);
        }

        // Output file
        generateObjFile(buildParameters, precompileIndex);
    }

    return JobResult::ReleaseJob;
}

bool BackendLLVM::generateObjFile(const BuildParameters& buildParameters, int precompileIndex)
{
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    llvmModule[precompileIndex]->setTargetTriple(targetTriple);

    std::string Error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    auto CPU      = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto                RM               = llvm::Optional<llvm::Reloc::Model>();
    auto                theTargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);
    llvmModule[precompileIndex]->setDataLayout(theTargetMachine->createDataLayout());

    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + bufferFiles[precompileIndex];

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

    llvmPass.run(*llvmModule[precompileIndex]);
    dest.flush();
    dest.close();

    // Output IR code
    if (buildParameters.target.backendLLVM.outputIR)
    {
        auto                 filenameIR = path;
        llvm::raw_fd_ostream destFileIR(filename + ".ir", EC, llvm::sys::fs::OF_None);
        llvmModule[precompileIndex]->print(destFileIR, nullptr);
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

    Utf8 linkArguments;
    BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

    // Add all object files
    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    for (auto i = 0; i < numPreCompileBuffers; i++)
    {
        SWAG_ASSERT(!bufferFiles[i].empty());
        auto path = targetPath + "/" + bufferFiles[i].c_str();
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

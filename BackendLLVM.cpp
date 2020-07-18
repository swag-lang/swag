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

JobResult BackendLLVM::preCompile(const BuildParameters& buildParameters, Job* ownerJob, int preCompileIndex)
{
    if (pass[preCompileIndex] == BackendPreCompilePass::Init)
    {
        pass[preCompileIndex] = BackendPreCompilePass::FunctionBodies;

        bufferFiles[preCompileIndex] = format("%s%d", module->name.c_str(), preCompileIndex) + ".obj";
        llvmContext[preCompileIndex] = new llvm::LLVMContext();
        llvmModule[preCompileIndex]  = new llvm::Module(bufferFiles[preCompileIndex].c_str(), *llvmContext[preCompileIndex]);
        llvmBuilder[preCompileIndex] = new llvm::IRBuilder<>(*llvmContext[preCompileIndex]);

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', llvm backend, generating files", bufferFiles[preCompileIndex].c_str(), module->byteCodeTestFunc.size()));

        // Do we need to generate the file ?
        if (!mustCompile)
            return JobResult::ReleaseJob;

        emitDataSegment(&module->bssSegment, preCompileIndex);
        emitDataSegment(&module->mutableSegment, preCompileIndex);
        emitDataSegment(&module->constantSegment, preCompileIndex);
    }

    if (pass[preCompileIndex] == BackendPreCompilePass::FunctionBodies)
    {
        pass[preCompileIndex] = BackendPreCompilePass::End;
        emitAllFunctionBody(module, ownerJob, preCompileIndex);
        return JobResult::KeepJobAlivePending;
    }

    if (pass[preCompileIndex] == BackendPreCompilePass::End)
    {
        if (preCompileIndex == 0)
        {
            emitMain(buildParameters, preCompileIndex);
        }

        // Output file
        generateObjFile(buildParameters, preCompileIndex);
    }

    return JobResult::ReleaseJob;
}

bool BackendLLVM::generateObjFile(const BuildParameters& buildParameters, int preCompileIndex)
{
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    llvmModule[preCompileIndex]->setTargetTriple(targetTriple);

    std::string Error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    auto CPU      = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto                RM               = llvm::Optional<llvm::Reloc::Model>();
    auto                theTargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);
    llvmModule[preCompileIndex]->setDataLayout(theTargetMachine->createDataLayout());

    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + bufferFiles[preCompileIndex];

    auto                 filename = path;
    std::error_code      EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager llvmPass;
    if (theTargetMachine->addPassesToEmitFile(llvmPass, dest, nullptr, llvm::CGFT_ObjectFile))
    {
        //errs() << "TheTargetMachine can't emit a file of this type";
        SWAG_ASSERT(false);
        return false;
    }

    llvmPass.run(*llvmModule[preCompileIndex]);
    dest.flush();
    dest.close();


    ///////////// OUTPUT RESULT /////////////
    auto                 filename1 = path;
    llvm::raw_fd_ostream dest1(filename + ".txt", EC, llvm::sys::fs::OF_None);
    llvmModule[preCompileIndex]->print(dest1, nullptr);
    dest1.flush();
    dest1.close();

    return true;
}

bool BackendLLVM::compile(const BuildParameters& buildParameters)
{
    Utf8 linkArguments;
    BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

    // Add all object files
    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    for (auto i = 0; i < numPreCompileBuffers; i++)
    {
        auto path = targetPath + "/" + bufferFiles[i].c_str();
        linkArguments += path + " ";
    }

    uint32_t numErrors = 0;
    auto     cmdLine   = "\"" + BackendSetupWin32::linkerPath + BackendSetupWin32::linkerExe + "\" " + linkArguments;
    SWAG_CHECK(OS::doProcess(cmdLine, BackendSetupWin32::linkerPath, true, numErrors, LogColor::DarkCyan, "CL "));

    g_Workspace.numErrors += numErrors;
    module->numErrors += numErrors;
    return numErrors == 0;
}

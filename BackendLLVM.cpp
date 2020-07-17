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

JobResult BackendLLVM::preCompile(Job* ownerJob, int preCompileIndex)
{  
    SWAG_ASSERT(preCompileIndex == 0);

    llvmModule = new llvm::Module(module->name.c_str(), llvmContext);

    emitDataSegment(&module->bssSegment, preCompileIndex);
    emitDataSegment(&module->mutableSegment, preCompileIndex);
    emitDataSegment(&module->constantSegment, preCompileIndex);
    emitMain();

    return JobResult::ReleaseJob;
}

bool BackendLLVM::link(const BuildParameters& buildParameters)
{
    Utf8 linkArguments;
    BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

    auto targetPath = g_Workspace.cachePath.string();
    auto path       = targetPath + "/" + module->name.c_str() + ".obj";
    linkArguments += path + " ";

    uint32_t numErrors = 0;
    auto     cmdLine   = "\"" + BackendSetupWin32::linkerPath + BackendSetupWin32::linkerExe + "\" " + linkArguments;
    SWAG_CHECK(OS::doProcess(cmdLine, BackendSetupWin32::linkerPath, true, numErrors, LogColor::DarkCyan, "CL "));

    g_Workspace.numErrors += numErrors;
    module->numErrors += numErrors;
    return numErrors == 0;
}

bool BackendLLVM::compile(const BuildParameters& buildParameters)
{
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    llvmModule->setTargetTriple(targetTriple);

    std::string Error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    auto CPU      = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto                RM               = llvm::Optional<llvm::Reloc::Model>();
    auto                theTargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);
    llvmModule->setDataLayout(theTargetMachine->createDataLayout());

    auto targetPath = g_Workspace.cachePath.string();
    auto path       = targetPath + "/" + module->name.c_str() + ".obj";

    auto                 filename = path;
    std::error_code      EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager pass;
    if (theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile))
    {
        //errs() << "TheTargetMachine can't emit a file of this type";
        SWAG_ASSERT(false);
        return false;
    }

    pass.run(*llvmModule);
    dest.flush();
    dest.close();

    SWAG_CHECK(link(buildParameters));
    return true;
}

#include "pch.h"
#include "BackendLLVM.h"
#include "Job.h"
#include "Module.h"

bool BackendLLVM::check()
{
    llvmModule = new llvm::Module(module->name.c_str(), llvmContext);
    return true;
}

JobResult BackendLLVM::preCompile(Job* ownerJob, int preCompileIndex)
{
    return JobResult::ReleaseJob;
}

bool BackendLLVM::compile(const BuildParameters& buildParameters)
{
    llvm::InitializeNativeTarget();
    //llvm::InitializeAllTargetInfos();
    //llvm::InitializeAllTargets();
    //llvm::InitializeAllTargetMCs();
    //llvm::InitializeAllAsmParsers();
    //llvm::InitializeAllAsmPrinters();

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

    auto                 filename = "f:/output.obj";
    std::error_code      EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager pass;
    // auto                      FileType = llvm::CGFT_ObjectFile;

    if (!theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile))
    {
        //errs() << "TheTargetMachine can't emit a file of this type";
        SWAG_ASSERT(false);
        return 1;
    }

    pass.run(*llvmModule);
    dest.flush();

    return true;
}

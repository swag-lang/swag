#include "pch.h"
#include "BackendLLVM.h"
#include "Job.h"
#include "Module.h"
#include "Workspace.h"
#include "OS.h"
#include "BackendHelpersWin32.h"

void BackendLLVM::setup()
{
}

JobResult BackendLLVM::preCompile(Job* ownerJob, int preCompileIndex)
{
    return JobResult::ReleaseJob;
}

bool BackendLLVM::compile(const BuildParameters& buildParameters)
{
    if (module->name != "tests.compiler")
        return true;

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    //llvm::InitializeAllTargetInfos();
    //llvm::InitializeAllTargets();
    //llvm::InitializeAllTargetMCs();
    //llvm::InitializeAllAsmParsers();
    //llvm::InitializeAllAsmPrinters();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    llvmModule        = new llvm::Module(module->name.c_str(), llvmContext);
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

    if (theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile))
    {
        //errs() << "TheTargetMachine can't emit a file of this type";
        SWAG_ASSERT(false);
        return 1;
    }

    pass.run(*llvmModule);
    dest.flush();
    dest.close();

    vector<Utf8> libPath;
    libPath.push_back(format(R"(%s\lib\x64)", BackendHelpersWin32::visualStudioPath.c_str()));
    libPath.push_back(format(R"(%s\lib\%s\um\x64)", BackendHelpersWin32::winSdkPath.c_str(), BackendHelpersWin32::winSdkVersion.c_str()));
    libPath.push_back(format(R"(%s\lib\%s\ucrt\x64)", BackendHelpersWin32::winSdkPath.c_str(), BackendHelpersWin32::winSdkVersion.c_str()));
    libPath.push_back(g_Workspace.targetPath.string());
    libPath.push_back(g_CommandLine.exePath.parent_path().string());

    string linkArguments;

    // This is mandatory under windows
    linkArguments += "kernel32.lib ";
    linkArguments += "user32.lib ";

    // Add swag.runtime
    linkArguments += "swag.runtime.lib ";

    // Default libraries
    linkArguments += "/NODEFAULTLIB ";
    linkArguments += "ucrt.lib ";
    linkArguments += "vcruntime.lib ";
    linkArguments += "msvcrt.lib ";

    linkArguments += "f:\\output.obj ";

    for (const auto& oneLibPath : libPath)
        linkArguments += "/LIBPATH:\"" + oneLibPath + "\" ";

    linkArguments += "/INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
    linkArguments += "/OUT:f:\\tt.exe\" ";

    auto     compilerPath = BackendHelpersWin32::visualStudioPath + R"(\bin\Hostx64\x64\)";
    uint32_t numErrors    = 0;

    auto cmdLineCL = "\"" + compilerPath + "link.exe\" " + linkArguments;
    SWAG_CHECK(OS::doProcess(cmdLineCL, compilerPath, true, numErrors, LogColor::DarkCyan, "CL "));

    return true;
}

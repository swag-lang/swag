#include "pch.h"
#include "BackendLLVM.h"
#include "Job.h"
#include "Module.h"
#include "Workspace.h"
#include "OS.h"

bool BackendLLVM::check()
{
    llvmModule = new llvm::Module(module->name.c_str(), llvmContext);
    return true;
}

JobResult BackendLLVM::preCompile(Job* ownerJob, int preCompileIndex)
{
    return JobResult::ReleaseJob;
}

static bool getVSTarget(string& vsTarget)
{
    vector<string> toTest;
    toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC)");
    toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC)");
    toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Tools\MSVC)");
    toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC)");
    for (auto& one : toTest)
    {
        if (fs::exists(one))
        {
            vsTarget = one;
            break;
        }
    }

    if (vsTarget.empty())
        return false;

    for (auto& p : fs::directory_iterator(vsTarget))
        vsTarget = p.path().string();

    return !vsTarget.empty();
}

static string getStringRegKey(HKEY hKey, const string& strValueName)
{
    string strValue;
    char   szBuffer[512];
    DWORD  dwBufferSize = sizeof(szBuffer);
    ULONG  nError;
    nError = RegQueryValueExA(hKey, strValueName.c_str(), 0, NULL, (LPBYTE) szBuffer, &dwBufferSize);
    if (nError == ERROR_SUCCESS)
        strValue = szBuffer;
    return strValue;
}

static bool getWinSdkFolder(string& libPath, string& libVersion)
{
    HKEY hKey;

    // Folder, try in register first
    LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0\\", 0, KEY_READ, &hKey);
    if (lRes == ERROR_SUCCESS)
    {
        libPath    = getStringRegKey(hKey, "InstallationFolder");
        libVersion = getStringRegKey(hKey, "ProductVersion");
        if (!libVersion.empty())
            libVersion += ".0";
        RegCloseKey(hKey);
    }

    // Folder, fallback to direct folder
    if (libPath.empty())
    {
        libPath = R"(C:\Program Files (x86)\Windows Kits\10)";
        if (!fs::exists(libPath))
            return false;
    }

    // Version, fallback to direct folder
    if (libVersion.empty())
    {
        auto libSub = libPath + "\\" + "lib";
        if (!fs::exists(libSub))
            return false;
        fs::path tmpPath;
        for (auto& p : fs::directory_iterator(libSub))
            tmpPath = p.path();
        libVersion = tmpPath.filename().string();
    }

    return !libPath.empty() && !libVersion.empty();
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

    string visualStudioPath;

    // Visual studio folder
    // For vcruntime & msvcrt (mandatory under windows, even with clang...)
    // For clang-cl, it seems that it can find the folder itself
    if (!getVSTarget(visualStudioPath))
    {
        g_Log.error("error: c backend: cannot locate visual studio folder");
        return false;
    }

    string winSdkPath, winSdkVersion;
    // Windows sdk folders and version
    if (!getWinSdkFolder(winSdkPath, winSdkVersion))
    {
        g_Log.error("error: c backend: cannot locate windows sdk folder");
        return false;
    }

    vector<Utf8> libPath;
    libPath.push_back(format(R"(%s\lib\x64)", visualStudioPath.c_str()));
    libPath.push_back(format(R"(%s\lib\%s\um\x64)", winSdkPath.c_str(), winSdkVersion.c_str()));
    libPath.push_back(format(R"(%s\lib\%s\ucrt\x64)", winSdkPath.c_str(), winSdkVersion.c_str()));
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

    auto compilerPath = visualStudioPath + R"(\bin\Hostx64\x64\)";
    uint32_t numErrors = 0;

    auto cmdLineCL = "\"" + compilerPath + "link.exe\" " + linkArguments;
    SWAG_CHECK(OS::doProcess(cmdLineCL, compilerPath, true, numErrors, LogColor::DarkCyan, "CL "));

    return true;
}

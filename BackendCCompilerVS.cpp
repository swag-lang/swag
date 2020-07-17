#include "pch.h"
#ifdef _WIN32
#include "Os.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Workspace.h"

mutex  lockCheck;
string visualStudioPath;
string winSdkPath;
string winSdkVersion;
string compilerExe;
string compilerPath;

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

bool BackendCCompilerVS::check()
{
    if (!g_CommandLine.backendOutput)
        return true;

    // Need to be done only once
    unique_lock lk(lockCheck);
    if (!visualStudioPath.empty())
        return true;

    // Visual studio folder
    // For vcruntime & msvcrt (mandatory under windows, even with clang...)
    // For clang-cl, it seems that it can find the folder itself
    if (!getVSTarget(visualStudioPath))
    {
        g_Log.error("error: c backend: cannot locate visual studio folder");
        return false;
    }

    // Windows sdk folders and version
    if (!getWinSdkFolder(winSdkPath, winSdkVersion))
    {
        g_Log.error("error: c backend: cannot locate windows sdk folder");
        return false;
    }

    // Compiler
    switch (g_CommandLine.backendType)
    {
    case BackendType::C_Vs:
        compilerExe  = "cl.exe";
        compilerPath = visualStudioPath + R"(\bin\Hostx64\x64\)";
        break;
    case BackendType::C_Clang:
        compilerExe  = "clang-cl.exe";
        compilerPath = "C:/Program Files/LLVM/bin/";
        break;
    }

    auto fullPath = compilerPath + compilerExe;
    if (!fs::exists(fullPath))
    {
        g_Log.error(format("error: c backend: cannot locate compiler '%s'", fullPath.c_str()));
        return false;
    }

    return true;
}

bool BackendCCompilerVS::compile(const BuildParameters& buildParameters)
{
    auto         module = backend->module;
    vector<Utf8> libPath;

    g_Log.verbose(format("VS compilerPath is '%s'\n", compilerPath.c_str()));
    g_Log.verbose(format("VS winSdkPath is '%s'\n", winSdkPath.c_str()));
    g_Log.verbose(format("VS winSdkVersion is '%s'\n", winSdkVersion.c_str()));

    // For vcruntime & msvcrt (mandatory under windows, even with clang...)
    libPath.push_back(format(R"(%s\lib\x64)", visualStudioPath.c_str()));

    // Windows sdk library paths
    libPath.push_back(format(R"(%s\lib\%s\um\x64)", winSdkPath.c_str(), winSdkVersion.c_str()));
    libPath.push_back(format(R"(%s\lib\%s\ucrt\x64)", winSdkPath.c_str(), winSdkVersion.c_str()));

    // Modules
    libPath.push_back(g_Workspace.targetPath.string());

    // Runtime
    libPath.push_back(g_CommandLine.exePath.parent_path().string());

    string destFile = g_Workspace.targetPath.string() + buildParameters.destFile;

    // CL arguments
    string clArguments = "";
    bool   debugMode   = buildParameters.target.backendDebugInformations || g_CommandLine.debug;
    if (debugMode)
    {
        fs::path pdbPath = destFile + buildParameters.postFix + ".pdb";
        clArguments += "/Fd\"" + pdbPath.string() + "\" ";
        clArguments += "/Zi ";
    }

    // Append a string related to the version
    string outputTypeName;
    switch (buildParameters.type)
    {
    case BackendOutputType::StaticLib:
        outputTypeName = ".lib";
        break;
    case BackendOutputType::DynamicLib:
        outputTypeName = ".dll";
        break;
    }

    clArguments += "/nologo ";
    clArguments += "/GS- ";
    clArguments += "/MD ";

    for (int i = 0; i < backend->numPreCompileBuffers; i++)
        clArguments += "/Tc\"" + backend->bufferCFiles[i].path + "\" ";

    auto cachePath = g_Workspace.cachePath.string();
    if (buildParameters.flags & BUILDPARAM_FOR_TEST)
        cachePath += ("/test/");
    clArguments += "/Fo\"" + cachePath + "\" ";

    int optimLevel = 0;
    if (g_CommandLine.optim)
        optimLevel = g_CommandLine.optim;
    else if (!g_CommandLine.debug)
        optimLevel = buildParameters.target.backendOptimizeLevel;
    switch (optimLevel)
    {
    case 0:
        break;
    case 1:
        clArguments += "/O1 ";
        break;
    default:
        clArguments += "/O2 ";
        break;
    }

    //clArguments += "/O2 ";

    if (buildParameters.flags & BUILDPARAM_FOR_TEST)
        clArguments += "/DSWAG_HAS_TEST ";

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;

    uint32_t numErrors  = 0;
    string   resultFile = getResultFile(buildParameters);
    Utf8     linkArguments;

    switch (buildParameters.type)
    {
    case BackendOutputType::StaticLib:
    {
        auto cmdLineCL = "\"" + compilerPath + compilerExe + "\" " + clArguments + " /c";
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));

        string libArguments;
        libArguments = "/NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (g_CommandLine.verboseBackendCommand)
            libArguments += "/VERBOSE ";
        libArguments += "/OUT:\"" + resultFile + "\" ";

        for (int i = 0; i < backend->numPreCompileBuffers; i++)
        {
            fs::path nameObj = backend->bufferCFiles[i].path;
            nameObj.replace_extension(".obj");
            libArguments += "\"" + nameObj.string() + "\" ";
        }

        auto cmdLineLIB = "\"" + compilerPath + "lib.exe\" " + libArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineLIB + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineLIB, compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));
        break;
    }

    case BackendOutputType::DynamicLib:
    case BackendOutputType::Binary:
    {
        // Registered #foreignlib
        // As this is defined by the user, the consider the library must exists
        for (auto fl : buildParameters.foreignLibs)
        {
            linkArguments += fl;
            linkArguments += ".lib ";
        }

        // Registered #import dependencies
        // As this is generated by #import, it's possible that the .lib does not exists, because it
        // has another name
        for (const auto& dep : module->moduleDependencies)
        {
            auto libName = dep.first + ".lib ";
            for (const auto& oneLibPath : libPath)
            {
                auto fullName = oneLibPath;
                fullName += "/";
                fullName += libName;

                if (fs::exists(fs::path(fullName.c_str())))
                {
                    linkArguments += libName;
                    break;
                }
            }
        }

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

        for (const auto& oneLibPath : libPath)
            linkArguments += "/LIBPATH:\"" + oneLibPath + "\" ";

        linkArguments += "/INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (debugMode)
            linkArguments += "/DEBUG ";

        if (buildParameters.type == BackendOutputType::DynamicLib)
        {
            linkArguments += "/DLL ";
            linkArguments += "/OUT:\"" + resultFile + "\" ";
            clArguments += "/DSWAG_IS_DYNAMICLIB ";
        }
        else
        {
            linkArguments += "/OUT:\"" + resultFile + "\" ";
            clArguments += "/DSWAG_IS_BINARY ";
        }

        auto cmdLineCL = "\"" + compilerPath + compilerExe + "\" " + clArguments + "/link " + linkArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));
        break;
    }
    }

    g_Workspace.numErrors += numErrors;
    backend->module->numErrors += numErrors;
    return true;
}

#endif
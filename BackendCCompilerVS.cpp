#include "pch.h"
#ifdef _WIN32
#include "Os.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Workspace.h"

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

bool BackendCCompilerVS::compile()
{
    auto module = backend->module;

    string         compilerExe, compilerPath;
    string         linkArguments;
    vector<string> libPath;

    // Get compiler folder
    compilerExe = "cl.exe";
    if (!getVSTarget(compilerPath))
    {
        backend->module->error("C compiler backend, cannot locate visual studio folder");
        return false;
    }

    libPath.push_back(format(R"(%s\lib\x64)", compilerPath.c_str()));
    compilerPath += R"(\bin\Hostx64\x64\)";

    // Windows sdk folders and version
    string winSdkPath, winSdkVersion;
    if (!getWinSdkFolder(winSdkPath, winSdkVersion))
    {
        backend->module->error("C compiler backend, cannot locate windows sdk folder");
        return false;
    }

    g_Log.verbose(format("VS compilerPath is '%s'\n", compilerPath.c_str()));
    g_Log.verbose(format("VS winSdkPath is '%s'\n", winSdkPath.c_str()));
    g_Log.verbose(format("VS winSdkVersion is '%s'\n", winSdkVersion.c_str()));

    // Library paths
    libPath.push_back(format(R"(%s\lib\%s\um\x64)", winSdkPath.c_str(), winSdkVersion.c_str()));
    libPath.push_back(format(R"(%s\lib\%s\ucrt\x64)", winSdkPath.c_str(), winSdkVersion.c_str()));
    libPath.push_back(g_Workspace.targetPath.string());

    string destFile = g_Workspace.targetPath.string() + buildParameters->destFile;

    // CL arguments
    string clArguments = "";
    bool   debugMode   = buildParameters->target.backendDebugInformations || g_CommandLine.debug;
    if (debugMode)
    {
        fs::path pdbPath = destFile + buildParameters->postFix + ".pdb";
        clArguments += "/Fd\"" + pdbPath.string() + "\" ";
        clArguments += "/Zi ";
    }

    // Append a string related to the version
    string outputTypeName;
    switch (buildParameters->type)
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

    //clArguments += "/MP "; // Multithread compile

    for (int i = 0; i < backend->numPreCompileBuffers; i++)
        clArguments += "/Tc\"" + backend->bufferCFiles[i].path + "\" ";
    clArguments += "/Fo\"" + g_Workspace.cachePath.string() + "\" ";

    if (!g_CommandLine.debug)
    {
        switch (buildParameters->target.backendOptimizeLevel)
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
    }

    if (buildParameters->flags & BUILDPARAM_FOR_TEST)
        clArguments += "/DSWAG_HAS_TEST ";

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;

    uint32_t numErrors  = 0;
    string   resultFile = getResultFile();
    switch (buildParameters->type)
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
    }
    break;

    case BackendOutputType::DynamicLib:
    case BackendOutputType::Binary:
    {
        // Registered #foreignlib
        for (auto fl : buildParameters->foreignLibs)
        {
            linkArguments += fl;
            linkArguments += ".lib ";
        }

        // Registered #import dependencies
        for (const auto& dep : module->moduleDependencies)
            linkArguments += dep.first + ".lib ";

        // This is mandatory under windows
        linkArguments += "kernel32.lib ";
        linkArguments += "user32.lib ";

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

        if (buildParameters->type == BackendOutputType::DynamicLib)
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
    }
    break;
    }

    g_Workspace.numErrors += numErrors;
    backend->module->numErrors += numErrors;
    return true;
}

#endif
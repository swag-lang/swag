#include "pch.h"
#ifdef _WIN32
#include "Os.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Workspace.h"

bool BackendCCompilerVS::getVSTarget(string& vsTarget)
{
    vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC)";
    if (!fs::exists(vsTarget))
    {
        vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC)";
        if (!fs::exists(vsTarget))
        {
            vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC)";
            if (!fs::exists(vsTarget))
            {
                vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Tools\MSVC)";
                if (!fs::exists(vsTarget))
                {
                    backend->module->error("can't find visual studio backend folder");
                    return false;
                }
            }
        }
    }

    for (auto& p : fs::directory_iterator(vsTarget))
        vsTarget = p.path().string();

    return true;
}

bool BackendCCompilerVS::getWinSdk(string& winSdk)
{
    string libPath = R"(C:\Program Files (x86)\Windows Kits\10\Lib)";
    if (!fs::exists(libPath))
    {
        backend->module->error("can't find windows sdk folder");
        return false;
    }

    fs::path tmpPath;
    for (auto& p : fs::directory_iterator(libPath))
        tmpPath = p.path();

    winSdk = tmpPath.filename().string();
    return true;
}

bool BackendCCompilerVS::compile()
{
    auto module = backend->module;

    string vsTarget;
    SWAG_CHECK(getVSTarget(vsTarget));
    string winSdk;
    SWAG_CHECK(getWinSdk(winSdk));

    string clPath = vsTarget + R"(\bin\Hostx64\x64\)";

    // Library paths
    vector<string> libPath;
    libPath.push_back(format(R"(%s\lib\x64)", vsTarget.c_str()));
    libPath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\lib\%s\um\x64)", winSdk.c_str()));
    libPath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\lib\%s\ucrt\x64)", winSdk.c_str()));
    libPath.push_back(g_Workspace.targetPath.string());

    // Include paths
    vector<string> includePaths;
    //includePath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\um)", winSdk.c_str()));
    //includePath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\shared)", winSdk.c_str()));
    includePaths.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\ucrt)", winSdk.c_str()));
    includePaths.push_back(format(R"(%s\include)", vsTarget.c_str()));
	includePaths.push_back(g_Workspace.targetPath.string());

    // CL arguments
    string clArguments = "";
    if (buildParameters->target.backendDebugInformations)
    {
        fs::path pdbPath = buildParameters->destFile + buildParameters->postFix + ".pdb";
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
    clArguments += "/EHsc ";
    clArguments += "/Tc\"" + backend->bufferC.fileName + "\" ";
    string nameObj = buildParameters->destFile + outputTypeName + buildParameters->postFix + ".obj";
    clArguments += "/Fo\"" + nameObj + "\" ";
    switch (buildParameters->target.backendOptimizeLevel)
    {
    case 0:
        break;
    case 1:
        clArguments += "/01 ";
        break;
    default:
        clArguments += "/O2 ";
        break;
    }

    for (const auto& oneIncludePath : includePaths)
        clArguments += "/I\"" + oneIncludePath + "\" ";
    if (buildParameters->flags & BUILDPARAM_FOR_TEST)
        clArguments += "/DSWAG_HAS_TEST ";

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;

    uint32_t numErrors = 0;
    string   resultFile;
    switch (buildParameters->type)
    {
    case BackendOutputType::StaticLib:
    {
        auto cmdLineCL = "\"" + clPath + "cl.exe\" " + clArguments + " /c";
        if (verbose)
            g_Log.verbose("VS: " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, clPath, verbose, numErrors, LogColor::DarkCyan, "VS: "));

        string libArguments;
        libArguments = "/NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (g_CommandLine.verboseBackendCommand)
            libArguments += "/VERBOSE ";
        resultFile = buildParameters->destFile + buildParameters->postFix + ".lib";
        libArguments += "/OUT:\"" + resultFile + "\" ";
        libArguments += "\"" + nameObj + "\" ";

        if (verbose)
            g_Log.verbose(format("VS: '%s' => '%s'", backend->bufferC.fileName.c_str(), resultFile.c_str()));

        auto cmdLineLIB = "\"" + clPath + "lib.exe\" " + libArguments;
        if (verbose)
            g_Log.verbose("VS: " + cmdLineLIB + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineLIB, clPath, verbose, numErrors, LogColor::DarkCyan, "VS: "));
    }
    break;

    case BackendOutputType::DynamicLib:
    case BackendOutputType::Binary:
    {
        string linkArguments;
        linkArguments += "legacy_stdio_definitions.lib ";
        for (auto depName : module->moduleDependenciesNames)
            linkArguments += depName + ".lib ";
        for (const auto& oneLibPath : libPath)
            linkArguments += "/LIBPATH:\"" + oneLibPath + "\" ";

        linkArguments += "/INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (buildParameters->target.backendDebugInformations)
            linkArguments += "/DEBUG ";

        if (buildParameters->type == BackendOutputType::DynamicLib)
        {
            linkArguments += "/DLL ";
            resultFile = buildParameters->destFile + buildParameters->postFix + ".dll";
            linkArguments += "/OUT:\"" + resultFile + "\" ";
            clArguments += "/DSWAG_IS_DYNAMICLIB ";
        }
        else
        {
            resultFile = buildParameters->destFile + buildParameters->postFix + ".exe";
            linkArguments += "/OUT:\"" + resultFile + "\" ";
            clArguments += "/DSWAG_IS_BINARY ";
        }

        if (verbose)
            g_Log.verbose(format("VS: '%s' => '%s'", backend->bufferC.fileName.c_str(), resultFile.c_str()));

        auto cmdLineCL = "\"" + clPath + "cl.exe\" " + clArguments + "/link " + linkArguments;
        if (verbose)
            g_Log.verbose("VS: " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, clPath, verbose, numErrors, LogColor::DarkCyan, "VS: "));
    }
    break;
    }

    g_Workspace.numErrors += numErrors;
    backend->module->numErrors += numErrors;
    return true;
}

#endif
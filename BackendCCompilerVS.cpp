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

string BackendCCompilerVS::getResultFile()
{
    string resultFile;
    switch (buildParameters->type)
    {
    case BackendOutputType::StaticLib:
        resultFile = buildParameters->destFile + buildParameters->postFix + ".lib";
        break;

    case BackendOutputType::DynamicLib:
        resultFile = buildParameters->destFile + buildParameters->postFix + ".dll";
        break;

    case BackendOutputType::Binary:
        resultFile = buildParameters->destFile + buildParameters->postFix + ".exe";
        break;
    }

    return resultFile;
}

bool BackendCCompilerVS::mustCompile()
{
    if (g_CommandLine.rebuild)
        return true;

    if (!fs::exists(backend->bufferC.fileName))
        return true;
    auto resultFile = getResultFile();
    if (!fs::exists(resultFile))
        return true;

    fs::file_time_type mtime1 = fs::last_write_time(resultFile);
    time_t             t1     = fs::file_time_type::clock::to_time_t(mtime1);
    fs::file_time_type mtime2 = fs::last_write_time(backend->bufferC.fileName);
    time_t             t2     = fs::file_time_type::clock::to_time_t(mtime2);
    if (t1 >= t2)
        return false;

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

    if (buildParameters->flags & BUILDPARAM_FOR_TEST)
        clArguments += "/DSWAG_HAS_TEST ";

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;

    uint32_t numErrors  = 0;
    string   resultFile = getResultFile();
    switch (buildParameters->type)
    {
    case BackendOutputType::StaticLib:
    {
        auto cmdLineCL = "\"" + clPath + "cl.exe\" " + clArguments + " /c";
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, clPath, verbose, numErrors, LogColor::DarkCyan, "VS "));

        string libArguments;
        libArguments = "/NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (g_CommandLine.verboseBackendCommand)
            libArguments += "/VERBOSE ";
        libArguments += "/OUT:\"" + resultFile + "\" ";
        libArguments += "\"" + nameObj + "\" ";

        if (verbose)
            g_Log.verbose(format("VS '%s' => '%s'", backend->bufferC.fileName.c_str(), resultFile.c_str()));

        auto cmdLineLIB = "\"" + clPath + "lib.exe\" " + libArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineLIB + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineLIB, clPath, verbose, numErrors, LogColor::DarkCyan, "VS "));
    }
    break;

    case BackendOutputType::DynamicLib:
    case BackendOutputType::Binary:
    {
        string linkArguments;
        linkArguments += "ucrt.lib ";
        linkArguments += "/NODEFAULTLIB:libucrt.lib ";
		linkArguments += "libvcruntime.lib ";

        for (const auto& dep : module->moduleDependencies)
            linkArguments += dep.first + ".lib ";
        for (const auto& oneLibPath : libPath)
            linkArguments += "/LIBPATH:\"" + oneLibPath + "\" ";

        linkArguments += "/INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (buildParameters->target.backendDebugInformations)
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

        if (verbose)
            g_Log.verbose(format("VS '%s' => '%s'", backend->bufferC.fileName.c_str(), resultFile.c_str()));

        auto cmdLineCL = "\"" + clPath + "cl.exe\" " + clArguments + "/link " + linkArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, clPath, verbose, numErrors, LogColor::DarkCyan, "VS "));
    }
    break;
    }

    g_Workspace.numErrors += numErrors;
    backend->module->numErrors += numErrors;
    return true;
}

#endif
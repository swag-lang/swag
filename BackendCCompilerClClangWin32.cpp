#include "pch.h"
#ifdef _WIN32
#include "Os.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerClClangWin32.h"
#include "BackendLinkerWin32.h"
#include "BackendSetupWin32.h"
#include "Workspace.h"

bool BackendCCompilerClClangWin32::compile(const BuildParameters& buildParameters)
{
    auto module = backend->module;

    // CL arguments
    string clArguments = "";
    bool   debugMode   = buildParameters.target.backendDebugInformations || g_CommandLine.debug;
    if (debugMode)
    {
        fs::path dest = Backend::getOutputFileName(buildParameters);
        dest.replace_extension(".pdb");
        clArguments += "/Fd\"" + dest.string() + "\" ";
        clArguments += "/Zi ";
    }

    clArguments += "/nologo ";
    clArguments += "/GS- ";
    clArguments += "/MD ";

    for (int i = 0; i < backend->numPreCompileBuffers; i++)
        clArguments += "/Tc\"" + backend->bufferCFiles[i].path + "\" ";

    auto cachePath = Backend::getCacheFolder(buildParameters);
    clArguments += "/Fo\"" + cachePath + "\" ";

    // Optimization level
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

    if (buildParameters.compileType == BackendCompileType::Test)
        clArguments += "/DSWAG_HAS_TEST ";

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;

    uint32_t numErrors = 0;
    switch (buildParameters.outputType)
    {
    case BackendOutputType::StaticLib:
    {
        auto cmdLineCL = "\"" + Backend::compilerPath + Backend::compilerExe + "\" " + clArguments + " /c";
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, Backend::compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));

        string libArguments;
        libArguments = "/NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (g_CommandLine.verboseBackendCommand)
            libArguments += "/VERBOSE ";

        string resultFile = Backend::getOutputFileName(buildParameters);
        libArguments += "/OUT:\"" + resultFile + "\" ";

        for (int i = 0; i < backend->numPreCompileBuffers; i++)
        {
            fs::path nameObj = backend->bufferCFiles[i].path;
            nameObj.replace_extension(".obj");
            libArguments += "\"" + nameObj.string() + "\" ";
        }

        auto cmdLineLIB = "\"" + Backend::compilerPath + "lib.exe\" " + libArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineLIB + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineLIB, Backend::compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));
        break;
    }

    case BackendOutputType::DynamicLib:
    case BackendOutputType::Binary:
    {
        Utf8 linkArguments;
        BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

        if (buildParameters.outputType == BackendOutputType::DynamicLib)
            clArguments += "/DSWAG_IS_DYNAMICLIB ";
        else
            clArguments += "/DSWAG_IS_BINARY ";

        auto cmdLineCL = "\"" + Backend::compilerPath + Backend::compilerExe + "\" " + clArguments + "/link " + linkArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, Backend::compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));
        break;
    }
    }

    g_Workspace.numErrors += numErrors;
    backend->module->numErrors += numErrors;
    return true;
}

#endif
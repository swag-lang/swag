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

    clArguments += "/nologo ";
    clArguments += "/GS- ";
    clArguments += "/MD ";

    for (int i = 0; i < backend->numPreCompileBuffers; i++)
        clArguments += "/Tc\"" + backend->bufferCFiles[i].path + "\" ";

    auto cachePath = BackendLinkerWin32::getCacheFolder(buildParameters);
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
    Utf8     linkArguments;

    switch (buildParameters.outputType)
    {
    case BackendOutputType::StaticLib:
    {
        auto cmdLineCL = "\"" + BackendSetupWin32::compilerPath + BackendSetupWin32::compilerExe + "\" " + clArguments + " /c";
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, BackendSetupWin32::compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));

        string libArguments;
        libArguments = "/NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (g_CommandLine.verboseBackendCommand)
            libArguments += "/VERBOSE ";

        string resultFile = BackendLinkerWin32::getResultFile(buildParameters);
        libArguments += "/OUT:\"" + resultFile + "\" ";

        for (int i = 0; i < backend->numPreCompileBuffers; i++)
        {
            fs::path nameObj = backend->bufferCFiles[i].path;
            nameObj.replace_extension(".obj");
            libArguments += "\"" + nameObj.string() + "\" ";
        }

        auto cmdLineLIB = "\"" + BackendSetupWin32::compilerPath + "lib.exe\" " + libArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineLIB + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineLIB, BackendSetupWin32::compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));
        break;
    }

    case BackendOutputType::DynamicLib:
    case BackendOutputType::Binary:
    {
        BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

        if (buildParameters.outputType == BackendOutputType::DynamicLib)
            clArguments += "/DSWAG_IS_DYNAMICLIB ";
        else
            clArguments += "/DSWAG_IS_BINARY ";

        auto cmdLineCL = "\"" + BackendSetupWin32::compilerPath + BackendSetupWin32::compilerExe + "\" " + clArguments + "/link " + linkArguments;
        if (verbose)
            g_Log.verbose("VS " + cmdLineCL + "\n");
        SWAG_CHECK(OS::doProcess(cmdLineCL, BackendSetupWin32::compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));
        break;
    }
    }

    g_Workspace.numErrors += numErrors;
    backend->module->numErrors += numErrors;
    return true;
}

#endif
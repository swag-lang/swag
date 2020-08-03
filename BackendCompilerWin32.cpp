#include "pch.h"
#ifdef _WIN32
#include "Os.h"
#include "BackendC.h"
#include "BackendLinkerWin32.h"
#include "BackendSetupWin32.h"
#include "Workspace.h"

namespace OS
{
    bool compile(const BuildParameters& buildParameters, Module* module, const vector<string>& cFiles)
    {
        // CL arguments
        string clArguments = "";
        bool   debugMode   = buildParameters.buildCfg->backendDebugInformations;
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

        for (const auto& file : cFiles)
            clArguments += "/Tc\"" + file + "\" ";

        auto cachePath = Backend::getCacheFolder(buildParameters);
        clArguments += "/Fo\"" + cachePath + "\" ";

        // Optimization level
        if (buildParameters.buildCfg->backendOptimizeSize)
            clArguments += "/O1 ";
        else if (buildParameters.buildCfg->backendOptimizeSpeed)
            clArguments += "/O2 ";
        else
            clArguments += "/Od ";

        if (buildParameters.compileType == BackendCompileType::Test)
            clArguments += "/DSWAG_HAS_TEST ";

        bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;

        uint32_t numErrors = 0;
        switch (buildParameters.outputType)
        {
        case BackendOutputType::StaticLib:
        {
            // Generate object files
            auto cmdLineCL = "\"" + Backend::compilerPath + Backend::compilerExe + "\" " + clArguments + " /c";
            if (verbose)
                g_Log.verbose("VS " + cmdLineCL + "\n");
            SWAG_CHECK(OS::doProcess(cmdLineCL, Backend::compilerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));

            // Generate the library
            string libArguments;
            libArguments = "/NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
            if (g_CommandLine.verboseBackendCommand)
                libArguments += "/VERBOSE ";

            string resultFile = Backend::getOutputFileName(buildParameters);
            libArguments += "/OUT:\"" + resultFile + "\" ";

            for (const auto& file : cFiles)
            {
                fs::path nameObj = file;
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
        module->numErrors += numErrors;
        return true;
    }
} // namespace OS

#endif
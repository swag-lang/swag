#include "pch.h"
#ifdef _WIN32
#include "Workspace.h"
#include "Backend.h"
#include "OS.h"
#include "BackendSetupWin32.h"
#include "Module.h"

namespace BackendLinkerWin32
{
#if defined(_M_ARM) || defined(__arm_)
    static const char* target = "arm";
#endif
#if defined(_M_IX86) || defined(__i386__)
    static const char* target = "x86";
#endif
#if defined(_M_X64) || defined(__x86_64__)
    static const char* target = "x64";
#endif

    void getLibPaths(vector<Utf8>& libPath)
    {
        // Windows sdk library paths
        libPath.push_back(format(R"(%s\lib\%s\um\%s)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str(), target));
        libPath.push_back(format(R"(%s\lib\%s\ucrt\%s)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str(), target));

        // Modules
        libPath.push_back(g_Workspace.targetPath.string());

        // Runtime
        libPath.push_back(g_CommandLine.exePath.parent_path().string());
    }

    void getArguments(const BuildParameters& buildParameters, Module* module, Utf8& arguments)
    {
        vector<Utf8> libPath;
        getLibPaths(libPath);

        // Registered #foreignlib
        // As this is defined by the user, the consider the library must exists
        for (auto fl : buildParameters.foreignLibs)
        {
            arguments += fl;
            arguments += ".lib ";
        }

        // Registered #import dependencies
        for (const auto& dep : module->moduleDependencies)
        {
            auto libName  = dep.first + ".lib ";
            auto fullName = g_Workspace.targetPath.string(); // oneLibPath;
            fullName += "/";
            fullName += libName;

            // Be sure that the library exits. Some modules rely on external libraries, and do not have their
            // own one
            if (fs::exists(fs::path(fullName.c_str())))
                arguments += libName;
        }

        // External libraries from windows sdk
        arguments += "kernel32.lib ";
        arguments += "ucrt.lib ";

        //if (g_CommandLine.devMode)
        //arguments += "user32.lib "; // MessageBox

        // Add swag.runtime
        if (buildParameters.buildCfg->backendDebugInformations)
            arguments += "swag.runtime_d.lib ";
        else
            arguments += "swag.runtime.lib ";

        for (const auto& oneLibPath : libPath)
            arguments += "/LIBPATH:\"" + oneLibPath + "\" ";

        arguments += "/INCREMENTAL:NO ";
        arguments += "/NOLOGO ";
        arguments += "/SUBSYSTEM:CONSOLE ";
        arguments += "/NODEFAULTLIB ";
        arguments += format("/MACHINE:%s ", target);

        if (buildParameters.buildCfg->backendDebugInformations)
            arguments += "/DEBUG ";

        auto resultFile = Backend::getOutputFileName(buildParameters);
        if (buildParameters.outputType == BackendOutputType::DynamicLib)
        {
            arguments += "/DLL ";
            arguments += "/OUT:\"" + resultFile + "\" ";
        }
        else
        {
            arguments += "/OUT:\"" + resultFile + "\" ";
        }

        //printf(arguments.c_str());
        //printf("\n");
    }
} // namespace BackendLinkerWin32

namespace OS
{
    bool link(const BuildParameters& buildParameters, Module* module, vector<string>& objectFiles)
    {
        SWAG_PROFILE(0, format("link %s%s", module->name.c_str(), buildParameters.postFix.c_str()));

        Utf8 linkArguments;
        BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

        // Add all object files
        auto targetPath = Backend::getCacheFolder(buildParameters);
        for (auto& file : objectFiles)
        {
            auto path = targetPath + "/" + file.c_str();
            linkArguments += path + " ";
        }

        bool     verbose   = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;
        uint32_t numErrors = 0;
        auto     cmdLine   = "\"" + Backend::linkerPath + Backend::linkerExe + "\" " + linkArguments;
        auto     result    = OS::doProcess(cmdLine, Backend::linkerPath, verbose, numErrors, LogColor::DarkCyan, "CL ");

        g_Workspace.numErrors += numErrors;
        module->numErrors += numErrors;
        return result;
    }

} // namespace OS

#endif
#include "pch.h"
#ifdef _WIN32
#include "Workspace.h"
#include "Backend.h"
#include "OS.h"
#include "BackendSetupWin32.h"
#include "Module.h"
#include "Profile.h"
#include "lld/Common/Driver.h"

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
        libPath.push_back(format(R"(%slib\%s\um\%s)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str(), target));
        libPath.push_back(format(R"(%slib\%s\ucrt\%s)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str(), target));

        // Modules
        libPath.push_back(g_Workspace.targetPath.string());

        // Runtime
        libPath.push_back(g_CommandLine.exePath.parent_path().string());
    }

    void getArguments(const BuildParameters& buildParameters, Module* module, vector<Utf8>& arguments, bool addQuote)
    {
        vector<Utf8> libPath;
        getLibPaths(libPath);

        // Registered #foreignlib
        // As this is defined by the user, the consider the library must exists
        for (auto fl : buildParameters.foreignLibs)
        {
            Utf8 one = fl + ".lib";
            arguments.push_back(one);
        }

        // Registered #import dependencies
        for (const auto& dep : module->moduleDependencies)
        {
            auto libName  = dep->name + ".lib";
            auto fullName = g_Workspace.targetPath.string(); // oneLibPath;
            fullName += "/";
            fullName += libName;

            // Be sure that the library exits. Some modules rely on external libraries, and do not have their
            // own one
            if (fs::exists(fs::path(fullName.c_str())))
                arguments.push_back(libName);
        }

        // External libraries from windows sdk
        arguments.push_back("kernel32.lib");
        arguments.push_back("ucrt.lib");
        arguments.push_back("user32.lib");

        for (const auto& oneLibPath : libPath)
        {
            if (addQuote)
                arguments.push_back("/LIBPATH:\"" + oneLibPath + "\"");
            else
                arguments.push_back("/LIBPATH:" + oneLibPath);
        }

        arguments.push_back("/INCREMENTAL:NO");
        arguments.push_back("/NOLOGO");
        arguments.push_back("/SUBSYSTEM:CONSOLE");
        arguments.push_back("/NODEFAULTLIB");
        arguments.push_back(format("/MACHINE:%s", target));
        arguments.push_back(format("/STACK:%d", g_CommandLine.stackSize));

        if (buildParameters.buildCfg->backendDebugInformations)
            arguments.push_back("/DEBUG");

        auto resultFile = Backend::getOutputFileName(buildParameters);
        if (buildParameters.outputType == BackendOutputType::DynamicLib)
            arguments.push_back("/DLL");
        if (addQuote)
            arguments.push_back("/OUT:\"" + resultFile + "\"");
        else
            arguments.push_back("/OUT:" + resultFile);

        //printf(arguments.c_str());
        //printf("\n");
    }
} // namespace BackendLinkerWin32

namespace OS
{
    class MyOStream : public llvm::raw_ostream
    {
    public:
        MyOStream()
            : raw_ostream(true)
            , pos(0)
        {
        }
        void write_impl(const char* ptr, size_t len) override
        {
            g_Log.print(ptr);
            pos += len;
        }

        uint64_t current_pos() const override
        {
            return pos;
        }

        size_t pos;
    };

    bool link_static(const BuildParameters& buildParameters, Module* module, vector<string>& objectFiles)
    {
        SWAG_PROFILE(PRF_LINK, format("link %s", module->name.c_str()));

        vector<Utf8> linkArguments;
        BackendLinkerWin32::getArguments(buildParameters, module, linkArguments, false);

        // Add all object files
        auto targetPath = Backend::getCacheFolder(buildParameters);
        for (auto& file : objectFiles)
        {
            auto path = targetPath + "/" + file.c_str();
            linkArguments.push_back(path);
        }

        VectorNative<const char*> linkArgumentsPtr;
        linkArgumentsPtr.push_back("lld");
        for (auto& one : linkArguments)
            linkArgumentsPtr.push_back(one.c_str());

        llvm::ArrayRef<const char*> array_ref_args(linkArgumentsPtr.buffer, linkArgumentsPtr.size());

        MyOStream diag_stdout;
        MyOStream diag_stderr;

        static mutex oo;
        unique_lock  lk(oo);
        auto         result = lld::coff::link(array_ref_args, false, diag_stdout, diag_stderr);

        if (!result && g_CommandLine.devMode)
            OS::errorBox("[Developer Mode]", "Error raised !");

        if (!result)
        {
            g_Workspace.numErrors += 1;
            module->numErrors += 1;
        }

        return result;
    }

    bool link_process(const BuildParameters& buildParameters, Module* module, vector<string>& objectFiles)
    {
        SWAG_PROFILE(PRF_LINK, format("link %s", module->name.c_str()));

        vector<Utf8> linkArgumentsList;
        BackendLinkerWin32::getArguments(buildParameters, module, linkArgumentsList, true);
        Utf8 linkArguments;
        for (auto& one : linkArgumentsList)
            linkArguments += one + " ";

        // Add all object files
        auto targetPath = Backend::getCacheFolder(buildParameters);
        for (auto& file : objectFiles)
        {
            SWAG_ASSERT(!file.empty());
            auto path = targetPath + "/" + file.c_str();
            linkArguments += path + " ";
        }

        bool     verbose   = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;
        uint32_t numErrors = 0;
        auto     cmdLine   = "\"" + Backend::linkerPath + Backend::linkerExe + "\" " + linkArguments;

        if (g_CommandLine.verbose && g_CommandLine.verboseLink)
            g_Log.verbose(cmdLine);

        auto result = OS::doProcess(cmdLine, Backend::linkerPath, verbose, numErrors, LogColor::DarkCyan, "CL ");

        if (!result && g_CommandLine.devMode)
            OS::errorBox("[Developer Mode]", "Error raised !");

        g_Workspace.numErrors += numErrors;
        module->numErrors += numErrors;
        return result;
    }

    bool link(const BuildParameters& buildParameters, Module* module, vector<string>& objectFiles)
    {
        if (g_CommandLine.linkStatic)
            return link_static(buildParameters, module, objectFiles);
        return link_process(buildParameters, module, objectFiles);
    }

} // namespace OS

#endif
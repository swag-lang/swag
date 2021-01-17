#include "pch.h"
#include "Workspace.h"
#include "Backend.h"
#include "OS.h"
#include "BackendSetupWin32.h"
#include "Module.h"
#include "Profile.h"
#include "lld/Common/Driver.h"

namespace BackendLinker
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
            if (startLine)
            {
                errMsg.clear();
                startLine = false;
            }

            errMsg += ptr;

            if (strstr(ptr, "\n"))
            {
                errMsg.replaceAll('\n', ' ');
                module->error(errMsg);
                startLine = true;
            }

            pos += len;
        }

        uint64_t current_pos() const override
        {
            return pos;
        }

        Utf8    errMsg;
        size_t  pos;
        Module* module    = nullptr;
        bool    startLine = true;
    };

    void getArgumentsCoff(const BuildParameters& buildParameters, Module* module, vector<Utf8>& arguments)
    {
        vector<Utf8> libPath;

        // Windows sdk library paths
        const char* target = isArchArm(g_CommandLine.arch) ? "arm64" : "x64";
        libPath.push_back(format(R"(%slib\%s\um\%s)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str(), target));
        libPath.push_back(format(R"(%slib\%s\ucrt\%s)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str(), target));
        arguments.push_back("kernel32.lib");
        arguments.push_back("ucrt.lib");
        arguments.push_back("user32.lib");

        // Modules
        libPath.push_back(g_Workspace.targetPath.string());

        // Runtime
        libPath.push_back(g_CommandLine.exePath.parent_path().string());

        for (const auto& oneLibPath : libPath)
        {
            auto normalizedLibPath = normalizePath(fs::path(oneLibPath.c_str()));
            arguments.push_back("/LIBPATH:" + normalizedLibPath);
        }

        // Registered #global foreignlib
        // As this is defined by the user, we consider the library must exists
        for (auto fl : buildParameters.foreignLibs)
        {
            Utf8 one = fl + ".lib";
            arguments.push_back(one);
        }

        // Registered #import dependencies
        for (const auto& dep : module->moduleDependencies)
        {
            auto libName  = dep->name + ".lib";
            auto fullName = g_Workspace.targetPath.string();
            fullName      = normalizePath(fs::path(fullName.c_str()));
            fullName += "/";
            fullName += libName;

            // Be sure that the library exits. Some modules rely on external libraries, and do not have their
            // own one
            if (fs::exists(fs::path(fullName.c_str())))
                arguments.push_back(libName);
        }

        arguments.push_back("/INCREMENTAL:NO");
        arguments.push_back("/NOLOGO");
        arguments.push_back("/SUBSYSTEM:CONSOLE");
        arguments.push_back("/NODEFAULTLIB");

        if (isArchArm(g_CommandLine.arch))
            arguments.push_back("/MACHINE:ARM64");
        else
            arguments.push_back("/MACHINE:X64");

        if (buildParameters.outputType == BackendOutputType::Binary)
            arguments.push_back(format("/STACK:%d", g_CommandLine.stackSize));
        if (buildParameters.buildCfg->backendDebugInformations)
            arguments.push_back("/DEBUG");
        if (buildParameters.outputType == BackendOutputType::DynamicLib)
            arguments.push_back("/DLL");

        auto resultFile = Backend::getOutputFileName(buildParameters);
        arguments.push_back("/OUT:" + resultFile);
    }

    void getArguments(const BuildParameters& buildParameters, Module* module, vector<Utf8>& arguments)
    {
        auto objFileType = Backend::getObjType(g_CommandLine.os);
        switch (objFileType)
        {
        case BackendObjType::Coff:
            getArgumentsCoff(buildParameters, module, arguments);
            break;
        case BackendObjType::Elf:
            SWAG_ASSERT(false);
            break;
        case BackendObjType::MachO:
            SWAG_ASSERT(false);
            break;
        case BackendObjType::Wasm:
            SWAG_ASSERT(false);
            break;
        }
    }

    bool link(const BuildParameters& buildParameters, Module* module, vector<string>& objectFiles)
    {
        SWAG_PROFILE(PRF_LINK, format("link %s", module->name.c_str()));

        vector<Utf8> linkArguments;
        getArguments(buildParameters, module, linkArguments);

        // Add all object files
        auto targetPath = Backend::getCacheFolder(buildParameters);
        for (auto& file : objectFiles)
        {
            auto path              = targetPath + "/" + file.c_str();
            auto normalizedLibPath = normalizePath(fs::path(path.c_str()));
            linkArguments.push_back(normalizedLibPath);
        }

        VectorNative<const char*> linkArgumentsPtr;
        linkArgumentsPtr.push_back("lld");
        for (auto& one : linkArguments)
            linkArgumentsPtr.push_back(one.c_str());

        // Log linker parameters
        if (g_CommandLine.verboseLink)
        {
            Utf8 linkStr;
            for (auto& one : linkArguments)
            {
                linkStr += one;
                linkStr += " ";
            }

            g_Log.verbose(linkStr);
        }

        llvm::ArrayRef<const char*> array_ref_args(linkArgumentsPtr.buffer, linkArgumentsPtr.size());

        MyOStream diag_stdout;
        MyOStream diag_stderr;
        diag_stdout.module = module;
        diag_stderr.module = module;

        static mutex oo;
        unique_lock  lk(oo);

        auto objFileType = Backend::getObjType(g_CommandLine.os);
        bool result      = true;
        switch (objFileType)
        {
        case BackendObjType::Coff:
            result = lld::coff::link(array_ref_args, false, diag_stdout, diag_stderr);
            break;
        case BackendObjType::Elf:
            result = lld::elf::link(array_ref_args, false, diag_stdout, diag_stderr);
            break;
        case BackendObjType::MachO:
            result = lld::mach_o::link(array_ref_args, false, diag_stdout, diag_stderr);
            break;
        case BackendObjType::Wasm:
            result = lld::wasm::link(array_ref_args, false, diag_stdout, diag_stderr);
            break;
        }

        if (!result && g_CommandLine.devMode)
            OS::errorBox("[Developer Mode]", "Error raised !");

        if (!result)
        {
            g_Workspace.numErrors += 1;
            module->numErrors += 1;
        }

        return result;
    }

} // namespace BackendLinker
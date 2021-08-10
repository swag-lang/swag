#include "pch.h"
#include "Workspace.h"
#include "Backend.h"
#include "OS.h"
#include "Module.h"
#include "ThreadManager.h"

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
                vector<Utf8> subNames;
                Utf8::tokenize(errMsg.c_str(), '\n', subNames);

                g_Log.lock();
                for (auto& l : subNames)
                {
                    if (strstr(l.c_str(), ": error"))
                    {
                        errCount++;
                        g_Log.setColor(LogColor::Red);
                    }
                    else
                        g_Log.setColor(LogColor::Gray);
                    l += "\n";
                    g_Log.print(l);
                }

                g_Log.setDefaultColor();
                g_Log.unlock();
                startLine = true;
            }

            pos += len;
        }

        uint64_t current_pos() const override
        {
            return pos;
        }

        Utf8     errMsg;
        size_t   pos;
        uint32_t errCount  = 0;
        Module*  module    = nullptr;
        bool     startLine = true;
    };

    void getArgumentsCoff(const BuildParameters& buildParameters, Module* module, vector<Utf8>& arguments, bool forCmdLine)
    {
        vector<Utf8> libPaths;

        // System library paths
        for (auto p : g_CommandLine.libPaths)
            libPaths.push_back(p);

        // Modules
        libPaths.push_back(g_Workspace.targetPath.string());

        // Runtime
        libPaths.push_back(g_CommandLine.exePath.parent_path().string());

        for (const auto& oneLibPath : libPaths)
        {
            auto normalizedLibPath = Utf8::normalizePath(fs::path(oneLibPath.c_str()));
            if (forCmdLine)
                arguments.push_back("/LIBPATH:\"" + normalizedLibPath + "\"");
            else
                arguments.push_back("/LIBPATH:" + normalizedLibPath);
        }

        // Registered #global foreignlib
        // As this is defined by the user, we consider the library must exists
        for (auto fl : buildParameters.foreignLibs)
        {
            Utf8 one = fl + Backend::getOutputFileExtension(BuildCfgBackendKind::StaticLib);
            arguments.push_back(one);
        }

        // Registered #import dependencies
        for (const auto& dep : module->moduleDependencies)
        {
            auto libName  = dep->name + Backend::getOutputFileExtension(BuildCfgBackendKind::StaticLib);
            auto fullName = g_Workspace.targetPath.string();
            fullName      = Utf8::normalizePath(fs::path(fullName.c_str()));
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
        arguments.push_back("/ERRORLIMIT:0");

        if (isArchArm(g_CommandLine.arch))
            arguments.push_back("/MACHINE:ARM64");
        else
            arguments.push_back("/MACHINE:X64");

        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Executable)
            arguments.push_back(Utf8::format("/STACK:%d,%d", g_CommandLine.stackSizeRT, g_CommandLine.stackSizeRT));

        if (buildParameters.buildCfg->backendDebugInformations)
            arguments.push_back("/DEBUG");
        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
            arguments.push_back("/DLL");

        auto resultFile = Backend::getOutputFileName(buildParameters);
        arguments.push_back("/OUT:" + resultFile);
    }

    void getArguments(const BuildParameters& buildParameters, Module* module, vector<Utf8>& arguments, bool forCmdLine)
    {
        auto objFileType = Backend::getObjType(g_CommandLine.os);
        switch (objFileType)
        {
        case BackendObjType::Coff:
            getArgumentsCoff(buildParameters, module, arguments, forCmdLine);
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
        vector<Utf8> linkArguments;
        getArguments(buildParameters, module, linkArguments, false);

        // Add all object files
        auto targetPath = Backend::getCacheFolder(buildParameters);
        for (auto& file : objectFiles)
        {
            auto path              = targetPath + "/" + file.c_str();
            auto normalizedLibPath = Utf8::normalizePath(fs::path(path.c_str()));
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

        // It's not possible to launch lld linker in parallel (sight), because it's not thread safe.
        static mutex oo;
        while (!oo.try_lock())
        {
            g_ThreadMgr.tryExecuteJob();
            this_thread::yield();
        }

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

#ifdef SWAG_DEV_MODE
        if (!result)
            OS::errorBox("[Developer Mode]", "Error raised !");
#endif

        if (!result)
        {
            g_Workspace.numErrors += diag_stderr.errCount;
            module->numErrors += diag_stderr.errCount;
        }

        oo.unlock();
        return result;
    }

} // namespace BackendLinker
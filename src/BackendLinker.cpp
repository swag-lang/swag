#include "pch.h"
#include "Backend.h"
#include "Log.h"
#include "Module.h"
#include "Os.h"
#include "ThreadManager.h"
#include "Workspace.h"

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

            const Utf8 str{ptr, (uint32_t) len};
            errMsg += str;

            if (strstr(ptr, "\n"))
            {
                Vector<Utf8> subNames;
                Utf8::tokenize(errMsg, '\n', subNames);

                g_Log.lock();
                for (auto& l : subNames)
                {
                    const auto pze = strstr(l.c_str(), ": error:");
                    const auto pzw = strstr(l.c_str(), ": warning:");
                    if (pze || pzw)
                    {
                        errCount++;
                        g_Log.setColor(pze ? LogColor::Red : LogColor::Magenta);
                        if (module)
                        {
                            if (pze)
                            {
                                Utf8 l2 = pze + 8;
                                l2.trim();
                                const auto l1 = FMT("module [[%s]]: linker error: %s", module->name.c_str(), l2.c_str());
                                l             = l1;
                            }
                            else
                            {
                                Utf8 l2 = pzw + 10;
                                l2.trim();
                                const auto l1 = FMT("module [[%s]]: linker warning: %s", module->name.c_str(), l2.c_str());
                                l             = l1;
                            }
                        }

                        l += "\n";
                        g_Log.print(l);

#ifdef SWAG_DEV_MODE
                        if (pze)
                            OS::errorBox("[Developer Mode]", "Error raised !");
#endif
                    }
                    else
                    {
                        g_Log.setColor(LogColor::Gray);
                        l += "\n";
                        g_Log.print(l);
                    }
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

    void getArgumentsCoff(const BuildParameters& buildParameters, Module* module, Vector<Utf8>& arguments, bool forCmdLine)
    {
        Vector<Path> libPaths;

        // User defined library paths
        for (auto p : g_CommandLine.libPaths)
            libPaths.push_back(p);

        // Modules
        libPaths.push_back(g_Workspace->targetPath);

        // Runtime
        libPaths.push_back(g_CommandLine.exePath.parent_path());

        for (const auto& oneLibPath : libPaths)
        {
            auto normalizedLibPath = oneLibPath;
            if (forCmdLine)
                arguments.push_back("/LIBPATH:\"" + normalizedLibPath.string() + "\"");
            else
                arguments.push_back("/LIBPATH:" + normalizedLibPath.string());
        }

        // Register #foreignlib
        // As this is defined by the user, we consider the library must exists
        for (const auto& fl : buildParameters.foreignLibs)
        {
            Utf8 one = fl;
            if (Utf8::getExtension(one) != Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::StaticLib))
                one += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::StaticLib);
            arguments.push_back(one);
        }

        // Registered #import dependencies
        for (const auto& dep : module->moduleDependencies)
        {
            Utf8 libName = dep->name;
            if (Utf8::getExtension(libName) != Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::StaticLib))
                libName += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::StaticLib);
            auto fullName = g_Workspace->targetPath;
            fullName.append(libName.c_str());

            // Be sure that the library exists. Some modules rely on external libraries, and do not have their
            // own one
            error_code err;
            if (filesystem::exists(fullName, err))
                arguments.push_back(libName);
        }

        for (const auto& dep : module->moduleEmbedded)
        {
            if (dep->buildCfg.backendKind != BuildCfgBackendKind::Export)
                continue;

            auto libName = dep->name;
            if (Utf8::getExtension(libName) != Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::StaticLib))
                libName += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::StaticLib);
            auto fullName = g_Workspace->targetPath;
            fullName.append(libName.c_str());

            // Be sure that the library exits. Some modules rely on external libraries, and do not have their
            // own one
            error_code err;
            if (filesystem::exists(fullName, err))
                arguments.push_back(libName);
        }

        arguments.push_back("/INCREMENTAL:NO");
        arguments.push_back("/NOLOGO");
        if (buildParameters.buildCfg->backendSubKind == BuildCfgBackendSubKind::Console)
            arguments.push_back("/SUBSYSTEM:CONSOLE");
        else
            arguments.push_back("/SUBSYSTEM:WINDOWS");
        arguments.push_back("/NODEFAULTLIB");
        arguments.push_back("/ERRORLIMIT:0");

        if (isArchArm(g_CommandLine.target.arch))
            arguments.push_back("/MACHINE:ARM64");
        else
            arguments.push_back("/MACHINE:X64");

        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::Executable)
            arguments.push_back(FMT("/STACK:%d,%d", g_CommandLine.limitStackRT, g_CommandLine.limitStackRT));

        if (buildParameters.buildCfg->backendDebugInformations)
        {
            // 09/14/2023
            // If there's no test function to compile, in release x64, num-cores 1, because of a test filter on a file which should raise an error,
            // then lld never ends because of codeview. Don't know why !! But anyway, in that case, this is not necessary to generate debug infos,
            // because there's nothing to debug !
            if (module->kind != ModuleKind::Test || !module->byteCodeTestFunc.empty())
            {
                if (g_ThreadMgr.numWorkers == 1)
                    arguments.push_back("/DEBUG");
                else
                    arguments.push_back("/DEBUG:GHASH");
            }
        }

        if (buildParameters.buildCfg->backendKind == BuildCfgBackendKind::DynamicLib)
            arguments.push_back("/DLL");

        const auto resultFile = Backend::getOutputFileName(buildParameters);
        arguments.push_back("/OUT:" + resultFile.string());
    }

    void getArguments(const BuildParameters& buildParameters, Module* module, Vector<Utf8>& arguments, bool forCmdLine)
    {
        const auto objFileType = Backend::getObjType(g_CommandLine.target);
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

        // Add user additional linker arguments
        if (buildParameters.buildCfg->linkerArgs.count)
        {
            auto        pz  = (const char*) buildParameters.buildCfg->linkerArgs.buffer;
            const char* pze = pz + buildParameters.buildCfg->linkerArgs.count;
            while (pz < pze)
            {
                Utf8 value;
                while (pz < pze && !SWAG_IS_BLANK(*pz))
                    value += *pz++;
                while (pz < pze && SWAG_IS_BLANK(*pz))
                    pz++;
                arguments.push_back(value);
            }
        }
    }

    bool link(const BuildParameters& buildParameters, Module* module, const Vector<Path>& objectFiles)
    {
        Vector<Utf8> linkArguments;
        getArguments(buildParameters, module, linkArguments, false);

        // Add all object files
        const auto targetPath = Backend::getCacheFolder(buildParameters);
        for (auto& file : objectFiles)
        {
            auto path = targetPath;
            path.append(file.c_str());
            linkArguments.push_back(path.string());
        }

        VectorNative<const char*> linkArgumentsPtr;
        linkArgumentsPtr.push_back("lld");
        for (auto& one : linkArguments)
            linkArgumentsPtr.push_back(one);

        // Log linker parameters
        if (g_CommandLine.verboseLink)
        {
            Utf8 linkStr;
            for (const auto& one : linkArguments)
            {
                linkStr += one;
                linkStr += " ";
            }

            g_Log.messageVerbose(linkStr);
        }

        const llvm::ArrayRef<const char*> array_ref_args(linkArgumentsPtr.buffer, linkArgumentsPtr.size());

        MyOStream diag_stdout;
        MyOStream diag_stderr;
        diag_stdout.module = module;
        diag_stderr.module = module;

        // It's not possible to launch lld linker in parallel (sight), because it's not thread safe.
        static Mutex oo;
        while (!oo.try_lock())
        {
            g_ThreadMgr.tryExecuteJob();
            this_thread::yield();
        }

        const auto objFileType = Backend::getObjType(g_CommandLine.target);
        bool       result      = true;
        switch (objFileType)
        {
        case BackendObjType::Coff:
            result = lld::coff::link(array_ref_args, diag_stdout, diag_stderr, false, false);
            break;
        case BackendObjType::Elf:
            result = lld::elf::link(array_ref_args, diag_stdout, diag_stderr, false, false);
            break;
        case BackendObjType::MachO:
            result = lld::macho::link(array_ref_args, diag_stdout, diag_stderr, false, false);
            break;
        case BackendObjType::Wasm:
            result = lld::wasm::link(array_ref_args, diag_stdout, diag_stderr, false, false);
            break;
        }

#ifdef SWAG_DEV_MODE
        if (!result)
            OS::errorBox("[Developer Mode]", "Error raised !");
#endif

        if (!result)
        {
            g_Workspace->numErrors += diag_stderr.errCount;
            module->numErrors += diag_stderr.errCount;
        }

        oo.unlock();
        return result;
    }

} // namespace BackendLinker

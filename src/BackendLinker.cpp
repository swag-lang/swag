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
                                l = FMT("module [[%s]]: linker error: %s", module->name.c_str(), l2.c_str());
                            }
                            else
                            {
                                Utf8 l2 = pzw + 10;
                                l2.trim();
                                l = FMT("module [[%s]]: linker warning: %s", module->name.c_str(), l2.c_str());
                            }
                        }

                        l += "\n";
                        g_Log.print(l);

#ifdef SWAG_DEV_MODE
                        if (pze)
                            OS::errorBox("[Developer Mode]", "Linker Error raised !");
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

    void getArgumentsCoff(const BuildParameters& buildParameters, const Vector<Path>& objectFiles, Vector<Utf8>& arguments)
    {
        Vector<Path> libPaths;

        // User defined library paths
        for (const auto& p : g_CommandLine.libPaths)
            libPaths.push_back(p);

        // Modules
        libPaths.push_back(g_Workspace->targetPath);

        // Runtime
        libPaths.push_back(g_CommandLine.exePath.parent_path());

        for (const auto& oneLibPath : libPaths)
        {
            auto normalizedLibPath = oneLibPath;
            arguments.push_back("/LIBPATH:" + normalizedLibPath.string());
        }

        const auto libExt = Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgBackendKind::StaticLib);

        // Register #foreignlib
        // As this is defined by the user, we consider the library must exists
        for (const auto& fl : buildParameters.foreignLibs)
        {
            Utf8 one = fl;
            if (Utf8::getExtension(one) != libExt)
                one += libExt;
            arguments.push_back(one);
        }

        // Registered #import dependencies
        for (const auto& dep : buildParameters.module->moduleDependencies)
        {
            Utf8 libName = dep->name;
            if (Utf8::getExtension(libName) != libExt)
                libName += libExt;
            auto fullName = g_Workspace->targetPath;
            fullName.append(libName.c_str());

            // Be sure that the library exists. Some modules rely on external libraries, and do not have their
            // own one
            error_code err;
            if (exists(fullName, err))
                arguments.push_back(libName);
        }

        for (const auto& dep : buildParameters.module->moduleEmbedded)
        {
            if (dep->buildCfg.backendKind != BuildCfgBackendKind::Export)
                continue;

            auto libName = dep->name;
            if (Utf8::getExtension(libName) != libExt)
                libName += libExt;
            auto fullName = g_Workspace->targetPath;
            fullName.append(libName.c_str());

            // Be sure that the library exists. Some modules rely on external libraries, and do not have their
            // own one
            error_code err;
            if (exists(fullName, err))
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
            // then lld never ends because of code view. Don't know why !! But anyway, in that case, this is not necessary to generate debug infos,
            // because there's nothing to debug !
            if (buildParameters.module->kind != ModuleKind::Test || !buildParameters.module->byteCodeTestFunc.empty())
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

        // Add all object files
        const auto targetPath = Backend::getCacheFolder(buildParameters);
        for (auto& file : objectFiles)
        {
            auto path = targetPath;
            path.append(file.c_str());
            arguments.push_back(path.string());
        }
    }

    void getArguments(const BuildParameters& buildParameters, const Vector<Path>& objectFiles, Vector<Utf8>& arguments)
    {
        const auto objFileType = Backend::getObjType(g_CommandLine.target);
        switch (objFileType)
        {
        case BackendObjType::Coff:
            getArgumentsCoff(buildParameters, objectFiles, arguments);
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

    bool link(const BuildParameters& buildParameters, const Vector<Utf8>& linkArguments)
    {
        VectorNative<const char*> linkArgumentsPtr;
        linkArgumentsPtr.push_back("lld");
        for (auto& one : linkArguments)
            linkArgumentsPtr.push_back(one);

        const llvm::ArrayRef llvmArgs(linkArgumentsPtr.buffer, linkArgumentsPtr.size());

        MyOStream myStdOut, myStdErr;
        myStdOut.module = buildParameters.module;
        myStdErr.module = buildParameters.module;

        // It's not possible to launch lld linker in parallel (sight), because it's not thread safe.
        static Mutex oo;
        while (!oo.try_lock())
        {
            g_ThreadMgr.tryExecuteJob();
            this_thread::yield();
        }

        // Log linker parameters
        if (g_CommandLine.verboseLink)
        {
            g_Log.messageVerbose(FMT("linker arguments for module [[%s]]:\n", buildParameters.module->name.c_str()));
            for (const auto& one : linkArguments)
                g_Log.messageVerbose(one);
        }

        const auto objFileType = Backend::getObjType(g_CommandLine.target);
        bool       result      = true;
        switch (objFileType)
        {
        case BackendObjType::Coff:
            result = lld::coff::link(llvmArgs, myStdOut, myStdErr, false, false);
            break;
        case BackendObjType::Elf:
            result = lld::elf::link(llvmArgs, myStdOut, myStdErr, false, false);
            break;
        case BackendObjType::MachO:
            result = lld::macho::link(llvmArgs, myStdOut, myStdErr, false, false);
            break;
        case BackendObjType::Wasm:
            result = lld::wasm::link(llvmArgs, myStdOut, myStdErr, false, false);
            break;
        }

        if (!result)
        {
            g_Workspace->numErrors += myStdErr.errCount;
            buildParameters.module->numErrors += myStdErr.errCount;
        }

        oo.unlock();
        return result;
    }

    bool link(const BuildParameters& buildParameters, const Vector<Path>& objectFiles)
    {
        Vector<Utf8> linkArguments;
        getArguments(buildParameters, objectFiles, linkArguments);
        return link(buildParameters, linkArguments);
    }

} // namespace BackendLinker

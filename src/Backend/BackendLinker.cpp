#include "pch.h"
#include "Backend/BackendLinker.h"
#include "Backend/Backend.h"
#include "Report/Log.h"
#include "Wmf/Module.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Workspace.h"
#ifdef SWAG_DEV_MODE
#include "Os/Os.h"
#endif

class MyOStream final : public llvm::raw_ostream
{
public:
    MyOStream() :
        raw_ostream(true),
        pos(0)
    {
    }

    void write_impl(const char* ptr, size_t len) override
    {
        if (startLine)
        {
            errMsg.clear();
            startLine = false;
        }

        const Utf8 str{ptr, static_cast<uint32_t>(len)};
        errMsg += str;

        if (strstr(ptr, "\n"))
        {
            Vector<Utf8> subNames;
            Utf8::tokenize(errMsg, '\n', subNames);

            g_Log.lock();
            for (auto& l : subNames)
            {
                const auto pze = strstr(l, ": error:");
                const auto pzw = strstr(l, ": warning:");
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
                            l = form("module [[%s]]: linker error: %s", module->name.c_str(), l2.c_str());
                        }
                        else
                        {
                            Utf8 l2 = pzw + 10;
                            l2.trim();
                            l = form("module [[%s]]: linker warning: %s", module->name.c_str(), l2.c_str());
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

    uint64_t current_pos() const override { return pos; }

    Utf8     errMsg;
    size_t   pos;
    uint32_t errCount  = 0;
    Module*  module    = nullptr;
    bool     startLine = true;
};

void BackendLinker::getArguments(const BuildParameters& buildParameters, Vector<Utf8>& arguments, BuildCfgOutputKind outputKind)
{
    arguments.clear();

    const auto objFileType = Backend::getObjType(g_CommandLine.target);
    switch (objFileType)
    {
        case BackendObjType::Coff:
            getArgumentsCoff(buildParameters, arguments, outputKind);
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
        auto        pz  = static_cast<const char*>(buildParameters.buildCfg->linkerArgs.buffer);
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

bool BackendLinker::link(const BuildParameters& buildParameters, const Vector<Utf8>& linkArguments)
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
        std::this_thread::yield();
    }

    // Log linker parameters
    if (g_CommandLine.verboseLink)
    {
        g_Log.messageVerbose(form("linker arguments for module [[%s]]:\n", buildParameters.module->name.c_str()));
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

bool BackendLinker::link(const BuildParameters& buildParameters)
{
    Vector<Utf8> linkArguments;

    switch (buildParameters.buildCfg->backendKind)
    {
        case BuildCfgBackendKind::Executable:
            getArguments(buildParameters, linkArguments, BuildCfgOutputKind::Executable);
            SWAG_CHECK(link(buildParameters, linkArguments));
            break;
        case BuildCfgBackendKind::Library:
            getArguments(buildParameters, linkArguments, BuildCfgOutputKind::DynamicLib);
            SWAG_CHECK(link(buildParameters, linkArguments));
            break;
        default:
            SWAG_ASSERT(false);
            return true;
    }

    return true;
}

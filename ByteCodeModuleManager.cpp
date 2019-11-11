#include "pch.h"
#include "ByteCodeModuleManager.h"
#include "ByteCodeRunContext.h"
#include "SourceFile.h"
#include "Module.h"
#include "Workspace.h"
#include "Os.h"
#include "Context.h"

ByteCodeModuleManager g_ModuleMgr;

ByteCodeModuleManager::ByteCodeModuleManager()
{
    loadedModules["kernel32"] = ::GetModuleHandle(L"kernel32.dll");
#ifdef _DEBUG
    loadedModules["ucrtbase"] = ::GetModuleHandle(L"ucrtbased.dll");
#else
    loadedModules["ucrtbase"] = ::GetModuleHandle(L"ucrtbase.dll");
#endif
}

bool ByteCodeModuleManager::isModuleLoaded(const string& name)
{
    return loadedModules.find(name) != loadedModules.end();
}

bool ByteCodeModuleManager::loadModule(const string& name)
{
    if (loadedModules.find(name) != loadedModules.end())
        return true;

    fs::path path = g_Workspace.targetPath;
    path += "\\";
    path += name;
    path += ".dll";

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBuildPass;
    if (verbose)
        g_Log.verbose(format("   request to load module '%s': ", path.string().c_str()), false);

    auto h = ::LoadLibrary(path.c_str());
    if (h == NULL)
    {
        if (verbose)
            g_Log.verbose("FAIL");
        return false;
    }

    if (verbose)
        g_Log.verbose("success");
    loadedModules[name] = h;

	// Should initialize the module the first time
	// Note that the allocator function of the default context is not set, so the module
	// will initialize it with its internal function
    string funcName = format("%s_globalInit", name.c_str());
    auto   ptr      = ::GetProcAddress(h, funcName.c_str());
    if (ptr)
    {
        static swag_context_t defaultContext = {0};
        swag_process_infos_t  processInfos   = {0};
        processInfos.arguments.addr          = g_CommandLine.userArgumentsSlice.first;
        processInfos.arguments.count         = (uint64_t) g_CommandLine.userArgumentsSlice.second;
        processInfos.contextTlsId            = g_tlsContextIdByteCode;
        processInfos.defaultContext          = &defaultContext;

        typedef void (*funcCall)(void*);
        ((funcCall) ptr)(&processInfos);
    }

    return true;
}

void* ByteCodeModuleManager::getFnPointer(ByteCodeRunContext* context, const string& moduleName, const string& funcName)
{
    // Search in a specific module
    if (!moduleName.empty())
    {
        auto here = loadedModules.find(moduleName);
        if (here != loadedModules.end())
            return ::GetProcAddress((HMODULE) here->second, funcName.c_str());
        return nullptr;
    }

    // Else search in all loaded modules
    for (auto it : loadedModules)
    {
        auto ptr = ::GetProcAddress((HMODULE) it.second, funcName.c_str());
        if (ptr)
            return (void*) ptr;
    }

    return nullptr;
}
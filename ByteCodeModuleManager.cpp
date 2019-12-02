#include "pch.h"
#include "ByteCodeModuleManager.h"
#include "Workspace.h"
#include "Context.h"

ByteCodeModuleManager g_ModuleMgr;

bool ByteCodeModuleManager::isModuleLoaded(const string& name)
{
    shared_lock lk(mutex);
    return loadedModules.find(name) != loadedModules.end();
}

bool ByteCodeModuleManager::loadModule(const string& name)
{
    unique_lock lk(mutex);

    if (loadedModules.find(name) != loadedModules.end())
        return true;

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBuildPass;
    if (verbose)
        g_Log.verbose(format("   request to load module '%s': ", name.c_str()), false);

    // First try in the target folder (local modules)
    fs::path path = g_Workspace.targetPath;
    path += "\\";
    path += name;
    path += ".dll";

    auto h = OS::loadLibrary(path.string().c_str());
    if (h == NULL)
    {
        // Try on system folders
        path = name + ".dll";
        h    = OS::loadLibrary(path.string().c_str());
        if (h == NULL)
        {
            if (verbose)
                g_Log.verbose("FAIL");
            return false;
        }
    }

    if (verbose)
        g_Log.verbose("success");
    loadedModules[name] = h;

    // Should initialize the module the first time
    // Note that the allocator function of the default context is not set, so the module
    // will initialize it with its internal function
    string funcName = format("%s_globalInit", name.c_str());
    auto   ptr      = OS::getProcAddress(h, funcName.c_str());
    if (ptr)
    {
        typedef void (*funcCall)(void*);
        ((funcCall) ptr)(&g_processInfos);
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
            return OS::getProcAddress(here->second, funcName.c_str());
        return nullptr;
    }

    // Else search in all loaded modules
    for (auto it : loadedModules)
    {
        auto ptr = OS::getProcAddress(it.second, funcName.c_str());
        if (ptr)
            return (void*) ptr;
    }

    return nullptr;
}
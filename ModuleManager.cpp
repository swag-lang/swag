#include "pch.h"
#include "ModuleManager.h"
#include "Workspace.h"
#include "Context.h"
#include "Ast.h"
#include "Profile.h"

ModuleManager g_ModuleMgr;

bool ModuleManager::isModuleLoaded(const Utf8& name)
{
    shared_lock lk(mutex);
    return loadedModules.find(name) != loadedModules.end();
}

bool ModuleManager::loadModule(const Utf8& name, bool canBeSystem, bool acceptNotHere)
{
    SWAG_PROFILE(PRF_LOAD, format("load module %s", name.c_str()));

    if (isModuleLoaded(name))
        return true;

    unique_lock lk(mutex);

    // In case it is now loaded, after the lock
    if (loadedModules.find(name) != loadedModules.end())
        return true;

    bool verbose = g_CommandLine.verbose;

    // First try in the target folder (local modules)
    fs::path path = g_Workspace.targetPath;
    path += name.c_str();
    path += ".dll";

    auto h = OS::loadLibrary(path.string().c_str());
    if (h == NULL)
    {
        // Try on system folders
        if (canBeSystem)
        {
            path = name.c_str();
            path += ".dll";
            h = OS::loadLibrary(path.string().c_str());
        }

        if (h == NULL)
        {
            if (acceptNotHere)
                return true;
            if (verbose)
                g_Log.verbose(format("   load module '%s': FAIL\n", name.c_str()), false);
            return false;
        }
    }

    if (verbose)
        g_Log.verbose(format("   load module '%s': success\n", name.c_str()), false);

    loadedModules[name] = h;

    // Should initialize the module the first time
    // Note that the allocator function of the default context is not set, so the module
    // will initialize it with its internal function
    auto callName = name;
    Ast::normalizeIdentifierName(callName);
    Utf8 funcName = format("%s_globalInit", callName.c_str());
    auto ptr      = OS::getProcAddress(h, funcName.c_str());
    if (ptr)
    {
        typedef void (*funcCall)(void*);
        ((funcCall) ptr)(&g_processInfos);
    }

    return true;
}

void* ModuleManager::getFnPointer(ByteCodeRunContext* context, const Utf8& moduleName, const Utf8& funcName)
{
    shared_lock lk(mutex);

    SWAG_ASSERT(!moduleName.empty());
    auto here = loadedModules.find(moduleName);
    if (here != loadedModules.end())
        return OS::getProcAddress(here->second, funcName.c_str());
    return nullptr;
}
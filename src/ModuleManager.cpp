#include "pch.h"
#include "ModuleManager.h"
#include "Workspace.h"
#include "Context.h"
#include "Ast.h"
#include "Profile.h"
#include "Timer.h"
#include "Module.h"

ModuleManager g_ModuleMgr;

bool ModuleManager::isModuleLoaded(const Utf8& moduleName)
{
    shared_lock lk(mutexLoaded);
    return loadedModules.find(moduleName) != loadedModules.end();
}

bool ModuleManager::isModuleFailedLoaded(const Utf8& moduleName)
{
    shared_lock lk(mutexLoaded);
    return failedLoadedModules.find(moduleName) != failedLoadedModules.end();
}

void ModuleManager::resetFailedModule(const Utf8& moduleName)
{
    shared_lock lk(mutexLoaded);
    auto        it = failedLoadedModules.find(moduleName);
    if (it != failedLoadedModules.end())
        failedLoadedModules.erase(it);
}

bool ModuleManager::loadModule(const Utf8& name, bool canBeSystem)
{
    SWAG_PROFILE(PRF_LOAD, format("load module %s", name.c_str()));
    if (isModuleLoaded(name))
        return true;
    if (isModuleFailedLoaded(name))
        return false;

    // First try in the target folder (local modules)
    fs::path path = g_Workspace.targetPath;
    path += name.c_str();
    path += ".dll";

    Timer loadLibTimer;
    if (g_CommandLine.verbosePass)
        loadLibTimer.start();

    auto h = OS::loadLibrary(path.string().c_str());
    if (h == NULL)
    {
        loadModuleError = OS::getLastErrorAsString();

        // Try on system folders
        if (canBeSystem)
        {
            path = name.c_str();
            path += ".dll";
            h = OS::loadLibrary(path.string().c_str());
        }

        if (h == NULL)
        {
            unique_lock lk(mutexLoaded);
            failedLoadedModules.insert(name);
            if (g_CommandLine.verbosePass)
                g_Log.verbose(format("   load module '%s': FAIL\n", name.c_str()), false);
            return false;
        }
    }

    if (g_CommandLine.verbosePass)
    {
        loadLibTimer.stop();
        g_Log.verbosePass(LogPassType::Info, "LoadModule", name.c_str(), loadLibTimer.elapsed);
    }

    unique_lock lk(mutex);

    // In case it is now loaded, after the lock
    if (isModuleLoaded(name))
        return true;

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

    if (!applyPatches(name, h))
        return false;

    unique_lock lk1(mutexLoaded);
    loadedModules[name] = h;
    return true;
}

void* ModuleManager::getFnPointer(const Utf8& moduleName, const Utf8& funcName)
{
    SWAG_ASSERT(!moduleName.empty());
    shared_lock lk(mutexLoaded);
    auto        here = loadedModules.find(moduleName);
    if (here != loadedModules.end())
        return OS::getProcAddress(here->second, funcName.c_str());
    return nullptr;
}

void ModuleManager::addPatchFuncAddress(void** patchAddress, AstFuncDecl* func)
{
    unique_lock lk(mutexPatch);

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(func->typeInfo, TypeInfoKind::FuncAttr);

    ComputedValue moduleName;
    typeFunc->attributes.getValue("Swag.Foreign", "module", moduleName);
    SWAG_ASSERT(!moduleName.text.empty());

    // Apply patch now, because module is already loaded
    if (isModuleLoaded(moduleName.text))
    {
        auto fnPtr = getFnPointer(moduleName.text, func->fullnameForeign);
        SWAG_ASSERT(fnPtr);
        *patchAddress = doForeignLambda(fnPtr);
    }
    else
    {
        PatchOffset newPatch;
        newPatch.patchAddress = patchAddress;
        newPatch.funcDecl     = func;

        auto it = patchOffsets.find(moduleName.text);
        if (it == patchOffsets.end())
            patchOffsets[moduleName.text] = {newPatch};
        else
            it->second.push_back(newPatch);
    }
}

bool ModuleManager::applyPatches(const Utf8& moduleName, void* moduleHandle)
{
    unique_lock lk(mutexPatch);

    auto it = patchOffsets.find(moduleName);
    if (it == patchOffsets.end())
        return true;

    for (auto& one : it->second)
    {
        auto fnPtr = OS::getProcAddress(moduleHandle, one.funcDecl->fullnameForeign.c_str());
        if (!fnPtr)
            return false;

        *one.patchAddress = doForeignLambda(fnPtr);
    }

    return true;
}
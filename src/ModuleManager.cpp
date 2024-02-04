#include "pch.h"
#include "ModuleManager.h"
#include "Backend.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Workspace.h"

ModuleManager* g_ModuleMgr = nullptr;

bool ModuleManager::isModuleLoaded(const Utf8& moduleName)
{
    SharedLock lk(mutexLoaded);
    return loadedModules.find(moduleName) != loadedModules.end();
}

bool ModuleManager::isModuleFailedLoaded(const Utf8& moduleName)
{
    SharedLock lk(mutexLoaded);
    return failedLoadedModules.find(moduleName) != failedLoadedModules.end();
}

void ModuleManager::resetFailedModule(const Utf8& moduleName)
{
    SharedLock lk(mutexLoaded);
    const auto it = failedLoadedModules.find(moduleName);
    if (it != failedLoadedModules.end())
        failedLoadedModules.erase(it);
}

bool ModuleManager::loadModule(const Utf8& name, bool canBeSystem)
{
    if (isModuleLoaded(name))
        return true;
    if (isModuleFailedLoaded(name))
        return false;

    // First try in the target folder (local modules)
    const auto ext  = Backend::getOutputFileExtension(OS::getNativeTarget(), BuildCfgBackendKind::DynamicLib);
    Path       path = g_Workspace->getTargetPath(g_CommandLine.buildCfg, OS::getNativeTarget());
    path.append(name.c_str());
    path += ext.c_str();

    auto h = OS::loadLibrary(path.string().c_str());
    if (h == NULL)
    {
        // Try on system folders
        if (canBeSystem)
        {
            path = name.c_str();
            path += ext.c_str();
            h = OS::loadLibrary(path.string().c_str());
        }

        if (h == NULL)
        {
            ScopedLock lk(mutexLoaded);
            loadModuleError = OS::getLastErrorAsString();
            failedLoadedModules.insert(name);
            return false;
        }
    }

    ScopedLock lk(mutex);

    // In case it is now loaded, after the lock
    if (isModuleLoaded(name))
        return true;

    // Should initialize the module the first time
    // Note that the allocator function of the default context is not set, so the module
    // will initialize it with its internal function
    auto callName = name;
    Ast::normalizeIdentifierName(callName);
    const Utf8 funcName = Fmt(g_LangSpec->name_globalInit, callName.c_str());
    const auto ptr      = OS::getProcAddress(h, funcName.c_str());
    if (ptr)
    {
        typedef void (*funcCall)(void*);
        ((funcCall) ptr)(&g_ProcessInfos);
    }

    if (!applyPatches(name, h))
        return false;

    ScopedLock lk1(mutexLoaded);
    loadedModules[name] = h;
    return true;
}

void* ModuleManager::getFnPointer(const Utf8& moduleName, const Utf8& funcName)
{
    SWAG_ASSERT(!moduleName.empty());
    SharedLock lk(mutexLoaded);
    const auto here = loadedModules.find(moduleName);
    if (here != loadedModules.end())
    {
        const auto name = funcName.toZeroTerminated();
        return OS::getProcAddress(here->second, name.c_str());
    }

    return nullptr;
}

const Utf8& ModuleManager::getForeignModuleName(AstFuncDecl* func)
{
    const auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(func->typeInfo, TypeInfoKind::FuncAttr);
    const auto moduleName = typeFunc->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_module);
    SWAG_ASSERT(moduleName && !moduleName->text.empty());
    return moduleName->text;
}

void ModuleManager::addPatchFuncAddress(DataSegment* seg, void** patchAddress, AstFuncDecl* func)
{
    const auto moduleName = getForeignModuleName(func);
    ScopedLock lk(mutexPatch);

    // Apply patch now, because module is already loaded
    if (isModuleLoaded(moduleName))
    {
        const auto fnPtr = getFnPointer(moduleName, func->fullnameForeign);
        SWAG_ASSERT(fnPtr);
        *patchAddress = fnPtr;
    }
    else
    {
        PatchOffset newPatch;
        newPatch.segment      = seg;
        newPatch.patchAddress = patchAddress;
        newPatch.funcDecl     = func;
#ifdef SWAG_DEV_MODE
        *(uint64_t*) patchAddress = SWAG_PATCH_MARKER;
#endif

        const auto it = patchOffsets.find(moduleName);
        if (it == patchOffsets.end())
            patchOffsets[moduleName] = {newPatch};
        else
            it->second.push_back(newPatch);
    }
}

bool ModuleManager::applyPatches(const Utf8& moduleName, void* moduleHandle)
{
    ScopedLock lk(mutexPatch);

    const auto it = patchOffsets.find(moduleName);
    if (it == patchOffsets.end())
        return true;

    for (auto& one : it->second)
    {
        // Address is no more valid !!!
        if (one.segment->deleted)
            continue;

#ifdef SWAG_DEV_MODE
        SWAG_ASSERT(*(uint64_t*) one.patchAddress == SWAG_PATCH_MARKER);
#endif

        auto       foreign = one.funcDecl->fullnameForeign.toZeroTerminated();
        const auto fnPtr   = OS::getProcAddress(moduleHandle, foreign.c_str());
        if (!fnPtr)
        {
            loadModuleError = OS::getLastErrorAsString();
            return false;
        }

        *one.patchAddress = fnPtr;
    }

    return true;
}

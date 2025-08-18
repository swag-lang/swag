#include "pch.h"
#include "Wmf/ModuleManager.h"
#include "Backend/Backend.h"
#include "Backend/Context.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

#ifdef SWAG_DEV_MODE
// ReSharper disable once CppUseAuto
void* g_SwagPatchMarker = reinterpret_cast<void*>(0xAABBCCDD00112233);
#endif

ModuleManager* g_ModuleMgr = nullptr;

bool ModuleManager::isModuleLoaded(const Utf8& moduleName)
{
    SharedLock lk(mutexLoaded);
    return loadedModules.contains(moduleName);
}

bool ModuleManager::isModuleFailedLoaded(const Utf8& moduleName)
{
    SharedLock lk(mutexLoaded);
    return failedLoadedModules.contains(moduleName);
}

void ModuleManager::resetFailedModule(const Utf8& moduleName)
{
    SharedLock lk(mutexLoaded);
    if (const auto it = failedLoadedModules.find(moduleName); it != failedLoadedModules.end())
        failedLoadedModules.erase(it);
}

bool ModuleManager::loadModule(const Utf8& name, bool canBeSystem)
{
    if (isModuleLoaded(name))
        return true;
    if (isModuleFailedLoaded(name))
        return false;

    // First try in the target folder (local modules)
    auto path = Backend::getOutputFileName(OS::getNativeTarget(), name, BuildCfgOutputKind::DynamicLib);
    auto h    = OS::loadLibrary(path);
    if (h == nullptr)
    {
        // Try on system folders
        if (canBeSystem)
        {
            path = name;
            path += Backend::getOutputFileExtension(OS::getNativeTarget(), BuildCfgOutputKind::DynamicLib);
            h = OS::loadLibrary(path);
        }

        if (h == nullptr)
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
    const auto& callName = name;
    Ast::normalizeIdentifierName(callName);
    const Utf8 funcName = form(g_LangSpec->name_globalInit, callName.cstr());
    if (const auto ptr = OS::getProcAddress(h, funcName.cstr()))
    {
        using FuncCall = void (*)(void*);
        reinterpret_cast<FuncCall>(ptr)(&g_ProcessInfos);
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
    if (const auto here = loadedModules.find(moduleName); here != loadedModules.end())
    {
        const auto name = funcName.toZeroTerminated();
        return OS::getProcAddress(here->second, name.cstr());
    }

    return nullptr;
}

const Utf8& ModuleManager::getForeignModuleName(const AstFuncDecl* func)
{
    const auto typeFunc   = castTypeInfo<TypeInfoFuncAttr>(func->typeInfo, TypeInfoKind::FuncAttr);
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
        const auto fnPtr = getFnPointer(moduleName, func->getFullNameForeignImport());
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
        *patchAddress = g_SwagPatchMarker;
#endif

        if (const auto it            = patchOffsets.find(moduleName); it == patchOffsets.end())
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

    for (const auto& one : it->second)
    {
        // Address is no more valid!!!
        if (one.segment->deleted)
            continue;

#ifdef SWAG_DEV_MODE
        SWAG_ASSERT(*one.patchAddress == g_SwagPatchMarker);
#endif

        auto       foreign = one.funcDecl->getFullNameForeignImport().toZeroTerminated();
        const auto fnPtr   = OS::getProcAddress(moduleHandle, foreign.cstr());
        if (!fnPtr)
        {
            loadModuleError = OS::getLastErrorAsString();
            return false;
        }

        *one.patchAddress = fnPtr;
    }

    return true;
}

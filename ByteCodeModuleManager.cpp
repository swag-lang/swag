#include "pch.h"
#include "ByteCodeModuleManager.h"
#include "ByteCodeRunContext.h"
#include "SourceFile.h"
#include "Module.h"
#include "Workspace.h"

ByteCodeModuleManager g_ModuleMgr;

void ByteCodeModuleManager::loadModule(ByteCodeRunContext* context, const string& name)
{
    if (loadedModules.find(name) != loadedModules.end())
        return;

    fs::path path = context->sourceFile->module->workspace->cachePath;
    path += name;
    path += ".dll";

    auto h = ::LoadLibrary(path.c_str());
    if (h == INVALID_HANDLE_VALUE)
    {
        assert(false);
        return;
    }

    loadedModules[name] = h;
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
#pragma once
#pragma once
#include "Utf8.h"

struct ByteCodeRunContext;

struct ModuleManager
{
    bool  loadModule(const Utf8& name, bool canBeSystem = true, bool acceptNotHere = false);
    bool  isModuleLoaded(const Utf8& name);
    void* getFnPointer(ByteCodeRunContext* context, const Utf8& moduleName, const Utf8& funcName);

    shared_mutex     mutex;
    map<Utf8, void*> loadedModules;
};

extern ModuleManager g_ModuleMgr;
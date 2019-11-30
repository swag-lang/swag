#pragma once
#pragma once
struct ByteCodeRunContext;

struct ByteCodeModuleManager
{
    bool  loadModule(const string& name);
    bool  isModuleLoaded(const string& name);
    void* getFnPointer(ByteCodeRunContext* context, const string& moduleName, const string& funcName);

    map<string, void*> loadedModules;
};

extern ByteCodeModuleManager g_ModuleMgr;
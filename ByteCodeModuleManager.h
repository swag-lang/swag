#pragma once
struct ByteCodeRunContext;

struct ByteCodeModuleManager
{
	ByteCodeModuleManager();
    void  loadModule(const string& name);
    bool  isModuleLoaded(const string& name);
    void* getFnPointer(ByteCodeRunContext* context, const string& moduleName, const string& funcName);

    map<string, HMODULE> loadedModules;
};

extern ByteCodeModuleManager g_ModuleMgr;
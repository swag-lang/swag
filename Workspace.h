#pragma once
#include "SpinLock.h"
#include "ByteCodeRunContext.h"
struct Module;
struct Scope;

struct Workspace
{
    bool    build();
    Module* createOrUseModule(const fs::path& path);

    void enumerateFilesInModule(const fs::path& path);
    void buildRuntime();
    void enumerateModules();
    bool buildModules(const vector<Module*>& list);

    fs::path               cachePath;
    SpinLock               mutexModules;
    atomic<int>            numErrors = 0;
    vector<Module*>        modules;
    map<fs::path, Module*> mapModulesPaths;
    map<string, Module*>   mapModulesNames;
    Scope*                 scopeRoot;
    ByteCodeRunContext     runContext;
};

extern Workspace g_Workspace;
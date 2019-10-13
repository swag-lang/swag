#pragma once
#include "SpinLock.h"
#include "ByteCodeRunContext.h"
#include "swagScope.h"
struct Module;
struct Scope;

struct Workspace
{
    bool    build(const fs::path& path);
    Module* createOrUseModule(const fs::path& path);

    void    enumerateFilesInModule(const fs::path& path);
    void    addRuntime();
    void    setup(const fs::path& path);
    bool    buildModules(const vector<Module*>& list);
    Module* getModuleByName(const string& name);
    void    removeCache();

    fs::path               workspacePath;
    fs::path               cachePath;
    SpinLock               mutexModules;
    atomic<int>            numErrors = 0;
    vector<Module*>        modules;
    map<fs::path, Module*> mapModulesPaths;
    map<string, Module*>   mapModulesNames;
    ByteCodeRunContext     runContext;
    Module*                runtimeModule;
    SwagScope              swagScope;
};

extern Workspace g_Workspace;
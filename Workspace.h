#pragma once
#include "SpinLock.h"
#include "ByteCodeRunContext.h"
#include "swagScope.h"
struct Module;
struct Scope;

struct Workspace
{
    bool    build(const fs::path& path);
    Module* createOrUseModule(const string& moduleName);

    void    enumerateFilesInModule(const fs::path& path, Module* module, bool tests);
    void    addRuntime();
    void    setup(const fs::path& path);
    bool    buildModules(const vector<Module*>& list);
    Module* getModuleByName(const string& moduleName);
    void    removeCache();

    fs::path               workspacePath;
    fs::path               cachePath;
    fs::path               testsPath;
    SpinLock               mutexModules;
    atomic<int>            numErrors = 0;
    vector<Module*>        modules;
    map<string, Module*>   mapModulesNames;
    ByteCodeRunContext     runContext;
    Module*                runtimeModule;
    SwagScope              swagScope;
};

extern Workspace g_Workspace;
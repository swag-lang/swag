#pragma once
#include "SpinLock.h"
#include "ByteCodeRunContext.h"
#include "swagScope.h"
struct Module;
struct Scope;
struct Target;

struct Workspace
{
    bool    buildTarget();
    bool    build();
    Module* createOrUseModule(const string& moduleName);

    void    enumerateModules();
    void    enumerateFilesInModule(const fs::path& path, Module* module, bool tests);
    void    addRuntime();
    void    setup();
    bool    buildModules(const vector<Module*>& list);
    Module* getModuleByName(const string& moduleName);
    void    clearPath(const fs::path& path);
    void    setupTarget();

    Target*              currentTarget = nullptr;
    fs::path             workspacePath;
    fs::path             targetPath;
    fs::path             targetTestPath;
    fs::path             testsPath;
    fs::path             sourcePath;
    SpinLock             mutexModules;
    atomic<int>          numErrors = 0;
    vector<Module*>      modules;
    map<string, Module*> mapModulesNames;
    ByteCodeRunContext   runContext;
    Module*              runtimeModule;
    SwagScope            swagScope;
};

extern Workspace g_Workspace;
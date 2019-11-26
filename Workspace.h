#pragma once
#include "SpinLock.h"
#include "ByteCodeRunContext.h"
#include "swagScope.h"
struct Module;
struct Scope;
struct CompilerTarget;

struct Workspace
{
    bool    buildTarget();
    bool    build();
    Module* createOrUseModule(const string& moduleName);

    void    enumerateModules(const fs::path& path, bool fromTests);
    void    enumerateFilesInModule(const fs::path& path, Module* module);
    void    addRuntime();
    void    setup();
    bool    buildModules(const vector<Module*>& list);
    Module* getModuleByName(const string& moduleName);
    void    clearPath(const fs::path& path);
    void    setupTarget();

    CompilerTarget               target;
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
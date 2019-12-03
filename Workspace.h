#pragma once
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

    void    addRuntime();
    void    publishModule(Module* module);
    void    setup();
    Module* getModuleByName(const string& moduleName);
    void    deleteFolderContent(const fs::path& path);
    void    setupTarget();

    CompilerTarget       target;
    fs::path             workspacePath;
    fs::path             targetPath;
    fs::path             testsPath;
    fs::path             modulesPath;
    fs::path             dependenciesPath;
    shared_mutex         mutexModules;
    atomic<int>          numErrors = 0;
    vector<Module*>      modules;
    map<string, Module*> mapModulesNames;
    ByteCodeRunContext   runContext;
    Module*              runtimeModule;
    SwagScope            swagScope;
};

extern Workspace g_Workspace;
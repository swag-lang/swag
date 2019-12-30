#pragma once
#include "ByteCodeRunContext.h"
#include "swagScope.h"
struct Module;
struct Scope;
struct CompilerTarget;

struct Workspace
{
    void createNew();

    bool    buildTarget();
    bool    build();
    Module* createOrUseModule(const Utf8& moduleName);

    void    addBootstrap();
    void    setupPaths();
    void    setup();
    Module* getModuleByName(const Utf8& moduleName);
    void    deleteFolderContent(const fs::path& path);
    void    setupTarget();

    bool watch();

    CompilerTarget     target;
    fs::path           workspacePath;
    fs::path           targetPath;
    fs::path           cachePath;
    fs::path           testsPath;
    fs::path           modulesPath;
    fs::path           dependenciesPath;
    shared_mutex       mutexModules;
    atomic<int>        numErrors = 0;
    vector<Module*>    modules;
    map<Utf8, Module*> mapModulesNames;
    ByteCodeRunContext runContext;
    Module*            bootstrapModule;
    SwagScope          swagScope;
};

extern Workspace g_Workspace;
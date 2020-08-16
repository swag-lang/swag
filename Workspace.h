#pragma once
#include "SwagScope.h"
#include "Utf8.h"
struct Module;
struct Scope;

struct Workspace
{
    void createNew();

    void    checkPendingJobs();
    bool    buildTarget();
    bool    build();
    Module* createOrUseModule(const Utf8& moduleName);

    void    addBootstrap();
    void    setupPaths();
    void    setup();
    Module* getModuleByName(const Utf8& moduleName);
    void    deleteFolderContent(const fs::path& path);
    Utf8    GetArchName();
    Utf8    GetOsName();
    void    setupTarget();

    bool watch();

    fs::path              workspacePath;
    fs::path              targetPath;
    fs::path              cachePath;
    fs::path              testsPath;
    fs::path              modulesPath;
    fs::path              dependenciesPath;
    shared_mutex          mutexModules;
    atomic<int>           numErrors = 0;
    VectorNative<Module*> modules;
    map<Utf8, Module*>    mapModulesNames;
    Module*               filteredModule = nullptr;
    Module*               bootstrapModule;
    SwagScope             swagScope;
};

extern Workspace g_Workspace;
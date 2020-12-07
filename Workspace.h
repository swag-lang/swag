#pragma once
#include "SwagScope.h"
#include "Utf8.h"
#include "Register.h"
struct Module;
struct Scope;

struct OneTag
{
    Utf8          cmdLine;
    Utf8          name;
    TypeInfo*     type;
    ComputedValue value;
};

struct Workspace
{
    void createNew();

    void    checkPendingJobs();
    bool    buildTarget();
    bool    build();
    Module* createOrUseModule(const Utf8& moduleName, bool fromTestsFolder, bool fromExamplesFolder);

    void    addBootstrap();
    void    addRuntime();
    void    addRuntimeFile(const char* fileName);
    void    setupPaths();
    void    setupInternalTags();
    void    setupUserTags();
    void    setup();
    Module* getModuleByName(const Utf8& moduleName);
    void    deleteFolderContent(const fs::path& path);
    Utf8    GetArchName();
    Utf8    GetOsName();
    OneTag* hasTag(const Utf8& name);
    void    cleanPublic(const fs::path& basePath);
    void    clean();
    void    setupCachePath();
    void    setupTarget();

    bool watch();

    fs::path              workspacePath;
    fs::path              targetPath;
    fs::path              cachePath;
    fs::path              testsPath;
    fs::path              examplesPath;
    fs::path              modulesPath;
    fs::path              dependenciesPath;
    shared_mutex          mutexModules;
    atomic<int>           numErrors = 0;
    VectorNative<Module*> modules;

    vector<OneTag> tags;

    map<Utf8, Module*> mapModulesNames;
    Module*            filteredModule = nullptr;
    Module*            bootstrapModule;
    Module*            runtimeModule;
    SwagScope          swagScope;
};

extern Workspace g_Workspace;
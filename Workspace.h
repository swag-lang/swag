#pragma once
#include "SpinLock.h"
#include "ByteCodeRunContext.h"
struct Module;
struct Scope;
struct PoolFactory;

struct Workspace
{
    Workspace();

    bool    build();
    Module* createOrUseModule(const fs::path& path);

    void enumerateFilesInModule(const fs::path& path);
    void buildRuntime();
    void enumerateModules();

    SpinLock               mutexModules;
    atomic<int>            numErrors = 0;
    vector<Module*>        modules;
    map<fs::path, Module*> mapModules;
    PoolFactory*           poolFactory;
    Scope*                 scopeRoot;
    ByteCodeRunContext     runContext;
};

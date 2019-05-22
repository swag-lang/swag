#pragma once
#include "SpinLock.h"
struct Module;

struct Workspace
{
    bool    build();
    Module* createOrUseModule(const fs::path& path);

    void enumerateFilesInModule(const fs::path& path);
    void enumerateModules();

    SpinLock        mutexModules;
    atomic<int>     numErrors;
    vector<Module*> modules;
};

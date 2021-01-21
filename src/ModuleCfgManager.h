#pragma once
#include "Utf8.h"

struct SourceFile;
struct Module;
struct ModuleDependency;

struct ModuleCfgManager
{
    void    registerCfgFile(SourceFile* file);
    void    newCfgFile(vector<SourceFile*>& allFiles, string dirName, string fileName);
    void    enumerateCfgFiles(const fs::path& path);
    bool    resolveModuleDependency(Module* cfgModule, ModuleDependency* dep);
    bool    dependencyIsMatching(ModuleDependency* dep, Module* module);
    bool    execute();
    Module* getCfgModule(const Utf8& name);

    map<Utf8, Module*> allModules;
};

extern ModuleCfgManager g_ModuleCfgMgr;
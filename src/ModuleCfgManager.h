#pragma once
#include "Utf8.h"

struct SourceFile;
struct Module;
struct ModuleDependency;

struct ModuleCfgManager
{
    void    registerCfgFile(SourceFile* file);
    void    newCfgFile(vector<SourceFile*>& allFiles, const Utf8& dirName, const Utf8& fileName);
    void    enumerateCfgFiles(const fs::path& path);
    bool    fetchModuleCfgLocal(ModuleDependency* dep, bool& done, Utf8& cfgFilePath, Utf8& cfgFileName);
    bool    fetchModuleCfg(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName);
    bool    resolveModuleDependency(Module* cfgModule, ModuleDependency* dep);
    bool    dependencyIsMatching(ModuleDependency* dep, Module* module);
    void    parseCfgFile(Module* cfgModule);
    bool    execute();
    Module* getCfgModule(const Utf8& name);

    map<Utf8, Module*>  allModules;
    vector<SourceFile*> allNewCfgFiles;
};

extern ModuleCfgManager g_ModuleCfgMgr;
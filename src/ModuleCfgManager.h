#pragma once
#include "Vector.h"

struct SourceFile;
struct Module;
struct ModuleDependency;
struct Utf8;

enum class CompareVersionResult
{
    VERSION_LOWER,
    VERSION_GREATER,
    REVISION_LOWER,
    REVISION_GREATER,
    BUILDNUM_LOWER,
    BUILDNUM_GREATER,
    EQUAL,
};

struct ModuleCfgManager
{
    fs::path             getAliasPath(const fs::path& srcPath);
    void                 registerCfgFile(SourceFile* file);
    void                 newCfgFile(Vector<SourceFile*>& allFiles, const Utf8& dirName, const Utf8& fileName);
    void                 enumerateCfgFiles(const fs::path& path);
    bool                 fetchModuleCfgLocal(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName);
    bool                 fetchModuleCfgSwag(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName, bool fetch);
    bool                 fetchModuleCfgDisk(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName, bool fetch);
    bool                 fetchModuleCfg(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName, bool fetch);
    bool                 resolveModuleDependency(Module* srcModule, ModuleDependency* dep);
    CompareVersionResult compareVersions(uint32_t depVer, uint32_t depRev, uint32_t devBuildNum, uint32_t modVer, uint32_t modRev, uint32_t modBuildNum);
    void                 parseCfgFile(Module* cfgModule);
    bool                 execute();
    Module*              getCfgModule(const Utf8& name);

    map<Utf8, Module*> allModules;
    set<Module*>       pendingCfgModules;
};

extern ModuleCfgManager* g_ModuleCfgMgr;
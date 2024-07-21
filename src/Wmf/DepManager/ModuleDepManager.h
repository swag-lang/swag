#pragma once

struct SourceFile;
struct Module;
struct ModuleDependency;
struct Utf8;

enum class CompareVersionResult
{
    VersionLower,
    VersionGreater,
    RevisionLower,
    RevisionGreater,
    BuildNumLower,
    BuildNumGreater,
    Equal,
};

struct ModuleDepManager
{
    static Path getAliasPath(const Path& srcPath);
    static bool fetchModuleCfgLocal(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName);
    static bool fetchModuleCfgSwag(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName, bool fetch);
    static bool fetchModuleCfgDisk(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName, bool fetch);
    static bool fetchModuleCfg(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName, bool fetch);

    void                        registerCfgFile(SourceFile* file);
    void                        newCfgFile(Vector<SourceFile*>& allFiles, const Utf8& dirName, const Utf8& fileName);
    void                        enumerateCfgFiles(const Path& path);
    static void                 parseCfgFile(Module* cfgModule);
    bool                        resolveModuleDependency(const Module* srcModule, ModuleDependency* dep);
    static CompareVersionResult compareVersions(uint32_t depVer, uint32_t depRev, uint32_t devBuildNum, uint32_t modVer, uint32_t modRev, uint32_t modBuildNum);
    bool                        execute();
    Module*                     getCfgModule(const Utf8& name);

    MapUtf8<Module*> allModules;
    Set<Module*>     pendingCfgModules;
};

extern ModuleDepManager* g_ModuleCfgMgr;

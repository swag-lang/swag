#pragma once
struct SourceFile;
struct Module;

struct ModuleCfgManager
{
    void registerCfgFile(SourceFile* file);
    void newCfgFile(vector<SourceFile*>& allFiles, string dirName, string fileName);
    void enumerateCfgFiles(const fs::path& path);
    bool execute();
};

extern ModuleCfgManager g_ModuleCfgMgr;
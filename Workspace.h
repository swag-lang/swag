#pragma once
struct Module;

struct Workspace
{
    bool    build();
    Module* createOrUseModule(const fs::path& path);

    void enumerateFilesInModule(const fs::path& path);
    void enumerateModules();

    atomic<int>     numErrors;
    vector<Module*> m_modules;
};

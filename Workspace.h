#pragma once
class Module;

class Workspace
{
public:
    bool    build();
    Module* createOrUseModule(const fs::path& path);

private:
    void enumerateFilesInModule(const fs::path& path);
    void enumerateModules();

private:
    vector<Module*> m_modules;
};

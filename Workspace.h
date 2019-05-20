#pragma once
class Workspace
{
public:
    bool build();

private:
    void enumerateFilesInModule(const fs::path& path);
	void enumerateModules();

private:
	vector<class Module*> m_modules;
};

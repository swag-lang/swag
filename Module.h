#pragma once
class SourceFile;

class Module
{
public:
    fs::path            m_path;
    string              m_name;
    vector<SourceFile*> m_files;

public:
    Module(const fs::path& path)
        : m_path{path}
    {
        m_name = path.filename().string();
    }

	void addFile(SourceFile* file);
	void removeFile(SourceFile* file);
};

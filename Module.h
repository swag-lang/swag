#pragma once
class SourceFile;

class Module
{
public:
    fs::path            m_path;
    string              m_name;
    vector<SourceFile*> m_files;
    atomic<int>         numErrors;

public:
    Module(const fs::path& path)
        : m_path{path}
    {
        m_name = path.filename().string();
    }

    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);
    bool build();

private:
    struct AstNode* m_astRoot = nullptr;
};

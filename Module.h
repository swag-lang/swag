#pragma once
struct SourceFile;
struct AstNode;

struct Module
{
    Module(const fs::path& path)
        : m_path{path}
    {
        m_name = path.filename().string();
    }

    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);

    fs::path            m_path;
    string              m_name;
    vector<SourceFile*> m_files;
    atomic<int>         numErrors;
    AstNode*            m_astRoot = nullptr;
};

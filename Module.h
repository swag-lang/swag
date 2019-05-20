#pragma once
struct SourceFile;
struct AstNode;

struct Module
{
    Module(const fs::path& path)
        : path{path}
    {
        name = path.filename().string();
    }

    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);

    fs::path            path;
    string              name;
    vector<SourceFile*> files;
    atomic<int>         numErrors;
    AstNode*            astRoot = nullptr;
};

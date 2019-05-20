#pragma once
struct SourceFile;
struct AstScopeNode;
struct SymTable;

struct Module
{
    Module(const fs::path& path);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);

    fs::path            path;
    string              name;
    vector<SourceFile*> files;
    atomic<int>         numErrors;
    AstScopeNode*       astRoot = nullptr;
};

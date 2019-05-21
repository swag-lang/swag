#pragma once
#include "AstNode.h"
struct SourceFile;
struct SymTable;

struct Module : public AstScope
{
    Module(const fs::path& path);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);

    fs::path            path;
    string              name;
    vector<SourceFile*> files;
    atomic<int>         numErrors;
};

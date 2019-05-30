#pragma once
#include "AstNode.h"
#include "PoolFactory.h"
#include "TypeManager.h"
struct SourceFile;
struct SymTable;
struct Utf8Crc;
struct Workspace;

struct Module : public PoolElement
{
    Module(Workspace* workspace, const fs::path& path, bool runtime = false);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);

    fs::path            path;
    string              name;
    atomic<int>         numErrors;
    SpinLock            mutexFile;
    vector<SourceFile*> files;
    AstNode*            astRoot;
    Scope*              scopeRoot;
    PoolFactory         poolFactory;
    TypeManager         typeMgr;
    Workspace*          workspace;
};

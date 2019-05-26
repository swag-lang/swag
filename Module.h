#pragma once
#include "AstNode.h"
#include "PoolFactory.h"
#include "TypeManager.h"
struct SourceFile;
struct SymTable;
struct Utf8Crc;

struct Module : public PoolElement
{
    Module(const fs::path& path);
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
};

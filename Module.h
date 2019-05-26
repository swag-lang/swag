#pragma once
#include "AstNode.h"
#include "PoolFactory.h"
struct SourceFile;
struct SymTable;
struct Utf8Crc;

struct Module : public PoolElement
{
    Module(const fs::path& path);
    void   addFile(SourceFile* file);
    void   removeFile(SourceFile* file);
    Scope* newNamespace(Scope* parentScope, Utf8Crc& npName);

    fs::path             path;
    string               name;
    atomic<int>          numErrors;
    SpinLock             mutexFile;
    vector<SourceFile*>  files;
    SpinLock             mutexNamespace;
    map<Utf8Crc, Scope*> namespaces;
    AstNode*             astRoot;
    Scope*               scopeRoot;
    PoolFactory          poolFactory;
};

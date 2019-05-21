#pragma once
#include "AstNode.h"
#include "PoolFactory.h"
struct SourceFile;
struct SymTable;
struct utf8crc;

struct Module : public PoolElement
{
    Module(const fs::path& path);
    void      addFile(SourceFile* file);
    void      removeFile(SourceFile* file);
    Scope* newNamespace(Scope* parentScope, const utf8crc& npName);

    fs::path                path;
    string                  name;
    atomic<int>             numErrors;
    SpinLock                mutexFile;
    vector<SourceFile*>     files;
    SpinLock                mutexNamespace;
    map<utf8crc, Scope*> namespaces;
    AstNode*                astRoot;
    Scope*               scopeRoot;
    PoolFactory             poolFactory;
};

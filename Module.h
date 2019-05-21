#pragma once
#include "AstNode.h"
#include "PoolFactory.h"
struct SourceFile;
struct SymTable;
struct utf8crc;

struct Module : public AstScope
{
    Module(const fs::path& path);
    void      addFile(SourceFile* file);
    void      removeFile(SourceFile* file);
    AstScope* newNamespace(AstScope* parentScope, const utf8crc& npName);

    fs::path                path;
    string                  name;
    atomic<int>             numErrors;
    SpinLock                mutexFile;
    vector<SourceFile*>     files;
    SpinLock                mutexNamespace;
    map<utf8crc, AstScope*> namespaces;
    PoolFactory             poolFactory;
};

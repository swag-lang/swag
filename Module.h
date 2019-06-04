#pragma once
#include "Pool.h"
#include "SpinLock.h"
struct SourceFile;
struct SymTable;
struct Utf8Crc;
struct PoolFactory;
struct Workspace;
struct AstNode;
struct Scope;

struct Module : public PoolElement
{
    Module(Workspace* workspace, const fs::path& path, bool runtime = false);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);

    fs::path            path;
    string              name;
    atomic<int>         numErrors = 0;
    SpinLock            mutexFile;
    vector<SourceFile*> files;
    AstNode*            astRoot;
    Scope*              scopeRoot;
    Workspace*          workspace;
    bool                isRuntime = false;

    uint32_t reserveRegisterRC();
    void     freeRegisterRC(uint32_t reg);
    void     reserveRegisterRR(uint32_t count);
    bool     executeNode(SourceFile* sourceFile, AstNode* node);

    SpinLock           mutexRegisterRC;
    SpinLock           mutexRegisterRR;
    uint32_t           maxReservedRegisterRC = 0;
    vector<uint32_t>   availableRegistersRC;
    uint32_t           maxReservedRegisterRR = 0;
};

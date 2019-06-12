#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "BuildPass.h"
struct Utf8;
struct SourceFile;
struct SymTable;
struct Utf8Crc;
struct Workspace;
struct AstNode;
struct Scope;
struct ByteCode;

struct Module : public PoolElement
{
    void setup(Workspace* wkp, const fs::path& pth, bool runtime = false);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);
    void error(const Utf8& msg);
    void internalError(const Utf8& msg);

    fs::path            path;
    string              name;
    atomic<int>         numErrors = 0;
    SpinLock            mutexFile;
    vector<SourceFile*> files;
    AstNode*            astRoot;
    Scope*              scopeRoot;
    Workspace*          workspace;
    bool                isRuntime = false;

    uint32_t reserveRegisterRC(ByteCode* bc);
    void     freeRegisterRC(uint32_t reg);
    void     reserveRegisterRR(uint32_t count);
    bool     executeNode(SourceFile* sourceFile, AstNode* node);

    SpinLock         mutexRegisterRC;
    SpinLock         mutexRegisterRR;
    uint32_t         maxReservedRegisterRC = 0;
    vector<uint32_t> availableRegistersRC;
    uint32_t         maxReservedRegisterRR = 0;

    int reserveDataSegment(int size, void* content = nullptr)
    {
        scoped_lock lk(mutexDataSeg);
        int         result = (int) dataSegment.size();
        dataSegment.resize((int) dataSegment.size() + size);
        if (content)
            memcpy(&dataSegment[result], content, size);
        return result;
    }

    SpinLock        mutexDataSeg;
    vector<uint8_t> dataSegment;

    void setBuildPass(BuildPass buildP);

    SpinLock  mutexBuildPass;
    BuildPass buildPass = BuildPass::Full;

    void addByteCodeFunc(ByteCode* bc);

    SpinLock          mutexByteCode;
    vector<ByteCode*> byteCodeFunc;
    vector<ByteCode*> byteCodeTestFunc;
};

extern Pool<Module> g_Pool_module;
#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "BuildPass.h"
#include "BackendParameters.h"
struct Utf8;
struct SourceFile;
struct SymTable;
struct Utf8Crc;
struct Workspace;
struct AstNode;
struct Scope;
struct ByteCode;
struct Job;
struct Backend;

struct Module : public PoolElement
{
    void setup(Workspace* wkp, const fs::path& pth, bool runtime = false);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);
    void error(const Utf8& msg);
    void internalError(const Utf8& msg);

    fs::path            path;
    string              name;
    string              nameUp;
    atomic<int>         numErrors = 0;
    SpinLock            mutexFile;
    vector<SourceFile*> files;
    AstNode*            astRoot;
    Scope*              scopeRoot;
    Workspace*          workspace;
    bool                isRuntime = false;
    BackendParameters   backendParameters;
    Backend*            backend = nullptr;
    set<string>         compileVersion;

    uint32_t reserveRegisterRC(ByteCode* bc);
    void     freeRegisterRC(uint32_t reg);
    void     reserveRegisterRR(uint32_t count);
    bool     executeNode(SourceFile* sourceFile, AstNode* node);

    SpinLock         mutexRegisterRC;
    SpinLock         mutexRegisterRR;
    uint32_t         maxReservedRegisterRC = 0;
    vector<uint32_t> availableRegistersRC;
    uint32_t         maxReservedRegisterRR = 0;

    uint32_t reserveDataSegmentString(const Utf8& str);
    int      reserveDataSegment(int size, void* content = nullptr);

    SpinLock            mutexDataSeg;
    vector<uint8_t>     dataSegment;
    vector<Utf8>        strBuffer;
    map<Utf8, uint32_t> mapStrBuffer;

    void setBuildPass(BuildPass buildP);

    SpinLock  mutexBuildPass;
    BuildPass buildPass = BuildPass::Full;

    void addByteCodeFunc(ByteCode* bc);

    SpinLock          mutexByteCode;
    vector<ByteCode*> byteCodeFunc;
    vector<ByteCode*> byteCodeTestFunc;

    void addDependency(AstNode* importNode);

    SpinLock         mutexDependency;
    set<string>      moduleDependenciesNames;
    vector<AstNode*> moduleDependencies;
    bool             hasBeenBuilt = false;
};

extern Pool<Module> g_Pool_module;
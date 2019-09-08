#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "BuildPass.h"
#include "BackendParameters.h"
#include "TypeTable.h"
#include "DataSegment.h"

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
    void setup(Workspace* wkp, const fs::path& pth);
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
    BackendParameters   backendParameters;
    Backend*            backend = nullptr;
    set<string>         compileVersion;

    void reserveRegisterRR(uint32_t count);
    bool executeNode(SourceFile* sourceFile, AstNode* node);
    bool executeNodeNoLock(SourceFile* sourceFile, AstNode* node, string& exception);

    SpinLock mutexRegisterRR;
    uint32_t maxReservedRegisterRR = 0;

    uint32_t reserveString(const Utf8& str);
    void     addDataSegmentInitString(uint32_t segOffset, uint32_t strIndex);
    void     addConstantSegmentInitString(uint32_t segOffset, uint32_t strIndex);
    int      reserveDataSegment(int size);
    int      reserveConstantSegment(int size);
	int      reserveConstantSegmentNoLock(int size);

    SpinLock                mutexDataSeg;
    SpinLock                mutexConstantSeg;
    SpinLock                mutexString;
    vector<uint8_t>         dataSegment;
    vector<uint8_t>         constantSegment;
    vector<Utf8>            strBuffer;
    map<uint32_t, uint32_t> strBufferDataSegInit;
    map<uint32_t, uint32_t> strBufferConstantSegInit;
    map<Utf8, uint32_t>     mapStrBuffer;

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

    TypeTable typeTable;
};

extern Pool<Module> g_Pool_module;
#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "BuildPass.h"
#include "BuildParameters.h"
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
struct Target;

struct Module : public PoolElement
{
    void setup(const string& moduleName);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);
    void error(const Utf8& msg);
    void internalError(const Utf8& msg);
    void deferReleaseChilds(AstNode* node);

    string              name;
    string              nameUp;
    atomic<int>         numErrors = 0;
    SpinLock            mutexFile;
    vector<SourceFile*> files;
    AstNode*            astRoot;
    Scope*              scopeRoot;
    BuildParameters     buildParameters;
    Backend*            backend = nullptr;
    set<string>         compileVersion;
    int                 m_exceptionCode = 0;
    bool                fromTests       = false;

    void reserveRegisterRR(uint32_t count);
    bool executeNode(SourceFile* sourceFile, AstNode* node);
    bool executeNodeNoLock(SourceFile* sourceFile, AstNode* node, bool& exception);

    SpinLock mutexRegisterRR;
    uint32_t maxReservedRegisterRR = 0;

    uint32_t            reserveString(const Utf8& str);
    SpinLock            mutexString;
    vector<Utf8>        strBuffer;
    map<Utf8, uint32_t> mapStrBuffer;

    DataSegment mutableSegment;
    DataSegment constantSegment;

    void setBuildPass(BuildPass buildP);

    SpinLock  mutexBuildPass;
    BuildPass buildPass = BuildPass::Full;

    void addByteCodeFunc(ByteCode* bc);

    SpinLock          mutexByteCode;
    vector<ByteCode*> byteCodeFunc;
    vector<ByteCode*> byteCodeTestFunc;
    vector<ByteCode*> byteCodeInitFunc;
    vector<ByteCode*> byteCodeDropFunc;
    vector<ByteCode*> byteCodeRunFunc;
    ByteCode*         byteCodeMainFunc = nullptr;
    AstNode*          mainIsDefined    = nullptr;

    void addDependency(AstNode* importNode);

    SpinLock         mutexDependency;
    set<string>      moduleDependenciesNames;
    vector<AstNode*> moduleDependencies;
    bool             hasBeenBuilt = false;

    TypeTable typeTable;
};

extern Pool<Module> g_Pool_module;
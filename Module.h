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
struct CompilerTarget;
struct SourceLocation;
struct AstFuncDecl;

struct ModuleDependency
{
    string   name;
    AstNode* node      = nullptr;
    bool     generated = false;
};

struct Module : public PoolElement
{
    void setup(const string& moduleName);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);
    void error(const Utf8& msg);
    bool internalError(const Utf8& msg);
    bool internalError(uint32_t sourceFileIdx, SourceLocation& startLocation, SourceLocation& endLocation, const Utf8& msg);
    void deferReleaseChilds(AstNode* node);

    time_t              moreRecentSourceFile;
    string              path;
    string              name;
    string              nameDown;
    string              nameUp;
    atomic<int>         numErrors = 0;
    SpinLock            mutexFile;
    vector<SourceFile*> files;
    SourceFile*         buildFile = nullptr;
    AstNode*            astRoot;
    Scope*              scopeRoot;
    BuildParameters     buildParameters;
    Backend*            backend = nullptr;
    set<string>         compileVersion;
    bool                fromTests = false;

    void reserveRegisterRR(uint32_t count);
    bool executeNode(SourceFile* sourceFile, AstNode* node);

    SpinLock mutexRegisterRR;
    uint32_t maxReservedRegisterRR = 0;

    DataSegment mutableSegment;
    DataSegment constantSegment;

    void setBuildPass(BuildPass buildP);

    SpinLock  mutexBuildPass;
    BuildPass buildPass = BuildPass::Full;

    void addByteCodeFunc(ByteCode* bc);
    void registerForeign(AstFuncDecl* node);

    SpinLock             mutexByteCode;
    vector<ByteCode*>    byteCodeFunc;
    vector<ByteCode*>    byteCodeTestFunc;
    vector<ByteCode*>    byteCodeInitFunc;
    vector<ByteCode*>    byteCodeDropFunc;
    vector<ByteCode*>    byteCodeRunFunc;
    vector<AstFuncDecl*> allForeign;
    ByteCode*            byteCodeMainFunc = nullptr;
    AstNode*             mainIsDefined    = nullptr;

    void addDependency(AstNode* importNode);

    SpinLock                      mutexDependency;
    map<string, ModuleDependency> moduleDependencies;
    bool                          hasBeenBuilt = false;

    TypeTable typeTable;
};

extern Pool<Module> g_Pool_module;
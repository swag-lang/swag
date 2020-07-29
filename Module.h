#pragma once
#include "Pool.h"
#include "BuildPass.h"
#include "BuildParameters.h"
#include "TypeTable.h"
#include "DataSegment.h"
#include "DependentJobs.h"

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
struct Token;

struct ModuleDependency
{
    string   name;
    AstNode* node      = nullptr;
    bool     generated = false;
};

static const uint32_t BUILDRES_NONE     = 0x00000000;
static const uint32_t BUILDRES_EXPORT   = 0x00000001;
static const uint32_t BUILDRES_COMPILER = 0x00000002;
static const uint32_t BUILDRES_FULL     = BUILDRES_EXPORT | BUILDRES_COMPILER;

struct Module
{
    bool setup(const Utf8& moduleName);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);
    void error(const Utf8& msg);
    bool internalError(const Utf8& msg);
    bool internalError(AstNode* node, Token& token, const Utf8& msg);

    string                    path;
    fs::path                  documentPath;
    Utf8                      name;
    Utf8                      nameDown;
    Utf8                      nameUp;
    atomic<int>               numErrors = 0;
    shared_mutex              mutexFile;
    VectorNative<SourceFile*> files;
    BuildParameters           buildParameters;
    AstNode*                  astRoot              = nullptr;
    Scope*                    scopeRoot            = nullptr;
    SourceFile*               buildFile            = nullptr;
    Backend*                  backend              = nullptr;
    uint64_t                  moreRecentSourceFile = 0;
    bool                      fromTests            = false;
    bool                      addedToBuild         = false;

    void reserveRegisterRR(uint32_t count);
    bool executeNode(SourceFile* sourceFile, AstNode* node);
    bool executeNodeNoLock(SourceFile* sourceFile, AstNode* node);
    void printUserMessage(const BuildParameters& bp);

    shared_mutex mutexRegisterRR;
    uint32_t     maxReservedRegisterRR = 0;

    DataSegment mutableSegment;
    DataSegment constantSegment;
    DataSegment bssSegment;

    void setBuildPass(BuildPass buildP);

    shared_mutex mutexBuildPass;
    BuildPass    buildPass = BuildPass::Full;

    void addByteCodeFunc(ByteCode* bc);
    void registerForeign(AstFuncDecl* node);

    DependentJobs              dependentJobs;
    shared_mutex               mutexByteCode;
    VectorNative<ByteCode*>    byteCodeFunc;
    VectorNative<ByteCode*>    byteCodeTestFunc;
    VectorNative<ByteCode*>    byteCodeInitFunc;
    VectorNative<ByteCode*>    byteCodeDropFunc;
    VectorNative<ByteCode*>    byteCodeRunFunc;
    VectorNative<AstFuncDecl*> allForeign;

    ByteCode* byteCodeMainFunc = nullptr;
    AstNode*  mainIsDefined    = nullptr;
    bool      hasUnittestError = false;

    void     addDependency(AstNode* importNode);
    void     setHasBeenBuilt(uint32_t buildResult);
    uint32_t getHasBeenBuilt();

    shared_mutex                mutexDependency;
    map<Utf8, ModuleDependency> moduleDependencies;
    uint32_t                    hasBeenBuilt = BUILDRES_NONE;

    TypeTable typeTable;

    shared_mutex     mutexGlobalVars;
    vector<AstNode*> globalVarsBss;
    vector<AstNode*> globalVarsMutable;
    void             addGlobalVar(AstNode* node, bool bss);
};

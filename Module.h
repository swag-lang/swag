#pragma once
#include "ByteCodeRunContext.h"
#include "ByteCodeRun.h"
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
struct BuildCfg;
struct SourceLocation;
struct AstFuncDecl;
struct Token;
struct Module;

struct ModuleDependency
{
    Utf8     name;
    AstNode* node       = nullptr;
    Module*  module     = nullptr;
    bool     generated  = false;
    bool     importDone = false;
};

static const uint32_t BUILDRES_NONE     = 0x00000000;
static const uint32_t BUILDRES_EXPORT   = 0x00000001;
static const uint32_t BUILDRES_COMPILER = 0x00000002;
static const uint32_t BUILDRES_FULL     = BUILDRES_EXPORT | BUILDRES_COMPILER;

struct Module
{
    bool setup(const Utf8& moduleName);
    void allocateBackend();
    void addPublicSourceFile(SourceFile* file);
    void addCompilerPassSourceFile(SourceFile* file);
    void addFileNoLock(SourceFile* file);
    void addFile(SourceFile* file);
    void removeFile(SourceFile* file);
    bool error(const Utf8& msg);
    bool internalError(const Utf8& msg);
    bool internalError(AstNode* node, Token& token, const Utf8& msg);
    bool mustOptimizeBC(AstNode* node);
    bool mustEmitSafety(AstNode* node);
    bool mustGenerateTestExe();
    bool canGenerateLegit();

    string                    path;
    fs::path                  documentPath;
    Utf8                      name;
    Utf8                      nameDown;
    Utf8                      nameUp;
    atomic<int>               numErrors      = 0;
    atomic<int>               criticalErrors = 0;
    shared_mutex              mutexFile;
    VectorNative<SourceFile*> files;
    BuildCfg                  buildCfg;
    BuildParameters           buildParameters;
    AstNode*                  astRoot              = nullptr;
    Scope*                    scopeRoot            = nullptr;
    Backend*                  backend              = nullptr;
    uint64_t                  moreRecentSourceFile = 0;
    bool                      fromTestsFolder      = false;
    bool                      fromExamplesFolder   = false;
    bool                      addedToBuild         = false;
    bool                      saveBssValues        = false;
    bool                      saveMutableValues    = false;
    bool                      bssCannotChange      = false;

    bool executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    bool executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    void printUserMessage(const BuildParameters& bp);

    DataSegment mutableSegment;
    DataSegment typeSegment;
    DataSegment constantSegment;
    DataSegment constantSegmentCompiler;
    DataSegment bssSegment;
    DataSegment tempSegment;

    void setBuildPass(BuildPass buildP);

    shared_mutex     mutexCompilerPass;
    set<SourceFile*> filesForCompilerPass;
    set<SourceFile*> publicSourceFiles;

    shared_mutex mutexBuildPass;
    BuildPass    buildPass = BuildPass::Full;

    bool sendCompilerMessage(CompilerMsgKind kind, Job* dependentJob);
    bool sendCompilerMessage(ConcreteCompilerMessage* msg, Job* dependentJob);
    void addCompilerFunc(ByteCode* bc);
    bool hasCompilerFuncFor(CompilerMsgKind kind);
    void addByteCodeFunc(ByteCode* bc);
    void registerForeign(AstFuncDecl* node);
    bool hasBytecodeToRun();
    bool WaitForDependenciesDone(Job* job);
    void printBC();
    bool compileString(const Utf8& str);

    DependentJobs              dependentJobs;
    shared_mutex               mutexByteCode;
    shared_mutex               mutexByteCodeCompiler;
    VectorNative<ByteCode*>    byteCodeCompiler[64];
    VectorNative<ByteCode*>    byteCodeFunc;
    VectorNative<ByteCode*>    byteCodeTestFunc;
    VectorNative<ByteCode*>    byteCodeInitFunc;
    VectorNative<ByteCode*>    byteCodeDropFunc;
    VectorNative<ByteCode*>    byteCodeRunFunc;
    VectorNative<ByteCode*>    byteCodePrintBC;
    VectorNative<AstFuncDecl*> allForeign;
    ByteCodeRunContext         runContext;
    ByteCodeRun                runner;
    mutex                      mutexExecuteNode;

    ByteCode* byteCodeMainFunc = nullptr;
    AstNode*  mainIsDefined    = nullptr;
    bool      isBootStrap      = false;
    bool      isRuntime        = false;
    bool      hasUnittestError = false;
    bool      setupDone        = false;
    bool      dependenciesDone = false;
    int       numConcreteBC    = 0;

    void     addForeignLib(const Utf8& text);
    void     addDependency(AstNode* importNode);
    void     setHasBeenBuilt(uint32_t buildResult);
    uint32_t getHasBeenBuilt();
    bool     mustOutputSomething();

    shared_mutex                    mutexDependency;
    VectorNative<ModuleDependency*> moduleDependencies;
    uint32_t                        hasBeenBuilt = BUILDRES_NONE;

    TypeTable typeTable;
    void*     compilerItf[2];

    shared_mutex           mutexGlobalVars;
    VectorNative<AstNode*> globalVarsBss;
    VectorNative<AstNode*> globalVarsMutable;
    void                   addGlobalVar(AstNode* node, bool bss);
};

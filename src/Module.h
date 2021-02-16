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
struct Utf8;
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

enum class GlobalVarKind
{
    Mutable,
    Bss,
    Constant,
};

enum class DependencyFetchKind
{
    FileSystem,
    Invalid,
};

struct ModuleDependency
{
    Utf8                name;
    Utf8                forceNamespace;
    Utf8                location;
    Utf8                resolvedLocation;
    Utf8                version;
    int                 verNum, revNum, buildNum;
    AstNode*            node       = nullptr;
    Module*             module     = nullptr;
    bool                importDone = false;
    DependencyFetchKind fetchKind  = DependencyFetchKind::Invalid;
};

enum class ModuleKind
{
    Example,
    Test,
    Module,
    Dependency,
    BootStrap,
    Runtime,
    Config,
};

static const uint32_t BUILDRES_NONE     = 0x00000000;
static const uint32_t BUILDRES_EXPORT   = 0x00000001;
static const uint32_t BUILDRES_COMPILER = 0x00000002;
static const uint32_t BUILDRES_FULL     = BUILDRES_EXPORT | BUILDRES_COMPILER;

struct Module
{
    static bool isValidName(const Utf8& name, Utf8& errorStr);
    void        setup(const Utf8& moduleName, const Utf8& modulePath);
    void        release();
    void        allocateBackend();
    void        addExportSourceFile(SourceFile* file);
    void        addFileNoLock(SourceFile* file);
    void        addFile(SourceFile* file);
    void        removeFile(SourceFile* file);
    bool        error(const Utf8& msg);
    bool        internalError(const Utf8& msg);
    bool        internalError(AstNode* node, Token& token, const Utf8& msg);
    bool        mustOptimizeBC(AstNode* node);
    bool        mustOptimizeBK(AstNode* node);
    bool        mustEmitSafetyOF(AstNode* node);
    bool        mustEmitSafety(AstNode* node, uint64_t whatOn, uint64_t whatOff);
    bool        mustGenerateTestExe();
    bool        canGenerateLegit();
    SourceFile* findFile(const Utf8& fileName);

    string                       path;
    Utf8                         name;
    Utf8                         nameDown;
    Utf8                         nameUp;
    ModuleKind                   kind;
    atomic<int>                  numErrors      = 0;
    atomic<int>                  criticalErrors = 0;
    shared_mutex                 mutexFile;
    shared_mutex                 mutexCompilerPass;
    VectorNative<SourceFile*>    files;
    set<SourceFile*>             exportSourceFiles;
    set<SourceFile*>             importedSourceFiles;
    map<Utf8, TypeInfoFuncAttr*> mapRuntimeFcts;
    BuildCfg                     buildCfg;
    BuildParameters              buildParameters;
    AstNode*                     astRoot   = nullptr;
    Scope*                       scopeRoot = nullptr;
    Backend*                     backend   = nullptr;
    mutex                        mutexGeneratedFile;
    FILE*                        handleGeneratedFile     = nullptr;
    uint32_t                     countLinesGeneratedFile = 0;
    uint64_t                     moreRecentSourceFile    = 0;
    bool                         addedToBuild            = false;
    bool                         saveBssValues           = false;
    bool                         saveMutableValues       = false;
    bool                         bssCannotChange         = false;
    bool                         isSwag                  = false;
    bool                         firstGenerated          = true;

    bool              executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    bool              executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    void              printUserMessage(const BuildParameters& bp);
    TypeInfoFuncAttr* getRuntimeTypeFct(const char* fctName);

    DataSegment mutableSegment;
    DataSegment typeSegment;
    DataSegment constantSegment;
    DataSegment constantSegmentCompiler;
    DataSegment bssSegment;
    DataSegment tempSegment;

    void setBuildPass(BuildPass buildP);

    shared_mutex mutexBuildPass;
    BuildPass    buildPass = BuildPass::Full;

    vector<ConcreteCompilerMessage> compilerMessages;

    bool sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob);
    bool sendCompilerMessage(ConcreteCompilerMessage* msg, Job* dependentJob);
    void postCompilerMessage(ConcreteCompilerMessage& msg);
    bool flushCompilerMessages(JobContext* context);
    void addCompilerFunc(ByteCode* bc);
    void addByteCodeFunc(ByteCode* bc);
    bool hasBytecodeToRun();
    bool WaitForDependenciesDone(Job* job);
    void printBC();
    bool compileString(const Utf8& str);
    bool hasDependencyTo(Module* module);
    void sortDependenciesByInitOrder(VectorNative<ModuleDependency*>& result);

    DependentJobs           dependentJobs;
    shared_mutex            mutexByteCode;
    shared_mutex            mutexByteCodeCompiler;
    VectorNative<ByteCode*> byteCodeCompiler[64];
    VectorNative<ByteCode*> byteCodeFunc;
    VectorNative<ByteCode*> byteCodeTestFunc;
    VectorNative<ByteCode*> byteCodeInitFunc;
    VectorNative<ByteCode*> byteCodeDropFunc;
    VectorNative<ByteCode*> byteCodeRunFunc;
    VectorNative<ByteCode*> byteCodePrintBC;
    ByteCodeRunContext      runContext;
    ByteCodeRun             runner;
    mutex                   mutexExecuteNode;

    atomic<int> numCompilerFunctions;
    ByteCode*   byteCodeMainFunc = nullptr;
    AstNode*    mainIsDefined    = nullptr;
    atomic<int> numTestErrors    = 0;
    bool        setupDone        = false;
    bool        dependenciesDone = false;

    bool              mustFetchDep = false;
    bool              wasAddedDep  = false;
    ModuleDependency* fetchDep     = nullptr;
    Utf8              remoteLocationDep;
    BuildCfg          localCfgDep;

    void     addForeignLib(const Utf8& text);
    bool     addDependency(AstNode* importNode, const Token& tokenNamespace, const Token& tokenLocation, const Token& tokenVersion);
    void     setHasBeenBuilt(uint32_t buildResult);
    uint32_t getHasBeenBuilt();
    bool     isOnlyPublic();
    bool     mustOutputSomething();

    shared_mutex                    mutexDependency;
    VectorNative<ModuleDependency*> moduleDependencies;
    uint32_t                        hasBeenBuilt = BUILDRES_NONE;

    TypeTable typeTable;
    void*     compilerItf[2];

    shared_mutex           mutexGlobalVars;
    VectorNative<AstNode*> globalVarsBss;
    VectorNative<AstNode*> globalVarsMutable;
    VectorNative<AstNode*> globalVarsConstant;
    void                   addGlobalVar(AstNode* node, GlobalVarKind varKind);

    atomic<int> optimNeedRestart;
    int         optimPass = 0;
};

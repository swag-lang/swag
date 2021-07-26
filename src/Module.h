#pragma once
#include "ByteCodeRunContext.h"
#include "ByteCodeRun.h"
#include "Pool.h"
#include "BackendParameters.h"
#include "TypeTable.h"
#include "DataSegment.h"
#include "DependentJobs.h"
#include "SymTable.h"
#include "Tokenizer.h"

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
struct Module;

enum class GlobalVarKind
{
    Mutable,
    Bss,
    Constant,
};

enum class DependencyFetchKind
{
    Swag,
    Disk,
    Invalid,
};

struct ModuleDependency
{
    Token               tokenLocation;
    Token               tokenVersion;
    Utf8                name;
    Utf8                location;
    Utf8                locationParam;
    Utf8                resolvedLocation;
    Utf8                version;
    int                 verNum, revNum, buildNum;
    AstNode*            node               = nullptr;
    Module*             module             = nullptr;
    bool                importDone         = false;
    bool                isLocalToWorkspace = false;
    DependencyFetchKind fetchKind          = DependencyFetchKind::Invalid;
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
    Script,
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
    void        addErrorModule(Module* module);

    string                    path;
    Utf8                      name;
    Utf8                      nameDown;
    Utf8                      nameUp;
    ModuleKind                kind;
    atomic<int>               numErrors      = 0;
    atomic<int>               numWarnings    = 0;
    atomic<int>               criticalErrors = 0;
    shared_mutex              mutexFile;
    shared_mutex              mutexCompilerPass;
    VectorNative<SourceFile*> files;
    VectorNative<Module*>     errorModules;
    set<SourceFile*>          exportSourceFiles;
    set<SourceFile*>          importedSourceFiles;
    map<Utf8, ByteCode*>      mapRuntimeFcts;
    BuildCfg                  buildCfg;
    BuildParameters           buildParameters;
    AstNode*                  astRoot   = nullptr;
    Scope*                    scopeRoot = nullptr;
    Backend*                  backend   = nullptr;
    mutex                     mutexGeneratedFile;
    FILE*                     handleGeneratedFile     = nullptr;
    uint32_t                  countLinesGeneratedFile = 0;
    uint64_t                  moreRecentSourceFile    = 0;
    bool                      addedToBuild            = false;
    bool                      saveBssValues           = false;
    bool                      saveMutableValues       = false;
    bool                      bssCannotChange         = false;
    bool                      isSwag                  = false;
    bool                      firstGenerated          = true;
    bool                      isLocalToWorkspace      = false;

    bool              executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    bool              executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    void              printUserMessage(const BuildParameters& bp);
    TypeInfoFuncAttr* getRuntimeTypeFct(const char* fctName);
    ByteCode*         getRuntimeFct(const char* fctName);

    DataSegment mutableSegment;
    DataSegment constantSegment;
    DataSegment bssSegment;
    DataSegment compilerSegment;
    DataSegment tlsSegment;

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
    atomic<int> numTestWarnings  = 0;
    bool        setupDone        = false;
    bool        dependenciesDone = false;

    bool              mustFetchDep = false;
    bool              wasAddedDep  = false;
    ModuleDependency* fetchDep     = nullptr;
    Utf8              remoteLocationDep;
    BuildCfg          localCfgDep;

    void     addForeignLib(const Utf8& text);
    bool     addDependency(AstNode* importNode, const Token& tokenLocation, const Token& tokenVersion);
    bool     removeDependency(AstNode* importNode);
    void     setHasBeenBuilt(uint32_t buildResult);
    uint32_t getHasBeenBuilt();
    bool     areAllFilesExported();
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

    struct forSolve
    {
        uint32_t      count;
        DependentJobs dependentJobs;
    };
    map<Utf8, forSolve> implForToSolve;
    void                addImplForToSolve(const Utf8& structName, uint32_t count = 1);
    bool                waitImplForToSolve(Job* job, TypeInfoStruct* typeStruct);
    void                decImplForToSolve(TypeInfoStruct* typeStruct);

    atomic<int> optimNeedRestart;
    int         optimPass = 0;
};

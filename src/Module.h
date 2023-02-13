#pragma once
#include "ByteCodeRunContext.h"
#include "ByteCodeRun.h"
#include "BackendParameters.h"
#include "TypeGen.h"
#include "DataSegment.h"
#include "DependentJobs.h"
#include "SymTable.h"
#include "Tokenizer.h"
#include "JobGroup.h"

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

struct SourceLocationCache
{
    SwagSourceCodeLocation loc;
    DataSegment*           storageSegment;
    uint32_t               storageOffset;
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
    AstNode*            node      = nullptr;
    Module*             module    = nullptr;
    DependencyFetchKind fetchKind = DependencyFetchKind::Invalid;
    int                 verNum, revNum, buildNum;
    bool                importDone           = false;
    bool                isLocalToWorkspace   = false;
    bool                locationAutoResolved = false;
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

struct ExecuteNodeParams
{
    VectorNative<uint64_t> callParams;
    SymbolOverload*        specReturnOpCount = nullptr;
    SymbolOverload*        specReturnOpSlice = nullptr;
    SymbolOverload*        specReturnOpDrop  = nullptr;
    uint8_t*               inheritSp         = nullptr;
    uint8_t*               inheritSpAlt      = nullptr;
    uint8_t*               inheritBp         = nullptr;
    uint8_t*               inheritStack      = nullptr;

    void* debuggerResult[2];
    bool  forDebugger  = false;
    bool  breakOnStart = false;
};

struct CompilerMessage
{
    ExportedCompilerMessage concrete;
    TypeInfo*               typeInfo;
    AstNode*                node;
};

static const uint32_t BUILDRES_NONE     = 0x00000000;
static const uint32_t BUILDRES_EXPORT   = 0x00000001;
static const uint32_t BUILDRES_COMPILER = 0x00000002;
static const uint32_t BUILDRES_PUBLISH  = 0x00000004;
static const uint32_t BUILDRES_FULL     = BUILDRES_EXPORT | BUILDRES_COMPILER | BUILDRES_PUBLISH;

struct Module
{
    static bool isValidName(const Utf8& name, Utf8& errorStr);
    void        setup(const Utf8& moduleName, const Utf8& modulePath);
    void        release();
    void        allocateBackend();
    void        initFrom(Module* other);
    void        computePublicPath();
    void        buildModulesSlice();
    void        buildGlobalVarsToDropSlice();
    void        buildTypesSlice();
    void        initProcessInfos();
    void        callPreMain();
    Utf8        getGlobalPrivFct(const Utf8& name);
    void        inheritCfgFrom(Module* from);

    void        addExportSourceFile(SourceFile* file);
    void        addFile(SourceFile* file);
    void        addFileNoLock(SourceFile* file);
    void        addErrorModule(Module* module);
    void        removeFile(SourceFile* file);
    SourceFile* findFile(const Utf8& fileName);
    ByteCode*   findBc(const Utf8& bcName);

    void printStartBuilding(const BuildParameters& bp);
    void printBC();

    bool computeExecuteResult(ByteCodeRunContext* runContext, SourceFile* sourceFile, AstNode* node, JobContext* callerContext, ExecuteNodeParams* params);
    bool executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext, ExecuteNodeParams* params = nullptr);
    bool compileString(const Utf8& str);
    bool hasBytecodeToRun();

    bool mustOptimizeBC(AstNode* node);
    bool mustOptimizeBK(AstNode* node);
    bool mustEmitSafetyOF(AstNode* node);
    bool mustEmitSafety(AstNode* node, uint16_t what);

    void setBuildPass(BuildPass buildP);

    bool sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob);
    bool sendCompilerMessage(ExportedCompilerMessage* msg, Job* dependentJob);
    bool postCompilerMessage(JobContext* context, CompilerMessage& msg);
    bool prepareCompilerMessages(JobContext* context, uint32_t pass);
    bool flushCompilerMessages(JobContext* context, uint32_t pass, Job* job);

    void              addCompilerFunc(ByteCode* bc);
    void              addByteCodeFunc(ByteCode* bc);
    void              addGlobalVar(AstNode* node, GlobalVarKind varKind);
    void              addGlobalVarToDrop(AstNode* node, uint32_t storageOffset, DataSegment* storageSegment);
    void              addForeignLib(const Utf8& text);
    TypeInfoFuncAttr* getRuntimeTypeFct(const Utf8& fctName);
    ByteCode*         getRuntimeFct(const Utf8& fctName);

    bool addInclude(AstNode* includeNode);
    bool removeInclude(AstNode* includeNode);
    bool addDependency(AstNode* importNode, const Token& tokenLocation, const Token& tokenVersion);
    bool removeDependency(AstNode* importNode);
    bool hasDependencyTo(Module* module);
    void sortDependenciesByInitOrder(VectorNative<ModuleDependency*>& result);
    bool waitForDependenciesDone(Job* job, const set<Utf8>& modules);
    bool waitForDependenciesDone(Job* job);
    bool filterFunctionsToEmit();

    bool     mustOutputSomething();
    bool     mustGenerateTestExe();
    bool     canGenerateLegit();
    void     setHasBeenBuilt(uint32_t buildResult);
    uint32_t getHasBeenBuilt();
    void     flushGenFiles();

    void addImplForToSolve(const Utf8& structName, uint32_t count = 1);
    bool waitImplForToSolve(Job* job, TypeInfoStruct* typeStruct);
    void decImplForToSolve(TypeInfoStruct* typeStruct);

    void logStage(const char* msg);

    struct ForToSolve
    {
        uint32_t      count;
        DependentJobs dependentJobs;
    };

    SharedMutex mutexDependency;
    SharedMutex mutexFile;
    SharedMutex mutexByteCode;
    Mutex       mutexGlobalVars;
    Mutex       mutexBuildPass;
    Mutex       mutexFlushGenFiles;

    string path;
    Utf8   name;
    Utf8   nameNormalized;
    Utf8   remoteLocationDep;
    Utf8   publicPath;

    BuildCfg        buildCfg;
    BuildCfg        localCfgDep;
    BuildParameters buildParameters;

    DataSegment mutableSegment;
    DataSegment constantSegment;
    DataSegment bssSegment;
    DataSegment compilerSegment;
    DataSegment tlsSegment;

    TypeGen        typeTable;
    ByteCodeRun      runner;
    DependentJobs    dependentJobs;
    SwagProcessInfos processInfos = {{0}};

    VectorNative<SourceFile*>                        files;
    VectorNative<Module*>                            errorModules;
    VectorNative<ByteCode*>                          byteCodeCompiler[(int) CompilerMsgKind::Max];
    Mutex                                            byteCodeCompilerMutex[(int) CompilerMsgKind::Max];
    Mutex                                            mutexCompilerMessages;
    Mutex                                            mutexSourceLoc;
    VectorNative<ByteCode*>                          byteCodeFunc;
    VectorNative<ByteCode*>                          byteCodeFuncToGen;
    VectorNative<ByteCode*>                          byteCodeTestFunc;
    VectorNative<ByteCode*>                          byteCodeInitFunc;
    VectorNative<ByteCode*>                          byteCodeDropFunc;
    VectorNative<ByteCode*>                          byteCodePreMainFunc;
    VectorNative<ByteCode*>                          byteCodeRunFunc;
    VectorNative<ByteCode*>                          byteCodePrintBC;
    VectorNative<ModuleDependency*>                  moduleDependencies;
    VectorNative<Module*>                            moduleEmbbeded;
    VectorNative<AstNode*>                           includes;
    VectorNative<AstNode*>                           globalVarsBss;
    VectorNative<AstNode*>                           globalVarsMutable;
    VectorNative<AstNode*>                           globalVarsConstant;
    Vector<CompilerMessage>                          compilerMessages[2];
    set<SourceFile*>                                 exportSourceFiles;
    map<Utf8, TypeInfoFuncAttr*>                     mapRuntimeFctsTypes;
    map<Utf8, ByteCode*>                             mapRuntimeFcts;
    map<Utf8, ForToSolve>                            implForToSolve;
    map<uint32_t, VectorNative<SourceLocationCache>> cacheSourceLoc;
    JobGroup                                         syntaxGroup;
    Vector<Utf8>                                     contentJobGeneratedFile;
    VectorNative<uint32_t>                           countLinesGeneratedFile;
    VectorNative<DataSegment*>                       compilerSegmentPerThread;

    struct GlobalVarToDrop
    {
        TypeInfoStruct* type;
        uint32_t        storageOffset;
        DataSegment*    storageSegment;
    };
    VectorNative<GlobalVarToDrop> globalVarsToDrop;

    AstNode*             astRoot               = nullptr;
    Scope*               scopeRoot             = nullptr;
    Backend*             backend               = nullptr;
    ByteCode*            byteCodeMainFunc      = nullptr;
    AstNode*             mainIsDefined         = nullptr;
    ModuleDependency*    fetchDep              = nullptr;
    SwagModule*          modulesSlice          = nullptr;
    SwagGlobalVarToDrop* globalVarsToDropSlice = nullptr;

    void* compilerItf[2];

    uint64_t moreRecentSourceFile = 0;

    ModuleKind  kind;
    BuildPass   buildPass                   = BuildPass::Full;
    uint32_t    hasBeenBuilt                = BUILDRES_NONE;
    uint32_t    modulesSliceOffset          = UINT32_MAX;
    uint32_t    typesSliceOffset            = UINT32_MAX;
    uint32_t    globalVarsToDropSliceOffset = UINT32_MAX;
    uint32_t    numKickedFunc               = 0;
    int         optimPass                   = 0;
    atomic<int> optimNeedRestart            = 0;
    atomic<int> numCompilerFunctions        = 0;
    atomic<int> numErrors                   = 0;
    atomic<int> numWarnings                 = 0;
    atomic<int> criticalErrors              = 0;

    bool shouldHaveError   = false;
    bool shouldHaveWarning = false;

    bool isLocalToWorkspace = false;
    bool isErrorModule      = false;
    bool isScriptFile       = false;
    bool isSwag             = false;

    bool addedToBuild         = false;
    bool saveBssValues        = false;
    bool saveMutableValues    = false;
    bool bssCannotChange      = false;
    bool dependenciesDone     = false;
    bool mustFetchDep         = false;
    bool wasAddedDep          = false;
    bool acceptsCompileString = true;
    bool acceptsCompileImpl   = true;
    bool hasCycleError        = false;
};

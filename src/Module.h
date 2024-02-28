#pragma once
#include "BackendParameters.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "DataSegment.h"
#include "DependentJobs.h"
#include "JobGroup.h"
#include "Path.h"
#include "SymTable.h"
#include "Token.h"
#include "TypeGen.h"

enum class ModuleBuildPass
{
    Init,
    Dependencies,
    Syntax,
    IncludeSwg,
    BeforeCompilerMessagesPass0,
    CompilerMessagesPass0,
    BeforeCompilerMessagesPass1,
    AfterSemantic,
    WaitForDependencies,
    FlushGenFiles,
    OptimizeBc,
    Publish,
    SemanticModule,
    RunByteCode,
    Output,
    RunNative,
    GenerateDoc,
    Done,
    Invalid
};

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
enum class ModuleBuildPass;

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
    Utf8                locationParam;
    Utf8                location;
    Path                resolvedLocation;
    Utf8                version;
    AstNode*            node      = nullptr;
    Module*             module    = nullptr;
    DependencyFetchKind fetchKind = DependencyFetchKind::Invalid;
    uint32_t            verNum, revNum, buildNum;
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
    SymbolOverload*        specReturnOpCount    = nullptr;
    SymbolOverload*        specReturnOpSlice    = nullptr;
    SymbolOverload*        specReturnOpDrop     = nullptr;
    SymbolOverload*        specReturnOpPostMove = nullptr;
    uint8_t*               inheritSp            = nullptr;
    uint8_t*               inheritSpAlt         = nullptr;
    uint8_t*               inheritBp            = nullptr;
    uint8_t*               inheritStack         = nullptr;

    void* debuggerResult[2];
    bool  forDebugger  = false;
    bool  breakOnStart = false;
    bool  forConstExpr = false;
};

struct CompilerMessage
{
    ExportedCompilerMessage concrete;
    TypeInfo*               typeInfo = nullptr;
    AstNode*                node     = nullptr;
};

constexpr uint32_t BUILDRES_NONE     = 0x00000000;
constexpr uint32_t BUILDRES_EXPORT   = 0x00000001;
constexpr uint32_t BUILDRES_COMPILER = 0x00000002;
constexpr uint32_t BUILDRES_PUBLISH  = 0x00000004;
constexpr uint32_t BUILDRES_FULL     = BUILDRES_EXPORT | BUILDRES_COMPILER | BUILDRES_PUBLISH;

struct Module
{
    static bool isValidName(const Utf8& name, Utf8& errorStr);
    void        setup(const Utf8& moduleName, const Path& modulePath);
    void        release();
    void        allocateBackend();
    void        initFrom(Module* other);
    void        computePublicPath();
    void        buildModulesSlice();
    void        buildGlobalVarsToDropSlice();
    void        buildTypesSlice();
    void        initProcessInfos();
    void        callPreMain();
    Utf8        getGlobalPrivFct(const Utf8& nameFct) const;
    void        inheritCfgFrom(const Module* from);

    void        addExportSourceFile(SourceFile* file);
    void        addFile(SourceFile* file);
    void        addFileNoLock(SourceFile* file);
    void        addErrorModule(Module* module);
    void        removeFile(SourceFile* file);
    SourceFile* findFile(const Utf8& fileName);

    void startBuilding() const;
    void printBC();

    bool computeExecuteResult(ByteCodeRunContext* runContext, SourceFile* sourceFile, AstNode* node, JobContext* callerContext, const ExecuteNodeParams* params);
    bool executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext, ExecuteNodeParams* params = nullptr);
    bool compileString(const Utf8& text);
    bool hasBytecodeToRun() const;

    bool mustOptimizeBytecode(const AstNode* node) const;
    bool mustOptimizeBackend(const AstNode* node) const;
    bool mustEmitSafetyOverflow(const AstNode* node, bool compileTime = false) const;
    bool mustEmitSafety(const AstNode* node, SafetyFlags what, bool compileTime = false) const;

    void setBuildPass(BuildPass buildP);

    bool sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob);
    bool sendCompilerMessage(ExportedCompilerMessage* msg, Job* dependentJob);
    bool postCompilerMessage(CompilerMessage& msg);
    bool prepareCompilerMessages(const JobContext* context, uint32_t pass);
    bool flushCompilerMessages(JobContext* context, uint32_t pass, Job* job);

    void              addCompilerFunc(ByteCode* bc);
    void              addByteCodeFunc(ByteCode* bc);
    void              addGlobalVar(AstNode* node, GlobalVarKind varKind);
    void              addGlobalVarToDrop(const AstNode* node, uint32_t storageOffset, DataSegment* storageSegment);
    void              addForeignLib(const Utf8& text);
    TypeInfoFuncAttr* getRuntimeTypeFct(const Utf8& fctName);
    ByteCode*         getRuntimeFct(const Utf8& fctName);

    bool addFileToLoad(AstNode* includeNode);
    bool removeFileToLoad(AstNode* includeNode);
    bool addDependency(AstNode* importNode, const Token& tokenLocation, const Token& tokenVersion);
    bool removeDependency(const AstNode* importNode);
    bool hasDependencyTo(Module* module);
    void sortDependenciesByInitOrder(VectorNative<ModuleDependency*>& result) const;
    bool waitForDependenciesDone(Job* job, const SetUtf8& modules);
    bool waitForDependenciesDone(Job* job);
    bool filterFunctionsToEmit();

    bool     mustOutputSomething() const;
    bool     mustGenerateTestExe() const;
    bool     mustGenerateLegit() const;
    void     setHasBeenBuilt(uint32_t buildResult);
    uint32_t getHasBeenBuilt();
    void     flushGenFiles();

    void addImplForToSolve(const Utf8& structName, uint32_t count = 1);
    bool waitImplForToSolve(Job* job, const TypeInfoStruct* typeStruct);
    void decImplForToSolve(const TypeInfoStruct* typeStruct);

    void logStage(const char* msg) const;
    void logPass(ModuleBuildPass pass);

    bool is(ModuleKind what) const { return kind == what; }
    bool isNot(ModuleKind what) const { return kind != what; }

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

    Path path;
    Path remoteLocationDep;
    Path publicPath;
    Utf8 name;
    Utf8 nameNormalized;

    BuildCfg        buildCfg;
    BuildCfg        localCfgDep;
    BuildParameters buildParameters;

    DataSegment mutableSegment;
    DataSegment constantSegment;
    DataSegment bssSegment;
    DataSegment compilerSegment;
    DataSegment tlsSegment;

    TypeGen          typeGen;
    ByteCodeRun      runner;
    DependentJobs    dependentJobs;
    SwagProcessInfos processInfos;

    VectorNative<SourceFile*>                        files;
    Vector<Path>                                     objFiles;
    VectorNative<Module*>                            errorModules;
    VectorNative<ByteCode*>                          byteCodeCompiler[static_cast<int>(CompilerMsgKind::Max)];
    Mutex                                            byteCodeCompilerMutex[static_cast<int>(CompilerMsgKind::Max)];
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
    VectorNative<Module*>                            moduleEmbedded;
    VectorNative<AstNode*>                           compilerLoads;
    VectorNative<AstNode*>                           globalVarsBss;
    VectorNative<AstNode*>                           globalVarsMutable;
    VectorNative<AstNode*>                           globalVarsConstant;
    Vector<CompilerMessage>                          compilerMessages[2];
    Set<SourceFile*>                                 exportSourceFiles;
    MapUtf8<TypeInfoFuncAttr*>                       mapRuntimeFctTypes;
    MapUtf8<ByteCode*>                               mapRuntimeFct;
    MapUtf8<ForToSolve>                              implForToSolve;
    Map<uint32_t, VectorNative<SourceLocationCache>> cacheSourceLoc;
    JobGroup                                         syntaxJobGroup;
    Vector<Utf8>                                     contentJobGeneratedFile;
    VectorNative<uint32_t>                           countLinesGeneratedFile;
    VectorNative<DataSegment*>                       compilerSegmentPerThread;
    Utf8                                             docComment;

    struct GlobalVarToDrop
    {
        TypeInfoStruct* type;
        uint32_t        storageOffset;
        DataSegment*    storageSegment;
        uint32_t        count;
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

    ModuleKind            kind;
    BuildPass             buildPass                   = BuildPass::Full;
    uint32_t              hasBeenBuilt                = BUILDRES_NONE;
    uint32_t              modulesSliceOffset          = UINT32_MAX;
    uint32_t              typesSliceOffset            = UINT32_MAX;
    uint32_t              globalVarsToDropSliceOffset = UINT32_MAX;
    uint32_t              optimPass                   = 0;
    std::atomic<uint32_t> optimNeedRestart            = 0;
    std::atomic<uint32_t> numCompilerFunctions        = 0;
    std::atomic<uint32_t> numErrors                   = 0;
    std::atomic<uint32_t> numWarnings                 = 0;
    std::atomic<uint32_t> criticalErrors              = 0;
    ModuleBuildPass       curPass                     = ModuleBuildPass::Invalid;

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

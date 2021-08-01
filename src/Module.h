#pragma once
#include "ByteCodeRunContext.h"
#include "ByteCodeRun.h"
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
    void        addFile(SourceFile* file);
    void        addFileNoLock(SourceFile* file);
    void        addErrorModule(Module* module);
    void        removeFile(SourceFile* file);
    SourceFile* findFile(const Utf8& fileName);

    bool error(const Utf8& msg);
    bool internalError(const Utf8& msg);
    bool internalError(AstNode* node, Token& token, const Utf8& msg);
    void printUserMessage(const BuildParameters& bp);
    void printBC();

    bool executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    bool executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext);
    bool compileString(const Utf8& str);
    bool hasBytecodeToRun();

    bool mustOptimizeBC(AstNode* node);
    bool mustOptimizeBK(AstNode* node);
    bool mustEmitSafetyOF(AstNode* node);
    bool mustEmitSafety(AstNode* node, uint64_t whatOn, uint64_t whatOff);

    void setBuildPass(BuildPass buildP);

    bool sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob);
    bool sendCompilerMessage(ConcreteCompilerMessage* msg, Job* dependentJob);
    void postCompilerMessage(ConcreteCompilerMessage& msg);
    bool flushCompilerMessages(JobContext* context);

    void              addCompilerFunc(ByteCode* bc);
    void              addByteCodeFunc(ByteCode* bc);
    void              addGlobalVar(AstNode* node, GlobalVarKind varKind);
    void              addForeignLib(const Utf8& text);
    TypeInfoFuncAttr* getRuntimeTypeFct(const char* fctName);
    ByteCode*         getRuntimeFct(const char* fctName);

    bool addDependency(AstNode* importNode, const Token& tokenLocation, const Token& tokenVersion);
    bool removeDependency(AstNode* importNode);
    bool hasDependencyTo(Module* module);
    void sortDependenciesByInitOrder(VectorNative<ModuleDependency*>& result);
    bool waitForDependenciesDone(Job* job);

    bool     areAllFilesExported();
    bool     mustOutputSomething();
    bool     mustGenerateTestExe();
    bool     canGenerateLegit();
    void     setHasBeenBuilt(uint32_t buildResult);
    uint32_t getHasBeenBuilt();

    void addImplForToSolve(const Utf8& structName, uint32_t count = 1);
    bool waitImplForToSolve(Job* job, TypeInfoStruct* typeStruct);
    void decImplForToSolve(TypeInfoStruct* typeStruct);

    struct ForToSolve
    {
        uint32_t      count;
        DependentJobs dependentJobs;
    };

    shared_mutex mutexDependency;
    shared_mutex mutexFile;
    shared_mutex mutexByteCode;
    mutex        mutexGlobalVars;
    mutex        mutexByteCodeCompiler;
    mutex        mutexBuildPass;
    mutex        mutexGeneratedFile;

    string path;
    Utf8   name;
    Utf8   nameNormalized;
    Utf8   remoteLocationDep;

    BuildCfg        buildCfg;
    BuildCfg        localCfgDep;
    BuildParameters buildParameters;

    DataSegment mutableSegment;
    DataSegment constantSegment;
    DataSegment bssSegment;
    DataSegment compilerSegment;
    DataSegment tlsSegment;

    TypeTable     typeTable;
    ByteCodeRun   runner;
    DependentJobs dependentJobs;

    VectorNative<SourceFile*>       files;
    VectorNative<Module*>           errorModules;
    VectorNative<ByteCode*>         byteCodeCompiler[64];
    VectorNative<ByteCode*>         byteCodeFunc;
    VectorNative<ByteCode*>         byteCodeTestFunc;
    VectorNative<ByteCode*>         byteCodeInitFunc;
    VectorNative<ByteCode*>         byteCodeDropFunc;
    VectorNative<ByteCode*>         byteCodeRunFunc;
    VectorNative<ByteCode*>         byteCodePrintBC;
    VectorNative<ModuleDependency*> moduleDependencies;
    VectorNative<AstNode*>          globalVarsBss;
    VectorNative<AstNode*>          globalVarsMutable;
    VectorNative<AstNode*>          globalVarsConstant;
    vector<ConcreteCompilerMessage> compilerMessages;
    set<SourceFile*>                exportSourceFiles;
    map<Utf8, ByteCode*>            mapRuntimeFcts;
    map<Utf8, ForToSolve>           implForToSolve;

    AstNode*          astRoot             = nullptr;
    Scope*            scopeRoot           = nullptr;
    Backend*          backend             = nullptr;
    FILE*             handleGeneratedFile = nullptr;
    ByteCode*         byteCodeMainFunc    = nullptr;
    AstNode*          mainIsDefined       = nullptr;
    ModuleDependency* fetchDep            = nullptr;
    void*             compilerItf[2];

    uint64_t moreRecentSourceFile = 0;

    ModuleKind  kind;
    BuildPass   buildPass               = BuildPass::Full;
    uint32_t    countLinesGeneratedFile = 0;
    uint32_t    hasBeenBuilt            = BUILDRES_NONE;
    atomic<int> numTestErrors           = 0;
    atomic<int> numTestWarnings         = 0;
    atomic<int> optimNeedRestart        = 0;
    atomic<int> numCompilerFunctions    = 0;
    atomic<int> numErrors               = 0;
    atomic<int> numWarnings             = 0;
    atomic<int> criticalErrors          = 0;
    int         optimPass               = 0;

    bool addedToBuild       = false;
    bool saveBssValues      = false;
    bool saveMutableValues  = false;
    bool bssCannotChange    = false;
    bool isSwag             = false;
    bool firstGenerated     = true;
    bool isLocalToWorkspace = false;
    bool dependenciesDone   = false;
    bool mustFetchDep       = false;
    bool wasAddedDep        = false;
};

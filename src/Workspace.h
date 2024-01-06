#pragma once
#include "ScopeSwag.h"
#include "ComputedValue.h"
#include "Mutex.h"
#include "Path.h"

struct Module;
struct Scope;
struct SourceFile;
struct AstNode;
struct BackendTarget;
struct Diagnostic;
struct ByteCode;
enum class ModuleKind;

#define SWAG_CACHE_FOLDER "swag_cache"
#define SWAG_SCRIPT_WORKSPACE "__workspace"
#define SWAG_TESTS_FOLDER "tests"
#define SWAG_MODULES_FOLDER "modules"
#define SWAG_OUTPUT_FOLDER "output"
#define SWAG_DEPENDENCIES_FOLDER "dependencies"
#define SWAG_ALIAS_FILENAME "alias"

#define SWAG_SRC_FOLDER "src"
#define SWAG_PUBLIC_FOLDER "public"
#define SWAG_PUBLISH_FOLDER "publish"

#define SWAG_CFG_FILE "module.swg"

struct OneTag
{
    Utf8          cmdLine;
    Utf8          name;
    TypeInfo*     type;
    ComputedValue value;
};

struct PendingJob
{
    Job*     pendingJob;
    AstNode* node;
};

struct Workspace

{
    Diagnostic* errorPendingJob(Job* prevJob, Job* depJob);
    void        errorPendingJobs(Vector<PendingJob>& pendingJobs);
    void        computeWaitingJobs();
    void        checkPendingJobs();
    bool        buildRTModule(Module* module);
    bool        buildTarget();
    bool        build();
    Module*     createOrUseModule(const Utf8& moduleName, const Path& modulePath, ModuleKind kind, bool errorModule = false);

    void        addBootstrap();
    void        addRuntime();
    void        addRuntimeFile(const char* fileName);
    void        setupPaths();
    void        setupInternalTags();
    void        setupUserTags();
    void        setup();
    void        computeModuleName(const Path& path, Utf8& moduleName, Path& moduleFolder, ModuleKind& kind);
    SourceFile* findFile(const char* fileName);
    Module*     getModuleByName(const Utf8& moduleName);
    void        cleanFolderContent(const Path& path);
    OneTag*     hasTag(const Utf8& name);
    void        setupCachePath();
    void        setScriptWorkspace(const Utf8& name);
    Utf8        getTargetFullName(const Utf8& buildCfg, const BackendTarget& target);
    Path        getTargetPath(const Utf8& buildCfg, const BackendTarget& target);
    void        setupTarget();

    void cleanPublic(const Path& basePath);
    void cleanScript(bool all);
    void cleanCommand();
    void newModule(const Utf8& moduleName);
    void newCommand();
    void scriptCommand();

    Path                  workspacePath;
    Path                  targetPath;
    Path                  cachePath;
    Path                  testsPath;
    Path                  modulesPath;
    Path                  dependenciesPath;
    SharedMutex           mutexModules;
    atomic<int>           numErrors   = 0;
    atomic<int>           numWarnings = 0;
    VectorNative<Module*> modules;
    Module*               runModule = nullptr;

    Vector<OneTag> tags;

    MapPath<Module*> mapFirstPassModulesNames;
    MapUtf8<Module*> mapModulesNames;
    Module*          filteredModule = nullptr;
    Module*          bootstrapModule;
    Module*          runtimeModule;
    ScopeSwag        swagScope;

    atomic<int>      skippedModules = 0;
    atomic<uint64_t> totalTime      = 0;
};

extern Workspace* g_Workspace;
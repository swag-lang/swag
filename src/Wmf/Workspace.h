#pragma once
#include "Core/Path.h"
#include "Semantic/ScopeSwag.h"
#include "Syntax/ComputedValue.h"
#include "Threading/Mutex.h"

struct Module;
struct Scope;
struct SourceFile;
struct AstNode;
struct BackendTarget;
struct Diagnostic;
struct ByteCode;
enum class ModuleKind;

#define SWAG_CACHE_FOLDER        "swag_cache"
#define SWAG_SCRIPT_WORKSPACE    "__workspace"
#define SWAG_TESTS_FOLDER        "tests"
#define SWAG_MODULES_FOLDER      "modules"
#define SWAG_OUTPUT_FOLDER       "output"
#define SWAG_DEPENDENCIES_FOLDER "dependencies"
#define SWAG_ALIAS_FILENAME      "alias"

#define SWAG_SRC_FOLDER     "src"
#define SWAG_PUBLIC_FOLDER  "public"
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
    static Diagnostic* errorPendingJob(Job* prevJob, const Job* depJob);
    static void        errorPendingJobs(const Vector<PendingJob>& pendingJobs);
    static void        computeWaitingJobs();
    static void        checkPendingJobs();
    static bool        buildRTModule(Module* module);
    bool               buildTarget();
    bool               build();
    Module*            createOrUseModule(const Utf8& moduleName, const Path& modulePath, ModuleKind kind, bool errorModule = false);

    Module*     getModuleByName(const Utf8& moduleName);
    OneTag*     hasTag(const Utf8& name);
    Path        getTargetPath() const;
    SourceFile* findFile(const char* fileName) const;
    static Utf8 getTargetFullName(const Utf8& buildCfg, const BackendTarget& target);
    static void cleanFolderContent(const Path& path);
    static void computeModuleName(const Path& path, Utf8& moduleName, Path& moduleFolder, ModuleKind& kind);
    void        addBootstrap();
    void        addRuntime();
    void        addRuntimeFile(const char* fileName) const;
    void        setScriptWorkspace(const Utf8& name);
    void        setup();
    void        setupCachePath();
    void        setupInternalTags();
    void        setupPaths();
    void        setupTarget();
    void        setupUserTags();

    static void cleanPublic(const Path& basePath);
    void        cleanScript();
    void        cleanCommand();
    void        newModule(const Utf8& moduleName) const;
    void        newCommand();
    static void formatCommand();
    static void scriptCommand();

    Path                  workspacePath;
    Path                  targetPath;
    Path                  cachePath;
    Path                  testsPath;
    Path                  modulesPath;
    Path                  dependenciesPath;
    SharedMutex           mutexModules;
    std::atomic<uint32_t> numErrors   = 0;
    std::atomic<uint32_t> numWarnings = 0;
    VectorNative<Module*> modules;
    Module*               runModule = nullptr;

    Vector<OneTag> tags;

    MapPath<Module*> mapFirstPassModulesNames;
    MapUtf8<Module*> mapModulesNames;
    Module*          filteredModule  = nullptr;
    Module*          bootstrapModule = nullptr;
    Module*          runtimeModule   = nullptr;
    ScopeSwag        swagScope;

    std::atomic<uint32_t> skippedModules = 0;
    std::atomic<uint64_t> totalTime      = 0;
};

extern Workspace* g_Workspace;

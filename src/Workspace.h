#pragma once
#include "SwagScope.h"
#include "Utf8.h"
#include "Register.h"
#include "Mutex.h"
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
#define SWAG_EXAMPLES_FOLDER "examples"
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
    void        errorPendingJobs(vector<PendingJob>& pendingJobs);
    void        computeWaitingJobs();
    void        checkPendingJobs();
    bool        buildRTModule(Module* module);
    bool        buildTarget();
    bool        build();
    Module*     createOrUseModule(const Utf8& moduleName, const Utf8& modulePath, ModuleKind kind, bool errorModule = false);

    void        addBootstrap();
    void        addRuntime();
    void        addRuntimeFile(const char* fileName);
    void        setupPaths();
    void        setupInternalTags();
    void        setupUserTags();
    void        setup();
    void        computeModuleName(const fs::path& path, Utf8& moduleName, Utf8& moduleFolder, ModuleKind& kind);
    SourceFile* findFile(const char* fileName);
    ByteCode*   findBc(const char* name);
    Module*     getModuleByName(const Utf8& moduleName);
    void        cleanFolderContent(const fs::path& path);
    OneTag*     hasTag(const Utf8& name);
    void        setupCachePath();
    void        setScriptWorkspace(const Utf8& name);
    Utf8        getTargetFullName(const string& buildCfg, const BackendTarget& target);
    fs::path    getTargetPath(const string& buildCfg, const BackendTarget& target);
    void        setupTarget();

    void cleanPublic(const fs::path& basePath);
    void cleanScript(bool all);
    void cleanCommand();
    void newModule(string moduleName);
    void newCommand();
    void scriptCommand();

    fs::path              workspacePath;
    fs::path              targetPath;
    fs::path              cachePath;
    fs::path              testsPath;
    fs::path              examplesPath;
    fs::path              modulesPath;
    fs::path              dependenciesPath;
    SharedMutex           mutexModules;
    atomic<int>           numErrors   = 0;
    atomic<int>           numWarnings = 0;
    VectorNative<Module*> modules;
    Module*               runModule = nullptr;

    vector<OneTag> tags;

    map<Utf8, Module*> mapFirstPassModulesNames;
    map<Utf8, Module*> mapModulesNames;
    set<void*>         doneErrSymbols;
    Module*            filteredModule = nullptr;
    Module*            bootstrapModule;
    Module*            runtimeModule;
    SwagScope          swagScope;
};

extern Workspace* g_Workspace;
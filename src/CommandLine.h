#pragma once
#include "Utf8.h"
#include "BackendParameters.h"

enum class BuildPass
{
    Lexer,
    Syntax,
    Semantic,
    Backend,
    Full,
};

enum class StatsWhat
{
    None,
    All,
    Count,
    Memory,
    Time,
    Module,
};

struct CommandLine
{
    // Compiler
    int       numCores      = 0;
    BuildPass buildPass     = BuildPass::Full;
    bool      rebuild       = false;
    bool      rebuildAll    = false;
    bool      run           = false;
    bool      scriptMode    = false;
    bool      scriptCommand = false;

    // Profile
    bool   profile = false;
    string profileFilter;
    int    profileChildsLevel = 0;

    // Input
    string scriptName;
    string workspacePath;
    string cachePath;
    string moduleName;
    string testFilter;

    // Test
    bool test             = false;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;

    bool warningsAsErrors = false;
    bool dbgCatch         = false;
#ifdef SWAG_DEV_MODE
    bool randomize = false;
    int  randSeed  = 0;
#endif

    // User arguments
    string                     userArguments;
    vector<Utf8>               userArgumentsVec;
    vector<pair<void*, void*>> userArgumentsStr;
    pair<void*, void*>         userArgumentsSlice;

    // Display
    StatsWhat statsWhat             = StatsWhat::None;
    bool      stats                 = false;
    bool      silent                = false;
    bool      verbose               = false;
    bool      verboseCmdLine        = false;
    bool      verbosePath           = false;
    bool      verboseLink           = false;
    bool      verboseTestErrors     = false;
    bool      verboseConcreteTypes  = false;
    bool      verboseBackendCommand = false;
    bool      verboseStages         = false;
    bool      errorSourceOut        = true;
    bool      errorNoteOut          = true;
    bool      errorMarkdown         = false;
    bool      errorAbsolute         = false;

    // Output
    string buildCfg            = "fast-debug";
    string buildCfgDebug       = "default";
    string buildCfgSafety      = "default";
    string buildCfgInlineBC    = "default";
    string buildCfgOptimBC     = "default";
    string buildCfgRemoveDupBC = "default";
    string buildCfgOptimSpeed  = "default";
    string buildCfgOptimSize   = "default";
    string buildCfgStackTrace  = "default";

    BackendTarget target;

    set<string>  tags;
    vector<Utf8> libPaths;

    uint32_t stackSizeRT = 1024 * 1024;
    uint32_t stackSizeBC = 512 * 1024;
    uint32_t maxRecurse  = 16 * 1024;

    // Backend
    BackendGenType backendGenType = BackendGenType::X64;
    bool           output         = true;
    bool           outputTest     = true;
    bool           outputLegit    = true;

    // Dependencies
    bool getDepCmd   = false;
    bool listDepCmd  = false;
    bool computeDep  = false;
    bool fetchDep    = true;
    bool getDepForce = false;

    // Clean
    bool cleanDep = false;
    bool cleanLog = false;

    fs::path exePath;
    string   exePathStr;

    bool check();
};

extern CommandLine* g_CommandLine;
#pragma once
#include "Utf8.h"
#include "BackendParameters.h"
#include "Path.h"

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
    bool profile = false;
    Utf8 profileFilter;
    int  profileChildsLevel = 0;

    // Input
    Path workspacePath;
    Path cachePath;
    Utf8 scriptName;
    Utf8 moduleName;
    Utf8 testFilter;

    // Test
    bool test             = false;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;

    bool dbgCatch = false;
    bool dbgMain  = false;
    bool dbgOff   = false;
#ifndef SWAG_DEV_MODE
    bool dbgDevMode = false;
#endif

#ifdef SWAG_DEV_MODE
    bool randomize = false;
    int  randSeed  = 0;
#endif

    // User arguments
    Utf8                       userArguments;
    Vector<Utf8>               userArgumentsVec;
    Vector<pair<void*, void*>> userArgumentsStr;
    pair<void*, void*>         userArgumentsSlice;

    // Display
    StatsWhat statsWhat             = StatsWhat::None;
    bool      stats                 = false;
    bool      silent                = false;
    bool      logColors             = true;
    bool      verbose               = false;
    bool      verboseCmdLine        = false;
    bool      verbosePath           = false;
    bool      verboseLink           = false;
    bool      verboseTestErrors     = false;
    bool      verboseConcreteTypes  = false;
    bool      verboseBackendCommand = false;
    bool      verboseStages         = false;
    bool      errorOneLine          = false;
    bool      errorCompact          = true;
    bool      errorAbsolute         = false;

    // Output
    Utf8 buildCfg            = "fast-debug";
    Utf8 buildCfgDebug       = "default";
    Utf8 buildCfgSafety      = "default";
    Utf8 buildCfgInlineBC    = "default";
    Utf8 buildCfgOptimBC     = "default";
    Utf8 buildCfgRemoveDupBC = "default";
    Utf8 buildCfgOptimSpeed  = "default";
    Utf8 buildCfgOptimSize   = "default";
    Utf8 buildCfgStackTrace  = "default";
    Utf8 buildCfgDebugAlloc  = "default";

    BackendTarget target;

    SetUtf8      tags;
    Vector<Path> libPaths;

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

    Path exePath;
    Utf8 exePathStr;

    bool check();
};

extern CommandLine g_CommandLine;
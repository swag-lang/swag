#pragma once
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

struct CommandLine
{
    // Compiler
    uint32_t  numCores      = 0;
    BuildPass buildPass     = BuildPass::Full;
    bool      rebuild       = false;
    bool      rebuildAll    = false;
    bool      run           = false;
    bool      scriptMode    = false;
    bool      scriptRun     = true;
    bool      scriptCommand = false;
    bool      genDoc        = false;

    // Stats
#ifdef SWAG_STATS
    bool     stats     = false;
    bool     statsFreq = false;
    Utf8     statsFreqOp0;
    Utf8     statsFreqOp1;
    uint32_t statsFreqCount = 100;
    bool     profile        = false;
    Utf8     profileFilter;
    int      profileChildsLevel = 0;
    float    profileMinTime     = 0.00001f;
#endif

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

    bool dbgCatch      = false;
    bool dbgMain       = false;
    bool dbgOff        = false;
    bool dbgCallStack  = false;
    bool dbgPrintBcExt = false;
#ifdef SWAG_DEV_MODE
    bool dbgDevMode = true;
#else
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
    bool silent                = false;
    bool ignoreBadParams       = false;
    bool logColors             = true;
    bool logAscii              = false;
    bool verbose               = false;
    bool verboseCmdLine        = false;
    bool verbosePath           = false;
    bool verboseLink           = false;
    bool verboseTestErrors     = false;
    bool verboseConcreteTypes  = false;
    bool verboseBackendCommand = false;
    bool verboseStages         = false;
    Utf8 verboseErrorFilter;

    bool     errorOneLine        = false;
    bool     errorAbsolute       = false;
    bool     errorSyntaxColor    = true;
    uint32_t errorRightColumn    = 80;
    float    errorSyntaxColorLum = -1.0f;

    // Output
    Utf8 buildCfg           = "fast-debug";
    Utf8 buildCfgDebug      = "default";
    Utf8 buildCfgSafety     = "default";
    Utf8 buildCfgInlineBC   = "default";
    Utf8 buildCfgOptimBC    = "default";
    Utf8 buildCfgOptim      = "default";
    Utf8 buildCfgStackTrace = "default";
    Utf8 buildCfgDebugAlloc = "default";
    Utf8 buildCfgLlvmIR     = "default";

    BackendTarget target;

    SetUtf8      tags;
    Vector<Path> libPaths;

    uint32_t limitStackRT     = 1024 * 1024;
    uint32_t limitStackBC     = 512 * 1024;
    uint32_t limitRecurseBC   = 16 * 1024;
    uint32_t limitInlineLevel = 50;

    // Backend
    BackendGenType backendGenType = BackendGenType::SCBE;
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

    // Filled
    Path exePath;
    Utf8 exePathStr;

    // Doc
    Utf8 docCss;
    Utf8 docExtension;

    bool check();
};

extern CommandLine g_CommandLine;
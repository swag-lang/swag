#pragma once
#include "BuildPass.h"
#include "Utf8.h"

enum class BackendArch
{
    X64,
};

enum class BackendOs
{
    Windows,
};

enum class BackendType
{
    LLVM,
    X64,
};

struct CommandLine
{
    // Compiler
    int       numCores   = 0;
    BuildPass buildPass  = BuildPass::Full;
    bool      rebuild    = false;
    bool      run        = false;
    bool      script     = false;
    bool      linkStatic = true;

    // Input
    string workspacePath;
    string cachePath;
    string moduleFilter;
    string fileFilter;
    string testFilter;

    // Test
    bool test             = false;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;

    bool devMode   = false;
    bool randomize = false;
    int  randSeed  = 0;

    // User arguments
    string                     userArguments;
    vector<Utf8>               userArgumentsVec;
    vector<pair<void*, void*>> userArgumentsStr;
    pair<void*, void*>         userArgumentsSlice;

    // Display
    bool silent                = false;
    bool stats                 = false;
    bool verbose               = false;
    bool verboseCmdLine        = false;
    bool verbosePath           = false;
    bool verboseLink           = false;
    bool verboseTestErrors     = false;
    bool verbosePass           = false;
    bool verboseConcreteTypes  = false;
    bool verboseBackendCommand = false;
    bool errorSourceOut        = true;
    bool errorNoteOut          = true;

    // Watch
    bool watch = false;

    // Output
    string      buildCfg           = "debug";
    string      buildCfgDebug      = "default";
    string      buildCfgSafety     = "default";
    string      buildCfgInlineBC   = "default";
    string      buildCfgOptimBC    = "default";
    string      buildCfgOptimSpeed = "default";
    string      buildCfgOptimSize  = "default";
    BackendOs   os                 = BackendOs::Windows;
    BackendArch arch               = BackendArch::X64;
    set<string> tags;

    uint32_t stackSize  = 1024 * 1024;
    uint32_t maxRecurse = 16 * 1024;

    // Backend
    BackendType backendType = BackendType::X64;
    bool        output      = true;
    bool        outputTest  = true;
    bool        outputLegit = true;

    fs::path exePath;
    string   exePathStr;

    bool check();
};

extern CommandLine g_CommandLine;
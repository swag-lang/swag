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
    C,
    LLVM,
    X64,
};

struct CommandLine
{
    // Compiler
    int       numCores  = 0;
    BuildPass buildPass = BuildPass::Full;
    bool      rebuild   = false;
    bool      run       = false;
    bool      script    = false;

    // Input
    bool   clean = false;
    string workspacePath;
    string cachePath;
    string moduleFilter;
    string fileFilter;
    string testFilter;

    // Test
    bool test             = false;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;
    bool runDocTests      = true;
    bool devMode          = false;

    // Language options
    uint32_t staticArrayMaxSize = 32 * 1024 * 1024;

    // User arguments
    string                     userArguments;
    vector<Utf8>               userArgumentsVec;
    vector<pair<void*, void*>> userArgumentsStr;
    pair<void*, void*>         userArgumentsSlice;

    // Display
    bool silent                = false;
    bool stats                 = false;
    bool verbose               = false;
    bool verboseUnittestErrors = false;
    bool verboseBackendCommand = true;
    bool errorSourceOut        = true;
    bool errorNoteOut          = true;

    // Watch
    bool watch = false;

    // Documentation
    bool generateDoc = false;

    // Output
    string      buildCfg           = "debug";
    string      buildCfgDebug      = "default";
    string      buildCfgSafety     = "default";
    string      buildCfgOptimBC    = "default";
    string      buildCfgOptimSpeed = "default";
    string      buildCfgOptimSize  = "default";
    BackendOs   os                 = BackendOs::Windows;
    BackendArch arch               = BackendArch::X64;

    // Backend
    BackendType backendType = BackendType::LLVM;
    bool        output      = true;
    bool        outputTest  = true;
    bool        outputLegit = true;

    fs::path exePath;
    string   exePathStr;
};

extern CommandLine g_CommandLine;
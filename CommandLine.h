#pragma once
#include "BuildPass.h"
#include "Utf8.h"

enum class BackendArchi
{
    Win64,
};

enum class BackendType
{
    Cl,
    Clang,
    LLVM,
};

struct CommandLine
{
    // Compiler
    int       numCores  = 0;
    BuildPass buildPass = BuildPass::Full;
    bool      rebuild   = false;

    // Input
    bool   clean        = false;
    bool   addBootstrap = true;
    string workspacePath;
    string cachePath;
    string fileFilter;
    string testFilter;

    // Test
    bool test             = false;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;
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
    bool verboseBuildPass      = true;
    bool errorSourceOut        = true;
    bool errorNoteOut          = true;

    // Watch
    bool watch = false;

    // Documentation
    bool generateDoc = false;

    // Output
    string       config = "debug";
    BackendArchi arch   = BackendArchi::Win64;

    // Backend
    BackendType backendType        = BackendType::Cl;
    bool        backendOutput      = true;
    bool        backendOutputTest  = true;
    bool        backendOutputLegit = true;
    bool        debug              = false;
    int         optim              = 0;

    fs::path exePath;
};

extern CommandLine g_CommandLine;
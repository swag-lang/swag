#pragma once
#include "BuildPass.h"

enum class BackendType
{
    C,
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
    int    tabSize = 4;

    // Test
    bool test             = false;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;

    // Language options
    uint32_t staticArrayMaxSize = 32 * 1024 * 1024;

    // User arguments
    string                     userArguments;
    vector<string>             userArgumentsVec;
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
    string config = "debug";
    string arch   = "win64";

    // Backend
    BackendType backendType        = BackendType::C;
    bool        backendOutput      = true;
    bool        backendOutputTest  = true;
    bool        backendOutputLegit = true;
    bool        debug       = false;

    fs::path exePath;
};

extern CommandLine g_CommandLine;
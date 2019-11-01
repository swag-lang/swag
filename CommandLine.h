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
    bool   cleanTarget      = true;
    bool   addRuntimeModule = true;
    string workspacePath;
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

    // Verbose
    bool help                  = false;
    bool silent                = false;
    bool stats                 = true;
    bool verbose               = false;
    bool verboseUnittestErrors = false;
    bool verboseBackendCommand = true;
    bool verboseBuildPass      = true;
    bool errorSourceOut        = true;
    bool errorNoteOut          = true;

    // Output
    string configuration = "debug";
    string platform      = "win64";

    // Backend
    BackendType backendType        = BackendType::C;
    bool        backendOutput      = true;
    bool        backendOutputTest  = true;
    bool        backendOutputLegit = true;

    string exePath;
};

extern CommandLine g_CommandLine;
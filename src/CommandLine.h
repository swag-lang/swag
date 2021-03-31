#pragma once
#include "BuildPass.h"
#include "Utf8.h"
#include "BuildParameters.h"

struct CommandLine
{
    // Compiler
    int       numCores  = 0;
    BuildPass buildPass = BuildPass::Full;
    bool      rebuild   = false;
    bool      run       = false;
    bool      script    = false;

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

    bool warningsAsErrors = false;
    bool devMode          = false;
    bool dbgCatch    = false;
    bool randomize        = false;
    int  randSeed         = 0;

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

    // Output
    string buildCfg           = "fast-debug";
    string buildCfgDebug      = "default";
    string buildCfgSafety     = "default";
    string buildCfgInlineBC   = "default";
    string buildCfgOptimBC    = "default";
    string buildCfgOptimSpeed = "default";
    string buildCfgOptimSize  = "default";
    string buildCfgStackTrace = "default";

    BackendArch   arch   = BackendArch::X86_64;
    BackendVendor vendor = BackendVendor::Pc;
    BackendOs     os     = BackendOs::Windows;
    BackendAbi    abi    = BackendAbi::Msvc;

    set<string>  tags;
    vector<Utf8> libPaths;

    uint32_t stackSize  = 1024 * 1024;
    uint32_t maxRecurse = 16 * 1024;

    // Backend
    BackendType backendType = BackendType::X64;
    bool        output      = true;
    bool        outputTest  = true;
    bool        outputLegit = true;

    // Dependencies
    bool getDepCmd  = false;
    bool listDepCmd = false;
    bool computeDep = false;
    bool fetchDep   = true;

    // Clean
    bool cleanDep = false;
    bool cleanLog = false;

    fs::path exePath;
    string   exePathStr;

    bool check();
};

extern CommandLine g_CommandLine;
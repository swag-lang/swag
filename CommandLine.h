#pragma once
#include "BuildPass.h"

struct CommandLine
{
    // Compiler
    int       numCores  = 0;
    BuildPass buildPass = BuildPass::Full;

    // Input
    bool        cleanTarget      = true;
    bool        addRuntimeModule = true;
    string      workspacePath;
    string      fileFilter;
    set<string> compileVersion;
    int         tabSize = 4;

    // Test
    bool test             = false;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;

    // Language options
    uint32_t staticArrayMaxSize = 32 * 1024 * 1024;

    // Bytecode generation
    bool bytecodeBoundCheck   = true;
    bool bytecodeAnyCastCheck = true;

    // Bytecode execution
    uint32_t                   byteCodeMaxRecurse = 1024;
    uint32_t                   byteCodeStackSize  = 16 * 1024;
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
    bool verboseBackendCommand = false;
    bool verboseTest           = true;
    bool verboseBuildPass      = true;
    bool errorSourceOut        = true;
    bool errorNoteOut          = true;

    // Backend
    bool backendOutput = true;

    string exePath;
};

extern CommandLine g_CommandLine;
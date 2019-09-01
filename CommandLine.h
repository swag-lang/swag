#pragma once
#include "BuildPass.h"

struct CommandLine
{
    bool help             = false;
    bool unittest         = true;
    bool test             = true;
    bool silent           = false;
    bool stats            = true;
    bool output           = true;
    bool errorSourceOut   = true;
    bool errorNoteOut     = true;
    bool runByteCodeTests = true;
    bool runBackendTests  = true;
    bool cleanCache       = true;

    bool debug            = false;
    bool debugBoundCheck  = true;
    bool addRuntimeModule = true;
    bool optimizeByteCode = true;

    bool verbose               = false;
    bool verboseUnittestErrors = false;
    bool verboseBackendCommand = false;
    bool verboseTest           = true;
    bool verboseBuildPass      = true;

    BuildPass   buildPass = BuildPass::Full;
    int         numCores  = 0;
    int         tabSize   = 4;
    string      fileFilter;
    set<string> compileVersion;
    uint32_t    maxStaticArraySize = 32 * 1024 * 1024;

    fs::path exePath;
};

extern CommandLine g_CommandLine;
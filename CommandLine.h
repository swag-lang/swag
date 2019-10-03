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

    bool debug           = false;
    bool debugBoundCheck = true;
    bool debugAnycast    = true;

    bool addRuntimeModule = true;

    bool verbose               = false;
    bool verboseUnittestErrors = false;
    bool verboseBackendCommand = false;
    bool verboseTest           = true;
    bool verboseBuildPass      = true;

    bool cOutputCode     = false;
    bool cOutputByteCode = false;

    BuildPass   buildPass = BuildPass::Full;
    string      fileFilter;
    set<string> compileVersion;

    int      numCores           = 0;
    int      tabSize            = 4;
    uint32_t byteCodeMaxRecurse = 1024;
    uint32_t byteCodeStackSize  = 16 * 1024;
    uint32_t staticArrayMaxSize = 32 * 1024 * 1024;

    fs::path exePath;
};

extern CommandLine g_CommandLine;
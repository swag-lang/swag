#pragma once
#include "BuildPass.h"

struct CommandLine
{
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

    bool verbose                 = false;
    bool verbose_unittest_errors = false;
    bool verbose_backend_command = false;
    bool verbose_test            = true;
    bool verbose_build_pass      = true;

    BuildPass   buildPass = BuildPass::Full;
    int         numCores  = 0;
    int         tabSize   = 4;
    string      fileFilter;
    set<string> compileVersion;

    fs::path exePath;
};

extern CommandLine g_CommandLine;
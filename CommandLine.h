#pragma once
#include "BuildPass.h"

struct CommandLine
{
    bool test             = true;
    bool silent           = false;
    bool stats            = true;
    bool output           = true;
    bool errorSourceOut   = true; // Errors will output source code
    bool errorNoteOut     = true; // Errors will output notes, if defined
    bool runByteCodeTests = true;
    bool runBackendTests  = true;

    bool verbose                 = false;
    bool verbose_unittest_errors = false;
    bool verbose_backend_command = false;
    bool verbose_test            = true;
    bool verbose_build_pass      = true;

    BuildPass buildPass = BuildPass::Full;
    int       numCores  = 0;
    int       tabSize   = 4;
    string    fileFilter;

    fs::path exePath;
};

extern CommandLine g_CommandLine;
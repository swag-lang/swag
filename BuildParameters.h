#pragma once
#include "Target.h"

enum class BackendOutputType
{
    Exe,
    Dll,
    Lib,
};

struct BuildParameters
{
    BackendOutputType type = BackendOutputType::Exe;
    Target            target;
    string            postFix;
    vector<string>    defines;
    string            destFile;
};

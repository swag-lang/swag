#pragma once
#include "Target.h"

enum class BackendType
{
    Exe,
    Dll,
    Lib,
};

struct BuildParameters
{
    BackendType    type = BackendType::Exe;
    Target         target;
    string         postFix;
    vector<string> defines;
    string         destFile;
};

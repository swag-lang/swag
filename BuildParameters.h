#pragma once
#include "Target.h"

enum class BackendOutputType
{
    Binary,
    DynamicLib,
    StaticLib,
};

struct BuildParameters
{
    BackendOutputType type = BackendOutputType::Binary;
    Target            target;
    string            postFix;
    vector<string>    defines;
    string            destFile;
};

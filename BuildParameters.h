#pragma once
#include "BuildCfg.h"
#include "Utf8.h"

enum class BackendOutputType
{
    Binary,
    DynamicLib,
    StaticLib,
};

enum BackendCompileType
{
    Normal,
    Test,
    Count,
};

struct BuildParameters
{
    BackendOutputType  outputType = BackendOutputType::Binary;
    set<Utf8>          foreignLibs;
    string             buildModeName;
    BuildCfg*          buildCfg = nullptr;
    string             postFix;
    string             outputFileName;
    int                precompileIndex = 0;
    BackendCompileType compileType;
};

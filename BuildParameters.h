#pragma once
#include "Utf8.h"
struct BuildCfg;

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
    Example,
    Count,
};

struct BuildParameters
{
    BackendOutputType  outputType = BackendOutputType::Binary;
    set<Utf8>          foreignLibs;
    BuildCfg*          buildCfg = nullptr;
    string             outputFileName;
    int                precompileIndex = 0;
    BackendCompileType compileType;
};

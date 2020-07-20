#pragma once
#include "CompilerTarget.h"
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
    string             config;
    CompilerTarget     target;
    string             postFix;
    string             destFile;
    int                precompileIndex = 0;
    BackendCompileType compileType;
};

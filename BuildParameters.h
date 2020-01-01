#pragma once
#include "CompilerTarget.h"
#include "Utf8.h"

enum class BackendOutputType
{
    Binary,
    DynamicLib,
    StaticLib,
};

static const uint32_t BUILDPARAM_FOR_TEST = 0x00000001;

struct BuildParameters
{
    BackendOutputType type = BackendOutputType::Binary;
    set<Utf8>         foreignLibs;
    string            config;
    CompilerTarget    target;
    string            postFix;
    string            destFile;
    uint32_t          flags = 0;
};

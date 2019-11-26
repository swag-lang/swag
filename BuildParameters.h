#pragma once
#include "Target.h"

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
    string            config;
    string            arch;
    Target            target;
    string            postFix;
    string            destFile;
    uint32_t          flags = 0;
};

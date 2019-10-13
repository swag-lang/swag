#pragma once
#include "CommandLine.h"

struct CommandLineBackendC
{
    bool     outputCode              = false;
    bool     outputByteCode          = false;
    uint32_t maxApplicationArguments = 64;
};

struct Target
{
    string name;

    bool                debugInformations = false;
    int                 optimizeLevel     = 0;
    CommandLineBackendC cBackend;
};

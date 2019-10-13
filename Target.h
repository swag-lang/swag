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

    // Bytecode
    bool     bytecodeBoundCheck   = true;
    bool     bytecodeAnyCastCheck = true;
    uint32_t byteCodeMaxRecurse   = 1024;
    uint32_t byteCodeStackSize    = 16 * 1024;

    // Backend
    bool                debugInformations = false;
    int                 optimizeLevel     = 0;
    CommandLineBackendC cBackend;
};

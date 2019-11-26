#pragma once
#include "CommandLine.h"

struct ParametersBackendC
{
    bool     writeSourceCode          = false;
    bool     writeByteCodeInstruction = false;
    uint32_t maxApplicationArguments  = 64;
};

struct Target
{
    // Bytecode
    bool     bytecodeBoundCheck   = true;
    bool     bytecodeAnyCastCheck = true;
    uint32_t byteCodeMaxRecurse   = 1024;
    uint32_t byteCodeStackSize    = 16 * 1024;

    // Backend
    bool               backendDebugInformations = false;
    int                backendOptimizeLevel     = 0;
    ParametersBackendC backendC;
};

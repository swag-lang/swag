#pragma once
#include "CommandLine.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct CompilerParamsBackendC
{
    bool     writeSourceCode          = false;
    bool     writeByteCodeInstruction = true;
    uint32_t maxApplicationArguments  = 64;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct CompilerTarget
{
    // Debug
    bool debugBoundCheck   = true;
    bool debugAnyCastCheck = true;
    bool debugDivZeroCheck = true;

    // Bytecode
    uint32_t byteCodeMaxRecurse = 1024;
    uint32_t byteCodeStackSize  = 16 * 1024;

    // Backend common
    bool     backendDebugInformations = false;
    uint32_t backendOptimizeLevel     = 0;

    // Backend specific
    CompilerParamsBackendC backendC;
};

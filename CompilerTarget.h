#pragma once
#include "CommandLine.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct CompilerParamsBackendC
{
    bool     writeSourceCode          = false;
    bool     writeByteCodeInstruction = false;
    uint32_t maxApplicationArguments  = 64;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct CompilerTarget
{
    // Bytecode
    bool     bytecodeBoundCheck   = true;
    bool     bytecodeAnyCastCheck = true;
    uint32_t byteCodeMaxRecurse   = 1024;
    uint32_t byteCodeStackSize    = 16 * 1024;

    // Backend common
    bool                   backendDebugInformations = false;
    uint32_t               backendOptimizeLevel     = 0;

	// Backend specific
    CompilerParamsBackendC backendC;
};

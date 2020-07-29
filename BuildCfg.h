#pragma once
#include "CommandLine.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfgBackendC
{
    uint32_t maxApplicationArguments  = 64;
    uint32_t minFunctionPerFile       = 1024;
    uint32_t maxFunctionPerFile       = 2048;
    bool     writeSourceCode          = false;
    bool     writeByteCodeInstruction = false;
};

struct BuildCfgBackendLLVM
{
    uint32_t minFunctionPerFile = 256;
    uint32_t maxFunctionPerFile = 1024;
    bool     outputIR           = false; // Write a 'file.ir' text file just next to the output file
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfg
{
    // Debug
    bool guardBoundCheck = true;
    bool guardAnyCast    = true;
    bool guardDivZero    = true;

    // Bytecode
    uint32_t byteCodeMaxRecurse = 1024;
    uint32_t byteCodeStackSize  = 16 * 1024;

    // Backend common
    bool     backendDebugInformations = false;
    uint32_t backendOptimizeLevel     = 0;

    // Specific backend parameters
    BuildCfgBackendC    backendC;
    BuildCfgBackendLLVM backendLLVM;
};

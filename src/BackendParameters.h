#pragma once
#include "Utf8.h"
#include "Runtime.h"
struct BuildCfg;

const uint32_t SWAG_LIMIT_ARRAY_SIZE     = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_COMPILER_LOAD  = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_SEGMENT        = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_MIN_STACK      = 1024;
const uint32_t SWAG_LIMIT_MAX_STACK      = 16 * 1024 * 1024;
const uint32_t SWAG_LIMIT_CB_MAX_PARAMS  = 6;
const uint32_t SWAG_LIMIT_CLOSURE_SIZEOF = 8 * sizeof(void*);

struct BackendTarget
{
    SwagTargetArch arch = SwagTargetArch::X86_64;
    SwagTargetOs   os   = SwagTargetOs::Windows;
};

enum class BackendGenType
{
    LLVM,
    X64,
};

enum class BackendObjType
{
    Coff,
    Elf,
    MachO,
    Wasm,
};

enum BackendCompileType
{
    Normal,
    Test,
    Example,
    Count,
};

inline bool isOsDarwin(SwagTargetOs os)
{
    switch (os)
    {
    case SwagTargetOs::MacOSX:
        return true;
    }

    return false;
}

inline bool isArchArm(SwagTargetArch arch)
{
    switch (arch)
    {
    case SwagTargetArch::X86_64:
        return false;
    }

    return true;
}

struct BuildParameters
{
    set<Utf8>              foreignLibs;
    VectorNative<AstNode*> globalUsings;
    Utf8                   outputFileName;
    BuildCfg*              buildCfg        = nullptr;
    int                    precompileIndex = 0;
    BackendCompileType     compileType     = BackendCompileType::Normal;
};

#pragma once
#include "Utf8.h"
#include "Runtime.h"
#include "Set.h"
struct BuildCfg;

const uint32_t SWAG_LIMIT_ARRAY_SIZE          = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_COMPILER_LOAD       = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_SEGMENT             = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_MIN_STACK           = 1024;
const uint32_t SWAG_LIMIT_MAX_STACK           = 16 * 1024 * 1024;
const uint32_t SWAG_LIMIT_PAGE_STACK          = 0x1000;
const uint32_t SWAG_LIMIT_MAX_VARIADIC_PARAMS = 255;
const uint32_t SWAG_LIMIT_CB_MAX_PARAMS       = 6;
const uint32_t SWAG_LIMIT_CLOSURE_SIZEOF      = 8 * sizeof(void*);

struct BackendTarget
{
    SwagTargetArch arch = SwagTargetArch::X86_64;
    SwagTargetOs   os   = SwagTargetOs::Windows;
    Utf8           cpu;
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
    default:
        break;
    }

    return false;
}

inline bool isArchArm(SwagTargetArch arch)
{
    switch (arch)
    {
    case SwagTargetArch::X86_64:
        return false;
    default:
        break;
    }

    return true;
}

struct BuildParameters
{
    SetUtf8                foreignLibs;
    VectorNative<AstNode*> globalUsings;
    Utf8                   outputFileName;
    BuildCfg*              buildCfg        = nullptr;
    int                    precompileIndex = 0;
    BackendCompileType     compileType     = BackendCompileType::Normal;

    bool isDebug() const
    {
        return buildCfg->backendOptimize == BuildCfgBackendOptim::O0;
    }
};

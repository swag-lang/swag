#pragma once
#include "Utf8.h"
struct BuildCfg;

const uint32_t SWAG_LIMIT_ARRAY_SIZE    = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_COMPILER_LOAD = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_SEGMENT       = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_MIN_STACK     = 1024;
const uint32_t SWAG_LIMIT_MAX_STACK     = 16 * 1024 * 1024;
const uint32_t SWAG_LIMIT_CB_MAX_PARAMS = 6;

enum class TargetArch
{
    X86_64,
};

enum class TargetOs
{
    Windows,
    Linux,
    MacOSX,
};

struct BackendTarget
{
    TargetArch arch = TargetArch::X86_64;
    TargetOs   os   = TargetOs::Windows;
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

inline bool isOsDarwin(TargetOs os)
{
    switch (os)
    {
    case TargetOs::MacOSX:
        return true;
    }

    return false;
}

inline bool isArchArm(TargetArch arch)
{
    switch (arch)
    {
    case TargetArch::X86_64:
        return false;
    }

    return true;
}

struct BuildParameters
{
    set<Utf8>          foreignLibs;
    Utf8               outputFileName;
    BuildCfg*          buildCfg        = nullptr;
    int                precompileIndex = 0;
    BackendCompileType compileType     = BackendCompileType::Normal;
};

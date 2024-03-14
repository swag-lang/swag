#pragma once
#include "Backend/Runtime.h"

struct AstNode;
struct BuildCfg;
struct Module;

constexpr uint32_t SWAG_LIMIT_ARRAY_SIZE          = 0x7FFFFFFF;
constexpr uint32_t SWAG_LIMIT_COMPILER_LOAD       = 0x7FFFFFFF;
constexpr uint32_t SWAG_LIMIT_SEGMENT             = 0x7FFFFFFF;
constexpr uint32_t SWAG_LIMIT_MIN_STACK           = 1024;
constexpr uint32_t SWAG_LIMIT_MAX_STACK           = 16 * 1024 * 1024;
constexpr uint32_t SWAG_LIMIT_PAGE_STACK          = 0x1000;
constexpr uint32_t SWAG_LIMIT_MAX_VARIADIC_PARAMS = 255;
constexpr uint32_t SWAG_LIMIT_CB_MAX_PARAMS       = 6;
constexpr uint32_t SWAG_LIMIT_CLOSURE_SIZEOF      = 8 * sizeof(void*);

struct BackendTarget
{
    SwagTargetArch arch = SwagTargetArch::X86_64;
    SwagTargetOs   os   = SwagTargetOs::Windows;
    Utf8           cpu;
};

enum class BackendGenType
{
    LLVM,
    SCBE,
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
        case SwagTargetOs::MacOsX:
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
    SetUtf8                foreignLibs;
    VectorNative<AstNode*> globalUsing;
    Module*                module          = nullptr;
    BuildCfg*              buildCfg        = nullptr;
    uint32_t               precompileIndex = 0;
    BackendCompileType     compileType     = Normal;

    bool isDebug() const
    {
        return buildCfg->backendOptimize == BuildCfgBackendOptim::O0;
    }
};

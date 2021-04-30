#pragma once
#include "Utf8.h"
struct BuildCfg;

enum class BackendArch
{
    X86_64,
};

enum class BackendOs
{
    Windows,
    Linux,
    MacOSX,
};

enum class BackendVendor
{
    Pc,
};

enum class BackendAbi
{
    Msvc,
    Gnu,
};

enum class BackendType
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

inline bool isAbiGnu(BackendAbi abi)
{
    switch (abi)
    {
    case BackendAbi::Gnu:
        return true;
    }

    return false;
}

inline bool isOsDarwin(BackendOs os)
{
    switch (os)
    {
    case BackendOs::MacOSX:
        return true;
    }

    return false;
}

inline bool isArchArm(BackendArch arch)
{
    switch (arch)
    {
    case BackendArch::X86_64:
        return false;
    }

    return true;
}

struct BuildParameters
{
    set<Utf8>          foreignLibs;
    string             outputFileName;
    BuildCfg*          buildCfg        = nullptr;
    int                precompileIndex = 0;
    BackendCompileType compileType     = BackendCompileType::Normal;
};

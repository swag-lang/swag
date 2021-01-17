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

enum class BackendOutputType
{
    Binary,
    DynamicLib,
    StaticLib,
};

enum BackendCompileType
{
    Normal,
    Test,
    Example,
    Count,
};

struct BuildParameters
{
    BackendOutputType  outputType = BackendOutputType::Binary;
    set<Utf8>          foreignLibs;
    BuildCfg*          buildCfg = nullptr;
    string             outputFileName;
    int                precompileIndex = 0;
    BackendCompileType compileType;
};

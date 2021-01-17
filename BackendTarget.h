#pragma once

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

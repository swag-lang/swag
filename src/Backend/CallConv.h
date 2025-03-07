#pragma once

struct TypeInfo;
struct TypeInfoFuncAttr;

enum class CallConvKind
{
    X86_64,
    ByteCode,
    Swag,
    Max
};

// ReSharper disable CppInconsistentNaming
enum class CPUReg : uint8_t
{
    RAX,
    RBX,
    RCX,
    RDX,
    RSP,
    RBP,
    RSI,
    RDI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    XMM0,
    XMM1,
    XMM2,
    XMM3,
    RIP,
    Max,
};
// ReSharper restore CppInconsistentNaming

struct CallConv
{
    constexpr static uint32_t MAX_CALL_CONV_REGISTERS = 4;
    static const CallConv*    get(CallConvKind kind);

    // The number of parameters to pass by register
    uint32_t paramByRegisterCount = 4;

    // Stack align
    uint32_t stackAlign = 16;

    // The registers to use when passing parameter by register, and if it's integer
    VectorNative<CPUReg> paramByRegisterInteger;

    // The registers to use when passing parameter by register, and if it's float
    VectorNative<CPUReg> paramByRegisterFloat;

    // The register used to return an integer
    CPUReg returnByRegisterInteger = CPUReg::RAX;

    // The register used to return a float
    CPUReg returnByRegisterFloat = CPUReg::XMM0;

    // The base register used for ffi
    CPUReg ffiBaseRegister = CPUReg::RDI;

    // If a float is passed by register, use 'paramByRegisterFloat' instead of 'paramByRegisterInteger'
    bool useRegisterFloat = true;

    // If a const struct fits in a register, pass it by value instead of by pointer
    bool structParamByRegister = true;

    // Returns a struct by register if it fits
    bool structReturnByRegister = true;
};

extern CallConv g_CallConv[static_cast<int>(CallConvKind::Max)];
extern void     initCallConvKinds();

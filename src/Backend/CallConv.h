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
enum class CpuReg : uint8_t
{
    RAX,
    RBX,
    RCX,
    RDX,
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
    RSI,
    RDI,
    RSP,
    RBP,
    RIP,
    Max,
};
// ReSharper restore CppInconsistentNaming

struct CallConv
{
    constexpr static uint32_t MAX_CALL_CONV_REGISTERS = 4;
    static const CallConv*    get(CallConvKind kind);
    static CpuReg             getVolatileRegister(const CallConv& ccCaller, const CallConv& ccCallee);

    // The number of parameters to pass by register
    uint32_t paramByRegisterCount = 4;

    // Stack align
    uint32_t stackAlign = 16;

    // The registers to use when passing parameter by register, and if it's integer
    VectorNative<CpuReg> paramByRegisterInteger;

    // The registers to use when passing parameter by register, and if it's float
    VectorNative<CpuReg> paramByRegisterFloat;

    // All registers considered as volatile
    VectorNative<CpuReg> volatileRegisters;

    // All registers considered as nonvolatile
    VectorNative<CpuReg> nonVolatileRegisters;

    // The register used to return an integer
    CpuReg returnByRegisterInteger = CpuReg::RAX;

    // The register used to return a float
    CpuReg returnByRegisterFloat = CpuReg::XMM0;

    // The base register used for ffi
    CpuReg ffiBaseRegister = CpuReg::RDI;

    CpuReg cpuReg0 = CpuReg::RAX;
    CpuReg cpuReg1 = CpuReg::RCX;

    // If a float is passed by register, use 'paramByRegisterFloat' instead of 'paramByRegisterInteger'
    bool useRegisterFloat = true;

    // If a const struct fits in a register, pass it by value instead of by pointer
    bool structParamByRegister = true;

    // Returns a struct by register if it fits
    bool structReturnByRegister = true;
};

extern CallConv g_CallConv[static_cast<int>(CallConvKind::Max)];
extern void     initCallConvKinds();

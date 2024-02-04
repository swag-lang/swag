#pragma once

struct TypeInfo;
struct TypeInfoFuncAttr;

enum CallConvKind
{
    Swag,
    Max
};

enum CPURegister : uint8_t
{
    RAX  = 0b0000,
    RBX  = 0b0011,
    RCX  = 0b0001,
    RDX  = 0b0010,
    RSP  = 0b0100,
    RBP  = 0b0101,
    RSI  = 0b0110,
    RDI  = 0b0111,
    R8   = 0b1000,
    R9   = 0b1001,
    R10  = 0b1010,
    R11  = 0b1011,
    R12  = 0b1100,
    R13  = 0b1101,
    R14  = 0b1110,
    R15  = 0b1111,
    XMM0 = 0b0000,
    XMM1 = 0b0001,
    XMM2 = 0b0010,
    XMM3 = 0b0011,
    RIP  = 0b10000,
};

struct CallConv
{
    // The number of parameters to pass by register
    uint32_t paramByRegisterCount = 4;

    // The registers to use when passing parameter by register, and if it's integer
    VectorNative<CPURegister> paramByRegisterInteger;

    // The registers to use when passing parameter by register, and if it's float
    VectorNative<CPURegister> paramByRegisterFloat;

    // If a float is passed by register, use 'paramByRegisterFloat' instead of 'paramByRegisterInteger'
    bool useRegisterFloat = true;

    // If a const struct fits in a register, pass it by value instead of by pointer
    bool structParamByRegister = true;

    // Returns a struct by register if it fits
    bool structReturnByRegister = true;

    // The register used to return an integer
    CPURegister returnByRegisterInteger = CPURegister::RAX;

    // The register used to return a float
    CPURegister returnByRegisterFloat = CPURegister::XMM0;

    // Scratch registers used to optimized generation (x64 backend)
    CPURegister firstScratchRegister = CPURegister::R12;
    uint32_t    numScratchRegisters  = 4;

    static bool structParamByValue(TypeInfoFuncAttr* typeFunc, TypeInfo* typeParam);
    static bool returnByAddress(TypeInfoFuncAttr* typeFunc);
    static bool returnByStackAddress(TypeInfoFuncAttr* typeFunc);
    static bool returnNeedsStack(TypeInfoFuncAttr* typeFunc);
    static bool returnByValue(TypeInfoFuncAttr* typeFunc);
    static bool returnStructByValue(TypeInfoFuncAttr* typeFunc);
};

extern CallConv g_CallConv[CallConvKind::Max];
extern void     initCallConvKinds();

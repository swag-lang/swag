#pragma once
#include "Backend/RegisterSet.h"

struct TypeInfo;
struct TypeInfoFuncAttr;

enum class CallConvKind
{
    X86_64,
    Compiler,
    Swag,
    Max
};

enum class CpuReg : uint8_t
{
    Rax,
    Rbx,
    Rcx,
    Rdx,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    Xmm0,
    Xmm1,
    Xmm2,
    Xmm3,
    Rsi,
    Rdi,
    Rsp,
    Rbp,
    Rip,
    Max,
};

using VolatileFlags                                  = Flags<uint32_t>;
static constexpr VolatileFlags VF_ZERO               = 0x00000000;
static constexpr VolatileFlags VF_EXCLUDE_COMPUTE_I0 = 0x00000001;
static constexpr VolatileFlags VF_EXCLUDE_COMPUTE_I1 = 0x00000002;
static constexpr VolatileFlags VF_EXCLUDE_COMPUTE_I2 = 0x00000004;
static constexpr VolatileFlags VF_EXCLUDE_COMPUTE    = VF_EXCLUDE_COMPUTE_I0 | VF_EXCLUDE_COMPUTE_I1 | VF_EXCLUDE_COMPUTE_I2;
static constexpr VolatileFlags VF_EXCLUDE_PARAM0     = 0x00000008;
static constexpr VolatileFlags VF_EXCLUDE_PARAM1     = 0x00000010;
static constexpr VolatileFlags VF_EXCLUDE_PARAM2     = 0x00000020;
static constexpr VolatileFlags VF_EXCLUDE_PARAM3     = 0x00000040;
static constexpr VolatileFlags VF_EXCLUDE_PARAMS     = VF_EXCLUDE_PARAM0 | VF_EXCLUDE_PARAM1 | VF_EXCLUDE_PARAM2 | VF_EXCLUDE_PARAM3;
static constexpr VolatileFlags VF_EXCLUDE_RETURN     = 0x00000080;

struct CallConv
{
    constexpr static uint32_t MAX_CALL_CONV_REGISTERS = 4;
    static const CallConv*    get(CallConvKind kind);
    static CpuReg             getVolatileRegisterInteger(const CallConv* ccCaller, const CallConv* ccCallee, VolatileFlags flags);
    void                      compute();

    // Stack align
    uint32_t stackAlign = 16;

    // The registers to use when passing parameters by register
    Vector<CpuReg> paramsRegistersInteger;
    Vector<CpuReg> paramsRegistersFloat;

    // All registers considered as volatile
    Vector<CpuReg> volatileRegistersInteger;
    Vector<CpuReg> volatileRegistersFloat;

    // All registers considered as nonvolatile
    Vector<CpuReg> nonVolatileRegistersInteger;
    Vector<CpuReg> nonVolatileRegistersFloat;

    // The register used to return an integer or a float
    CpuReg returnByRegisterInteger = CpuReg::Rax;
    CpuReg returnByRegisterFloat   = CpuReg::Xmm0;

    // The base register used for ffi
    CpuReg ffiBaseRegister = CpuReg::Max;

    // Base computing registers
    CpuReg computeRegI0 = CpuReg::Max;
    CpuReg computeRegI1 = CpuReg::Max;
    CpuReg computeRegI2 = CpuReg::Max;
    CpuReg computeRegF0 = CpuReg::Max;
    CpuReg computeRegF1 = CpuReg::Max;
    CpuReg computeRegF2 = CpuReg::Max;

    // If a float is passed by register, use 'paramByRegisterFloat' instead of 'paramByRegisterInteger'
    bool useRegisterFloat = true;

    // If a const struct fits in a register, pass it by value instead of by pointer
    bool structParamByRegister = true;

    // Returns a struct by register if it fits
    bool structReturnByRegister = true;

    // Cache
    RegisterSet paramsRegistersIntegerSet;
    RegisterSet paramsRegistersFloatSet;
    RegisterSet volatileRegistersIntegerSet;
    RegisterSet volatileRegistersFloatSet;
    RegisterSet nonVolatileRegistersIntegerSet;
};

extern CallConv g_CallConv[static_cast<int>(CallConvKind::Max)];
extern void     initCallConvKinds();

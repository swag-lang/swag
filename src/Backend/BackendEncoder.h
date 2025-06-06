#pragma once
#include "BackendParameters.h"
#include "CallConv.h"

struct CpuFunction;
struct Module;
struct TypeInfo;
struct ByteCodeInstruction;
struct CallConv;
enum class ByteCodeOp : uint16_t;

enum class BackendPreCompilePass
{
    Init,
    FunctionBodies,
    End,
    GenerateObj,
    Release,
};

enum class OpBits : uint8_t
{
    Zero = 0,
    B8   = 8,
    B16  = 16,
    B32  = 32,
    B64  = 64,
    B128 = 128,
};

struct BackendEncoder
{
    virtual ~BackendEncoder() = default;
    virtual void init(const BuildParameters& buildParameters);

    static bool mustCheckOverflow(const Module* module, const ByteCodeInstruction* ip);

    static uint32_t  getNumBits(OpBits opBits);
    static uint32_t  getNumBytes(OpBits opBits) { return getNumBits(opBits) / 8; }
    static OpBits    getOpBitsByBytes(uint32_t numBytes);
    static bool      isFloat(ByteCodeOp op);
    static bool      isInt(ByteCodeOp op);
    static OpBits    getOpBits(ByteCodeOp op);
    static TypeInfo* getOpType(ByteCodeOp op);
    static uint32_t  getNumBits(ByteCodeOp op) { return getNumBits(getOpBits(op)); }
    static uint32_t  getNumBytes(ByteCodeOp op) { return getNumBits(getOpBits(op)) / 8; }
    static bool      isFloat(CpuReg reg) { return reg == CpuReg::Xmm0 || reg == CpuReg::Xmm1 || reg == CpuReg::Xmm2 || reg == CpuReg::Xmm3; }
    static bool      isInt(CpuReg reg) { return !isFloat(reg); }
    static void      maskValue(uint64_t& value, OpBits opBits);

    bool isFuncReturnRegister(CpuReg reg) const;
    bool isFuncParameterRegister(CpuReg reg) const;

    BuildParameters       buildParams;
    Utf8                  filename;
    ByteCodeInstruction*  ip      = nullptr;
    CpuFunction*          cpuFct  = nullptr;
    int32_t               ipIndex = 0;
    Module*               module  = nullptr;
    BackendPreCompilePass pass    = {BackendPreCompilePass::Init};
};

#pragma once
#include "BackendParameters.h"

struct CPUFunction;
struct Module;
struct TypeInfo;
struct ByteCodeInstruction;
enum class ByteCodeOp : uint16_t;

enum class BackendPreCompilePass
{
    Init,
    FunctionBodies,
    End,
    GenerateObj,
    Release,
};

enum class OpBits : uint32_t
{
    Invalid = 0,
    B8      = 8,
    B16     = 16,
    B32     = 32,
    B64     = 64,
    F32     = 65,
    F64     = 66,
};

struct BackendEncoder
{
    BuildParameters       buildParams;
    Utf8                  filename;
    ByteCodeInstruction*  ip      = nullptr;
    CPUFunction*          cpuFct  = nullptr;
    int32_t               ipIndex = 0;
    Module*               module  = nullptr;
    BackendPreCompilePass pass    = {BackendPreCompilePass::Init};

    void init(const BuildParameters& buildParameters);

    static uint32_t  getNumBits(OpBits opBits);
    static OpBits    getOpBitsByBytes(uint32_t numBytes, bool forFloat = false);
    static OpBits    getOpBits(ByteCodeOp op);
    static TypeInfo* getOpType(ByteCodeOp op);
    static bool      mustCheckOverflow(const Module* module, const ByteCodeInstruction* ip);

    static uint32_t getNumBits(ByteCodeOp op) { return getNumBits(getOpBits(op)); }
    static uint32_t getNumBytes(ByteCodeOp op) { return getNumBits(getOpBits(op)) / 8; }
    static bool     isInt(OpBits opBits) { return opBits == OpBits::B8 || opBits == OpBits::B16 || opBits == OpBits::B32 || opBits == OpBits::B64; }
    static bool     isFloat(OpBits opBits) { return opBits == OpBits::F32 || opBits == OpBits::F64; }
};

#pragma once
struct Module;
struct TypeInfo;
enum class ByteCodeOp : uint16_t;

enum class BackendPreCompilePass
{
    Init,
    FunctionBodies,
    End,
    GenerateObj,
    Release,
};

enum class CPUBits : uint32_t
{
    INVALID = 0,
    B8      = 8,
    B16     = 16,
    B32     = 32,
    B64     = 64,
    F32     = 65,
    F64     = 66,
};

struct BackendEncoder
{
    Utf8                  filename;
    Module*               module = nullptr;
    BackendPreCompilePass pass   = {BackendPreCompilePass::Init};

    static uint32_t  getBitsCount(CPUBits numBits);
    static CPUBits   getCPUBits(ByteCodeOp op);
    static TypeInfo* getCPUType(ByteCodeOp op);

    static bool isInt(CPUBits numBits) { return numBits == CPUBits::B8 || numBits == CPUBits::B16 || numBits == CPUBits::B32 || numBits == CPUBits::B64; }
    static bool isFloat(CPUBits numBits) { return numBits == CPUBits::F32 || numBits == CPUBits::F64; }
};

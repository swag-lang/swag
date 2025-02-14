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

enum class OpBits : uint32_t
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

    static uint32_t  getBitsCount(OpBits opBits);
    static OpBits   getCPUBits(ByteCodeOp op);
    static TypeInfo* getCPUType(ByteCodeOp op);

    static bool isInt(OpBits opBits) { return opBits == OpBits::B8 || opBits == OpBits::B16 || opBits == OpBits::B32 || opBits == OpBits::B64; }
    static bool isFloat(OpBits opBits) { return opBits == OpBits::F32 || opBits == OpBits::F64; }
};

#pragma once
#include "Pool.h"
#include "Concat.h"
#include "Utf8.h"
#include "Register.h"
enum class ByteCodeOp : uint16_t;

struct ByteCodeInstruction
{
    ByteCodeOp op;
    uint32_t   r0;
    uint32_t   r1;
    union {
        uint32_t r2;
        Register value;
    };
};

struct ByteCode : public PoolElement
{
    Concat       out;
    bool         hasDebugInfos = true;
    vector<Utf8> strBuffer;
};

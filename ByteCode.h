#pragma once
#include "Pool.h"
#include "Concat.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "Register.h"
enum class ByteCodeOp : uint16_t;

struct ByteCodeInstruction
{
    Register       a;
    Register       b;
    Register       c;
    uint32_t       sourceFileIdx;
    SourceLocation startLocation;
    SourceLocation endLocation;
    ByteCodeOp     op;
};

struct ByteCode : public PoolElement
{
    ByteCodeInstruction* out             = nullptr;
    uint32_t             numInstructions = 0;
    uint32_t             maxInstructions = 0;
    vector<Utf8>         strBuffer;
};

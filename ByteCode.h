#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "Register.h"
enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;

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
    void print();

    static const int     ALIGN_RIGHT_OPCODE = 20;
    ByteCodeInstruction* out                = nullptr;
    uint32_t             numInstructions    = 0;
    uint32_t             maxInstructions    = 0;
    int                  maxCallParameters  = 0;
    vector<Utf8>         strBuffer;
    SourceFile*          sourceFile;
    AstNode*             node;

    set<int> usedRegisters;
};

extern Pool<ByteCode> g_Pool_byteCode;
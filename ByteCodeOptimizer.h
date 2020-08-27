#pragma once
#include "ByteCode.h"
#include "ByteCodeOp.h"
struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;

struct ByteCodeOptContext
{
    ByteCode*                          bc;
    VectorNative<ByteCodeInstruction*> jumps;
    VectorNative<ByteCodeInstruction*> nops;
};

struct ByteCodeOptimizer
{
    inline static void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip)
    {
        ip->op = ByteCodeOp::Nop;
        context->nops.push_back(ip);
    }

    static bool optimizeJumps(ByteCodeOptContext* context);

    static bool setJumps(ByteCodeOptContext* context);
    static bool removeNops(ByteCodeOptContext* context);

    static void optimize(ByteCodeGenContext* context);
};
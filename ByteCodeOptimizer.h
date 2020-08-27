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
    VectorNative<ByteCodeInstruction*> toDo;
    bool                               allPassesHaveDoneSomething = false;
    bool                               passHasDoneSomething       = false;
};

struct ByteCodeOptimizer
{
    inline static void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip)
    {
        if (ip->op == ByteCodeOp::Nop)
            return;
        SWAG_ASSERT(ip->op != ByteCodeOp::End);
        context->passHasDoneSomething = true;
        ip->op                        = ByteCodeOp::Nop;
        context->nops.push_back(ip);
    }

    static void optimizePassJumps(ByteCodeOptContext* context);
    static void optimizePassDeadCode(ByteCodeOptContext* context);

    static void setJumps(ByteCodeOptContext* context);
    static void removeNops(ByteCodeOptContext* context);

    static void optimize(ByteCodeGenContext* context);
};
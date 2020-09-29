#pragma once
#include "ByteCode.h"
#include "ByteCodeOp.h"
struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Module;

struct ByteCodeOptContext
{
    ByteCode*                          bc;
    VectorNative<ByteCodeInstruction*> jumps;
    VectorNative<ByteCodeInstruction*> nops;
    VectorNative<ByteCodeInstruction*> tmpBufInst;
    VectorNative<uint64_t>             tmpBufU64;
    bool                               allPassesHaveDoneSomething = false;
    bool                               passHasDoneSomething       = false;
    bool                               hasError                   = false;
    ByteCodeGenContext*                semContext                 = nullptr;
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

    inline static bool isJump(ByteCodeInstruction* inst)
    {
        return inst->op == ByteCodeOp::Jump ||
               inst->op == ByteCodeOp::JumpIfTrue ||
               inst->op == ByteCodeOp::JumpIfFalse ||
               inst->op == ByteCodeOp::JumpIfZero64 ||
               inst->op == ByteCodeOp::JumpIfNotZero64 ||
               inst->op == ByteCodeOp::JumpIfFalse ||
               inst->op == ByteCodeOp::JumpIfZero32 ||
               inst->op == ByteCodeOp::JumpIfNotZero32;
    }

    static void optimizePassJumps(ByteCodeOptContext* context);
    static void optimizePassDeadCode(ByteCodeOptContext* context);
    static void optimizePassEmptyFct(ByteCodeOptContext* context);
    static void optimizePassDeadStore(ByteCodeOptContext* context);
    static void optimizePassImmediate(ByteCodeOptContext* context);
    static void optimizePassStack(ByteCodeOptContext* context);
    static void optimizePassConst(ByteCodeOptContext* context);
    static void optimizePassDupCopyRBRA(ByteCodeOptContext* context);

    static void setJumps(ByteCodeOptContext* context);
    static void removeNops(ByteCodeOptContext* context);

    static bool optimize(Module* module);
};
#pragma once
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "ByteCodeOptContext.h"
struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Module;
struct Job;

struct ByteCodeOptimizer
{
    inline static bool isMemCpy(ByteCodeInstruction* inst)
    {
        return inst->op == ByteCodeOp::MemCpy8 ||
               inst->op == ByteCodeOp::MemCpy16 ||
               inst->op == ByteCodeOp::MemCpy32 ||
               inst->op == ByteCodeOp::MemCpy64 ||
               inst->op == ByteCodeOp::MemCpyX;
    }

    inline static bool isJump(ByteCodeInstruction* inst)
    {
        return inst->op == ByteCodeOp::Jump ||
               inst->op == ByteCodeOp::JumpIfTrue ||
               inst->op == ByteCodeOp::JumpIfFalse ||
               inst->op == ByteCodeOp::JumpIfNotZero8 ||
               inst->op == ByteCodeOp::JumpIfNotZero16 ||
               inst->op == ByteCodeOp::JumpIfNotZero32 ||
               inst->op == ByteCodeOp::JumpIfNotZero64 ||
               inst->op == ByteCodeOp::JumpIfFalse ||
               inst->op == ByteCodeOp::JumpIfZero8 ||
               inst->op == ByteCodeOp::JumpIfZero16 ||
               inst->op == ByteCodeOp::JumpIfZero32 ||
               inst->op == ByteCodeOp::JumpIfZero64;
    }

    inline static bool isJumpBlock(ByteCodeInstruction* inst)
    {
        if (!isJump(inst))
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero8 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero16 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero32 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero64 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero8 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero16 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero32 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero64 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::IntrinsicAssert)
            return false;
        return true;
    }

    static uint32_t newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here);
    static void     genTree(ByteCodeOptContext* context, uint32_t nodeIdx);
    static void     genTree(ByteCodeOptContext* context);
    static void     parseTree(ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt);
    static void     parseTree(ByteCodeOptContext* context, uint32_t startNode, ByteCodeInstruction* startIp, uint32_t doneFlag, function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb);

    static void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void optimizePassJumps(ByteCodeOptContext* context);
    static void optimizePassDeadCode(ByteCodeOptContext* context);
    static void optimizePassEmptyFct(ByteCodeOptContext* context);
    static void optimizePassDeadStore(ByteCodeOptContext* context);
    static void optimizePassDeadStore2(ByteCodeOptContext* context);
    static void optimizePassImmediate(ByteCodeOptContext* context);
    static void optimizePassConst(ByteCodeOptContext* context);
    static void optimizePassDupCopyRBRA(ByteCodeOptContext* context);
    static void optimizePassDupCopyRA(ByteCodeOptContext* context);
    static void optimizePassDupMisc(ByteCodeOptContext* context);
    static void optimizePassRetCopyLocal(ByteCodeOptContext* context);
    static void optimizePassRetCopyGlobal(ByteCodeOptContext* context);
    static void optimizePassRetCopyInline(ByteCodeOptContext* context);
    static void optimizePassReduce(ByteCodeOptContext* context);

    static void optimizePassDupCopy(ByteCodeOptContext* context, ByteCodeOp op);

    static void setJumps(ByteCodeOptContext* context);
    static void removeNops(ByteCodeOptContext* context);

    static bool optimize(Job* job, Module* module, bool& done);
};
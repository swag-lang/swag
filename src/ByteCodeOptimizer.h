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
    inline static bool hasRefToRegA(ByteCodeInstruction* inst, uint32_t reg)
    {
        return inst->a.u32 == reg && !(inst->flags & BCI_IMM_A) && g_ByteCodeOpFlags[(int) inst->op] & (OPFLAG_READ_A | OPFLAG_WRITE_A);
    }

    inline static bool hasRefToRegB(ByteCodeInstruction* inst, uint32_t reg)
    {
        return inst->b.u32 == reg && !(inst->flags & BCI_IMM_B) && g_ByteCodeOpFlags[(int) inst->op] & (OPFLAG_READ_B | OPFLAG_WRITE_B);
    }

    inline static bool hasRefToRegC(ByteCodeInstruction* inst, uint32_t reg)
    {
        return inst->c.u32 == reg && !(inst->flags & BCI_IMM_C) && g_ByteCodeOpFlags[(int) inst->op] & (OPFLAG_READ_C | OPFLAG_WRITE_C);
    }

    inline static bool hasRefToRegD(ByteCodeInstruction* inst, uint32_t reg)
    {
        return inst->d.u32 == reg && !(inst->flags & BCI_IMM_D) && g_ByteCodeOpFlags[(int) inst->op] & (OPFLAG_READ_D | OPFLAG_WRITE_D);
    }

    inline static bool hasRefToReg(ByteCodeInstruction* inst, uint32_t reg)
    {
        return hasRefToRegA(inst, reg) || hasRefToRegB(inst, reg) || hasRefToRegC(inst, reg) || hasRefToRegD(inst, reg);
    }

    inline static bool isJumpBlock(ByteCodeInstruction* inst)
    {
        if (!ByteCode::isJump(inst))
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero8 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero16 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero32 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        if (inst->op == ByteCodeOp::JumpIfNotZero64 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero8 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero16 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero32 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        if (inst->op == ByteCodeOp::JumpIfZero64 && inst->b.s32 == 1 && inst[1].op == ByteCodeOp::InternalPanic)
            return false;
        return true;
    }

    static uint32_t newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here);
    static void     genTree(ByteCodeOptContext* context, uint32_t nodeIdx);
    static void     genTree(ByteCodeOptContext* context);
    static void     parseTree(ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt);
    static void     parseTree(ByteCodeOptContext* context, uint32_t startNode, ByteCodeInstruction* startIp, uint32_t doneFlag, function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb);

    static void replaceRegister(ByteCodeOptContext* context, ByteCodeInstruction* from, uint32_t srcReg, uint32_t dstReg);
    static void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void setJumps(ByteCodeOptContext* context);
    static void removeNops(ByteCodeOptContext* context);

    static void optimizePassJumps(ByteCodeOptContext* context);
    static void optimizePassLoop(ByteCodeOptContext* context);
    static void optimizePassDeadCode(ByteCodeOptContext* context);
    static void optimizePassEmptyFct(ByteCodeOptContext* context);
    static void optimizePassDeadStore(ByteCodeOptContext* context);
    static void optimizePassImmediate(ByteCodeOptContext* context);
    static void optimizePassConst(ByteCodeOptContext* context);
    static void optimizePassRetCopyLocal(ByteCodeOptContext* context);
    static void optimizePassRetCopyGlobal(ByteCodeOptContext* context);
    static void optimizePassRetCopyInline(ByteCodeOptContext* context);
    static void optimizePassSwap(ByteCodeOptContext* context);

    static void optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op);
    static void optimizePassDupCopyRBRA(ByteCodeOptContext* context);
    static void optimizePassDupCopyOp(ByteCodeOptContext* context, ByteCodeOp op);
    static void optimizePassDupCopy(ByteCodeOptContext* context);

    static void reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceSetAt(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reducex2(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void optimizePassReduce(ByteCodeOptContext* context);

    static void optimizePassErr(ByteCodeOptContext* context);

    static bool optimize(Job* job, Module* module, bool& done);
};
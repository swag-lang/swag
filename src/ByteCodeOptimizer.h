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
    // clang-format off
    inline static bool hasRefToRegA(ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int) inst->op].flags & (OPFLAG_READ_A | OPFLAG_WRITE_A); }
    inline static bool hasRefToRegB(ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int) inst->op].flags & (OPFLAG_READ_B | OPFLAG_WRITE_B); }
    inline static bool hasRefToRegC(ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int) inst->op].flags & (OPFLAG_READ_C | OPFLAG_WRITE_C); }
    inline static bool hasRefToRegD(ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int) inst->op].flags & (OPFLAG_READ_D | OPFLAG_WRITE_D); }
    inline static bool hasRefToReg(ByteCodeInstruction* inst, uint32_t reg)  { return hasRefToRegA(inst, reg) || hasRefToRegB(inst, reg) || hasRefToRegC(inst, reg) || hasRefToRegD(inst, reg); }
    // clang-format on

    inline static bool isJumpBlock(ByteCodeInstruction* inst)
    {
        if (!ByteCode::isJump(inst))
            return false;
        if (inst[1].op != ByteCodeOp::InternalPanic || inst->b.s32 != 1)
            return true;
        switch (inst->op)
        {
        case ByteCodeOp::JumpIfNotZero8:
        case ByteCodeOp::JumpIfNotZero16:
        case ByteCodeOp::JumpIfNotZero32:
        case ByteCodeOp::JumpIfNotZero64:
        case ByteCodeOp::JumpIfZero8:
        case ByteCodeOp::JumpIfZero16:
        case ByteCodeOp::JumpIfZero32:
        case ByteCodeOp::JumpIfZero64:
            return false;
        }

        return true;
    }

    static uint32_t newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here);
    static void     genTree(ByteCodeOptContext* context, uint32_t nodeIdx);
    static void     genTree(ByteCodeOptContext* context);
    static void     parseTree(ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt);
    static void     parseTree(ByteCodeOptContext* context, uint32_t startNode, ByteCodeInstruction* startIp, uint32_t doneFlag, function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb);

    static void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void setJumps(ByteCodeOptContext* context);
    static void removeNops(ByteCodeOptContext* context);

    static bool optimizePassJumps(ByteCodeOptContext* context);
    static bool optimizePassLoop(ByteCodeOptContext* context);
    static bool optimizePassDeadCode(ByteCodeOptContext* context);
    static bool optimizePassEmptyFct(ByteCodeOptContext* context);
    static bool optimizePassDeadStore(ByteCodeOptContext* context);
    static bool optimizePassImmediate(ByteCodeOptContext* context);
    static bool optimizePassConst(ByteCodeOptContext* context);
    static bool optimizePassRetCopyLocal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyGlobal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyInline(ByteCodeOptContext* context);
    static bool optimizePassErr(ByteCodeOptContext* context);
    static bool optimizePassNullPointer(ByteCodeOptContext* context);

    static void optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op);
    static void optimizePassDupCopyOp(ByteCodeOptContext* context, ByteCodeOp op);
    static bool optimizePassDupCopyRBRA(ByteCodeOptContext* context);
    static bool optimizePassDupCopy(ByteCodeOptContext* context);

    static void reduceErr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceEmptyFct(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceAppend(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceMemcpy(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceSwap(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceSetAt(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reducex2(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static bool optimizePassReduce(ByteCodeOptContext* context);

    static bool optimize(Job* job, Module* module, bool& done);
};

#ifdef SWAG_DEV_MODE
#define SET_OP(__ip, __op)             \
    do                                 \
    {                                  \
        (__ip)->op         = __op;     \
        (__ip)->sourceFile = __FILE__; \
        (__ip)->sourceLine = __LINE__; \
    } while (0);
#else
#define SET_OP(__ip, __op) \
    do                     \
    {                      \
        (__ip)->op = __op; \
    } while (0);
#endif
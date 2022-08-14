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

    inline static bool hasWriteRefToRegA(ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_A; }
    inline static bool hasWriteRefToRegB(ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_B; }
    inline static bool hasWriteRefToRegC(ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_C; }
    inline static bool hasWriteRefToRegD(ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_D; }
    inline static bool hasWriteRefToReg(ByteCodeInstruction* inst, uint32_t reg)  { return hasWriteRefToRegA(inst, reg) || hasWriteRefToRegB(inst, reg) || hasWriteRefToRegC(inst, reg) || hasWriteRefToRegD(inst, reg); }

    inline static bool hasReadRefToRegA(ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_A; }
    inline static bool hasReadRefToRegB(ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_B; }
    inline static bool hasReadRefToRegC(ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_C; }
    inline static bool hasReadRefToRegD(ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_D; }
    inline static bool hasReadRefToReg(ByteCodeInstruction* inst, uint32_t reg)  { return hasReadRefToRegA(inst, reg) || hasReadRefToRegB(inst, reg) || hasReadRefToRegC(inst, reg) || hasReadRefToRegD(inst, reg); }

    inline static bool hasWriteRegInA(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_A; }
    inline static bool hasWriteRegInB(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_B; }
    inline static bool hasWriteRegInC(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_C; }
    inline static bool hasWriteRegInD(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_D; }

    inline static bool hasReadRegInA(ByteCodeInstruction* inst)  { return !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_A; }
    inline static bool hasReadRegInB(ByteCodeInstruction* inst)  { return !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_B; }
    inline static bool hasReadRegInC(ByteCodeInstruction* inst)  { return !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_C; }
    inline static bool hasReadRegInD(ByteCodeInstruction* inst)  { return !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_D; }

    inline static bool hasSomethingInA(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_A | OPFLAG_WRITE_A | OPFLAG_READ_VAL32_A | OPFLAG_READ_VAL64_A); }
    inline static bool hasSomethingInB(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_B | OPFLAG_WRITE_B | OPFLAG_READ_VAL32_B | OPFLAG_READ_VAL64_B); }
    inline static bool hasSomethingInC(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_C | OPFLAG_WRITE_C | OPFLAG_READ_VAL32_C | OPFLAG_READ_VAL64_C); }
    inline static bool hasSomethingInD(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_D | OPFLAG_WRITE_D | OPFLAG_READ_VAL32_D | OPFLAG_READ_VAL64_D); }
    // clang-format on

    static uint32_t newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here);
    static void     genTree(ByteCodeOptContext* context, uint32_t nodeIdx);
    static void     genTree(ByteCodeOptContext* context);
    static void     parseTree(ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt);
    static void     parseTree(ByteCodeOptContext* context, uint32_t startNode, ByteCodeInstruction* startIp, uint32_t doneFlag, function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb);

    static void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void setContextFlags(ByteCodeOptContext* context);
    static void setJumps(ByteCodeOptContext* context);
    static void removeNops(ByteCodeOptContext* context);

    static void registerParamsReg(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void registerMakeAddr(ByteCodeOptContext* context, ByteCodeInstruction* ip);

    static bool optimizePassJumps(ByteCodeOptContext* context);
    static bool optimizePassLoop(ByteCodeOptContext* context);
    static bool optimizePassDeadCode(ByteCodeOptContext* context);
    static bool optimizePassDeadStore(ByteCodeOptContext* context);
    static bool optimizePassDeadStoreDup(ByteCodeOptContext* context);
    static bool optimizePassImmediate(ByteCodeOptContext* context);
    static bool optimizePassConst(ByteCodeOptContext* context);
    static bool optimizePassAlias(ByteCodeOptContext* context);
    static bool optimizePassRetCopyLocal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyGlobal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyInline(ByteCodeOptContext* context);

    static void optimizePassSwitch(ByteCodeOptContext* context, ByteCodeOp op0, ByteCodeOp op1);
    static bool optimizePassSwitch(ByteCodeOptContext* context);

    static void optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op);
    static void optimizePassDupCopyOp(ByteCodeOptContext* context, ByteCodeOp op);
    static bool optimizePassDupCopyRBRA(ByteCodeOptContext* context);
    static bool optimizePassDupCopy(ByteCodeOptContext* context);
    static bool optimizePassDupBlocks(ByteCodeOptContext* context);

    static void reduceFactor(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceErr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceEmptyFct(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceAppend(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceMemcpy(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceSwap(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceFunc(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceSetAt(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceForceSafe(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static bool optimizePassReduce(ByteCodeOptContext* context);
    static bool optimizePassReduceX2(ByteCodeOptContext* context);

    static bool optimize(Job* job, Module* module, bool& done);
};

#ifdef SWAG_DEV_MODE
#define SET_OP(__ip, __op)                        \
    do                                            \
    {                                             \
        (__ip)->op                    = __op;     \
        (__ip)->sourceFile            = __FILE__; \
        (__ip)->sourceLine            = __LINE__; \
        context->passHasDoneSomething = true;     \
    } while (0);
#else
#define SET_OP(__ip, __op) \
    do                     \
    {                      \
        (__ip)->op = __op; \
    } while (0);
#endif
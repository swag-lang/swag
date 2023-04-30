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
    static uint32_t newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here);
    static void     genTree(ByteCodeOptContext* context, uint32_t nodeIdx, bool computeCrc);
    static void     genTree(ByteCodeOptContext* context, bool computeCrc);
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
    static bool optimizePassRetCopyLocal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyGlobal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyInline(ByteCodeOptContext* context);
    static bool optimizePassRetCopyStructVal(ByteCodeOptContext* context);

    static void optimizePassSwitch(ByteCodeOptContext* context, ByteCodeOp op0, ByteCodeOp op1);
    static bool optimizePassSwitch(ByteCodeOptContext* context);
    static bool optimizePassSwap(ByteCodeOptContext* context);

    static void optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op);
    static void optimizePassDupCopyOp(ByteCodeOptContext* context, ByteCodeOp op);
    static bool optimizePassDupCopyRBRA(ByteCodeOptContext* context);
    static bool optimizePassDupCopy(ByteCodeOptContext* context);
    static bool optimizePassDupBlocks(ByteCodeOptContext* context);

    static void reduceMath(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceFactor(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceErr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCallEmptyFct(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceAppend(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceMemcpy(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceFunc(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCast(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceSetAt(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceForceSafe(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStackOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceLateStack(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceDupInstr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static bool optimizePassReduce(ByteCodeOptContext* context);
    static bool optimizePassReduceX2(ByteCodeOptContext* context);

    static bool optimizePassSanity(ByteCodeOptContext* context);

    static bool optimize(Job* job, Module* module, bool& done);
    static bool optimize(ByteCodeOptContext& optContext, ByteCode* bc, bool& restart);
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
#define SET_OP(__ip, __op)                    \
    do                                        \
    {                                         \
        (__ip)->op                    = __op; \
        context->passHasDoneSomething = true; \
    } while (0);
#endif
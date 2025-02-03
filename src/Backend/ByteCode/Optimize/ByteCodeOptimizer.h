#pragma once
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCodeOp.h"
#include "ByteCodeOptContext.h"
struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Module;
struct Job;

struct ByteCodeOptimizer
{
    static bool hasReadRefToReg(ByteCodeOptContext* context, uint32_t regScan, uint32_t curNode, ByteCodeInstruction* curIp);

    using ParseTreeCB = std::function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)>;
    static uint32_t newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here);
    static void     genTree(ByteCodeOptContext* context, uint32_t nodeIdx, bool computeCrc);
    static void     genTree(ByteCodeOptContext* context, bool computeCrc);
    static void     parseTree(ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt);
    static void     parseTree(ByteCodeOptContext* context, uint32_t startNode, ByteCodeInstruction* startIp, uint32_t doneFlag, const ParseTreeCB& cb);

    static void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void setContextFlags(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void setJumps(ByteCodeOptContext* context);
    static bool insertNopBefore(ByteCodeOptContext* context, ByteCodeInstruction* insert);
    static void removeNop(ByteCodeOptContext* context);

    static void registerParamsReg(ByteCodeOptContext* context, const ByteCodeInstruction* ip);
    static void registerMakeAddr(ByteCodeOptContext* context, const ByteCodeInstruction* ip);

    static bool optimizePassJump(ByteCodeOptContext* context);
    static bool optimizePassConstJump(ByteCodeOptContext* context);
    static bool optimizePassDeadCode(ByteCodeOptContext* context);
    static bool optimizePassDeadStore(ByteCodeOptContext* context);
    static bool optimizePassDeadStoreDup(ByteCodeOptContext* context);
    static bool optimizePassImmediate(ByteCodeOptContext* context);
    static bool optimizePassImmediate2(ByteCodeOptContext* context);
    static bool optimizePassConst(ByteCodeOptContext* context);
    static bool optimizePassParam(ByteCodeOptContext* context);
    static bool optimizePassRetCopyLocal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyGlobal(ByteCodeOptContext* context);
    static bool optimizePassRetCopyStructVal(ByteCodeOptContext* context);
    static bool optimizePassErr(ByteCodeOptContext* context);

    static void optimizePassSwitch(ByteCodeOptContext* context, ByteCodeOp op0, ByteCodeOp op1);
    static bool optimizePassSwitch(ByteCodeOptContext* context);
    static bool optimizePassSwap(ByteCodeOptContext* context);
    static bool optimizePassLoop(ByteCodeOptContext* context);

    static void optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op);
    static void optimizePassDupSetRAOp(ByteCodeOptContext* context, ByteCodeOp op);
    static bool optimizePassDupCopyRBRA(ByteCodeOptContext* context);
    static bool optimizePassDupSetRA(ByteCodeOptContext* context);
    static bool optimizePassDupBlocks(ByteCodeOptContext* inContext);
    static bool optimizePassDupInstruction(ByteCodeOptContext* context);

    static void reduceStackJumps(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceMath(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceAffectOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceFactor(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceErr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCallEmptyFct(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceAppend(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceMemcpy(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceFunc(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStack1(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStack2(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStack3(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCast(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceSetAt(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCall(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceInvCopy(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceForceSafe(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceStackOp(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceLateStack(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceDupInstr(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static void reduceCopy(ByteCodeOptContext* context, ByteCodeInstruction* ip);
    static bool optimizePassReduce(ByteCodeOptContext* context);
    static bool optimizePassReduceX2(ByteCodeOptContext* context);

    static bool optimize(Job* job, Module* module, bool& done);
    static bool optimize(ByteCodeOptContext& optContext, ByteCode* bc, bool& restart);
};

#ifdef SWAG_DEV_MODE
#define SET_OP(__ip, __op)             \
    do                                 \
    {                                  \
        (__ip)->op         = __op;     \
        (__ip)->sourceFile = __FILE__; \
        (__ip)->sourceLine = __LINE__; \
        context->setDirtyPass();       \
    } while (0)
#else
#define SET_OP(__ip, __op)       \
    do                           \
    {                            \
        (__ip)->op = __op;       \
        context->setDirtyPass(); \
    } while (0)
#endif

#define OPT_PASS(__func, __minLevel)                                              \
    if (optContext.module->buildCfg.byteCodeOptimizeLevel >= (__minLevel))        \
    {                                                                             \
        optContext.passHasDoneSomething = false;                                  \
        if (!__func(&optContext))                                                 \
            return false;                                                         \
        if (optContext.hasError)                                                  \
            return false;                                                         \
        optContext.allPassesHaveDoneSomething |= optContext.passHasDoneSomething; \
    }

#define OPT_SUB_PASS(__func, __minLevel)                                 \
    if (context->module->buildCfg.byteCodeOptimizeLevel >= (__minLevel)) \
    {                                                                    \
        __func(context, ip);                                             \
    }

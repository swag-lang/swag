#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeGen.h"
#include "ByteCodeOptimizerJob.h"
#include "Module.h"
#include "Statistics.h"
#include "ThreadManager.h"

uint32_t ByteCodeOptimizer::newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here)
{
    here = false;

    const auto it = context->mapInstNode.find(ip);
    if (it != context->mapInstNode.end())
    {
        here = true;
        return it->second;
    }

    if (context->nextTreeNode < context->tree.size())
    {
        const auto tn = &context->tree[context->nextTreeNode];
        tn->start     = ip;
        tn->end       = nullptr;
        tn->next.clear();
        tn->parent.clear();
        tn->mark  = 0;
        tn->flags = 0;
        tn->crc   = 0;

        context->mapInstNode[ip] = context->nextTreeNode;
        return context->nextTreeNode++;
    }

    ByteCodeOptTreeNode newNode;
    newNode.start = ip;
    context->tree.push_back(newNode);
    context->nextTreeNode++;

    const uint32_t pos       = (uint32_t) context->tree.size() - 1;
    context->mapInstNode[ip] = pos;
    return pos;
}

void ByteCodeOptimizer::setContextFlags(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Mark some instructions for some specific passes
    if (ip->node &&
        ip->node->sourceFile &&
        ip->node->sourceFile->module &&
        ip->node->sourceFile->module->mustEmitSafetyOverflow(ip->node))
    {
        ip->dynFlags |= BCID_SAFETY_OF;
    }
    else
    {
        ip->dynFlags &= ~BCID_SAFETY_OF;
    }

    if (ip->node && ip->node->ownerInline)
        context->contextBcFlags |= OCF_HAS_INLINE;

    switch (ip->op)
    {
    case ByteCodeOp::CopyRBtoRA8:
    case ByteCodeOp::CopyRBtoRA16:
    case ByteCodeOp::CopyRBtoRA32:
    case ByteCodeOp::CopyRBtoRA64:
        context->contextBcFlags |= OCF_HAS_COPY_RBRA;
        break;

    case ByteCodeOp::CopyRTtoRA:
        context->contextBcFlags |= OCF_HAS_COPY_RTRC;
        break;

    case ByteCodeOp::CopyRAtoRT:
        context->contextBcFlags |= OCF_HAS_COPY_RCRT;
        break;

    case ByteCodeOp::MakeBssSegPointer:
    case ByteCodeOp::MakeConstantSegPointer:
    case ByteCodeOp::MakeMutableSegPointer:
    case ByteCodeOp::SetImmediate32:
    case ByteCodeOp::SetImmediate64:
    case ByteCodeOp::GetParam64x2:
    case ByteCodeOp::GetParam64DeRef8:
    case ByteCodeOp::GetParam64DeRef16:
    case ByteCodeOp::GetParam64DeRef32:
    case ByteCodeOp::GetParam64DeRef64:
    case ByteCodeOp::GetIncParam64DeRef8:
    case ByteCodeOp::GetIncParam64DeRef16:
    case ByteCodeOp::GetIncParam64DeRef32:
    case ByteCodeOp::GetIncParam64DeRef64:
        context->contextBcFlags |= OCF_HAS_DUP_COPY;
        break;

    case ByteCodeOp::CopyRRtoRA:
    case ByteCodeOp::GetParam64:
    case ByteCodeOp::GetIncParam64:
        context->contextBcFlags |= OCF_HAS_DUP_COPY;
        context->contextBcFlags |= OCF_HAS_PARAM;
        break;

    case ByteCodeOp::JumpIfNoError:
        context->contextBcFlags |= OCF_HAS_ERR;
        break;

    default:
        break;
    }
}

void ByteCodeOptimizer::genTree(ByteCodeOptContext* context, uint32_t nodeIdx, bool computeCrc)
{
    ByteCodeOptTreeNode* node = &context->tree[nodeIdx];
    node->end                 = node->start;

    while (!ByteCode::isRet(node->end) && !ByteCode::isJumpOrDyn(node->end) && !(node->end[1].flags & BCI_START_STMT))
    {
        setContextFlags(context, node->end);
        if (computeCrc)
            node->crc = context->bc->computeCrc(node->end, node->crc, true, false);
#ifdef SWAG_DEV_MODE
        node->end->treeNode = nodeIdx + 1;
#endif
        node->end++;
    }

    setContextFlags(context, node->end);
    if (computeCrc)
        node->crc = context->bc->computeCrc(node->end, node->crc, true, false);

#ifdef SWAG_DEV_MODE
    node->end->treeNode = nodeIdx + 1;
    auto ip             = node->start;
    while (ip != node->end + 1)
    {
        ip->crc = node->crc;
        ip++;
    }
#endif

    if (ByteCode::isRet(node->end))
        return;

    bool here = false;

    if (ByteCode::isJumpDyn(node->end))
    {
        const int32_t* table = (int32_t*) context->module->compilerSegment.address(node->end->d.u32);
        for (uint32_t i = 0; i < node->end->c.u32; i++)
        {
            auto newNode = newTreeNode(context, node->end + table[i] + 1, here);
            if (!here)
                genTree(context, newNode, computeCrc);
            node = &context->tree[nodeIdx];
            node->next.push_back(newNode);
            const auto newNodePtr = &context->tree[newNode];
            newNodePtr->parent.push_back(nodeIdx);
        }
    }
    else if (ByteCode::isJump(node->end))
    {
        ByteCodeInstruction* nextIp = node->end + node->end->b.s32 + 1;
        if (nextIp->op != ByteCodeOp::End)
        {
            const auto newNode = newTreeNode(context, nextIp, here);
            if (!here)
                genTree(context, newNode, computeCrc);
            node = &context->tree[nodeIdx];
            node->next.push_back(newNode);
            const auto newNodePtr = &context->tree[newNode];
            newNodePtr->parent.push_back(nodeIdx);
        }

        if (node->end->op != ByteCodeOp::Jump && node->end->op != ByteCodeOp::End)
        {
            nextIp             = node->end + 1;
            const auto newNode = newTreeNode(context, nextIp, here);
            if (!here)
                genTree(context, newNode, computeCrc);
            node = &context->tree[nodeIdx];
            node->next.push_back(newNode);
            const auto newNodePtr = &context->tree[newNode];
            newNodePtr->parent.push_back(nodeIdx);
        }
    }
    else
    {
        const auto nextIp  = node->end + 1;
        const auto newNode = newTreeNode(context, nextIp, here);
        if (!here)
            genTree(context, newNode, computeCrc);
        node = &context->tree[nodeIdx];
        node->next.push_back(newNode);
        const auto newNodePtr = &context->tree[newNode];
        newNodePtr->parent.push_back(nodeIdx);
    }
}

void ByteCodeOptimizer::genTree(ByteCodeOptContext* context, bool computeCrc)
{
    context->mapInstNode.clear();
    context->nextTreeNode   = 0;
    context->contextBcFlags = 0;

    const auto bc = context->bc;

    bool       here = false;
    const auto node = newTreeNode(context, bc->out, here);

    genTree(context, node, computeCrc);
}

void ByteCodeOptimizer::parseTree(ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt)
{
    ByteCodeOptTreeNode* node = &context->tree[parseCxt.curNode];

    while (true)
    {
        // Back to first instruction
        if ((node->flags & parseCxt.doneFlag) && parseCxt.curIp == parseCxt.startIp)
            return;

        parseCxt.cb(context, parseCxt);

        // Stop the parsing
        if (parseCxt.mustStopAll)
        {
            return;
        }

        // Just stop that block
        if (parseCxt.mustStopBlock)
        {
            parseCxt.mustStopBlock = false;
            return;
        }

        // We have reached the last instruction of the block
        if (parseCxt.curIp == node->end)
            break;

        parseCxt.curIp++;
    }

    for (const auto n : node->next)
    {
        SWAG_ASSERT(n < context->nextTreeNode);
        if (!(context->tree[n].flags & parseCxt.doneFlag))
        {
            context->tree[n].flags |= parseCxt.doneFlag;
            parseCxt.curNode = n;
            parseCxt.curIp   = context->tree[n].start;
            parseTree(context, parseCxt);
            if (parseCxt.mustStopAll)
                return;
        }
    }
}

void ByteCodeOptimizer::parseTree(ByteCodeOptContext*                                               context,
                                  uint32_t                                                          startNode,
                                  ByteCodeInstruction*                                              startIp,
                                  uint32_t                                                          doneFlag,
                                  function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb)
{
    for (uint32_t i = 0; i < context->nextTreeNode; i++)
        context->tree[i].flags &= ~doneFlag;

    ByteCodeOptTreeParseContext parseCxt;
    parseCxt.curNode  = startNode;
    parseCxt.startIp  = startIp;
    parseCxt.curIp    = startIp;
    parseCxt.cb       = cb;
    parseCxt.doneFlag = doneFlag;
    parseTree(context, parseCxt);
}

void ByteCodeOptimizer::setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::Nop)
        return;
    if (ip->flags & BCI_UNPURE && ip->op != ByteCodeOp::ClearRA)
        return;
    const auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;
    if (flags & OPFLAG_UNPURE)
        return;

    SWAG_ASSERT(ip->op != ByteCodeOp::End);
    context->passHasDoneSomething = true;
    ip->op                        = ByteCodeOp::Nop;
    context->nops.push_back(ip);
}

void ByteCodeOptimizer::removeNops(ByteCodeOptContext* context)
{
    if (context->nops.empty())
        return;

    context->allPassesHaveDoneSomething = true;

    auto ip     = context->bc->out;
    auto ipDest = context->bc->out;
    for (uint32_t i = 0; i < context->bc->numInstructions; i++)
    {
        if (ip->op == ByteCodeOp::Nop)
        {
#ifdef SWAG_STATS
            g_Stats.totalOptimsBC++;
#endif
            ip++;
            continue;
        }

        if (ByteCode::isJump(ip))
        {
            const auto srcJump = (int) (ip - context->bc->out);
            const auto dstJump = srcJump + ip->b.s32 + 1;
            for (const auto nop : context->nops)
            {
                const auto idxNop = (int) (nop - context->bc->out);

                // If this is a ordered jump, and there's a nop between the destination jump
                // and the jump, then we need to remove one jump instruction
                if (srcJump < dstJump && idxNop > srcJump && idxNop < dstJump)
                {
                    ip->b.s32--;
                }

                // If this is a back jump, and there's a nop between the destination jump
                // and the jump, then we need to remove one jump instruction
                else if (srcJump > dstJump && idxNop >= dstJump && idxNop < srcJump)
                {
                    ip->b.s32++;
                }
            }
        }
        else if (ByteCode::isJumpDyn(ip))
        {
            int32_t* table = (int32_t*) context->module->compilerSegment.address(ip->d.u32);
            for (uint32_t idx = 0; idx < ip->c.u32; idx++)
            {
                const auto srcJump = (int) (ip - context->bc->out);
                const auto dstJump = srcJump + (int) table[idx] + 1;
                for (const auto nop : context->nops)
                {
                    const auto idxNop = (int) (nop - context->bc->out);
                    if (srcJump < dstJump && idxNop > srcJump && idxNop < dstJump)
                        table[idx]--;
                    else if (srcJump > dstJump && idxNop >= dstJump && idxNop < srcJump)
                        table[idx]++;
                }
            }
        }

        *ipDest++ = *ip++;
    }

    context->bc->numInstructions -= (int) context->nops.size();
    context->nops.clear();
}

bool ByteCodeOptimizer::insertNopBefore(ByteCodeOptContext* context, ByteCodeInstruction* insert)
{
    if (insert != context->bc->out && insert[-1].op == ByteCodeOp::Nop)
        return true;

    // We do not want to invalid all pointer to instructions by reallocating the instruction buffer.
    // So if we do not have at least one free instruction, then we fail.
    if (context->bc->numInstructions == context->bc->maxInstructions)
        return false;

    for (auto& nop : context->nops)
    {
        if (nop > insert)
            nop++;
    }

    for (int it = 0; it < (int) context->jumps.size(); it++)
    {
        const auto jump = context->jumps[it];
        SWAG_ASSERT(ByteCode::isJumpOrDyn(jump));

        if (ByteCode::isJumpDyn(jump))
        {
            int32_t* table = (int32_t*) context->module->compilerSegment.address(jump->d.u32);
            for (uint32_t i = 0; i < jump->c.u32; i++)
            {
                const auto ipDest = jump + table[i] + 1;
                if (jump < insert && ipDest > insert)
                {
                    table[i] += 1;
                    jump[table[i] + 1].flags |= BCI_START_STMT;
                }
                else if (jump >= insert && ipDest < insert)
                {
                    table[i] -= 1;
                    jump[table[i] + 1].flags |= BCI_START_STMT;
                }
            }
        }
        else
        {
            const auto ipDest = jump + jump->b.s32 + 1;
            if (jump < insert && ipDest > insert)
            {
                jump->b.s32 += 1;
                jump[jump->b.s32].flags |= BCI_START_STMT;
            }
            else if (jump >= insert && ipDest < insert)
            {
                jump->b.s32 -= 1;
                jump[jump->b.s32].flags |= BCI_START_STMT;
            }
        }

        if (jump >= insert)
            context->jumps[it]++;
    }

    const auto insIndex = insert - context->bc->out;
    size_t     size     = context->bc->numInstructions - insIndex;
    size *= sizeof(ByteCodeInstruction);
    memmove(insert + 1, insert, size);
    insert->op = ByteCodeOp::Nop;
    context->bc->numInstructions++;
    return true;
}

void ByteCodeOptimizer::setJumps(ByteCodeOptContext* context)
{
    context->jumps.clear();
    if (!context->bc->numJumps)
        return;

    context->jumps.reserve(context->bc->numJumps);
    context->bc->numJumps = 0;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        ip->flags &= ~BCI_START_STMT;
        if (ByteCode::isJumpOrDyn(ip))
            context->jumps.push_back(ip);
    }

    // Mark all instructions which are a jump destination
    context->bc->numJumps = (uint32_t) context->jumps.size();
    for (const auto jump : context->jumps)
    {
        if (ByteCode::isJumpDyn(jump))
        {
            const int32_t* table = (int32_t*) context->module->compilerSegment.address(jump->d.u64 & 0xFFFFFFFF);
            for (uint32_t i = 0; i < jump->c.u32; i++)
            {
                if (jump->flags & BCI_SAFETY)
                    jump[table[i] + 1].flags |= BCI_START_STMT_S;
                else
                    jump[table[i] + 1].flags |= BCI_START_STMT_N;
            }
        }
        else
        {
            if (jump->flags & BCI_SAFETY)
                jump[jump->b.s32 + 1].flags |= BCI_START_STMT_S;
            else
                jump[jump->b.s32 + 1].flags |= BCI_START_STMT_N;
        }
    }
}

bool ByteCodeOptimizer::optimize(Job* job, Module* module, bool& done)
{
    done = true;
    if (module->byteCodeFunc.empty())
        return true;

    done = false;
    if (module->numErrors)
        return false;

    // Determine if we need to restart the whole optim pass because something has been done
    if (module->optimPass == 1)
    {
        if (module->buildCfg.byteCodeOptimizeLevel == 2 && module->optimNeedRestart.load() > 0)
        {
            module->optimPass = 0;
        }
        else
        {
            done = true;
            return true;
        }
    }

    if (module->optimPass == 0)
    {
        module->optimNeedRestart.store(0);

        // Divide so that each job has the quite same amout of bytecode to optimize
        uint32_t totalInstructions = 0;
        for (const auto bc : module->byteCodeFunc)
        {
            if (bc->canEmit())
                totalInstructions += bc->numInstructions;
        }

        totalInstructions /= (g_ThreadMgr.numWorkers * 4);
        totalInstructions = max(totalInstructions, 1);

        size_t startIndex = 0;
        while (startIndex < module->byteCodeFunc.size())
        {
            const auto newJob    = Allocator::alloc<ByteCodeOptimizerJob>();
            newJob->module       = module;
            newJob->dependentJob = job;
            newJob->startIndex   = (int) startIndex;
            newJob->endIndex     = (int) min(startIndex + 1, module->byteCodeFunc.size());
            startIndex           = newJob->endIndex;

            auto curInst = module->byteCodeFunc[newJob->startIndex]->numInstructions;
            while (curInst < totalInstructions && newJob->endIndex + 1 < (int) module->byteCodeFunc.size())
            {
                const auto bc = module->byteCodeFunc[newJob->endIndex];
                if (bc->canEmit())
                    curInst += bc->numInstructions;
                bc->isEmpty = bc->isDoingNothing();
                newJob->endIndex++;
                startIndex = newJob->endIndex;
            }

            if (job)
                job->jobsToAdd.push_back(newJob);
            else
                g_ThreadMgr.addJob(newJob);
        }

        module->optimPass = 1;
        done              = false;
        return true;
    }

    return true;
}

#define OPT_PASS(__func)                     \
    optContext.passHasDoneSomething = false; \
    if (!__func(&optContext))                \
        return false;                        \
    optContext.allPassesHaveDoneSomething |= optContext.passHasDoneSomething;

bool ByteCodeOptimizer::optimize(ByteCodeOptContext& optContext, ByteCode* bc, bool& restart)
{
    SWAG_RACE_CONDITION_WRITE(bc->raceCond);
    optContext.bc = bc;

    if (bc->node && !bc->sanDone && optContext.module->mustEmitSafety(bc->node, SAFETY_SANITY))
    {
        bc->sanDone = true;
        setJumps(&optContext);
        genTree(&optContext, false);
        SWAG_CHECK(optimizePassSanity(&optContext));
    }

    if (optContext.module->mustOptimizeBytecode(bc->node))
    {
        while (true)
        {
            if (!bc->isEmpty && bc->isDoingNothing())
            {
                bc->isEmpty = true;
                restart     = true;
            }

            if (bc->isEmpty)
                return true;

            setJumps(&optContext);
            genTree(&optContext, false);

            if (optContext.hasError)
                return false;
            optContext.allPassesHaveDoneSomething = false;

            OPT_PASS(optimizePassJumps);
            OPT_PASS(optimizePassDeadCode);
            OPT_PASS(optimizePassImmediate);
            OPT_PASS(optimizePassConst);
            OPT_PASS(optimizePassDupCopyRBRA);
            OPT_PASS(optimizePassDupCopy);
            OPT_PASS(optimizePassRetCopyLocal);
            OPT_PASS(optimizePassRetCopyInline);
            OPT_PASS(optimizePassRetCopyGlobal);
            OPT_PASS(optimizePassRetCopyStructVal);
            OPT_PASS(optimizePassReduce);
            OPT_PASS(optimizePassDeadStore);
            OPT_PASS(optimizePassDeadStoreDup);
            OPT_PASS(optimizePassSwap);
            OPT_PASS(optimizePassParam);

            removeNops(&optContext);
            if (!optContext.allPassesHaveDoneSomething)
            {
                setJumps(&optContext);
                genTree(&optContext, true);

                OPT_PASS(optimizePassErr);
                OPT_PASS(optimizePassLoop);
                OPT_PASS(optimizePassSwitch);
                OPT_PASS(optimizePassDupBlocks);
                OPT_PASS(optimizePassReduceX2);
                removeNops(&optContext);
                if (!optContext.allPassesHaveDoneSomething)
                {
#ifdef SWAG_STATS
                    if (g_CommandLine.statsFreq)
                    {
                        for (uint32_t i = 0; i < optContext.bc->numInstructions - 1; i++)
                        {
                            const auto ip = optContext.bc->out + i;
                            ++g_Stats.countOpFreq[(int) ip[0].op][(int) ByteCodeOp::End];

                            if (ip[1].flags & BCI_START_STMT)
                                continue;
                            ++g_Stats.countOpFreq[(int) ip[0].op][(int) ip[1].op];
                        }
                    }
#endif

                    break;
                }
            }

            restart = true;
        }
    }

    return true;
}

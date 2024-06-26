#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "ByteCodeOptimizerJob.h"
#include "Main/Statistics.h"
#include "Syntax/AstNode.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"

bool ByteCodeOptimizer::hasReadRefToReg(ByteCodeOptContext* context, uint32_t regScan, uint32_t curNode, ByteCodeInstruction* curIp)
{
    bool hasRead = false;

    parseTree(context, curNode, curIp, BCOTN_USER2, [&](ByteCodeOptContext*, ByteCodeOptTreeParseContext& parseCxt1) {
        const auto ip1 = parseCxt1.curIp;
        if (ip1 == curIp)
            return;

        const auto flags1 = g_ByteCodeOpDesc[static_cast<int>(ip1->op)].flags;
        if (flags1.has(OPFLAG_READ_A) && !ip1->hasFlag(BCI_IMM_A))
        {
            if (ip1->a.u32 == regScan)
            {
                hasRead               = true;
                parseCxt1.mustStopAll = true;
                return;
            }
        }

        if (flags1.has(OPFLAG_READ_B) && !ip1->hasFlag(BCI_IMM_B))
        {
            if (ip1->b.u32 == regScan)
            {
                hasRead               = true;
                parseCxt1.mustStopAll = true;
                return;
            }
        }

        if (flags1.has(OPFLAG_READ_C) && !ip1->hasFlag(BCI_IMM_C))
        {
            if (ip1->c.u32 == regScan)
            {
                hasRead               = true;
                parseCxt1.mustStopAll = true;
                return;
            }
        }

        if (flags1.has(OPFLAG_READ_D) && !ip1->hasFlag(BCI_IMM_D))
        {
            if (ip1->d.u32 == regScan)
            {
                hasRead               = true;
                parseCxt1.mustStopAll = true;
                return;
            }
        }

        if (flags1.has(OPFLAG_WRITE_A))
        {
            if (ip1->a.u32 == regScan)
            {
                parseCxt1.mustStopBlock = true;
                return;
            }
        }

        if (flags1.has(OPFLAG_WRITE_B))
        {
            if (ip1->b.u32 == regScan)
            {
                parseCxt1.mustStopBlock = true;
                return;
            }
        }

        if (flags1.has(OPFLAG_WRITE_C))
        {
            if (ip1->c.u32 == regScan)
            {
                parseCxt1.mustStopBlock = true;
                return;
            }
        }

        if (flags1.has(OPFLAG_WRITE_D))
        {
            if (ip1->d.u32 == regScan)
            {
                parseCxt1.mustStopBlock = true;
                return;
            }
        }
    });

    return hasRead;
}

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

    const uint32_t pos       = context->tree.size() - 1;
    context->mapInstNode[ip] = pos;
    return pos;
}

void ByteCodeOptimizer::setContextFlags(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Mark some instructions for some specific passes
    if (ip->node &&
        ip->node->token.sourceFile &&
        ip->node->token.sourceFile->module &&
        ip->node->token.sourceFile->module->mustEmitSafetyOverflow(ip->node))
    {
        ip->dynFlags.add(BCID_SAFETY_OF);
    }
    else
    {
        ip->dynFlags.remove(BCID_SAFETY_OF);
    }

    if (ip->node && ip->node->hasOwnerInline())
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
    }
}

void ByteCodeOptimizer::genTree(ByteCodeOptContext* context, uint32_t nodeIdx, bool computeCrc)
{
    ByteCodeOptTreeNode* treeNode = &context->tree[nodeIdx];
    treeNode->end                 = treeNode->start;

    while (!ByteCode::isRet(treeNode->end) && !ByteCode::isJumpOrDyn(treeNode->end) && !treeNode->end[1].hasFlag(BCI_START_STMT))
    {
        setContextFlags(context, treeNode->end);
        if (computeCrc)
            treeNode->crc = context->bc->computeCrc(treeNode->end, treeNode->crc, true, false);
#ifdef SWAG_DEV_MODE
        treeNode->end->treeNode = nodeIdx + 1;
#endif
        treeNode->end++;
    }

    setContextFlags(context, treeNode->end);
    if (computeCrc)
        treeNode->crc = context->bc->computeCrc(treeNode->end, treeNode->crc, true, false);

#ifdef SWAG_DEV_MODE
    treeNode->end->treeNode = nodeIdx + 1;
    auto ip                 = treeNode->start;
    while (ip != treeNode->end + 1)
    {
        ip->crc = treeNode->crc;
        ip++;
    }
#endif

    if (ByteCode::isRet(treeNode->end))
        return;

    bool here = false;

    if (ByteCode::isJumpDyn(treeNode->end))
    {
        const auto table = reinterpret_cast<int32_t*>(context->module->compilerSegment.address(treeNode->end->d.u32));
        for (uint32_t i = 0; i < treeNode->end->c.u32; i++)
        {
            auto newNode = newTreeNode(context, treeNode->end + table[i] + 1, here);
            if (!here)
                genTree(context, newNode, computeCrc);
            treeNode = &context->tree[nodeIdx];
            treeNode->next.push_back(newNode);
            const auto newNodePtr = &context->tree[newNode];
            newNodePtr->parent.push_back(nodeIdx);
        }
    }
    else if (ByteCode::isJump(treeNode->end))
    {
        ByteCodeInstruction* nextIp = treeNode->end + treeNode->end->b.s32 + 1;
        if (nextIp->op != ByteCodeOp::End)
        {
            const auto newNode = newTreeNode(context, nextIp, here);
            if (!here)
                genTree(context, newNode, computeCrc);
            treeNode = &context->tree[nodeIdx];
            treeNode->next.push_back(newNode);
            const auto newNodePtr = &context->tree[newNode];
            newNodePtr->parent.push_back(nodeIdx);
        }

        if (treeNode->end->op != ByteCodeOp::Jump && treeNode->end->op != ByteCodeOp::End)
        {
            nextIp             = treeNode->end + 1;
            const auto newNode = newTreeNode(context, nextIp, here);
            if (!here)
                genTree(context, newNode, computeCrc);
            treeNode = &context->tree[nodeIdx];
            treeNode->next.push_back(newNode);
            const auto newNodePtr = &context->tree[newNode];
            newNodePtr->parent.push_back(nodeIdx);
        }
    }
    else
    {
        const auto nextIp  = treeNode->end + 1;
        const auto newNode = newTreeNode(context, nextIp, here);
        if (!here)
            genTree(context, newNode, computeCrc);
        treeNode = &context->tree[nodeIdx];
        treeNode->next.push_back(newNode);
        const auto newNodePtr = &context->tree[newNode];
        newNodePtr->parent.push_back(nodeIdx);
    }
}

void ByteCodeOptimizer::genTree(ByteCodeOptContext* context, bool computeCrc)
{
    context->mapInstNode.clear();
    context->nextTreeNode   = 0;
    context->contextBcFlags = 0;

    bool       here     = false;
    const auto treeNode = newTreeNode(context, context->bc->out, here);
    genTree(context, treeNode, computeCrc);
}

void ByteCodeOptimizer::parseTree(ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt)
{
    ByteCodeOptTreeNode* node = &context->tree[parseCxt.curNode];

    while (true)
    {
        // Back to first instruction
        if (node->flags & parseCxt.doneFlag && parseCxt.curIp == parseCxt.startIp)
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

void ByteCodeOptimizer::parseTree(ByteCodeOptContext*                                                           context,
                                  uint32_t                                                                      startNode,
                                  ByteCodeInstruction*                                                          startIp,
                                  uint32_t                                                                      doneFlag,
                                  const std::function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)>& cb)
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
    if (ip->hasFlag(BCI_NOT_PURE) && ip->op != ByteCodeOp::ClearRA)
        return;
    const auto flags = g_ByteCodeOpDesc[static_cast<int>(ip->op)].flags;
    if (flags.has(OPFLAG_NOT_PURE))
        return;

    SWAG_ASSERT(ip->op != ByteCodeOp::End);
    context->setDirtyPass();
    ip->op = ByteCodeOp::Nop;
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
            ++g_Stats.totalOptimBC;
#endif
            ip++;
            continue;
        }

        if (ByteCode::isJump(ip))
        {
            const auto srcJump = static_cast<int32_t>(ip - context->bc->out);
            const auto dstJump = srcJump + ip->b.s32 + 1;
            for (const auto nop : context->nops)
            {
                const auto idxNop = static_cast<int32_t>(nop - context->bc->out);

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
            const auto table = reinterpret_cast<int32_t*>(context->module->compilerSegment.address(ip->d.u32));
            for (uint32_t idx = 0; idx < ip->c.u32; idx++)
            {
                const auto srcJump = static_cast<int32_t>(ip - context->bc->out);
                const auto dstJump = srcJump + table[idx] + 1;
                for (const auto nop : context->nops)
                {
                    const auto idxNop = static_cast<int32_t>(nop - context->bc->out);
                    if (srcJump < dstJump && idxNop > srcJump && idxNop < dstJump)
                        table[idx]--;
                    else if (srcJump > dstJump && idxNop >= dstJump && idxNop < srcJump)
                        table[idx]++;
                }
            }
        }

        *ipDest++ = *ip++;
    }

    context->bc->numInstructions -= context->nops.size();
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

    for (uint32_t it = 0; it < context->jumps.size(); it++)
    {
        const auto jump = context->jumps[it];
        SWAG_ASSERT(ByteCode::isJumpOrDyn(jump));

        if (ByteCode::isJumpDyn(jump))
        {
            const auto table = reinterpret_cast<int32_t*>(context->module->compilerSegment.address(jump->d.u32));
            for (uint32_t i = 0; i < jump->c.u32; i++)
            {
                const auto ipDest = jump + table[i] + 1;
                if (jump < insert && ipDest > insert)
                {
                    table[i] += 1;
                    jump[table[i] + 1].addFlag(BCI_START_STMT);
                }
                else if (jump >= insert && ipDest < insert)
                {
                    table[i] -= 1;
                    jump[table[i] + 1].addFlag(BCI_START_STMT);
                }
            }
        }
        else
        {
            const auto ipDest = jump + jump->b.s32 + 1;
            if (jump < insert && ipDest > insert)
            {
                jump->b.s32 += 1;
                jump[jump->b.s32].addFlag(BCI_START_STMT);
            }
            else if (jump >= insert && ipDest < insert)
            {
                jump->b.s32 -= 1;
                jump[jump->b.s32].addFlag(BCI_START_STMT);
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
        ip->removeFlag(BCI_START_STMT);
        if (ByteCode::isJumpOrDyn(ip))
            context->jumps.push_back(ip);
    }

    // Mark all instructions which are a jump destination
    context->bc->numJumps = context->jumps.size();
    for (const auto jump : context->jumps)
    {
        if (ByteCode::isJumpDyn(jump))
        {
            const auto table = reinterpret_cast<int32_t*>(context->module->compilerSegment.address(jump->d.u64 & 0xFFFFFFFF));
            for (uint32_t i = 0; i < jump->c.u32; i++)
            {
                if (jump->hasFlag(BCI_SAFETY))
                    jump[table[i] + 1].addFlag(BCI_START_STMT_S);
                else
                    jump[table[i] + 1].addFlag(BCI_START_STMT_N);
            }
        }
        else
        {
            if (jump->hasFlag(BCI_SAFETY))
                jump[jump->b.s32 + 1].addFlag(BCI_START_STMT_S);
            else
                jump[jump->b.s32 + 1].addFlag(BCI_START_STMT_N);
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

        // Divide so that each job has the quite same amount of bytecode to optimize
        uint32_t totalInstructions = 0;
        for (const auto bc : module->byteCodeFunc)
        {
            if (bc->canEmit())
                totalInstructions += bc->numInstructions;
        }

        totalInstructions /= g_ThreadMgr.numWorkers * 4;
        totalInstructions = max(totalInstructions, 1);

        uint32_t startIndex = 0;
        while (startIndex < module->byteCodeFunc.size())
        {
            const auto newJob    = Allocator::alloc<ByteCodeOptimizerJob>();
            newJob->module       = module;
            newJob->dependentJob = job;
            newJob->startIndex   = startIndex;
            newJob->endIndex     = min(startIndex + 1, module->byteCodeFunc.size());
            startIndex           = newJob->endIndex;

            auto curInst = module->byteCodeFunc[newJob->startIndex]->numInstructions;
            while (curInst < totalInstructions && newJob->endIndex + 1 < module->byteCodeFunc.size())
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

#define OPT_PASS(__func)                                                          \
    do                                                                            \
    {                                                                             \
        optContext.passHasDoneSomething = false;                                  \
        if (!__func(&optContext))                                                 \
            return false;                                                         \
        if (optContext.hasError)                                                  \
            return false;                                                         \
        optContext.allPassesHaveDoneSomething |= optContext.passHasDoneSomething; \
    } while (0)

bool ByteCodeOptimizer::optimize(ByteCodeOptContext& optContext, ByteCode* bc, bool& restart)
{
    SWAG_RACE_CONDITION_WRITE(bc->raceCond);
    optContext.bc = bc;

    // Sanity must be done before any optimization, in order to have to deal with all extra instructions.
    if (bc->node && !bc->sanDone && optContext.module->mustEmitSafety(bc->node, SAFETY_SANITY))
    {
        bc->sanDone = true;
        setJumps(&optContext);
        genTree(&optContext, false);
        SWAG_CHECK(optimizePassSanity(&optContext));
    }

    if (!optContext.module->mustOptimizeBytecode(bc->node))
        return true;

    restart = false;
    while (true)
    {
        restart = restart || optContext.allPassesHaveDoneSomething;

        if (!bc->isEmpty && bc->isDoingNothing())
        {
            bc->isEmpty = true;
            restart     = true;
        }
        else if (bc->isEmpty)
            return true;

        optContext.allPassesHaveDoneSomething = false;

        setJumps(&optContext);
        genTree(&optContext, false);
        OPT_PASS(optimizePassJumps);
        OPT_PASS(optimizePassDeadCode);
        OPT_PASS(optimizePassImmediate);
        OPT_PASS(optimizePassImmediate2);
        OPT_PASS(optimizePassConst);
        OPT_PASS(optimizePassDupCopyRBRA);
        OPT_PASS(optimizePassDupCopy);
        OPT_PASS(optimizePassRetCopyLocal);
        OPT_PASS(optimizePassRetCopyGlobal);
        OPT_PASS(optimizePassRetCopyStructVal);
        OPT_PASS(optimizePassReduce);
        OPT_PASS(optimizePassDeadStore);
        OPT_PASS(optimizePassDeadStoreDup);
        OPT_PASS(optimizePassSwap);
        OPT_PASS(optimizePassParam);
        removeNops(&optContext);
        if (optContext.allPassesHaveDoneSomething)
            continue;

        setJumps(&optContext);
        genTree(&optContext, true);
        OPT_PASS(optimizePassErr);
        OPT_PASS(optimizePassLoop);
        OPT_PASS(optimizePassSwitch);
        OPT_PASS(optimizePassDupBlocks);
        OPT_PASS(optimizePassReduceX2);
        removeNops(&optContext);
        if (optContext.allPassesHaveDoneSomething)
            continue;

#ifdef SWAG_STATS
        if (g_CommandLine.statsFreq)
        {
            for (uint32_t i = 0; i < optContext.bc->numInstructions - 1; i++)
            {
                const auto ip = optContext.bc->out + i;
                ++g_Stats.countOpFreq[static_cast<int>(ip[0].op)][static_cast<int>(ByteCodeOp::End)];

                if (ip[1].hasFlag(BCI_START_STMT))
                    continue;
                ++g_Stats.countOpFreq[static_cast<int>(ip[0].op)][static_cast<int>(ip[1].op)];
            }
        }
#endif

        break;
    }

    return true;
}

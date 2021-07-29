#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeOptimizerJob.h"
#include "Module.h"

uint32_t ByteCodeOptimizer::newTreeNode(ByteCodeOptContext* context, ByteCodeInstruction* ip, bool& here)
{
    here = false;

    auto it = context->mapInstNode.find(ip);
    if (it != context->mapInstNode.end())
    {
        here = true;
        return it->second;
    }

    ByteCodeOptTreeNode newNode;
    newNode.start = ip;
    context->tree.push_back(newNode);
    uint32_t pos             = (uint32_t) context->tree.size() - 1;
    context->mapInstNode[ip] = pos;
    return pos;
}

void ByteCodeOptimizer::genTree(ByteCodeOptContext* context, uint32_t nodeIdx)
{
    ByteCodeOptTreeNode* node = &context->tree[nodeIdx];
    node->end                 = node->start;
    while (node->end->op != ByteCodeOp::Ret && !ByteCode::isJump(node->end))
        node->end++;
    if (node->end->op == ByteCodeOp::Ret)
        return;

    bool here = false;

    ByteCodeInstruction* nextIp  = node->end + node->end->b.s32 + 1;
    auto                 newNode = newTreeNode(context, nextIp, here);
    if (!here)
        genTree(context, newNode);
    node        = &context->tree[nodeIdx];
    node->next1 = newNode;

    if (node->end->op != ByteCodeOp::Jump)
    {
        nextIp  = node->end + 1;
        newNode = newTreeNode(context, nextIp, here);
        if (!here)
            genTree(context, newNode);
        node        = &context->tree[nodeIdx];
        node->next2 = newNode;
    }
}

void ByteCodeOptimizer::genTree(ByteCodeOptContext* context)
{
    context->tree.clear();
    context->mapInstNode.clear();

    auto bc   = context->bc;
    bool here = false;
    auto node = newTreeNode(context, bc->out, here);
    genTree(context, node);
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

        if (parseCxt.mustStopAll)
            return;
        if (parseCxt.mustStopBlock)
        {
            parseCxt.mustStopBlock = false;
            return;
        }

        if (parseCxt.curIp == node->end)
            break;
        parseCxt.curIp++;
    }

    if (node->next1 == UINT32_MAX)
        return;
    if (!(context->tree[node->next1].flags & parseCxt.doneFlag))
    {
        context->tree[node->next1].flags |= parseCxt.doneFlag;
        parseCxt.curNode = node->next1;
        parseCxt.curIp   = context->tree[node->next1].start;
        parseTree(context, parseCxt);
        if (parseCxt.mustStopAll)
            return;
    }

    if (node->next2 == UINT32_MAX)
        return;
    if (!(context->tree[node->next2].flags & parseCxt.doneFlag))
    {
        context->tree[node->next2].flags |= parseCxt.doneFlag;
        parseCxt.curNode = node->next2;
        parseCxt.curIp   = context->tree[node->next2].start;
        parseTree(context, parseCxt);
    }
}

void ByteCodeOptimizer::parseTree(ByteCodeOptContext* context, uint32_t startNode, ByteCodeInstruction* startIp, uint32_t doneFlag, function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb)
{
    for (auto& n : context->tree)
        n.flags &= ~doneFlag;

    ByteCodeOptTreeParseContext parseCxt;
    parseCxt.curNode  = startNode;
    parseCxt.startIp  = startIp;
    parseCxt.curIp    = startIp;
    parseCxt.cb       = cb;
    parseCxt.doneFlag = doneFlag;
    parseTree(context, parseCxt);
}

void ByteCodeOptimizer::replaceRegister(ByteCodeOptContext* context, ByteCodeInstruction* from, uint32_t srcReg, uint32_t dstReg)
{
    auto flags = g_ByteCodeOpDesc[(int) from->op].flags;
    if (flags & OPFLAG_WRITE_A && from->a.u32 == srcReg)
        from->a.u32 = dstReg;
    if (flags & OPFLAG_WRITE_B && from->b.u32 == srcReg)
        from->b.u32 = dstReg;
    if (flags & OPFLAG_WRITE_C && from->c.u32 == srcReg)
        from->c.u32 = dstReg;
    if (flags & OPFLAG_WRITE_D && from->d.u32 == srcReg)
        from->d.u32 = dstReg;

    set<ByteCodeInstruction*>          done;
    VectorNative<ByteCodeInstruction*> toScan;
    toScan.push_back(from);
    done.insert(from);
    while (!toScan.empty())
    {
        auto ip = toScan.back();
        toScan.pop_back();

        while (true)
        {
            flags = g_ByteCodeOpDesc[(int) ip->op].flags;
            if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A) && ip->a.u32 == srcReg)
                ip->a.u32 = dstReg;
            if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B) && ip->b.u32 == srcReg)
                ip->b.u32 = dstReg;
            if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C) && ip->c.u32 == srcReg)
                ip->c.u32 = dstReg;
            if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D) && ip->d.u32 == srcReg)
                ip->d.u32 = dstReg;

            if (flags & OPFLAG_WRITE_A && ip->a.u32 == srcReg)
                break;
            if (flags & OPFLAG_WRITE_B && ip->b.u32 == srcReg)
                break;
            if (flags & OPFLAG_WRITE_C && ip->c.u32 == srcReg)
                break;
            if (flags & OPFLAG_WRITE_D && ip->d.u32 == srcReg)
                break;

            if (ip->op == ByteCodeOp::Ret)
                break;

            if (ByteCode::isJump(ip))
            {
                if (done.find(ip + ip->b.s32 + 1) == done.end())
                {
                    toScan.push_back(ip + ip->b.s32 + 1);
                    done.insert(ip + ip->b.s32 + 1);
                }

                if (ip->op != ByteCodeOp::Jump)
                {
                    if (done.find(ip + 1) != done.end())
                        break;
                    done.insert(ip + 1);
                }
            }

            ip++;
            if (ip == from)
                break;
        }
    }
}

void ByteCodeOptimizer::setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::Nop || (ip->flags & BCI_UNPURE))
        return;
    auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;
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
            if (g_CommandLine.stats)
                g_Stats.totalOptimsBC = g_Stats.totalOptimsBC + 1;
            ip++;
            continue;
        }

        if (ByteCode::isJump(ip))
        {
            auto srcJump = (int) (ip - context->bc->out);
            auto dstJump = srcJump + ip->b.s32 + 1;
            for (auto nop : context->nops)
            {
                auto idxNop = (int) (nop - context->bc->out);

                // If this is a ordered jump, and there's a nop between the destination jump
                // and the jump, then we need to remove one jump instruction
                // If we jump on a nop, we must NOT decrease by 1 to jump to the following instruction
                if (srcJump < dstJump && idxNop > srcJump && idxNop < dstJump)
                {
                    ip->b.s32--;
                }

                // If this is a back jump, and there's a nop between the destination jump
                // and the jump, then we need to remove one jump instruction
                // If we jump on a nop, we MUST decrease also by 1 to jump to the following instruction
                else if (srcJump > dstJump && idxNop >= dstJump && idxNop < srcJump)
                {
                    ip->b.s32++;
                }
            }
        }

        *ipDest++ = *ip++;
    }

    context->bc->numInstructions -= (int) context->nops.size();
    context->nops.clear();
}

void ByteCodeOptimizer::setJumps(ByteCodeOptContext* context)
{
    context->jumps.clear();
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        ip->flags &= ~BCI_START_STMT;
        if (ByteCode::isJump(ip))
            context->jumps.push_back(ip);
    }

    // Mark all instructions which are a jump destination
    for (auto jump : context->jumps)
        jump[jump->b.s32 + 1].flags |= BCI_START_STMT;
}

bool ByteCodeOptimizer::optimize(Job* job, Module* module, bool& done)
{
    done = true;
    if (module->numTestErrors || module->byteCodeFunc.empty())
        return true;

    done = false;
    if (module->numErrors)
        return false;

    // Determin if we need to restart the whole optim pass because something has been done
    if (module->optimPass == 1)
    {
        if (module->optimNeedRestart.load() > 0)
            module->optimPass = 0;
        else
        {
            done = true;
            return true;
        }
    }

    if (module->optimPass == 0)
    {
        module->optimNeedRestart.store(0);
        auto count     = (int) module->byteCodeFunc.size() / g_Stats.numWorkers;
        count          = max(count, 1);
        count          = min(count, (int) module->byteCodeFunc.size());
        int startIndex = 0;
        while (startIndex < module->byteCodeFunc.size())
        {
            auto newJob          = g_Allocator.alloc<ByteCodeOptimizerJob>();
            newJob->module       = module;
            newJob->startIndex   = startIndex;
            newJob->endIndex     = min(startIndex + count, (int) module->byteCodeFunc.size());
            newJob->dependentJob = job;
            startIndex += count;
            job->jobsToAdd.push_back(newJob);
        }

        module->optimPass = 1;
        done              = false;
        return true;
    }

    return true;
}
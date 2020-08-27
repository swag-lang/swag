#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOptimizer.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeOp.h"
#include "Module.h"

bool ByteCodeOptimizer::optimizeJumps(ByteCodeOptContext* context)
{
    bool hasDoneSomething = false;
    for (int idx = 0; idx < context->jumps.size(); idx++)
    {
        auto ip = context->jumps[idx];

        // Jump to another jump
        auto destIp = ip + ip->b.s32 + 1;
        while (destIp->op == ByteCodeOp::Jump)
        {
            ip->b.s32 += destIp->b.s32 + 1;
            destIp += destIp->b.s32 + 1;
            hasDoneSomething = true;
        }

        // Jump to the next instruction
        if (ip->b.s32 == 0)
        {
            setNop(context, ip);
            hasDoneSomething = true;
            context->jumps.erase_unordered(idx);
        }

        // Next instruction is a nop, move the jump forward if the jump is positive
        else if (ip[1].op == ByteCodeOp::Nop && ip->b.s32 > 0)
        {
            std::swap(ip[0], ip[1]);
            ip[1].b.s32--;

            context->jumps[idx] = ip + 1;
            hasDoneSomething    = true;
        }

        if (ip->op == ByteCodeOp::Jump)
        {
            // Next instruction is a jump to the same target
            if (ip[1].op == ByteCodeOp::Jump && (ip->b.s32 - 1 == ip[1].b.s32))
            {
                setNop(context, ip);
                hasDoneSomething = true;
                context->jumps.erase_unordered(idx);
            }
        }
    }

    return hasDoneSomething;
}

bool ByteCodeOptimizer::removeNops(ByteCodeOptContext* context)
{
    bool hasDoneSomething = false;

    // Adapt all jumps
    for (int idx = 0; idx < context->jumps.size(); idx++)
    {
        auto ip      = context->jumps[idx];
        auto srcJump = (int) (ip - context->bc->out);
        auto dstJump = srcJump + ip->b.s32 + 1;

        for (auto nop : context->nops)
        {
            auto idxNop = (int) (nop - context->bc->out);
            if (srcJump < dstJump && idxNop > srcJump && idxNop <= dstJump)
            {
                ip->b.s32--;
                hasDoneSomething = true;
            }
            else if (srcJump > dstJump && idxNop >= dstJump && idxNop < srcJump)
            {
                ip->b.s32++;
                hasDoneSomething = true;
            }
        }
    }

    if (!hasDoneSomething)
        return false;

    auto ip     = context->bc->out;
    auto ipDest = context->bc->out;
    for (uint32_t i = 0; i < context->bc->numInstructions; i++)
    {
        if (ip->op != ByteCodeOp::Nop)
            *ipDest++ = *ip++;
        else
            ip++;
    }

    context->bc->numInstructions -= (int) context->nops.size();
    context->nops.clear();
    return true;
}

bool ByteCodeOptimizer::setJumps(ByteCodeOptContext* context)
{
    context->jumps.clear();
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::Jump ||
            ip->op == ByteCodeOp::JumpIfTrue ||
            ip->op == ByteCodeOp::JumpIfFalse ||
            ip->op == ByteCodeOp::JumpIfZero32 ||
            ip->op == ByteCodeOp::JumpIfZero64 ||
            ip->op == ByteCodeOp::JumpIfNotZero32 ||
            ip->op == ByteCodeOp::JumpIfNotZero64)
        {
            context->jumps.push_back(ip);
        }
    }

    return !context->jumps.empty();
}

void ByteCodeOptimizer::optimize(ByteCodeGenContext* context)
{
    if (!context->bc)
        return;

    auto job    = context->job;
    auto module = job->originalNode->sourceFile->module;
    //if (module->mustOptimizeBC(job->originalNode) < 2)
    //   return;
    if (job->originalNode->sourceFile->name == "compiler1904.swg")
        job = job;

    ByteCodeOptContext optContext;
    optContext.bc = context->bc;

    // Get all jumps

    while (true)
    {
        if (!setJumps(&optContext))
            break;
        if (!optimizeJumps(&optContext))
            break;
        removeNops(&optContext);
    }
}
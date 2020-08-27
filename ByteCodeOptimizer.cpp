#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOptimizer.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeOp.h"
#include "Module.h"

inline void setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    ip->op = ByteCodeOp::Nop;
    context->nops.push_back(ip);
}

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

void ByteCodeOptimizer::optimize(ByteCodeGenContext* context)
{
    if (!context->bc)
        return;

    auto job    = context->job;
    auto module = job->originalNode->sourceFile->module;
    //if (module->mustOptimizeBC(job->originalNode) < 2)
    //    return;

    ByteCodeOptContext optContext;
    optContext.bc = context->bc;

    // Get all jumps
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
            optContext.jumps.push_back(ip);
        }
    }

    while (optimizeJumps(&optContext)) {}
}
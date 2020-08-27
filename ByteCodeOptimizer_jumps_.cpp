#include "pch.h"
#include "ByteCodeOptimizer.h"

bool optimizeJumps(ByteCodeOptContext* context)
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
            ByteCodeOptimizer::setNop(context, ip);
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
                ByteCodeOptimizer::setNop(context, ip);
                hasDoneSomething = true;
                context->jumps.erase_unordered(idx);
            }
        }
    }

    context->passHasDoneSomething |= hasDoneSomething;
    return hasDoneSomething;
}

void ByteCodeOptimizer::optimizePassJumps(ByteCodeOptContext* context)
{
    while (optimizeJumps(context)) {};
    removeNops(context);
}
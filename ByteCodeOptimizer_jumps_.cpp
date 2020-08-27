#include "pch.h"
#include "ByteCodeOptimizer.h"

void ByteCodeOptimizer::optimizePassJumps(ByteCodeOptContext* context)
{
    for (int idx = 0; idx < context->jumps.size(); idx++)
    {
        auto ip = context->jumps[idx];

        // Jump to an unconditional jump
        auto destIp = ip + ip->b.s32 + 1;
        while (destIp->op == ByteCodeOp::Jump)
        {
            ip->b.s32 += destIp->b.s32 + 1;
            destIp += destIp->b.s32 + 1;
            context->passHasDoneSomething = true;
        }

        // Jump to the next instruction
        if (ip->b.s32 == 0)
        {
            setNop(context, ip);
            context->jumps.erase_unordered(idx);
        }

        if (ip->op == ByteCodeOp::Jump)
        {
            // Next instruction is a jump to the same target
            if (ip[1].op == ByteCodeOp::Jump && (ip->b.s32 - 1 == ip[1].b.s32))
            {
                setNop(context, ip);
                context->jumps.erase_unordered(idx);
            }
        }
    }
}

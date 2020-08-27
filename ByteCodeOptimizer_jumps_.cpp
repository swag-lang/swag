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
            setNop(context, ip);

        if (ip->op == ByteCodeOp::Jump)
        {
            // Next instruction is a jump to the same target
            if (ip[1].op == ByteCodeOp::Jump && (ip->b.s32 - 1 == ip[1].b.s32))
                setNop(context, ip);
        }

        // If a conditional jump has a constant value (preceded with a constant assign to the register that
        // will be tested by the jump condition), then we can evaluate the jump right now
        if (ip->op != ByteCodeOp::Jump &&
            !(ip->flags & BCI_START_STMT) &&
            ip != context->bc->out &&
            ip[-1].op == ByteCodeOp::CopyRAVB32 &&
            ip[-1].a.u32 == ip->a.u32)
        {
            switch (ip->op)
            {
            case ByteCodeOp::JumpIfFalse:
                context->passHasDoneSomething = true;
                if (ip[-1].b.b)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfTrue:
                context->passHasDoneSomething = true;
                if (!ip[-1].b.b)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfNotZero32:
                context->passHasDoneSomething = true;
                if (!ip[-1].b.u32)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfZero32:
                context->passHasDoneSomething = true;
                if (ip[-1].b.u32)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            }
        }
    }
}

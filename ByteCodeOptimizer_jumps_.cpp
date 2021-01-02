#include "pch.h"
#include "ByteCodeOptimizer.h"

// Eliminate unnecessary jumps
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

        // Jump if false to a jump if false with the same register
        if (ip->op == ByteCodeOp::JumpIfFalse)
        {
            destIp = ip + ip->b.s32 + 1;
            if (destIp->op == ByteCodeOp::JumpIfFalse && destIp->a.u32 == ip->a.u32)
            {
                ip->b.s32 += destIp->b.s32 + 1;
                context->passHasDoneSomething = true;
            }
        }

        // Jump if true to a jump if true with the same register
        if (ip->op == ByteCodeOp::JumpIfTrue)
        {
            destIp = ip + ip->b.s32 + 1;
            if (destIp->op == ByteCodeOp::JumpIfTrue && destIp->a.u32 == ip->a.u32)
            {
                ip->b.s32 += destIp->b.s32 + 1;
                context->passHasDoneSomething = true;
            }
        }

        // Remove empty loop
        if (ip->op == ByteCodeOp::JumpIfNotZero32 && ip->b.s32 == -2)
        {
            if (ip[-1].op == ByteCodeOp::DecrementRA32 && ip[-1].a.u32 == ip->a.u32)
            {
                setNop(context, ip);
                setNop(context, ip - 1);
            }
        }

        // If we have :
        // 0: (jump if false) to 2
        // 1: (jump) to whatever
        // 2: ...
        // Then we switch to (jump if true) whatever, and eliminate the unconditional jump
        if (ip->op == ByteCodeOp::JumpIfFalse && ip->b.s32 == 1 && ip[1].op == ByteCodeOp::Jump)
        {
            ip->op    = ByteCodeOp::JumpIfTrue;
            ip->b.s32 = ip[1].b.s32 + 1;
            setNop(context, ip + 1);
        }

        if (ip->op == ByteCodeOp::JumpIfTrue && ip->b.s32 == 1 && ip[1].op == ByteCodeOp::Jump)
        {
            ip->op    = ByteCodeOp::JumpIfFalse;
            ip->b.s32 = ip[1].b.s32 + 1;
            setNop(context, ip + 1);
        }

        // Evaluate the jump if the condition is constant
        if (ip->op != ByteCodeOp::Jump && (ip->flags & BCI_IMM_A))
        {
            switch (ip->op)
            {
            case ByteCodeOp::JumpIfFalse:
                context->passHasDoneSomething = true;
                if (ip->a.u8)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfTrue:
                context->passHasDoneSomething = true;
                if (!ip->a.u8)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfZero8:
                context->passHasDoneSomething = true;
                if (ip->a.u8)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfZero16:
                context->passHasDoneSomething = true;
                if (ip->a.u16)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfZero32:
                context->passHasDoneSomething = true;
                if (ip->a.u32)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfZero64:
                context->passHasDoneSomething = true;
                if (ip->a.u64)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfNotZero32:
                context->passHasDoneSomething = true;
                if (!ip->a.u32)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            case ByteCodeOp::JumpIfNotZero64:
                context->passHasDoneSomething = true;
                if (!ip->a.u64)
                    setNop(context, ip);
                else
                    ip->op = ByteCodeOp::Jump;
                break;
            }
        }
    }
}

#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "AstNode.h"
#include "SourceFile.h"

// Eliminate unnecessary jumps
bool ByteCodeOptimizer::optimizePassJumps(ByteCodeOptContext* context)
{
    for (int idx = 0; idx < context->jumps.size(); idx++)
    {
        auto ip = context->jumps[idx];
        if (!ByteCode::isJump(ip))
            continue;

        // Jump to an unconditional jump
        auto destIp = ip + ip->b.s32 + 1;
        while (destIp->op == ByteCodeOp::Jump)
        {
            if (destIp->b.s32 + 1 == 0) // infinite jump
                break;
            ip->b.s32 += destIp->b.s32 + 1;
            destIp += destIp->b.s32 + 1;
            context->passHasDoneSomething = true;
        }

        // Jump to the next instruction
        if (ip->b.s32 == 0)
            setNop(context, ip);

        // Jump conditional followed by unconditional jump
        // Revert the condition to remove the unconditional jump
        switch (ip->op)
        {
            //
        case ByteCodeOp::JumpIfGreaterF32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerEqF32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfGreaterEqF32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerF32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerF32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterEqF32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerEqF32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterF32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

            //
        case ByteCodeOp::JumpIfGreaterF64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerEqF64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfGreaterEqF64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerF64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerF64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterEqF64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerEqF64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterF64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

            //
        case ByteCodeOp::JumpIfGreaterS32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerEqS32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfGreaterEqS32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerS32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerS32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerEqS32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterS32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

            //
        case ByteCodeOp::JumpIfGreaterS64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerEqS64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfGreaterEqS64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerS64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerS64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerEqS64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterS64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

            //
        case ByteCodeOp::JumpIfGreaterU32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerEqU32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfGreaterEqU32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerU32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerU32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerEqU32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterU32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

            //
        case ByteCodeOp::JumpIfGreaterU64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerEqU64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfGreaterEqU64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfLowerU64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerU64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfLowerEqU64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfGreaterU64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

            //
        case ByteCodeOp::JumpIfFalse:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfTrue);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfTrue:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfFalse);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotZero8:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfZero8);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotZero16:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfZero16);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotZero32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfZero32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotZero64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfZero64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfZero8:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotZero8);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfZero16:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotZero16);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfZero32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotZero32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfZero64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotZero64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfEqual8:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotEqual8);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfEqual16:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotEqual16);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfEqual32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotEqual32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfEqual64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfNotEqual64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotEqual8:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfEqual8);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotEqual16:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfEqual16);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotEqual32:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfEqual32);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotEqual64:
            if (ip[1].op == ByteCodeOp::Jump &&
                ip[0].b.s32 == 1 &&
                !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfEqual64);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;
        }

        switch (ip->op)
        {
        case ByteCodeOp::Jump:
            // Next instruction is a jump to the same target
            if (ip[1].op == ByteCodeOp::Jump && (ip->b.s32 - 1 == ip[1].b.s32))
            {
                setNop(context, ip);
            }

            // Jump to a JumpIfZero, which in turns jumps right after the Jump id really zero
            // (occurs in loops).
            // Replace Jump with jumpIfNotZero
            else if (ip->b.s32 < 0)
            {
                destIp = ip + ip->b.s32 + 1;
                if (destIp->op == ByteCodeOp::JumpIfZero64)
                {
                    auto destIp1 = destIp + destIp->b.s32 + 1;
                    if (destIp1 == ip + 1)
                    {
                        SET_OP(ip, ByteCodeOp::JumpIfNotZero64);
                        ip->a.u32 = destIp->a.u32;
                        ip->b.s32 += 1;
                    }
                }
            }
            break;

        case ByteCodeOp::JumpIfFalse:
            destIp = ip + ip->b.s32 + 1;

            // Jump if false to a jump if false with the same register
            if (destIp->op == ByteCodeOp::JumpIfFalse &&
                destIp->a.u32 == ip->a.u32 &&
                !(ip->flags & BCI_IMM_A) &&
                (destIp->b.s32 + 1) && // in case it's an empty loop
                !(destIp->flags & BCI_IMM_A))
            {
                ip->b.s32 += destIp->b.s32 + 1;
                context->passHasDoneSomething = true;
            }

            // Jump if false to a jump if true with the same register
            else if (destIp->op == ByteCodeOp::JumpIfTrue &&
                     destIp->a.u32 == ip->a.u32 &&
                     !(ip->flags & BCI_IMM_A) &&
                     !(destIp->flags & BCI_IMM_A))
            {
                ip->b.s32 += 1;
                destIp[1].flags |= BCI_START_STMT;
                context->passHasDoneSomething = true;
            }

            // If we have :
            // 0: (jump if false) to 2
            // 1: (jump) to whatever
            // 2: ...
            // Then we switch to (jump if true) whatever, and eliminate the unconditional jump
            else if (ip->b.s32 == 1 &&
                     ip[1].op == ByteCodeOp::Jump &&
                     !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfTrue);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfTrue:
            destIp = ip + ip->b.s32 + 1;

            // Jump if true to a jump if true with the same register
            if (destIp->op == ByteCodeOp::JumpIfTrue &&
                destIp->a.u32 == ip->a.u32 &&
                !(ip->flags & BCI_IMM_A) &&
                (destIp->b.s32 + 1) && // in case it's an empty loop
                !(destIp->flags & BCI_IMM_A))
            {
                ip->b.s32 += destIp->b.s32 + 1;
                context->passHasDoneSomething = true;
            }

            // Jump if true to a jump if false with the same register
            else if (destIp->op == ByteCodeOp::JumpIfFalse &&
                     destIp->a.u32 == ip->a.u32 &&
                     !(ip->flags & BCI_IMM_A) &&
                     !(destIp->flags & BCI_IMM_A))
            {
                ip->b.s32 += 1;
                destIp[1].flags |= BCI_START_STMT;
                context->passHasDoneSomething = true;
            }

            // If we have :
            // 0: (jump if true) to 2
            // 1: (jump) to whatever
            // 2: ...
            // Then we switch to (jump if false) whatever, and eliminate the unconditional jump
            else if (ip->b.s32 == 1 &&
                     ip[1].op == ByteCodeOp::Jump &&
                     !(ip[1].flags & BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::JumpIfFalse);
                ip->b.s32 = ip[1].b.s32 + 1;
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::JumpIfNotZero32:
            if (ip->b.s32 == -2)
            {
                if (ip[-1].op == ByteCodeOp::DecrementRA32 && ip[-1].a.u32 == ip->a.u32)
                {
                    setNop(context, ip);
                    setNop(context, ip - 1);
                }
            }
            break;

        case ByteCodeOp::JumpIfNotZero64:
            if (ip->b.s32 == -2)
            {
                if (ip[-1].op == ByteCodeOp::DecrementRA64 && ip[-1].a.u32 == ip->a.u32)
                {
                    setNop(context, ip);
                    setNop(context, ip - 1);
                }
            }
            break;
        }

#define OPT_JMPAC(__op, __val)        \
    if (ip->a.__val __op ip->c.__val) \
    {                                 \
        SET_OP(ip, ByteCodeOp::Jump); \
    }                                 \
    else                              \
    {                                 \
        setNop(context, ip);          \
    }

        if (ip->flags & BCI_IMM_A && ip->flags & BCI_IMM_C)
        {
            switch (ip->op)
            {
            case ByteCodeOp::JumpIfEqual8:
                OPT_JMPAC(==, u8);
                break;
            case ByteCodeOp::JumpIfEqual16:
                OPT_JMPAC(==, u16);
                break;
            case ByteCodeOp::JumpIfEqual32:
                OPT_JMPAC(==, u32);
                break;
            case ByteCodeOp::JumpIfEqual64:
                OPT_JMPAC(==, u64);
                break;

            case ByteCodeOp::JumpIfNotEqual8:
                OPT_JMPAC(!=, u8);
                break;
            case ByteCodeOp::JumpIfNotEqual16:
                OPT_JMPAC(!=, u16);
                break;
            case ByteCodeOp::JumpIfNotEqual32:
                OPT_JMPAC(!=, u32);
                break;
            case ByteCodeOp::JumpIfNotEqual64:
                OPT_JMPAC(!=, u64);
                break;

            case ByteCodeOp::JumpIfGreaterS32:
                OPT_JMPAC(>, s32);
                break;
            case ByteCodeOp::JumpIfGreaterS64:
                OPT_JMPAC(>, s64);
                break;
            case ByteCodeOp::JumpIfGreaterU32:
                OPT_JMPAC(>, u32);
                break;
            case ByteCodeOp::JumpIfGreaterU64:
                OPT_JMPAC(>, u64);
                break;
            case ByteCodeOp::JumpIfGreaterF32:
                OPT_JMPAC(>, f32);
                break;
            case ByteCodeOp::JumpIfGreaterF64:
                OPT_JMPAC(>, f64);
                break;

            case ByteCodeOp::JumpIfGreaterEqS32:
                OPT_JMPAC(>=, s32);
                break;
            case ByteCodeOp::JumpIfGreaterEqS64:
                OPT_JMPAC(>=, s64);
                break;
            case ByteCodeOp::JumpIfGreaterEqU32:
                OPT_JMPAC(>=, u32);
                break;
            case ByteCodeOp::JumpIfGreaterEqU64:
                OPT_JMPAC(>=, u64);
                break;
            case ByteCodeOp::JumpIfGreaterEqF32:
                OPT_JMPAC(>=, f32);
                break;
            case ByteCodeOp::JumpIfGreaterEqF64:
                OPT_JMPAC(>=, f64);
                break;

            case ByteCodeOp::JumpIfLowerS32:
                OPT_JMPAC(<, s32);
                break;
            case ByteCodeOp::JumpIfLowerS64:
                OPT_JMPAC(<, s64);
                break;
            case ByteCodeOp::JumpIfLowerU32:
                OPT_JMPAC(<, u32);
                break;
            case ByteCodeOp::JumpIfLowerU64:
                OPT_JMPAC(<, u64);
                break;
            case ByteCodeOp::JumpIfLowerF32:
                OPT_JMPAC(<, f32);
                break;
            case ByteCodeOp::JumpIfLowerF64:
                OPT_JMPAC(<, f64);
                break;

            case ByteCodeOp::JumpIfLowerEqS32:
                OPT_JMPAC(<=, s32);
                break;
            case ByteCodeOp::JumpIfLowerEqS64:
                OPT_JMPAC(<=, s64);
                break;
            case ByteCodeOp::JumpIfLowerEqU32:
                OPT_JMPAC(<=, u32);
                break;
            case ByteCodeOp::JumpIfLowerEqU64:
                OPT_JMPAC(<=, u64);
                break;
            case ByteCodeOp::JumpIfLowerEqF32:
                OPT_JMPAC(<=, f32);
                break;
            case ByteCodeOp::JumpIfLowerEqF64:
                OPT_JMPAC(<=, f64);
                break;
            }
        }

#define OPT_JMPA0(__op, __val)        \
    if (ip->a.__val __op 0)           \
    {                                 \
        SET_OP(ip, ByteCodeOp::Jump); \
    }                                 \
    else                              \
    {                                 \
        setNop(context, ip);          \
    }

        // Evaluate the jump if the condition is constant
        if (ip->flags & BCI_IMM_A)
        {
            switch (ip->op)
            {
            case ByteCodeOp::JumpIfFalse:
            case ByteCodeOp::JumpIfZero8:
                OPT_JMPA0(==, u8);
                break;
            case ByteCodeOp::JumpIfTrue:
            case ByteCodeOp::JumpIfNotZero8:
                OPT_JMPA0(!=, u8);
                break;
            case ByteCodeOp::JumpIfZero16:
                OPT_JMPA0(==, u16);
                break;
            case ByteCodeOp::JumpIfZero32:
                OPT_JMPA0(==, u32);
                break;
            case ByteCodeOp::JumpIfZero64:
                OPT_JMPA0(==, u64);
                break;
            case ByteCodeOp::JumpIfNotZero16:
                OPT_JMPA0(!=, u16);
                break;
            case ByteCodeOp::JumpIfNotZero32:
                OPT_JMPA0(!=, u32);
                break;
            case ByteCodeOp::JumpIfNotZero64:
                OPT_JMPA0(!=, u64);
                break;
            }
        }
    }

    return true;
}

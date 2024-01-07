#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeGen.h"
#include "Tokenizer.h"
#include "Log.h"
#include "AstNode.h"
#include "Diagnostic.h"
#include "SourceFile.h"

// If an instruction can have an immediate form, then transform it if the corresponding
// register is a constant.
bool ByteCodeOptimizer::optimizePassImmediate(ByteCodeOptContext* context)
{
    auto& regs = context->mapRegInstA;
    regs.clear();

    auto& regsRW = context->mapRegU64;
    regsRW.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Do not care about jump destination from safety blocks
        if (ip->flags & BCI_START_STMT_N)
            regs.clear();

        auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

        bool optPostWrite = false;
        switch (ip->op)
        {
        case ByteCodeOp::ClearRA:
            regsRW.set(ip->a.u32, 0);
            regs.set(ip->a.u32, ip);
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;

        case ByteCodeOp::ClearRAx2:
            regsRW.set(ip->a.u32, 0);
            regsRW.set(ip->b.u32, 0);
            regs.set(ip->a.u32, ip);
            regs.set(ip->b.u32, ip);
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;

        case ByteCodeOp::ClearRAx3:
            regsRW.set(ip->a.u32, 0);
            regsRW.set(ip->b.u32, 0);
            regsRW.set(ip->c.u32, 0);
            regs.set(ip->a.u32, ip);
            regs.set(ip->b.u32, ip);
            regs.set(ip->c.u32, ip);
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;

        case ByteCodeOp::ClearRAx4:
            regsRW.set(ip->a.u32, 0);
            regsRW.set(ip->b.u32, 0);
            regsRW.set(ip->c.u32, 0);
            regsRW.set(ip->d.u32, 0);
            regs.set(ip->a.u32, ip);
            regs.set(ip->b.u32, ip);
            regs.set(ip->c.u32, ip);
            regs.set(ip->d.u32, ip);
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;

        case ByteCodeOp::SetImmediate32:
            regsRW.set(ip->a.u32, ip->b.u32);
            regs.set(ip->a.u32, ip);
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;

        case ByteCodeOp::SetImmediate64:
            regsRW.set(ip->a.u32, ip->b.u64);
            regs.set(ip->a.u32, ip);
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;

        default:
            if (ip->flags & BCI_START_STMT)
                regs.clear();
            if (!regs.count)
                break;

            optPostWrite = true;

            // Read/write to A, and A is a constant, we store the current value in B. The constant folding pass can take care of that
            // depending on the instruction
            if (!(ip->flags & BCI_IMM_B) && (flags & OPFLAG_READ_A) && (flags & OPFLAG_WRITE_A) && regs.contains(ip->a.u32) &&
                !(flags & OPFLAG_READ_B) && !(flags & OPFLAG_READ_VAL32_B) && !(flags & OPFLAG_READ_VAL64_B))
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_B;
                regs.remove(ip->a.u32);
                ip->b.u64 = regsRW.val[ip->a.u32];
                break;
            }

            if (!(ip->flags & BCI_IMM_B) && (flags & OPFLAG_WRITE_A) && (flags & OPFLAG_READ_B) && regs.contains(ip->a.u32) &&
                ip->a.u32 == ip->b.u32)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_B;
                regs.remove(ip->a.u32);
                ip->b.u64 = regsRW.val[ip->a.u32];
                break;
            }

            // Read/write to A, and A is a constant, we store the current value in C. The constant folding pass can take care of that
            // depending on the instruction
            if (!(ip->flags & BCI_IMM_C) && (flags & OPFLAG_READ_A) && (flags & OPFLAG_WRITE_A) && regs.contains(ip->a.u32) &&
                !(flags & OPFLAG_READ_C) && !(flags & OPFLAG_READ_VAL32_C) && !(flags & OPFLAG_READ_VAL64_C))
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_C;
                regs.remove(ip->a.u32);
                ip->c.u64 = regsRW.val[ip->a.u32];
                break;
            }

            // Operators can read from A and write to C, with A == C.
            // In that case we want the optim to take place on A if it's immediate, so do not cancel it for C.
            if ((flags & OPFLAG_READ_A) && (flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_A) && regs.contains(ip->a.u32))
                break;

            if (flags & OPFLAG_WRITE_A)
                regs.remove(ip->a.u32);
            if (flags & OPFLAG_WRITE_B)
                regs.remove(ip->b.u32);
            if (flags & OPFLAG_WRITE_C)
                regs.remove(ip->c.u32);
            if (flags & OPFLAG_WRITE_D)
                regs.remove(ip->d.u32);
            break;
        }

        if (!regs.count)
            continue;

        if (!(ip->flags & BCI_IMM_A) && (flags & OPFLAG_READ_A) && regs.contains(ip->a.u32))
        {
            if (flags & OPFLAG_IMM_A)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_A;
                regs.remove(ip->a.u32);
                ip->a.u64 = regsRW.val[ip->a.u32];
            }
        }

        if (!(ip->flags & BCI_IMM_B) && (flags & OPFLAG_READ_B) && regs.contains(ip->b.u32))
        {
            if (flags & OPFLAG_IMM_B)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_B;
                regs.remove(ip->b.u32);
                ip->b.u64 = regsRW.val[ip->b.u32];
            }
            else if (ip->op == ByteCodeOp::CopyRBtoRA64)
            {
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                regs.remove(ip->b.u32);
                ip->b.u64 = regsRW.val[ip->b.u32];
                flags     = g_ByteCodeOpDesc[(int) ip->op].flags;
            }
        }

        if (!(ip->flags & BCI_IMM_C) && (flags & OPFLAG_READ_C) && regs.contains(ip->c.u32))
        {
            if (flags & OPFLAG_IMM_C)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_C;
                regs.remove(ip->c.u32);
                ip->c.u64 = regsRW.val[ip->c.u32];
            }
        }

        if (!(ip->flags & BCI_IMM_D) && (flags & OPFLAG_READ_D) && regs.contains(ip->d.u32))
        {
            if (flags & OPFLAG_IMM_D)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_D;
                regs.remove(ip->d.u32);
                ip->d.u64 = regsRW.val[ip->d.u32];
            }
        }

        if (optPostWrite)
        {
            if (flags & OPFLAG_WRITE_A)
                regs.remove(ip->a.u32);
            if (flags & OPFLAG_WRITE_B)
                regs.remove(ip->b.u32);
            if (flags & OPFLAG_WRITE_C)
                regs.remove(ip->c.u32);
            if (flags & OPFLAG_WRITE_D)
                regs.remove(ip->d.u32);
        }

        if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
            regs.remove(ip->a.u32);
        if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
            regs.remove(ip->b.u32);
        if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
            regs.remove(ip->c.u32);
        if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
            regs.remove(ip->d.u32);
    }

    return true;
}

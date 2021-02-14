#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeGenJob.h"
#include "Tokenizer.h"
#include "Log.h"
#include "AstNode.h"
#include "Diagnostic.h"
#include "SourceFile.h"

// If an instruction can have an immediate form, then transform it if the corresponding
// register is a constant.
void ByteCodeOptimizer::optimizePassImmediate(ByteCodeOptContext* context)
{
    auto  maxReg = context->bc->maxReservedRegisterRC;
    auto& regs   = context->vecInst;
    regs.reserve(maxReg);
    regs.count = maxReg;
    memset(regs.buffer, 0, maxReg * sizeof(void*));

    auto& regsRW = context->vecU64;
    regsRW.reserve(maxReg);
    regsRW.count = maxReg;
    memset(regsRW.buffer, 0, maxReg * sizeof(uint64_t));

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT)
            memset(regs.buffer, 0, maxReg * sizeof(void*));

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

        switch (ip->op)
        {
        case ByteCodeOp::ClearRA:
            regsRW[ip->a.u32] = 0;
            regs[ip->a.u32]   = ip;
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;
        case ByteCodeOp::SetImmediate32:
            regsRW[ip->a.u32] = ip->b.u32;
            regs[ip->a.u32]   = ip;
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;
        case ByteCodeOp::SetImmediate64:
            regsRW[ip->a.u32] = ip->b.u64;
            regs[ip->a.u32]   = ip;
            ip->flags &= ~BCI_IMM_C;
            ip->flags &= ~BCI_IMM_D;
            break;
        default:
            // Read/write to A, and A is a constant, we store the current value in B. The constant folding pass can take care of that
            // depending on the instruction
            if (!(ip->flags & BCI_IMM_B) && (flags & OPFLAG_READ_A) && (flags & OPFLAG_WRITE_A) && regs[ip->a.u32] &&
                !(flags & OPFLAG_READ_B) && !(flags & OPFLAG_READ_VAL32_B) && !(flags & OPFLAG_READ_VAL64_B))
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_B;
                regs[ip->a.u32] = nullptr;
                ip->b.u64       = regsRW[ip->a.u32];
            }
            // Read/write to A, and A is a constant, we store the current value in C. The constant folding pass can take care of that
            // depending on the instruction
            if (!(ip->flags & BCI_IMM_C) && (flags & OPFLAG_READ_A) && (flags & OPFLAG_WRITE_A) && regs[ip->a.u32] &&
                !(flags & OPFLAG_READ_C) && !(flags & OPFLAG_READ_VAL32_C) && !(flags & OPFLAG_READ_VAL64_C))
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_C;
                regs[ip->a.u32] = nullptr;
                ip->c.u64       = regsRW[ip->a.u32];
            }
            else
            {
                if (flags & OPFLAG_WRITE_A)
                    regs[ip->a.u32] = nullptr;
                if (flags & OPFLAG_WRITE_B)
                    regs[ip->b.u32] = nullptr;
                if (flags & OPFLAG_WRITE_C)
                    regs[ip->c.u32] = nullptr;
                if (flags & OPFLAG_WRITE_D)
                    regs[ip->d.u32] = nullptr;
            }
            break;
        }

        if (!(ip->flags & BCI_IMM_A) && (flags & OPFLAG_READ_A) && regs[ip->a.u32])
        {
            if (flags & OPFLAG_IMM_A)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_A;
                regs[ip->a.u32] = nullptr;
                ip->a.u64       = regsRW[ip->a.u32];
            }
        }

        if (!(ip->flags & BCI_IMM_B) && (flags & OPFLAG_READ_B) && regs[ip->b.u32])
        {
            if (flags & OPFLAG_IMM_B)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_B;
                regs[ip->b.u32] = nullptr;
                ip->b.u64       = regsRW[ip->b.u32];
            }
            else if (ip->op == ByteCodeOp::CopyRBtoRA)
            {
                context->passHasDoneSomething = true;
                ip->op                        = ByteCodeOp::SetImmediate64;
                regs[ip->b.u32]               = nullptr;
                ip->b.u64                     = regsRW[ip->b.u32];
                flags                         = g_ByteCodeOpFlags[(int) ip->op];
            }
        }

        if (!(ip->flags & BCI_IMM_C) && (flags & OPFLAG_READ_C) && regs[ip->c.u32])
        {
            if (flags & OPFLAG_IMM_C)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_C;
                regs[ip->c.u32] = nullptr;
                ip->c.u64       = regsRW[ip->c.u32];
            }
        }

        if (!(ip->flags & BCI_IMM_D) && (flags & OPFLAG_READ_D) && regs[ip->d.u32])
        {
            if (flags & OPFLAG_IMM_D)
            {
                context->passHasDoneSomething = true;
                ip->flags |= BCI_IMM_D;
                regs[ip->d.u32] = nullptr;
                ip->d.u64       = regsRW[ip->d.u32];
            }
        }

        if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
            regs[ip->a.u32] = nullptr;
        if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
            regs[ip->b.u32] = nullptr;
        if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
            regs[ip->c.u32] = nullptr;
        if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
            regs[ip->d.u32] = nullptr;

        if (isJumpBlock(ip))
            memset(regs.buffer, 0, maxReg * sizeof(void*));
    }
}

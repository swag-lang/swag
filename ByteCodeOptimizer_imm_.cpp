#include "pch.h"
#include "ByteCodeOptimizer.h"

void ByteCodeOptimizer::optimizePassImmediate(ByteCodeOptContext* context)
{
    auto  maxReg = context->bc->maxReservedRegisterRC;
    auto& regs   = context->tmpBufInst;
    regs.reserve(maxReg);
    regs.count = maxReg;
    memset(regs.buffer, 0, maxReg * sizeof(void*));

    auto& regsRW = context->tmpBufU64;
    regsRW.reserve(maxReg);
    regsRW.count = maxReg;
    memset(regsRW.buffer, 0, maxReg * sizeof(uint64_t));

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJump(ip))
            memset(regs.buffer, 0, maxReg * sizeof(void*));

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

        if (ip->op == ByteCodeOp::SetImmediate32)
        {
            regsRW[ip->a.u32] = ip->b.u32;
            regs[ip->a.u32]   = ip;
        }
        else if (ip->op == ByteCodeOp::SetImmediate64)
        {
            regsRW[ip->a.u32] = ip->b.u64;
            regs[ip->a.u32]   = ip;
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

        if ((flags & OPFLAG_IMM_A) && !(ip->flags & BCI_IMM_A) && (flags & OPFLAG_READ_A) && regs[ip->a.u32])
        {
            context->passHasDoneSomething = true;
            ip->flags |= BCI_IMM_A;
            regs[ip->a.u32] = nullptr;
            ip->a.u64       = regsRW[ip->a.u32];
        }

        if ((flags & OPFLAG_IMM_B) && !(ip->flags & BCI_IMM_B) && (flags & OPFLAG_READ_B) && regs[ip->b.u32])
        {
            context->passHasDoneSomething = true;
            ip->flags |= BCI_IMM_B;
            regs[ip->b.u32] = nullptr;
            ip->b.u64       = regsRW[ip->b.u32];
        }

        if ((flags & OPFLAG_IMM_C) && !(ip->flags & BCI_IMM_C) && (flags & OPFLAG_READ_C) && regs[ip->c.u32])
        {
            context->passHasDoneSomething = true;
            ip->flags |= BCI_IMM_C;
            regs[ip->c.u32] = nullptr;
            ip->c.u64       = regsRW[ip->c.u32];
        }

        if ((flags & OPFLAG_IMM_D) && !(ip->flags & BCI_IMM_D) && (flags & OPFLAG_READ_D) && regs[ip->d.u32])
        {
            context->passHasDoneSomething = true;
            ip->flags |= BCI_IMM_D;
            regs[ip->d.u32] = nullptr;
            ip->d.u64       = regsRW[ip->d.u32];
        }

        if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
            regs[ip->a.u32] = nullptr;
        if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
            regs[ip->b.u32] = nullptr;
        if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
            regs[ip->c.u32] = nullptr;
        if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
            regs[ip->d.u32] = nullptr;
    }
}

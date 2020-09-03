#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Log.h"

// If an instruction can have an immediate form, then transform it if the corresponding
// register is a constant.
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
        if (ip->flags & BCI_START_STMT)
            memset(regs.buffer, 0, maxReg * sizeof(void*));

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

        switch (ip->op)
        {
        case ByteCodeOp::SetImmediate32:
            regsRW[ip->a.u32] = ip->b.u32;
            regs[ip->a.u32]   = ip;
            break;
        case ByteCodeOp::SetImmediate64:
            regsRW[ip->a.u32] = ip->b.u64;
            regs[ip->a.u32]   = ip;
            break;
        default:
            if (flags & OPFLAG_WRITE_A)
                regs[ip->a.u32] = nullptr;
            if (flags & OPFLAG_WRITE_B)
                regs[ip->b.u32] = nullptr;
            if (flags & OPFLAG_WRITE_C)
                regs[ip->c.u32] = nullptr;
            if (flags & OPFLAG_WRITE_D)
                regs[ip->d.u32] = nullptr;
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
            /*else if(ip->op != ByteCodeOp::PushRAParam)
            {
                g_Log.lock();
                printf("%s\n", context->bc->callName().c_str());
                context->bc->printInstruction(ip);
                g_Log.unlock();
            }*/
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
            else if(!(flags & OPFLAG_IMM_A))
            {
                g_Log.lock();
                printf("%s\n", context->bc->callName().c_str());
                context->bc->printInstruction(ip);
                g_Log.unlock();
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

        if (isJump(ip))
            memset(regs.buffer, 0, maxReg * sizeof(void*));
    }
}

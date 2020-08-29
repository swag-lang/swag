#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Log.h"

// If two instructions write in the same register in the same block (between jumps), then the
// first write is useless and can be removed
void ByteCodeOptimizer::optimizePassDeadStore(ByteCodeOptContext* context)
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

    static const uint64_t READ  = 0x1;
    static const uint64_t WRITE = 0x2;

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Reset at each statement
        //if (ip->flags & BCI_START_STMT || isJump(ip))
        if (isJump(ip))
            memset(regs.buffer, 0, maxReg * sizeof(void*));

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

        if ((flags & OPFLAG_READ_A) && !(ip->flags & BCI_IMM_A))
        {
            regs[ip->a.u32] = nullptr;
            regsRW[ip->a.u32] |= READ;
        }

        if ((flags & OPFLAG_READ_B) && !(ip->flags & BCI_IMM_B))
        {
            regs[ip->b.u32] = nullptr;
            regsRW[ip->b.u32] |= READ;
        }

        if ((flags & OPFLAG_READ_C) && !(ip->flags & BCI_IMM_C))
        {
            regs[ip->c.u32] = nullptr;
            regsRW[ip->c.u32] |= READ;
        }

        if ((flags & OPFLAG_READ_D) && !(ip->flags & BCI_IMM_D))
        {
            regs[ip->d.u32] = nullptr;
            regsRW[ip->d.u32] |= READ;
        }

        // Optimize only A. This is on purpose because this is what will make relevant changes.
        // Not that optimizing other registers can create some problems (for example C, with MkInterface);
        if (flags & OPFLAG_WRITE_A)
        {
            if (regs[ip->a.u32])
                setNop(context, regs[ip->a.u32]);
            if (!(flags & (OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D)))
                regs[ip->a.u32] = ip;
        }

        // If we leave the function, then we can discard all pending write that are not read
        if (ip->op == ByteCodeOp::Ret)
        {
            for (int i = 0; i < maxReg; i++)
            {
                if (regs[i])
                    setNop(context, regs[i]);
            }
        }

        if (flags & OPFLAG_WRITE_A)
            regsRW[ip->a.u32] |= WRITE;
        if (flags & OPFLAG_WRITE_B)
            regsRW[ip->b.u32] |= WRITE;
        if (flags & OPFLAG_WRITE_C)
            regsRW[ip->c.u32] |= WRITE;
        if (flags & OPFLAG_WRITE_D)
            regsRW[ip->d.u32] |= WRITE;
    }

    // If a register is written to, but never read in the whole function, then we
    // can remove all instructions that write to that register
    bool secondPass = false;
    for (uint32_t i = 0; i < maxReg; i++)
    {
        if (regsRW[i] == WRITE)
        {
            secondPass = true;
            break;
        }
    }

    if (secondPass)
    {
        for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
        {
            auto flags = g_ByteCodeOpFlags[(int) ip->op];

            // Need to be sure that every registers written to are not read anywhere
#define SOLO_W(__r, __f) (!(flags & __f) || (regsRW[__r] == WRITE))
            if (flags & OPFLAG_WRITE_MASK)
            {
                if (SOLO_W(ip->a.u32, OPFLAG_WRITE_A) &&
                    SOLO_W(ip->b.u32, OPFLAG_WRITE_B) &&
                    SOLO_W(ip->c.u32, OPFLAG_WRITE_C) &&
                    SOLO_W(ip->d.u32, OPFLAG_WRITE_D))
                {
                    setNop(context, ip);
                }
            }
        }
    }
}

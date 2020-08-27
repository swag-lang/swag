#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassDeadStore(ByteCodeOptContext* context)
{
    auto& regs = context->toDo;
    regs.reserve(context->bc->maxReservedRegisterRC);
    regs.count = context->bc->maxReservedRegisterRC;
    memset(regs.buffer, 0, regs.size() * sizeof(void*));

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Reset at each statement
        if (ip->flags & BCI_START_STMT || isJump(ip))
            memset(regs.buffer, 0, regs.size() * sizeof(void*));

        auto flags = g_ByteCodeOpFlags[(int) ip->op];
        if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
            regs[ip->a.u32] = nullptr;
        if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
            regs[ip->b.u32] = nullptr;
        if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
            regs[ip->c.u32] = nullptr;
        if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
            regs[ip->d.u32] = nullptr;

        if (flags & OPFLAG_WRITE_A)
        {
            if (regs[ip->a.u32])
            {
                setNop(context, regs[ip->a.u32]);
                /*g_Log.lock();
                printf(context->bc->callName().c_str());
                printf("\n");
                context->bc->printInstruction(regs[ip->a.u32]);
                context->bc->printInstruction(ip);
                g_Log.unlock();*/
            }

            if (!(flags & (OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D)))
                regs[ip->a.u32] = ip;
        }
    }
}

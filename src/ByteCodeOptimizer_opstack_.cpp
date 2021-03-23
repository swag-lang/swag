#include "pch.h"
#include "ByteCodeOptimizer.h"

void ByteCodeOptimizer::optimizePassOpStack(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        auto flags = g_ByteCodeOpFlags[(int) ip->op];
        if (!(flags & OPFLAG_STACKPTR_A))
            continue;
        if (ip->flags & BCI_STACKPTR_A)
            continue;
        if (ip->flags & BCI_IMM_A)
            continue;

        auto ipScan = ip;
        bool found  = false;
        while (true)
        {
            if (ipScan->flags & BCI_START_STMT)
                break;

            if (ipScan->op == ByteCodeOp::MakeStackPointer && ipScan->a.u32 == ip->a.u32)
            {
                found = true;
                break;
            }

            auto flagsScan = g_ByteCodeOpFlags[(int) ipScan->op];
            if (flagsScan & OPFLAG_WRITE_A && ipScan->a.u32 == ip->a.u32 && !(ipScan->flags & BCI_IMM_A))
                break;
            if (flagsScan & OPFLAG_WRITE_B && ipScan->b.u32 == ip->a.u32 && !(ipScan->flags & BCI_IMM_B))
                break;
            if (flagsScan & OPFLAG_WRITE_C && ipScan->c.u32 == ip->a.u32 && !(ipScan->flags & BCI_IMM_C))
                break;
            if (flagsScan & OPFLAG_WRITE_D && ipScan->d.u32 == ip->a.u32 && !(ipScan->flags & BCI_IMM_D))
                break;

            if (ipScan == context->bc->out)
                break;
            ipScan--;
        }

        if (found)
        {
            ip->flags |= BCI_STACKPTR_A;
            ip->flags |= BCI_IMM_A;
            ip->a.u32 = ipScan->b.u32;
        }
    }
}
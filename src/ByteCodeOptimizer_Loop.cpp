#include "pch.h"
#include "ByteCodeOptimizer.h"

// Try to move some 'constant' assignment outside simple loops
bool ByteCodeOptimizer::optimizePassLoop(ByteCodeOptContext* context)
{
    auto& mapRA = context->mapRegInstA;
    auto& mapRB = context->mapRegInstB;

    for (size_t idx = 0; idx < context->jumps.size(); idx++)
    {
        auto ip = context->jumps[idx];
        if (!ByteCode::isJump(ip))
            continue;

        if (ip->op == ByteCodeOp::JumpIfNotZero64 && ip->b.s32 < 0)
        {
            mapRA.clear();
            mapRB.clear();

            auto ipStart = ip + ip->b.s32 + 1;
            auto ipScan  = ipStart + 1;

            while (ipScan != ip)
            {
                // Do not optimize if complex context
                if (ByteCode::isJump(ipScan) || ipScan->flags & BCI_START_STMT)
                {
                    mapRA.clear();
                    mapRB.clear();
                    break;
                }

                auto flags = g_ByteCodeOpDesc[(int) ipScan->op].flags;

                if (ipScan->op == ByteCodeOp::MakeStackPointer)
                {
                    if (mapRA.contains(ipScan->a.u32))
                        mapRB.set(ipScan->a.u32, ipScan);
                    mapRA.set(ipScan->a.u32, ipScan);
                    ipScan++;
                    continue;
                }

                if (flags & OPFLAG_WRITE_A && !(ipScan->flags & BCI_IMM_A))
                    mapRB.set(ipScan->a.u32, ipScan);
                if (flags & OPFLAG_WRITE_B && !(ipScan->flags & BCI_IMM_B))
                    mapRB.set(ipScan->b.u32, ipScan);
                if (flags & OPFLAG_WRITE_C && !(ipScan->flags & BCI_IMM_C))
                    mapRB.set(ipScan->c.u32, ipScan);
                if (flags & OPFLAG_WRITE_D && !(ipScan->flags & BCI_IMM_D))
                    mapRB.set(ipScan->d.u32, ipScan);

                ipScan++;
            }

            if (mapRA.count == 0)
                continue;
            for (uint32_t i = 0; i < RegisterList::MAX_REGISTERS; i++)
            {
                if (!mapRA.contains(i))
                    continue;
                if (mapRB.contains(i))
                    continue;

                // Move instruction to the start of the loop, and shift all the rest
                auto cpy = *mapRA.val[i];
                memmove(ipStart + 1, ipStart, (mapRA.val[i] - ipStart) * sizeof(ByteCodeInstruction));
                *ipStart = cpy;
                ipStart->flags |= BCI_START_STMT;
                ipStart->location = ipStart[1].location;

                // Decrease jump by one
                ip->b.s32 += 1;

                context->passHasDoneSomething = true;
            }
        }
    }

    return true;
}
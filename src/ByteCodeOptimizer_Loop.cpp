#include "pch.h"
#include "ByteCodeOptimizer.h"

// Try to move some 'constant' assignment outside simple loops
bool ByteCodeOptimizer::optimizePassLoop(ByteCodeOptContext* context)
{
    auto& mapRA = context->mapU32InstA;
    auto& mapRB = context->mapU32InstB;

    for (int idx = 0; idx < context->jumps.size(); idx++)
    {
        auto ip = context->jumps[idx];
        if (ip->op == ByteCodeOp::JumpIfNotZero64 && ip->b.s32 < 0)
        {
            mapRA.clear();
            mapRB.clear();

            auto ipStart = ip + ip->b.s32 + 1;
            auto ipScan  = ipStart + 1;

            while (ipScan != ip)
            {
                // Do not optimize if complex context
                if (ByteCode::isJump(ipScan) || ipScan->flags & BCI_START_STMT || ipScan->flags & BCI_JUMP_DEST)
                {
                    mapRA.clear();
                    mapRB.clear();
                    break;
                }

                auto flags = g_ByteCodeOpDesc[(int) ipScan->op].flags;

                if (ipScan->op == ByteCodeOp::MakeStackPointer)
                {
                    if (mapRA.find(ipScan->a.u32) != mapRA.end())
                        mapRB[ipScan->a.u32] = ipScan;
                    mapRA[ipScan->a.u32] = ipScan;
                    ipScan++;
                    continue;
                }

                if (flags & OPFLAG_WRITE_A && !(ipScan->flags & BCI_IMM_A))
                    mapRB[ipScan->a.u32] = ipScan;
                if (flags & OPFLAG_WRITE_B && !(ipScan->flags & BCI_IMM_B))
                    mapRB[ipScan->b.u32] = ipScan;
                if (flags & OPFLAG_WRITE_C && !(ipScan->flags & BCI_IMM_C))
                    mapRB[ipScan->c.u32] = ipScan;
                if (flags & OPFLAG_WRITE_D && !(ipScan->flags & BCI_IMM_D))
                    mapRB[ipScan->d.u32] = ipScan;

                ipScan++;
            }

            for (auto& it : mapRA)
            {
                if (mapRB.find(it.first) != mapRB.end())
                    continue;

                // Move instruction to the start of the loop, and shift all the rest
                auto cpy = *it.second;
                memmove(ipStart + 1, ipStart, (it.second - ipStart) * sizeof(ByteCodeInstruction));
                *ipStart = cpy;
                ipStart->flags |= BCI_JUMP_DEST;
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
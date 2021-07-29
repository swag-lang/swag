#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "SourceFile.h"

// If a CopyRBRA is followed by the same CopyRBRA, and between them there's no write to RA or RB,
// then the second CopyRBRA is useless
void ByteCodeOptimizer::optimizePassDupCopyRBRAOp(ByteCodeOptContext* context, ByteCodeOp op)
{
    auto& mapCopyRA = context->mapU32InstA;
    auto& mapCopyRB = context->mapU32InstB;
    mapCopyRA.clear();
    mapCopyRB.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT)
        {
            mapCopyRA.clear();
            mapCopyRB.clear();
        }

        auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

        // CopyRBRA X, X
        if (ip->op == op && ip->a.u32 == ip->b.u32)
            setNop(context, ip);

        // CopyRBRA X, Y followed by CopyRBRA Y, X
        if (ip->op == op &&
            ip[1].op == op &&
            ip->a.u32 == ip[1].b.u32 &&
            ip->b.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
        }

        // CopyRBRA followed by one single pushraparam, take the original register
        if (ip->op == ByteCodeOp::CopyRBtoRA64 &&
            ip[1].op == ByteCodeOp::PushRAParam &&
            ip[0].a.u32 == ip[1].a.u32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[1].a.u32                   = ip[0].b.u32;
            context->passHasDoneSomething = true;
        }

        // 2 CopyRBRA followed by one single pushraparam2, take the original registers
        if (ip[0].op == ByteCodeOp::CopyRBtoRA64 &&
            ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
            ip[2].op == ByteCodeOp::PushRAParam2 &&
            ip[0].a.u32 == ip[2].b.u32 &&
            ip[1].a.u32 == ip[2].a.u32 &&
            !(ip[0].flags & BCI_START_STMT) &&
            !(ip[1].flags & BCI_START_STMT) &&
            !(ip[2].flags & BCI_START_STMT))
        {
            ip[2].a.u32                   = ip[1].b.u32;
            ip[2].b.u32                   = ip[0].b.u32;
            context->passHasDoneSomething = true;
        }

        if (ip->op == op)
        {
            auto it = mapCopyRA.find(ip->a.u32);
            if (it != mapCopyRA.end())
            {
                auto it1 = mapCopyRB.find(ip->b.u32);
                if (it1 != mapCopyRB.end())
                {
                    if (it1->second == it->second)
                    {
                        setNop(context, ip);
                    }
                }
            }

            auto it1 = mapCopyRB.find(ip->a.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);

            mapCopyRA[ip->a.u32] = ip;
            mapCopyRB[ip->b.u32] = ip;
            continue;
        }

        if (op == ByteCodeOp::CopyRBtoRA64)
        {
            // If we use a register that comes from a CopyRBRA, then use the initial
            // register instead (that way, the Copy can become deadstore and removed later)
            // Do it only for CopyRBtoRA64, as other copy have an implicit cast
            // *NOT* for PushRAParam, because the register numbers are important in case of variadic parameters.
            // *NOT* for CopySP, because the register numbers are important in case of variadic parameters.
            if (ip->op != ByteCodeOp::PushRAParam &&
                ip->op != ByteCodeOp::PushRAParam2 &&
                ip->op != ByteCodeOp::PushRAParam3 &&
                ip->op != ByteCodeOp::PushRAParam4 &&
                ip->op != ByteCodeOp::CopySP)
            {
                if ((flags & OPFLAG_READ_A) && !(ip->flags & BCI_IMM_A) && !(flags & OPFLAG_WRITE_A))
                {
                    auto it = mapCopyRA.find(ip->a.u32);
                    if (it != mapCopyRA.end())
                    {
                        auto it1 = mapCopyRB.find(it->second->b.u32);
                        if (it1 != mapCopyRB.end() && it->second == it1->second)
                        {
                            ip->a.u32                     = it->second->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }

                if ((flags & OPFLAG_READ_B) && !(ip->flags & BCI_IMM_B) && !(flags & OPFLAG_WRITE_B))
                {
                    auto it = mapCopyRA.find(ip->b.u32);
                    if (it != mapCopyRA.end())
                    {
                        auto it1 = mapCopyRB.find(it->second->b.u32);
                        if (it1 != mapCopyRB.end() && it->second == it1->second)
                        {
                            ip->b.u32                     = it->second->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }

                if ((flags & OPFLAG_READ_C) && !(ip->flags & BCI_IMM_C) && !(flags & OPFLAG_WRITE_C))
                {
                    auto it = mapCopyRA.find(ip->c.u32);
                    if (it != mapCopyRA.end())
                    {
                        auto it1 = mapCopyRB.find(it->second->b.u32);
                        if (it1 != mapCopyRB.end() && it->second == it1->second)
                        {
                            ip->c.u32                     = it->second->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }

                if ((flags & OPFLAG_READ_D) && !(ip->flags & BCI_IMM_D) && !(flags & OPFLAG_WRITE_D))
                {
                    auto it = mapCopyRA.find(ip->d.u32);
                    if (it != mapCopyRA.end())
                    {
                        auto it1 = mapCopyRB.find(it->second->b.u32);
                        if (it1 != mapCopyRB.end() && it->second == it1->second)
                        {
                            ip->d.u32                     = it->second->b.u32;
                            context->passHasDoneSomething = true;
                        }
                    }
                }
            }
        }

        // Reset CopyRARB map
        if (isJumpBlock(ip))
        {
            mapCopyRA.clear();
            mapCopyRB.clear();
        }

        if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
        {
            auto it = mapCopyRA.find(ip->a.u32);
            if (it != mapCopyRA.end())
            {
                auto it1 = mapCopyRB.find(it->second->b.u32);
                if (it1 != mapCopyRB.end() && it->second == it1->second)
                    mapCopyRB.erase(it1);
                mapCopyRA.erase(it);
            }

            auto it1 = mapCopyRB.find(ip->a.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
        {
            auto it = mapCopyRA.find(ip->b.u32);
            if (it != mapCopyRA.end())
            {
                auto it1 = mapCopyRB.find(it->second->b.u32);
                if (it1 != mapCopyRB.end() && it->second == it1->second)
                    mapCopyRB.erase(it1);
                mapCopyRA.erase(it);
            }

            auto it1 = mapCopyRB.find(ip->b.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
        {
            auto it = mapCopyRA.find(ip->c.u32);
            if (it != mapCopyRA.end())
            {
                auto it1 = mapCopyRB.find(it->second->b.u32);
                if (it1 != mapCopyRB.end() && it->second == it1->second)
                    mapCopyRB.erase(it1);
                mapCopyRA.erase(it);
            }

            auto it1 = mapCopyRB.find(ip->c.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
        {
            auto it = mapCopyRA.find(ip->d.u32);
            if (it != mapCopyRA.end())
            {
                auto it1 = mapCopyRB.find(it->second->b.u32);
                if (it1 != mapCopyRB.end() && it->second == it1->second)
                    mapCopyRB.erase(it1);
                mapCopyRA.erase(it);
            }

            auto it1 = mapCopyRB.find(ip->d.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
    }
}

bool ByteCodeOptimizer::optimizePassDupCopyRBRA(ByteCodeOptContext* context)
{
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA8);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA16);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA32);
    optimizePassDupCopyRBRAOp(context, ByteCodeOp::CopyRBtoRA64);
    return true;
}

void ByteCodeOptimizer::optimizePassDupCopyOp(ByteCodeOptContext* context, ByteCodeOp op)
{
    auto& mapRA = context->mapU32InstA;
    mapRA.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJumpBlock(ip))
            mapRA.clear();

        auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

        if (ip->op == op)
        {
            auto it = mapRA.find(ip->a.u32);
            if (it != mapRA.end() && it->second)
            {
                if (it->second->b.u64 == ip->b.u64 &&
                    it->second->c.u64 == ip->c.u64 &&
                    it->second->d.u64 == ip->d.u64 &&
                    it->second->flags == ip->flags)
                {
                    setNop(context, ip);
                    mapRA[ip->a.u32] = nullptr;
                    continue;
                }
            }

            mapRA[ip->a.u32] = ip;
        }
        else
        {
            if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
            {
                auto it = mapRA.find(ip->a.u32);
                if (it != mapRA.end())
                    mapRA.erase(it);
            }
            if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
            {
                auto it = mapRA.find(ip->b.u32);
                if (it != mapRA.end())
                    mapRA.erase(it);
            }
            if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
            {
                auto it = mapRA.find(ip->c.u32);
                if (it != mapRA.end())
                    mapRA.erase(it);
            }
            if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
            {
                auto it = mapRA.find(ip->d.u32);
                if (it != mapRA.end())
                    mapRA.erase(it);
            }
        }
    }
}

// Remove duplicated pure instructions (set RA to a constant)
bool ByteCodeOptimizer::optimizePassDupCopy(ByteCodeOptContext* context)
{
    optimizePassDupCopyOp(context, ByteCodeOp::CopyRRtoRC);
    optimizePassDupCopyOp(context, ByteCodeOp::MakeStackPointerParam);
    optimizePassDupCopyOp(context, ByteCodeOp::MakeBssSegPointer);
    optimizePassDupCopyOp(context, ByteCodeOp::MakeConstantSegPointer);
    optimizePassDupCopyOp(context, ByteCodeOp::MakeMutableSegPointer);
    optimizePassDupCopyOp(context, ByteCodeOp::GetFromStackParam64);
    optimizePassDupCopyOp(context, ByteCodeOp::SetImmediate32);
    optimizePassDupCopyOp(context, ByteCodeOp::SetImmediate64);
    return true;
}
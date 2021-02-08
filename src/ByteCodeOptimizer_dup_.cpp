#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "SourceFile.h"

// If a CopyRBRA is followed by the same CopyRBRA, and between them there's no write to RA or RB,
// then the second CopyRBRA is useless
void ByteCodeOptimizer::optimizePassDupCopyRBRA(ByteCodeOptContext* context)
{
    auto& mapCopyRA = context->mapU32InstA;
    auto& mapCopyRB = context->mapU32InstB;
    mapCopyRA.clear();
    mapCopyRB.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJumpBlock(ip))
        {
            mapCopyRA.clear();
            mapCopyRB.clear();
        }

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

        // CopyRBRA X, Y followed by CopyRBRA Y, X
        if (ip->op == ByteCodeOp::CopyRBtoRA &&
            ip[1].op == ByteCodeOp::CopyRBtoRA &&
            ip->a.u32 == ip[1].b.u32 &&
            ip->b.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::CopyRBtoRA)
        {
            auto it = mapCopyRA.find(ip->a.u32);
            if (it != mapCopyRA.end())
            {
                auto it1 = mapCopyRB.find(ip->b.u32);
                if (it1 != mapCopyRB.end())
                {
                    if (it1->second == it->second)
                        setNop(context, ip);
                }
            }

            mapCopyRA[ip->a.u32] = ip;
            mapCopyRB[ip->b.u32] = ip;
            continue;
        }

        // If we use a register that comes from a CopyRBRA, then use the initial
        // register instead (that way, the Copy can become deadstore and removed later)
        if ((flags & OPFLAG_READ_A) && !(ip->flags & BCI_IMM_A) && !(flags & OPFLAG_WRITE_A))
        {
            auto it = mapCopyRA.find(ip->a.u32);
            if (it != mapCopyRA.end())
            {
                auto it1 = mapCopyRB.find(it->second->b.u32);
                if (it1 != mapCopyRB.end())
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
                if (it1 != mapCopyRB.end())
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
                if (it1 != mapCopyRB.end())
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
                if (it1 != mapCopyRB.end())
                {
                    ip->d.u32                     = it->second->b.u32;
                    context->passHasDoneSomething = true;
                }
            }
        }

        // Reset CopyRARB map
        if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
        {
            auto it = mapCopyRA.find(ip->a.u32);
            if (it != mapCopyRA.end())
                mapCopyRA.erase(it);
            auto it1 = mapCopyRB.find(ip->a.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
        {
            auto it = mapCopyRA.find(ip->b.u32);
            if (it != mapCopyRA.end())
                mapCopyRA.erase(it);
            auto it1 = mapCopyRB.find(ip->b.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
        {
            auto it = mapCopyRA.find(ip->c.u32);
            if (it != mapCopyRA.end())
                mapCopyRA.erase(it);
            auto it1 = mapCopyRB.find(ip->c.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
        {
            auto it = mapCopyRA.find(ip->d.u32);
            if (it != mapCopyRA.end())
                mapCopyRA.erase(it);
            auto it1 = mapCopyRB.find(ip->d.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
    }
}

void ByteCodeOptimizer::optimizePassDupCopy(ByteCodeOptContext* context, ByteCodeOp op)
{
    auto& mapRA = context->mapU32InstA;
    mapRA.clear();

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJumpBlock(ip))
            mapRA.clear();

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

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
void ByteCodeOptimizer::optimizePassDupCopyRA(ByteCodeOptContext* context)
{
    optimizePassDupCopy(context, ByteCodeOp::CopyRRtoRC);
    optimizePassDupCopy(context, ByteCodeOp::MakeStackPointerParam);
    optimizePassDupCopy(context, ByteCodeOp::MakeBssSegPointer);
    optimizePassDupCopy(context, ByteCodeOp::MakeConstantSegPointer);
    optimizePassDupCopy(context, ByteCodeOp::MakeMutableSegPointer);
    optimizePassDupCopy(context, ByteCodeOp::MakeTypeSegPointer);
    optimizePassDupCopy(context, ByteCodeOp::GetFromStackParam64);
    optimizePassDupCopy(context, ByteCodeOp::SetImmediate32);
    optimizePassDupCopy(context, ByteCodeOp::SetImmediate64);
}
#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "TypeManager.h"

// If a CopyRBRA is followed by the same CopyRBRA, and between them there's no write to RA or RB,
// then the second CopyRBRA is useless
void ByteCodeOptimizer::optimizePassDupCopyRBRA(ByteCodeOptContext* context)
{
    map<uint32_t, ByteCodeInstruction*> mapCopyRA;
    map<uint32_t, ByteCodeInstruction*> mapCopyRB;
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJump(ip))
        {
            mapCopyRA.clear();
            mapCopyRB.clear();
        }

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

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
        }
        else if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
        {
            auto it = mapCopyRA.find(ip->a.u32);
            if (it != mapCopyRA.end())
                mapCopyRA.erase(it);
            auto it1 = mapCopyRB.find(ip->a.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        else if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
        {
            auto it = mapCopyRA.find(ip->b.u32);
            if (it != mapCopyRA.end())
                mapCopyRA.erase(it);
            auto it1 = mapCopyRB.find(ip->b.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        else if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
        {
            auto it = mapCopyRA.find(ip->c.u32);
            if (it != mapCopyRA.end())
                mapCopyRA.erase(it);
            auto it1 = mapCopyRB.find(ip->c.u32);
            if (it1 != mapCopyRB.end())
                mapCopyRB.erase(it1);
        }
        else if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
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
    map<uint32_t, ByteCodeInstruction*> mapRA;
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJump(ip))
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
        else if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
        {
            auto it = mapRA.find(ip->a.u32);
            if (it != mapRA.end())
                mapRA.erase(it);
        }
        else if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
        {
            auto it = mapRA.find(ip->b.u32);
            if (it != mapRA.end())
                mapRA.erase(it);
        }
        else if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
        {
            auto it = mapRA.find(ip->c.u32);
            if (it != mapRA.end())
                mapRA.erase(it);
        }
        else if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
        {
            auto it = mapRA.find(ip->d.u32);
            if (it != mapRA.end())
                mapRA.erase(it);
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
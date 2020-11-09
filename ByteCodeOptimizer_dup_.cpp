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

// If a CopyRBRA is followed by the same CopyRRtoRC, and between them there's no write to RC,
// then the second CopyRBRA is useless
void ByteCodeOptimizer::optimizePassDupCopyRRRC(ByteCodeOptContext* context)
{
    map<uint32_t, uint32_t> mapCopyRBRA;
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->flags & BCI_START_STMT || isJump(ip))
            mapCopyRBRA.clear();

        auto flags = g_ByteCodeOpFlags[(int) ip->op];

        if (ip[0].op == ByteCodeOp::CopyRRtoRC)
        {
            auto it = mapCopyRBRA.find(ip->a.u32);
            if (it != mapCopyRBRA.end())
            {
                if (it->second == ip->b.u32)
                    setNop(context, ip);
            }

            mapCopyRBRA[ip->a.u32] = ip->b.u32;
        }
        else if ((flags & OPFLAG_WRITE_A) && !(ip->flags & BCI_IMM_A))
        {
            auto it = mapCopyRBRA.find(ip->a.u32);
            if (it != mapCopyRBRA.end())
                mapCopyRBRA.erase(it);
        }
        else if ((flags & OPFLAG_WRITE_B) && !(ip->flags & BCI_IMM_B))
        {
            auto it = mapCopyRBRA.find(ip->b.u32);
            if (it != mapCopyRBRA.end())
                mapCopyRBRA.erase(it);
        }
        else if ((flags & OPFLAG_WRITE_C) && !(ip->flags & BCI_IMM_C))
        {
            auto it = mapCopyRBRA.find(ip->c.u32);
            if (it != mapCopyRBRA.end())
                mapCopyRBRA.erase(it);
        }
        else if ((flags & OPFLAG_WRITE_D) && !(ip->flags & BCI_IMM_D))
        {
            auto it = mapCopyRBRA.find(ip->d.u32);
            if (it != mapCopyRBRA.end())
                mapCopyRBRA.erase(it);
        }
    }
}

#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Log.h"

// If two instructions write in the same register in the same block (between jumps), and there's no
// read of that register between them, then the first write is useless and can be removed
bool ByteCodeOptimizer::optimizePassDeadStore(ByteCodeOptContext* context)
{
    parseTree(context, 0, context->tree[0].start, 0x00000001, [](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt) {
        auto ip    = parseCxt.curIp;
        auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

        uint32_t regScan = UINT32_MAX;
        if (flags & OPFLAG_WRITE_A && !(flags & (OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D)))
            regScan = ip->a.u32;
        if (flags & OPFLAG_WRITE_B && !(flags & (OPFLAG_WRITE_A | OPFLAG_WRITE_C | OPFLAG_WRITE_D)))
            regScan = ip->b.u32;
        if (flags & OPFLAG_WRITE_C && !(flags & (OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_D)))
            regScan = ip->c.u32;
        if (flags & OPFLAG_WRITE_D && !(flags & (OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C)))
            regScan = ip->d.u32;

        if (regScan == UINT32_MAX)
            return;

        bool hasRead  = false;
        bool hasWrite = false;
        parseTree(context, parseCxt.curNode, parseCxt.curIp, 0x00000002, [&](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt1) {
            auto ip1 = parseCxt1.curIp;
            if (ip1 == ip)
                return;

            auto flags1 = g_ByteCodeOpDesc[(int) ip1->op].flags;
            if ((flags1 & OPFLAG_READ_A) && !(ip1->flags & BCI_IMM_A))
            {
                if (ip1->a.u32 == regScan)
                {
                    hasRead               = true;
                    parseCxt1.mustStopAll = true;
                    return;
                }
            }

            if ((flags1 & OPFLAG_READ_B) && !(ip1->flags & BCI_IMM_B))
            {
                if (ip1->b.u32 == regScan)
                {
                    hasRead               = true;
                    parseCxt1.mustStopAll = true;
                    return;
                }
            }

            if ((flags1 & OPFLAG_READ_C) && !(ip1->flags & BCI_IMM_C))
            {
                if (ip1->c.u32 == regScan)
                {
                    hasRead               = true;
                    parseCxt1.mustStopAll = true;
                    return;
                }
            }

            if ((flags1 & OPFLAG_READ_D) && !(ip1->flags & BCI_IMM_D))
            {
                if (ip1->d.u32 == regScan)
                {
                    hasRead               = true;
                    parseCxt1.mustStopAll = true;
                    return;
                }
            }

            if (flags1 & OPFLAG_WRITE_A)
            {
                if (ip1->a.u32 == regScan)
                {
                    hasWrite                = true;
                    parseCxt1.mustStopBlock = true;
                    return;
                }
            }

            if (flags1 & OPFLAG_WRITE_B)
            {
                if (ip1->b.u32 == regScan)
                {
                    hasWrite                = true;
                    parseCxt1.mustStopBlock = true;
                    return;
                }
            }

            if (flags1 & OPFLAG_WRITE_C)
            {
                if (ip1->c.u32 == regScan)
                {
                    hasWrite                = true;
                    parseCxt1.mustStopBlock = true;
                    return;
                }
            }

            if (flags1 & OPFLAG_WRITE_D)
            {
                if (ip1->d.u32 == regScan)
                {
                    hasWrite                = true;
                    parseCxt1.mustStopBlock = true;
                    return;
                }
            }
        });

        if (hasRead)
            return;

        setNop(context, ip);
    });

    return true;
}
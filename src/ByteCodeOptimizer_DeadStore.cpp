#include "pch.h"
#include "ByteCodeOptimizer.h"

// If two instructions write in the same register in the same block (between jumps), and there's no
// read of that register between them, then the first write is useless and can be removed
bool ByteCodeOptimizer::optimizePassDeadStore(ByteCodeOptContext* context)
{
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* context, const ByteCodeOptTreeParseContext& parseCxt)
    {
        const auto ip    = parseCxt.curIp;
        const auto flags = g_ByteCodeOpDesc[(int) ip->op].flags;

        uint32_t regScan = UINT32_MAX;
        if (flags & OPFLAG_WRITE_A && !(flags & (OPFLAG_READ_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D)))
            regScan = ip->a.u32;
        if (flags & OPFLAG_WRITE_B && !(flags & (OPFLAG_READ_B | OPFLAG_WRITE_A | OPFLAG_WRITE_C | OPFLAG_WRITE_D)))
            regScan = ip->b.u32;
        if (flags & OPFLAG_WRITE_C && !(flags & (OPFLAG_READ_C | OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_D)))
            regScan = ip->c.u32;
        if (flags & OPFLAG_WRITE_D && !(flags & (OPFLAG_READ_D | OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C)))
            regScan = ip->d.u32;

        if (regScan == UINT32_MAX)
            return;

        bool hasRead = false;

        parseTree(context, parseCxt.curNode, parseCxt.curIp, BCOTN_USER2, [&](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt1)
        {
            const auto ip1 = parseCxt1.curIp;
            if (ip1 == ip)
                return;

            const auto flags1 = g_ByteCodeOpDesc[(int) ip1->op].flags;
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
                    parseCxt1.mustStopBlock = true;
                    return;
                }
            }

            if (flags1 & OPFLAG_WRITE_B)
            {
                if (ip1->b.u32 == regScan)
                {
                    parseCxt1.mustStopBlock = true;
                    return;
                }
            }

            if (flags1 & OPFLAG_WRITE_C)
            {
                if (ip1->c.u32 == regScan)
                {
                    parseCxt1.mustStopBlock = true;
                    return;
                }
            }

            if (flags1 & OPFLAG_WRITE_D)
            {
                if (ip1->d.u32 == regScan)
                {
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

namespace
{
    bool optimizePassDeadStoreDupScan(ByteCodeOptContext*  context,
                                      uint32_t             curNode,
                                      ByteCodeOptTreeNode* node,
                                      ByteCodeInstruction* ip,
                                      ByteCodeInstruction* ipScan,
                                      bool&                canRemove)
    {
        node->mark = context->mark;

        const bool hasA = ByteCode::hasWriteRegInA(ip);
        const bool hasB = ByteCode::hasWriteRegInB(ip);
        const bool hasC = ByteCode::hasWriteRegInC(ip);
        const bool hasD = ByteCode::hasWriteRegInD(ip);

        if (ipScan >= node->start)
        {
            while (true)
            {
                if (ipScan == ip)
                    return false;

                if (ipScan->op == ip->op)
                {
                    switch (ip->op)
                    {
                    case ByteCodeOp::ClearRA:
                        if (ipScan->a.u32 == ip->a.u32)
                        {
                            canRemove = true;
                            return true;
                        }
                        break;

                    case ByteCodeOp::SetImmediate32:
                        if (ipScan->a.u32 == ip->a.u32 && ipScan->b.u32 == ip->b.u32)
                        {
                            canRemove = true;
                            return true;
                        }
                        break;
                    case ByteCodeOp::SetImmediate64:
                        if (ipScan->a.u32 == ip->a.u32 && ipScan->b.u64 == ip->b.u64)
                        {
                            canRemove = true;
                            return true;
                        }
                        break;

                    case ByteCodeOp::MakeStackPointer:
                    case ByteCodeOp::MakeConstantSegPointer:
                    case ByteCodeOp::MakeCompilerSegPointer:
                    case ByteCodeOp::MakeBssSegPointer:
                    case ByteCodeOp::MakeMutableSegPointer:
                        if (ipScan->a.u32 == ip->a.u32 && ipScan->b.u32 == ip->b.u32)
                        {
                            canRemove = true;
                            return true;
                        }
                        break;

                    case ByteCodeOp::GetParam8:
                    case ByteCodeOp::GetParam16:
                    case ByteCodeOp::GetParam32:
                    case ByteCodeOp::GetParam64:
                        if (ipScan->a.u32 == ip->a.u32 && ipScan->b.u64 == ip->b.u64)
                        {
                            canRemove = true;
                            return true;
                        }
                        break;

                    case ByteCodeOp::GetIncParam64:
                        if (ipScan->a.u32 == ip->a.u32 && ipScan->b.u64 == ip->b.u64 && ipScan->d.u64 == ip->d.u64)
                        {
                            canRemove = true;
                            return true;
                        }
                        break;
                    default:
                        break;
                    }
                }

                if (hasA && ByteCode::hasWriteRefToReg(ipScan, ip->a.u32))
                    return false;
                if (hasB && ByteCode::hasWriteRefToReg(ipScan, ip->b.u32))
                    return false;
                if (hasC && ByteCode::hasWriteRefToReg(ipScan, ip->c.u32))
                    return false;
                if (hasD && ByteCode::hasWriteRefToReg(ipScan, ip->d.u32))
                    return false;

                if (ipScan <= node->start)
                    break;
                ipScan--;
            }
        }

        // In on first block, then we must keep the instruction
        if (node->parent.empty())
            return false;

        for (const auto n : node->parent)
        {
            ByteCodeOptTreeNode* parentNode = &context->tree[n];
            if (parentNode->mark == context->mark && n != curNode)
                continue;

            if (!optimizePassDeadStoreDupScan(context, curNode, parentNode, ip, parentNode->end, canRemove))
                return false;
        }

        return true;
    }
}

bool ByteCodeOptimizer::optimizePassDeadStoreDup(ByteCodeOptContext* context)
{
    context->mark = 0;
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* context, const ByteCodeOptTreeParseContext& parseCxt)
    {
        const auto ip = parseCxt.curIp;
        switch (ip->op)
        {
        case ByteCodeOp::GetParam8:
        case ByteCodeOp::GetParam16:
        case ByteCodeOp::GetParam32:
        case ByteCodeOp::GetParam64:
        case ByteCodeOp::MakeStackPointer:
        case ByteCodeOp::MakeConstantSegPointer:
        case ByteCodeOp::MakeCompilerSegPointer:
        case ByteCodeOp::MakeBssSegPointer:
        case ByteCodeOp::MakeMutableSegPointer:
        case ByteCodeOp::ClearRA:
        case ByteCodeOp::SetImmediate32:
        case ByteCodeOp::SetImmediate64:
        case ByteCodeOp::GetIncParam64:
            break;

        default:
            return;
        }

        ByteCodeOptTreeNode* node = &context->tree[parseCxt.curNode];
        if (node->parent.empty())
            return;

        bool canRemove = false;
        context->mark++;
        if (!optimizePassDeadStoreDupScan(context, parseCxt.curNode, node, ip, ip - 1, canRemove))
            return;

        if (canRemove)
        {
            setNop(context, ip);
        }
    });

    return true;
}

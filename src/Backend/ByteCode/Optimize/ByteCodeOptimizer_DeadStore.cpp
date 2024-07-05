#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

// If two instructions write in the same register in the same block (between jumps), and there's no
// read of that register between them, then the first write is useless and can be removed
bool ByteCodeOptimizer::optimizePassDeadStore(ByteCodeOptContext* context)
{
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* context, const ByteCodeOptTreeParseContext& parseCxt) {
        auto       ip    = parseCxt.curIp;
        const auto flags = g_ByteCodeOpDesc[static_cast<int>(ip->op)].flags;

        uint32_t regScan = UINT32_MAX;
        if (flags.has(OPFLAG_WRITE_A) && !flags.has(OPFLAG_READ_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D))
            regScan = ip->a.u32;
        if (flags.has(OPFLAG_WRITE_B) && !flags.has(OPFLAG_READ_B | OPFLAG_WRITE_A | OPFLAG_WRITE_C | OPFLAG_WRITE_D))
            regScan = ip->b.u32;
        if (flags.has(OPFLAG_WRITE_C) && !flags.has(OPFLAG_READ_C | OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_D))
            regScan = ip->c.u32;
        if (flags.has(OPFLAG_WRITE_D) && !flags.has(OPFLAG_READ_D | OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C))
            regScan = ip->d.u32;
        if (regScan == UINT32_MAX)
            return;

        bool forCpy = false;
        if (ip[1].b.u32 == regScan &&
            !ip[1].flags.has(BCI_START_STMT) &&
            !ip[1].flags.has(BCI_NOT_PURE))
        {
            if (ip[1].op == ByteCodeOp::CopyRBtoRA8 ||
                ip[1].op == ByteCodeOp::CopyRBtoRA16 ||
                ip[1].op == ByteCodeOp::CopyRBtoRA32 ||
                ip[1].op == ByteCodeOp::CopyRBtoRA64)
            {
                forCpy = true;
                ip++;
            }
        }

        if (hasReadRefToReg(context, regScan, parseCxt.curNode, ip))
            return;

        if (forCpy)
        {
            if (ByteCode::hasWriteRegInA(ip - 1))
                ip[-1].a.u32 = ip->a.u32;
            else if (ByteCode::hasWriteRegInB(ip - 1))
                ip[-1].b.u32 = ip->a.u32;
            else if (ByteCode::hasWriteRegInC(ip - 1))
                ip[-1].c.u32 = ip->a.u32;
            else if (ByteCode::hasWriteRegInD(ip - 1))
                ip[-1].d.u32 = ip->a.u32;
            setNop(context, ip);
        }
        else
            setNop(context, ip);
    });

    return true;
}

namespace
{
    bool optimizePassDeadStoreDupScan(ByteCodeOptContext*        context,
                                      uint32_t                   curNode,
                                      ByteCodeOptTreeNode*       node,
                                      ByteCodeInstruction*       ip,
                                      const ByteCodeInstruction* ipScan,
                                      bool&                      canRemove)
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
                    return true;

                if (ipScan->op == ip->op)
                {
                    bool same = true;
                    if (ByteCode::hasSomethingInA(ipScan) && ipScan->a.u64 != ip->a.u64)
                        same = false;
                    if (ByteCode::hasSomethingInB(ipScan) && ipScan->b.u64 != ip->b.u64)
                        same = false;
                    if (ByteCode::hasSomethingInC(ipScan) && ipScan->c.u64 != ip->c.u64)
                        same = false;
                    if (ByteCode::hasSomethingInD(ipScan) && ipScan->d.u64 != ip->d.u64)
                        same = false;
                    if (same)
                    {
                        canRemove = true;
                        return true;
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

// Remove some duplicated instructions
bool ByteCodeOptimizer::optimizePassDeadStoreDup(ByteCodeOptContext* context)
{
    context->mark = 0;
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* context, const ByteCodeOptTreeParseContext& parseCxt) {
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

        context->mark++;

        bool canRemove = false;
        if (!optimizePassDeadStoreDupScan(context, parseCxt.curNode, node, ip, ip - 1, canRemove))
            return;

        if (canRemove)
            setNop(context, ip);
    });

    return true;
}

#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

// If two instructions write in the same register in the same block (between jumps), and there's no
// read of that register between them, then the first write is useless and can be removed
bool ByteCodeOptimizer::optimizePassDeadStore(ByteCodeOptContext* context)
{
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* inContext, const ByteCodeOptTreeParseContext& parseCxt) {
        const auto ip    = parseCxt.curIp;
        const auto flags = ByteCode::opFlags(ip->op);

        uint32_t regScan = UINT32_MAX;

        if (flags.has(OPF_WRITE_A) && !flags.has(OPF_WRITE_B | OPF_WRITE_C | OPF_WRITE_D))
        {
            switch (ip->op)
            {
                case ByteCodeOp::ClearMaskU32:
                case ByteCodeOp::ClearMaskU64:
                    regScan = ip->a.u32;
                    break;
                default:
                    if (!flags.has(OPF_READ_A))
                        regScan = ip->a.u32;
                    break;
            }
        }
        else if (flags.has(OPF_WRITE_B) && !flags.has(OPF_READ_B | OPF_WRITE_A | OPF_WRITE_C | OPF_WRITE_D))
            regScan = ip->b.u32;
        else if (flags.has(OPF_WRITE_C) && !flags.has(OPF_READ_C | OPF_WRITE_A | OPF_WRITE_B | OPF_WRITE_D))
            regScan = ip->c.u32;
        else if (flags.has(OPF_WRITE_D) && !flags.has(OPF_READ_D | OPF_WRITE_A | OPF_WRITE_B | OPF_WRITE_C))
            regScan = ip->d.u32;

        if (regScan == UINT32_MAX)
            return;

        if (hasReadRefToReg(inContext, regScan, parseCxt.curNode, ip))
            return;

        setNop(inContext, ip);
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

        const bool hasA = ip->hasWriteRegInA();
        const bool hasB = ip->hasWriteRegInB();
        const bool hasC = ip->hasWriteRegInC();
        const bool hasD = ip->hasWriteRegInD();

        if (ipScan >= node->start)
        {
            while (true)
            {
                if (ipScan == ip)
                    return true;

                if (ipScan->op == ip->op)
                {
                    bool same = true;
                    if (ipScan->hasSomethingInA() && ipScan->a.u64 != ip->a.u64)
                        same = false;
                    if (ipScan->hasSomethingInB() && ipScan->b.u64 != ip->b.u64)
                        same = false;
                    if (ipScan->hasSomethingInC() && ipScan->c.u64 != ip->c.u64)
                        same = false;
                    if (ipScan->hasSomethingInD() && ipScan->d.u64 != ip->d.u64)
                        same = false;
                    if (same)
                    {
                        canRemove = true;
                        return true;
                    }
                }

                if (hasA && ipScan->hasWriteRefToReg(ip->a.u32))
                    return false;
                if (hasB && ipScan->hasWriteRefToReg(ip->b.u32))
                    return false;
                if (hasC && ipScan->hasWriteRefToReg(ip->c.u32))
                    return false;
                if (hasD && ipScan->hasWriteRefToReg(ip->d.u32))
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
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* inContext, const ByteCodeOptTreeParseContext& parseCxt) {
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

        ByteCodeOptTreeNode* node = &inContext->tree[parseCxt.curNode];
        if (node->parent.empty())
            return;

        inContext->mark++;

        bool canRemove = false;
        if (!optimizePassDeadStoreDupScan(inContext, parseCxt.curNode, node, ip, ip - 1, canRemove))
            return;

        if (canRemove)
            setNop(inContext, ip);
    });

    return true;
}

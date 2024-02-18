#include "pch.h"
#include "ByteCodeOptimizer.h"

bool ByteCodeOptimizer::optimizePassDupBlocks(ByteCodeOptContext* context)
{
    context->map32Node.clear();
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt)
    {
        ByteCodeOptTreeNode* node = &context->tree[parseCxt.curNode];
        parseCxt.curIp            = node->end;

        const uint64_t countBlock = node->end - node->start + 1;

        // Only factorize terminal blocks
        if (!ByteCode::isRet(node->end) && node->end->op != ByteCodeOp::Jump)
            return;

        // Only one instruction in the block, do nothing
        if (countBlock == 1)
            return;

        // If this is a return block, then when exactly should we factorise ?
        // Is it a good idea to add a jump (for speed) ? 
        // Should we test optimize for size versus optimize for speed ?

        // Here we consider that if the block has only 2 instructions, then we do not jump to a block with also 2 instructions.
        // But '2' is kind of magical...
        //if (ByteCode::isRet(node->end) && countBlock <= 2)
        //    return;

        const auto it = context->map32Node.find(node->crc);
        if (it != context->map32Node.end())
        {
            const bool same = ByteCode::areSame(node->start, node->end + 1, it->second->start, it->second->end + 1, true, false);
            if (same)
            {
                SET_OP(node->start, ByteCodeOp::Jump);
                node->start->b.s32 = static_cast<int32_t>(it->second->start - node->start) - 1;
            }
        }
        else
        {
            context->map32Node[node->crc] = node;
        }
    });

    return true;
}

#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

bool ByteCodeOptimizer::optimizePassDupBlocks(ByteCodeOptContext* context)
{
    context->map32Node.clear();
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt) {
        ByteCodeOptTreeNode* node = &context->tree[parseCxt.curNode];
        parseCxt.curIp            = node->end;

        // Only factorize terminal blocks
        if (!ByteCode::isRet(node->end) && node->end->op != ByteCodeOp::Jump)
            return;

        // Only one instruction in the block, do nothing
        const uint64_t countBlock = node->end - node->start + 1;
        if (countBlock == 1 && !ByteCode::isRet(node->end))
            return;

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

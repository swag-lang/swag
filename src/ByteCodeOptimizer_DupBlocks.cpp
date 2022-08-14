#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Log.h"

bool ByteCodeOptimizer::optimizePassDupBlocks(ByteCodeOptContext* context)
{
    context->map32Node.clear();
    parseTree(context, 0, context->tree[0].start, BCOTN_USER1, [](ByteCodeOptContext* context, ByteCodeOptTreeParseContext& parseCxt)
              {
            ByteCodeOptTreeNode* node = &context->tree[parseCxt.curNode];
            parseCxt.curIp = node->end;

            if (node->end->op != ByteCodeOp::Ret)
                return;

            auto it = context->map32Node.find(node->crc);
            if (it != context->map32Node.end())
            {
                bool same = it->second->end - it->second->start == node->end - node->start;
                ByteCodeInstruction* ip0 = it->second->start;
                ByteCodeInstruction* ip1 = node->start;
                while (same && ip1 != node->end + 1)
                {
                    if (ip0->op != ip1->op)
                        same = false;
                    else if (ip0->flags != ip1->flags)
                        same = false;
                    else if (ByteCode::hasSomethingInA(ip0) && ip0->a.u64 != ip1->a.u64)
                        same = false;
                    else if (ByteCode::hasSomethingInB(ip0) && ip0->b.u64 != ip1->b.u64)
                        same = false;
                    else if (ByteCode::hasSomethingInC(ip0) && ip0->c.u64 != ip1->c.u64)
                        same = false;
                    else if (ByteCode::hasSomethingInD(ip0) && ip0->d.u64 != ip1->d.u64)
                        same = false;
                    ip0++;
                    ip1++;
                }

                if (same)
                {
                    SET_OP(node->start, ByteCodeOp::Jump);
                    node->start->b.s32 = (int32_t) (it->second->start - node->start) - 1;
                }
            }
            else
            {
                context->map32Node[node->crc] = node;
            } });

    return true;
}
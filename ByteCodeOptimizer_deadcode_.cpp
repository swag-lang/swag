#include "pch.h"
#include "ByteCodeOptimizer.h"

// Eliminate all the instructions that can never be called
// We parse all the branches to see what can be reached for an execution flow
void ByteCodeOptimizer::optimizePassDeadCode(ByteCodeOptContext* context)
{
    context->toDo.reserve(context->bc->numInstructions);
    context->toDo.clear();

#define ADD_TODO(__ip)                   \
    if (!((__ip)->flags & BCI_OPT_FLAG)) \
    {                                    \
        (__ip)->flags |= BCI_OPT_FLAG;   \
        context->toDo.push_back(__ip);            \
    }

    ADD_TODO(context->bc->out);
    while (!context->toDo.empty())
    {
        auto ip = context->toDo.back();
        context->toDo.pop_back();

        if (ip->op == ByteCodeOp::Jump)
        {
            ADD_TODO(ip + ip->b.s32 + 1);
        }
        else if (ip->op == ByteCodeOp::JumpIfTrue ||
                 ip->op == ByteCodeOp::JumpIfFalse ||
                 ip->op == ByteCodeOp::JumpIfZero32 ||
                 ip->op == ByteCodeOp::JumpIfZero64 ||
                 ip->op == ByteCodeOp::JumpIfNotZero32 ||
                 ip->op == ByteCodeOp::JumpIfNotZero64)
        {
            ADD_TODO(ip + ip->b.s32 + 1);
            ADD_TODO(ip + 1);
        }
        else if (ip->op != ByteCodeOp::Ret && ip->op != ByteCodeOp::End)
        {
            ADD_TODO(ip + 1);
        }
    }

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (!(ip->flags & BCI_OPT_FLAG))
            setNop(context, ip);
        else
            ip->flags &= ~BCI_OPT_FLAG;
    }
}

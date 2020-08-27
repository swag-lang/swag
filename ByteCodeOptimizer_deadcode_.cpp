#include "pch.h"
#include "ByteCodeOptimizer.h"

void optimizeDeadCode(ByteCodeOptContext* context)
{
    VectorNative<ByteCodeInstruction*> toDo;
    toDo.reserve(context->bc->numInstructions);

#define ADD_TODO(__ip)                   \
    if (!((__ip)->flags & BCI_OPT_FLAG)) \
    {                                    \
        (__ip)->flags |= BCI_OPT_FLAG;   \
        toDo.push_back(__ip);            \
    }

    ADD_TODO(context->bc->out);
    while (!toDo.empty())
    {
        auto ip = toDo.back();
        toDo.pop_back();

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
            ByteCodeOptimizer::setNop(context, ip);
        else
            ip->flags &= ~BCI_OPT_FLAG;
    }
}

void ByteCodeOptimizer::optimizePassDeadCode(ByteCodeOptContext* context)
{
    optimizeDeadCode(context);
    removeNops(context);
}
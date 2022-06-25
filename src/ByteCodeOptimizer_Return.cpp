#include "pch.h"
#include "ByteCodeOptimizer.h"

bool ByteCodeOptimizer::optimizePassReturn(ByteCodeOptContext* context)
{
    context->vecReg.clear();

    int idx = 0;
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::Ret)
            context->vecReg.push_back(idx);
        idx++;
    }

    if (context->vecReg.size() <= 1)
        return true;

    for (int i = 0; i < context->vecReg.size() - 1; i++)
    {
        auto ip = context->bc->out + context->vecReg[i];
        SET_OP(ip, ByteCodeOp::Jump);
        ip->b.s32 = context->vecReg.back() - context->vecReg[i] - 1;
    }

    return true;
}

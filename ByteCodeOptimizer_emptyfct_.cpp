#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "TypeManager.h"

void ByteCodeOptimizer::optimizePassEmptyFct(ByteCodeOptContext* context)
{
    if (context->bc->numInstructions == 2)
    {
        if (context->bc->out[0].op == ByteCodeOp::Ret && (context->bc->out[1].op == ByteCodeOp::End))
        {
            SWAG_ASSERT(context->bc->callType()->returnType == g_TypeMgr.typeInfoVoid);
            context->bc->isEmpty = true;
        }

        return;
    }

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::LocalCall)
        {
            auto destBC = (ByteCode*) ip->a.pointer;
            if (destBC->isEmpty.load() == true)
            {
                setNop(context, ip);
            }
        }
    }
}

#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "TypeManager.h"

// Mark all functions that does nothing
// Elimite all local calls to such functions
void ByteCodeOptimizer::optimizePassEmptyFct(ByteCodeOptContext* context)
{
    if (context->bc->numInstructions == 2)
    {
        if (context->bc->out[0].op == ByteCodeOp::Ret)
        {
            SWAG_ASSERT(context->bc->callType()->returnType == g_TypeMgr.typeInfoVoid);
            context->bc->isEmpty = true;
        }

        return;
    }

    if (context->bc->numInstructions == 4)
    {
        if (context->bc->out[0].op == ByteCodeOp::DecSPBP &&
            context->bc->out[1].op == ByteCodeOp::IncSP &&
            context->bc->out[2].op == ByteCodeOp::Ret)
        {
            SWAG_ASSERT(context->bc->callType()->returnType == g_TypeMgr.typeInfoVoid);
            context->bc->isEmpty = true;
        }

        return;
    }

    // Eliminate local calls to empty functions
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::LocalCall)
        {
            auto destBC = (ByteCode*) ip->a.pointer;
            if (destBC->isEmpty.load() != true)
                continue;

            // We eliminate the call to the function that does nothing
            setNop(context, ip);
            if (ip[1].op == ByteCodeOp::IncSPPostCall)
                setNop(context, ip + 1);

            // Then we can eliminate some instructions related to the function call parameters
            auto backIp = ip;
            while (backIp != context->bc->out &&
                   !(backIp->flags & BCI_START_STMT) &&
                   backIp->op != ByteCodeOp::LocalCall &&
                   backIp->op != ByteCodeOp::ForeignCall &&
                   backIp->op != ByteCodeOp::LambdaCall)
            {
                if (backIp->op == ByteCodeOp::PushRAParam ||
                    backIp->op == ByteCodeOp::PushRAParam2 ||
                    backIp->op == ByteCodeOp::PushRAParam3 ||
                    backIp->op == ByteCodeOp::PushRAParam4 ||
                    backIp->op == ByteCodeOp::CopySPVaargs ||
                    backIp->op == ByteCodeOp::CopySP)
                    setNop(context, backIp);
                backIp--;
            }
        }
    }
}

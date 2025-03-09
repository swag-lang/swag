#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Wmf/Module.h"

// Eliminate all the instructions that can never be called
// We parse all the branches to see what can be reached for an execution flow
bool ByteCodeOptimizer::optimizePassDeadCode(ByteCodeOptContext* context)
{
    context->vecInst.reserve(context->bc->numInstructions);
    context->vecInst.clear();

#define ADD_TODO(__ip)                            \
    do                                            \
    {                                             \
        if (!(__ip)->dynFlags.has(BCID_OPT_FLAG)) \
        {                                         \
            (__ip)->dynFlags.add(BCID_OPT_FLAG);  \
            context->vecInst.push_back(__ip);     \
        }                                         \
    } while (0)

    ADD_TODO(context->bc->out);
    while (!context->vecInst.empty())
    {
        const auto ip = context->vecInst.back();
        context->vecInst.pop_back();

        if (ip->op == ByteCodeOp::Jump)
        {
            ADD_TODO(ip + ip->b.s32 + 1);
        }
        else if (ip->isJump())
        {
            ADD_TODO(ip + ip->b.s32 + 1);
            ADD_TODO(ip + 1);
        }
        else if (ip->isJumpDyn())
        {
            const auto table = reinterpret_cast<int32_t*>(context->module->compilerSegment.address(ip->d.u32));
            for (uint32_t i = 0; i < ip->c.u32; i++)
            {
                ADD_TODO(ip + table[i] + 1);
            }
        }
        else if (!ip->isRet() && ip->op != ByteCodeOp::End)
        {
            ADD_TODO(ip + 1);
        }
    }

    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (!ip->dynFlags.has(BCID_OPT_FLAG))
            setNop(context, ip);
        else
            ip->dynFlags.remove(BCID_OPT_FLAG);
    }

    return true;
}

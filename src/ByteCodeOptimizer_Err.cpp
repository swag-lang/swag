#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Math.h"

// Try to change JumpIfNotError with JumpIfError.
// This make the "no error path" the "hot path", branching with a jump only in case of errors.
//
// The error code is placed at the end of the function. So this function can increase temporary the number of instructions.
bool ByteCodeOptimizer::optimizePassErr(ByteCodeOptContext* context)
{
    if (!(context->contextBcFlags & OCF_HAS_ERR))
        return true;

    const auto bc = context->bc;
    if (!context->nops.empty())
        return true;

    for (uint32_t cpt = 0; cpt < bc->numInstructions; cpt++)
    {
        auto ip = bc->out + cpt;
        if (ip->op != ByteCodeOp::JumpIfNoError)
            continue;

        const auto countErrInst = ip->b.s32;
        bc->makeRoomForInstructions(countErrInst);
        ip = bc->out + cpt;

        const auto ipJump = ip;
        ip += 1;
        const auto saveEnd = bc->out[bc->numInstructions - 1];

        bool invalid = false;
        if (ip[countErrInst - 1].op != ByteCodeOp::Ret)
            invalid = true;
        else
        {
            for (int i = 0; i < countErrInst; i++)
            {
                if (ip[i].flags & BCI_START_STMT)
                {
                    invalid = true;
                    break;
                }

                if (ByteCode::isRet(ip + i) && i != countErrInst - 1)
                {
                    invalid = true;
                    break;
                }

                if (ByteCode::isJump(ip + i))
                {
                    invalid = true;
                    break;
                }
            }
        }

        if (invalid)
            continue;

        for (int i = 0; i < countErrInst; i++)
        {
            bc->out[bc->numInstructions + i - 1] = ip[i];
            setNop(context, ip + i);
        }

        SET_OP(ipJump, ByteCodeOp::JumpIfError);
        ipJump->b.s32 = bc->numInstructions - cpt - 2;

        bc->numInstructions += countErrInst;
        bc->out[bc->numInstructions - 1] = saveEnd;
    }

    return true;
}

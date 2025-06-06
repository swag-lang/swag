#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Wmf/SourceFile.h"

// Try to change JumpIfNotError with JumpIfError.
// This makes the "no error path" the "hot path", branching with a jump only in case of errors.
//
// The error code is placed at the end of the function. So this function can temporarily increase the number of instructions.
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
                if (ip[i].hasFlag(BCI_START_STMT))
                {
                    invalid = true;
                    break;
                }

                if (ip[i].isRet() && i != countErrInst - 1)
                {
                    invalid = true;
                    break;
                }

                if (ip[i].isJump())
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
        ipJump->b.s32 = static_cast<int32_t>(bc->numInstructions - cpt - 2);

        bc->numInstructions += countErrInst;
        bc->out[bc->numInstructions - 1] = saveEnd;
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassRetErr(ByteCodeOptContext* context)
{
    ByteCodeInstruction* ip0 = nullptr;
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->hasFlag(BCI_TRY_CATCH) && ip->isRet())
        {
            if (ip0)
            {
                if (ByteCode::areSame(ip0, ip0 + 1, ip, ip + 1, false, false))
                {
                    while (ByteCode::areSame(ip0 - 1, ip0, ip - 1, ip, false, false))
                    {
                        if (ip0[-1].isJump() || ip0[-1].isRet() || ip0[-1].isCall())
                            break;
                        ip--;
                        ip0--;
                        if (ip->hasFlag(BCI_START_STMT) || ip0->hasFlag(BCI_START_STMT))
                            break;
                    }

                    SET_OP(ip, ByteCodeOp::Jump);
                    ip->b.s32 = static_cast<int32_t>(ip0 - ip - 1);
                    ip0->addFlag(BCI_START_STMT);
                    continue;
                }
            }

            ip0 = ip;
        }
    }

    return true;
}

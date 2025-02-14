#include "pch.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"

// Eliminate unnecessary jumps
bool ByteCodeOptimizer::optimizePassJump(ByteCodeOptContext* context)
{
    for (uint32_t idx = 0; idx < context->jumps.size(); idx++)
    {
        const auto ip = context->jumps[idx];
        if (!ByteCode::isJump(ip))
            continue;

        auto destIp = ip + ip->b.s32 + 1;

        // Jump to an unconditional jump
        while (destIp->op == ByteCodeOp::Jump)
        {
            if (destIp->b.s32 + 1 == 0) // infinite jump
                break;
            ip->b.s32 += destIp->b.s32 + 1;
            destIp += destIp->b.s32 + 1;
            context->setDirtyPass();
        }

        // Jump to the next instruction
        if (ip->b.s32 == 0)
            setNop(context, ip);

        // Jump conditional followed by unconditional jump
        // Revert the condition to remove the unconditional jump
        switch (ip->op)
        {
            case ByteCodeOp::JumpIfGreaterF32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqF32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqF32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqF32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerF32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerF32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerF32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqF32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqF32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqF32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterF32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterF32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterF64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqF64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqF64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqF64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerF64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerF64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerF64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqF64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqF64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqF64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterF64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterF64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterS8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqS8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerS8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqS8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterS16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqS16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerS16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqS16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterS32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqS32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerS32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqS32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterS64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqS64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqS64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerS64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerS64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqS64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqS64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterS64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterU8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqU8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerU8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqU8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterU16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqU16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerU16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqU16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterU32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqU32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerU32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqU32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfGreaterU64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerEqU64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfGreaterEqU64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfLowerU64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerU64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterEqU64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfLowerEqU64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfGreaterU64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            //
            case ByteCodeOp::JumpIfRTFalse:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfRTTrue);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfRTTrue);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfFalse:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfTrue);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfTrue);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfRTTrue:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfRTFalse);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfRTFalse);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfTrue:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfFalse);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfFalse);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }
                break;

            case ByteCodeOp::JumpIfNotZero8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfNotZero16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfNotZero32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfNotZero64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfZero64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfZero8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfZero16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfZero32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfZero64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotZero64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                if (destIp->op == ByteCodeOp::JumpIfNotZero64 &&
                    ip->a.u32 == destIp->a.u32 &&
                    !ip->flags.has(BCI_IMM_A) &&
                    !destIp->flags.has(BCI_IMM_A))
                {
                    ip->b.u64 += 1;
                    context->setDirtyPass();
                }

                if (destIp->op == ByteCodeOp::JumpIfZero64 &&
                    ip->a.u32 == destIp->a.u32 &&
                    !ip->flags.has(BCI_IMM_A) &&
                    !destIp->flags.has(BCI_IMM_A))
                {
                    ip->b.s64 += destIp->b.s64 + 1;
                    context->setDirtyPass();
                }

                break;

            case ByteCodeOp::JumpIfEqual8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfEqual16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfEqual32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfEqualF32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqualF32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqualF32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfEqual64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqual64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfEqualF64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqualF64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfNotEqualF64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfNotEqual8:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual8);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual8);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfNotEqual16:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual16);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual16);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfNotEqual32:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual32);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual32);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            case ByteCodeOp::JumpIfNotEqual64:
                if (ip[1].op == ByteCodeOp::Jump &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual64);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                    break;
                }

                if (ip[1].op == ByteCodeOp::Ret &&
                    context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret &&
                    ip[0].b.s32 == 1 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfEqual64);
                    ip->b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1));
                    setNop(context, ip + 1);
                    break;
                }

                break;

            default:
                break;
        }

        switch (ip->op)
        {
            case ByteCodeOp::Jump:
                // Next instruction is a jump to the same target
                if (ip[1].op == ByteCodeOp::Jump && ip->b.s32 - 1 == ip[1].b.s32)
                {
                    setNop(context, ip);
                }

                // Jump to a JumpIfZero, which in turns jumps right after the Jump id really zero
                // (occurs in loops).
                // Replace Jump with jumpIfNotZero
                else if (ip->b.s32 < 0)
                {
                    destIp = ip + ip->b.s32 + 1;
                    if (destIp->op == ByteCodeOp::JumpIfZero64)
                    {
                        const auto destIp1 = destIp + destIp->b.s32 + 1;
                        if (destIp1 == ip + 1)
                        {
                            SET_OP(ip, ByteCodeOp::JumpIfNotZero64);
                            ip->a.u32 = destIp->a.u32;
                            ip->b.s32 += 1;
                        }
                    }
                }
                break;

            case ByteCodeOp::JumpIfFalse:
                destIp = ip + ip->b.s32 + 1;

                // Jump if false to a jump if false with the same register
                if (destIp->op == ByteCodeOp::JumpIfFalse &&
                    destIp->a.u32 == ip->a.u32 &&
                    !ip->hasFlag(BCI_IMM_A) &&
                    destIp->b.s32 + 1 && // in case it's an empty loop
                    !destIp->hasFlag(BCI_IMM_A))
                {
                    ip->b.s32 += destIp->b.s32 + 1;
                    context->setDirtyPass();
                }

                // Jump if false to a jump if true with the same register
                else if (destIp->op == ByteCodeOp::JumpIfTrue &&
                         destIp->a.u32 == ip->a.u32 &&
                         !ip->hasFlag(BCI_IMM_A) &&
                         !destIp->hasFlag(BCI_IMM_A))
                {
                    ip->b.s32 += 1;
                    destIp[1].addFlag(BCI_START_STMT);
                    context->setDirtyPass();
                }

                // If we have :
                // 0: (jump if false) to 2
                // 1: (jump) to whatever
                // 2: ...
                // Then we switch to (jump if true) whatever, and eliminate the unconditional jump
                else if (ip->b.s32 == 1 &&
                         ip[1].op == ByteCodeOp::Jump &&
                         !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfTrue);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                }
                break;

            case ByteCodeOp::JumpIfTrue:
                destIp = ip + ip->b.s32 + 1;

                // Jump if true to a jump if true with the same register
                if (destIp->op == ByteCodeOp::JumpIfTrue &&
                    destIp->a.u32 == ip->a.u32 &&
                    !ip->hasFlag(BCI_IMM_A) &&
                    destIp->b.s32 + 1 && // in case it's an empty loop
                    !destIp->hasFlag(BCI_IMM_A))
                {
                    ip->b.s32 += destIp->b.s32 + 1;
                    context->setDirtyPass();
                }

                // Jump if true to a jump if false with the same register
                else if (destIp->op == ByteCodeOp::JumpIfFalse &&
                         destIp->a.u32 == ip->a.u32 &&
                         !ip->hasFlag(BCI_IMM_A) &&
                         !destIp->hasFlag(BCI_IMM_A))
                {
                    ip->b.s32 += 1;
                    destIp[1].addFlag(BCI_START_STMT);
                    context->setDirtyPass();
                }

                // If we have :
                // 0: (jump if true) to 2
                // 1: (jump) to whatever
                // 2: ...
                // Then we switch to (jump if false) whatever, and eliminate the unconditional jump
                else if (ip->b.s32 == 1 &&
                         ip[1].op == ByteCodeOp::Jump &&
                         !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip, ByteCodeOp::JumpIfFalse);
                    ip->b.s32 = ip[1].b.s32 + 1;
                    setNop(context, ip + 1);
                }
                break;

            case ByteCodeOp::JumpIfNotZero32:
                if (ip->b.s32 == -2)
                {
                    if (ip[-1].op == ByteCodeOp::DecrementRA32 && ip[-1].a.u32 == ip->a.u32)
                    {
                        setNop(context, ip);
                        setNop(context, ip - 1);
                        break;
                    }
                }
                break;

            case ByteCodeOp::JumpIfNotZero64:
                if (ip[-1].op == ByteCodeOp::MakeConstantSegPointer ||
                    ip[-1].op == ByteCodeOp::MakeBssSegPointer ||
                    ip[-1].op == ByteCodeOp::MakeCompilerSegPointer ||
                    ip[-1].op == ByteCodeOp::MakeMutableSegPointer ||
                    ip[-1].op == ByteCodeOp::MakeStackPointer)
                {
                    if (ip[-1].a.u32 == ip->a.u32)
                    {
                        SET_OP(ip, ByteCodeOp::Jump);
                        break;
                    }
                }

                if (ip->b.s32 == -2)
                {
                    if (ip[-1].op == ByteCodeOp::DecrementRA64 && ip[-1].a.u32 == ip->a.u32)
                    {
                        setNop(context, ip);
                        setNop(context, ip - 1);
                    }
                }
                break;

            default:
                break;
        }
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassConstJump(ByteCodeOptContext* context)
{
#define OPT_JMP_AC(__op, __val)           \
    do                                    \
    {                                     \
        if (ip->a.__val __op ip->c.__val) \
        {                                 \
            SET_OP(ip, ByteCodeOp::Jump); \
        }                                 \
        else                              \
        {                                 \
            setNop(context, ip);          \
        }                                 \
    } while (0)

    for (uint32_t idx = 0; idx < context->jumps.size(); idx++)
    {
        const auto ip = context->jumps[idx];
        if (!ByteCode::isJump(ip))
            continue;

        if (ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))
        {
            switch (ip->op)
            {
                case ByteCodeOp::JumpIfEqual8:
                    OPT_JMP_AC(==, u8);
                    break;
                case ByteCodeOp::JumpIfEqual16:
                    OPT_JMP_AC(==, u16);
                    break;
                case ByteCodeOp::JumpIfEqual32:
                    OPT_JMP_AC(==, u32);
                    break;
                case ByteCodeOp::JumpIfEqual64:
                    OPT_JMP_AC(==, u64);
                    break;
                case ByteCodeOp::JumpIfEqualF32:
                    OPT_JMP_AC(==, f32);
                    break;
                case ByteCodeOp::JumpIfEqualF64:
                    OPT_JMP_AC(==, f64);
                    break;

                case ByteCodeOp::JumpIfNotEqual8:
                    OPT_JMP_AC(!=, u8);
                    break;
                case ByteCodeOp::JumpIfNotEqual16:
                    OPT_JMP_AC(!=, u16);
                    break;
                case ByteCodeOp::JumpIfNotEqual32:
                    OPT_JMP_AC(!=, u32);
                    break;
                case ByteCodeOp::JumpIfNotEqual64:
                    OPT_JMP_AC(!=, u64);
                    break;
                case ByteCodeOp::JumpIfNotEqualF32:
                    OPT_JMP_AC(!=, f32);
                    break;
                case ByteCodeOp::JumpIfNotEqualF64:
                    OPT_JMP_AC(!=, f64);
                    break;

                case ByteCodeOp::JumpIfGreaterS8:
                    OPT_JMP_AC(>, s8);
                    break;
                case ByteCodeOp::JumpIfGreaterS16:
                    OPT_JMP_AC(>, s16);
                    break;
                case ByteCodeOp::JumpIfGreaterS32:
                    OPT_JMP_AC(>, s32);
                    break;
                case ByteCodeOp::JumpIfGreaterS64:
                    OPT_JMP_AC(>, s64);
                    break;
                case ByteCodeOp::JumpIfGreaterU8:
                    OPT_JMP_AC(>, u8);
                    break;
                case ByteCodeOp::JumpIfGreaterU16:
                    OPT_JMP_AC(>, u16);
                    break;
                case ByteCodeOp::JumpIfGreaterU32:
                    OPT_JMP_AC(>, u32);
                    break;
                case ByteCodeOp::JumpIfGreaterU64:
                    OPT_JMP_AC(>, u64);
                    break;
                case ByteCodeOp::JumpIfGreaterF32:
                    OPT_JMP_AC(>, f32);
                    break;
                case ByteCodeOp::JumpIfGreaterF64:
                    OPT_JMP_AC(>, f64);
                    break;

                case ByteCodeOp::JumpIfGreaterEqS8:
                    OPT_JMP_AC(>=, s8);
                    break;
                case ByteCodeOp::JumpIfGreaterEqS16:
                    OPT_JMP_AC(>=, s16);
                    break;
                case ByteCodeOp::JumpIfGreaterEqS32:
                    OPT_JMP_AC(>=, s32);
                    break;
                case ByteCodeOp::JumpIfGreaterEqS64:
                    OPT_JMP_AC(>=, s64);
                    break;
                case ByteCodeOp::JumpIfGreaterEqU8:
                    OPT_JMP_AC(>=, u8);
                    break;
                case ByteCodeOp::JumpIfGreaterEqU16:
                    OPT_JMP_AC(>=, u16);
                    break;
                case ByteCodeOp::JumpIfGreaterEqU32:
                    OPT_JMP_AC(>=, u32);
                    break;
                case ByteCodeOp::JumpIfGreaterEqU64:
                    OPT_JMP_AC(>=, u64);
                    break;
                case ByteCodeOp::JumpIfGreaterEqF32:
                    OPT_JMP_AC(>=, f32);
                    break;
                case ByteCodeOp::JumpIfGreaterEqF64:
                    OPT_JMP_AC(>=, f64);
                    break;

                case ByteCodeOp::JumpIfLowerS8:
                    OPT_JMP_AC(<, s8);
                    break;
                case ByteCodeOp::JumpIfLowerS16:
                    OPT_JMP_AC(<, s16);
                    break;
                case ByteCodeOp::JumpIfLowerS32:
                    OPT_JMP_AC(<, s32);
                    break;
                case ByteCodeOp::JumpIfLowerS64:
                    OPT_JMP_AC(<, s64);
                    break;
                case ByteCodeOp::JumpIfLowerU8:
                    OPT_JMP_AC(<, u8);
                    break;
                case ByteCodeOp::JumpIfLowerU16:
                    OPT_JMP_AC(<, u16);
                    break;
                case ByteCodeOp::JumpIfLowerU32:
                    OPT_JMP_AC(<, u32);
                    break;
                case ByteCodeOp::JumpIfLowerU64:
                    OPT_JMP_AC(<, u64);
                    break;
                case ByteCodeOp::JumpIfLowerF32:
                    OPT_JMP_AC(<, f32);
                    break;
                case ByteCodeOp::JumpIfLowerF64:
                    OPT_JMP_AC(<, f64);
                    break;

                case ByteCodeOp::JumpIfLowerEqS8:
                    OPT_JMP_AC(<=, s8);
                    break;
                case ByteCodeOp::JumpIfLowerEqS16:
                    OPT_JMP_AC(<=, s16);
                    break;
                case ByteCodeOp::JumpIfLowerEqS32:
                    OPT_JMP_AC(<=, s32);
                    break;
                case ByteCodeOp::JumpIfLowerEqS64:
                    OPT_JMP_AC(<=, s64);
                    break;
                case ByteCodeOp::JumpIfLowerEqU8:
                    OPT_JMP_AC(<=, u8);
                    break;
                case ByteCodeOp::JumpIfLowerEqU16:
                    OPT_JMP_AC(<=, u16);
                    break;
                case ByteCodeOp::JumpIfLowerEqU32:
                    OPT_JMP_AC(<=, u32);
                    break;
                case ByteCodeOp::JumpIfLowerEqU64:
                    OPT_JMP_AC(<=, u64);
                    break;
                case ByteCodeOp::JumpIfLowerEqF32:
                    OPT_JMP_AC(<=, f32);
                    break;
                case ByteCodeOp::JumpIfLowerEqF64:
                    OPT_JMP_AC(<=, f64);
                    break;

                default:
                    break;
            }
        }

#define OPT_JMP_A0(__op, __val)           \
    do                                    \
    {                                     \
        if (ip->a.__val __op 0)           \
        {                                 \
            SET_OP(ip, ByteCodeOp::Jump); \
        }                                 \
        else                              \
        {                                 \
            setNop(context, ip);          \
        }                                 \
    } while (0)

        // Evaluate the jump if the condition is constant
        if (ip->hasFlag(BCI_IMM_A))
        {
            switch (ip->op)
            {
                case ByteCodeOp::JumpIfFalse:
                case ByteCodeOp::JumpIfZero8:
                    OPT_JMP_A0(==, u8);
                    break;
                case ByteCodeOp::JumpIfTrue:
                case ByteCodeOp::JumpIfNotZero8:
                    OPT_JMP_A0(!=, u8);
                    break;
                case ByteCodeOp::JumpIfZero16:
                    OPT_JMP_A0(==, u16);
                    break;
                case ByteCodeOp::JumpIfZero32:
                    OPT_JMP_A0(==, u32);
                    break;
                case ByteCodeOp::JumpIfZero64:
                    OPT_JMP_A0(==, u64);
                    break;
                case ByteCodeOp::JumpIfNotZero16:
                    OPT_JMP_A0(!=, u16);
                    break;
                case ByteCodeOp::JumpIfNotZero32:
                    OPT_JMP_A0(!=, u32);
                    break;
                case ByteCodeOp::JumpIfNotZero64:
                    OPT_JMP_A0(!=, u64);
                    break;

                default:
                    break;
            }
        }
    }

    return true;
}

namespace
{
    void optimizePassSwitchOp(ByteCodeOptContext* context, ByteCodeOp op0, ByteCodeOp op1)
    {
        int sizeOf = 0;
        switch (op0)
        {
            case ByteCodeOp::JumpIfEqual8:
                sizeOf = 1;
                break;
            case ByteCodeOp::JumpIfEqual16:
                sizeOf = 2;
                break;
            case ByteCodeOp::JumpIfEqual32:
                sizeOf = 4;
                break;
            case ByteCodeOp::JumpIfEqual64:
                sizeOf = 8;
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }

        constexpr int64_t minJumpTableSize = 4;
        constexpr int64_t maxJumpTableSize = UINT32_MAX;
        constexpr int64_t minDensity       = 10;

        for (uint32_t idx = 0; idx < context->jumps.size(); idx++)
        {
            const auto ip = context->jumps[idx];
            if (!ByteCode::isJump(ip))
                continue;

            const auto orgValue0 = ip->a.u32;
            auto       orgValue1 = UINT32_MAX;
            if (ip[-1].op == ByteCodeOp::CopyRBtoRA64 && ip[-1].b.u32 == ip->a.u32 && !ip->hasFlag(BCI_START_STMT))
                orgValue1 = ip[-1].a.u32;

            const auto ipStart = ip;
            context->map6432.clear();
            context->vecInst.clear();

            auto                       destIp    = ip;
            const ByteCodeInstruction* defaultIp = nullptr;
            while (true)
            {
                defaultIp = destIp;

                if (destIp->op == op0 && destIp->hasFlag(BCI_IMM_C) && destIp->b.s32 > 0 &&
                    (destIp->a.u32 == orgValue0 || destIp->a.u32 == orgValue1))
                {
                    if (context->map6432.contains(destIp->c.u64)) // Only one value per jump
                        break;

                    context->vecInst.push_back(destIp);
                    const auto offset = static_cast<uint32_t>(destIp - ipStart) + destIp->b.s32;
                    switch (sizeOf)
                    {
                        case 1:
                            context->map6432[destIp->c.s8] = offset;
                            break;
                        case 2:
                            context->map6432[destIp->c.s16] = offset;
                            break;
                        case 4:
                            context->map6432[destIp->c.s32] = offset;
                            break;
                        case 8:
                            context->map6432[destIp->c.s64] = offset;
                            break;
                        default:
                            break;
                    }

                    destIp++;
                    continue;
                }

                if (destIp->op == op1 && destIp->hasFlag(BCI_IMM_C) && destIp->b.s32 > 0 &&
                    (destIp->a.u32 == orgValue0 || destIp->a.u32 == orgValue1))
                {
                    if (context->map6432.contains(destIp->c.u64)) // Only one value per jump
                        break;

                    context->vecInst.push_back(destIp);
                    const auto offset = static_cast<uint32_t>(destIp - ipStart);
                    switch (sizeOf)
                    {
                        case 1:
                            context->map6432[destIp->c.s8] = offset;
                            break;
                        case 2:
                            context->map6432[destIp->c.s16] = offset;
                            break;
                        case 4:
                            context->map6432[destIp->c.s32] = offset;
                            break;
                        case 8:
                            context->map6432[destIp->c.s64] = offset;
                            break;
                        default:
                            break;
                    }

                    destIp = destIp + destIp->b.s32 + 1;
                    continue;
                }

                break;
            }

            int64_t minValue = INT64_MAX;
            int64_t maxValue = 0;
            for (const auto inst : context->vecInst)
            {
                switch (sizeOf)
                {
                    case 1:
                        minValue = min(minValue, inst->c.s8);
                        maxValue = max(maxValue, inst->c.s8);
                        break;
                    case 2:
                        minValue = min(minValue, inst->c.s16);
                        maxValue = max(maxValue, inst->c.s16);
                        break;
                    case 4:
                        minValue = min(minValue, inst->c.s32);
                        maxValue = max(maxValue, inst->c.s32);
                        break;
                    case 8:
                        minValue = min(minValue, inst->c.s64);
                        maxValue = max(maxValue, inst->c.s64);
                        break;
                    default:
                        break;
                }
            }

            // Heuristic : too much values compared to the number of cases
            // Comes from
            // TargetLoweringBase::getMinimumJumpTableEntries()
            // TargetLoweringBase::isSuitableForJumpTable in llvm

            const auto range    = maxValue - minValue + 1;
            const auto numCases = static_cast<int64_t>(context->vecInst.size());
            if (numCases < 4)
                continue;
            const bool canGen = range >= minJumpTableSize && range <= maxJumpTableSize && numCases * 100 >= range * minDensity;
            if (!canGen)
                continue;

            // Create the jump table
            // First element is always the "default" one
            uint8_t*   addrCompiler        = nullptr;
            const auto offsetTableCompiler = context->module->compilerSegment.reserve((static_cast<uint32_t>(range) + 1) * sizeof(uint32_t), &addrCompiler);
            const auto patchCompiler       = reinterpret_cast<uint32_t*>(addrCompiler);

            // Set table to default jump
            for (uint32_t i = 0; i < range + 1; i++)
                patchCompiler[i] = static_cast<uint32_t>(defaultIp - ipStart) - 1;

            // Then register each value
            for (const auto& it : context->map6432)
            {
                const int64_t v      = static_cast<int64_t>(it.first - minValue);
                patchCompiler[v + 1] = it.second;
            }

            switch (sizeOf)
            {
                case 1:
                    SET_OP(ipStart, ByteCodeOp::JumpDyn8);
                    break;
                case 2:
                    SET_OP(ipStart, ByteCodeOp::JumpDyn16);
                    break;
                case 4:
                    SET_OP(ipStart, ByteCodeOp::JumpDyn32);
                    break;
                case 8:
                    SET_OP(ipStart, ByteCodeOp::JumpDyn64);
                    break;
                default:
                    break;
            }

            ip->b.u64 = minValue;
            ip->c.u64 = range + 1;
            ip->d.u64 = offsetTableCompiler;
            break;
        }
    }
}

bool ByteCodeOptimizer::optimizePassSwitch(ByteCodeOptContext* context)
{
    optimizePassSwitchOp(context, ByteCodeOp::JumpIfEqual8, ByteCodeOp::JumpIfNotEqual8);
    optimizePassSwitchOp(context, ByteCodeOp::JumpIfEqual16, ByteCodeOp::JumpIfNotEqual16);
    optimizePassSwitchOp(context, ByteCodeOp::JumpIfEqual32, ByteCodeOp::JumpIfNotEqual32);
    optimizePassSwitchOp(context, ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfNotEqual64);
    return true;
}

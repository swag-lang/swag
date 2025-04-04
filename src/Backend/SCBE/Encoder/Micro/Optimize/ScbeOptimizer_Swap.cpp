#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

void ScbeOptimizer::optimizePassSwap(const ScbeMicro& out)
{
    ScbeMicroInstruction* toMove = nullptr;

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isRet() ||
            inst->isJump() ||
            inst->isTest())
        {
            toMove = nullptr;
        }

        if (inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadAddressM ||
            inst->op == ScbeMicroOp::LoadZeroExtendRR ||
            inst->op == ScbeMicroOp::LoadSignedExtendRR)
        {
            if (out.cc->paramsRegistersInteger.contains(inst->regA) || out.cc->paramsRegistersFloat.contains(inst->regA))
                toMove = nullptr;
            else
                toMove = inst;
        }
        else if (toMove)
        {
            auto regs = out.cpu->getReadRegisters(inst);
            if (!regs.contains(toMove->regA))
            {
                regs = out.cpu->getWriteRegisters(inst);
                if (regs.contains(toMove->regB))
                    toMove = nullptr;
                else if (regs.contains(toMove->regA))
                {
                    ignore(out, toMove);
                    toMove = nullptr;
                }
                else
                {
                    const auto next = ScbeMicro::getNextInstruction(inst);
                    if (next->op != ScbeMicroOp::SetCC)
                    {
                        swapInstruction(out, toMove, inst);
                        toMove = inst;
                    }
                    else
                        toMove = nullptr;
                }
            }
            else
                toMove = nullptr;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

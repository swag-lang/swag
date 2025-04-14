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
        if (inst->isJump() ||
            inst->isJumpDest() ||
            inst->isRet() ||
            inst->isTest())
        {
            toMove = nullptr;
        }

        if (inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadAddr ||
            inst->op == ScbeMicroOp::LoadZeroExtRR ||
            inst->op == ScbeMicroOp::LoadSignedExtRR)
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
                    if (next->op != ScbeMicroOp::SetCondR)
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

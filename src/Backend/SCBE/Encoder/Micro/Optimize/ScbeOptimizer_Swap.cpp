#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

void ScbeOptimizer::optimizePassSwap(const ScbeMicro& out)
{
    ScbeMicroInstruction* toMove = nullptr;
    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->isJump() ||
            inst->isJumpDest() ||
            inst->isRet() ||
            inst->isTest())
        {
            toMove = nullptr;
        }

        if (inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadAddrRM ||
            inst->op == ScbeMicroOp::LoadZeroExtRR ||
            inst->op == ScbeMicroOp::LoadSignedExtRR)
        {
            if (out.cpuFct->cc->paramsRegistersInteger.contains(inst->regA) || out.cpuFct->cc->paramsRegistersFloat.contains(inst->regA))
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
    }
}

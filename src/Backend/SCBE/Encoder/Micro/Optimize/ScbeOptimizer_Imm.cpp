#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

void ScbeOptimizer::optimizePassImmediate(const ScbeMicro& out)
{
    mapValInst.clear();
    
    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isRet())
            mapValInst.clear();

        if (inst->op == ScbeMicroOp::LoadMI)
        {
            const auto stackOffset = inst->getStackOffsetWrite();
            if (out.cpuFct->isStackOffsetReg(stackOffset))
                mapValInst[stackOffset] = inst;
        }
        else if (inst->op == ScbeMicroOp::LoadRM)
        {
            const auto stackOffset = inst->getStackOffsetRead();
            if (mapValInst.contains(stackOffset))
            {
                setOp(inst, ScbeMicroOp::LoadRI);
                setValueA(inst, mapValInst[stackOffset]->valueB);
            }
        }
        else
        {
            const auto stackOffset = inst->getStackOffsetWrite();
            if (out.cpuFct->isStackOffsetReg(stackOffset))
                mapValInst.erase(stackOffset);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

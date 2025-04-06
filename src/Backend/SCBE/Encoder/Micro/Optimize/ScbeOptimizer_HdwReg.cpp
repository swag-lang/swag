#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassAliasHdw(const ScbeMicro& out)
{
    mapRegInst.clear();
    mapRegInst2.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isJump() ||
            inst->isRet())
        {
            mapRegInst.clear();
            mapRegInst2.clear();
        }

        if (inst->hasReadRegB())
        {
            const auto prev = mapRegInst[inst->regB];
            if (prev && mapRegInst2[prev->regB] == prev)
            {
                if (inst->op == ScbeMicroOp::LoadRR &&
                    inst->regA == prev->regA &&
                    inst->regB == prev->regB &&
                    inst->opBitsA == prev->opBitsA)
                {
                    ignore(out, inst);
                }
                /*else if (ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(mapRegInst[inst->regB]->regB) &&
                         !inst->hasWriteRegB() &&
                         out.cpu->acceptsRegB(inst, mapRegInst[inst->regB]->regB))
                {
                    // out.print();
                    setRegB(inst, mapRegInst[inst->regB]->regB);
                    // out.print();
                }*/
            }
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            mapRegInst.erase(r);
            mapRegInst2.erase(r);
        }

        if (inst->op == ScbeMicroOp::LoadRR)
        {
            mapRegInst[inst->regA]  = inst;
            mapRegInst2[inst->regB] = inst;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

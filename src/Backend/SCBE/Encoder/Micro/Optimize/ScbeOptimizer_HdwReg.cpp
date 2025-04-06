#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassAliasHdw(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isJump() ||
            inst->isRet())
        {
            mapRegInst.clear();
        }

        if (inst->hasReadRegB())
        {
            if (mapRegInst.contains(inst->regB))
            {
                const auto prev = mapRegInst[inst->regB];
                if (inst->op == ScbeMicroOp::LoadRR &&
                    inst->regA == prev->regA &&
                    inst->regB == prev->regB &&
                    inst->opBitsA == prev->opBitsA)
                {
                    ignore(out, inst);
                }
                else if (ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(prev->regB) &&
                         inst->opBitsA == prev->opBitsA &&
                         !inst->hasWriteRegB() &&
                         out.cpu->acceptsRegB(inst, prev->regB))
                {
                    setRegB(inst, prev->regB);
                }
            }
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            mapRegInst.erase(r);

            VectorNative<CpuReg> toErase;
            for (const auto& [r1, i] : mapRegInst)
            {
                if (i->regB == r)
                    toErase.push_back(r1);
            }

            for (const auto r1 : toErase)
                mapRegInst.erase(r1);
        }

        if (inst->op == ScbeMicroOp::LoadRR)
        {
            mapRegInst[inst->regA] = inst;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

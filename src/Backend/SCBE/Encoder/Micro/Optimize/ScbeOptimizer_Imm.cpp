#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

void ScbeOptimizer::optimizePassImmediate(const ScbeMicro& out)
{
    mapValInst.clear();
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isRet())
        {
            mapValInst.clear();
            mapRegInst.clear();
        }

        if (inst->op == ScbeMicroOp::LoadMI)
        {
            const auto stackOffset = inst->getStackOffsetWrite();
            if (out.cpuFct->isStackOffsetTransient(stackOffset))
                mapValInst[stackOffset] = inst;
        }
        else if (inst->op == ScbeMicroOp::LoadRI)
        {
            mapRegInst[inst->regA] = inst;
        }
        else if (inst->op == ScbeMicroOp::LoadRM)
        {
            mapRegInst.erase(inst->regA);
            const auto stackOffset = inst->getStackOffsetRead();
            if (mapValInst.contains(stackOffset) &&
                out.cpu->encodeLoadRegImm(inst->regA, mapValInst[stackOffset]->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(inst, ScbeMicroOp::LoadRI);
                setValueA(inst, mapValInst[stackOffset]->valueB);
            }
        }
        else if (inst->op == ScbeMicroOp::LoadRR)
        {
            mapRegInst.erase(inst->regA);
            if (mapRegInst.contains(inst->regB) &&
                out.cpu->encodeLoadRegImm(inst->regA, mapRegInst[inst->regB]->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(inst, ScbeMicroOp::LoadRI);
                setValueA(inst, mapRegInst[inst->regB]->valueA);
            }
        }
        else if (inst->op == ScbeMicroOp::LoadMR)
        {
            mapValInst.erase(inst->valueA);
            if (mapRegInst.contains(inst->regB) &&
                out.cpu->encodeLoadMemImm(inst->regA, inst->valueA, mapRegInst[inst->regB]->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(inst, ScbeMicroOp::LoadMI);
                setValueB(inst, mapRegInst[inst->regB]->valueA);
            }
        }
        else
        {
            const auto stackOffset = inst->getStackOffsetWrite();
            if (out.cpuFct->isStackOffsetTransient(stackOffset))
                mapValInst.erase(stackOffset);

            const auto writeRegs = out.cpu->getWriteRegisters(inst);
            for (const auto r : writeRegs)
                mapRegInst.erase(r);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

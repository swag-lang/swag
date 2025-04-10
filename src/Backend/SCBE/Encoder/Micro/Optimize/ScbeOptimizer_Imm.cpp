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

        switch (inst->op)
        {
            case ScbeMicroOp::LoadMI:
            {
                const auto stackOffset = inst->getStackOffsetWrite();
                if (out.cpuFct->isStackOffsetTransient(stackOffset))
                    mapValInst[stackOffset] = inst;
                break;
            }

            case ScbeMicroOp::LoadRI:
                mapRegInst[inst->regA] = inst;
                break;

            case ScbeMicroOp::LoadRM:
            {
                mapRegInst.erase(inst->regA);
                const auto stackOffset = inst->getStackOffsetRead();
                if (mapValInst.contains(stackOffset) &&
                    out.cpu->encodeLoadRegImm(inst->regA, mapValInst[stackOffset]->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::LoadRI);
                    setValueA(inst, mapValInst[stackOffset]->valueB);
                }
                break;
            }

            case ScbeMicroOp::LoadRR:
                mapRegInst.erase(inst->regA);
                if (mapRegInst.contains(inst->regB) &&
                    out.cpu->encodeLoadRegImm(inst->regA, mapRegInst[inst->regB]->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::LoadRI);
                    setValueA(inst, mapRegInst[inst->regB]->valueA);
                }
                break;

            case ScbeMicroOp::CmpRR:
                if (mapRegInst.contains(inst->regB) &&
                    out.cpu->encodeCmpRegImm(inst->regA, mapRegInst[inst->regB]->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::CmpRI);
                    setValueA(inst, mapRegInst[inst->regB]->valueA);
                }
                break;

            case ScbeMicroOp::CmpMR:
                if (mapRegInst.contains(inst->regB) &&
                    out.cpu->encodeCmpMemImm(inst->regA, inst->valueA, mapRegInst[inst->regB]->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::CmpMI);
                    setValueB(inst, mapRegInst[inst->regB]->valueA);
                }
                break;

            case ScbeMicroOp::OpBinaryRR:
                mapRegInst.erase(inst->regA);
                if (mapRegInst.contains(inst->regB) &&
                    out.cpu->encodeOpBinaryRegImm(inst->regA, mapRegInst[inst->regB]->valueA, inst->cpuOp, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRI);
                    setValueA(inst, mapRegInst[inst->regB]->valueA);
                }
                break;

            case ScbeMicroOp::LoadMR:
                mapValInst.erase(inst->valueA);
                if (mapRegInst.contains(inst->regB) &&
                    out.cpu->encodeLoadMemImm(inst->regA, inst->valueA, mapRegInst[inst->regB]->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::LoadMI);
                    setValueB(inst, mapRegInst[inst->regB]->valueA);
                }
                break;

            default:
            {
                const auto stackOffset = inst->getStackOffsetWrite();
                if (out.cpuFct->isStackOffsetTransient(stackOffset))
                    mapValInst.erase(stackOffset);

                const auto writeRegs = out.cpu->getWriteRegisters(inst);
                for (const auto r : writeRegs)
                    mapRegInst.erase(r);
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"

void ScbeOptimizer::optimizePassImmediate(const ScbeMicro& out)
{
    mapValVal.clear();
    mapRegVal.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isRet())
        {
            mapValVal.clear();
            mapRegVal.clear();
        }

        switch (inst->op)
        {
            case ScbeMicroOp::LoadMI:
            {
                const auto stackOffset = inst->getStackOffsetWrite();
                if (out.cpuFct->isStackOffsetTransient(stackOffset))
                    mapValVal[stackOffset] = inst->valueB;
                break;
            }

            case ScbeMicroOp::LoadRI:
                mapRegVal[inst->regA] = inst->valueA;
                break;
            case ScbeMicroOp::ClearR:
                mapRegVal[inst->regA] = 0;
                break;

            case ScbeMicroOp::LoadRM:
            {
                mapRegVal.erase(inst->regA);
                const auto stackOffset = inst->getStackOffsetRead();
                if (mapValVal.contains(stackOffset) &&
                    out.cpu->encodeLoadRegImm(inst->regA, mapValVal[stackOffset], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::LoadRI);
                    setValueA(inst, mapValVal[stackOffset]);
                }
                break;
            }

            case ScbeMicroOp::LoadRR:
                mapRegVal.erase(inst->regA);
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeLoadRegImm(inst->regA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::LoadRI);
                    setValueA(inst, mapRegVal[inst->regB]);
                }
                break;

            case ScbeMicroOp::CmpRR:
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeCmpRegImm(inst->regA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::CmpRI);
                    setValueA(inst, mapRegVal[inst->regB]);
                }
                break;

            case ScbeMicroOp::CmpMR:
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeCmpMemImm(inst->regA, inst->valueA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::CmpMI);
                    setValueB(inst, mapRegVal[inst->regB]);
                }
                break;

            case ScbeMicroOp::OpBinaryRR:
                mapRegVal.erase(inst->regA);
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeOpBinaryRegImm(inst->regA, mapRegVal[inst->regB], inst->cpuOp, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRI);
                    setValueA(inst, mapRegVal[inst->regB]);
                }
                break;

            case ScbeMicroOp::LoadMR:
                mapValVal.erase(inst->valueA);
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeLoadMemImm(inst->regA, inst->valueA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::LoadMI);
                    setValueB(inst, mapRegVal[inst->regB]);
                }
                break;

            default:
            {
                if (inst->op == ScbeMicroOp::OpBinaryRI)
                {
                    if (mapRegVal.contains(inst->regA))
                    {
                        //out.print();
                    }
                }
                
                const auto stackOffset = inst->getStackOffsetWrite();
                if (out.cpuFct->isStackOffsetTransient(stackOffset))
                    mapValVal.erase(stackOffset);

                const auto writeRegs = out.cpu->getWriteRegisters(inst);
                for (const auto r : writeRegs)
                    mapRegVal.erase(r);
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

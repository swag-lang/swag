#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Wmf/SourceFile.h"

namespace
{
    void handleConstantCmp(ScbeOptimizer& opt, const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* const next)
    {
        auto value    = opt.mapRegVal[inst->regA];
        auto curValue = inst->valueA;
        BackendEncoder::maskValue(value, inst->opBitsA);
        BackendEncoder::maskValue(curValue, inst->opBitsA);

        bool alwaysFalse = false;
        bool alwaysTrue  = false;

        switch (next->jumpType)
        {
            case CpuCondJump::JZ:
                alwaysTrue  = value == curValue;
                alwaysFalse = value != curValue;
                break;
            case CpuCondJump::JNZ:
                alwaysTrue  = value != curValue;
                alwaysFalse = value == curValue;
                break;
            case CpuCondJump::JGE:
                alwaysTrue  = value >= curValue;
                alwaysFalse = value < curValue;
                break;
        }

        if (alwaysFalse)
        {
            opt.ignore(out, inst);
            opt.ignore(out, next);
        }
        else if (alwaysTrue)
        {
            next->jumpType = CpuCondJump::JUMP;
            opt.setDirtyPass();
        }
    }
}

void ScbeOptimizer::optimizePassImmediate(const ScbeMicro& out)
{
    mapValVal.clear();
    mapRegVal.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->isJump() || inst->isJumpDest() || inst->isRet())
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
                    setOp(out, inst, ScbeMicroOp::LoadRI);
                    setValueA(out, inst, mapValVal[stackOffset]);
                    break;
                }
                break;
            }

            case ScbeMicroOp::LoadRR:
            case ScbeMicroOp::LoadZeroExtRR:
                mapRegVal.erase(inst->regA);
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeLoadRegImm(inst->regA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(out, inst, ScbeMicroOp::LoadRI);
                    setValueA(out, inst, mapRegVal[inst->regB]);
                    break;
                }
                break;

            case ScbeMicroOp::LoadSignedExtRR:
            {
                mapRegVal.erase(inst->regA);
                if (mapRegVal.contains(inst->regB))
                {
                    auto signedValue = mapRegVal[inst->regB];
                    switch (inst->opBitsB)
                    {
                        case OpBits::B8:
                            signedValue = static_cast<int64_t>(static_cast<int8_t>(signedValue));
                            break;
                        case OpBits::B16:
                            signedValue = static_cast<int64_t>(static_cast<int16_t>(signedValue));
                            break;
                        case OpBits::B32:
                            signedValue = static_cast<int64_t>(static_cast<int32_t>(signedValue));
                            break;
                    }

                    if (out.cpu->encodeLoadRegImm(inst->regA, signedValue, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setOp(out, inst, ScbeMicroOp::LoadRI);
                        setValueA(out, inst, signedValue);
                        break;
                    }
                }
                break;
            }

            case ScbeMicroOp::CmpRI:
                if (mapRegVal.contains(inst->regA))
                {
                    const auto next = ScbeMicro::getNextInstruction(inst);
                    if (next->isJumpCond() && !next->isJumpDest())
                    {
                        handleConstantCmp(*this, out, inst, next);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::CmpRR:
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeCmpRegImm(inst->regA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(out, inst, ScbeMicroOp::CmpRI);
                    setValueA(out, inst, mapRegVal[inst->regB]);
                    break;
                }
                break;

            case ScbeMicroOp::CmpMR:
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeCmpMemImm(inst->regA, inst->valueA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(out, inst, ScbeMicroOp::CmpMI);
                    setValueB(out, inst, mapRegVal[inst->regB]);
                    break;
                }
                break;

            case ScbeMicroOp::OpBinaryRR:
                mapRegVal.erase(inst->regA);
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeOpBinaryRegImm(inst->regA, mapRegVal[inst->regB], inst->cpuOp, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(out, inst, ScbeMicroOp::OpBinaryRI);
                    setValueA(out, inst, mapRegVal[inst->regB]);
                    break;
                }
                break;

            case ScbeMicroOp::LoadMR:
                mapValVal.erase(inst->valueA);
                if (mapRegVal.contains(inst->regB) &&
                    out.cpu->encodeLoadMemImm(inst->regA, inst->valueA, mapRegVal[inst->regB], inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(out, inst, ScbeMicroOp::LoadMI);
                    setValueB(out, inst, mapRegVal[inst->regB]);
                    break;
                }
                break;

            default:
            {
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

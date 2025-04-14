#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassDeadRegBeforeLeave(const ScbeMicro& out)
{
    mapValInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->isJump() || inst->isJumpDest())
            mapValInst.clear();

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapValInst | std::views::values)
                ignore(out, i);
            mapValInst.clear();
        }
        else
        {
            const auto stackOffset = inst->getStackOffset();
            if (out.cpuFct->isStackOffsetTransient(stackOffset))
            {
                if (aliasStack.contains(stackOffset, sizeof(uint8_t)))
                    mapValInst.erase(stackOffset);
                else if (inst->op == ScbeMicroOp::LoadMR || inst->op == ScbeMicroOp::LoadMI)
                    mapValInst[stackOffset] = inst;
                else
                    mapValInst.erase(stackOffset);
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::optimizePassStore(const ScbeMicro& out)
{
    mapValReg.clear();
    mapRegVal.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->isJump() || inst->isJumpDest() || inst->isRet())
        {
            mapValReg.clear();
            mapRegVal.clear();
        }

        if (inst->isCall())
        {
            mapValReg.erase(out.cpuFct->getStackOffsetRT(0));
            mapValReg.erase(out.cpuFct->getStackOffsetRT(1));
        }

        const auto stackOffset = inst->getStackOffset();
        auto       legitReg    = CpuReg::Max;
        const auto isStack     = out.cpuFct->isStackOffsetTransient(stackOffset) || out.cpuFct->isStackOffsetBC(stackOffset);

        if (inst->op == ScbeMicroOp::LoadMR && isStack)
        {
            if (mapValReg.contains(stackOffset))
                mapRegVal.erase(mapValReg[stackOffset].first);

            if (!aliasStack.contains(stackOffset, ScbeCpu::getNumBytes(inst->opBitsA)))
            {
                mapValReg[stackOffset] = {inst->regB, inst->opBitsA};
                mapRegVal[inst->regB]  = stackOffset;
            }
        }
        else if (inst->hasWriteMemA())
        {
            mapValReg.erase(stackOffset);
        }

        switch (inst->op)
        {
            case ScbeMicroOp::LoadAddr:
                if (inst->regB == CpuReg::Rsp && isStack)
                    mapValReg.erase(stackOffset);
                break;

            case ScbeMicroOp::LoadRM:
                if (mapValReg.contains(stackOffset) &&
                    mapRegVal.contains(mapValReg[stackOffset].first) &&
                    ScbeCpu::isInt(inst->regA) == ScbeCpu::isInt(mapValReg[stackOffset].first) &&
                    ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[stackOffset].second) &&
                    mapRegVal[mapValReg[stackOffset].first] == stackOffset)
                {
                    if (mapValReg[stackOffset].first == inst->regA)
                    {
                        ignore(out, inst);
                    }
                    else
                    {
                        setOp(inst, ScbeMicroOp::LoadRR);
                        setRegB(inst, mapValReg[stackOffset].first);
                    }
                    break;
                }

                if (isStack)
                {
                    if (mapRegVal.contains(inst->regA))
                        mapValReg.erase(mapRegVal[inst->regA]);
                    legitReg               = inst->regA;
                    mapValReg[stackOffset] = {inst->regA, inst->opBitsA};
                    mapRegVal[inst->regA]  = stackOffset;
                    break;
                }

                break;

            case ScbeMicroOp::CmpMR:
                if (mapValReg.contains(stackOffset) &&
                    mapRegVal[mapValReg[stackOffset].first] == stackOffset &&
                    ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(mapValReg[stackOffset].first) &&
                    ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[stackOffset].second))
                {
                    setOp(inst, ScbeMicroOp::CmpRR);
                    setRegA(inst, mapValReg[stackOffset].first);
                    break;
                }

                break;

            case ScbeMicroOp::CmpMI:
                if (mapValReg.contains(stackOffset) &&
                    mapRegVal[mapValReg[stackOffset].first] == stackOffset &&
                    ScbeCpu::isInt(mapValReg[stackOffset].first) &&
                    ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[stackOffset].second) &&
                    out.cpu->encodeCmpRegImm(mapValReg[stackOffset].first, inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                {
                    setOp(inst, ScbeMicroOp::CmpRI);
                    setRegA(inst, mapValReg[stackOffset].first);
                    setValueA(inst, inst->valueB);
                    break;
                }

                break;
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (auto r : writeRegs)
        {
            if (r != legitReg)
                mapRegVal.erase(r);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

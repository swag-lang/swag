#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

void ScbeOptimizer::optimizePassDeadRegBeforeLeave(const ScbeMicro& out)
{
    mapValInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST))
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
                if (inst->op == ScbeMicroOp::LoadMR || inst->op == ScbeMicroOp::LoadMI)
                    mapValInst[stackOffset] = inst;
                else
                    mapValInst.erase(stackOffset);
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::optimizePassDeadHdwRegBeforeLeave(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isCall())
            mapRegInst.clear();

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapRegInst | std::views::values)
                ignore(out, i);
            mapRegInst.clear();
        }
        else if (inst->op == ScbeMicroOp::LoadRR)
        {
            if (!out.cpuFct->typeFunc->returnByValue() &&
                !out.cpuFct->typeFunc->returnStructByValue())
            {
                mapRegInst[inst->regA] = inst;
            }
            else if (inst->regA != out.cc->returnByRegisterInteger &&
                     inst->regA != out.cc->returnByRegisterFloat)
            {
                mapRegInst[inst->regA] = inst;
            }

            mapRegInst.erase(inst->regB);
        }
        else
        {
            const auto readRegs = out.cpu->getReadRegisters(inst);
            for (const auto r : readRegs)
                mapRegInst.erase(r);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::optimizePassDeadStore(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isRet())
            mapRegInst.clear();

        const auto readRegs = out.cpu->getReadRegisters(inst);
        for (const auto r : readRegs)
            mapRegInst.erase(r);

        auto legitReg = CpuReg::Max;
        if (inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadRI ||
            inst->op == ScbeMicroOp::LoadZeroExtendRM ||
            inst->op == ScbeMicroOp::LoadSignedExtendRM ||
            inst->op == ScbeMicroOp::LoadRM)
        {
            if (mapRegInst.contains(inst->regA))
                ignore(out, mapRegInst[inst->regA]);
            mapRegInst[inst->regA] = inst;
            legitReg               = inst->regA;
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            if (r != legitReg)
                mapRegInst.erase(r);
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
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isRet())
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

        if (inst->op == ScbeMicroOp::LoadMR && out.cpuFct->isStackOffsetTransient(stackOffset))
        {
            if (mapValReg.contains(stackOffset))
                mapRegVal.erase(mapValReg[stackOffset].first);
            mapValReg[stackOffset] = {inst->regB, inst->opBitsA};
            mapRegVal[inst->regB]  = stackOffset;
        }
        else if (inst->hasWriteMemA() && mapValReg.contains(stackOffset))
        {
            mapValReg.erase(stackOffset);
        }

        switch (inst->op)
        {
            case ScbeMicroOp::LoadAddressM:
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

                if (out.cpuFct->isStackOffsetTransient(stackOffset))
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
                    ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[stackOffset].second))
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

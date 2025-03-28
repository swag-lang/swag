#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

void ScbeOptimizer::optimizePassStoreToRegBeforeLeave(const ScbeMicro& out)
{
    mapValInst.clear();

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isJump())
        {
            mapValInst.clear();
        }

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapValInst | std::views::values)
                ignore(i);
            mapValInst.clear();
        }
        else if (inst->op == ScbeMicroOp::LoadMR &&
                 inst->regA == CpuReg::Rsp &&
                 out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            mapValInst[inst->valueA] = inst;
        }
        else
        {
            if (inst->hasReadMemA())
                mapValInst.erase(static_cast<uint32_t>(inst->valueA));
            if (inst->hasReadMemB())
                mapValInst.erase(static_cast<uint32_t>(inst->valueB));
        }

        inst = zap(inst + 1);
    }
}

void ScbeOptimizer::optimizePassStoreToHdwRegBeforeLeave(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isJump() ||
            inst->isCall() ||
            inst->isRet())
        {
            mapRegInst.clear();
        }

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapRegInst | std::views::values)
                ignore(i);
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

            if (inst->hasReadRegB())
                mapRegInst.erase(inst->regB);
            if (inst->hasReadRegC())
                mapRegInst.erase(inst->regC);
        }
        else
        {
            if (inst->hasReadRegA())
                mapRegInst.erase(inst->regA);
            if (inst->hasReadRegB())
                mapRegInst.erase(inst->regB);
            if (inst->hasReadRegC())
                mapRegInst.erase(inst->regC);
        }

        inst = zap(inst + 1);
    }
}

void ScbeOptimizer::optimizePassDeadStore(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isJump() ||
            inst->isRet())
        {
            mapRegInst.clear();
        }

        if (inst->hasReadRegA())
            mapRegInst.erase(inst->regA);
        if (inst->hasReadRegB())
            mapRegInst.erase(inst->regB);
        if (inst->hasReadRegC())
            mapRegInst.erase(inst->regC);

        auto legitReg = CpuReg::Max;
        if (inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadRI ||
            inst->op == ScbeMicroOp::LoadZeroExtendRM ||
            inst->op == ScbeMicroOp::LoadSignedExtendRM ||
            inst->op == ScbeMicroOp::LoadRM)
        {
            if (mapRegInst.contains(inst->regA))
            {
                ignore(mapRegInst[inst->regA]);
            }

            mapRegInst[inst->regA] = inst;
            legitReg               = inst->regA;
        }

        auto details = out.cpu->getWriteRegisters(inst);
        for (const auto r : details)
        {
            if (r != legitReg)
                mapRegInst.erase(r);
        }

        inst = zap(inst + 1);
    }
}

void ScbeOptimizer::optimizePassStore(const ScbeMicro& out)
{
    mapValReg.clear();
    mapRegVal.clear();

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isJump() ||
            inst->isRet())
        {
            mapValReg.clear();
            mapRegVal.clear();
        }

        auto legitReg = CpuReg::Max;
        if (inst->op == ScbeMicroOp::LoadMR &&
            inst->regA == CpuReg::Rsp &&
            out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            if (mapValReg.contains(inst->valueA))
                mapRegVal.erase(mapValReg[inst->valueA].first);
            mapValReg[inst->valueA] = {inst->regB, inst->opBitsA};
            mapRegVal[inst->regB]   = inst->valueA;
        }
        else if (inst->hasWriteMemA() &&
                 inst->regA == CpuReg::Rsp &&
                 mapValReg.contains(inst->valueA))
        {
            mapValReg.erase(inst->valueA);
        }

        switch (inst->op)
        {
            case ScbeMicroOp::LoadAddressM:
                if (inst->regB == CpuReg::Rsp)
                    mapValReg.erase(inst->valueB);
                break;

            case ScbeMicroOp::LoadRM:
                if (inst->regB == CpuReg::Rsp &&
                    mapValReg.contains(inst->valueA) &&
                    mapRegVal.contains(mapValReg[inst->valueA].first) &&
                    ScbeCpu::isInt(inst->regA) == ScbeCpu::isInt(mapValReg[inst->valueA].first) &&
                    ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[inst->valueA].second) &&
                    mapRegVal[mapValReg[inst->valueA].first] == inst->valueA)
                {
                    if (mapValReg[inst->valueA].first == inst->regA)
                    {
                        ignore(inst);
                    }
                    else
                    {
                        setOp(inst, ScbeMicroOp::LoadRR);
                        inst->regB = mapValReg[inst->valueA].first;
                    }
                    break;
                }

                if (inst->regB == CpuReg::Rsp &&
                    out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
                {
                    if (mapRegVal.contains(inst->regA))
                        mapValReg.erase(mapRegVal[inst->regA]);
                    legitReg                = inst->regA;
                    mapValReg[inst->valueA] = {inst->regA, inst->opBitsA};
                    mapRegVal[inst->regA]   = inst->valueA;
                    break;
                }

                break;

            case ScbeMicroOp::CmpMR:
                if (inst->regA == CpuReg::Rsp &&
                    mapValReg.contains(inst->valueA) &&
                    mapRegVal[mapValReg[inst->valueA].first] == inst->valueA &&
                    ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(mapValReg[inst->valueA].first) &&
                    ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[inst->valueA].second))
                {
                    setOp(inst, ScbeMicroOp::CmpRR);
                    inst->regA = mapValReg[inst->valueA].first;
                    break;
                }

                break;

            case ScbeMicroOp::CmpMI:
                if (inst->regA == CpuReg::Rsp &&
                    mapValReg.contains(inst->valueA) &&
                    mapRegVal[mapValReg[inst->valueA].first] == inst->valueA &&
                    ScbeCpu::isInt(mapValReg[inst->valueA].first) &&
                    ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[inst->valueA].second))
                {
                    setOp(inst, ScbeMicroOp::CmpRI);
                    inst->regA   = mapValReg[inst->valueA].first;
                    inst->valueA = inst->valueB;
                    break;
                }

                break;
        }

        auto details = out.cpu->getWriteRegisters(inst);
        for (auto r : details)
        {
            if (r != legitReg)
                mapRegVal.erase(r);
        }

        inst = zap(inst + 1);
    }
}

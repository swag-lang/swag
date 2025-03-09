#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Main/Statistics.h"
#include "ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#pragma optimize("", off)

void ScbeOptimizer::ignore(ScbeMicroInstruction* inst)
{
#ifdef SWAG_STATS
    g_Stats.totalOptimScbe += 1;
#endif
    inst->op = ScbeMicroOp::Ignore;
    setDirtyPass();
}

void ScbeOptimizer::setOp(ScbeMicroInstruction* inst, ScbeMicroOp op)
{
    inst->op = op;
    setDirtyPass();
}

ScbeMicroInstruction* ScbeOptimizer::zap(ScbeMicroInstruction* inst)
{
    while (inst->op == ScbeMicroOp::Nop || inst->op == ScbeMicroOp::Label || inst->op == ScbeMicroOp::Debug || inst->op == ScbeMicroOp::Ignore)
        inst++;
    return inst;
}

void ScbeOptimizer::optimizePassReduce(const ScbeMicro& out)
{
    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        const auto next = zap(inst + 1);
        if (next->flags.has(MIF_JUMP_DEST))
        {
            inst = zap(next);
            continue;
        }

        switch (inst[0].op)
        {
            case ScbeMicroOp::LoadRR:
                if (next->hasReadRegA() &&
                    inst->regA == next->regA &&
                    inst->opBitsA == OpBits::B64)
                {
                    const auto details = encoder->getInstructionDetails(next);
                    if (!details.has(1ULL << static_cast<uint32_t>(inst->regB)))
                    {
                        next->regA = inst->regB;
                        setDirtyPass();
                        break;
                    }
                }

                if (next->hasReadRegB() &&
                    inst->regA == next->regB &&
                    inst->opBitsA == OpBits::B64)
                {
                    const auto details = encoder->getInstructionDetails(next);
                    if (!details.has(1ULL << static_cast<uint32_t>(inst->regB)))
                    {
                        next->regB = inst->regB;
                        setDirtyPass();
                        break;
                    }
                }

                if (next->op == ScbeMicroOp::LoadRR &&
                    inst->regA == next->regB &&
                    inst->regB == next->regA &&
                    inst->opBitsA == next->opBitsA &&
                    inst->opBitsA == OpBits::B64)
                {
                    ignore(inst);
                    ignore(next);
                    break;
                }
                break;

            case ScbeMicroOp::StoreMR:
                if (next->op == ScbeMicroOp::LoadRM &&
                    inst[0].opBitsA == next->opBitsA &&
                    inst[0].regA == next->regB &&
                    inst[0].valueA == next->valueA)
                {
                    if (inst[0].regB == next->regA)
                    {
                        ignore(next);
                        break;
                    }

                    if (inst[0].opBitsA == OpBits::B64)
                    {
                        setOp(next, ScbeMicroOp::LoadRR);
                        next->regB = inst[0].regB;
                        break;
                    }
                }

                if (inst[0].regA == CpuReg::RSP &&
                    next->op == ScbeMicroOp::Leave)
                {
                    ignore(inst);
                    break;
                }
                break;
        }

        inst = zap(next);
    }
}

void ScbeOptimizer::optimizePassStoreToRegBeforeLeave(const ScbeMicro& out)
{
    mapValInst.clear();

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump())
            mapValInst.clear();

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapValInst | std::views::values)
                ignore(i);
            mapValInst.clear();
        }
        else if (inst->op == ScbeMicroOp::StoreMR &&
                 inst->regA == CpuReg::RSP &&
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
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump() || inst->isCall())
            mapRegInst.clear();

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapRegInst | std::views::values)
                ignore(i);
            mapRegInst.clear();
        }
        else if (inst->hasWriteRegA())
        {
            if (!out.cpuFct->typeFunc->returnByValue() &&
                !out.cpuFct->typeFunc->returnStructByValue())
            {
                mapRegInst[inst->regA] = inst;
            }
            else if (inst->regA != out.cpuFct->cc->returnByRegisterInteger &&
                     inst->regA != out.cpuFct->cc->returnByRegisterFloat)
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
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump())
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
            inst->op == ScbeMicroOp::LoadZeroExtendRM ||
            inst->op == ScbeMicroOp::LoadRM)
        {
            if (mapRegInst.contains(inst->regA))
            {
                ignore(mapRegInst[inst->regA]);
            }

            mapRegInst[inst->regA] = inst;
            legitReg               = inst->regA;
        }
        else
        {
            if (inst->hasWriteRegA())
                mapRegInst.erase(inst->regA);
            if (inst->hasWriteRegB())
                mapRegInst.erase(inst->regB);
            if (inst->hasWriteRegC())
                mapRegInst.erase(inst->regC);
        }

        const auto details = encoder->getInstructionDetails(inst);
        if (details.has(MOD_REG_ALL))
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(CpuReg::Max); i++)
            {
                if (legitReg == static_cast<CpuReg>(i))
                    continue;

                if (details.has(1ULL << i))
                {
                    mapRegInst.erase(static_cast<CpuReg>(i));
                }
            }
        }

        inst = zap(inst + 1);
    }
}

void ScbeOptimizer::optimizePassStoreMR(const ScbeMicro& out)
{
    mapValReg.clear();
    mapRegVal.clear();

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump())
        {
            mapValReg.clear();
            mapRegVal.clear();
        }

        auto legitReg = CpuReg::Max;
        if (inst->op == ScbeMicroOp::StoreMR &&
            inst->regA == CpuReg::RSP &&
            out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            mapValReg[inst->valueA] = {inst->regB, inst->opBitsA};
            mapRegVal[inst->regB]   = inst->valueA;
        }
        else if (inst->hasWriteMemA() &&
                 inst->regA == CpuReg::RSP &&
                 mapValReg.contains(inst->valueA))
        {
            mapValReg.erase(inst->valueA);
        }
        else if (inst->op == ScbeMicroOp::LoadRM &&
                 inst->regB == CpuReg::RSP &&
                 mapValReg.contains(inst->valueA) &&
                 mapRegVal.contains(mapValReg[inst->valueA].first) &&
                 inst->opBitsA == mapValReg[inst->valueA].second &&
                 mapRegVal[mapValReg[inst->valueA].first] == inst->valueA)
        {
            if (mapValReg[inst->valueA].first == inst->regA)
            {
                ignore(inst);
            }
            else if (inst->opBitsA == OpBits::B64)
            {
                setOp(inst, ScbeMicroOp::LoadRR);
                inst->regB = mapValReg[inst->valueA].first;
            }
        }
        else if (inst->op == ScbeMicroOp::LoadRM &&
                 inst->regB == CpuReg::RSP &&
                 out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            legitReg                = inst->regA;
            mapValReg[inst->valueA] = {inst->regA, inst->opBitsA};
            mapRegVal[inst->regA]   = inst->valueA;
        }

        if (inst->op != ScbeMicroOp::Ignore)
        {
            const auto details = encoder->getInstructionDetails(inst);
            if (details.has(MOD_REG_ALL))
            {
                for (uint32_t i = 0; i < static_cast<uint32_t>(CpuReg::Max); i++)
                {
                    if (static_cast<CpuReg>(i) == legitReg)
                        continue;

                    if (details.has(1ULL << i))
                    {
                        mapRegVal.erase(static_cast<CpuReg>(i));
                    }
                }
            }
        }

        inst = zap(inst + 1);
    }
}

void ScbeOptimizer::optimize(const ScbeMicro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;

    setDirtyPass();
    while (passHasDoneSomething)
    {
        passHasDoneSomething = false;
        optimizePassReduce(out);
        optimizePassStoreMR(out);
        optimizePassDeadStore(out);
        optimizePassStoreToRegBeforeLeave(out);
        optimizePassStoreToHdwRegBeforeLeave(out);
    }
}

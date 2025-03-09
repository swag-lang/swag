#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/SCBEOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/SCBEMicro.h"
#include "Main/Statistics.h"
#include "Semantic/Type/TypeInfo.h"
#pragma optimize("", off)

void SCBEOptimizer::ignore(SCBEMicroInstruction* inst)
{
#ifdef SWAG_STATS
    g_Stats.totalOptimScbe += 1;
#endif
    inst->op             = SCBEMicroOp::Ignore;
    passHasDoneSomething = true;
}

void SCBEOptimizer::setOp(SCBEMicroInstruction* inst, SCBEMicroOp op)
{
    inst->op             = op;
    passHasDoneSomething = true;
}

SCBEMicroInstruction* SCBEOptimizer::zap(SCBEMicroInstruction* inst)
{
    while (inst->op == SCBEMicroOp::Nop || inst->op == SCBEMicroOp::Label || inst->op == SCBEMicroOp::Debug || inst->op == SCBEMicroOp::Ignore)
        inst++;
    return inst;
}

void SCBEOptimizer::passReduce(const SCBEMicro& out)
{
    auto inst = reinterpret_cast<SCBEMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBEMicroOp::End)
    {
        const auto next = zap(inst + 1);
        if (next->flags.has(MIF_JUMP_DEST))
        {
            inst = zap(next);
            continue;
        }

        const auto& nextInfos = g_MicroOpInfos[static_cast<int>(next->op)];

        switch (inst[0].op)
        {
            case SCBEMicroOp::LoadRR:
                if (nextInfos.leftFlags.has(MOF_REG_A) &&
                    nextInfos.leftFlags.has(MOF_VALUE_A) &&
                    !nextInfos.leftFlags.has(MOF_REG_B) &&
                    inst->regA == next->regA &&
                    inst->opBitsA == OpBits::B64)
                {
                    const auto details = encoder->getInstructionDetails(next);
                    if (!details.has(1ULL << static_cast<uint32_t>(inst->regB)))
                    {
                        next->regA           = inst->regB;
                        passHasDoneSomething = true;
                        break;
                    }
                }

                if (nextInfos.rightFlags.has(MOF_REG_B) &&
                    inst->regA == next->regB &&
                    nextInfos.rightFlags.has(MOF_OPBITS_B) &&
                    next->opBitsB == OpBits::B64)
                {
                    const auto details = encoder->getInstructionDetails(next);
                    if (!details.has(1ULL << static_cast<uint32_t>(inst->regB)))
                    {
                        next->regB           = inst->regB;
                        passHasDoneSomething = true;
                        break;
                    }
                }

                if (next->op == SCBEMicroOp::LoadRR &&
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

            case SCBEMicroOp::StoreMR:
                if (next->op == SCBEMicroOp::LoadRM &&
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
                        setOp(next, SCBEMicroOp::LoadRR);
                        next->regB = inst[0].regB;
                        break;
                    }
                }

                if (inst[0].regA == CPUReg::RSP &&
                    next->op == SCBEMicroOp::Leave)
                {
                    ignore(inst);
                    break;
                }
                break;
        }

        inst = zap(next);
    }
}

void SCBEOptimizer::passStoreToRegBeforeLeave(const SCBEMicro& out)
{
    mapValInst.clear();

    auto inst = reinterpret_cast<SCBEMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBEMicroOp::End)
    {
        const auto& infos = g_MicroOpInfos[static_cast<int>(inst->op)];

        if (inst->flags.has(MIF_JUMP_DEST))
        {
            mapValInst.clear();
        }

        if (inst->op == SCBEMicroOp::StoreMR &&
            inst->regA == CPUReg::RSP &&
            out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            mapValInst[inst->valueA] = inst;
        }
        else
        {
            if (infos.leftFlags.has(MOF_REG_A | MOF_REG_B) && infos.leftFlags.has(MOF_VALUE_A))
                mapValInst.erase(static_cast<uint32_t>(inst->valueA));
            if (infos.leftFlags.has(MOF_REG_A | MOF_REG_B) && infos.leftFlags.has(MOF_VALUE_B))
                mapValInst.erase(static_cast<uint32_t>(inst->valueB));
            if (infos.rightFlags.has(MOF_REG_A | MOF_REG_B) && infos.rightFlags.has(MOF_VALUE_A))
                mapValInst.erase(static_cast<uint32_t>(inst->valueA));
            if (infos.rightFlags.has(MOF_REG_A | MOF_REG_B) && infos.rightFlags.has(MOF_VALUE_B))
                mapValInst.erase(static_cast<uint32_t>(inst->valueB));
        }

        if (inst->op == SCBEMicroOp::Leave && !mapValInst.empty())
        {
            for (const auto& i : mapValInst | std::views::values)
                ignore(i);
            mapValInst.clear();
        }

        inst = zap(inst + 1);
    }
}

void SCBEOptimizer::passStoreToHdwRegBeforeLeave(const SCBEMicro& out)
{
    mapValInst.clear();

    auto inst = reinterpret_cast<SCBEMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBEMicroOp::End)
    {
        const auto& infos = g_MicroOpInfos[static_cast<int>(inst->op)];

        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump() || inst->isCall())
        {
            mapValInst.clear();
        }

        if (inst->op == SCBEMicroOp::LoadRR)
        {
            if (!out.cpuFct->typeFunc->returnByValue() && !out.cpuFct->typeFunc->returnStructByValue())
            {
                mapValInst[static_cast<uint32_t>(inst->regA)] = inst;
            }
            else if (inst->regA != out.cpuFct->cc->returnByRegisterInteger &&
                     inst->regA != out.cpuFct->cc->returnByRegisterFloat)
            {
                mapValInst[static_cast<uint32_t>(inst->regA)] = inst;
            }

            mapValInst.erase(static_cast<uint32_t>(inst->regB));
        }
        else
        {
            if (infos.leftFlags.has(MOF_REG_A) || infos.rightFlags.has(MOF_REG_A))
                mapValInst.erase(static_cast<uint32_t>(inst->regA));
            if (infos.leftFlags.has(MOF_REG_B) || infos.rightFlags.has(MOF_REG_B))
                mapValInst.erase(static_cast<uint32_t>(inst->regB));
        }

        if (inst->op == SCBEMicroOp::Leave && !mapValInst.empty())
        {
            for (const auto& i : mapValInst | std::views::values)
                ignore(i);
            mapValInst.clear();
        }

        inst = zap(inst + 1);
    }
}

void SCBEOptimizer::passDeadStore(const SCBEMicro& out)
{
    mapRegInst.clear();

    auto inst = reinterpret_cast<SCBEMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBEMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump())
        {
            mapRegInst.clear();
        }

        const auto& infos = g_MicroOpInfos[static_cast<int>(inst->op)];

        if (infos.rightFlags.has(MOF_REG_A))
            mapRegInst.erase(inst->regA);
        if (infos.rightFlags.has(MOF_REG_B))
            mapRegInst.erase(inst->regB);

        CPUReg legitReg = CPUReg::Max;
        if (inst->op == SCBEMicroOp::LoadRR ||
            inst->op == SCBEMicroOp::LoadZeroExtendRM ||
            inst->op == SCBEMicroOp::LoadRM)
        {
            if (mapRegInst.contains(inst->regA))
            {
                ignore(mapRegInst[inst->regA]);
            }

            mapRegInst[inst->regA] = inst;
            legitReg = inst->regA;
        }
        else
        {
            if (infos.leftFlags.has(MOF_REG_A))
                mapRegInst.erase(inst->regA);
            if (infos.leftFlags.has(MOF_REG_B))
                mapRegInst.erase(inst->regB);
        }

        const auto details = encoder->getInstructionDetails(inst);
        if (details.has(MOD_REG_ALL))
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(CPUReg::Max); i++)
            {
                if (legitReg == static_cast<CPUReg>(i))
                    continue;
                
                if (details.has(1ULL << i))
                {
                    mapRegInst.erase(static_cast<CPUReg>(i));
                }
            }
        }

        inst = zap(inst + 1);
    }
}

void SCBEOptimizer::passStoreMR(const SCBEMicro& out)
{
    mapValReg.clear();
    mapRegVal.clear();

    auto inst = reinterpret_cast<SCBEMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBEMicroOp::End)
    {
        const auto& infos = g_MicroOpInfos[static_cast<int>(inst->op)];

        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump())
        {
            mapValReg.clear();
            mapRegVal.clear();
        }

        auto legitReg = CPUReg::Max;
        if (inst->op == SCBEMicroOp::StoreMR &&
            inst->regA == CPUReg::RSP &&
            out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            mapValReg[inst->valueA] = {inst->regB, inst->opBitsA};
            mapRegVal[inst->regB]   = inst->valueA;
        }
        else if (infos.leftFlags.has(MOF_REG_A) &&
                 infos.leftFlags.has(MOF_VALUE_A) &&
                 inst->regA == CPUReg::RSP &&
                 mapValReg.contains(inst->valueA))
        {
            mapValReg[inst->valueA] = {CPUReg::Max, OpBits::Zero};
        }
        else if (inst->op == SCBEMicroOp::LoadRM &&
                 inst->regB == CPUReg::RSP &&
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
                setOp(inst, SCBEMicroOp::LoadRR);
                inst->regB = mapValReg[inst->valueA].first;
            }
        }
        else if (inst->op == SCBEMicroOp::LoadRM &&
                 inst->regB == CPUReg::RSP &&
                 out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            legitReg                = inst->regA;
            mapValReg[inst->valueA] = {inst->regA, inst->opBitsA};
            mapRegVal[inst->regA]   = inst->valueA;
        }

        if (inst->op != SCBEMicroOp::Ignore)
        {
            const auto details = encoder->getInstructionDetails(inst);
            if (details.has(MOD_REG_ALL))
            {
                for (uint32_t i = 0; i < static_cast<uint32_t>(CPUReg::Max); i++)
                {
                    if (static_cast<CPUReg>(i) == legitReg)
                        continue;

                    if (details.has(1ULL << i))
                    {
                        mapRegVal[static_cast<CPUReg>(i)] = UINT64_MAX;
                    }
                }
            }
        }

        inst = zap(inst + 1);
    }
}

void SCBEOptimizer::optimize(const SCBEMicro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;

    passHasDoneSomething = true;
    while (passHasDoneSomething)
    {
        passHasDoneSomething = false;
        passReduce(out);
        passStoreMR(out);
        passDeadStore(out);
        passStoreToRegBeforeLeave(out);
        passStoreToHdwRegBeforeLeave(out);
    }
}

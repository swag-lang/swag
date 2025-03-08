#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/SCBE_Optimizer.h"
#include "Backend/SCBE/Encoder/Micro/SCBE_Micro.h"
#include "Main/Statistics.h"
#pragma optimize("", off)

void SCBE_Optimizer::ignore(SCBE_MicroInstruction* inst)
{
#ifdef SWAG_STATS
    g_Stats.totalOptimScbe += 1;
#endif
    inst->op             = SCBE_MicroOp::Ignore;
    passHasDoneSomething = true;
}

void SCBE_Optimizer::setOp(SCBE_MicroInstruction* inst, SCBE_MicroOp op)
{
    inst->op             = op;
    passHasDoneSomething = true;
}

SCBE_MicroInstruction* SCBE_Optimizer::zap(SCBE_MicroInstruction* inst)
{
    while (inst->op == SCBE_MicroOp::Nop || inst->op == SCBE_MicroOp::Label || inst->op == SCBE_MicroOp::Debug || inst->op == SCBE_MicroOp::Ignore)
        inst++;
    return inst;
}

void SCBE_Optimizer::passReduce(const SCBE_Micro& out)
{
    auto inst = reinterpret_cast<SCBE_MicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBE_MicroOp::End)
    {
        const auto next = zap(inst + 1);

        switch (inst[0].op)
        {
            case SCBE_MicroOp::StoreMR:
                if (next->op == SCBE_MicroOp::LoadRM &&
                    !next->flags.has(MIF_JUMP_DEST) &&
                    inst[0].opBitsA == next->opBitsA &&
                    inst[0].regA == next->regB &&
                    inst[0].valueA == next->valueA)
                {
                    if (inst[0].regB == next->regA)
                    {
                        ignore(next);
                    }
                    else if (inst[0].opBitsA == OpBits::B64)
                    {
                        setOp(next, SCBE_MicroOp::LoadRR);
                        next->regB = inst[0].regB;
                    }
                    break;
                }

                if (inst[0].regA == CPUReg::RSP &&
                    next->op == SCBE_MicroOp::Leave)
                {
                    ignore(inst);
                    break;
                }
                break;
        }

        inst = zap(next);
    }
}

void SCBE_Optimizer::passStoreMR(const SCBE_Micro& out)
{
    /*if (!out.cpuFct->bc->getPrintName().containsNoCase(".BuildCfg.opInitGenerated"))
        return;
    out.print();*/

    mapValReg.clear();
    mapRegVal.clear();

    auto inst = reinterpret_cast<SCBE_MicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != SCBE_MicroOp::End)
    {
        const auto& infos = g_MicroOpInfos[static_cast<int>(inst->op)];

        if (inst->flags.has(MIF_JUMP_DEST))
        {
            mapValReg.clear();
            mapRegVal.clear();
        }

        if (inst->op == SCBE_MicroOp::JumpM ||
            inst->op == SCBE_MicroOp::JumpTable ||
            inst->op == SCBE_MicroOp::JumpCC ||
            inst->op == SCBE_MicroOp::JumpCI)
        {
            mapValReg.clear();
            mapRegVal.clear();
        }

        auto legitReg = CPUReg::Max;
        if (inst->op == SCBE_MicroOp::StoreMR &&
            inst->regA == CPUReg::RSP &&
            out.cpuFct->isStackOffsetReg(static_cast<uint32_t>(inst->valueA)))
        {
            mapValReg[inst->valueA] = {inst->regB, inst->opBitsA};
            mapRegVal[inst->regB]   = inst->valueA;
        }
        else if (inst->op == SCBE_MicroOp::StoreMR &&
                 inst->regA == CPUReg::RSP &&
                 out.cpuFct->isStackOffsetParam(static_cast<uint32_t>(inst->valueA)))
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
        else if (inst->op == SCBE_MicroOp::LoadRM &&
                 inst->regB == CPUReg::RSP &&
                 mapValReg.contains(inst->valueA) &&
                 mapRegVal.contains(mapValReg[inst->valueA].first) &&
                 inst->opBitsA == mapValReg[inst->valueA].second &&
                 mapRegVal[mapValReg[inst->valueA].first] == inst->valueA)
        {
            if (mapValReg[inst->valueA].first == inst->regA)
            {
                //out.print();
                ignore(inst);
                //out.print();
            }
            else if (inst->opBitsA == OpBits::B64)
            {
                setOp(inst, SCBE_MicroOp::LoadRR);
                inst->regB = mapValReg[inst->valueA].first;                
            }
        }
        else if (inst->op == SCBE_MicroOp::LoadRM &&
                 inst->regB == CPUReg::RSP &&
                 out.cpuFct->isStackOffsetReg(static_cast<uint32_t>(inst->valueA)))
        {
            legitReg                = inst->regA;
            mapValReg[inst->valueA] = {inst->regA, inst->opBitsA};
            mapRegVal[inst->regA]   = inst->valueA;
        }

        if (inst->op != SCBE_MicroOp::Ignore)
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

void SCBE_Optimizer::optimize(const SCBE_Micro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;

    passHasDoneSomething = true;
    while (passHasDoneSomething)
    {
        passHasDoneSomething = false;
        passReduce(out);
        passStoreMR(out);
    }
}

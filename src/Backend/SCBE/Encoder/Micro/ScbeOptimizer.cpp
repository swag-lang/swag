#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Main/Statistics.h"
#include "ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#pragma optimize("", off)

void ScbeOptimizer::memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg)
{
    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        switch (inst->op)
        {
            case ScbeMicroOp::LoadSignedExtendRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadSignedExtendRR);
                    inst->regB = reg;
                }
                break;
            case ScbeMicroOp::LoadZeroExtendRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadZeroExtendRR);
                    inst->regB = reg;
                }
                break;
            case ScbeMicroOp::LoadRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRR);
                    inst->regB = reg;
                }
                break;
            case ScbeMicroOp::LoadMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRR);
                    inst->regA = reg;
                }
                break;
            case ScbeMicroOp::LoadMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRI);
                    inst->regA   = reg;
                    inst->valueA = inst->valueB;
                }
                break;
            /*case ScbeMicroOp::LoadAddressM:
                if (inst->regB == memReg &&
                    inst->valueB == memOffset)
                {
                    out.print();
                    SWAG_ASSERT(false);
                }
                break;*/
            case ScbeMicroOp::CmpMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::CmpRR);
                    inst->regA = reg;
                }
                break;
            case ScbeMicroOp::CmpMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::CmpRI);
                    inst->regA   = reg;
                    inst->valueA = inst->valueB;
                }
                break;
            case ScbeMicroOp::ClearM:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    SWAG_ASSERT(false);
                }
                break;
            case ScbeMicroOp::Copy:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    SWAG_ASSERT(false);
                }
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    SWAG_ASSERT(false);
                }
                break;
            case ScbeMicroOp::OpUnaryM:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpUnaryR);
                    inst->regA = reg;
                }
                break;
            case ScbeMicroOp::OpBinaryMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRI);
                    inst->regA   = reg;
                    inst->valueA = inst->valueB;
                }
                break;
            case ScbeMicroOp::OpBinaryMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRR);
                    inst->regA = reg;
                }
                break;
            case ScbeMicroOp::OpBinaryRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRR);
                    inst->regB = reg;
                }
                break;
        }

        inst = zap(inst + 1);
    }
}

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
                    ScbeCpu::isInt(inst->regA) &&
                    next->hasLeftOpFlag(MOF_OPBITS_A) &&
                    ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA) &&
                    !encoder->manipulateRegister(next, inst->regB))
                {
                    next->regA = inst->regB;
                    setDirtyPass();
                    break;
                }

                if (next->hasReadRegB() &&
                    inst->regA == next->regB &&
                    ScbeCpu::isInt(inst->regA) &&
                    next->hasLeftOpFlag(MOF_OPBITS_A) &&
                    ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA) &&
                    !encoder->manipulateRegister(next, inst->regB))
                {
                    next->regB = inst->regB;
                    setDirtyPass();
                    break;
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

            case ScbeMicroOp::LoadMR:
                if (next->op == ScbeMicroOp::LoadRM &&
                    ScbeCpu::isInt(next->opBitsA) == ScbeCpu::isInt(inst->opBitsA) &&
                    ScbeCpu::getNumBits(next->opBitsA) <= ScbeCpu::getNumBits(inst->opBitsA) &&
                    next->regB == inst->regA &&
                    next->valueA == inst->valueA)
                {
                    if (inst->regB == next->regA)
                    {
                        ignore(next);
                    }
                    else
                    {
                        setOp(next, ScbeMicroOp::LoadRR);
                        next->regB = inst->regB;
                    }
                    break;
                }

                if (inst->regA == CpuReg::Rsp &&
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
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump() || inst->isCall())
            mapRegInst.clear();

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
        else
        {
            if (inst->hasWriteRegA())
                mapRegInst.erase(inst->regA);
            if (inst->hasWriteRegB())
                mapRegInst.erase(inst->regB);
            if (inst->hasWriteRegC())
                mapRegInst.erase(inst->regC);
        }

        auto details = encoder->getInstructionDetails(inst);
        for (uint32_t i = 0; details.has(MOD_REG_ALL) && i < static_cast<uint32_t>(CpuReg::Max); details.remove(1ULL << i++))
        {
            if (legitReg == static_cast<CpuReg>(i))
                continue;
            if (details.has(1ULL << i))
                mapRegInst.erase(static_cast<CpuReg>(i));
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
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump())
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
        else if (inst->op == ScbeMicroOp::LoadRM &&
                 inst->regB == CpuReg::Rsp &&
                 mapValReg.contains(inst->valueA) &&
                 mapRegVal.contains(mapValReg[inst->valueA].first) &&
                 ScbeCpu::isInt(inst->opBitsA) == ScbeCpu::isInt(mapValReg[inst->valueA].second) &&
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
        }
        else if (inst->op == ScbeMicroOp::LoadRM &&
                 inst->regB == CpuReg::Rsp &&
                 out.cpuFct->isStackOffsetTransient(static_cast<uint32_t>(inst->valueA)))
        {
            if (mapRegVal.contains(inst->regA))
                mapValReg.erase(mapRegVal[inst->regA]);
            legitReg                = inst->regA;
            mapValReg[inst->valueA] = {inst->regA, inst->opBitsA};
            mapRegVal[inst->regA]   = inst->valueA;
        }
        else if (inst->op == ScbeMicroOp::CmpMR &&
                 inst->regA == CpuReg::Rsp &&
                 mapValReg.contains(inst->valueA) &&
                 mapRegVal[mapValReg[inst->valueA].first] == inst->valueA &&
                 ScbeCpu::isInt(inst->opBitsA) == ScbeCpu::isInt(mapValReg[inst->valueA].second) &&
                 ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(mapValReg[inst->valueA].second))
        {
            setOp(inst, ScbeMicroOp::CmpRR);
            inst->regA = mapValReg[inst->valueA].first;
            std::swap(inst->regA, inst->regB);
        }

        auto details = encoder->getInstructionDetails(inst);
        for (uint32_t i = 0; details.has(MOD_REG_ALL) && i < static_cast<uint32_t>(CpuReg::Max); details.remove(1ULL << i++))
        {
            if (static_cast<CpuReg>(i) == legitReg)
                continue;
            if (details.has(1ULL << i))
                mapRegVal.erase(static_cast<CpuReg>(i));
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
        optimizePassStore(out);
        optimizePassDeadStore(out);
        optimizePassStoreToRegBeforeLeave(out);
        optimizePassStoreToHdwRegBeforeLeave(out);
    }

    // memToReg(out, CpuReg::Rsp, out.cpuFct->getStackOffsetReg(0), CpuReg::Rdi);
}

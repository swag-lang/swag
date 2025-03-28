#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Main/Statistics.h"
#include "ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/Module.h"
#include "Wmf/SourceFile.h"

void ScbeOptimizer::memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg)
{
    SWAG_ASSERT(memReg != CpuReg::Rsp || !takeAddressRsp.contains(memOffset));

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
    if (inst->op != op)
    {
        inst->op = op;
        setDirtyPass();
    }
}

void ScbeOptimizer::setRegA(ScbeMicroInstruction* inst, CpuReg reg)
{
    if (inst->regA != reg)
    {
        inst->regA = reg;
        setDirtyPass();
    }
}

void ScbeOptimizer::setRegB(ScbeMicroInstruction* inst, CpuReg reg)
{
    if (inst->regB != reg)
    {
        inst->regB = reg;
        setDirtyPass();
    }
}

void ScbeOptimizer::setRegC(ScbeMicroInstruction* inst, CpuReg reg)
{
    if (inst->regC != reg)
    {
        inst->regC = reg;
        setDirtyPass();
    }
}

ScbeMicroInstruction* ScbeOptimizer::zap(ScbeMicroInstruction* inst)
{
    while (inst->op == ScbeMicroOp::Nop || inst->op == ScbeMicroOp::Label || inst->op == ScbeMicroOp::Debug || inst->op == ScbeMicroOp::Ignore)
        inst++;
    return inst;
}

void ScbeOptimizer::reduceNoOp(const ScbeMicro& out, ScbeMicroInstruction* inst)
{
    const auto next = zap(inst + 1);
    switch (inst->op)
    {
        case ScbeMicroOp::LoadRR:
            if (inst->regA == inst->regB)
            {
                ignore(inst);
                break;
            }

            break;

        case ScbeMicroOp::LoadMR:
            if (inst->regA == CpuReg::Rsp &&
                next->op == ScbeMicroOp::Leave)
            {
                ignore(inst);
                break;
            }

            break;
    }
}

void ScbeOptimizer::reduceNext(const ScbeMicro& out, ScbeMicroInstruction* inst)
{
    const auto next     = zap(inst + 1);
    const auto nextRegs = out.cpu->getWriteRegisters(next);
    if (next->flags.has(MIF_JUMP_DEST))
        return;

    switch (inst->op)
    {
        case ScbeMicroOp::LoadZeroExtendRM:
            if (next->op == ScbeMicroOp::LoadSignedExtendRR &&
                next->regA == inst->regA &&
                next->regB == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                next->opBitsB == inst->opBitsB)
            {
                setOp(inst, ScbeMicroOp::LoadSignedExtendRM);
                ignore(next);
            }
            break;

        case ScbeMicroOp::LoadRR:
            if (next->hasReadRegA() &&
                inst->regA == next->regA &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextRegs.contains(inst->regB) &&
                out.cpu->acceptsRegA(next, inst->regB))
            {
                setRegA(next, inst->regB);
                break;
            }

            if (next->hasReadRegB() &&
                inst->regA == next->regB &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextRegs.contains(inst->regB) &&
                out.cpu->acceptsRegB(next, inst->regB))
            {
                setRegB(next, inst->regB);
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
                ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(next->regA) &&
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
                    setRegB(next, inst->regB);
                }
                break;
            }

            if (next->op == ScbeMicroOp::LoadSignedExtendRM &&
                ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(next->regA) &&
                next->regB == inst->regA &&
                next->valueA == inst->valueA)
            {
                setOp(next, ScbeMicroOp::LoadSignedExtendRR);
                setRegB(next, inst->regB);
                break;
            }

            if (next->op == ScbeMicroOp::LoadZeroExtendRM &&
                ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(next->regA) &&
                next->regB == inst->regA &&
                next->valueA == inst->valueA)
            {
                setOp(next, ScbeMicroOp::LoadZeroExtendRR);
                setRegB(next, inst->regB);
                break;
            }

            break;
    }
}

void ScbeOptimizer::optimizePassReduce(const ScbeMicro& out)
{
    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        reduceNoOp(out, inst);
        reduceNext(out, inst);
        inst = zap(inst + 1);
    }
}

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

void ScbeOptimizer::computeContext(const ScbeMicro& out)
{
    takeAddressRsp.clear();
    usedRegs.clear();
    contextFlags.clear();

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->op == ScbeMicroOp::LoadAddressM &&
            inst->regB == CpuReg::Rsp)
        {
            takeAddressRsp.push_back(inst->valueB);
        }

        if (inst->isCall())
            contextFlags.add(CF_HAS_CALL);
        if (inst->isJump())
            contextFlags.add(CF_HAS_JUMP);

        if (inst->hasReadRegA() || inst->hasWriteRegA())
            usedRegs[inst->regA] += 1;
        if (inst->hasReadRegB() || inst->hasWriteRegB())
            usedRegs[inst->regB] += 1;
        if (inst->hasReadRegC() || inst->hasWriteRegC())
            usedRegs[inst->regC] += 1;

        auto details = out.cpu->getWriteRegisters(inst);
        for (auto r : details)
            usedRegs[r] += 1;

        inst = zap(inst + 1);
    }
}

void ScbeOptimizer::optimize(const ScbeMicro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;
    if (!out.cpuFct->bc->sourceFile->module->mustOptimizeBackend(out.cpuFct->bc->node))
        return;

    setDirtyPass();
    while (passHasDoneSomething)
    {
        passHasDoneSomething = false;
        computeContext(out);

        optimizePassReduce(out);
        optimizePassStore(out);
        optimizePassDeadStore(out);
        optimizePassStoreToRegBeforeLeave(out);
        optimizePassStoreToHdwRegBeforeLeave(out);

        /*if (!contextFlags.has(CF_HAS_CALL) &&
            out.cpuFct->typeFunc->numParamsRegisters() &&
            !out.cc->paramByRegisterInteger.empty() &&
            usedRegs[out.cc->paramByRegisterInteger[0]] == 1 &&
            (!out.cc->useRegisterFloat || !out.cpuFct->typeFunc->registerIdxToType(0)->isNativeFloat()) &&
            !takeAddressRsp.contains(out.cpuFct->getStackOffsetParam(0)))
        {
            memToReg(out, CpuReg::Rsp, out.cpuFct->getStackOffsetParam(0), out.cc->paramByRegisterInteger[0]);
        }*/
    }
}

#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/SourceFile.h"

void ScbeOptimizer::optimizePassAliasLoadRM(const ScbeMicro& out)
{
    mapRegInst.clear();
    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->op != ScbeMicroOp::LoadRM)
            continue;

        mapRegInst.clear();
        auto next = ScbeMicro::getNextInstruction(inst);
        while (true)
        {
            if (next->isJump() || next->isJumpDest() || next->isRet())
                break;

            if (next->op == ScbeMicroOp::LoadRR &&
                next->regB == inst->regA &&
                next->regA != inst->regB &&
                inst->opBitsA == next->opBitsA)
            {
                if (mapRegInst.contains(next->regA))
                    break;
                auto nextNext = ScbeMicro::getNextInstruction(next);
                while (true)
                {
                    if (nextNext->isJump())
                        break;
                    const auto readRegs1 = out.cpu->getReadRegisters(nextNext);
                    if (readRegs1.contains(next->regB))
                        break;
                    const auto writeRegs1 = out.cpu->getWriteRegisters(nextNext);
                    if (writeRegs1.contains(next->regB) || nextNext->isRet())
                    {
                        setRegA(out, inst, next->regA);
                        ignore(out, next);
                        break;
                    }

                    nextNext = ScbeMicro::getNextInstruction(nextNext);
                }

                break;
            }

            const auto regs = out.cpu->getReadWriteRegisters(next);
            for (const auto r : regs)
                mapRegInst[r] = next;
            if (mapRegInst.contains(inst->regA))
                break;

            next = ScbeMicro::getNextInstruction(next);
        }
    }
}

void ScbeOptimizer::optimizePassAliasLoadAddrRM(const ScbeMicro& out)
{
    mapRegInst.clear();
    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->isJump() && !inst->isJumpCond())
            mapRegInst.clear();
        if (inst->isJumpDest() || inst->isRet())
            mapRegInst.clear();

        switch (inst->op)
        {
            case ScbeMicroOp::CmpMR:
                if (mapRegInst.contains(inst->regA))
                {
                    const auto prev = mapRegInst[inst->regA];
                    if (out.cpu->encodeCmpMemReg(prev->regB, prev->valueA + inst->valueA, inst->regB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegA(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::CmpMI:
                if (mapRegInst.contains(inst->regA))
                {
                    const auto prev = mapRegInst[inst->regA];
                    if (out.cpu->encodeCmpMemImm(prev->regB, prev->valueA + inst->valueA, inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegA(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::OpBinaryMR:
                if (mapRegInst.contains(inst->regA))
                {
                    const auto prev = mapRegInst[inst->regA];
                    if (out.cpu->encodeOpBinaryMemReg(prev->regB, prev->valueA + inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegA(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::OpBinaryRR:
                if (inst->cpuOp == CpuOp::ADD)
                {
                    if (mapRegInst.contains(inst->regA))
                    {
                        const auto prev = mapRegInst[inst->regA];
                        if (inst->opBitsA == prev->opBitsA &&
                            out.cpu->encodeLoadAmcRegMem(inst->regA, inst->opBitsA, inst->regB, prev->regB, 1, prev->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                        {
                            inst->op      = ScbeMicroOp::LoadAddrAmcRM;
                            inst->regC    = prev->regB;
                            inst->valueA  = 1;
                            inst->valueB  = prev->valueA;
                            inst->opBitsB = inst->opBitsA;
                            mapRegInst.erase(inst->regA);
                            break;
                        }
                    }
                }
                break;

            case ScbeMicroOp::OpBinaryMI:
                if (mapRegInst.contains(inst->regA))
                {
                    const auto prev = mapRegInst[inst->regA];
                    if (out.cpu->encodeOpBinaryMemImm(prev->regB, prev->valueA + inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegA(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::LoadRR:
                if (mapRegInst.contains(inst->regB))
                {
                    const auto prev = mapRegInst[inst->regB];
                    if (inst->opBitsA == OpBits::B64)
                    {
                        setOp(out, inst, ScbeMicroOp::LoadAddrRM);
                        setRegB(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::LoadMI:
                if (mapRegInst.contains(inst->regA))
                {
                    const auto prev = mapRegInst[inst->regA];
                    if (out.cpu->encodeLoadMemImm(prev->regB, prev->valueA + inst->valueA, inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegA(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::LoadMR:
                if (mapRegInst.contains(inst->regA))
                {
                    const auto prev = mapRegInst[inst->regA];
                    if (out.cpu->encodeLoadMemReg(prev->regB, prev->valueA + inst->valueA, inst->regB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegA(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::LoadRM:
                if (mapRegInst.contains(inst->regB))
                {
                    const auto prev = mapRegInst[inst->regB];
                    if (out.cpu->encodeLoadRegMem(inst->regA, prev->regB, prev->valueA + inst->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegB(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::LoadZeroExtRM:
                if (mapRegInst.contains(inst->regB))
                {
                    const auto prev = mapRegInst[inst->regB];
                    if (out.cpu->encodeLoadZeroExtendRegMem(inst->regA, prev->regB, prev->valueA + inst->valueA, inst->opBitsA, inst->opBitsB, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegB(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;

            case ScbeMicroOp::LoadSignedExtRM:
                if (mapRegInst.contains(inst->regB))
                {
                    const auto prev = mapRegInst[inst->regB];
                    if (out.cpu->encodeLoadSignedExtendRegMem(inst->regA, prev->regB, prev->valueA + inst->valueA, inst->opBitsA, inst->opBitsB, EMIT_CanEncode) == CpuEncodeResult::Zero)
                    {
                        setRegB(out, inst, prev->regB);
                        setValueA(out, inst, prev->valueA + inst->valueA);
                        break;
                    }
                }
                break;
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            mapRegInst.erase(r);

            VectorNative<CpuReg> toErase;
            for (const auto& [r1, i] : mapRegInst)
            {
                if (i->regB == r)
                    toErase.push_back(r1);
            }

            for (const auto r1 : toErase)
                mapRegInst.erase(r1);
        }

        if (inst->op == ScbeMicroOp::LoadAddrRM && inst->regA != inst->regB)
            mapRegInst[inst->regA] = inst;
    }
}

void ScbeOptimizer::optimizePassAliasSymbolReloc(const ScbeMicro& out)
{
    mapRegInst.clear();
    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->isJump() || inst->isJumpDest() || inst->isRet())
            mapRegInst.clear();

        switch (inst->op)
        {
            case ScbeMicroOp::LoadRR:
                if (mapRegInst.contains(inst->regB))
                {
                    const auto prev = mapRegInst[inst->regB];
                    if (!hasReadRegAfter(out, inst, prev->regA))
                    {
                        setOp(out, inst, ScbeMicroOp::SymbolRelocAddr);
                        setValueA(out, inst, prev->valueA);
                        setValueB(out, inst, prev->valueB);
                        ignore(out, prev);
                        mapRegInst.erase(prev->regA);
                        break;
                    }
                }
                break;
        }

        const auto readRegs = out.cpu->getReadWriteRegisters(inst);
        for (const auto r : readRegs)
            mapRegInst.erase(r);

        if (inst->op == ScbeMicroOp::SymbolRelocAddr)
            mapRegInst[inst->regA] = inst;
    }
}

void ScbeOptimizer::optimizePassAliasLoadRR(const ScbeMicro& out)
{
    mapRegInst.clear();

    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->isJump() && !inst->isJumpCond())
            mapRegInst.clear();
        if (inst->isJumpDest() || inst->isRet())
            mapRegInst.clear();

        if (inst->hasReadRegA() && !inst->hasWriteRegA())
        {
            if (mapRegInst.contains(inst->regA))
            {
                const auto prev = mapRegInst[inst->regA];
                if (out.cpu->acceptsRegA(inst, prev->regB))
                {
                    if (inst->hasWriteMemA())
                    {
                        setRegA(out, inst, prev->regB);
                    }
                    else if (!inst->hasOpFlag(MOF_OPBITS_A) ||
                             !prev->hasOpFlag(MOF_OPBITS_A) ||
                             ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(prev->opBitsA))
                    {
                        setRegA(out, inst, prev->regB);
                    }
                }
            }
        }

        if (inst->hasReadRegB() && !inst->hasWriteRegB())
        {
            if (mapRegInst.contains(inst->regB))
            {
                const auto prev = mapRegInst[inst->regB];
                if (out.cpu->acceptsRegB(inst, prev->regB))
                {
                    if (inst->hasReadMemB())
                    {
                        setRegB(out, inst, prev->regB);
                    }
                    else if (!inst->hasOpFlag(MOF_OPBITS_A) ||
                             !prev->hasOpFlag(MOF_OPBITS_A) ||
                             ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(prev->opBitsA))
                    {
                        setRegB(out, inst, prev->regB);
                    }
                }
            }
        }

        if (inst->hasReadRegC() && !inst->hasWriteRegC())
        {
            if (mapRegInst.contains(inst->regC))
            {
                const auto prev = mapRegInst[inst->regC];
                if (out.cpu->acceptsRegC(inst, prev->regB))
                {
                    if (!inst->hasOpFlag(MOF_OPBITS_A) ||
                        !prev->hasOpFlag(MOF_OPBITS_A) ||
                        ScbeCpu::getNumBits(inst->opBitsA) <= ScbeCpu::getNumBits(prev->opBitsA))
                    {
                        setRegC(out, inst, prev->regB);
                    }
                }
            }
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            mapRegInst.erase(r);

            VectorNative<CpuReg> toErase;
            for (const auto& [r1, i] : mapRegInst)
            {
                if (i->regB == r)
                    toErase.push_back(r1);
            }

            for (const auto r1 : toErase)
                mapRegInst.erase(r1);
        }

        if (inst->op == ScbeMicroOp::LoadRR)
            mapRegInst[inst->regA] = inst;
    }
}

void ScbeOptimizer::optimizePassAliasLoadExtend(const ScbeMicro& out)
{
    mapRegInst.clear();

    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->isJump() && !inst->isJumpCond())
            mapRegInst.clear();
        if (inst->isJumpDest() || inst->isRet())
            mapRegInst.clear();

        if (inst->hasReadRegA() && !inst->hasWriteRegA())
        {
            if (mapRegInst.contains(inst->regA))
            {
                const auto prev = mapRegInst[inst->regA];
                if (prev->regB != inst->regA && inst->getOpBitsReadReg() == prev->opBitsB)
                {
                    setRegA(out, inst, prev->regB);
                }
            }
        }
        else if (inst->hasReadRegB() && !inst->hasWriteRegB())
        {
            if (mapRegInst.contains(inst->regB))
            {
                const auto prev = mapRegInst[inst->regB];
                if (prev->regB != inst->regB && inst->getOpBitsReadReg() == prev->opBitsB)
                {
                    setRegB(out, inst, prev->regB);
                }
            }
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            mapRegInst.erase(r);

            VectorNative<CpuReg> toErase;
            for (const auto& [r1, i] : mapRegInst)
            {
                if (i->regB == r)
                    toErase.push_back(r1);
            }

            for (const auto r1 : toErase)
                mapRegInst.erase(r1);
        }

        if (inst->op == ScbeMicroOp::LoadZeroExtRR)
            mapRegInst[inst->regA] = inst;
    }
}

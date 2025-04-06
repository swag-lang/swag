#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

void ScbeOptimizer::reduceNoOp(const ScbeMicro& out, ScbeMicroInstruction* inst, const ScbeMicroInstruction* next)
{
    switch (inst->op)
    {
        case ScbeMicroOp::LoadRR:
            if (inst->regA == inst->regB)
            {
                ignore(out, inst);
                break;
            }

            break;

        case ScbeMicroOp::LoadMR:
            if (inst->regA == CpuReg::Rsp &&
                next->op == ScbeMicroOp::Leave)
            {
                ignore(out, inst);
                break;
            }

            break;
    }
}

void ScbeOptimizer::reduceLoadRR(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->flags.has(MIF_JUMP_DEST))
        return;

    if (next->op == ScbeMicroOp::LoadRR &&
        !inst->hasReadRegA() &&
        inst->hasWriteRegA() &&
        next->regB == inst->regA &&
        ScbeCpu::isInt(next->regA) &&
        ScbeCpu::isInt(next->regB))
    {
        auto nextNext = ScbeMicro::getNextInstruction(next);
        while (true)
        {
            if (nextNext->flags.has(MIF_JUMP_DEST))
                break;
            if (nextNext->isJump() || nextNext->isRet())
                break;

            const auto readRegs  = out.cpu->getReadRegisters(nextNext);
            const auto writeRegs = out.cpu->getWriteRegisters(nextNext);

            if (nextNext->op != ScbeMicroOp::LoadRR &&
                out.cpu->acceptsRegB(inst, next->regA) &&
                writeRegs.contains(inst->regA) &&
                !readRegs.contains(inst->regA))
            {
                setRegA(inst, next->regA);
                ignore(out, next);
                break;
            }

            if (readRegs.contains(inst->regA))
                break;
            nextNext = ScbeMicro::getNextInstruction(nextNext);
        }
    }
}

void ScbeOptimizer::reduceOffset(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->flags.has(MIF_JUMP_DEST))
        return;

    switch (inst->op)
    {
        case ScbeMicroOp::OpBinaryRI:
            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::OpBinaryMI &&
                next->regA == inst->regA &&
                out.cpu->encodeOpBinaryMemImm(next->regA, next->valueA + inst->valueA, next->valueB, next->cpuOp, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->valueA += inst->valueA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::OpBinaryMR &&
                next->regA == inst->regA &&
                out.cpu->encodeOpBinaryMemReg(next->regA, next->valueA + inst->valueA, inst->regB, next->cpuOp, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->valueA += inst->valueA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::LoadMI &&
                next->regA == inst->regA &&
                out.cpu->encodeLoadMemImm(next->regA, next->valueA + inst->valueA, next->valueB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->valueA += inst->valueA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::LoadMR &&
                next->regA == inst->regA &&
                out.cpu->encodeLoadMemReg(next->regA, next->valueA + inst->valueA, next->regB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->valueA += inst->valueA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::OpUnaryM &&
                next->regA == inst->regA &&
                out.cpu->encodeOpUnaryMem(next->regA, next->valueA + inst->valueA, next->cpuOp, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->valueA += inst->valueA;
                swapInstruction(out, inst, next);
                break;
            }
            break;
    }
}

void ScbeOptimizer::reduceDup(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->flags.has(MIF_JUMP_DEST))
        return;

    switch (inst->op)
    {
        case ScbeMicroOp::LoadMR:
            if (next->op == ScbeMicroOp::LoadMR &&
                next->regA == inst->regA &&
                next->regB == inst->regB &&
                next->opBitsA == inst->opBitsA &&
                next->valueA == inst->valueA)
            {
                ignore(out, next);
                break;
            }

            break;

        case ScbeMicroOp::LoadRR:
            if (next->op == ScbeMicroOp::LoadRR &&
                inst->regA == next->regA &&
                inst->regB == next->regB &&
                inst->opBitsA == next->opBitsA)
            {
                ignore(out, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadRR &&
                inst->regA == next->regB &&
                inst->regB == next->regA &&
                inst->opBitsA == next->opBitsA &&
                inst->opBitsA == OpBits::B64)
            {
                ignore(out, next);
                break;
            }
            break;
    }
}

void ScbeOptimizer::reduceLoadAddress(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->flags.has(MIF_JUMP_DEST))
        return;

    switch (inst->op)
    {
        case ScbeMicroOp::LoadAddressAddMul:
            if (inst->regB == CpuReg::Max &&
                next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !ScbeMicro::getNextFlagSensitive(next)->isJumpCond() && // :x64optimoverflow
                out.cpu->encodeLoadAddressAddMul(inst->regA, next->regB, inst->regC, inst->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegB(inst, next->regB);
                ignore(out, next);
                break;
            }
            break;

        case ScbeMicroOp::LoadRR:
            if (next->op == ScbeMicroOp::OpBinaryRI &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !ScbeMicro::getNextFlagSensitive(next)->isJumpCond() && // :x64optimoverflow
                out.cpu->encodeLoadAddressMem(next->regA, inst->regB, next->valueA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(next, ScbeMicroOp::LoadAddressM);
                next->regB = inst->regB;
                ignore(out, inst);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryRI &&
                next->cpuOp == CpuOp::SHL &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !ScbeMicro::getNextFlagSensitive(next)->isJumpCond() && // :x64optimoverflow
                out.cpu->encodeLoadAddressAddMul(next->regA, CpuReg::Max, inst->regB, 1ULL << next->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(next, ScbeMicroOp::LoadAddressAddMul);
                next->regB   = CpuReg::Max;
                next->regC   = inst->regB;
                next->valueA = 1ULL << next->valueA;
                ignore(out, inst);
                break;
            }

            break;

        case ScbeMicroOp::OpBinaryRI:
            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::LoadRR &&
                next->regB == inst->regA &&
                out.cpu->encodeLoadAddressMem(next->regA, inst->regA, inst->valueA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(next, ScbeMicroOp::LoadAddressM);
                next->regB   = inst->regA;
                next->valueA = inst->valueA;
                swapInstruction(out, inst, next);
                break;
            }
            break;
    }
}

void ScbeOptimizer::reduceNext(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->flags.has(MIF_JUMP_DEST))
        return;
    RegisterSet nextRegs;

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
                ignore(out, next);
            }
            break;

        case ScbeMicroOp::LoadRR:
            nextRegs = out.cpu->getWriteRegisters(next);
            if (next->hasReadRegA() &&
                !next->hasWriteRegA() &&
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
                !next->hasWriteRegB() &&
                inst->regA == next->regB &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextRegs.contains(inst->regB) &&
                out.cpu->acceptsRegB(next, inst->regB))
            {
                setRegB(next, inst->regB);
                break;
            }

            if (next->hasReadRegC() &&
                inst->regA == next->regC &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextRegs.contains(inst->regB) &&
                out.cpu->acceptsRegC(next, inst->regC))
            {
                setRegC(next, inst->regB);
                break;
            }
            break;

        case ScbeMicroOp::OpBinaryRI:
            if (next->op == ScbeMicroOp::LoadRR &&
                next->regA == inst->regA &&
                next->regB != next->regA)
            {
                ignore(out, inst);
                break;
            }

            if (next->op == ScbeMicroOp::LoadRI &&
                next->regA == inst->regA)
            {
                ignore(out, inst);
                break;
            }
            break;

        case ScbeMicroOp::LoadAddressM:
            if (next->op == ScbeMicroOp::LoadRR &&
                inst->regA != inst->regB &&
                next->regB == inst->regA &&
                next->opBitsA == OpBits::B64)
            {
                setOp(next, ScbeMicroOp::LoadAddressM);
                setRegB(next, inst->regB);
                setValueA(next, inst->valueA);
                break;
            }

            if (next->op == ScbeMicroOp::LoadMR &&
                next->regA == inst->regA &&
                next->regB != inst->regA &&
                out.cpu->encodeLoadMemReg(inst->regB, next->valueA + inst->valueA, next->regB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegA(next, inst->regB);
                setValueA(next, next->valueA + inst->valueA);
                swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadRM &&
                next->regB == inst->regA &&
                next->regA != inst->regB &&
                out.cpu->encodeLoadRegMem(next->regA, inst->regB, next->valueA + inst->valueA, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegB(next, inst->regB);
                setValueA(next, next->valueA + inst->valueA);
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadZeroExtendRM &&
                next->regB == inst->regA &&
                next->regA != inst->regB &&
                out.cpu->encodeLoadZeroExtendRegMem(next->regA, inst->regB, next->valueA + inst->valueA, next->opBitsA, next->opBitsB, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegB(next, inst->regB);
                setValueA(next, next->valueA + inst->valueA);
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            break;

        case ScbeMicroOp::LoadMR:
            if (next->op == ScbeMicroOp::LoadRM &&
                ScbeCpu::getNumBits(next->opBitsA) <= ScbeCpu::getNumBits(inst->opBitsA) &&
                next->regB == inst->regA &&
                next->valueA == inst->valueA)
            {
                if (inst->regB == next->regA)
                {
                    ignore(out, next);
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
                next->valueA == inst->valueA &&
                ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsB))
            {
                setOp(next, ScbeMicroOp::LoadSignedExtendRR);
                setRegB(next, inst->regB);
                break;
            }

            if (next->op == ScbeMicroOp::LoadZeroExtendRM &&
                ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(next->regA) &&
                next->regB == inst->regA &&
                next->valueA == inst->valueA &&
                ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsB))
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
    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        const auto next = ScbeMicro::getNextInstruction(inst);
        reduceNoOp(out, inst, next);
        reduceNext(out, inst, next);
        reduceLoadAddress(out, inst, next);
        reduceDup(out, inst, next);
        reduceOffset(out, inst, next);
        reduceLoadRR(out, inst, next);
        inst = next;
    }
}

void ScbeOptimizer::reduceUnusedStack(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction*)
{
    const auto stackOffset = inst->getStackOffsetWrite();
    if (out.cpuFct->isStackOffsetTransient(stackOffset))
    {
        const auto size = inst->getNumBytes();
        if (!rangeReadStack.contains(static_cast<uint32_t>(inst->valueA), size))
            ignore(out, inst);
    }
}

void ScbeOptimizer::optimizePassReduce2(const ScbeMicro& out)
{
    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        const auto next = ScbeMicro::getNextInstruction(inst);
        reduceUnusedStack(out, inst, next);
        inst = next;
    }
}

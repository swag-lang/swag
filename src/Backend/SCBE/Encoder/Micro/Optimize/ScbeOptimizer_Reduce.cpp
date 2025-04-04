#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

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
                next->op == ScbeMicroOp::CmpMI &&
                next->regA == inst->regA &&
                out.cpu->encodeCmpMemImm(next->regA, next->valueA + inst->valueA, next->valueB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->valueA += inst->valueA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::CmpMR &&
                next->regA == inst->regA &&
                out.cpu->encodeCmpMemReg(next->regA, next->valueA + inst->valueA, next->regB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
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

        case ScbeMicroOp::LoadMR:
            if (next->op == ScbeMicroOp::LoadRM &&
                ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(next->regA) &&
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

void ScbeOptimizer::reduceUnusedStack(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction*)
{
    const auto stackOffset = inst->getStackOffsetWrite();
    if (out.cpuFct->isStackOffsetTransient(stackOffset) &&
        !usedReadStack.contains(static_cast<uint32_t>(inst->valueA)))
    {
        ignore(out, inst);
    }
}

void ScbeOptimizer::optimizePassReduce(const ScbeMicro& out)
{
    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        const auto next = ScbeMicro::getNextInstruction(inst);
        reduceUnusedStack(out, inst, next);
        reduceNoOp(out, inst, next);
        reduceNext(out, inst, next);
        reduceDup(out, inst, next);
        reduceOffset(out, inst, next);
        reduceLoadRR(out, inst, next);
        inst = next;
    }
}

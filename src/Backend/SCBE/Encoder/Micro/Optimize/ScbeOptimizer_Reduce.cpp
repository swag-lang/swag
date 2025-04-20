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

        case ScbeMicroOp::CmpMI:
        case ScbeMicroOp::CmpMR:
        case ScbeMicroOp::CmpRI:
        case ScbeMicroOp::CmpRR:
        {
            const auto nextF = out.getNextFlagSensitive(inst);
            if (!out.cpu->doesReadFlags(nextF))
                ignore(out, inst);
            break;
        }

        case ScbeMicroOp::JumpCondI:
        case ScbeMicroOp::JumpCond:
        {
            const auto destJump = out.getFirstInstruction() + inst->valueB;
            auto       nextJ    = inst + 1;
            while (nextJ->op == ScbeMicroOp::Nop ||
                   nextJ->op == ScbeMicroOp::Label ||
                   nextJ->op == ScbeMicroOp::Debug ||
                   nextJ->op == ScbeMicroOp::PatchJump ||
                   nextJ->op == ScbeMicroOp::Ignore)
            {
                nextJ++;
                if (nextJ == destJump)
                    break;
            }
            if (nextJ == destJump)
            {
                ignore(out, inst);
                break;
            }
            break;
        }
    }
}

void ScbeOptimizer::reduceInst(const ScbeMicro& out, ScbeMicroInstruction* inst)
{
    switch (inst->op)
    {
        case ScbeMicroOp::LoadAmcMI:
            if (inst->regB == CpuReg::Rsp)
            {
                SWAG_ASSERT(inst->valueA == 1);
                std::swap(inst->regA, inst->regB);
                setDirtyPass();
                break;
            }

            break;

        case ScbeMicroOp::LoadAmcMR:
            if (inst->regB == CpuReg::Rsp)
            {
                SWAG_ASSERT(inst->valueA == 1);
                std::swap(inst->regA, inst->regB);
                setDirtyPass();
                break;
            }

            break;

        case ScbeMicroOp::LoadAmcRM:
            if (inst->regC == CpuReg::Rsp)
            {
                SWAG_ASSERT(inst->valueA == 1);
                std::swap(inst->regB, inst->regC);
                setDirtyPass();
                break;
            }

            if (inst->regB == CpuReg::Max &&
                inst->valueA == 1 &&
                out.cpu->encodeLoadRegMem(inst->regA, inst->regC, inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, inst, ScbeMicroOp::LoadRM);
                setRegB(out, inst, inst->regC);
                setValueA(out, inst, inst->valueB);
                break;
            }
            break;

        case ScbeMicroOp::LoadAddrAmcRM:
            if (inst->regC == CpuReg::Rsp)
            {
                SWAG_ASSERT(inst->valueA == 1);
                std::swap(inst->regB, inst->regC);
                setDirtyPass();
                break;
            }

            if (inst->regB == CpuReg::Max &&
                inst->valueA == 1 &&
                out.cpu->encodeLoadRegMem(inst->regA, inst->regC, inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, inst, ScbeMicroOp::LoadAddrRM);
                setRegB(out, inst, inst->regC);
                setValueA(out, inst, inst->valueB);
                break;
            }
            break;
    }
}

void ScbeOptimizer::reduceLoadRR(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (inst->hasReadRegA() || !inst->hasWriteRegA())
        return;
    if (next->isJump() || next->isJumpDest())
        return;
    if (next->op != ScbeMicroOp::LoadRR)
        return;
    if (next->regB != inst->regA || !ScbeCpu::isInt(next->regA) || !ScbeCpu::isInt(next->regB))
        return;
    if (!out.cpu->acceptsRegB(inst, next->regA))
        return;

    auto nextNext = ScbeMicro::getNextInstruction(next);
    while (true)
    {
        if (nextNext->isJump() || nextNext->isJumpDest() || nextNext->isRet())
            break;

        const auto readRegs  = out.cpu->getReadRegisters(nextNext);
        const auto writeRegs = out.cpu->getWriteRegisters(nextNext);

        if (nextNext->op != ScbeMicroOp::LoadRR &&
            writeRegs.contains(inst->regA) &&
            !readRegs.contains(inst->regA))
        {
            setRegA(out, inst, next->regA);
            ignore(out, next);
            break;
        }

        if (readRegs.contains(inst->regA))
            break;
        nextNext = ScbeMicro::getNextInstruction(nextNext);
    }
}

void ScbeOptimizer::reduceMemOffset(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->isJump() || next->isJumpDest())
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

void ScbeOptimizer::reduceDup(const ScbeMicro& out, const ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->isJump() || next->isJumpDest())
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

        case ScbeMicroOp::LoadZeroExtRR:
        case ScbeMicroOp::LoadSignedExtRR:
            if (next->op == inst->op &&
                inst->regA == next->regA &&
                inst->regB == next->regB &&
                inst->opBitsA == next->opBitsA &&
                inst->opBitsB == next->opBitsB)
            {
                ignore(out, next);
                break;
            }
            break;

        case ScbeMicroOp::LoadRM:
            if (next->op == ScbeMicroOp::LoadRM &&
                inst->regA != inst->regB &&
                inst->regA != next->regB &&
                inst->regA == next->regA &&
                inst->regB == next->regB &&
                inst->valueA == next->valueA &&
                inst->opBitsA == next->opBitsA)
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
    if (next->isJump() || next->isJumpDest())
        return;

    switch (inst->op)
    {
        case ScbeMicroOp::LoadAddrRM:
            if (inst->regB == inst->regA &&
                inst->opBitsA == inst->opBitsB)
            {
                inst->op      = ScbeMicroOp::OpBinaryRI;
                inst->cpuOp   = CpuOp::ADD;
                inst->opBitsB = inst->opBitsA;
                break;
            }

            if (next->op == ScbeMicroOp::LoadAddrAmcRM &&
                next->regB == inst->regA &&
                next->opBitsA == OpBits::B64 &&
                next->opBitsB == OpBits::B64 &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, next->opBitsA, inst->regB, next->regC, next->valueA, next->valueB + inst->valueA, next->opBitsB, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegB(out, next, inst->regB);
                setValueB(out, next, next->valueB + inst->valueA);
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regB == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, next->regA, inst->regA, 1, inst->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->op      = ScbeMicroOp::LoadAddrAmcRM;
                next->opBitsB = inst->opBitsA;
                next->valueA  = 1;
                next->valueB  = inst->valueA;
                next->regB    = next->regA;
                next->regC    = inst->regB;
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryRI &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == OpBits::B64 &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressRegMem(inst->regA, inst->regB, inst->valueA + next->valueA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setValueA(out, inst, inst->valueA + next->valueA);
                ignore(out, next);
                break;
            }

            if (next->op == ScbeMicroOp::CmpMI &&
                inst->regA != inst->regB &&
                next->regA == inst->regA &&
                out.cpu->encodeCmpMemImm(inst->regB, inst->valueA + next->valueA, next->valueB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegA(out, next, inst->regB);
                setValueA(out, next, inst->valueA + next->valueA);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryMI &&
                inst->regA != inst->regB &&
                next->regA == inst->regA &&
                out.cpu->encodeOpBinaryMemImm(inst->regB, inst->valueA + next->valueA, next->valueB, next->cpuOp, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegA(out, next, inst->regB);
                setValueA(out, next, inst->valueA + next->valueA);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryMR &&
                inst->regA != inst->regB &&
                next->regA == inst->regA &&
                out.cpu->encodeOpBinaryMemReg(inst->regB, inst->valueA + next->valueA, next->regB, next->cpuOp, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegA(out, next, inst->regB);
                setValueA(out, next, inst->valueA + next->valueA);
                break;
            }

            break;

        case ScbeMicroOp::LoadAmcRM:
            if (inst->regB == inst->regC && inst->valueA == 1 && inst->valueB == 0)
            {
                inst->regB   = CpuReg::Max;
                inst->valueA = 2;
                break;
            }
            break;

        case ScbeMicroOp::LoadAddrAmcRM:
            if (inst->regB == inst->regC && inst->valueA == 1 && inst->valueB == 0)
            {
                inst->regB   = CpuReg::Max;
                inst->valueA = 2;
                break;
            }

            if (inst->regB == CpuReg::Max &&
                inst->valueB == 0 &&
                next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressAmcRegMem(inst->regA, inst->opBitsA, next->regB, inst->regC, inst->valueA, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegB(out, inst, next->regB);
                ignore(out, next);
                break;
            }

            if (inst->regB == CpuReg::Max &&
                next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regB == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, next->regA, inst->regC, inst->valueA, inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->op      = ScbeMicroOp::LoadAddrAmcRM;
                next->opBitsB = inst->opBitsA;
                next->valueA  = inst->valueA;
                next->valueB  = inst->valueB;
                next->regB    = next->regA;
                next->regC    = inst->regC;
                swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadRM &&
                next->regB == inst->regA &&
                (next->regA != inst->regB || next->regA == inst->regA) &&
                (next->regA != inst->regC || next->regA == inst->regA) &&
                out.cpu->encodeLoadAmcRegMem(next->regA, next->opBitsA, inst->regB, inst->regC, inst->valueA, next->valueA + inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->op      = ScbeMicroOp::LoadAmcRM;
                next->opBitsB = inst->opBitsA;
                next->valueB  = next->valueA + inst->valueB;
                next->valueA  = inst->valueA;
                next->regB    = inst->regB;
                next->regC    = inst->regC;
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadMR &&
                next->regA == inst->regA &&
                out.cpu->encodeLoadAmcMemReg(inst->regB, inst->regC, inst->valueA, next->valueA + inst->valueB, inst->opBitsA, next->regB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->op      = ScbeMicroOp::LoadAmcMR;
                next->regC    = next->regB;
                next->regA    = inst->regB;
                next->regB    = inst->regC;
                next->valueB  = next->valueA + inst->valueB;
                next->valueA  = inst->valueA;
                next->opBitsB = next->opBitsA;
                next->opBitsA = inst->opBitsA;
                swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadMI &&
                next->regA == inst->regA &&
                out.cpu->encodeLoadAmcMemImm(inst->regB, inst->regC, inst->valueA, next->valueA + inst->valueB, inst->opBitsA, next->valueB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                next->op      = ScbeMicroOp::LoadAmcMI;
                next->regA    = inst->regB;
                next->regB    = inst->regC;
                next->valueC  = next->valueB;
                next->valueB  = next->valueA + inst->valueB;
                next->valueA  = inst->valueA;
                next->opBitsB = next->opBitsA;
                next->opBitsA = inst->opBitsA;
                swapInstruction(out, inst, next);
                break;
            }

            break;

        case ScbeMicroOp::LoadRI:
            if (next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressRegMem(next->regA, next->regB, inst->valueA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrRM);
                setValueA(out, next, inst->valueA);
                ignore(out, inst);
                break;
            }
            break;

        case ScbeMicroOp::LoadRR:
            if (next->op == ScbeMicroOp::OpBinaryRI &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressRegMem(next->regA, inst->regB, next->valueA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrRM);
                setRegB(out, next, inst->regB);
                ignore(out, inst);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryRI &&
                next->cpuOp == CpuOp::SHL &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, CpuReg::Max, inst->regB, 1ULL << next->valueA, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrAmcRM);
                next->regB    = CpuReg::Max;
                next->regC    = inst->regB;
                next->valueA  = 1ULL << next->valueA;
                next->valueB  = 0;
                next->opBitsA = inst->opBitsA;
                next->opBitsB = inst->opBitsA;
                ignore(out, inst);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                !out.getNextFlagSensitive(next)->isJumpCond() &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, CpuReg::Max, inst->regB, 1, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrAmcRM);
                next->regC    = next->regB;
                next->regB    = inst->regB;
                next->valueA  = 1;
                next->valueB  = 0;
                next->opBitsA = inst->opBitsA;
                next->opBitsB = inst->opBitsA;
                ignore(out, inst);
                break;
            }

            if (next->op == ScbeMicroOp::OpBinaryRM &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->opBitsA == inst->opBitsA)
            {
                setOp(out, next, ScbeMicroOp::LoadRM);
                setOp(out, inst, ScbeMicroOp::OpBinaryRR);
                inst->cpuOp = CpuOp::ADD;
                swapInstruction(out, inst, next);
                break;
            }

            break;

        case ScbeMicroOp::OpBinaryRI:
            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::LoadRR &&
                next->regB == inst->regA &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, CpuReg::Max, inst->regA, 1, inst->valueA, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrAmcRM);
                next->regB    = CpuReg::Max;
                next->regC    = inst->regA;
                next->valueA  = 1;
                next->valueB  = inst->valueA;
                next->opBitsA = inst->opBitsA;
                next->opBitsB = inst->opBitsA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::SHL &&
                next->op == ScbeMicroOp::LoadRR &&
                next->regB == inst->regA &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, CpuReg::Max, inst->regA, 1ULL << inst->valueA, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrAmcRM);
                next->regB    = CpuReg::Max;
                next->regC    = inst->regA;
                next->valueA  = 1ULL << inst->valueA;
                next->valueB  = 0;
                next->opBitsA = inst->opBitsA;
                next->opBitsB = inst->opBitsA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::SHL &&
                next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regB == inst->regA &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, next->regA, inst->regA, 1ULL << inst->valueA, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrAmcRM);
                next->regB    = next->regA;
                next->regC    = inst->regA;
                next->valueA  = 1ULL << inst->valueA;
                next->valueB  = 0;
                next->opBitsA = inst->opBitsA;
                next->opBitsB = inst->opBitsA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::SHL &&
                next->op == ScbeMicroOp::OpBinaryRR &&
                next->cpuOp == CpuOp::ADD &&
                next->regA == inst->regA &&
                next->regB != next->regA &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, next->regB, inst->regA, 1ULL << inst->valueA, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrAmcRM);
                next->regC    = inst->regA;
                next->valueA  = 1ULL << inst->valueA;
                next->valueB  = 0;
                next->opBitsA = inst->opBitsA;
                next->opBitsB = inst->opBitsA;
                ignore(out, inst);
                break;
            }

            break;

        case ScbeMicroOp::OpBinaryRR:
            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::LoadRR &&
                next->opBitsA == inst->opBitsA &&
                next->regB == inst->regA &&
                next->regA != inst->regA &&
                next->regA != inst->regB &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, inst->opBitsA, inst->regA, inst->regB, 1, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrAmcRM);
                next->regB    = inst->regA;
                next->regC    = inst->regB;
                next->valueA  = 1;
                next->valueB  = 0;
                next->opBitsA = inst->opBitsA;
                next->opBitsB = inst->opBitsA;
                swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::LoadRM &&
                next->regB == inst->regA &&
                inst->regB != next->regA &&
                out.cpu->encodeLoadAddressAmcRegMem(next->regA, next->opBitsA, next->regB, inst->regB, 1, 0, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAmcRM);
                next->regC    = inst->regB;
                next->valueB  = next->valueA;
                next->valueA  = 1;
                next->opBitsB = OpBits::B64;
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            if (inst->cpuOp == CpuOp::ADD &&
                next->op == ScbeMicroOp::LoadMI &&
                next->regA == inst->regA &&
                out.cpu->encodeLoadAmcMemImm(next->regA, inst->regB, 1, next->valueA, inst->opBitsA, next->valueB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadAmcMI);
                next->regB    = inst->regB;
                next->valueC  = next->valueB;
                next->valueB  = next->valueA;
                next->valueA  = 1;
                next->opBitsB = next->opBitsA;
                next->opBitsA = inst->opBitsA;
                swapInstruction(out, inst, next);
                break;
            }

            break;
    }
}

void ScbeOptimizer::reduceNext(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next)
{
    if (next->isJumpDest())
        return;

    const auto nextReadRegs  = out.cpu->getReadRegisters(next);
    const auto nextWriteRegs = out.cpu->getWriteRegisters(next);

    switch (inst->op)
    {
        case ScbeMicroOp::LoadZeroExtRM:
            if (next->op == ScbeMicroOp::LoadSignedExtRR &&
                next->regA == inst->regA &&
                next->regB == inst->regA &&
                next->opBitsA == inst->opBitsA &&
                next->opBitsB == inst->opBitsB)
            {
                setOp(out, inst, ScbeMicroOp::LoadSignedExtRM);
                ignore(out, next);
                break;
            }
            break;

        case ScbeMicroOp::LoadZeroExtRR:
        case ScbeMicroOp::LoadSignedExtRR:
            if (next->op == ScbeMicroOp::LoadRR &&
                inst->regA != inst->regB &&
                next->regB == inst->regA &&
                next->opBitsA == inst->opBitsA)
            {
                setOp(out, next, inst->op);
                setRegB(out, next, inst->regB);
                next->opBitsB = inst->opBitsB;
                break;
            }

            if (next->hasReadRegA() &&
                !next->hasWriteRegA() &&
                inst->regA != inst->regB &&
                inst->regA == next->regA &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsB) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextWriteRegs.contains(inst->regB) &&
                out.cpu->acceptsRegA(next, inst->regB))
            {
                setRegA(out, next, inst->regB);
                break;
            }

            if (next->hasReadRegB() &&
                !next->hasWriteRegB() &&
                inst->regA != inst->regB &&
                inst->regA == next->regB &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsB) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextWriteRegs.contains(inst->regB) &&
                out.cpu->acceptsRegB(next, inst->regB))
            {
                setRegB(out, next, inst->regB);
                break;
            }

            if (next->hasReadRegC() &&
                inst->regA != inst->regB &&
                inst->regA == next->regC &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsB) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextWriteRegs.contains(inst->regB) &&
                out.cpu->acceptsRegC(next, inst->regC))
            {
                setRegC(out, next, inst->regB);
                break;
            }
            break;

        case ScbeMicroOp::LoadRR:
            if (next->hasReadRegA() &&
                !next->hasWriteRegA() &&
                inst->regA == next->regA &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextWriteRegs.contains(inst->regB) &&
                out.cpu->acceptsRegA(next, inst->regB))
            {
                setRegA(out, next, inst->regB);
                break;
            }

            if (next->hasReadRegB() &&
                !next->hasWriteRegB() &&
                inst->regA == next->regB &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextWriteRegs.contains(inst->regB) &&
                out.cpu->acceptsRegB(next, inst->regB))
            {
                setRegB(out, next, inst->regB);
                break;
            }

            if (next->hasReadRegC() &&
                inst->regA == next->regC &&
                ScbeCpu::isInt(inst->regA) &&
                (!next->hasOpFlag(MOF_OPBITS_A) || ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsA)) &&
                !nextWriteRegs.contains(inst->regB) &&
                out.cpu->acceptsRegC(next, inst->regC))
            {
                setRegC(out, next, inst->regB);
                break;
            }

            if (next->op == ScbeMicroOp::LoadRI &&
                inst->regA != inst->regB &&
                inst->regB == next->regA)
            {
                const auto nextNext = ScbeMicro::getNextInstruction(next);
                if (!nextNext->isJumpDest() &&
                    nextNext->op == ScbeMicroOp::LoadRR &&
                    nextNext->regB == inst->regA)
                {
                    setRegB(out, nextNext, inst->regB);
                    swapInstruction(out, next, nextNext);
                    break;
                }
            }

            {
                const auto nextNext = ScbeMicro::getNextInstruction(next);
                if (!nextNext->isJumpDest() &&
                    nextNext->op == ScbeMicroOp::LoadRR &&
                    nextNext->regB == inst->regA &&
                    !nextWriteRegs.contains(inst->regA) &&
                    nextWriteRegs.contains(inst->regB) &&
                    !nextReadRegs.contains(inst->regA) &&
                    !nextWriteRegs.contains(nextNext->regA) &&
                    !nextReadRegs.contains(nextNext->regA) &&
                    inst->opBitsA == nextNext->opBitsA)
                {
                    setRegB(out, nextNext, inst->regB);
                    swapInstruction(out, next, nextNext);
                    break;
                }
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

        case ScbeMicroOp::LoadAddrRM:
            if (next->op == ScbeMicroOp::LoadRR &&
                inst->regA != inst->regB &&
                next->regB == inst->regA &&
                next->opBitsA == OpBits::B64)
            {
                setOp(out, next, ScbeMicroOp::LoadAddrRM);
                setRegB(out, next, inst->regB);
                setValueA(out, next, inst->valueA);
                break;
            }

            if (next->op == ScbeMicroOp::LoadMR &&
                next->regA == inst->regA &&
                next->regB != inst->regA &&
                out.cpu->encodeLoadMemReg(inst->regB, next->valueA + inst->valueA, next->regB, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegA(out, next, inst->regB);
                setValueA(out, next, next->valueA + inst->valueA);
                swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadRM &&
                next->regB == inst->regA &&
                next->regA != inst->regB &&
                out.cpu->encodeLoadRegMem(next->regA, inst->regB, next->valueA + inst->valueA, next->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegB(out, next, inst->regB);
                setValueA(out, next, next->valueA + inst->valueA);
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            if (next->op == ScbeMicroOp::LoadZeroExtRM &&
                next->regB == inst->regA &&
                next->regA != inst->regB &&
                out.cpu->encodeLoadZeroExtendRegMem(next->regA, inst->regB, next->valueA + inst->valueA, next->opBitsA, next->opBitsB, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setRegB(out, next, inst->regB);
                setValueA(out, next, next->valueA + inst->valueA);
                if (next->regA == inst->regA)
                    ignore(out, inst);
                else
                    swapInstruction(out, inst, next);
                break;
            }

            break;

        case ScbeMicroOp::LoadMI:
            if (next->op == ScbeMicroOp::LoadRM &&
                inst->regA == next->regB &&
                inst->valueA == next->valueA &&
                inst->opBitsA == next->opBitsA &&
                out.cpu->encodeLoadRegImm(next->regA, inst->valueB, inst->opBitsA, EMIT_CanEncode) == CpuEncodeResult::Zero)
            {
                setOp(out, next, ScbeMicroOp::LoadRI);
                setValueA(out, next, inst->valueB);
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
                    setOp(out, next, ScbeMicroOp::LoadRR);
                    setRegB(out, next, inst->regB);
                }
                break;
            }

            if (next->op == ScbeMicroOp::LoadSignedExtRM &&
                ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(next->regA) &&
                next->regB == inst->regA &&
                next->valueA == inst->valueA &&
                ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsB))
            {
                setOp(out, next, ScbeMicroOp::LoadSignedExtRR);
                setRegB(out, next, inst->regB);
                break;
            }

            if (next->op == ScbeMicroOp::LoadZeroExtRM &&
                ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(next->regA) &&
                next->regB == inst->regA &&
                next->valueA == inst->valueA &&
                ScbeCpu::getNumBits(inst->opBitsA) >= ScbeCpu::getNumBits(next->opBitsB))
            {
                setOp(out, next, ScbeMicroOp::LoadZeroExtRR);
                setRegB(out, next, inst->regB);
                break;
            }

            break;
    }
}

void ScbeOptimizer::reduceAliasHwdReg(const ScbeMicro& out, ScbeMicroInstruction* inst)
{
    switch (inst->op)
    {
        case ScbeMicroOp::LoadRR:
            if (usedWriteRegs[inst->regA] == 1 &&
                usedWriteRegs[inst->regB] <= 1 &&
                out.cc->nonVolatileRegistersInteger.contains(inst->regA) &&
                out.cc->nonVolatileRegistersInteger.contains(inst->regB))
            {
                regToReg(out, inst->regB, inst->regA);
                ignore(out, inst);
            }
            break;

        case ScbeMicroOp::LoadSignedExtRR:
        case ScbeMicroOp::LoadZeroExtRR:
            if (usedWriteRegs[inst->regA] == 1 &&
                usedWriteRegs[inst->regB] <= 1 &&
                out.cc->nonVolatileRegistersInteger.contains(inst->regA) &&
                out.cc->nonVolatileRegistersInteger.contains(inst->regB))
            {
                regToReg(out, inst->regB, inst->regA);
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
        if (next->op == ScbeMicroOp::End)
            break;
        reduceNoOp(out, inst, next);
        reduceNext(out, inst, next);
        reduceLoadAddress(out, inst, next);
        reduceDup(out, inst, next);
        reduceMemOffset(out, inst, next);
        reduceLoadRR(out, inst, next);
        reduceInst(out, inst);
        reduceAliasHwdReg(out, inst);
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

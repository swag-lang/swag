#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

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

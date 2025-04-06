#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassAliasHdwReg(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isJump() ||
            inst->isRet())
        {
            mapRegInst.clear();
        }

        if (inst->hasReadRegB())
        {
            if (mapRegInst.contains(inst->regB))
            {
                const auto prev = mapRegInst[inst->regB];
                if (inst->op == ScbeMicroOp::LoadRR &&
                    inst->regA == prev->regA &&
                    inst->regB == prev->regB &&
                    inst->opBitsA == prev->opBitsA)
                {
                    ignore(out, inst);
                }
                else if (ScbeCpu::isInt(inst->regB) == ScbeCpu::isInt(prev->regB) &&
                         inst->opBitsA == prev->opBitsA &&
                         !inst->hasWriteRegB() &&
                         out.cpu->acceptsRegB(inst, prev->regB))
                {
                    setRegB(inst, prev->regB);
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
        {
            mapRegInst[inst->regA] = inst;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::optimizePassDeadHdwReg(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) ||
            inst->isRet() ||
            inst->isJump())
        {
            mapRegInst.clear();
        }

        const auto readRegs = out.cpu->getReadRegisters(inst);
        for (const auto r : readRegs)
            mapRegInst.erase(r);

        auto legitReg = CpuReg::Max;
        if (inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadRI ||
            inst->op == ScbeMicroOp::LoadRM ||
            inst->op == ScbeMicroOp::LoadAddressM ||
            inst->op == ScbeMicroOp::SymbolRelocationAddress ||
            inst->op == ScbeMicroOp::LoadZeroExtendRM ||
            inst->op == ScbeMicroOp::LoadSignedExtendRM ||
            inst->op == ScbeMicroOp::ClearR)
        {
            if (mapRegInst.contains(inst->regA))
                ignore(out, mapRegInst[inst->regA]);
            mapRegInst[inst->regA] = inst;
            legitReg               = inst->regA;
        }
        else if (inst->op == ScbeMicroOp::OpBinaryRI)
        {
            mapRegInst[inst->regA] = inst;
            legitReg               = inst->regA;
        }
        else if (inst->isCall())
        {
            for (const auto& [r, i] : mapRegInst)
            {
                if (!inst->cc->paramsRegistersInteger.contains(r) && !inst->cc->paramsRegistersFloat.contains(r))
                {
                    if (inst->cc->volatileRegistersInteger.contains(r) || inst->cc->volatileRegistersFloat.contains(r))
                    {
                        if (inst->op == ScbeMicroOp::CallIndirect && r == inst->regA)
                            continue;
                        ignore(out, i);
                    }
                }
            }
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            if (r != legitReg)
                mapRegInst.erase(r);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::optimizePassDeadHdwRegBeforeLeave(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->flags.has(MIF_JUMP_DEST) || inst->isJump())
            mapRegInst.clear();

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapRegInst | std::views::values)
                ignore(out, i);
            mapRegInst.clear();
        }
        else if (inst->op == ScbeMicroOp::LoadAddressM ||
                 inst->op == ScbeMicroOp::LoadRR ||
                 inst->op == ScbeMicroOp::LoadRI ||
                 inst->op == ScbeMicroOp::LoadZeroExtendRR ||
                 inst->op == ScbeMicroOp::LoadZeroExtendRM ||
                 inst->op == ScbeMicroOp::LoadSignedExtendRR ||
                 inst->op == ScbeMicroOp::LoadSignedExtendRM ||
                 inst->op == ScbeMicroOp::OpBinaryRR ||
                 inst->op == ScbeMicroOp::OpBinaryRI ||
                 inst->op == ScbeMicroOp::SetCC ||
                 inst->op == ScbeMicroOp::OpUnaryR)
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

            mapRegInst.erase(inst->regB);
        }
        else
        {
            const auto readRegs = out.cpu->getReadRegisters(inst);
            for (const auto r : readRegs)
                mapRegInst.erase(r);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

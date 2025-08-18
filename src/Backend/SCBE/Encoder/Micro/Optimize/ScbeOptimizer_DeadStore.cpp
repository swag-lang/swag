#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

namespace
{
    bool isDeadHwdReg(const ScbeMicroInstruction* inst)
    {
        return inst->op == ScbeMicroOp::LoadAddrRM ||
               inst->op == ScbeMicroOp::LoadAmcRM ||
               inst->op == ScbeMicroOp::LoadAddrAmcRM ||
               inst->op == ScbeMicroOp::LoadRR ||
               inst->op == ScbeMicroOp::LoadRI ||
               inst->op == ScbeMicroOp::LoadRM ||
               inst->op == ScbeMicroOp::ClearR ||
               inst->op == ScbeMicroOp::OpBinaryRI ||
               inst->op == ScbeMicroOp::OpBinaryRR ||
               inst->op == ScbeMicroOp::OpBinaryRM ||
               inst->op == ScbeMicroOp::OpUnaryR ||
               inst->op == ScbeMicroOp::LoadCallParam ||
               inst->op == ScbeMicroOp::LoadCallAddrParam ||
               inst->op == ScbeMicroOp::LoadCallZeroExtParam ||
               inst->op == ScbeMicroOp::LoadZeroExtRR ||
               inst->op == ScbeMicroOp::LoadZeroExtRM ||
               inst->op == ScbeMicroOp::LoadSignedExtRR ||
               inst->op == ScbeMicroOp::LoadSignedExtRM;
    }
}

void ScbeOptimizer::optimizePassDeadHdwReg2(const ScbeMicro& out)
{
    auto inst = out.getFirstInstruction();
    while (!inst->isEnd())
    {
        if (isDeadHwdReg(inst))
        {
            if (inst->op == ScbeMicroOp::OpBinaryRI ||
                inst->op == ScbeMicroOp::OpBinaryRR ||
                inst->op == ScbeMicroOp::OpBinaryRM ||
                inst->op == ScbeMicroOp::OpUnaryR)
            {
                if (out.getNextFlagSensitive(inst)->isJumpCond())
                {
                    inst = ScbeMicro::getNextInstruction(inst);
                    continue;
                }
            }

            if (!hasReadRegAfter(out, inst, inst->regA))
            {
                if (!cxt.hasReachedEndOnce || !out.isFuncReturnRegister(inst->regA))
                    ignore(out, inst);
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::optimizePassDeadHdwReg(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (!inst->isEnd())
    {
        if (inst->isRet() || inst->isJump())
            mapRegInst.clear();

        const auto readRegs = out.cpu->getReadRegisters(inst);
        for (const auto r : readRegs)
            mapRegInst.erase(r);

        auto legitReg = CpuReg::Max;
        if (isDeadHwdReg(inst))
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
    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->isJump() || inst->isJumpDest())
            mapRegInst.clear();

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapRegInst | std::views::values)
                ignore(out, i);
            mapRegInst.clear();
        }
        else if (inst->op == ScbeMicroOp::LoadAddrRM ||
                 inst->op == ScbeMicroOp::LoadRR ||
                 inst->op == ScbeMicroOp::LoadRI ||
                 inst->op == ScbeMicroOp::LoadZeroExtRR ||
                 inst->op == ScbeMicroOp::LoadZeroExtRM ||
                 inst->op == ScbeMicroOp::LoadSignedExtRR ||
                 inst->op == ScbeMicroOp::LoadSignedExtRM ||
                 inst->op == ScbeMicroOp::OpBinaryRR ||
                 inst->op == ScbeMicroOp::OpBinaryRI ||
                 inst->op == ScbeMicroOp::OpUnaryR ||
                 inst->op == ScbeMicroOp::ClearR ||
                 inst->op == ScbeMicroOp::SetCondR)
        {
            if (!out.isFuncReturnRegister(inst->regA))
                mapRegInst[inst->regA] = inst;
            mapRegInst.erase(inst->regB);
        }
        else
        {
            const auto readRegs = out.cpu->getReadRegisters(inst);
            for (const auto r : readRegs)
                mapRegInst.erase(r);
        }
    }
}

void ScbeOptimizer::optimizePassDeadRegBeforeLeave(const ScbeMicro& out)
{
    mapValInst.clear();
    for (auto inst = out.getFirstInstruction(); !inst->isEnd(); inst = ScbeMicro::getNextInstruction(inst))
    {
        if (inst->isJump() || inst->isJumpDest())
            mapValInst.clear();

        if (inst->op == ScbeMicroOp::Leave)
        {
            for (const auto& i : mapValInst | std::views::values)
                ignore(out, i);
            mapValInst.clear();
        }
        else
        {
            const auto stackOffset = inst->getStackOffset();
            if (out.cpuFct->isStackOffsetTransient(stackOffset))
            {
                if (aliasStack.contains(stackOffset, sizeof(uint8_t)))
                    mapValInst.erase(stackOffset);
                else if (inst->op == ScbeMicroOp::LoadMR || inst->op == ScbeMicroOp::LoadMI)
                    mapValInst[stackOffset] = inst;
                else
                    mapValInst.erase(stackOffset);
            }
        }
    }
}

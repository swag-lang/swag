#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassAliasRegMem(const ScbeMicro& out)
{
    mapRegInst.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->op == ScbeMicroOp::LoadRM)
        {
            mapRegInst.clear();
            auto next = ScbeMicro::getNextInstruction(inst);
            while (true)
            {
                if (next->flags.has(MIF_JUMP_DEST) ||
                    next->isJump() ||
                    next->isRet())
                {
                    break;
                }

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
                            setRegA(inst, next->regA);
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

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

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

        if (inst->hasReadRegB() && !inst->hasWriteRegB())
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
                         out.cpu->acceptsRegB(inst, prev->regB))
                {
                    setRegB(inst, prev->regB);
                }
            }
        }
        else if (inst->hasReadRegA() && !inst->hasWriteRegA())
        {
            if (mapRegInst.contains(inst->regA))
            {
                const auto prev = mapRegInst[inst->regA];
                if (out.cpu->acceptsRegA(inst, prev->regB))
                {
                    if (inst->hasWriteMemA())
                    {
                        setRegA(inst, prev->regB);
                    }
                    else if (ScbeCpu::isInt(inst->regA) == ScbeCpu::isInt(prev->regB) &&
                             inst->opBitsA == prev->opBitsA)
                    {
                        setRegA(inst, prev->regB);
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
        {
            mapRegInst[inst->regA] = inst;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::optimizePassDeadHdwReg2(const ScbeMicro& out)
{
    auto               inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->op == ScbeMicroOp::LoadAddr ||
            inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadRI ||
            inst->op == ScbeMicroOp::LoadZeroExtRR ||
            inst->op == ScbeMicroOp::LoadZeroExtRM ||
            inst->op == ScbeMicroOp::LoadSignedExtRR ||
            inst->op == ScbeMicroOp::LoadSignedExtRM)
        {
            bool hasRead  = false;
            cxt.startInst = inst;

            const auto valid = explore(cxt, out, [&hasRead](const ScbeMicro& outIn, const ScbeExploreContext& cxtIn) {
                const auto readRegs = outIn.cpu->getReadRegisters(cxtIn.curInst);
                if (readRegs.contains(cxtIn.startInst->regA))
                {
                    hasRead = true;
                    return ScbeExploreReturn::Stop;
                }

                const auto writeRegs = outIn.cpu->getWriteRegisters(cxtIn.curInst);
                if (writeRegs.contains(cxtIn.startInst->regA))
                    return ScbeExploreReturn::Break;

                return ScbeExploreReturn::Continue;
            });

            if (valid && !hasRead)
            {
                if (!cxt.hasReachedEndOnce ||
                    (!out.cpuFct->typeFunc->returnByValue() && !out.cpuFct->typeFunc->returnStructByValue()) ||
                    (inst->regA != out.cc->returnByRegisterInteger && inst->regA != out.cc->returnByRegisterFloat))
                {
                    ignore(out, inst);
                }
            }
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
        if (inst->isRet() || inst->isJump())
            mapRegInst.clear();

        const auto readRegs = out.cpu->getReadRegisters(inst);
        for (const auto r : readRegs)
            mapRegInst.erase(r);

        auto legitReg = CpuReg::Max;
        if (inst->op == ScbeMicroOp::LoadRR ||
            inst->op == ScbeMicroOp::LoadRI ||
            inst->op == ScbeMicroOp::LoadRM ||
            inst->op == ScbeMicroOp::LoadAddr ||
            inst->op == ScbeMicroOp::SymbolRelocAddr ||
            inst->op == ScbeMicroOp::LoadZeroExtRM ||
            inst->op == ScbeMicroOp::LoadSignedExtRM ||
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
        else if (inst->op == ScbeMicroOp::LoadAddr ||
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
                 inst->op == ScbeMicroOp::SetCC)
        {
            if ((!out.cpuFct->typeFunc->returnByValue() && !out.cpuFct->typeFunc->returnStructByValue()) ||
                (inst->regA != out.cc->returnByRegisterInteger && inst->regA != out.cc->returnByRegisterFloat))
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

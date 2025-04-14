#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Main/Statistics.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/Module.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

bool ScbeOptimizer::regToReg(const ScbeMicro& out, CpuReg regDst, CpuReg regSrc, uint32_t firstInst, uint32_t lastInst)
{
    auto inst       = out.getFirstInstruction() + firstInst;
    bool hasChanged = false;
    while (inst != out.getFirstInstruction() + lastInst && inst->op != ScbeMicroOp::End)
    {
        if (inst->hasRegA() && inst->regA == regSrc)
        {
            setRegA(inst, regDst);
            hasChanged = true;
        }

        if (inst->hasRegB() && inst->regB == regSrc)
        {
            setRegB(inst, regDst);
            hasChanged = true;
        }

        if (inst->hasRegC() && inst->regC == regSrc)
        {
            setRegC(inst, regDst);
            hasChanged = true;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }

    return hasChanged;
}

bool ScbeOptimizer::memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg)
{
    auto inst       = out.getFirstInstruction();
    bool hasChanged = false;
    while (inst->op != ScbeMicroOp::End)
    {
        switch (inst->op)
        {
            case ScbeMicroOp::LoadSignedExtRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadSignedExtRR);
                    setRegB(inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadZeroExtRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadZeroExtRR);
                    setRegB(inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRR);
                    setRegB(inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRR);
                    setRegA(inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRI);
                    setRegA(inst, reg);
                    setValueA(inst, inst->valueB);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::CmpMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::CmpRR);
                    setRegA(inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::CmpMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::CmpRI);
                    setRegA(inst, reg);
                    setValueA(inst, inst->valueB);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpUnaryM:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpUnaryR);
                    setRegA(inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpBinaryMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRI);
                    setRegA(inst, reg);
                    setValueA(inst, inst->valueB);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpBinaryMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRR);
                    setRegA(inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpBinaryRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRR);
                    setRegB(inst, reg);
                    hasChanged = true;
                }
                break;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }

    return hasChanged;
}

void ScbeOptimizer::swapInstruction(const ScbeMicro& out, ScbeMicroInstruction* before, ScbeMicroInstruction* after)
{
    if (before->isJumpDest())
    {
        after->flags.add(MIF_JUMP_DEST);
        before->flags.remove(MIF_JUMP_DEST);
    }

    std::swap(*before, *after);
    setDirtyPass();
}

void ScbeOptimizer::ignore(const ScbeMicro& out, ScbeMicroInstruction* inst)
{
#ifdef SWAG_STATS
    g_Stats.totalOptimScbe += 1;
#endif

    inst->op = ScbeMicroOp::Ignore;

    if (inst->isJumpDest())
    {
        const auto next = ScbeMicro::getNextInstruction(inst);
        next->flags.add(MIF_JUMP_DEST);
    }

    inst->flags.clear();
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

void ScbeOptimizer::setValueA(ScbeMicroInstruction* inst, uint64_t value)
{
    if (inst->valueA != value)
    {
        inst->valueA = value;
        setDirtyPass();
    }
}

void ScbeOptimizer::setValueB(ScbeMicroInstruction* inst, uint64_t value)
{
    if (inst->valueB != value)
    {
        inst->valueB = value;
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

void ScbeOptimizer::solveLabels(const ScbeMicro& out)
{
    const auto first = out.getFirstInstruction();
    auto       inst  = first;
    while (inst->op != ScbeMicroOp::End)
    {
        inst->flags.remove(MIF_JUMP_DEST);
        inst++;
    }

    inst     = first;
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->op == ScbeMicroOp::JumpCondI)
        {
            const auto it = out.labels.find(static_cast<uint32_t>(inst->valueA));
            SWAG_ASSERT(it != out.labels.end());
            inst->valueB = it->second;
            auto next    = first + inst->valueB;
            while (next->op == ScbeMicroOp::Label || next->op == ScbeMicroOp::Debug || next->op == ScbeMicroOp::Ignore)
                next++;
            next->flags.add(MIF_JUMP_DEST);
        }
        else if (inst->op == ScbeMicroOp::PatchJump)
        {
            const auto jump = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data + inst->valueA);
            auto       next = ScbeMicro::getNextInstruction(inst);
            while (next->op == ScbeMicroOp::Label || next->op == ScbeMicroOp::Debug || next->op == ScbeMicroOp::Ignore)
                next++;
            jump->valueB = static_cast<uint64_t>(next - first);
            next->flags.add(MIF_JUMP_DEST);
        }
        else if (inst->op == ScbeMicroOp::Label)
        {
            const auto next = ScbeMicro::getNextInstruction(inst);
            next->flags.add(MIF_JUMP_DEST);
        }

        inst++;
    }
}

void ScbeOptimizer::computeContextRegs(const ScbeMicro& out)
{
    usedReadRegs.clear();
    usedWriteRegs.clear();
    unusedVolatileInteger.clear();
    unusedVolatileInteger.append(out.cc->volatileRegistersIntegerSet);
    unusedNonVolatileInteger.clear();
    unusedNonVolatileInteger.append(out.cc->nonVolatileRegistersIntegerSet);

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            usedWriteRegs[r] += 1;
            if (out.cc->volatileRegistersIntegerSet.contains(r))
                unusedVolatileInteger.erase(r);
            if (out.cc->nonVolatileRegistersIntegerSet.contains(r))
                unusedNonVolatileInteger.erase(r);
        }

        const auto readRegs = out.cpu->getReadRegisters(inst);
        for (const auto r : readRegs)
        {
            usedReadRegs[r] += 1;
            if (out.cc->volatileRegistersIntegerSet.contains(r))
                unusedVolatileInteger.erase(r);
            if (out.cc->nonVolatileRegistersIntegerSet.contains(r))
                unusedNonVolatileInteger.erase(r);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

void ScbeOptimizer::computeContextStack(const ScbeMicro& out)
{
    aliasStack.clear();
    usedStack.clear();
    rangeReadStack.clear();
    usedStackRanges.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        const auto stackOffset = inst->getStackOffset();
        if (stackOffset != UINT32_MAX)
        {
            if (inst->op == ScbeMicroOp::LoadAddr)
                aliasStack.push_back(stackOffset);
            else
            {
                const uint32_t size = inst->getNumBytes();
                SWAG_ASSERT(size);
                if (out.cpuFct->isStackOffsetBC(stackOffset) || !usedStack.contains(stackOffset))
                {
                    for (const auto& [r, i] : usedStackRanges)
                    {
                        bool hasAlias = false;
                        if (stackOffset == r && stackOffset + size == r + i)
                            continue;
                        if (r >= stackOffset && r < stackOffset + size)
                            hasAlias = true;
                        else if (stackOffset >= r && stackOffset < r + i)
                            hasAlias = true;
                        if (hasAlias)
                        {
                            aliasStack.push_back(stackOffset, size);
                            aliasStack.push_back(r, i);
                        }
                    }

                    usedStackRanges.push_back({stackOffset, size});
                }
            }

            usedStack[stackOffset] += 1;

            const auto readOffset = inst->getStackOffsetRead();
            if (readOffset != UINT32_MAX)
            {
                const uint32_t size = std::max(inst->getNumBytes(), static_cast<uint32_t>(1));
                rangeReadStack.push_back(readOffset, size);
            }
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

bool ScbeOptimizer::explore(ScbeExploreContext& cxt, const ScbeMicro& out, const std::function<ScbeExploreReturn(const ScbeMicro& out, const ScbeExploreContext&)>& callback)
{
    cxt.done.clear();
    cxt.pending.clear();
    cxt.hasReachedEndOnce = false;

    cxt.done.insert(cxt.startInst);
    cxt.curInst = ScbeMicro::getNextInstruction(cxt.startInst);

    ScbeExploreReturn result = ScbeExploreReturn::Continue;
    while (true)
    {
        if (result == ScbeExploreReturn::Break || cxt.curInst->isRet() || cxt.done.contains(cxt.curInst))
        {
            if (cxt.curInst->isRet())
                cxt.hasReachedEndOnce = true;
            if (cxt.pending.empty())
                break;
            cxt.curInst = cxt.pending.back();
            cxt.pending.pop_back();
        }

        cxt.done.insert(cxt.curInst);

        if (cxt.curInst->isJump())
        {
            if (cxt.curInst->op == ScbeMicroOp::JumpCondI && cxt.curInst->jumpType == CpuCondJump::JUMP)
            {
                cxt.curInst = out.getFirstInstruction() + cxt.curInst->valueB;
                continue;
            }

            if (cxt.curInst->op == ScbeMicroOp::JumpCondI || cxt.curInst->op == ScbeMicroOp::JumpCond)
            {
                const auto destInst = out.getFirstInstruction() + cxt.curInst->valueB;
                cxt.pending.push_back(destInst);
            }

            if (cxt.curInst->op == ScbeMicroOp::JumpTable)
                return false;
        }

        result = callback(out, cxt);
        if (result == ScbeExploreReturn::Stop)
            break;
        if (result == ScbeExploreReturn::Break)
            continue;

        if (cxt.curInst->op != ScbeMicroOp::End)
            cxt.curInst = ScbeMicro::getNextInstruction(cxt.curInst);
    }

    return true;
}

void ScbeOptimizer::optimizeStep1(const ScbeMicro& out)
{
    passHasDoneSomething = false;
    computeContextRegs(out);
    solveLabels(out);

    optimizePassImmediate(out);
    optimizePassReduce(out);
    optimizePassStore(out);
    optimizePassDeadHdwReg(out);
    optimizePassDeadHdwRegBeforeLeave(out);
    optimizePassAliasHdwReg(out);
    optimizePassAliasRegMem(out);
    optimizePassSwap(out);
}

void ScbeOptimizer::optimizeStep2(const ScbeMicro& out)
{
    passHasDoneSomething = false;
    computeContextRegs(out);
    computeContextStack(out);
    solveLabels(out);

    optimizePassDeadRegBeforeLeave(out);
    optimizePassParamsKeepReg(out);
    optimizePassReduce2(out);
    optimizePassStackToHwdReg(out);
}

void ScbeOptimizer::optimizeStep3(const ScbeMicro& out)
{
    passHasDoneSomething = false;
    computeContextRegs(out);
    computeContextStack(out);
    solveLabels(out);
    
    optimizePassDeadHdwReg2(out);
    optimizePassDupHdwReg(out);
    optimizePassMakeVolatile(out);
}

void ScbeOptimizer::optimize(const ScbeMicro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;
    if (!out.cpuFct->bc->sourceFile->module->mustOptimizeBackend(out.cpuFct->bc->node))
        return;

    bool globalChanged = true;
    while (globalChanged)
    {
        globalChanged = false;

        optimizeStep1(out);
        globalChanged = globalChanged || passHasDoneSomething;
        if (passHasDoneSomething)
            continue;

        optimizeStep2(out);
        globalChanged = globalChanged || passHasDoneSomething;
        if (passHasDoneSomething)
            continue;

        optimizeStep3(out);
        globalChanged = globalChanged || passHasDoneSomething;
    }
}

#include "pch.h"
#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Main/Statistics.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"
#include "Wmf/SourceFile.h"

bool ScbeOptimizer::regToReg(const ScbeMicro& out, CpuReg regDst, CpuReg regSrc, uint32_t firstInst, uint32_t lastInst)
{
    // First, we need to determine if all instructions that can change accept the new register
    auto inst = out.getFirstInstruction() + firstInst;
    while (inst != out.getFirstInstruction() + lastInst && !inst->isEnd())
    {
        if (inst->hasRegA() && inst->regA == regSrc && !out.cpu->acceptsRegA(inst, regDst))
            return false;
        if (inst->hasRegB() && inst->regB == regSrc && !out.cpu->acceptsRegB(inst, regDst))
            return false;
        if (inst->hasRegC() && inst->regC == regSrc && !out.cpu->acceptsRegC(inst, regDst))
            return false;
        inst = ScbeMicro::getNextInstruction(inst);
    }

    inst            = out.getFirstInstruction() + firstInst;
    bool hasChanged = false;
    while (inst != out.getFirstInstruction() + lastInst && !inst->isEnd())
    {
        if (inst->hasRegA() && inst->regA == regSrc)
        {
            setRegA(out, inst, regDst);
            hasChanged = true;
        }

        if (inst->hasRegB() && inst->regB == regSrc)
        {
            setRegB(out, inst, regDst);
            hasChanged = true;
        }

        if (inst->hasRegC() && inst->regC == regSrc)
        {
            setRegC(out, inst, regDst);
            hasChanged = true;
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }

    return hasChanged;
}

bool ScbeOptimizer::memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg, ScbeMicroInstruction* start, const ScbeMicroInstruction* end)
{
    auto inst       = start ? start : out.getFirstInstruction();
    bool hasChanged = false;
    while (!inst->isEnd() && inst != end)
    {
        switch (inst->op)
        {
            case ScbeMicroOp::LoadSignedExtRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::LoadSignedExtRR);
                    setRegB(out, inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadZeroExtRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::LoadZeroExtRR);
                    setRegB(out, inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::LoadRR);
                    setRegB(out, inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::LoadRR);
                    setRegA(out, inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::LoadMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::LoadRI);
                    setRegA(out, inst, reg);
                    setValueA(out, inst, inst->valueB);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::CmpMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::CmpRR);
                    setRegA(out, inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::CmpMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::CmpRI);
                    setRegA(out, inst, reg);
                    setValueA(out, inst, inst->valueB);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpUnaryM:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::OpUnaryR);
                    setRegA(out, inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpBinaryMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::OpBinaryRI);
                    setRegA(out, inst, reg);
                    setValueA(out, inst, inst->valueB);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpBinaryMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::OpBinaryRR);
                    setRegA(out, inst, reg);
                    hasChanged = true;
                }
                break;
            case ScbeMicroOp::OpBinaryRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(out, inst, ScbeMicroOp::OpBinaryRR);
                    setRegB(out, inst, reg);
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
    SWAG_ASSERT(!inst->isEnd());
    SWAG_ASSERT(inst->op != ScbeMicroOp::Enter);
    SWAG_ASSERT(inst->op != ScbeMicroOp::Leave);

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

void ScbeOptimizer::setOp(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroOp op)
{
    if (inst->op != op)
    {
        inst->op = op;
        setDirtyPass();
    }
}

void ScbeOptimizer::setOpBitsA(const ScbeMicro& out, ScbeMicroInstruction* inst, OpBits opBits)
{
    if (inst->opBitsA != opBits)
    {
        inst->opBitsA = opBits;
        setDirtyPass();
    }
}

void ScbeOptimizer::setOpBitsB(const ScbeMicro& out, ScbeMicroInstruction* inst, OpBits opBits)
{
    if (inst->opBitsB != opBits)
    {
        inst->opBitsB = opBits;
        setDirtyPass();
    }
}

void ScbeOptimizer::setValueA(const ScbeMicro& out, ScbeMicroInstruction* inst, uint64_t value)
{
    if (inst->valueA != value)
    {
        inst->valueA = value;
        setDirtyPass();
    }
}

void ScbeOptimizer::setValueB(const ScbeMicro& out, ScbeMicroInstruction* inst, uint64_t value)
{
    if (inst->valueB != value)
    {
        inst->valueB = value;
        setDirtyPass();
    }
}

void ScbeOptimizer::setRegA(const ScbeMicro& out, ScbeMicroInstruction* inst, CpuReg reg)
{
    if (inst->regA != reg)
    {
        inst->regA = reg;
        setDirtyPass();
    }
}

void ScbeOptimizer::setRegB(const ScbeMicro& out, ScbeMicroInstruction* inst, CpuReg reg)
{
    if (inst->regB != reg)
    {
        SWAG_ASSERT(!ScbeCpu::isFloat(reg) || (inst->op != ScbeMicroOp::LoadRM &&
            inst->op != ScbeMicroOp::LoadZeroExtRM &&
            inst->op != ScbeMicroOp::LoadSignedExtRM));
        inst->regB = reg;
        setDirtyPass();
    }
}

void ScbeOptimizer::setRegC(const ScbeMicro& out, ScbeMicroInstruction* inst, CpuReg reg)
{
    if (inst->regC != reg)
    {
        inst->regC = reg;
        setDirtyPass();
    }
}

void ScbeOptimizer::solveLabels(const ScbeMicro& out)
{
    const auto first        = out.getFirstInstruction();
    auto       inst         = first;
    bool       hasJumpTable = false;
    while (!inst->isEnd())
    {
        if (inst->op == ScbeMicroOp::JumpTable)
            hasJumpTable = true;
        inst->flags.remove(MIF_JUMP_DEST);
        inst++;
    }

    inst = first;
    while (!inst->isEnd())
    {
        if (inst->op == ScbeMicroOp::JumpCondI)
        {
            const auto it = out.labels.find(static_cast<uint32_t>(inst->valueA));
            SWAG_ASSERT(it != out.labels.end());
            inst->valueB = it->second;
            auto next    = first + inst->valueB;
            while (next->op == ScbeMicroOp::Label || next->op == ScbeMicroOp::Debug || next->op == ScbeMicroOp::Ignore)
            {
                next++;
                inst->valueB++;
            }
            next->flags.add(MIF_JUMP_DEST);
        }
        else if (inst->op == ScbeMicroOp::PatchJump)
        {
            const auto jump = reinterpret_cast<ScbeMicroInstruction*>(out.concat.firstBucket->data + inst->valueA);
            if (jump->isJump())
            {
                auto next = ScbeMicro::getNextInstruction(inst);
                while (next->op == ScbeMicroOp::Label || next->op == ScbeMicroOp::Debug || next->op == ScbeMicroOp::Ignore)
                    next++;
                jump->valueB = static_cast<uint64_t>(next - first);
                next->flags.add(MIF_JUMP_DEST);
            }
            else
            {
                ignore(out, inst);
            }
        }
        else if (inst->op == ScbeMicroOp::Label && hasJumpTable)
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
    unusedNonVolatileInteger.clear();
    unusedVolatileInteger.append(out.cpuFct->cc->volatileRegistersIntegerSet);
    unusedNonVolatileInteger.append(out.cpuFct->cc->nonVolatileRegistersIntegerSet);
    contextFlags.clear();

    auto inst = out.getFirstInstruction();
    while (!inst->isEnd())
    {
        switch (inst->op)
        {
            case ScbeMicroOp::LoadMR:
            case ScbeMicroOp::LoadMI:
                if (inst->regA != CpuReg::Rsp)
                    contextFlags.add(OCF_INDIRECT_STACK_WRITE);
                break;
        }

        const auto writeRegs = out.cpu->getWriteRegisters(inst);
        for (const auto r : writeRegs)
        {
            usedWriteRegs[r] += 1;
            unusedVolatileInteger.erase(r);
            unusedNonVolatileInteger.erase(r);
        }

        const auto readRegs = out.cpu->getReadRegisters(inst);
        for (const auto r : readRegs)
        {
            usedReadRegs[r] += 1;
            unusedVolatileInteger.erase(r);
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

    for (const auto n : out.cpuFct->bc->localVars)
    {
        const auto over = n->resolvedSymbolOverload();
        if (!over)
            continue;
        const auto offset = out.cpuFct->getStackOffsetBC() + over->computedValue.storageOffset;
        aliasStack.push_back(offset, over->typeInfo->sizeOf);
    }

    auto inst = out.getFirstInstruction();
    while (!inst->isEnd())
    {
        const auto stackOffset = inst->getStackOffset();
        if (stackOffset != UINT32_MAX)
        {
            if (inst->op == ScbeMicroOp::LoadAddrRM ||
                inst->op == ScbeMicroOp::LoadAddrAmcRM ||
                inst->op == ScbeMicroOp::LoadAmcMR)
            {
                aliasStack.push_back(stackOffset);
            }

            const uint32_t size = std::max(inst->getNumBytes(), static_cast<uint32_t>(1));
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

            usedStack[stackOffset] += 1;

            const auto readOffset = inst->getStackOffsetRead();
            if (readOffset != UINT32_MAX)
                rangeReadStack.push_back(readOffset, size);
        }

        inst = ScbeMicro::getNextInstruction(inst);
    }
}

bool ScbeOptimizer::exploreAfter(const ScbeMicro& out, ScbeMicroInstruction* inst, const std::function<ScbeExploreReturn(const ScbeMicro& out, const ScbeExploreContext&)>& callback)
{
    cxt.startInst = inst;
    cxt.done.clear();
    cxt.pending.clear();
    cxt.hasReachedEndOnce = false;

    cxt.done.insert(cxt.startInst);
    cxt.curInst = ScbeMicro::getNextInstruction(cxt.startInst);

    auto result = ScbeExploreReturn::Continue;
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

        result = callback(out, cxt);
        if (result == ScbeExploreReturn::Stop)
            break;
        if (result == ScbeExploreReturn::Break)
            continue;

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

        if (!cxt.curInst->isEnd())
            cxt.curInst = ScbeMicro::getNextInstruction(cxt.curInst);
    }

    return true;
}

bool ScbeOptimizer::hasReadRegAfter(const ScbeMicro& out, ScbeMicroInstruction* inst, CpuReg reg)
{
    bool       hasRead = false;
    const auto valid   = exploreAfter(out, inst, [reg, &hasRead](const ScbeMicro& outIn, const ScbeExploreContext& cxtIn) {
        const auto readReg = outIn.cpu->getReadRegisters(cxtIn.curInst);
        if (readReg.contains(reg))
        {
            hasRead = true;
            return ScbeExploreReturn::Stop;
        }

        const auto writeReg = outIn.cpu->getWriteRegisters(cxtIn.curInst);
        if (writeReg.contains(reg))
            return ScbeExploreReturn::Break;
        return ScbeExploreReturn::Continue;
    });

    return !valid || hasRead;
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
    optimizePassAliasLoadRM(out);
    optimizePassAliasLoadRR(out);
    optimizePassAliasLoadExtend(out);
    optimizePassAliasLoadAddrRM(out);
    optimizePassAliasInvLoadRM(out);
    optimizePassAliasSymbolReloc(out);
    optimizePassSwap(out);
    optimizePassReduceBits(out);
    optimizePassStackToHwdRegFlat(out);
}

void ScbeOptimizer::optimizeStep2(const ScbeMicro& out)
{
    passHasDoneSomething = false;
    computeContextRegs(out);
    computeContextStack(out);
    solveLabels(out);

    optimizePassDeadRegBeforeLeave(out);
    optimizePassReduce2(out);
}

void ScbeOptimizer::optimizeStep3(const ScbeMicro& out)
{
    passHasDoneSomething = false;
    computeContextRegs(out);
    computeContextStack(out);
    solveLabels(out);

    optimizePassParamsKeepHwdReg(out);
    optimizePassStackToHwdRegGlobal(out);
    optimizePassDeadHdwReg2(out);
    optimizePassMakeHwdRegVolatile(out);
    optimizePassDeadCode(out);
}

void ScbeOptimizer::optimize(const ScbeMicro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;
    if (!out.cpuFct->bc->sourceFile->module->mustOptimizeBackend(out.cpuFct->bc->node))
        return;
    //if (!out.cpuFct->bc->getPrintName().containsNoCase("day2B1"))
    //    return;

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

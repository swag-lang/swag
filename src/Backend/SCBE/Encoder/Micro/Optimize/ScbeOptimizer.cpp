#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Main/Statistics.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/Module.h"
#include "Wmf/SourceFile.h"

void ScbeOptimizer::memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg)
{
    SWAG_ASSERT(memReg != CpuReg::Rsp || !takeAddressRsp.contains(memOffset));

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        switch (inst->op)
        {
            case ScbeMicroOp::LoadSignedExtendRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadSignedExtendRR);
                    setRegB(inst, reg);
                }
                break;
            case ScbeMicroOp::LoadZeroExtendRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadZeroExtendRR);
                    setRegB(inst, reg);
                }
                break;
            case ScbeMicroOp::LoadRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRR);
                    setRegB(inst, reg);
                }
                break;
            case ScbeMicroOp::LoadMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRR);
                    setRegA(inst, reg);
                }
                break;
            case ScbeMicroOp::LoadMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::LoadRI);
                    setRegA(inst, reg);
                    setValueA(inst, inst->valueB);
                }
                break;
            case ScbeMicroOp::CmpMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::CmpRR);
                    setRegA(inst, reg);
                }
                break;
            case ScbeMicroOp::CmpMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::CmpRI);
                    setRegA(inst, reg);
                    setValueA(inst, inst->valueB);
                }
                break;
            case ScbeMicroOp::OpUnaryM:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpUnaryR);
                    setRegA(inst, reg);
                }
                break;
            case ScbeMicroOp::OpBinaryMI:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRI);
                    setRegA(inst, reg);
                    setValueA(inst, inst->valueB);
                }
                break;
            case ScbeMicroOp::OpBinaryMR:
                if (inst->regA == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRR);
                    setRegA(inst, reg);
                }
                break;
            case ScbeMicroOp::OpBinaryRM:
                if (inst->regB == memReg &&
                    inst->valueA == memOffset)
                {
                    setOp(inst, ScbeMicroOp::OpBinaryRR);
                    setRegB(inst, reg);
                }
                break;
        }

        inst = nextInstruction(inst);
    }
}

void ScbeOptimizer::ignore(ScbeMicroInstruction* inst)
{
#ifdef SWAG_STATS
    g_Stats.totalOptimScbe += 1;
#endif
    inst->op = ScbeMicroOp::Ignore;

    if (inst->flags.has(MIF_JUMP_DEST))
    {
        const auto next = nextInstruction(inst);
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

ScbeMicroInstruction* ScbeOptimizer::nextInstruction(ScbeMicroInstruction* inst)
{
    inst++;
    while (inst->op == ScbeMicroOp::Nop || inst->op == ScbeMicroOp::Label || inst->op == ScbeMicroOp::Debug || inst->op == ScbeMicroOp::Ignore)
        inst++;
    return inst;
}

void ScbeOptimizer::computeContext(const ScbeMicro& out)
{
    takeAddressRsp.clear();
    usedRegs.clear();
    usedStack.clear();
    contextFlags.clear();

    auto inst = out.getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        const auto stackOffset = inst->getStackOffset();
        if (inst->op == ScbeMicroOp::LoadAddressM)
            takeAddressRsp.push_back(stackOffset);
        usedStack[stackOffset] += 1;

        if (inst->hasRegA())
            usedRegs[inst->regA] += 1;
        if (inst->hasRegB())
            usedRegs[inst->regB] += 1;
        if (inst->hasRegC())
            usedRegs[inst->regC] += 1;

        auto details = out.cpu->getWriteRegisters(inst);
        for (auto r : details)
            usedRegs[r] += 1;

        inst = nextInstruction(inst);
    }
}

void ScbeOptimizer::optimize(const ScbeMicro& out)
{
    if (out.optLevel == BuildCfgBackendOptim::O0)
        return;
    if (!out.cpuFct->bc->sourceFile->module->mustOptimizeBackend(out.cpuFct->bc->node))
        return;

    setDirtyPass();
    while (passHasDoneSomething)
    {
        passHasDoneSomething = false;
        computeContext(out);

        optimizePassReduce(out);
        optimizePassStore(out);
        optimizePassDeadStore(out);
        optimizePassDeadRegBeforeLeave(out);
        optimizePassDeadHdwRegBeforeLeave(out);
        optimizePassParams(out);
    }
}

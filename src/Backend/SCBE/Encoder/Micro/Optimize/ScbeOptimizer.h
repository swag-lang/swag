// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/CallConv.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/StackRange.h"

struct ScbeCpu;
struct ScbeMicro;
struct ScbeMicroInstruction;
enum class ScbeMicroOp : uint8_t;
enum class OpBits : uint8_t;

enum class ScbeExploreReturn
{
    Stop,
    Break,
    Continue,
};

struct ScbeExploreContext
{
    ScbeMicroInstruction*               startInst         = nullptr;
    ScbeMicroInstruction*               curInst           = nullptr;
    bool                                hasReachedEndOnce = false;
    Set<ScbeMicroInstruction*>          done;
    VectorNative<ScbeMicroInstruction*> pending;
};

struct ScbeOptimizer
{
    static bool explore(ScbeExploreContext& cxt, const ScbeMicro& out, const std::function<ScbeExploreReturn(const ScbeMicro&, const ScbeExploreContext&)>& callback);

    bool regToReg(const ScbeMicro& out, CpuReg regDst, CpuReg regSrc, uint32_t firstInst = 0, uint32_t lastInst = UINT32_MAX);
    bool memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg);
    void swapInstruction(const ScbeMicro& out, ScbeMicroInstruction* before, ScbeMicroInstruction* after);
    void ignore(const ScbeMicro& out, ScbeMicroInstruction* inst);
    void setOp(ScbeMicroInstruction* inst, ScbeMicroOp op);
    void setValueA(ScbeMicroInstruction* inst, uint64_t value);
    void setValueB(ScbeMicroInstruction* inst, uint64_t value);
    void setRegA(ScbeMicroInstruction* inst, CpuReg reg);
    void setRegB(ScbeMicroInstruction* inst, CpuReg reg);
    void setRegC(ScbeMicroInstruction* inst, CpuReg reg);

    void reduceNoOp(const ScbeMicro& out, ScbeMicroInstruction* inst, const ScbeMicroInstruction* next);
    void reduceLoadRR(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next);
    void reduceOffset(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next);
    void reduceDup(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next);
    void reduceLoadAddress(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next);
    void reduceNext(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next);
    void optimizePassReduce(const ScbeMicro& out);
    void optimizePassReduce2(const ScbeMicro& out);

    void reduceUnusedStack(const ScbeMicro& out, ScbeMicroInstruction* inst, ScbeMicroInstruction* next);
    void optimizePassDeadRegBeforeLeave(const ScbeMicro& out);
    void optimizePassDeadHdwRegBeforeLeave(const ScbeMicro& out);
    void optimizePassMakeVolatile(const ScbeMicro& out);
    void optimizePassDupHdwReg(const ScbeMicro& out);
    void optimizePassDeadHdwReg(const ScbeMicro& out);
    void optimizePassDeadHdwReg2(const ScbeMicro& out);
    void optimizePassStore(const ScbeMicro& out);
    void optimizePassParamsKeepReg(const ScbeMicro& out);
    void optimizePassStackToHwdReg(const ScbeMicro& out);
    void optimizePassImmediate(const ScbeMicro& out);
    void optimizePassSwap(const ScbeMicro& out);
    void optimizePassAliasRegMem(const ScbeMicro& out);
    void optimizePassAliasHdwReg(const ScbeMicro& out);

    static void solveLabels(const ScbeMicro& out);
    void        computeContextRegs(const ScbeMicro& out);
    void        computeContextStack(const ScbeMicro& out);
    void        optimizeStep1(const ScbeMicro& out);
    void        optimizeStep2(const ScbeMicro& out);
    void        optimizeStep3(const ScbeMicro& out);
    void        optimize(const ScbeMicro& out);
    void        setDirtyPass() { passHasDoneSomething = true; }

    ScbeCpu*                                 encoder = nullptr;
    Map<uint64_t, std::pair<CpuReg, OpBits>> mapValReg;
    Map<CpuReg, uint64_t>                    mapRegVal;
    Map<uint64_t, uint64_t>                  mapValVal;
    Map<uint64_t, ScbeMicroInstruction*>     mapValInst;
    Map<CpuReg, ScbeMicroInstruction*>       mapRegInst;
    bool                                     passHasDoneSomething = false;

    // Context
    ScbeExploreContext                    cxt;
    Map<uint32_t, uint32_t>               usedStack;
    Map<CpuReg, uint32_t>                 usedReadRegs;
    Map<CpuReg, uint32_t>                 usedWriteRegs;
    Vector<std::pair<uint32_t, uint32_t>> usedStackRanges;
    StackRange                            aliasStack;
    StackRange                            rangeReadStack;
    RegisterSet                           unusedVolatileInteger;
    RegisterSet                           unusedNonVolatileInteger;
};

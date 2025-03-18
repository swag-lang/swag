// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/CallConv.h"

struct ScbeCpu;
struct ScbeMicro;
struct ScbeMicroInstruction;
enum class ScbeMicroOp : uint8_t;
enum class OpBits : uint8_t;

struct ScbeOptimizer
{
    void                         memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg);
    void                         ignore(ScbeMicroInstruction* inst);
    void                         setOp(ScbeMicroInstruction* inst, ScbeMicroOp op);
    static ScbeMicroInstruction* zap(ScbeMicroInstruction* inst);

    void optimizePassReduce(const ScbeMicro& out);
    void optimizePassStoreToRegBeforeLeave(const ScbeMicro& out);
    void optimizePassStoreToHdwRegBeforeLeave(const ScbeMicro& out);
    void optimizePassDeadStore(const ScbeMicro& out);
    void optimizePassStore(const ScbeMicro& out);

    void optimize(const ScbeMicro& out);
    void setDirtyPass() { passHasDoneSomething = true; }

    ScbeCpu*                                 encoder = nullptr;
    Map<uint64_t, std::pair<CpuReg, OpBits>> mapValReg;
    Map<CpuReg, uint64_t>                    mapRegVal;
    Map<uint64_t, ScbeMicroInstruction*>     mapValInst;
    Map<CpuReg, ScbeMicroInstruction*>       mapRegInst;
    bool                                     passHasDoneSomething = false;
};

// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/CallConv.h"

struct ScbeCpu;
struct ScbeMicro;
struct ScbeMicroInstruction;
enum class ScbeMicroOp : uint8_t;
enum class OpBits : uint8_t;

using ScbeOptContextFlags             = Flags<uint32_t>;
constexpr ScbeOptContextFlags CF_ZERO = 0x00000000;

struct ScbeOptimizer
{
    void                         memToReg(const ScbeMicro& out, CpuReg memReg, uint32_t memOffset, CpuReg reg);
    void                         ignore(ScbeMicroInstruction* inst);
    void                         setOp(ScbeMicroInstruction* inst, ScbeMicroOp op);
    void                         setValueA(ScbeMicroInstruction* inst, uint64_t value);
    void                         setValueB(ScbeMicroInstruction* inst, uint64_t value);
    void                         setRegA(ScbeMicroInstruction* inst, CpuReg reg);
    void                         setRegB(ScbeMicroInstruction* inst, CpuReg reg);
    void                         setRegC(ScbeMicroInstruction* inst, CpuReg reg);
    static ScbeMicroInstruction* nextInstruction(ScbeMicroInstruction* inst);

    void reduceNoOp(const ScbeMicro& out, ScbeMicroInstruction* inst);
    void reduceNext(const ScbeMicro& out, ScbeMicroInstruction* inst);
    void optimizePassReduce(const ScbeMicro& out);
    void optimizePassDeadRegBeforeLeave(const ScbeMicro& out);
    void optimizePassDeadHdwRegBeforeLeave(const ScbeMicro& out);
    void optimizePassDeadStore(const ScbeMicro& out);
    void optimizePassStore(const ScbeMicro& out);
    void optimizePassParams(const ScbeMicro& out);

    void computeContext(const ScbeMicro& out);
    void optimize(const ScbeMicro& out);
    void setDirtyPass() { passHasDoneSomething = true; }

    ScbeCpu*                                 encoder = nullptr;
    Map<uint64_t, std::pair<CpuReg, OpBits>> mapValReg;
    Map<CpuReg, uint64_t>                    mapRegVal;
    Map<uint64_t, ScbeMicroInstruction*>     mapValInst;
    Map<CpuReg, ScbeMicroInstruction*>       mapRegInst;
    bool                                     passHasDoneSomething = false;

    // Context
    VectorNative<uint64_t>  takeAddressRsp;
    Map<CpuReg, uint32_t>   usedRegs;
    Map<uint32_t, uint32_t> usedStack;
    ScbeOptContextFlags     contextFlags = CF_ZERO;
};

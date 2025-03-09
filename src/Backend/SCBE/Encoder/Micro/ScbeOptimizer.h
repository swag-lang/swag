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
    void                         ignore(ScbeMicroInstruction* inst);
    void                         setOp(ScbeMicroInstruction* inst, ScbeMicroOp op);
    static ScbeMicroInstruction* zap(ScbeMicroInstruction* inst);

    void passReduce(const ScbeMicro& out);
    void passStoreToRegBeforeLeave(const ScbeMicro& out);
    void passStoreToHdwRegBeforeLeave(const ScbeMicro& out);
    void passDeadStore(const ScbeMicro& out);
    void passStoreMR(const ScbeMicro& out);

    void optimize(const ScbeMicro& out);

    ScbeCpu*                                 encoder = nullptr;
    Map<uint64_t, std::pair<CpuReg, OpBits>> mapValReg;
    Map<CpuReg, uint64_t>                    mapRegVal;
    Map<uint64_t, ScbeMicroInstruction*>     mapValInst;
    Map<CpuReg, ScbeMicroInstruction*>       mapRegInst;
    bool                                     passHasDoneSomething = false;
};

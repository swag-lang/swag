// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/CallConv.h"

struct SCBECPU;
struct SCBEMicro;
struct SCBEMicroInstruction;
enum class SCBEMicroOp : uint8_t;
enum class OpBits : uint8_t;

struct SCBEOptimizer
{
    void                         ignore(SCBEMicroInstruction* inst);
    void                         setOp(SCBEMicroInstruction* inst, SCBEMicroOp op);
    static SCBEMicroInstruction* zap(SCBEMicroInstruction* inst);

    void passReduce(const SCBEMicro& out);
    void passStoreToRegBeforeLeave(const SCBEMicro& out);
    void passStoreToHdwRegBeforeLeave(const SCBEMicro& out);
    void passDeadStore(const SCBEMicro& out);
    void passStoreMR(const SCBEMicro& out);

    void optimize(const SCBEMicro& out);

    SCBECPU*                                 encoder = nullptr;
    Map<uint64_t, std::pair<CPUReg, OpBits>> mapValReg;
    Map<CPUReg, uint64_t>                    mapRegVal;
    Map<uint64_t, SCBEMicroInstruction*>     mapValInst;
    Map<CPUReg, SCBEMicroInstruction*>       mapRegInst;
    bool                                     passHasDoneSomething = false;
};

// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/CallConv.h"

struct SCBECPU;
struct SCBEMicro;
struct SCBE_MicroInstruction;
enum class SCBE_MicroOp : uint8_t;
enum class OpBits : uint8_t;

struct SCBEOptimizer
{
    void                          ignore(SCBE_MicroInstruction* inst);
    void                          setOp(SCBE_MicroInstruction* inst, SCBE_MicroOp op);
    static SCBE_MicroInstruction* zap(SCBE_MicroInstruction* inst);

    void passReduce(const SCBEMicro& out);
    void passStoreToRegBeforeLeave(const SCBEMicro& out);
    void passStoreToHdwRegBeforeLeave(const SCBEMicro& out);
    void passDeadStore(const SCBEMicro& out);
    void passStoreMR(const SCBEMicro& out);

    void optimize(const SCBEMicro& out);

    SCBECPU*                                encoder = nullptr;
    Map<uint64_t, std::pair<CPUReg, OpBits>> mapValReg;
    Map<CPUReg, uint64_t>                    mapRegVal;
    Map<uint64_t, SCBE_MicroInstruction*>    mapValInst;
    Map<CPUReg, SCBE_MicroInstruction*>      mapRegInst;
    bool                                     passHasDoneSomething = false;
};

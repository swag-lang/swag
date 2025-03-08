// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/CallConv.h"

struct SCBE_CPU;
struct SCBE_Micro;
struct SCBE_MicroInstruction;
enum class SCBE_MicroOp : uint8_t;
enum class OpBits : uint8_t;

struct SCBE_Optimizer
{
    void                          ignore(SCBE_MicroInstruction* inst);
    void                          setOp(SCBE_MicroInstruction* inst, SCBE_MicroOp op);
    static SCBE_MicroInstruction* zap(SCBE_MicroInstruction* inst);

    void passReduce(const SCBE_Micro& out);
    void passStoreToRegBeforeLeave(const SCBE_Micro& out);
    void passStoreMR(const SCBE_Micro& out);

    void optimize(const SCBE_Micro& out);

    SCBE_CPU*                                encoder = nullptr;
    Map<uint64_t, std::pair<CPUReg, OpBits>> mapValReg;
    Map<CPUReg, uint64_t>                    mapRegVal;
    Map<uint64_t, SCBE_MicroInstruction*>    mapValInst;
    bool                                     passHasDoneSomething = false;
};

#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

void ScbeOptimizer::optimizePassParams(const ScbeMicro& out)
{
    for (uint32_t i = 0; i < out.cc->paramByRegisterInteger.size(); ++i)
    {
        if (out.cpuFct->typeFunc->numParamsRegisters() <= i)
            break;
        const auto r = out.cc->paramByRegisterInteger[i];
        if (usedRegs[r] == 1 &&
            (!out.cc->useRegisterFloat || !out.cpuFct->typeFunc->registerIdxToType(i)->isNativeFloat()) &&
            !takeAddressRsp.contains(out.cpuFct->getStackOffsetParam(i)))
        {
            memToReg(out, CpuReg::Rsp, out.cpuFct->getStackOffsetParam(i), r);
        }
    }
}

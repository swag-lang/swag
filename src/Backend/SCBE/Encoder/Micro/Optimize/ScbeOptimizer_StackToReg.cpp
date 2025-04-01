#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"

void ScbeOptimizer::optimizePassParamsToReg(const ScbeMicro& out)
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

void ScbeOptimizer::optimizePassStackToReg(const ScbeMicro& out)
{
    std::vector<std::pair<uint32_t, uint32_t>> vec(usedStack.begin(), usedStack.end());
    std::ranges::sort(vec, [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    for (const auto& offset : vec | std::views::keys)
    {
        if (!out.cpuFct->isStackOffsetLocalParam(offset) && !out.cpuFct->isStackOffsetReg(offset))
            continue;
        if (takeAddressRsp.contains(offset))
            continue;

        for (const auto r : out.cc->volatileRegistersInteger)
        {
            if (usedRegs[r] == 0)
            {
                memToReg(out, CpuReg::Rsp, offset, r);
                return;
            }
        }
    }
}
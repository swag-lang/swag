#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/Module.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassParamsKeepReg(const ScbeMicro& out)
{
    for (uint32_t i = 0; i < out.cc->paramsRegistersInteger.size(); ++i)
    {
        if (out.cpuFct->typeFunc->numParamsRegisters() <= i)
            break;
        const auto r = out.cc->paramsRegistersInteger[i];
        if (usedRegs[r] == 1 &&
            (!out.cc->useRegisterFloat || !out.cpuFct->typeFunc->registerIdxToType(i)->isNativeFloat()) &&
            !takeAddressRsp.contains(out.cpuFct->getStackOffsetParam(i)))
        {
            memToReg(out, CpuReg::Rsp, out.cpuFct->getStackOffsetParam(i), r);
        }
    }
}

void ScbeOptimizer::optimizePassStackToVolatileReg(const ScbeMicro& out)
{
    if (!unusedVolatileInteger.empty() && !usedStack.empty())
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

            const auto r = unusedVolatileInteger.first();
            unusedVolatileInteger.erase(r);
            memToReg(out, CpuReg::Rsp, offset, r);
            return;
        }
    }

    if (!unusedNonVolatileInteger.empty() && !usedStack.empty())
    {
        std::vector<std::pair<uint32_t, uint32_t>> vec(usedStack.begin(), usedStack.end());
        std::ranges::sort(vec, [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        for (const auto& it : vec)
        {
            if (it.second < out.buildParams.buildCfg->backendSCBE.regToRegLimit)
                break;
            if (!out.cpuFct->isStackOffsetLocalParam(it.first) && !out.cpuFct->isStackOffsetReg(it.first))
                continue;
            if (takeAddressRsp.contains(it.first))
                continue;

            const auto r = unusedNonVolatileInteger.first();
            unusedNonVolatileInteger.erase(r);
            memToReg(out, CpuReg::Rsp, it.first, r);
            return;
        }
    }
}

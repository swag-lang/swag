#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/SourceFile.h"
#pragma optimize("", off)

void ScbeOptimizer::optimizePassParamsKeepReg(const ScbeMicro& out)
{
    for (uint32_t i = 0; i < out.cc->paramsRegistersInteger.size(); ++i)
    {
        if (out.cpuFct->typeFunc->numParamsRegisters() <= i)
            break;
        const auto r = out.cc->paramsRegistersInteger[i];
        if (usedWriteRegs.contains(r))
            continue;
        if (out.cpuFct->typeFunc->registerIdxToType(i)->isNativeFloat())
            continue;

        const auto offset = out.cpuFct->getStackOffsetParam(i);
        if (aliasStack.contains(offset))
            continue;

        memToReg(out, CpuReg::Rsp, offset, r);
    }
}

void ScbeOptimizer::optimizePassStackToHwdReg(const ScbeMicro& out)
{
    if (usedStack.empty())
        return;
    if (unusedVolatileInteger.empty() && unusedNonVolatileInteger.empty())
        return;

    std::vector<std::pair<uint32_t, uint32_t>> vec(usedStack.begin(), usedStack.end());
    std::ranges::sort(vec, [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    const bool aliasBC = aliasStack.contains(out.cpuFct->getStackOffsetBC(), out.cpuFct->bc->stackSize);

    if (!unusedVolatileInteger.empty())
    {
        for (const auto& offset : vec | std::views::keys)
        {
            if (!out.cpuFct->isStackOffsetLocalParam(offset) && !out.cpuFct->isStackOffsetReg(offset))
                continue;
            if (aliasStack.contains(offset))
                continue;

            const auto r = unusedVolatileInteger.first();
            if (memToReg(out, CpuReg::Rsp, offset, r))
            {
                unusedVolatileInteger.erase(r);
                return;
            }
        }

        if (!aliasBC)
        {
            for (const auto& offset : vec | std::views::keys)
            {
                if (!out.cpuFct->isStackOffsetBC(offset))
                    continue;

                const auto r = unusedVolatileInteger.first();
                if (memToReg(out, CpuReg::Rsp, offset, r))
                {
                    unusedVolatileInteger.erase(r);
                    return;
                }
            }
        }
    }

    if (!unusedNonVolatileInteger.empty())
    {
        for (const auto& it : vec)
        {
            if (it.second < 2)
                break;
            if (!out.cpuFct->isStackOffsetLocalParam(it.first) && !out.cpuFct->isStackOffsetReg(it.first))
                continue;
            if (aliasStack.contains(it.first))
                continue;

            const auto r = unusedNonVolatileInteger.first();
            if (memToReg(out, CpuReg::Rsp, it.first, r))
            {
                unusedNonVolatileInteger.erase(r);
                return;
            }
        }

        if (!aliasBC)
        {
            for (const auto& offset : vec | std::views::keys)
            {
                if (!out.cpuFct->isStackOffsetBC(offset))
                    continue;

                const auto r = unusedNonVolatileInteger.first();
                if (memToReg(out, CpuReg::Rsp, offset, r))
                {
                    unusedNonVolatileInteger.erase(r);
                    return;
                }
            }
        }
    }
}

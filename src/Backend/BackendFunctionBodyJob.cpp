#include "pch.h"
#include "BackendFunctionBodyJob.h"
#include "Backend/Backend.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Syntax/AstNode.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#include "Main/Statistics.h"
#endif

struct ScbeCpu;
JobResult BackendFunctionBodyJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJobGenFunc};
#endif

    if (firstPass)
    {
        cpuFunc.reserve(byteCodeFunc.size());
        for (const auto one : byteCodeFunc)
        {
            if (one->node && one->node->hasAttribute(ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO | ATTRIBUTE_COMPILER))
                continue;
            // Do not emit a text function if we are not compiling a test executable
            if (one->node && one->node->hasAttribute(ATTRIBUTE_TEST_FUNC) && buildParameters.compileType != Test)
                continue;

            if (!backend->emitFunctionBodyPass0(this, buildParameters, one))
            {
                jobsToAdd.clear();
                return JobResult::ReleaseJob;
            }

            const auto ct              = buildParameters.compileType;
            const auto precompileIndex = buildParameters.precompileIndex;
            auto&      ppCPU           = backend->encoder<ScbeCpu>(ct, precompileIndex);
            cpuFunc.push_back(ppCPU.cpuFct);
        }

        if (!jobsToAdd.empty())
        {
            firstPass = false;
            return JobResult::KeepJobAlive;
        }
    }
    else
    {
        for (const auto one : cpuFunc)
        {
            if (!backend->emitFunctionBodyPass1(one))
                return JobResult::ReleaseJob;
        }
    }

    return JobResult::ReleaseJob;
}

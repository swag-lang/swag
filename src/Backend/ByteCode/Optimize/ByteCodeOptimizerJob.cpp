#include "pch.h"
#include "ByteCodeOptimizerJob.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Main/Statistics.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#endif

JobResult ByteCodeOptimizerJob::execute()
{
#ifdef SWAG_STATS
    Timer tm(&g_Stats.optimBCTime);
#endif

    optContext.module = module;

    bool restart = false;
    for (uint32_t i = startIndex; i < endIndex; i++)
    {
        const auto bc = module->byteCodeFunc[i];
        if (!ByteCodeOptimizer::optimize(optContext, bc, restart))
        {
            if (bc->node && bc->node->hasAttribute(ATTRIBUTE_PRINT_BC))
            {
                constexpr ByteCodePrintOptions opt;
                bc->print(opt);
            }

            return JobResult::ReleaseJob;
        }
    }

    // Restart everything if something has been done during this pass
    if (restart)
        ++module->optimNeedRestart;

    return JobResult::ReleaseJob;
}

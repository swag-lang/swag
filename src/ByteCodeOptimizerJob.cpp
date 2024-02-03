#include "pch.h"
#include "ByteCodeOptimizerJob.h"
#include "ByteCodeOptimizer.h"
#include "Module.h"
#include "Statistics.h"
#include "Timer.h"

JobResult ByteCodeOptimizerJob::execute()
{
#ifdef SWAG_STATS
    Timer tm(&g_Stats.optimBCTime);
#endif

    optContext.module = module;

    bool restart = false;
    for (int i = startIndex; i < endIndex; i++)
    {
        auto bc = module->byteCodeFunc[i];
        if (!ByteCodeOptimizer::optimize(optContext, bc, restart))
        {
            if (bc->node && bc->node->attributeFlags & ATTRIBUTE_PRINT_BC)
            {
                ByteCodePrintOptions opt;
                bc->print(opt);
            }

            return JobResult::ReleaseJob;
        }
    }

    // Restart everything if something has been done during this pass
    if (restart)
        module->optimNeedRestart++;

    return JobResult::ReleaseJob;
}

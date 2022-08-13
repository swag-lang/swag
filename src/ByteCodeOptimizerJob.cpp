#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeOptimizerJob.h"
#include "Module.h"
#include "Timer.h"

#define OPT_PASS(__func)                     \
    optContext.passHasDoneSomething = false; \
    if (!__func(&optContext))                \
        return false;                        \
    optContext.allPassesHaveDoneSomething |= optContext.passHasDoneSomething;

bool ByteCodeOptimizerJob::optimize(ByteCode* bc, bool& restart)
{
    if (!module->mustOptimizeBC(bc->node))
        return true;
    optContext.bc     = bc;
    optContext.module = module;

    while (true)
    {
        ByteCodeOptimizer::setContextFlags(&optContext);
        ByteCodeOptimizer::setJumps(&optContext);
        ByteCodeOptimizer::genTree(&optContext);

        if (optContext.hasError)
            return false;
        optContext.allPassesHaveDoneSomething = false;

        OPT_PASS(ByteCodeOptimizer::optimizePassJumps);
        OPT_PASS(ByteCodeOptimizer::optimizePassDeadCode);
        OPT_PASS(ByteCodeOptimizer::optimizePassImmediate);
        OPT_PASS(ByteCodeOptimizer::optimizePassConst);
        OPT_PASS(ByteCodeOptimizer::optimizePassDupCopyRBRA);
        OPT_PASS(ByteCodeOptimizer::optimizePassDupCopy);
        OPT_PASS(ByteCodeOptimizer::optimizePassRetCopyLocal);
        OPT_PASS(ByteCodeOptimizer::optimizePassRetCopyInline);
        OPT_PASS(ByteCodeOptimizer::optimizePassRetCopyGlobal);
        OPT_PASS(ByteCodeOptimizer::optimizePassReduce);
        OPT_PASS(ByteCodeOptimizer::optimizePassDeadStore);
        OPT_PASS(ByteCodeOptimizer::optimizePassDeadStoreDup);
        OPT_PASS(ByteCodeOptimizer::optimizePassLoop);
        OPT_PASS(ByteCodeOptimizer::optimizePassAlias);
        OPT_PASS(ByteCodeOptimizer::optimizePassSwitch);

        ByteCodeOptimizer::removeNops(&optContext);
        if (!optContext.allPassesHaveDoneSomething)
        {
            OPT_PASS(ByteCodeOptimizer::optimizePassReduceX2);
            ByteCodeOptimizer::removeNops(&optContext);
            if (!optContext.allPassesHaveDoneSomething)
                break;
        }

        restart = true;
    }

    return true;
}

bool ByteCodeOptimizerJob::optimize()
{
    Timer tm(&g_Stats.optimBCTime);

    bool restart = false;
    for (int i = startIndex; i < endIndex; i++)
    {
        auto bc = module->byteCodeFunc[i];
        SWAG_CHECK(optimize(bc, restart));
    }

    // Restart everything if something has been done during this pass
    if (restart)
        module->optimNeedRestart = module->optimNeedRestart + 1;

    return true;
}

JobResult ByteCodeOptimizerJob::execute()
{
    optimize();
    return JobResult::ReleaseJob;
}

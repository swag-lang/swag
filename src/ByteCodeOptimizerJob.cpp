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
    SWAG_RACE_CONDITION_WRITE(bc->raceCond);

    optContext.bc     = bc;
    optContext.module = module;

    if (bc->node && !bc->sanDone && module->mustEmitSafety(bc->node, SAFETY_SANITY))
    {
        bc->sanDone = true;
        ByteCodeOptimizer::setContextFlags(&optContext);
        ByteCodeOptimizer::setJumps(&optContext);
        ByteCodeOptimizer::genTree(&optContext, false);
        SWAG_CHECK(ByteCodeOptimizer::optimizePassSanity(&optContext));
    }

    if (module->mustOptimizeBC(bc->node))
    {
        while (true)
        {
            if (!bc->isEmpty && bc->isDoingNothing())
            {
                bc->isEmpty = true;
                restart     = true;
            }

            if (bc->isEmpty)
                return true;

            ByteCodeOptimizer::setContextFlags(&optContext);
            ByteCodeOptimizer::setJumps(&optContext);
            ByteCodeOptimizer::genTree(&optContext, false);

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
            OPT_PASS(ByteCodeOptimizer::optimizePassSwap);

            ByteCodeOptimizer::removeNops(&optContext);
            if (!optContext.allPassesHaveDoneSomething)
            {
                ByteCodeOptimizer::setJumps(&optContext);
                ByteCodeOptimizer::genTree(&optContext, true);

                OPT_PASS(ByteCodeOptimizer::optimizePassDupBlocks);
                OPT_PASS(ByteCodeOptimizer::optimizePassReduceX2);
                ByteCodeOptimizer::removeNops(&optContext);
                if (!optContext.allPassesHaveDoneSomething)
                    break;
            }

            restart = true;
        }
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
        if (!optimize(bc, restart))
        {
            if (bc->node && bc->node->attributeFlags & ATTRIBUTE_PRINT_BC)
                bc->print();
            return false;
        }
    }

    // Restart everything if something has been done during this pass
    if (restart)
        module->optimNeedRestart++;

    return true;
}

JobResult ByteCodeOptimizerJob::execute()
{
    optimize();
    return JobResult::ReleaseJob;
}

#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOptimizer.h"
#include "ByteCode.h"
#include "ByteCodeOptimizerJob.h"
#include "Module.h"
#include "Timer.h"

thread_local Pool<ByteCodeOptimizerJob> g_Pool_byteCodeOptimizerJob;

ByteCodeOptimizerJob::ByteCodeOptimizerJob()
{
    passes.push_back(ByteCodeOptimizer::optimizePassErr);
    passes.push_back(ByteCodeOptimizer::optimizePassJumps);
    passes.push_back(ByteCodeOptimizer::optimizePassEmptyFct);
    passes.push_back(ByteCodeOptimizer::optimizePassDeadCode);
    passes.push_back(ByteCodeOptimizer::optimizePassImmediate);
    passes.push_back(ByteCodeOptimizer::optimizePassConst);
    passes.push_back(ByteCodeOptimizer::optimizePassDupCopyRBRA);
    passes.push_back(ByteCodeOptimizer::optimizePassDupCopyRA);
    passes.push_back(ByteCodeOptimizer::optimizePassRetCopyLocal);
    passes.push_back(ByteCodeOptimizer::optimizePassRetCopyInline);
    passes.push_back(ByteCodeOptimizer::optimizePassRetCopyGlobal);
    passes.push_back(ByteCodeOptimizer::optimizePassReduce);
    passes.push_back(ByteCodeOptimizer::optimizePassDeadStore);
    passes.push_back(ByteCodeOptimizer::optimizePassLoop);
}

bool ByteCodeOptimizerJob::optimize(bool isAsync)
{
    Timer tm(&g_Stats.optimBCTime);
    tm.start();

    while (true)
    {
        bool restart = false;
        for (int i = startIndex; i < endIndex; i++)
        {
            auto bc = module->byteCodeFunc[i];
            if (!module->mustOptimizeBC(bc->node))
                continue;
            optContext.bc = bc;

            while (true)
            {
                ByteCodeOptimizer::genTree(&optContext);
                ByteCodeOptimizer::setJumps(&optContext);

                if (optContext.hasError)
                    return false;
                optContext.allPassesHaveDoneSomething = false;
                for (auto pass : passes)
                {
                    optContext.passHasDoneSomething = false;
                    pass(&optContext);
                    optContext.allPassesHaveDoneSomething |= optContext.passHasDoneSomething;
                }

                ByteCodeOptimizer::removeNops(&optContext);
                if (!optContext.allPassesHaveDoneSomething)
                    break;

                restart = true;
            }
        }

        // Restart everything if something has been done during this pass
        if (!restart)
            break;
        module->optimNeedRestart = module->optimNeedRestart + 1;
        if (isAsync)
            break;
    }

    tm.stop();
    return true;
}

JobResult ByteCodeOptimizerJob::execute()
{
    optimize(true);
    return JobResult::ReleaseJob;
}

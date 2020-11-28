#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOptimizer.h"
#include "ByteCode.h"
#include "ByteCodeOptimizerJob.h"
#include "Module.h"

thread_local Pool<ByteCodeOptimizerJob> g_Pool_byteCodeOptimizerJob;

bool ByteCodeOptimizerJob::optimize(Module* module, int startIndex, int endIndex, bool isAsync)
{
    vector<function<void(ByteCodeOptContext*)>> passes;
    passes.push_back(ByteCodeOptimizer::optimizePassJumps);
    passes.push_back(ByteCodeOptimizer::optimizePassEmptyFct);
    passes.push_back(ByteCodeOptimizer::optimizePassDeadStore);
    passes.push_back(ByteCodeOptimizer::optimizePassDeadCode);
    passes.push_back(ByteCodeOptimizer::optimizePassStack);
    passes.push_back(ByteCodeOptimizer::optimizePassImmediate);
    passes.push_back(ByteCodeOptimizer::optimizePassConst);
    passes.push_back(ByteCodeOptimizer::optimizePassDupCopyRBRA);
    passes.push_back(ByteCodeOptimizer::optimizePassDupCopyRA);
    passes.push_back(ByteCodeOptimizer::optimizePassRetCopyLocal);
    passes.push_back(ByteCodeOptimizer::optimizePassRetCopyInline);
    passes.push_back(ByteCodeOptimizer::optimizePassRetCopyGlobal);
    passes.push_back(ByteCodeOptimizer::optimizePassReduce);

    ByteCodeOptContext optContext;

    while (true)
    {
        bool restart = false;
        for (int i = startIndex; i < endIndex; i++)
        {
            auto bc = module->byteCodeFunc[i];

            if (!module->mustOptimizeBC(bc->node))
                continue;
            optContext.bc = bc;

            // Get all jumps
            ByteCodeOptimizer::setJumps(&optContext);

            while (true)
            {
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
        if (restart)
            module->optimNeedRestart++;
        else
            break;
        if (isAsync)
            break;
    }

    return true;
}

JobResult ByteCodeOptimizerJob::execute()
{
    optimize(module, startIndex, endIndex, true);
    return JobResult::ReleaseJob;
}

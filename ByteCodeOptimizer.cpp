#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOptimizer.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Stats.h"
#include "Timer.h"

void ByteCodeOptimizer::removeNops(ByteCodeOptContext* context)
{
    if (context->nops.empty())
        return;

    context->allPassesHaveDoneSomething = true;

    auto ip     = context->bc->out;
    auto ipDest = context->bc->out;
    for (uint32_t i = 0; i < context->bc->numInstructions; i++)
    {
        if (ip->op == ByteCodeOp::Nop)
        {
            if (g_CommandLine.stats)
                g_Stats.totalOptimsBC = g_Stats.totalOptimsBC + 1;
            ip++;
            continue;
        }

        if (isJump(ip))
        {
            auto srcJump = (int) (ip - context->bc->out);
            auto dstJump = srcJump + ip->b.s32 + 1;
            for (auto nop : context->nops)
            {
                auto idxNop = (int) (nop - context->bc->out);
                if (srcJump < dstJump && idxNop > srcJump && idxNop < dstJump)
                {
                    ip->b.s32--;
                }
                else if (srcJump > dstJump && idxNop > dstJump && idxNop < srcJump)
                {
                    ip->b.s32++;
                }
            }
        }

        *ipDest++ = *ip++;
    }

    context->bc->numInstructions -= (int) context->nops.size();
    context->nops.clear();
    setJumps(context);
}

void ByteCodeOptimizer::setJumps(ByteCodeOptContext* context)
{
    context->jumps.clear();
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        ip->flags &= ~BCI_START_STMT;
        if (isJump(ip))
            context->jumps.push_back(ip);
    }

    // Mark all instructions which are a jump destination
    for (auto jump : context->jumps)
        jump[jump->b.s32 + 1].flags |= BCI_START_STMT;
}

bool ByteCodeOptimizer::optimize(Module* module)
{
    if (module->buildCfg.byteCodeOptimize == 0)
        return true;
    if (module->hasUnittestError || module->numErrors)
        return true;

    Timer tm(g_Stats.optimBCTime);
    tm.start();

    vector<function<void(ByteCodeOptContext*)>> passes;
    passes.push_back(optimizePassJumps);
    passes.push_back(optimizePassEmptyFct);
    passes.push_back(optimizePassDeadStore);
    passes.push_back(optimizePassDeadCode);
    passes.push_back(optimizePassStack);
    passes.push_back(optimizePassImmediate);
    passes.push_back(optimizePassConst);
    passes.push_back(optimizePassDupCopyRBRA);
    passes.push_back(optimizePassRetCopy);

    ByteCodeOptContext optContext;

    while (true)
    {
        bool restart = false;
        for (auto bc : module->byteCodeFunc)
        {
            if (!module->mustOptimizeBC(bc->node))
                continue;
            optContext.bc = bc;

            // Get all jumps
            setJumps(&optContext);

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

                removeNops(&optContext);
                if (!optContext.allPassesHaveDoneSomething)
                    break;

                restart = true;
            }
        }

        // Restart everything if something has been done during this pass
        if (!restart || module->buildCfg.byteCodeOptimize == 1)
            break;
    }

    tm.stop();
    return true;
}
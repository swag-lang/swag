#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeOptimizerJob.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Stats.h"
#include "Timer.h"
#include "Profile.h"

void ByteCodeOptimizer::setNop(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::Nop)
        return;
    if (context->semContext && ip->op == ByteCodeOp::IncPointer64)
        return;
    auto flags = g_ByteCodeOpFlags[(int) ip->op];
    if (flags & OPFLAG_UNPURE)
        return;
    SWAG_ASSERT(ip->op != ByteCodeOp::End);
    context->passHasDoneSomething = true;
    ip->op                        = ByteCodeOp::Nop;
    context->nops.push_back(ip);
}

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

                // If this is a ordered jump, and there's a nop between the destination jump
                // and the jump, then we need to remove one jump instruction
                // If we jump on a nop, we must NOT decrease by 1 to jump to the following instruction
                if (srcJump < dstJump && idxNop > srcJump && idxNop < dstJump)
                {
                    ip->b.s32--;
                }

                // If this is a back jump, and there's a nop between the destination jump
                // and the jump, then we need to remove one jump instruction
                // If we jump on a nop, we MUST decrease also by 1 to jump to the following instruction
                else if (srcJump > dstJump && idxNop >= dstJump && idxNop < srcJump)
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

bool ByteCodeOptimizer::optimize(Job* job, Module* module, bool& done)
{
    done = true;
    if (module->numTestErrors || module->byteCodeFunc.empty())
        return true;

    done = false;
    if (module->numErrors)
        return false;

    SWAG_PROFILE(PRF_OPT, format("optim BC %s", module->name.c_str()));

    // Determin if we need to restart the whole optim pass because something has been done
    if (module->optimPass == 1)
    {
        if (module->optimNeedRestart.load() > 0)
            module->optimPass = 0;
        else
        {
            done = true;
            return true;
        }
    }

    if (module->optimPass == 0)
    {
        module->optimNeedRestart.store(0);
        auto count     = (int) module->byteCodeFunc.size() / g_Stats.numWorkers;
        count          = max(count, 1);
        count          = min(count, (int) module->byteCodeFunc.size());
        int startIndex = 0;
        while (startIndex < module->byteCodeFunc.size())
        {
            auto newJob          = g_Pool_byteCodeOptimizerJob.alloc();
            newJob->module       = module;
            newJob->startIndex   = startIndex;
            newJob->endIndex     = min(startIndex + count, (int) module->byteCodeFunc.size());
            newJob->dependentJob = job;
            startIndex += count;
            job->jobsToAdd.push_back(newJob);
        }

        module->optimPass = 1;
        done              = false;
        return true;
    }

    return true;
}
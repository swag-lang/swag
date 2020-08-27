#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOptimizer.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeOp.h"
#include "Module.h"

void ByteCodeOptimizer::removeNops(ByteCodeOptContext* context)
{
    if (context->nops.empty())
        return;

    context->passHasDoneSomething = true;

    auto ip     = context->bc->out;
    auto ipDest = context->bc->out;
    for (uint32_t i = 0; i < context->bc->numInstructions; i++)
    {
        if (ip->op != ByteCodeOp::Nop)
        {
            if (ip->op == ByteCodeOp::Jump ||
                ip->op == ByteCodeOp::JumpIfTrue ||
                ip->op == ByteCodeOp::JumpIfFalse ||
                ip->op == ByteCodeOp::JumpIfZero32 ||
                ip->op == ByteCodeOp::JumpIfZero64 ||
                ip->op == ByteCodeOp::JumpIfNotZero32 ||
                ip->op == ByteCodeOp::JumpIfNotZero64)
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

            *ipDest++ = *ip;
        }

        ip++;
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
        if (ip->op == ByteCodeOp::Jump ||
            ip->op == ByteCodeOp::JumpIfTrue ||
            ip->op == ByteCodeOp::JumpIfFalse ||
            ip->op == ByteCodeOp::JumpIfZero32 ||
            ip->op == ByteCodeOp::JumpIfZero64 ||
            ip->op == ByteCodeOp::JumpIfNotZero32 ||
            ip->op == ByteCodeOp::JumpIfNotZero64)
        {
            context->jumps.push_back(ip);
        }
    }
}

void ByteCodeOptimizer::optimize(ByteCodeGenContext* context)
{
    if (!context->bc)
        return;

    auto job    = context->job;
    auto module = job->originalNode->sourceFile->module;
    //if (module->mustOptimizeBC(job->originalNode) < 2)
    //return;
    if (job->originalNode->sourceFile->name == "compiler1847.swg")
        job = job;

    ByteCodeOptContext optContext;
    optContext.bc = context->bc;

    vector<function<void(ByteCodeOptContext*)>> passes;
    passes.push_back(optimizePassJumps);
    passes.push_back(optimizePassDeadCode);

    // Get all jumps
    setJumps(&optContext);

    while (true)
    {
        optContext.allPassesHaveDoneSomething = false;
        for (auto pass : passes)
        {
            optContext.passHasDoneSomething = false;
            pass(&optContext);
            optContext.allPassesHaveDoneSomething |= optContext.passHasDoneSomething;
        }

        if (!optContext.allPassesHaveDoneSomething)
            break;
    }
}
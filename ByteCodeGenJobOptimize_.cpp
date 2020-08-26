#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Context.h"
#include "DiagnosticInfos.h"

static bool optimizeJumps(ByteCode* bc)
{
    bool hasDoneSomething = false;
    auto ip               = bc->out;
    while (ip->op != ByteCodeOp::End)
    {
        if (ip->op == ByteCodeOp::Jump ||
            ip->op == ByteCodeOp::JumpIfTrue ||
            ip->op == ByteCodeOp::JumpIfFalse ||
            ip->op == ByteCodeOp::JumpIfZero32 ||
            ip->op == ByteCodeOp::JumpIfZero64 ||
            ip->op == ByteCodeOp::JumpIfNotZero32 ||
            ip->op == ByteCodeOp::JumpIfNotZero64)
        {
            // Jump to another jump
            auto destIp = ip + ip->b.s32 + 1;
            while (destIp->op == ByteCodeOp::Jump)
            {
                ip->b.s32 += destIp->b.s32 + 1;
                destIp += destIp->b.s32 + 1;
                hasDoneSomething = true;
            }

            // Jump to the next instruction
            if (ip->b.s32 == 0)
            {
                hasDoneSomething = true;
                ip->op           = ByteCodeOp::Nop;
            }

            // Next instruction is a nop, move the jump forward if the jump is positive
            else if (ip[1].op == ByteCodeOp::Nop && ip->b.s32 > 0)
            {
                hasDoneSomething = true;
                std::swap(ip[0], ip[1]);
                ip[1].b.s32--;
            }
        }

        if (ip->op == ByteCodeOp::Jump)
        {
            // Next instruction is a jump to the same target
            if (ip[1].op == ByteCodeOp::Jump && (ip->b.s32 - 1 == ip[1].b.s32))
            {
                hasDoneSomething = true;
                ip->op           = ByteCodeOp::Nop;
            }
        }

        ip++;
    }

    return hasDoneSomething;
}

void ByteCodeGenJob::optimize(ByteCodeGenContext* context)
{
    if (!context->bc)
        return;

    auto job    = context->job;
    auto module = job->originalNode->sourceFile->module;
    //if (module->mustOptimizeBC(job->originalNode) < 2)
    //    return;

    while (optimizeJumps(context->bc)) {}
}
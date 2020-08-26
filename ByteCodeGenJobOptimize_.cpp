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
        if (ip->op == ByteCodeOp::Jump)
        {
            // Jump to another jump
            auto destIp = ip + ip->a.s32 + 1;
            while (destIp->op == ByteCodeOp::Jump)
            {
                ip->a.s32 += destIp->a.s32 + 1;
                destIp += destIp->a.s32 + 1;
                hasDoneSomething = true;
            }

            // Jump to the next instruction
            if (ip->a.s32 == 0)
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

    optimizeJumps(context->bc);
}
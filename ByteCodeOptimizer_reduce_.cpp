#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassReduce(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Testing if a stack pointer is not null is irrelevant. This can happen often because of
        // safety checks, when dereferencing a struct on the stack
        if ((ip[0].op == ByteCodeOp::MakeStackPointer || ip[0].op == ByteCodeOp::GetFromStackParam64) &&
            ip[1].op == ByteCodeOp::TestNotZero64 &&
            ip[0].a.u32 == ip[1].b.u32)
        {
            context->passHasDoneSomething = true;
            ip[1].op                      = ByteCodeOp::SetImmediate32;
            ip[1].b.u32                   = 1;
        }

        // Testing if a pointer is not null is irrelevant if previous instruction has incremented the pointer.
        if (ip[0].op == ByteCodeOp::IncPointer32 &&
            ip[1].op == ByteCodeOp::TestNotZero64 &&
            ip[0].c.u32 == ip[1].b.u32 &&
            (ip[0].flags & BCI_IMM_B) &&
            ip[0].b.u32)
        {
            context->passHasDoneSomething = true;
            ip[1].op                      = ByteCodeOp::SetImmediate32;
            ip[1].b.u32                   = 1;
        }
    }
}

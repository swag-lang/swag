#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassReduce(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // NegBool followed by JumpIfTrue, then transform to JumpIfFalse, and remove
        // the negation
        if (ip->op == ByteCodeOp::NegBool && ip[1].op == ByteCodeOp::JumpIfTrue)
        {
            if (ip->a.u32 == ip[1].a.u32)
            {
                //setNop(context, ip);
                //ip[1].op = ByteCodeOp::JumpIfFalse;
            }
        }

        if (ip->op == ByteCodeOp::NegBool && ip[1].op == ByteCodeOp::JumpIfFalse)
        {
            if (ip->a.u32 == ip[1].a.u32)
            {
                //setNop(context, ip);
                //ip[1].op = ByteCodeOp::JumpIfTrue;
            }
        }

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
        if (ip[0].op == ByteCodeOp::IncPointer64 &&
            ip[1].op == ByteCodeOp::TestNotZero64 &&
            ip[0].c.u32 == ip[1].b.u32 &&
            (ip[0].flags & BCI_IMM_B) &&
            ip[0].b.u32)
        {
            context->passHasDoneSomething = true;
            ip[1].op                      = ByteCodeOp::SetImmediate64;
            ip[1].b.u64                   = 1;
        }

        // MakeStackPointer Reg, ImmB
        // IncPointer64     Reg, Reg, ImmB
        // We can change the offset of the MakeStackPointer and remove the IncPointer
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::IncPointer64 &&
            ip[1].c.u32 == ip[0].a.u32 &&
            ip[1].c.u32 == ip[1].a.u32 &&
            ip[1].flags & BCI_IMM_B)
        {
            ip[0].b.u32 += ip[1].b.u32;
            setNop(context, ip + 1);
        }

        // Remove operators which do nothing
        if ((ip->flags & BCI_IMM_B) && !(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_C) && ip->a.u32 == ip->c.u32)
        {
            switch (ip->op)
            {
            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpPlusU32:
            case ByteCodeOp::BinOpMinusU32:
                if (ip->b.u32 == 0)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusU64:
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusU64:
            case ByteCodeOp::IncPointer64:
            case ByteCodeOp::DecPointer64:
                if (ip->b.u64 == 0)
                    setNop(context, ip);
                break;
            }
        }
    }
}

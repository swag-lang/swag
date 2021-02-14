#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "SourceFile.h"
#include "Module.h"
#include "Log.h"

void ByteCodeOptimizer::optimizePassReduce(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        // Compare to 0
        if (ip->op == ByteCodeOp::CompareOpEqual8 && ip->b.u8 == 0 && ip->flags & BCI_IMM_B)
        {
            if (ip[1].op == ByteCodeOp::JumpIfZero8 && ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                ip[1].op    = ByteCodeOp::JumpIfZero8;
                ip[1].a.u32 = ip->a.u32;
            }
        }

        if (ip->op == ByteCodeOp::CompareOpEqual16 && ip->b.u8 == 0 && ip->flags & BCI_IMM_B)
        {
            if (ip[1].op == ByteCodeOp::JumpIfZero16 && ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                ip[1].op    = ByteCodeOp::JumpIfZero16;
                ip[1].a.u32 = ip->a.u32;
            }
        }

        if (ip->op == ByteCodeOp::CompareOpEqual32 && ip->b.u8 == 0 && ip->flags & BCI_IMM_B)
        {
            if (ip[1].op == ByteCodeOp::JumpIfZero32 && ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                ip[1].op    = ByteCodeOp::JumpIfZero32;
                ip[1].a.u32 = ip->a.u32;
            }
        }

        if (ip->op == ByteCodeOp::CompareOpEqual64 && ip->b.u64 == 0 && ip->flags & BCI_IMM_B)
        {
            if (ip[1].op == ByteCodeOp::JumpIfNotZero8 && ip[1].a.u32 == ip->c.u32)
            {
                setNop(context, ip);
                ip[1].op    = ByteCodeOp::JumpIfZero64;
                ip[1].a.u32 = ip->a.u32;
            }
        }

        // SetAtStackPointer x2
        if (ip[0].op == ByteCodeOp::SetAtStackPointer8 &&
            ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].op    = ByteCodeOp::SetAtStackPointer8x2;
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::SetAtStackPointer16 &&
            ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].op    = ByteCodeOp::SetAtStackPointer16x2;
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::SetAtStackPointer32 &&
            ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].op    = ByteCodeOp::SetAtStackPointer32x2;
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::SetAtStackPointer64 &&
            ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].op    = ByteCodeOp::SetAtStackPointer64x2;
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            if (ip[1].flags & BCI_IMM_B)
                ip[0].flags |= BCI_IMM_D;
            setNop(context, ip + 1);
        }

        // GetFromStack64x2
        if (ip[0].op == ByteCodeOp::GetFromStack64 &&
            ip[1].op == ByteCodeOp::GetFromStack64 &&
            !(ip[1].flags & BCI_START_STMT))
        {
            ip[0].op    = ByteCodeOp::GetFromStack64x2;
            ip[0].c.u64 = ip[1].a.u64;
            ip[0].d.u64 = ip[1].b.u64;
            setNop(context, ip + 1);
        }

        // Useless pop/push
        if (ip[0].op == ByteCodeOp::PopRR &&
            ip[1].op == ByteCodeOp::PushRR)
        {
            setNop(context, ip);
            setNop(context, ip + 1);
        }

        // Useless multi return
        if (ip[0].op == ByteCodeOp::IncSP &&
            ip[1].op == ByteCodeOp::Ret &&
            ip[2].op == ByteCodeOp::IncSP &&
            ip[3].op == ByteCodeOp::Ret)
        {
            setNop(context, ip);
            setNop(context, ip + 1);
        }

        // MakeStackPointer followed by SetAtPointer, replace with SetAtStackPointer, but
        // leave the MakeStackPointer which will be removed later (?) if no more used
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::SetAtPointer8 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[1].op                      = ByteCodeOp::SetAtStackPointer8;
            ip[1].a.u32                   = ip[0].b.u32;
            context->passHasDoneSomething = true;
        }
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::SetAtPointer16 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[1].op                      = ByteCodeOp::SetAtStackPointer16;
            ip[1].a.u32                   = ip[0].b.u32;
            context->passHasDoneSomething = true;
        }
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::SetAtPointer32 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[1].op                      = ByteCodeOp::SetAtStackPointer32;
            ip[1].a.u32                   = ip[0].b.u32;
            context->passHasDoneSomething = true;
        }
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[1].op == ByteCodeOp::SetAtPointer64 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[1].op                      = ByteCodeOp::SetAtStackPointer64;
            ip[1].a.u32                   = ip[0].b.u32;
            context->passHasDoneSomething = true;
        }

        // Occurs when setting a string.
        if (ip[0].op == ByteCodeOp::MakeStackPointer &&
            ip[2].op == ByteCodeOp::IncPointer64 &&
            ip[3].op == ByteCodeOp::SetAtPointer64 &&
            ip[0].a.u32 == ip[2].a.u32 &&
            ip[2].c.u32 == ip[3].a.u32 &&
            ip[2].flags & BCI_IMM_B)
        {
            ip[3].op                      = ByteCodeOp::SetAtStackPointer64;
            ip[3].a.u32                   = ip[0].b.u32 + ip[2].b.u32;
            context->passHasDoneSomething = true;
        }

        // GetFromStack8/16/32 clear the other bits by convention, so no need to
        // have a ClearMaskU64 after
        if (ip[0].op == ByteCodeOp::GetFromStack8 &&
            ip[1].op == ByteCodeOp::ClearMaskU64 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::GetFromStack16 &&
            ip[1].op == ByteCodeOp::ClearMaskU64 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::GetFromStack8 &&
            ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::GetFromStack16 &&
            ip[1].op == ByteCodeOp::ClearMaskU32 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::GetFromStack32 &&
            ip[1].op == ByteCodeOp::ClearMaskU64 &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            setNop(context, ip + 1);
        }

        // Cast to bool, followed by a bool negation : replace with cast to inverse bool
        // and remove the negation
        if (ip[0].op == ByteCodeOp::CastBool8 &&
            ip[1].op == ByteCodeOp::NegBool &&
            !(ip[1].flags & BCI_JUMP_DEST) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[0].op = ByteCodeOp::CastInvBool8;
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::CastBool16 &&
            ip[1].op == ByteCodeOp::NegBool &&
            !(ip[1].flags & BCI_JUMP_DEST) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[0].op = ByteCodeOp::CastInvBool16;
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::CastBool32 &&
            ip[1].op == ByteCodeOp::NegBool &&
            !(ip[1].flags & BCI_JUMP_DEST) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[0].op = ByteCodeOp::CastInvBool32;
            setNop(context, ip + 1);
        }

        if (ip[0].op == ByteCodeOp::CastBool64 &&
            ip[1].op == ByteCodeOp::NegBool &&
            !(ip[1].flags & BCI_JUMP_DEST) &&
            ip[0].a.u32 == ip[1].a.u32)
        {
            ip[0].op = ByteCodeOp::CastInvBool64;
            setNop(context, ip + 1);
        }

        if (ip[1].op == ByteCodeOp::JumpIfNotZero64 && !(ip[1].flags & BCI_IMM_A))
        {
            // Testing if a stack pointer is not null is irrelevant. This can happen often because of
            // safety checks, when dereferencing a struct on the stack
            if ((ip[0].op == ByteCodeOp::MakeStackPointer ||
                 ip[0].op == ByteCodeOp::GetFromStackParam64 ||
                 ip[0].op == ByteCodeOp::CopyRRtoRC) &&
                ip[0].a.u32 == ip[1].a.u32)
            {
                context->passHasDoneSomething = true;
                ip[1].op                      = ByteCodeOp::Jump;
            }

            // Testing if a pointer is not null is irrelevant if previous instruction has incremented the pointer.
            if (ip[0].op == ByteCodeOp::IncPointer64 &&
                ip[0].c.u32 == ip[1].a.u32 &&
                (ip[0].flags & BCI_IMM_B) &&
                ip[0].b.u32)
            {
                context->passHasDoneSomething = true;
                ip[1].op                      = ByteCodeOp::Jump;
            }
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

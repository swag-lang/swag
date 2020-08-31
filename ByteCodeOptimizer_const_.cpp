#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "Log.h"

// Make constant folding. If an instruction is purely constant, then compute the result and replace the instruction
// with that result
void ByteCodeOptimizer::optimizePassConst(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::Nop)
            continue;
        if (ip->op == ByteCodeOp::SetImmediate32 || ip->op == ByteCodeOp::SetImmediate64)
            continue;

        if (ip->flags & BCI_IMM_A && ip->flags & BCI_IMM_B)
        {
            switch (ip->op)
            {
            case ByteCodeOp::CompareOpEqual8:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.u8 == ip->b.u8);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CompareOpEqual16:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.u16 == ip->b.u16);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CompareOpEqual32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.u32 == ip->b.u32);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CompareOpEqual64:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.u64 == ip->b.u64);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;

            case ByteCodeOp::CompareOpLowerU32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.u32 < ip->b.u32);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CompareOpLowerS32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.s32 < ip->b.s32);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;

            case ByteCodeOp::CompareOpGreaterU32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.u32 > ip->b.u32);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CompareOpGreaterS32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = (ip->a.s32 > ip->b.s32);
                ip->a.u32                     = ip->c.u32;
                context->passHasDoneSomething = true;
                break;
            default:
                /*g_Log.lock();
                printf("%s\n", context->bc->callName().c_str());
                context->bc->printInstruction(ip);
                g_Log.unlock();*/
                break;
            }
        }
        else if (ip->flags & BCI_IMM_A)
        {
            switch (ip->op)
            {
            case ByteCodeOp::IntrinsicAssert:
                if (ip->a.u8)
                    setNop(context, ip);
                break;
            }
        }
        else if (ip->flags & BCI_IMM_B)
        {
            switch (ip->op)
            {
            case ByteCodeOp::TestNotZero8:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = ip->b.u8 != 0;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::TestNotZero16:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = ip->b.u16 != 0;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::TestNotZero32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = ip->b.u32 != 0;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::TestNotZero64:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.u32                     = ip->b.u64 != 0;
                context->passHasDoneSomething = true;
                break;
            }
        }
    }
}

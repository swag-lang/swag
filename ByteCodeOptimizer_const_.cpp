#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeRun.h"
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
            default:
                /*g_Log.lock();
            printf("%s\n", context->bc->callName().c_str());
            context->bc->printInstruction(ip);
            g_Log.unlock();*/
                break;
            }
        }
        else if ((ip->flags & BCI_IMM_B) && (ip->flags & BCI_IMM_C))
        {
            switch (ip->op)
            {
            case ByteCodeOp::IntrinsicF32x2:
            {
                Register result;
                context->hasError             = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->b.u32                     = result.u32;
                ip->op                        = ByteCodeOp::SetImmediate32;
                context->passHasDoneSomething = true;
                break;
            }

            case ByteCodeOp::IntrinsicF64x2:
            {
                Register result;
                context->hasError             = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->b.u64                     = result.u64;
                ip->op                        = ByteCodeOp::SetImmediate64;
                context->passHasDoneSomething = true;
                break;
            }
            }
        }
        else if (ip->flags & BCI_IMM_B)
        {
            switch (ip->op)
            {
            case ByteCodeOp::DecrementRA32:
                ip->op = ByteCodeOp::SetImmediate32;
                ip->b.u32 -= 1;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::IncrementRA32:
                ip->op = ByteCodeOp::SetImmediate32;
                ip->b.u32 += 1;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::NegS32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.s32                     = -ip->b.s32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::NegS64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.s64                     = -ip->b.s64;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::NegF32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.f32                     = -ip->b.f32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::NegF64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.f64                     = -ip->b.f64;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::InvertS8:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.s8                      = ~ip->b.s8;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::InvertS16:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.s16                     = ~ip->b.s16;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::InvertS32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.s32                     = ~ip->b.s32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::InvertS64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.s64                     = ~ip->b.s64;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::NegBool:
                ip->op = ByteCodeOp::SetImmediate32;
                ip->b.b ^= 1;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastBool8:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.b                       = ip->b.u8 ? true : false;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastBool16:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.b                       = ip->b.u16 ? true : false;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastBool32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.b                       = ip->b.u32 ? true : false;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastBool64:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.b                       = ip->b.u64 ? true : false;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastS8S16:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.s16                     = ip->b.s8;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastS16S32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.s32                     = ip->b.s16;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastS32S64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.s64                     = ip->b.s32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastS32F32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.f32                     = (float) ip->b.s32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastS64F32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.f32                     = (float) ip->b.s64;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastS64F64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.f64                     = (double) ip->b.s64;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastU32F32:
                ip->op                        = ByteCodeOp::SetImmediate32;
                ip->b.f32                     = (float) ip->b.u32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastU64F64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.f64                     = (float) ip->b.u64;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastF32F64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.f64                     = ip->b.f32;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastF64S64:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.s64                     = (int64_t) ip->b.f64;
                context->passHasDoneSomething = true;
                break;
            case ByteCodeOp::CastF64F32:
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.f32                     = (float) ip->b.f64;
                context->passHasDoneSomething = true;
                break;
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
            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicF32x1:
            {
                Register result;
                context->hasError             = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->b.u32                     = result.u32;
                ip->op                        = ByteCodeOp::SetImmediate32;
                context->passHasDoneSomething = true;
                break;
            }

            case ByteCodeOp::IntrinsicF64x1:
            case ByteCodeOp::IntrinsicS64x1:
            {
                Register result;
                context->hasError             = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->op                        = ByteCodeOp::SetImmediate64;
                ip->b.u64                     = result.u64;
                context->passHasDoneSomething = true;
                break;
            }

            default:
                /*g_Log.lock();
                printf("%s\n", context->bc->callName().c_str());
                context->bc->printInstruction(ip);
                g_Log.unlock();*/
                break;
            }
        }
    }
}

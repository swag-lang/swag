#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeRun.h"
#include "Log.h"

#define OK()                              \
    context->passHasDoneSomething = true; \
    ip->flags &= ~BCI_IMM_A;
#define CMP3(__a, __b) __a < __b ? -1 : (__a > __b ? 1 : 0)

#define BINOP_S32(__op)                     \
    ip->op    = ByteCodeOp::SetImmediate32; \
    ip->b.s32 = ip->a.s32 __op ip->b.s32;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_S64(__op)                     \
    ip->op    = ByteCodeOp::SetImmediate64; \
    ip->b.s64 = ip->a.s64 __op ip->b.s64;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_U32(__op)                     \
    ip->op    = ByteCodeOp::SetImmediate32; \
    ip->b.u64 = ip->a.u32 __op ip->b.u32;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_U64(__op)                     \
    ip->op    = ByteCodeOp::SetImmediate64; \
    ip->b.u64 = ip->a.u64 __op ip->b.u64;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_F32(__op)                     \
    ip->op    = ByteCodeOp::SetImmediate32; \
    ip->b.f32 = ip->a.f32 __op ip->b.f32;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_F64(__op)                     \
    ip->op    = ByteCodeOp::SetImmediate64; \
    ip->b.f64 = ip->a.f64 __op ip->b.f64;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_B(__op)                       \
    ip->op    = ByteCodeOp::SetImmediate32; \
    ip->b.b   = ip->a.b __op ip->b.b;       \
    ip->a.u32 = ip->c.u32;                  \
    OK();

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

        if ((ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
        {
            switch (ip->op)
            {
            case ByteCodeOp::BinOpModuloS32:
                BINOP_S32(%);
                break;
            case ByteCodeOp::BinOpModuloS64:
                BINOP_S64(%);
                break;
            case ByteCodeOp::BinOpModuloU32:
                BINOP_U32(%);
                break;
            case ByteCodeOp::BinOpModuloU64:
                BINOP_U64(%);
                break;
            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpPlusU32:
                BINOP_S32(+);
                break;
            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusU64:
                BINOP_S64(+);
                break;
            case ByteCodeOp::BinOpPlusF32:
                BINOP_F32(+);
                break;
            case ByteCodeOp::BinOpPlusF64:
                BINOP_F64(+);
                break;
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpMinusU32:
                BINOP_S32(-);
                break;
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusU64:
                BINOP_S64(-);
                break;
            case ByteCodeOp::BinOpMinusF32:
                BINOP_F32(-);
                break;
            case ByteCodeOp::BinOpMinusF64:
                BINOP_F64(-);
                break;
            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulU32:
                BINOP_S32(*);
                break;
            case ByteCodeOp::BinOpMulS64:
            case ByteCodeOp::BinOpMulU64:
                BINOP_S64(*);
                break;
            case ByteCodeOp::BinOpMulF32:
                BINOP_F32(*);
                break;
            case ByteCodeOp::BinOpMulF64:
                BINOP_F64(*);
                break;
            case ByteCodeOp::BinOpDivS32:
                BINOP_S32(/);
                break;
            case ByteCodeOp::BinOpDivS64:
                BINOP_S64(/);
                break;
            case ByteCodeOp::BinOpDivU32:
                BINOP_U32(/);
                break;
            case ByteCodeOp::BinOpDivU64:
                BINOP_U64(/);
                break;
            case ByteCodeOp::BinOpDivF32:
                BINOP_F32(/);
                break;
            case ByteCodeOp::BinOpDivF64:
                BINOP_F64(/);
                break;
            case ByteCodeOp::BinOpBitmaskAndS32:
                BINOP_S32(&);
                break;
            case ByteCodeOp::BinOpBitmaskAndS64:
                BINOP_S64(&);
                break;
            case ByteCodeOp::BinOpBitmaskOrS32:
                BINOP_S32(|);
                break;
            case ByteCodeOp::BinOpBitmaskOrS64:
                BINOP_S64(|);
                break;
            case ByteCodeOp::BinOpXorU32:
                BINOP_U32(^);
                break;
            case ByteCodeOp::BinOpXorU64:
                BINOP_U64(^);
                break;
            case ByteCodeOp::BinOpShiftLeftU32:
                BINOP_U32(<<);
                break;
            case ByteCodeOp::BinOpShiftLeftU64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.u64 = ip->a.u64 << ip->b.u32;
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftRightU32:
                BINOP_U32(>>);
                break;
            case ByteCodeOp::BinOpShiftRightU64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.u64 = ip->a.u64 >> ip->b.u32;
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpEqual8:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u8 == ip->b.u8);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpEqual16:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u16 == ip->b.u16);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpEqual32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u32 == ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpEqual64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u64 == ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpNotEqual8:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u8 != ip->b.u8);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpNotEqual16:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u16 != ip->b.u16);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpNotEqual32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u32 != ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpNotEqual64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u64 != ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOp3WayU32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = CMP3(ip->a.u32, ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayU64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = CMP3(ip->a.u64, ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayS32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = CMP3(ip->a.s32, ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayS64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = CMP3(ip->a.s64, ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayF32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = CMP3(ip->a.f32, ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayF64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = CMP3(ip->a.f64, ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpLowerS32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s32 < ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerS64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s64 < ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerU32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u32 < ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerU64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u64 < ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerF32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f32 < ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerF64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f64 < ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpLowerEqS32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s32 <= ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqS64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s64 <= ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqU32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u32 <= ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqU64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u64 <= ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqF32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f32 <= ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqF64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f64 <= ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpGreaterS32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s32 > ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterS64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s64 > ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterU32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u32 > ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterU64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u64 > ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterF32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f32 > ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterF64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f64 > ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpGreaterEqS32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s32 >= ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqS64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.s64 >= ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqU32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u32 >= ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqU64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.u64 >= ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqF32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f32 >= ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqF64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = (ip->a.f64 >= ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            }
        }
        else if ((ip->flags & BCI_IMM_B) && (ip->flags & BCI_IMM_C))
        {
            switch (ip->op)
            {
            case ByteCodeOp::IntrinsicS8x2:
            case ByteCodeOp::IntrinsicS16x2:
            case ByteCodeOp::IntrinsicS32x2:
            case ByteCodeOp::IntrinsicU8x2:
            case ByteCodeOp::IntrinsicU16x2:
            case ByteCodeOp::IntrinsicU32x2:

            case ByteCodeOp::IntrinsicF32x2:
            {
                Register result;
                context->hasError = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->b.u32         = result.u32;
                ip->op            = ByteCodeOp::SetImmediate32;
                OK();
                break;
            }

            case ByteCodeOp::IntrinsicS64x2:
            case ByteCodeOp::IntrinsicU64x2:
            case ByteCodeOp::IntrinsicF64x2:
            {
                Register result;
                context->hasError = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->b.u64         = result.u64;
                ip->op            = ByteCodeOp::SetImmediate64;
                OK();
                break;
            }
            }
        }
        else if (ip->flags & BCI_IMM_B)
        {
            switch (ip->op)
            {
            case ByteCodeOp::LowerZeroToTrue:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->c.s32 < 0 ? true : false;
                OK();
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->c.s32 > 0 ? true : false;
                OK();
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->c.s32 <= 0 ? true : false;
                OK();
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->c.s32 >= 0 ? true : false;
                OK();
                break;

            case ByteCodeOp::IncrementRA32:
                ip->op = ByteCodeOp::SetImmediate32;
                ip->b.u32 += 1;
                OK();
                break;
            case ByteCodeOp::DecrementRA32:
                ip->op = ByteCodeOp::SetImmediate32;
                ip->b.u32 -= 1;
                OK();
                break;
            case ByteCodeOp::IncrementRA64:
                ip->op = ByteCodeOp::SetImmediate64;
                ip->b.u64 += 1;
                OK();
                break;
            case ByteCodeOp::DecrementRA64:
                ip->op = ByteCodeOp::SetImmediate64;
                ip->b.u64 -= 1;
                OK();
                break;

            case ByteCodeOp::NegS32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = -ip->b.s32;
                OK();
                break;
            case ByteCodeOp::NegS64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = -ip->b.s64;
                OK();
                break;
            case ByteCodeOp::NegF32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = -ip->b.f32;
                OK();
                break;
            case ByteCodeOp::NegF64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = -ip->b.f64;
                OK();
                break;
            case ByteCodeOp::InvertU8:
                ip->op   = ByteCodeOp::SetImmediate32;
                ip->b.s8 = ~ip->b.s8;
                OK();
                break;
            case ByteCodeOp::InvertU16:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s16 = ~ip->b.s16;
                OK();
                break;
            case ByteCodeOp::InvertU32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = ~ip->b.s32;
                OK();
                break;
            case ByteCodeOp::InvertU64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = ~ip->b.s64;
                OK();
                break;
            case ByteCodeOp::NegBool:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.b ^ 1;
                OK();
                break;

            case ByteCodeOp::CastBool8:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u8 ? true : false;
                OK();
                break;
            case ByteCodeOp::CastBool16:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u16 ? true : false;
                OK();
                break;
            case ByteCodeOp::CastBool32:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u32 ? true : false;
                OK();
                break;
            case ByteCodeOp::CastBool64:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u64 ? true : false;
                OK();
                break;

            case ByteCodeOp::CastInvBool8:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u8 ? false : true;
                OK();
                break;
            case ByteCodeOp::CastInvBool16:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u16 ? false : true;
                OK();
                break;
            case ByteCodeOp::CastInvBool32:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u32 ? false : true;
                OK();
                break;
            case ByteCodeOp::CastInvBool64:
                ip->op  = ByteCodeOp::SetImmediate32;
                ip->b.b = ip->b.u64 ? false : true;
                OK();
                break;

            case ByteCodeOp::CastS8S16:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s16 = ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS8S32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS8S64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS16S32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.s32 = ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS16S64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS32S64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = ip->b.s32;
                OK();
                break;
            case ByteCodeOp::CastS8F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS16F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS32F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.s32;
                OK();
                break;
            case ByteCodeOp::CastS64F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.s64;
                OK();
                break;
            case ByteCodeOp::CastU8F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.u8;
                OK();
                break;
            case ByteCodeOp::CastU16F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.u16;
                OK();
                break;
            case ByteCodeOp::CastU32F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.u32;
                OK();
                break;
            case ByteCodeOp::CastU64F32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.f32 = (float) ip->b.u64;
                OK();
                break;
            case ByteCodeOp::CastS8F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (double) ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS16F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (double) ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS32F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (double) ip->b.s32;
                OK();
                break;
            case ByteCodeOp::CastS64F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (double) ip->b.s64;
                OK();
                break;
            case ByteCodeOp::CastU8F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (double) ip->b.u8;
                OK();
                break;
            case ByteCodeOp::CastU16F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (double) ip->b.u16;
                OK();
                break;
            case ByteCodeOp::CastU32F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (double) ip->b.u32;
                OK();
                break;
            case ByteCodeOp::CastU64F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = (float) ip->b.u64;
                OK();
                break;
            case ByteCodeOp::CastF32F64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f64 = ip->b.f32;
                OK();
                break;
            case ByteCodeOp::CastF64S64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = (int64_t) ip->b.f64;
                OK();
                break;
            case ByteCodeOp::CastF64F32:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.f32 = (float) ip->b.f64;
                OK();
                break;
            case ByteCodeOp::TestNotZero8:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = ip->b.u8 != 0;
                OK();
                break;
            case ByteCodeOp::TestNotZero16:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = ip->b.u16 != 0;
                OK();
                break;
            case ByteCodeOp::TestNotZero32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = ip->b.u32 != 0;
                OK();
                break;
            case ByteCodeOp::TestNotZero64:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = ip->b.u64 != 0;
                OK();
                break;
            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicF32x1:
            {
                Register result;
                context->hasError = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->b.u32         = result.u32;
                ip->op            = ByteCodeOp::SetImmediate32;
                OK();
                break;
            }

            case ByteCodeOp::IntrinsicF64x1:
            case ByteCodeOp::IntrinsicS64x1:
            {
                Register result;
                context->hasError = !ByteCodeRun::executeMathIntrinsic(context->semContext, ip, result, ip->b, ip->c);
                ip->op            = ByteCodeOp::SetImmediate64;
                ip->b.u64         = result.u64;
                OK();
                break;
            }

            default:
                break;
            }
        }
        else if (ip->flags & BCI_IMM_C)
        {
            switch (ip->op)
            {
            case ByteCodeOp::Add32byVB32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = ip->c.u32 + ip->b.u32;
                OK();
                break;
            case ByteCodeOp::Add64byVB64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.u64 = ip->c.u64 + ip->b.u64;
                OK();
                break;
            case ByteCodeOp::Mul64byVB64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = ip->c.s64 * ip->b.s64;
                OK();
                break;
            case ByteCodeOp::Div64byVB64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.s64 = ip->c.s64 / ip->b.s64;
                OK();
                break;
            case ByteCodeOp::ClearMaskU32:
                ip->op    = ByteCodeOp::SetImmediate32;
                ip->b.u64 = ip->c.u32 & ip->b.u32;
                OK();
                break;
            case ByteCodeOp::ClearMaskU64:
                ip->op    = ByteCodeOp::SetImmediate64;
                ip->b.u64 = ip->c.u64 & ip->b.u64;
                OK();
                break;
            }
        }
    }
}

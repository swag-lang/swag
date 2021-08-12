#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeRun.h"
#include "Math.h"
#include "Log.h"
#include "Diagnostic.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"

#define OK()                              \
    context->passHasDoneSomething = true; \
    ip->flags &= ~BCI_IMM_A;
#define CMP3(__a, __b) __a < __b ? -1 : (__a > __b ? 1 : 0)

#define BINOP_S32(__op)                     \
    SET_OP(ip, ByteCodeOp::SetImmediate32); \
    ip->b.s32 = ip->a.s32 __op ip->b.s32;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_S64(__op)                     \
    SET_OP(ip, ByteCodeOp::SetImmediate64); \
    ip->b.s64 = ip->a.s64 __op ip->b.s64;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_U32(__op)                     \
    SET_OP(ip, ByteCodeOp::SetImmediate32); \
    ip->b.u64 = ip->a.u32 __op ip->b.u32;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_U64(__op)                     \
    SET_OP(ip, ByteCodeOp::SetImmediate64); \
    ip->b.u64 = ip->a.u64 __op ip->b.u64;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_F32(__op)                     \
    SET_OP(ip, ByteCodeOp::SetImmediate32); \
    ip->b.f32 = ip->a.f32 __op ip->b.f32;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_F64(__op)                     \
    SET_OP(ip, ByteCodeOp::SetImmediate64); \
    ip->b.f64 = ip->a.f64 __op ip->b.f64;   \
    ip->a.u32 = ip->c.u32;                  \
    OK();

#define BINOP_B(__op)                       \
    SET_OP(ip, ByteCodeOp::SetImmediate32); \
    ip->b.b   = ip->a.b __op ip->b.b;       \
    ip->a.u32 = ip->c.u32;                  \
    OK();    

// Make constant folding. If an instruction is purely constant, then compute the result and replace the instruction
// with that result
bool ByteCodeOptimizer::optimizePassConst(ByteCodeOptContext* context)
{
    for (auto ip = context->bc->out; ip->op != ByteCodeOp::End; ip++)
    {
        if (ip->op == ByteCodeOp::Nop)
            continue;
        if (ip->op == ByteCodeOp::SetImmediate32 || ip->op == ByteCodeOp::SetImmediate64)
            continue;

        auto node       = ip->node;
        auto sourceFile = node->sourceFile;

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
                SWAG_VERIFY(!addOverflow(node, ip->a.s32, ip->b.s32), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoS32)}));
                BINOP_S32(+);
                break;
            case ByteCodeOp::BinOpPlusU32:
                SWAG_VERIFY(!addOverflow(node, ip->a.u32, ip->b.u32), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoU32)}));
                BINOP_S32(+);
                break;
            case ByteCodeOp::BinOpPlusS64:
                SWAG_VERIFY(!addOverflow(node, ip->a.s64, ip->b.s64), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoS64)}));
                BINOP_S64(+);
                break;
            case ByteCodeOp::BinOpPlusU64:
                SWAG_VERIFY(!addOverflow(node, ip->a.u64, ip->b.u64), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoU64)}));
                BINOP_S64(+);
                break;
            case ByteCodeOp::BinOpPlusF32:
                BINOP_F32(+);
                break;
            case ByteCodeOp::BinOpPlusF64:
                BINOP_F64(+);
                break;

            case ByteCodeOp::BinOpMinusS32:
                SWAG_VERIFY(!subOverflow(node, ip->a.s32, ip->b.s32), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoS32)}));
                BINOP_S32(-);
                break;
            case ByteCodeOp::BinOpMinusU32:
                SWAG_VERIFY(!subOverflow(node, ip->a.u32, ip->b.u32), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoU32)}));
                BINOP_S32(-);
                break;
            case ByteCodeOp::BinOpMinusS64:
                SWAG_VERIFY(!subOverflow(node, ip->a.s64, ip->b.s64), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoS64)}));
                BINOP_S64(-);
                break;
            case ByteCodeOp::BinOpMinusU64:
                SWAG_VERIFY(!subOverflow(node, ip->a.u64, ip->b.u64), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoU64)}));
                BINOP_S64(-);
                break;
            case ByteCodeOp::BinOpMinusF32:
                BINOP_F32(-);
                break;
            case ByteCodeOp::BinOpMinusF64:
                BINOP_F64(-);
                break;

            case ByteCodeOp::BinOpMulS32:
                SWAG_VERIFY(!mulOverflow(node, ip->a.s32, ip->b.s32), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoS32)}));
                BINOP_S32(*);
                break;
            case ByteCodeOp::BinOpMulU32:
                SWAG_VERIFY(!mulOverflow(node, ip->a.u32, ip->b.u32), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoU32)}));
                BINOP_S32(*);
                break;
            case ByteCodeOp::BinOpMulS64:
                SWAG_VERIFY(!mulOverflow(node, ip->a.s64, ip->b.s64), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoS64)}));
                BINOP_S64(*);
                break;
            case ByteCodeOp::BinOpMulU64:
                SWAG_VERIFY(!mulOverflow(node, ip->a.u64, ip->b.u64), context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoU64)}));
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

            case ByteCodeOp::BinOpBitmaskAnd8:
            case ByteCodeOp::BinOpBitmaskAnd16:
            case ByteCodeOp::BinOpBitmaskAnd32:
                BINOP_U32(&);
                break;
            case ByteCodeOp::BinOpBitmaskAnd64:
                BINOP_U64(&);
                break;
            case ByteCodeOp::BinOpBitmaskOr8:
            case ByteCodeOp::BinOpBitmaskOr16:
            case ByteCodeOp::BinOpBitmaskOr32:
                BINOP_U32(|);
                break;
            case ByteCodeOp::BinOpBitmaskOr64:
                BINOP_U64(|);
                break;
            case ByteCodeOp::BinOpXorU32:
            case ByteCodeOp::BinOpXorU8:
                BINOP_U32(^);
                break;
            case ByteCodeOp::BinOpXorU64:
                BINOP_U64(^);
                break;

            case ByteCodeOp::BinOpShiftLeftU8:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftLeft(context, &ip->b, ip->a, ip->b, 8, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftLeftU16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftLeft(context, &ip->b, ip->a, ip->b, 16, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftLeftU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftLeft(context, &ip->b, ip->a, ip->b, 32, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::BinOpShiftLeftU64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ByteCodeRun::executeShiftLeft(context, &ip->b, ip->a, ip->b, 64, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::BinOpShiftRightS8:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 8, true, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftRightS16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 16, true, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftRightS32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 32, true, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftRightS64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 64, true, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::BinOpShiftRightU8:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 8, false, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftRightU16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 16, false, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftRightU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 32, false, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::BinOpShiftRightU64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ByteCodeRun::executeShiftRight(context, &ip->b, ip->a, ip->b, 64, false, ip->flags & BCI_SHIFT_SMALL);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpEqual8:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u8 == ip->b.u8);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpEqual16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u16 == ip->b.u16);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpEqual32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u32 == ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpEqual64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u64 == ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpNotEqual8:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u8 != ip->b.u8);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpNotEqual16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u16 != ip->b.u16);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpNotEqual32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u32 != ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpNotEqual64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u64 != ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOp3WayU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = CMP3(ip->a.u32, ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayU64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = CMP3(ip->a.u64, ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayS32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = CMP3(ip->a.s32, ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayS64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = CMP3(ip->a.s64, ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayF32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = CMP3(ip->a.f32, ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOp3WayF64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = CMP3(ip->a.f64, ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpLowerS32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s32 < ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerS64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s64 < ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u32 < ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerU64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u64 < ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerF32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.f32 < ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerF64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.f64 < ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpLowerEqS32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s32 <= ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqS64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s64 <= ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u32 <= ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqU64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u64 <= ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqF32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.f32 <= ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpLowerEqF64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.f64 <= ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpGreaterS32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s32 > ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterS64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s64 > ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u32 > ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterU64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u64 > ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterF32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.f32 > ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterF64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.f64 > ip->b.f64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;

            case ByteCodeOp::CompareOpGreaterEqS32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s32 >= ip->b.s32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqS64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.s64 >= ip->b.s64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u32 >= ip->b.u32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqU64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.u64 >= ip->b.u64);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqF32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = (ip->a.f32 >= ip->b.f32);
                ip->a.u32 = ip->c.u32;
                OK();
                break;
            case ByteCodeOp::CompareOpGreaterEqF64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
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
                SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, result, ip->b, ip->c));
                ip->b.u32 = result.u32;
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                OK();
                break;
            }

            case ByteCodeOp::IntrinsicS64x2:
            case ByteCodeOp::IntrinsicU64x2:
            case ByteCodeOp::IntrinsicF64x2:
            {
                Register result;
                SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, result, ip->b, ip->c));
                ip->b.u64 = result.u64;
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                OK();
                break;
            }
            }
        }
        else if (ip->flags & BCI_IMM_B)
        {
            switch (ip->op)
            {
            case ByteCodeOp::BinOpBitmaskAnd8:
                if (ip->b.u8 == 0xFF)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA8);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;
            case ByteCodeOp::BinOpBitmaskAnd16:
                if (ip->b.u16 == 0xFFFF)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA16);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;
            case ByteCodeOp::BinOpBitmaskAnd32:
                if (ip->b.u32 == 0xFFFFFFFF)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA32);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;
            case ByteCodeOp::BinOpBitmaskAnd64:
                if (ip->b.u64 == 0xFFFFFFFFFFFFFFFF)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA64);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;
            case ByteCodeOp::BinOpBitmaskOr8:
                if (ip->b.u8 == 0)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA8);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;
            case ByteCodeOp::BinOpBitmaskOr16:
                if (ip->b.u16 == 0)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA16);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;
            case ByteCodeOp::BinOpBitmaskOr32:
                if (ip->b.u32 == 0)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA32);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;
            case ByteCodeOp::BinOpBitmaskOr64:
                if (ip->b.u64 == 0)
                {
                    SET_OP(ip,  ByteCodeOp::CopyRBtoRA64);
                    ip->b.u32 = ip->a.u32;
                    ip->a.u32 = ip->c.u32;
                    ip->flags &= ~BCI_IMM_B;
                }
                break;

            case ByteCodeOp::LowerZeroToTrue:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->c.s32 < 0 ? true : false;
                OK();
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->c.s32 > 0 ? true : false;
                OK();
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->c.s32 <= 0 ? true : false;
                OK();
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->c.s32 >= 0 ? true : false;
                OK();
                break;

            case ByteCodeOp::IncrementRA32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u32 += 1;
                OK();
                break;
            case ByteCodeOp::DecrementRA32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u32 -= 1;
                OK();
                break;
            case ByteCodeOp::IncrementRA64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.u64 += 1;
                OK();
                break;
            case ByteCodeOp::DecrementRA64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.u64 -= 1;
                OK();
                break;

            case ByteCodeOp::NegS32:
                if (ip->b.s32 == INT32_MIN && sourceFile->module->mustEmitSafetyOF(node))
                    return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::Neg, g_TypeMgr->typeInfoS32)});
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = -ip->b.s32;
                OK();
                break;
            case ByteCodeOp::NegS64:
                if (ip->b.s64 == INT64_MIN && sourceFile->module->mustEmitSafetyOF(node))
                    return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::Neg, g_TypeMgr->typeInfoS64)});
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.s64 = -ip->b.s64;
                OK();
                break;
            case ByteCodeOp::NegF32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = -ip->b.f32;
                OK();
                break;
            case ByteCodeOp::NegF64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = -ip->b.f64;
                OK();
                break;
            case ByteCodeOp::InvertU8:
                SET_OP(ip,  ByteCodeOp::SetImmediate32);
                ip->b.s8 = ~ip->b.s8;
                OK();
                break;
            case ByteCodeOp::InvertU16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s16 = ~ip->b.s16;
                OK();
                break;
            case ByteCodeOp::InvertU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = ~ip->b.s32;
                OK();
                break;
            case ByteCodeOp::InvertU64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.s64 = ~ip->b.s64;
                OK();
                break;
            case ByteCodeOp::NegBool:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->b.b ^ 1;
                OK();
                break;

            case ByteCodeOp::CastBool8:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->b.u8 ? true : false;
                OK();
                break;
            case ByteCodeOp::CastBool16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->b.u16 ? true : false;
                OK();
                break;
            case ByteCodeOp::CastBool32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->b.u32 ? true : false;
                OK();
                break;
            case ByteCodeOp::CastBool64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.b = ip->b.u64 ? true : false;
                OK();
                break;

            case ByteCodeOp::CastS8S16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s16 = ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS8S32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS8S64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.s64 = ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS16S32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.s32 = ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS16S64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.s64 = ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS32S64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.s64 = ip->b.s32;
                OK();
                break;
            case ByteCodeOp::CastS8F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS16F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS32F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.s32;
                OK();
                break;
            case ByteCodeOp::CastS64F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.s64;
                OK();
                break;
            case ByteCodeOp::CastU8F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.u8;
                OK();
                break;
            case ByteCodeOp::CastU16F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.u16;
                OK();
                break;
            case ByteCodeOp::CastU32F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.u32;
                OK();
                break;
            case ByteCodeOp::CastU64F32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.f32 = (float) ip->b.u64;
                OK();
                break;
            case ByteCodeOp::CastS8F64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = (double) ip->b.s8;
                OK();
                break;
            case ByteCodeOp::CastS16F64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = (double) ip->b.s16;
                OK();
                break;
            case ByteCodeOp::CastS32F64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = (double) ip->b.s32;
                OK();
                break;
            case ByteCodeOp::CastS64F64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = (double) ip->b.s64;
                OK();
                break;
            case ByteCodeOp::CastU8F64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = (double) ip->b.u8;
                OK();
                break;
            case ByteCodeOp::CastU16F64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = (double) ip->b.u16;
                OK();
                break;
            case ByteCodeOp::CastU32F64:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.f64 = (double) ip->b.u32;
                OK();
                break;
            case ByteCodeOp::CastU64F64:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.f64 = (float) ip->b.u64;
                OK();
                break;
            case ByteCodeOp::CastF32F64:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.f64 = ip->b.f32;
                OK();
                break;
            case ByteCodeOp::CastF64S64:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.s64 = (int64_t) ip->b.f64;
                OK();
                break;
            case ByteCodeOp::CastF64F32:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.f32 = (float) ip->b.f64;
                OK();
                break;
            case ByteCodeOp::TestNotZero8:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = ip->b.u8 != 0;
                OK();
                break;
            case ByteCodeOp::TestNotZero16:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = ip->b.u16 != 0;
                OK();
                break;
            case ByteCodeOp::TestNotZero32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = ip->b.u32 != 0;
                OK();
                break;
            case ByteCodeOp::TestNotZero64:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = ip->b.u64 != 0;
                OK();
                break;
            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicF32x1:
            {
                Register result;
                SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, result, ip->b, ip->c));
                ip->b.u32 = result.u32;
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                OK();
                break;
            }

            case ByteCodeOp::IntrinsicF64x1:
            case ByteCodeOp::IntrinsicS64x1:
            {
                Register result;
                SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, result, ip->b, ip->c));
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.u64 = result.u64;
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
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = ip->c.u32 + ip->b.u32;
                OK();
                break;
            case ByteCodeOp::Add64byVB64:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.u64 = ip->c.u64 + ip->b.u64;
                OK();
                break;
            case ByteCodeOp::Mul64byVB64:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.s64 = ip->c.s64 * ip->b.s64;
                OK();
                break;
            case ByteCodeOp::Div64byVB64:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.s64 = ip->c.s64 / ip->b.s64;
                OK();
                break;
            case ByteCodeOp::ClearMaskU32:
                SET_OP(ip, ByteCodeOp::SetImmediate32);
                ip->b.u64 = ip->c.u32 & ip->b.u32;
                OK();
                break;
            case ByteCodeOp::ClearMaskU64:
                SET_OP(ip,  ByteCodeOp::SetImmediate64);
                ip->b.u64 = ip->c.u64 & ip->b.u64;
                OK();
                break;
            }
        }
    }

    return true;
}

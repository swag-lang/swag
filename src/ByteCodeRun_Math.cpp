#include "pch.h"
#include "ByteCodeRun.h"
#include "Diagnostic.h"
#include "ByteCode.h"
#include "Context.h"
#include "ErrorIds.h"

void ByteCodeRun::executeLeftShift(Register* rdest, const Register& rleft, const Register& rright, uint32_t numBits, bool isSigned)
{
    auto shift = rright.u32;
    if (shift >= numBits)
        rdest->u64 = 0;
    else
        rdest->u64 = rleft.u64 << shift;
}

void ByteCodeRun::executeRightShift(Register* rdest, const Register& rleft, const Register& rright, uint32_t numBits, bool isSigned)
{
    auto shift = rright.u32;
    if (shift >= numBits)
    {
        if (isSigned)
        {
            switch (numBits)
            {
            case 8:
                rdest->s64 = rleft.s8 < 0 ? -1 : 0;
                break;
            case 16:
                rdest->s64 = rleft.s16 < 0 ? -1 : 0;
                break;
            case 32:
                rdest->s64 = rleft.s32 < 0 ? -1 : 0;
                break;
            case 64:
                rdest->s64 = rleft.s64 < 0 ? -1 : 0;
                break;
            }
        }
        else
        {
            rdest->u64 = 0;
        }
    }
    else if (isSigned)
    {
        switch (numBits)
        {
        case 8:
            rdest->s64 = rleft.s8 >> shift;
            break;
        case 16:
            rdest->s64 = rleft.s16 >> shift;
            break;
        case 32:
            rdest->s64 = rleft.s32 >> shift;
            break;
        case 64:
            rdest->s64 = rleft.s64 >> shift;
            break;
        }
    }
    else
    {
        switch (numBits)
        {
        case 8:
            rdest->u64 = rleft.u8 >> shift;
            break;
        case 16:
            rdest->u64 = rleft.u16 >> shift;
            break;
        case 32:
            rdest->u64 = rleft.u32 >> shift;
            break;
        case 64:
            rdest->u64 = rleft.u64 >> shift;
            break;
        }
    }
}

bool ByteCodeRun::executeMathIntrinsic(JobContext* context, ByteCodeInstruction* ip, Register& ra, const Register& rb, const Register& rc, const Register& rd, bool runtime)
{
    switch (ip->op)
    {
    case ByteCodeOp::IntrinsicMulAddF32:
        ra.f32 = (rb.f32 * rc.f32) + rd.f32;
        break;
    case ByteCodeOp::IntrinsicMulAddF64:
        ra.f64 = (rb.f64 * rc.f64) + rd.f64;
        break;

    case ByteCodeOp::IntrinsicS8x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicAbs:
            ra.s8 = (int8_t) abs(rb.s8);
            if (!runtime && rb.s8 == INT8_MIN)
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0605), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0091), (int64_t) rb.s8));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicBitCountNz:
            ra.u8 = OS::bitcountnz(rb.u8);
            break;
        case TokenId::IntrinsicBitCountTz:
            ra.u8 = OS::bitcounttz(rb.u8);
            break;
        case TokenId::IntrinsicBitCountLz:
            ra.u8 = OS::bitcountlz(rb.u8);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicS16x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicAbs:
            ra.s16 = (int16_t) abs(rb.s16);
            if (!runtime && rb.s16 == INT16_MIN)
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0605), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0091), (int64_t) rb.s16));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicBitCountNz:
            ra.u16 = OS::bitcountnz(rb.u16);
            break;
        case TokenId::IntrinsicBitCountTz:
            ra.u16 = OS::bitcounttz(rb.u16);
            break;
        case TokenId::IntrinsicBitCountLz:
            ra.u16 = OS::bitcountlz(rb.u16);
            break;
        case TokenId::IntrinsicByteSwap:
            ra.u16 = OS::byteswap(rb.u16);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicS32x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicAbs:
            ra.s32 = abs(rb.s32);
            if (!runtime && rb.s32 == INT32_MIN)
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0605), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0091), (int64_t) rb.s32));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicBitCountNz:
            ra.u32 = OS::bitcountnz(rb.u32);
            break;
        case TokenId::IntrinsicBitCountTz:
            ra.u32 = OS::bitcounttz(rb.u32);
            break;
        case TokenId::IntrinsicBitCountLz:
            ra.u32 = OS::bitcountlz(rb.u32);
            break;
        case TokenId::IntrinsicByteSwap:
            ra.u32 = OS::byteswap(rb.u32);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicS64x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicAbs:
            ra.s64 = abs(rb.s64);
            if (!runtime && rb.s64 == INT64_MIN)
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0605), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0091), rb.s64));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicBitCountNz:
            ra.u64 = OS::bitcountnz(rb.u64);
            break;
        case TokenId::IntrinsicBitCountTz:
            ra.u64 = OS::bitcounttz(rb.u64);
            break;
        case TokenId::IntrinsicBitCountLz:
            ra.u64 = OS::bitcountlz(rb.u64);
            break;
        case TokenId::IntrinsicByteSwap:
            ra.u64 = OS::byteswap(rb.u64);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicS8x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.s8 = min(rb.s8, rc.s8);
            break;
        case TokenId::IntrinsicMax:
            ra.s8 = max(rb.s8, rc.s8);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicS16x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.s16 = min(rb.s16, rc.s16);
            break;
        case TokenId::IntrinsicMax:
            ra.s16 = max(rb.s16, rc.s16);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicS32x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.s32 = min(rb.s32, rc.s32);
            break;
        case TokenId::IntrinsicMax:
            ra.s32 = max(rb.s32, rc.s32);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicS64x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.s64 = min(rb.s64, rc.s64);
            break;
        case TokenId::IntrinsicMax:
            ra.s64 = max(rb.s64, rc.s64);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicU8x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.u8 = min(rb.u8, rc.u8);
            break;
        case TokenId::IntrinsicMax:
            ra.u8 = max(rb.u8, rc.u8);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicU16x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.u16 = min(rb.u16, rc.u16);
            break;
        case TokenId::IntrinsicMax:
            ra.u16 = max(rb.u16, rc.u16);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicU32x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.u32 = min(rb.u32, rc.u32);
            break;
        case TokenId::IntrinsicMax:
            ra.u32 = max(rb.u32, rc.u32);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicU64x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicMin:
            ra.u64 = min(rb.u64, rc.u64);
            break;
        case TokenId::IntrinsicMax:
            ra.u64 = max(rb.u64, rc.u64);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicF32x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicPow:
            ra.f32 = powf(rb.f32, rc.f32);
            if (!runtime && isnan(ra.f32))
            {
                Diagnostic diag{ip->node, ip->node->token, Fmt(Err(Err0417), rb.f32, rc.f32), Hnt(Hnt0001)};
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicMin:
            ra.f32 = min(rb.f32, rc.f32);
            break;
        case TokenId::IntrinsicMax:
            ra.f32 = max(rb.f32, rc.f32);
            break;
        case TokenId::IntrinsicATan2:
            ra.f32 = atan2f(rb.f32, rc.f32);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicF64x2:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicPow:
            ra.f64 = pow(rb.f64, rc.f64);
            if (!runtime && isnan(ra.f64))
            {
                Diagnostic diag{ip->node, ip->node->token, Fmt(Err(Err0417), rb.f64, rc.f64), Hnt(Hnt0001)};
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicMin:
            ra.f64 = min(rb.f64, rc.f64);
            break;
        case TokenId::IntrinsicMax:
            ra.f64 = max(rb.f64, rc.f64);
            break;
        case TokenId::IntrinsicATan2:
            ra.f64 = atan2(rb.f64, rc.f64);
            break;

        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicF32x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicSin:
            ra.f32 = sinf(rb.f32);
            break;
        case TokenId::IntrinsicCos:
            ra.f32 = cosf(rb.f32);
            break;
        case TokenId::IntrinsicTan:
            ra.f32 = tanf(rb.f32);
            break;
        case TokenId::IntrinsicSinh:
            ra.f32 = sinhf(rb.f32);
            break;
        case TokenId::IntrinsicCosh:
            ra.f32 = coshf(rb.f32);
            break;
        case TokenId::IntrinsicTanh:
            ra.f32 = tanhf(rb.f32);
            break;
        case TokenId::IntrinsicATan:
            ra.f32 = atanf(rb.f32);
            break;
        case TokenId::IntrinsicFloor:
            ra.f32 = floorf(rb.f32);
            break;
        case TokenId::IntrinsicCeil:
            ra.f32 = ceilf(rb.f32);
            break;
        case TokenId::IntrinsicTrunc:
            ra.f32 = truncf(rb.f32);
            break;
        case TokenId::IntrinsicRound:
            ra.f32 = roundf(rb.f32);
            break;
        case TokenId::IntrinsicAbs:
            ra.f32 = abs(rb.f32);
            break;
        case TokenId::IntrinsicExp:
            ra.f32 = expf(rb.f32);
            break;
        case TokenId::IntrinsicExp2:
            ra.f32 = exp2f(rb.f32);
            break;

        case TokenId::IntrinsicSqrt:
            ra.f32 = sqrtf(rb.f32);
            if (!runtime && (rb.f32 < 0 || isnan(ra.f32)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0425), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f32));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicASin:
            ra.f32 = asinf(rb.f32);
            if (!runtime && (rb.f32 < -1 || rb.f32 > 1 || isnan(ra.f32)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0426), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f32));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicACos:
            ra.f32 = acosf(rb.f32);
            if (!runtime && (rb.f32 < -1 || rb.f32 > 1 || isnan(ra.f32)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0427), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f32));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicLog:
            ra.f32 = log(rb.f32);
            if (!runtime && (rb.f32 <= 0 || isnan(ra.f32)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0428), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f32));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicLog2:
            ra.f32 = log2(rb.f32);
            if (!runtime && (rb.f32 <= 0 || isnan(ra.f32)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0423), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f32));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicLog10:
            ra.f32 = log10(rb.f32);
            if (!runtime && (rb.f32 <= 0 || isnan(ra.f32)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0424), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f32));
                return context->report(diag);
            }
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::IntrinsicF64x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicSin:
            ra.f64 = sin(rb.f64);
            break;
        case TokenId::IntrinsicCos:
            ra.f64 = cos(rb.f64);
            break;
        case TokenId::IntrinsicTan:
            ra.f64 = tan(rb.f64);
            break;
        case TokenId::IntrinsicSinh:
            ra.f64 = sinh(rb.f64);
            break;
        case TokenId::IntrinsicCosh:
            ra.f64 = cosh(rb.f64);
            break;
        case TokenId::IntrinsicTanh:
            ra.f64 = tanh(rb.f64);
            break;
        case TokenId::IntrinsicATan:
            ra.f64 = atan(rb.f64);
            break;
        case TokenId::IntrinsicFloor:
            ra.f64 = floor(rb.f64);
            break;
        case TokenId::IntrinsicCeil:
            ra.f64 = ceil(rb.f64);
            break;
        case TokenId::IntrinsicTrunc:
            ra.f64 = trunc(rb.f64);
            break;
        case TokenId::IntrinsicRound:
            ra.f64 = round(rb.f64);
            break;
        case TokenId::IntrinsicAbs:
            ra.f64 = fabs(rb.f64);
            break;
        case TokenId::IntrinsicExp:
            ra.f64 = exp(rb.f64);
            break;
        case TokenId::IntrinsicExp2:
            ra.f64 = exp2(rb.f64);
            break;

        case TokenId::IntrinsicSqrt:
            ra.f64 = sqrt(rb.f64);
            if (!runtime && (rb.f64 < 0 || isnan(ra.f64)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0425), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f64));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicASin:
            ra.f64 = asin(rb.f64);
            if (!runtime && (rb.f64 < -1 || rb.f64 > 1 || isnan(ra.f64)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0426), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f64));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicACos:
            ra.f64 = acos(rb.f64);
            if (!runtime && (rb.f64 < -1 || rb.f64 > 1 || isnan(ra.f64)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0427), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f64));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicLog:
            ra.f64 = log(rb.f64);
            if (!runtime && (rb.f64 <= 0 || isnan(ra.f64)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0428), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f64));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicLog2:
            ra.f64 = log2(rb.f64);
            if (!runtime && (rb.f64 <= 0 || isnan(ra.f64)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0423), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f64));
                return context->report(diag);
            }
            break;
        case TokenId::IntrinsicLog10:
            ra.f64 = log10(rb.f64);
            if (!runtime && (rb.f64 <= 0 || isnan(ra.f64)))
            {
                Diagnostic diag{ip->node, ip->node->token, Err(Err0424), Hnt(Hnt0001)};
                diag.addRange(ip->node->childs.front(), Fmt(Hnt(Hnt0090), rb.f64));
                return context->report(diag);
            }
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }
    default:
        break;
    }

    return true;
}

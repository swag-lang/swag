#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Run/ByteCodeRun.h"
#include "Os/Os.h"
#include "Syntax/Tokenizer/Token.h"

void ByteCodeRun::executeLeftShift(Register* regDest, const Register& regLeft, const Register& regRight, uint32_t opBits, bool /*isSigned*/)
{
    const auto shift = regRight.u32;
    if (shift >= opBits)
        regDest->u64 = 0;
    else
        regDest->u64 = regLeft.u64 << shift;
}

void ByteCodeRun::executeRightShift(Register* regDest, const Register& regLeft, const Register& regRight, uint32_t opBits, bool isSigned)
{
    const auto shift = regRight.u32;
    if (shift >= opBits)
    {
        if (isSigned)
        {
            switch (opBits)
            {
                case 8:
                    regDest->s64 = regLeft.s8 < 0 ? -1 : 0;
                    break;
                case 16:
                    regDest->s64 = regLeft.s16 < 0 ? -1 : 0;
                    break;
                case 32:
                    regDest->s64 = regLeft.s32 < 0 ? -1 : 0;
                    break;
                case 64:
                    regDest->s64 = regLeft.s64 < 0 ? -1 : 0;
                    break;
                default:
                    break;
            }
        }
        else
        {
            regDest->u64 = 0;
        }
    }
    else if (isSigned)
    {
        switch (opBits)
        {
            case 8:
                regDest->s64 = regLeft.s8 >> shift;
                break;
            case 16:
                regDest->s64 = regLeft.s16 >> shift;
                break;
            case 32:
                regDest->s64 = regLeft.s32 >> shift;
                break;
            case 64:
                regDest->s64 = regLeft.s64 >> shift;
                break;
            default:
                break;
        }
    }
    else
    {
        switch (opBits)
        {
            case 8:
                regDest->u64 = regLeft.u8 >> shift;
                break;
            case 16:
                regDest->u64 = regLeft.u16 >> shift;
                break;
            case 32:
                regDest->u64 = regLeft.u32 >> shift;
                break;
            case 64:
                regDest->u64 = regLeft.u64 >> shift;
                break;
            default:
                break;
        }
    }
}

bool ByteCodeRun::executeMathIntrinsic(JobContext*, ByteCodeInstruction* ip, Register& ra, const Register& rb, const Register& rc, const Register& rd)
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
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicAbs:
                    ra.s8 = static_cast<int8_t>(abs(rb.s8));
                    break;
                case TokenId::IntrinsicBitCountNz:
                    ra.u8 = OS::bitCountNz(rb.u8);
                    break;
                case TokenId::IntrinsicBitCountTz:
                    ra.u8 = OS::bitCountTz(rb.u8);
                    break;
                case TokenId::IntrinsicBitCountLz:
                    ra.u8 = OS::bitCountLz(rb.u8);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS16x1:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicAbs:
                    ra.s16 = static_cast<int16_t>(abs(rb.s16));
                    break;
                case TokenId::IntrinsicBitCountNz:
                    ra.u16 = OS::bitCountNz(rb.u16);
                    break;
                case TokenId::IntrinsicBitCountTz:
                    ra.u16 = OS::bitCountTz(rb.u16);
                    break;
                case TokenId::IntrinsicBitCountLz:
                    ra.u16 = OS::bitCountLz(rb.u16);
                    break;
                case TokenId::IntrinsicByteSwap:
                    ra.u16 = OS::byteSwap(rb.u16);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS32x1:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicAbs:
                    ra.s32 = abs(rb.s32);
                    break;
                case TokenId::IntrinsicBitCountNz:
                    ra.u32 = OS::bitCountNz(rb.u32);
                    break;
                case TokenId::IntrinsicBitCountTz:
                    ra.u32 = OS::bitCountTz(rb.u32);
                    break;
                case TokenId::IntrinsicBitCountLz:
                    ra.u32 = OS::bitCountLz(rb.u32);
                    break;
                case TokenId::IntrinsicByteSwap:
                    ra.u32 = OS::byteSwap(rb.u32);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS64x1:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicAbs:
                    ra.s64 = abs(rb.s64);
                    break;
                case TokenId::IntrinsicBitCountNz:
                    ra.u64 = OS::bitCountNz(rb.u64);
                    break;
                case TokenId::IntrinsicBitCountTz:
                    ra.u64 = OS::bitCountTz(rb.u64);
                    break;
                case TokenId::IntrinsicBitCountLz:
                    ra.u64 = OS::bitCountLz(rb.u64);
                    break;
                case TokenId::IntrinsicByteSwap:
                    ra.u64 = OS::byteSwap(rb.u64);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS8x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.s8 = std::min(rb.s8, rc.s8);
                    break;
                case TokenId::IntrinsicMax:
                    ra.s8 = std::max(rb.s8, rc.s8);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS16x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.s16 = std::min(rb.s16, rc.s16);
                    break;
                case TokenId::IntrinsicMax:
                    ra.s16 = std::max(rb.s16, rc.s16);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS32x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.s32 = std::min(rb.s32, rc.s32);
                    break;
                case TokenId::IntrinsicMax:
                    ra.s32 = std::max(rb.s32, rc.s32);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS64x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.s64 = std::min(rb.s64, rc.s64);
                    break;
                case TokenId::IntrinsicMax:
                    ra.s64 = std::max(rb.s64, rc.s64);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicU8x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.u8 = std::min(rb.u8, rc.u8);
                    break;
                case TokenId::IntrinsicMax:
                    ra.u8 = std::max(rb.u8, rc.u8);
                    break;
                case TokenId::IntrinsicRol:
                    ra.u8 = static_cast<uint8_t>((rb.u8 << rc.u32) | (rb.u8 >> (8 - rc.u32)));
                    break;
                case TokenId::IntrinsicRor:
                    ra.u8 = static_cast<uint8_t>((rb.u8 >> rc.u32) | (rb.u8 << (8 - rc.u32)));
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicU16x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.u16 = std::min(rb.u16, rc.u16);
                    break;
                case TokenId::IntrinsicMax:
                    ra.u16 = std::max(rb.u16, rc.u16);
                    break;
                case TokenId::IntrinsicRol:
                    ra.u16 = static_cast<uint16_t>((rb.u16 << rc.u32) | (rb.u16 >> (16 - rc.u32)));
                    break;
                case TokenId::IntrinsicRor:
                    ra.u16 = static_cast<uint16_t>((rb.u16 >> rc.u32) | (rb.u16 << (16 - rc.u32)));
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicU32x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.u32 = std::min(rb.u32, rc.u32);
                    break;
                case TokenId::IntrinsicMax:
                    ra.u32 = std::max(rb.u32, rc.u32);
                    break;
                case TokenId::IntrinsicRol:
                    ra.u32 = (rb.u32 << rc.u32) | (rb.u32 >> (32 - rc.u32));
                    break;
                case TokenId::IntrinsicRor:
                    ra.u32 = (rb.u32 >> rc.u32) | (rb.u32 << (32 - rc.u32));
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicU64x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicMin:
                    ra.u64 = std::min(rb.u64, rc.u64);
                    break;
                case TokenId::IntrinsicMax:
                    ra.u64 = std::max(rb.u64, rc.u64);
                    break;
                case TokenId::IntrinsicRol:
                    ra.u64 = (rb.u64 << rc.u32) | (rb.u64 >> (64 - rc.u32));
                    break;
                case TokenId::IntrinsicRor:
                    ra.u64 = (rb.u64 >> rc.u32) | (rb.u64 << (64 - rc.u32));
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF32x2:
        {
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicPow:
                    ra.f32 = powf(rb.f32, rc.f32);
                    break;
                case TokenId::IntrinsicMin:
                    ra.f32 = std::min(rb.f32, rc.f32);
                    break;
                case TokenId::IntrinsicMax:
                    ra.f32 = std::max(rb.f32, rc.f32);
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
            switch (static_cast<TokenId>(ip->d.u32))
            {
                case TokenId::IntrinsicPow:
                    ra.f64 = pow(rb.f64, rc.f64);
                    break;
                case TokenId::IntrinsicMin:
                    ra.f64 = std::min(rb.f64, rc.f64);
                    break;
                case TokenId::IntrinsicMax:
                    ra.f64 = std::max(rb.f64, rc.f64);
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
            switch (static_cast<TokenId>(ip->d.u32))
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
                    break;
                case TokenId::IntrinsicASin:
                    ra.f32 = asinf(rb.f32);
                    break;
                case TokenId::IntrinsicACos:
                    ra.f32 = acosf(rb.f32);
                    break;
                case TokenId::IntrinsicLog:
                    ra.f32 = log(rb.f32);
                    break;
                case TokenId::IntrinsicLog2:
                    ra.f32 = log2(rb.f32);
                    break;
                case TokenId::IntrinsicLog10:
                    ra.f32 = log10(rb.f32);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF64x1:
        {
            switch (static_cast<TokenId>(ip->d.u32))
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
                    break;
                case TokenId::IntrinsicASin:
                    ra.f64 = asin(rb.f64);
                    break;
                case TokenId::IntrinsicACos:
                    ra.f64 = acos(rb.f64);
                    break;
                case TokenId::IntrinsicLog:
                    ra.f64 = log(rb.f64);
                    break;
                case TokenId::IntrinsicLog2:
                    ra.f64 = log2(rb.f64);
                    break;
                case TokenId::IntrinsicLog10:
                    ra.f64 = log10(rb.f64);
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
            break;
        }
    }

    return true;
}

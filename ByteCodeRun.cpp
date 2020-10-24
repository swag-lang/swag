#include "pch.h"
#include "ByteCodeRun.h"
#include "Diagnostic.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Workspace.h"
#include "Context.h"
#include "Diagnostic.h"
#include "Runtime.h"
#include "Module.h"
#include "CompilerItf.h"
#include "ByteCodeStack.h"
#include "Runtime.h"

#define IMMA_B(ip) ((ip->flags & BCI_IMM_A) ? ip->a.b : registersRC[ip->a.u32].b)
#define IMMB_B(ip) ((ip->flags & BCI_IMM_B) ? ip->b.b : registersRC[ip->b.u32].b)

#define IMMA_F32(ip) ((ip->flags & BCI_IMM_A) ? ip->a.f32 : registersRC[ip->a.u32].f32)
#define IMMA_F64(ip) ((ip->flags & BCI_IMM_A) ? ip->a.f64 : registersRC[ip->a.u32].f64)

#define IMMB_F32(ip) ((ip->flags & BCI_IMM_B) ? ip->b.f32 : registersRC[ip->b.u32].f32)
#define IMMB_F64(ip) ((ip->flags & BCI_IMM_B) ? ip->b.f64 : registersRC[ip->b.u32].f64)

#define IMMA_S8(ip) ((ip->flags & BCI_IMM_A) ? ip->a.s8 : registersRC[ip->a.u32].s8)
#define IMMA_S32(ip) ((ip->flags & BCI_IMM_A) ? ip->a.s32 : registersRC[ip->a.u32].s32)
#define IMMA_S64(ip) ((ip->flags & BCI_IMM_A) ? ip->a.s64 : registersRC[ip->a.u32].s64)

#define IMMB_S8(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s8 : registersRC[ip->b.u32].s8)
#define IMMB_S16(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s16 : registersRC[ip->b.u32].s16)
#define IMMB_S32(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s32 : registersRC[ip->b.u32].s32)
#define IMMB_S64(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s64 : registersRC[ip->b.u32].s64)

#define IMMA_U8(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u8 : registersRC[ip->a.u32].u8)
#define IMMA_U16(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u16 : registersRC[ip->a.u32].u16)
#define IMMA_U32(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u32 : registersRC[ip->a.u32].u32)
#define IMMA_U64(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u64 : registersRC[ip->a.u32].u64)

#define IMMB_U8(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u8 : registersRC[ip->b.u32].u8)
#define IMMB_U16(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u16 : registersRC[ip->b.u32].u16)
#define IMMB_U32(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u32 : registersRC[ip->b.u32].u32)
#define IMMB_U64(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u64 : registersRC[ip->b.u32].u64)

#define IMMC_U32(ip) ((ip->flags & BCI_IMM_C) ? ip->c.u32 : registersRC[ip->c.u32].u32)

bool ByteCodeRun::executeMathIntrinsic(JobContext* context, ByteCodeInstruction* ip, Register& ra, const Register& rb, const Register& rc)
{
    switch (ip->op)
    {
    case ByteCodeOp::IntrinsicS8x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicAbs:
            ra.s8 = (int8_t) abs(rb.s8);
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
            if (isnan(ra.f32))
                return context->report({ip->node, ip->node->token, format("'@power' on invalid values '%.3f' and '%.3f'", rb.f32, rc.f32)});
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
            if (isnan(ra.f64))
                return context->report({ip->node, ip->node->token, format("'@power' on invalid values '%.3f' and '%.3f'", rb.f64, rc.f64)});
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
        case TokenId::IntrinsicSqrt:
            if (rb.f32 < 0)
                return context->report({ip->node, ip->node->token, format("'@sqrt' on an invalid value '%.3f'", rb.f32)});
            ra.f32 = sqrtf(rb.f32);
            break;
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
        case TokenId::IntrinsicASin:
            ra.f32 = asinf(rb.f32);
            if (isnan(ra.f32))
                return context->report({ip->node, ip->node->token, format("'@asin' on an invalid value '%.3f'", rb.f32)});
            break;
        case TokenId::IntrinsicACos:
            ra.f32 = acosf(rb.f32);
            if (isnan(ra.f32))
                return context->report({ip->node, ip->node->token, format("'@acos' on an invalid value '%.3f'", rb.f32)});
            break;
        case TokenId::IntrinsicATan:
            ra.f32 = atanf(rb.f32);
            break;
        case TokenId::IntrinsicLog:
            ra.f32 = log(rb.f32);
            if (isnan(ra.f32))
                return context->report({ip->node, ip->node->token, format("'@log' on an invalid value '%.3f'", rb.f32)});
            break;
        case TokenId::IntrinsicLog2:
            ra.f32 = log2(rb.f32);
            if (isnan(ra.f32))
                return context->report({ip->node, ip->node->token, format("'@log2' on an invalid value '%.3f'", rb.f32)});
            break;
        case TokenId::IntrinsicLog10:
            ra.f32 = log10(rb.f32);
            if (isnan(ra.f32))
                return context->report({ip->node, ip->node->token, format("'@log10' on an invalid value '%.3f'", rb.f32)});
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
            ra.f32 = Runtime::abs(rb.f32);
            break;
        case TokenId::IntrinsicExp:
            ra.f32 = expf(rb.f32);
            break;
        case TokenId::IntrinsicExp2:
            ra.f32 = exp2f(rb.f32);
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
        case TokenId::IntrinsicSqrt:
            ra.f64 = sqrt(rb.f64);
            if (isnan(ra.f64))
                return context->report({ip->node, ip->node->token, format("'@sqrt' on an invalid value '%.3f'", rb.f64)});
            break;
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
        case TokenId::IntrinsicASin:
            ra.f64 = asin(rb.f64);
            if (isnan(ra.f64))
                return context->report({ip->node, ip->node->token, format("'@asin' on an invalid value '%.3f'", rb.f64)});
            break;
        case TokenId::IntrinsicACos:
            ra.f64 = acos(rb.f64);
            if (isnan(ra.f64))
                return context->report({ip->node, ip->node->token, format("'@acos' on an invalid value '%.3f'", rb.f64)});
            break;
        case TokenId::IntrinsicATan:
            ra.f64 = atan(rb.f64);
            break;
        case TokenId::IntrinsicLog:
            ra.f64 = log(rb.f64);
            if (isnan(ra.f64))
                return context->report({ip->node, ip->node->token, format("'@lob' on an invalid value '%.3f'", rb.f64)});
            break;
        case TokenId::IntrinsicLog2:
            ra.f64 = log2(rb.f64);
            if (isnan(ra.f64))
                return context->report({ip->node, ip->node->token, format("'@log2' on an invalid value '%.3f'", rb.f64)});
            break;
        case TokenId::IntrinsicLog10:
            ra.f64 = log10(rb.f64);
            if (isnan(ra.f64))
                return context->report({ip->node, ip->node->token, format("'@log10' on an invalid value '%.3f'", rb.f64)});
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
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }
    }

    return true;
}

inline bool ByteCodeRun::executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->bc->registersRC[context->bc->curRC];
    auto registersRR = context->registersRR;

    switch (ip->op)
    {
    case ByteCodeOp::Nop:
        break;

    case ByteCodeOp::IntrinsicS8x1:
    case ByteCodeOp::IntrinsicS16x1:
    case ByteCodeOp::IntrinsicS32x1:
    case ByteCodeOp::IntrinsicS64x1:
    case ByteCodeOp::IntrinsicF32x1:
    case ByteCodeOp::IntrinsicF64x1:
    {
        auto& rb = (ip->flags & BCI_IMM_B) ? ip->b : registersRC[ip->b.u32];
        SWAG_CHECK(executeMathIntrinsic(context, ip, registersRC[ip->a.u32], rb, rb));
        break;
    }

    case ByteCodeOp::IntrinsicF32x2:
    case ByteCodeOp::IntrinsicF64x2:
    {
        auto& rb = (ip->flags & BCI_IMM_B) ? ip->b : registersRC[ip->b.u32];
        auto& rc = (ip->flags & BCI_IMM_C) ? ip->c : registersRC[ip->c.u32];
        SWAG_CHECK(executeMathIntrinsic(context, ip, registersRC[ip->a.u32], rb, rc));
        break;
    }

    case ByteCodeOp::TestNotZero8:
    {
        registersRC[ip->a.u32].b = IMMB_U8(ip) != 0;
        break;
    }
    case ByteCodeOp::TestNotZero16:
    {
        registersRC[ip->a.u32].b = IMMB_U16(ip) != 0;
        break;
    }
    case ByteCodeOp::TestNotZero32:
    {
        registersRC[ip->a.u32].b = IMMB_U32(ip) != 0;
        break;
    }
    case ByteCodeOp::TestNotZero64:
    {
        registersRC[ip->a.u32].b = IMMB_U64(ip) != 0;
        break;
    }

    case ByteCodeOp::JumpIfZero32:
    {
        if (!IMMA_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfZero64:
    {
        if (!registersRC[ip->a.u32].u64)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotZero32:
    {
        if (registersRC[ip->a.u32].u32)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotZero64:
    {
        if (registersRC[ip->a.u32].u64)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfFalse:
    {
        if (!registersRC[ip->a.u32].b)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfTrue:
    {
        if (registersRC[ip->a.u32].b)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::Jump:
    {
        context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::Ret:
    {
        if (context->sp == context->stack + context->stackSize)
            return false;
        context->bc->leaveByteCode();
        context->ip = context->pop<ByteCodeInstruction*>();
        context->bc = context->pop<ByteCode*>();
        context->bp = context->pop<uint8_t*>();
        break;
    }
    case ByteCodeOp::LocalCall:
    {
        context->bc->addCallStack(context);
        context->push(context->bp);
        context->push(context->bc);
        context->push(context->ip);
        context->bc = (ByteCode*) ip->a.pointer;
        SWAG_ASSERT(context->bc);
        SWAG_ASSERT(context->bc->out);
        context->ip = context->bc->out;
        SWAG_ASSERT(context->ip);
        context->bp = context->sp;
        context->bc->enterByteCode(context);
        break;
    }
    case ByteCodeOp::LambdaCall:
    {
        auto ptr = registersRC[ip->a.u32].u64;
        if (isByteCodeLambda((void*) ptr))
        {
            context->bc->addCallStack(context);
            context->push(context->bp);
            context->push(context->bc);
            context->push(context->ip);

            context->bc = (ByteCode*) undoByteCodeLambda((void*) ptr);
            SWAG_ASSERT(context->bc);
            context->ip = context->bc->out;
            SWAG_ASSERT(context->ip);
            context->bp = context->sp;
            context->bc->enterByteCode(context);
        }

        // Marked as foreign, need to resolve address
        else if (isForeignLambda((void*) ptr))
        {
            auto funcPtr = undoForeignLambda((void*) ptr);
            SWAG_ASSERT(funcPtr);
            auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::Lambda);
            ffiCall(context, funcPtr, typeInfoFunc);
        }

        // Normal lambda
        else
        {
            auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::Lambda);
            ffiCall(context, registersRC[ip->a.u32].pointer, typeInfoFunc);
        }

        break;
    }
    case ByteCodeOp::MakeLambdaForeign:
    {
        auto funcNode = (AstFuncDecl*) ip->b.pointer;
        SWAG_ASSERT(funcNode);
        auto funcPtr = ffiGetFuncAddress(context, funcNode);
        SWAG_ASSERT(funcPtr);

        // If this assert, then Houston we have a problem. At one point in time, i was using the lower bit to determine if a lambda is bytecode or native.
        // But thanks to clang, it seems that the function pointer could have it's lower bit set (optim level 1).
        // So now its the highest bit.
        SWAG_ASSERT(!isByteCodeLambda(funcPtr));

        registersRC[ip->a.u32].pointer = (uint8_t*) doForeignLambda(funcPtr);
        break;
    }
    case ByteCodeOp::MakeLambda:
    {
        registersRC[ip->a.u32].u64 = (uint64_t) doByteCodeLambda((void*) ip->b.pointer);
        break;
    }
    case ByteCodeOp::ForeignCall:
    {
        ffiCall(context, ip);
        break;
    }

    case ByteCodeOp::IncPointer32:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer + IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::DecPointer32:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer - IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::DeRef8:
    {
        auto ptr                   = registersRC[ip->a.u32].pointer;
        registersRC[ip->a.u32].u64 = *(uint8_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef16:
    {
        auto ptr                   = registersRC[ip->a.u32].pointer;
        registersRC[ip->a.u32].u64 = *(uint16_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef32:
    {
        auto ptr                   = registersRC[ip->a.u32].pointer;
        registersRC[ip->a.u32].u64 = *(uint32_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef64:
    {
        auto ptr                   = registersRC[ip->a.u32].pointer;
        registersRC[ip->a.u32].u64 = *(uint64_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRefPointer:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        registersRC[ip->b.u32].pointer = *(uint8_t**) (ptr + ip->c.u32);
        break;
    }
    case ByteCodeOp::DeRefStringSlice:
    {
        auto       ptr                 = registersRC[ip->a.u32].pointer;
        uint64_t** ptrptr              = (uint64_t**) ptr;
        registersRC[ip->a.u32].pointer = (uint8_t*) ptrptr[0];
        registersRC[ip->b.u32].u64     = (uint64_t) ptrptr[1];
        break;
    }

    case ByteCodeOp::PushRAParam:
        context->push(registersRC[ip->a.u32].u64);
        break;
    case ByteCodeOp::PushRAParam2:
        context->push(registersRC[ip->a.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        break;
    case ByteCodeOp::PushRAParam3:
        context->push(registersRC[ip->a.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->c.u32].u64);
        break;
    case ByteCodeOp::PushRAParam4:
        context->push(registersRC[ip->a.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->c.u32].u64);
        context->push(registersRC[ip->d.u32].u64);
        break;

    case ByteCodeOp::IncSP:
    case ByteCodeOp::IncSPPostCall:
    {
        context->incSP(ip->a.u32);
        break;
    }
    case ByteCodeOp::DecSP:
    {
        context->decSP(ip->a.u32);
        break;
    }
    case ByteCodeOp::DecSPBP:
    {
        context->decSP(ip->a.u32);
        context->bp = context->sp;
        break;
    }
    case ByteCodeOp::MemCpy:
    {
        void*    dst  = registersRC[ip->a.u32].pointer;
        void*    src  = registersRC[ip->b.u32].pointer;
        uint32_t size = IMMC_U32(ip);
        memcpy(dst, src, size);
        break;
    }
    case ByteCodeOp::MemSet:
    {
        void*    dst   = registersRC[ip->a.u32].pointer;
        uint32_t value = registersRC[ip->b.u32].u8;
        uint32_t size  = registersRC[ip->c.u32].u32;
        memset(dst, value, size);
        break;
    }

    case ByteCodeOp::IntrinsicMemCmp:
    {
        void*    dst               = registersRC[ip->b.u32].pointer;
        void*    src               = registersRC[ip->c.u32].pointer;
        uint32_t size              = registersRC[ip->d.u32].u32;
        registersRC[ip->a.u32].s32 = Runtime::memcmp(dst, src, size);
        break;
    }

    case ByteCodeOp::CopyRBtoRA:
    {
        registersRC[ip->a.u32] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRBAddrToRA:
    case ByteCodeOp::CopyRBAddrToRA2:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) (registersRC + ip->b.u32);
        break;
    }
    case ByteCodeOp::SetImmediate32:
    {
        registersRC[ip->a.u32].u32 = ip->b.u32;
        break;
    }
    case ByteCodeOp::SetImmediate64:
    {
        registersRC[ip->a.u32].u64 = ip->b.u64;
        break;
    }
    case ByteCodeOp::ClearRA:
    {
        registersRC[ip->a.u32].u64 = 0;
        break;
    }
    case ByteCodeOp::DecrementRA32:
    {
        registersRC[ip->a.u32].u32--;
        break;
    }
    case ByteCodeOp::IncrementRA32:
    {
        registersRC[ip->a.u32].u32++;
        break;
    }
    case ByteCodeOp::Add32byVB32:
    {
        registersRC[ip->a.u32].u32 += ip->b.u32;
        break;
    }

    case ByteCodeOp::PushRR:
    {
        SWAG_ASSERT(ip->a.u32 < 4);
        context->push(registersRR[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::PopRR:
    {
        SWAG_ASSERT(ip->a.u32 < 4);
        registersRR[ip->a.u32].u64 = context->pop<uint64_t>();
        break;
    }

    case ByteCodeOp::CopyRCtoRR:
    case ByteCodeOp::CopyRCtoRT:
    {
        SWAG_ASSERT(ip->a.u32 < 4);
        registersRR[ip->a.u32] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRRtoRC:
    case ByteCodeOp::CopyRTtoRC:
    {
        SWAG_ASSERT(ip->b.u32 < 4);
        registersRC[ip->a.u32] = registersRR[ip->b.u32];
        break;
    }
    case ByteCodeOp::PushBP:
    {
        context->push(context->bp);
        break;
    }
    case ByteCodeOp::PopBP:
    {
        context->bp = context->pop<uint8_t*>();
        break;
    }
    case ByteCodeOp::CopySP:
    case ByteCodeOp::CopySPVaargs:
        registersRC[ip->a.u32].pointer = context->sp - ip->b.u32;
        break;

    case ByteCodeOp::GetFromStack64:
    case ByteCodeOp::GetFromStackParam64:
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::MakeStackPointer:
    case ByteCodeOp::MakeStackPointerParam:
        registersRC[ip->a.u32].pointer = context->bp + ip->b.u32;
        break;

    case ByteCodeOp::SetZeroStack8:
        *(uint8_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStack16:
        *(uint16_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStack32:
        *(uint32_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStack64:
        *(uint64_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStackX:
        memset(context->bp + ip->a.u32, 0, ip->b.u32);
        break;

    case ByteCodeOp::SetZeroAtPointer8:
    {
        auto ptr                      = registersRC[ip->a.u32].pointer;
        *(uint8_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointer16:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint16_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointer32:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint32_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointer64:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint64_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointerX:
        memset(registersRC[ip->a.u32].pointer, 0, ip->b.u32);
        break;
    case ByteCodeOp::SetZeroAtPointerXRB:
        memset(registersRC[ip->a.u32].pointer, 0, registersRC[ip->b.u32].u32 * ip->c.u32);
        break;

    case ByteCodeOp::GetFromMutableSeg64:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromBssSeg64:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->bssSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        break;
    }

    case ByteCodeOp::MakeMutableSegPointer:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
        {
            ip->d.pointer = module->mutableSegment.address(ip->b.u32);
            if (module->saveMutableValues && ip->c.pointer)
            {
                SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                module->mutableSegment.saveValue(ip->d.pointer, over->typeInfo->sizeOf);
            }
        }

        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }
    case ByteCodeOp::MakeBssSegPointer:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
        {
            ip->d.pointer = module->bssSegment.address(ip->b.u32);
            if (ip->c.pointer)
            {
                if (module->saveBssValues)
                {
                    SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                    module->mutableSegment.saveValue(ip->d.pointer, over->typeInfo->sizeOf);
                }
                else if (module->bssCannotChange)
                {
                    SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                    context->error(format("variable '%s' is in the bss segment (content is zero) and cannot be changed at compile time. Initialize it with '?' if this is intended", over->node->name.c_str()));
                }
            }
        }
        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }

    case ByteCodeOp::MakeConstantSegPointer:
    {
        auto module = context->sourceFile->module;
        auto offset = ip->b.u32;
        if (!ip->d.pointer)
            ip->d.pointer = module->constantSegment.address(offset);
        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }
    case ByteCodeOp::MakeTypeSegPointer:
    {
        auto module = context->sourceFile->module;
        auto offset = ip->b.u32;
        if (!ip->d.pointer)
            ip->d.pointer = module->typeSegment.address(offset);
        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }

    case ByteCodeOp::Mul64byVB32:
    {
        registersRC[ip->a.u32].s64 *= ip->b.u32;
        break;
    }
    case ByteCodeOp::Div64byVB32:
    {
        registersRC[ip->a.u32].s64 /= ip->b.u32;
        break;
    }

    case ByteCodeOp::BinOpModuloS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloU32:
    {
        auto val1                  = IMMA_U32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloU64:
    {
        auto val1                  = IMMA_U64(ip);
        auto val2                  = IMMB_U64(ip);
        registersRC[ip->c.u32].u64 = val1 % val2;
        break;
    }

    case ByteCodeOp::BinOpPlusS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusF32:
    {
        auto val1                  = IMMA_F32(ip);
        auto val2                  = IMMB_F32(ip);
        registersRC[ip->c.u32].f32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusF64:
    {
        auto val1                  = IMMA_F64(ip);
        auto val2                  = IMMB_F64(ip);
        registersRC[ip->c.u32].f64 = val1 + val2;
        break;
    }

    case ByteCodeOp::BinOpMinusS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusF32:
    {
        auto val1                  = IMMA_F32(ip);
        auto val2                  = IMMB_F32(ip);
        registersRC[ip->c.u32].f32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusF64:
    {
        auto val1                  = IMMA_F64(ip);
        auto val2                  = IMMB_F64(ip);
        registersRC[ip->c.u32].f64 = val1 - val2;
        break;
    }

    case ByteCodeOp::BinOpMulS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulF32:
    {
        auto val1                  = IMMA_F32(ip);
        auto val2                  = IMMB_F32(ip);
        registersRC[ip->c.u32].f32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulF64:
    {
        auto val1                  = IMMA_F64(ip);
        auto val2                  = IMMB_F64(ip);
        registersRC[ip->c.u32].f64 = val1 * val2;
        break;
    }

    case ByteCodeOp::BinOpDivS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivU32:
    {
        auto val1                  = IMMA_U32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivU64:
    {
        auto val1                  = IMMA_U64(ip);
        auto val2                  = IMMB_U64(ip);
        registersRC[ip->c.u32].u64 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivF32:
    {
        auto val1                  = IMMA_F32(ip);
        auto val2                  = IMMB_F32(ip);
        registersRC[ip->c.u32].f32 = val1 / val2;
        break;
    }

    case ByteCodeOp::BinOpDivF64:
    {
        auto val1                  = IMMA_F64(ip);
        auto val2                  = IMMB_F64(ip);
        registersRC[ip->c.u32].f64 = val1 / val2;
        break;
    }

    case ByteCodeOp::BinOpBitmaskAndS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 & val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskAndS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 & val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOrS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 | val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOrS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 | val2;
        break;
    }

    case ByteCodeOp::BinOpShiftLeftU32:
    {
        auto val1                  = IMMA_U32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u32 = val1 << val2;
        break;
    }
    case ByteCodeOp::BinOpShiftLeftU64:
    {
        auto val1                  = IMMA_U64(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u64 = val1 << val2;
        break;
    }

    case ByteCodeOp::BinOpShiftRightS32:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].s32 = val1 >> val2;
        break;
    }
    case ByteCodeOp::BinOpShiftRightS64:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].s64 = val1 >> val2;
        break;
    }
    case ByteCodeOp::BinOpShiftRightU32:
    {
        auto val1                  = IMMA_U32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u32 = val1 >> val2;
        break;
    }
    case ByteCodeOp::BinOpShiftRightU64:
    {
        auto val1                  = IMMA_U64(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u64 = val1 >> val2;
        break;
    }

    case ByteCodeOp::BinOpXorU32:
    {
        auto val1                  = IMMA_U32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u32 = val1 ^ val2;
        break;
    }
    case ByteCodeOp::BinOpXorU64:
    {
        auto val1                  = IMMA_U64(ip);
        auto val2                  = IMMB_U64(ip);
        registersRC[ip->c.u32].u64 = val1 ^ val2;
        break;
    }

    case ByteCodeOp::BinOpShiftRightU64VB:
    {
        registersRC[ip->a.u32].u64 >>= ip->b.u32;
        break;
    }

    case ByteCodeOp::IntrinsicArguments:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) g_CommandLine.userArgumentsSlice.first;
        registersRC[ip->b.u32].u64     = (uint64_t) g_CommandLine.userArgumentsSlice.second;
        break;
    }
    case ByteCodeOp::IntrinsicCompiler:
    {
        auto itf                       = (uint8_t**) getCompilerItf(context->sourceFile->module);
        registersRC[ip->a.u32].pointer = itf[0];
        registersRC[ip->b.u32].pointer = itf[1];
        break;
    }
    case ByteCodeOp::IntrinsicIsByteCode:
    {
        registersRC[ip->a.u32].b = true;
        break;
    }
    case ByteCodeOp::IntrinsicError:
    {
        Utf8 msg;
        msg.append((const char*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].u32);
        auto location = (ConcreteCompilerSourceLocation*) registersRC[ip->c.u32].pointer;
        Runtime::error(msg.c_str(), msg.length(), location);
        break;
    }
    case ByteCodeOp::IntrinsicAssertMsg:
    {
        Utf8 msg;
        msg.append((const char*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].u32);
        auto location = (ConcreteCompilerSourceLocation*) registersRC[ip->c.u32].pointer;
        Runtime::assertMsg(msg.c_str(), msg.length(), location);
        break;
    }
    case ByteCodeOp::IntrinsicAssert:
    {
        if (IMMA_U8(ip))
            break;
        if (context->sourceFile->unittestError)
            context->error(ip->d.pointer ? (const char*) ip->d.pointer : "assertion failed");
        else
        {
            ConcreteCompilerSourceLocation loc;
            auto                           ipLocation = ip->getLocation(context->bc);
            loc.lineStart = loc.lineEnd = ipLocation->line;
            loc.colStart = loc.colEnd = ipLocation->column;
            loc.fileName.buffer       = (void*) ip->node->sourceFile->path.c_str();
            loc.fileName.count        = ip->node->sourceFile->path.length();
            auto msg                  = (const char*) ip->d.pointer;
            auto lenMsg               = msg ? (uint32_t) strlen(msg) : 0;
            Runtime::assertMsg(msg, lenMsg, &loc);
        }
        break;
    }
    case ByteCodeOp::IntrinsicAlloc:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) malloc(registersRC[ip->b.u32].u32);
        break;
    }
    case ByteCodeOp::IntrinsicRealloc:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) realloc(registersRC[ip->b.u32].pointer, registersRC[ip->c.u32].u32);
        break;
    }
    case ByteCodeOp::IntrinsicFree:
    {
        free(registersRC[ip->a.u32].pointer);
        break;
    }
    case ByteCodeOp::IntrinsicGetContext:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) Runtime::tlsGetValue(g_tlsContextId);
        break;
    }
    case ByteCodeOp::IntrinsicSetContext:
    {
        Runtime::tlsSetValue(g_tlsContextId, registersRC[ip->a.u32].pointer);
        break;
    }
    case ByteCodeOp::IntrinsicPrintF64:
    {
        g_Log.lock();
        Runtime::print(registersRC[ip->a.u32].f64);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintS64:
    {
        g_Log.lock();
        Runtime::print(registersRC[ip->a.u32].s64);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintString:
    {
        g_Log.lock();
        Runtime::print(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].u32);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicInterfaceOf:
    {
        registersRC[ip->c.u32].pointer = (uint8_t*) Runtime::interfaceOf(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].pointer);
        break;
    }

    case ByteCodeOp::SetAtPointer8:
    {
        auto ptr        = registersRC[ip->a.u32].pointer;
        *(uint8_t*) ptr = IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::SetAtPointer16:
    {
        auto ptr         = registersRC[ip->a.u32].pointer;
        *(uint16_t*) ptr = IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::SetAtPointer32:
    {
        auto ptr         = registersRC[ip->a.u32].pointer;
        *(uint32_t*) ptr = IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::SetAtPointer64:
    {
        auto ptr         = registersRC[ip->a.u32].pointer;
        *(uint64_t*) ptr = IMMB_U64(ip);
        break;
    }

    case ByteCodeOp::CompareOpEqual8:
    {
        registersRC[ip->c.u32].b = IMMA_U8(ip) == IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::CompareOpEqual16:
    {
        registersRC[ip->c.u32].b = IMMA_U16(ip) == IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::CompareOpEqual32:
    {
        registersRC[ip->c.u32].b = IMMA_U32(ip) == IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::CompareOpEqual64:
    {
        registersRC[ip->c.u32].b = IMMA_U64(ip) == IMMB_U64(ip);
        break;
    }
    case ByteCodeOp::IntrinsicStrCmp:
    {
        registersRC[ip->d.u32].b = Runtime::strcmp(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].u32, registersRC[ip->c.u32].pointer, registersRC[ip->d.u32].u32);
        break;
    }
    case ByteCodeOp::IntrinsicTypeCmp:
    {
        registersRC[ip->d.u32].b = Runtime::compareType(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].pointer, ip->c.u32);
        break;
    }
    case ByteCodeOp::CloneString:
    {
        char*    ptr                   = (char*) registersRC[ip->a.u32].pointer;
        uint32_t count                 = registersRC[ip->b.u32].u32;
        registersRC[ip->a.u32].pointer = (uint8_t*) malloc(count + 1);
        context->bc->autoFree.push_back(registersRC[ip->a.u32].pointer);
        memcpy(registersRC[ip->a.u32].pointer, ptr, count + 1);
        break;
    }

    case ByteCodeOp::CompareOp3WayS32:
    case ByteCodeOp::CompareOp3WayU32:
    {
        auto sub                   = IMMA_S32(ip) - IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = (int32_t)((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3WayU64:
    case ByteCodeOp::CompareOp3WayS64:
    {
        auto sub                   = IMMA_S64(ip) - IMMB_S64(ip);
        registersRC[ip->c.u32].s32 = (int32_t)((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3WayF32:
    {
        auto sub                   = IMMA_F32(ip) - IMMB_F32(ip);
        registersRC[ip->c.u32].s32 = (int32_t)((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3WayF64:
    {
        auto sub                   = IMMA_F64(ip) - IMMB_F64(ip);
        registersRC[ip->c.u32].s32 = (int32_t)((sub > 0) - (sub < 0));
        break;
    }

    case ByteCodeOp::CompareOpLowerS32:
    {
        registersRC[ip->c.u32].b = IMMA_S32(ip) < IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerS64:
    {
        registersRC[ip->c.u32].b = IMMA_S64(ip) < IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerU32:
    {
        registersRC[ip->c.u32].b = IMMA_U32(ip) < IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerU64:
    {
        registersRC[ip->c.u32].b = IMMA_U64(ip) < IMMB_U64(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerF32:
    {
        registersRC[ip->c.u32].b = IMMA_F32(ip) < IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerF64:
    {
        registersRC[ip->c.u32].b = IMMA_F64(ip) < IMMB_F64(ip);
        break;
    }

    case ByteCodeOp::CompareOpGreaterS32:
    {
        registersRC[ip->c.u32].b = IMMA_S32(ip) > IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterS64:
    {
        registersRC[ip->c.u32].b = IMMA_S64(ip) > IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterU32:
    {
        registersRC[ip->c.u32].b = IMMA_U32(ip) > IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterU64:
    {
        registersRC[ip->c.u32].b = IMMA_U64(ip) > IMMB_U64(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterF32:
    {
        registersRC[ip->c.u32].b = IMMA_F32(ip) > IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterF64:
    {
        registersRC[ip->c.u32].b = IMMA_F64(ip) > IMMB_F64(ip);
        break;
    }

    case ByteCodeOp::NegBool:
    {
        registersRC[ip->a.u32].b ^= 1;
        break;
    }
    case ByteCodeOp::NegS32:
    {
        registersRC[ip->a.u32].s32 = -registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::NegS64:
    {
        registersRC[ip->a.u32].s64 = -registersRC[ip->a.u32].s64;
        break;
    }
    case ByteCodeOp::NegF32:
    {
        registersRC[ip->a.u32].f32 = -registersRC[ip->a.u32].f32;
        break;
    }
    case ByteCodeOp::NegF64:
    {
        registersRC[ip->a.u32].f64 = -registersRC[ip->a.u32].f64;
        break;
    }

    case ByteCodeOp::InvertS8:
    {
        registersRC[ip->a.u32].s8 = ~registersRC[ip->a.u32].s8;
        break;
    }
    case ByteCodeOp::InvertS16:
    {
        registersRC[ip->a.u32].s16 = ~registersRC[ip->a.u32].s16;
        break;
    }
    case ByteCodeOp::InvertS32:
    {
        registersRC[ip->a.u32].s32 = ~registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::InvertS64:
    {
        registersRC[ip->a.u32].s64 = ~registersRC[ip->a.u32].s64;
        break;
    }

    case ByteCodeOp::ClearMaskU32:
    {
        registersRC[ip->a.u32].u32 &= ip->b.u32;
        break;
    }
    case ByteCodeOp::ClearMaskU64:
    {
        registersRC[ip->a.u32].u64 &= ip->b.u64;
        break;
    }

    case ByteCodeOp::CastBool8:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u8 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool16:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u16 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool32:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u32 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool64:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u64 ? true : false;
        break;
    }
    case ByteCodeOp::CastS32F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].s32);
        break;
    }
    case ByteCodeOp::CastU32F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].u32);
        break;
    }
    case ByteCodeOp::CastS16S32:
    {
        registersRC[ip->a.u32].s32 = static_cast<int32_t>(registersRC[ip->a.u32].s16);
        break;
    }
    case ByteCodeOp::CastS64F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].s64);
        break;
    }
    case ByteCodeOp::CastF64F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].f64);
        break;
    }
    case ByteCodeOp::CastF32F64:
    {
        registersRC[ip->a.u32].f64 = static_cast<double>(registersRC[ip->a.u32].f32);
        break;
    }
    case ByteCodeOp::CastU64F64:
    {
        registersRC[ip->a.u32].f64 = static_cast<double>(registersRC[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::CastS64F64:
    {
        registersRC[ip->a.u32].f64 = static_cast<double>(registersRC[ip->a.u32].s64);
        break;
    }
    case ByteCodeOp::CastF32S32:
    {
        registersRC[ip->a.u32].s32 = static_cast<int32_t>(registersRC[ip->a.u32].f32);
        break;
    }
    case ByteCodeOp::CastS8S16:
    {
        registersRC[ip->a.u32].s16 = static_cast<int16_t>(registersRC[ip->a.u32].s8);
        break;
    }
    case ByteCodeOp::CastS32S64:
    {
        registersRC[ip->a.u32].s64 = static_cast<int64_t>(registersRC[ip->a.u32].s32);
        break;
    }
    case ByteCodeOp::CastF64S64:
    {
        registersRC[ip->a.u32].s64 = static_cast<int64_t>(registersRC[ip->a.u32].f64);
        break;
    }

    case ByteCodeOp::AffectOpPlusEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer += IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer += IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer += IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer += IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer += IMMB_F64(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqPointer:
    {
        *(uint8_t**) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqPointer:
    {
        *(uint8_t**) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    }

    case ByteCodeOp::AffectOpMinusEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer -= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer -= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer -= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer -= IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer -= IMMB_F64(ip);
        break;
    }

    case ByteCodeOp::AffectOpMulEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer *= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer *= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer *= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer *= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer *= IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer *= IMMB_F64(ip);
        break;
    }

    case ByteCodeOp::AffectOpDivEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer /= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer /= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer /= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer /= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer /= IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer /= IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer /= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer /= IMMB_U64(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer /= IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::AffectOpDivEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer /= IMMB_F64(ip);
        break;
    }

    case ByteCodeOp::AffectOpAndEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer &= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpAndEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer &= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpAndEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer &= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpAndEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer &= IMMB_S64(ip);
        break;
    }

    case ByteCodeOp::AffectOpOrEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer |= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpOrEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer |= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpOrEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer |= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpOrEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer |= IMMB_S64(ip);
        break;
    }

    case ByteCodeOp::AffectOpXOrEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer ^= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer ^= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer ^= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer ^= IMMB_S64(ip);
        break;
    }

    case ByteCodeOp::AffectOpShiftLeftEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer <<= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer <<= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer <<= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer <<= IMMB_U32(ip);
        break;
    }

    case ByteCodeOp::AffectOpShiftRightEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer >>= IMMB_U32(ip);
        break;
    }

    case ByteCodeOp::AffectOpModuloEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer %= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer %= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer %= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer %= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer %= IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer %= IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer %= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer %= IMMB_U64(ip);
        break;
    }

    case ByteCodeOp::LowerZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 < 0 ? true : false;
        break;
    case ByteCodeOp::LowerEqZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 <= 0 ? true : false;
        break;
    case ByteCodeOp::GreaterZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 > 0 ? true : false;
        break;
    case ByteCodeOp::GreaterEqZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 >= 0 ? true : false;
        break;

    default:
        if (ip->op < ByteCodeOp::End)
            context->error(format("unknown bytecode instruction '%s'", g_ByteCodeOpNames[(int) ip->op]));
        break;
    }

    return true;
}

bool ByteCodeRun::runLoop(ByteCodeRunContext* context)
{
    while (context->ip)
    {
        // Get instruction
        auto ip = context->ip++;
        SWAG_ASSERT(ip->op <= ByteCodeOp::End);
        if (ip->op == ByteCodeOp::End)
            break;

        if (!executeInstruction(context, ip))
            break;

        // Error ?
        if (context->hasError)
        {
            JobContext* errorContext = context->callerContext ? context->callerContext : context;
            if (context->errorLoc)
            {
                SourceLocation start = {context->errorLoc->lineStart, context->errorLoc->colStart};
                SourceLocation end   = {context->errorLoc->lineEnd, context->errorLoc->colEnd};
                Diagnostic     diag{ip->node->sourceFile, start, end, context->errorMsg};
                errorContext->sourceFile = ip->node->sourceFile;
                errorContext->report(diag);
            }
            else
            {
                auto location = ip->getLocation(context->bc);
                if (location)
                {
                    Diagnostic diag{ip->node->sourceFile, *location, "error during bytecode execution, " + context->errorMsg};
                    errorContext->sourceFile = ip->node->sourceFile;
                    errorContext->report(diag);
                }
                else
                {
                    Diagnostic diag{ip->node, ip->node->token, "error during bytecode execution, " + context->errorMsg};
                    errorContext->sourceFile = ip->node->sourceFile;
                    errorContext->report(diag);
                }
            }

            return false;
        }
    }

    return true;
}

static int exceptionHandler(ByteCodeRunContext* runContext, LPEXCEPTION_POINTERS args)
{
    // Special exception raised by @error, to simply log an error message
    // This is called by assertion too, in certain conditions (if we do not want dialog boxes, when running tests for example)
    if (args->ExceptionRecord->ExceptionCode == 666)
    {
        auto location = (ConcreteCompilerSourceLocation*) args->ExceptionRecord->ExceptionInformation[0];

        Utf8 fileName;
        fileName.append((const char*) location->fileName.buffer, (uint32_t) location->fileName.count);

        Utf8 userMsg;
        if (args->ExceptionRecord->ExceptionInformation[1] && args->ExceptionRecord->ExceptionInformation[2])
            userMsg.append((const char*) args->ExceptionRecord->ExceptionInformation[1], (uint32_t) args->ExceptionRecord->ExceptionInformation[2]);
        else
            userMsg.append("assertion failed");

        // Add current context
        if (runContext->ip && runContext->ip->node && runContext->ip->node->sourceFile)
        {
            runContext->ip--; // ip is the next pointer instruction
            auto path1 = normalizePath(fs::path(runContext->ip->node->sourceFile->path.c_str()));
            auto path2 = normalizePath(fs::path(fileName.c_str()));
            if (path1 != path2 || runContext->ip->getLocation(runContext->bc)->line != location->lineStart)
                runContext->bc->addCallStack(runContext);
        }

        SourceFile dummyFile;
        dummyFile.path = fileName;
        SourceLocation sourceLocation;
        sourceLocation.line   = location->lineStart;
        sourceLocation.column = location->colStart;
        Diagnostic diag{&dummyFile, sourceLocation, userMsg};
        runContext->bc->sourceFile->report(diag);

        return EXCEPTION_EXECUTE_HANDLER;
    }

    // Hardware exception
    auto       ip = runContext->ip - 1;
    Diagnostic diag{ip->node, ip->node->token, "exception during bytecode execution !"};
    diag.exceptionError = true;
    runContext->bc->sourceFile->report(diag);
    return g_CommandLine.devMode ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_EXECUTE_HANDLER;
}

bool ByteCodeRun::run(ByteCodeRunContext* runContext)
{
    auto module = runContext->sourceFile->module;

    __try
    {
        if (!module->runner.runLoop(runContext))
            return false;
    }
    __except (exceptionHandler(runContext, GetExceptionInformation()))
    {
        return false;
    }

    return true;
}

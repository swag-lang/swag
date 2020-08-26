#include "pch.h"
#include "ByteCodeRun.h"
#include "Diagnostic.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Workspace.h"
#include "Context.h"
#include "Diagnostic.h"
#include "DiagnosticInfos.h"
#include "swag_runtime.h"
#include "Module.h"
#include "CompilerItf.h"

#define IMMB_U32(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u32 : registersRC[ip->b.u32].u32)
#define IMMC_U32(ip) ((ip->flags & BCI_IMM_C) ? ip->c.u32 : registersRC[ip->c.u32].u32)

inline bool ByteCodeRun::executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->bc->registersRC[context->bc->curRC];
    auto registersRR = context->registersRR;

    switch (ip->op)
    {
    case ByteCodeOp::IntrinsicS8x1:
    {
        switch ((TokenId) ip->d.u32)
        {
        case TokenId::IntrinsicAbs:
            registersRC[ip->a.u32].s8 = (int8_t) abs(registersRC[ip->b.u32].s8);
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
            registersRC[ip->a.u32].s16 = (int16_t) abs(registersRC[ip->b.u32].s16);
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
            registersRC[ip->a.u32].s32 = abs(registersRC[ip->b.u32].s32);
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
            registersRC[ip->a.u32].s64 = abs(registersRC[ip->b.u32].s64);
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
            registersRC[ip->a.u32].f32 = powf(registersRC[ip->b.u32].f32, registersRC[ip->c.u32].f32);
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
            registersRC[ip->a.u32].f64 = pow(registersRC[ip->b.u32].f64, registersRC[ip->c.u32].f64);
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
            registersRC[ip->a.u32].f32 = sqrtf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicSin:
            registersRC[ip->a.u32].f32 = sinf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicCos:
            registersRC[ip->a.u32].f32 = cosf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicTan:
            registersRC[ip->a.u32].f32 = tanf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicSinh:
            registersRC[ip->a.u32].f32 = sinhf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicCosh:
            registersRC[ip->a.u32].f32 = coshf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicTanh:
            registersRC[ip->a.u32].f32 = tanhf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicASin:
            registersRC[ip->a.u32].f32 = asinf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicACos:
            registersRC[ip->a.u32].f32 = acosf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicATan:
            registersRC[ip->a.u32].f32 = atanf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicLog:
            registersRC[ip->a.u32].f32 = log(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicLog2:
            registersRC[ip->a.u32].f32 = log2(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicLog10:
            registersRC[ip->a.u32].f32 = log10(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicFloor:
            registersRC[ip->a.u32].f32 = floorf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicCeil:
            registersRC[ip->a.u32].f32 = ceilf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicTrunc:
            registersRC[ip->a.u32].f32 = truncf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicRound:
            registersRC[ip->a.u32].f32 = roundf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicAbs:
            registersRC[ip->a.u32].f32 = fabsf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicExp:
            registersRC[ip->a.u32].f32 = expf(registersRC[ip->b.u32].f32);
            break;
        case TokenId::IntrinsicExp2:
            registersRC[ip->a.u32].f32 = exp2f(registersRC[ip->b.u32].f32);
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
            registersRC[ip->a.u32].f64 = sqrt(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicSin:
            registersRC[ip->a.u32].f64 = sin(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicCos:
            registersRC[ip->a.u32].f64 = cos(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicTan:
            registersRC[ip->a.u32].f64 = tan(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicSinh:
            registersRC[ip->a.u32].f64 = sinh(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicCosh:
            registersRC[ip->a.u32].f64 = cosh(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicTanh:
            registersRC[ip->a.u32].f64 = tanh(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicASin:
            registersRC[ip->a.u32].f64 = asin(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicACos:
            registersRC[ip->a.u32].f64 = acos(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicATan:
            registersRC[ip->a.u32].f64 = atan(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicLog:
            registersRC[ip->a.u32].f64 = log(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicLog2:
            registersRC[ip->a.u32].f64 = log2(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicLog10:
            registersRC[ip->a.u32].f64 = log10(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicFloor:
            registersRC[ip->a.u32].f64 = floor(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicCeil:
            registersRC[ip->a.u32].f64 = ceil(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicTrunc:
            registersRC[ip->a.u32].f64 = trunc(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicRound:
            registersRC[ip->a.u32].f64 = round(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicAbs:
            registersRC[ip->a.u32].f64 = fabs(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicExp:
            registersRC[ip->a.u32].f64 = exp(registersRC[ip->b.u32].f64);
            break;
        case TokenId::IntrinsicExp2:
            registersRC[ip->a.u32].f64 = exp2(registersRC[ip->b.u32].f64);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
        break;
    }

    case ByteCodeOp::TestNotZero8:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u8 != 0;
        break;
    }
    case ByteCodeOp::TestNotZero16:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u16 != 0;
        break;
    }
    case ByteCodeOp::TestNotZero32:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u32 != 0;
        break;
    }
    case ByteCodeOp::TestNotZero64:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u64 != 0;
        break;
    }

    case ByteCodeOp::JumpIfZero32:
    {
        if (!registersRC[ip->a.u32].u32)
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
        context->ip += ip->a.s32;
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

        // Foreign lambda
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
        registersRC[ip->a.u32].pointer = (uint8_t*) ffiGetFuncAddress(context, funcNode);

        // If this assert, then Houston we have a problem. At one point in time, i was using the lower bit to determine if a lambda is bytecode or native.
        // But thanks to clang, it seems that the function pointer could have it's lower bit set (optim level 1).
        // So now its the highest bit.
        SWAG_ASSERT(!isByteCodeLambda(registersRC[ip->a.u32].pointer));
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
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer - registersRC[ip->b.u32].u32;
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
    {
        context->push(registersRC[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::IncSP:
    {
        context->incSP(ip->a.u32);
        break;
    }
    case ByteCodeOp::DecSP:
    {
        context->decSP(ip->a.u32);
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
    case ByteCodeOp::MemCmp:
    {
        void*    dst               = registersRC[ip->b.u32].pointer;
        void*    src               = registersRC[ip->c.u32].pointer;
        uint32_t size              = registersRC[ip->d.u32].u32;
        registersRC[ip->a.u32].s32 = memcmp(dst, src, size);
        break;
    }

    case ByteCodeOp::CopyRBtoRA:
    {
        registersRC[ip->a.u32] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRBAddrToRA:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) (registersRC + ip->b.u32);
        break;
    }
    case ByteCodeOp::CopyRAVB32:
    {
        registersRC[ip->a.u32].u32 = ip->b.u32;
        break;
    }
    case ByteCodeOp::CopyRAVB64:
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
    case ByteCodeOp::AddVBtoRA32:
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
    case ByteCodeOp::CopySPtoBP:
    {
        context->bp = context->sp;
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

    case ByteCodeOp::BinOpModuloS32:
    {
        auto val1                  = registersRC[ip->a.u32].s32;
        auto val2                  = registersRC[ip->b.u32].s32;
        registersRC[ip->c.u32].s32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloS64:
    {
        auto val1                  = registersRC[ip->a.u32].s64;
        auto val2                  = registersRC[ip->b.u32].s64;
        registersRC[ip->c.u32].s64 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloU32:
    {
        auto val1                  = registersRC[ip->a.u32].u32;
        auto val2                  = registersRC[ip->b.u32].u32;
        registersRC[ip->c.u32].u32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloU64:
    {
        auto val1                  = registersRC[ip->a.u32].u64;
        auto val2                  = registersRC[ip->b.u32].u64;
        registersRC[ip->c.u32].u64 = val1 % val2;
        break;
    }

    case ByteCodeOp::BinOpPlusS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 + registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpPlusS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 + registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpPlusF32:
    {
        registersRC[ip->c.u32].f32 = registersRC[ip->a.u32].f32 + registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::BinOpPlusF64:
    {
        registersRC[ip->c.u32].f64 = registersRC[ip->a.u32].f64 + registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpMinusS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 - registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpMinusS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 - registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpMinusF32:
    {
        registersRC[ip->c.u32].f32 = registersRC[ip->a.u32].f32 - registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::BinOpMinusF64:
    {
        registersRC[ip->c.u32].f64 = registersRC[ip->a.u32].f64 - registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::Mul64byVB32:
    {
        registersRC[ip->a.u32].s32 *= ip->b.u32;
        break;
    }
    case ByteCodeOp::Div64byVB32:
    {
        registersRC[ip->a.u32].s64 /= ip->b.u32;
        break;
    }

    case ByteCodeOp::BinOpMulS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 * registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpMulS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 * registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpMulF32:
    {
        registersRC[ip->c.u32].f32 = registersRC[ip->a.u32].f32 * registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::BinOpMulF64:
    {
        registersRC[ip->c.u32].f64 = registersRC[ip->a.u32].f64 * registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpDivS32:
    {
        auto val1                  = registersRC[ip->a.u32].s32;
        auto val2                  = registersRC[ip->b.u32].s32;
        registersRC[ip->c.u32].s32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivS64:
    {
        auto val1                  = registersRC[ip->a.u32].s64;
        auto val2                  = registersRC[ip->b.u32].s64;
        registersRC[ip->c.u32].s64 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivU32:
    {
        auto val1                  = registersRC[ip->a.u32].u32;
        auto val2                  = registersRC[ip->b.u32].u32;
        registersRC[ip->c.u32].u32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivU64:
    {
        auto val1                  = registersRC[ip->a.u32].u64;
        auto val2                  = registersRC[ip->b.u32].u64;
        registersRC[ip->c.u32].u64 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivF32:
    {
        auto val1                  = registersRC[ip->a.u32].f32;
        auto val2                  = registersRC[ip->b.u32].f32;
        registersRC[ip->c.u32].f32 = val1 / val2;
        break;
    }

    case ByteCodeOp::BinOpDivF64:
    {
        auto val1                  = registersRC[ip->a.u32].f64;
        auto val2                  = registersRC[ip->b.u32].f64;
        registersRC[ip->c.u32].f64 = val1 / val2;
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
        registersRC[ip->a.u32].pointer = (uint8_t*) getCompilerItf(context->sourceFile->module);
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
        msg.append((const char*) registersRC[ip->b.u32].pointer, registersRC[ip->c.u32].u32);
        context->error(msg, (ConcreteCompilerSourceLocation*) registersRC[ip->a.u32].pointer);
        break;
    }
    case ByteCodeOp::IntrinsicAssert:
    {
        if (!registersRC[ip->a.u32].b)
            context->error(ip->d.pointer ? (const char*) ip->d.pointer : "assertion failed");
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
        registersRC[ip->a.u32].pointer = (uint8_t*) swag_runtime_tlsGetValue(g_tlsContextId);
        break;
    }
    case ByteCodeOp::IntrinsicSetContext:
    {
        swag_runtime_tlsSetValue(g_tlsContextId, registersRC[ip->a.u32].pointer);
        break;
    }
    case ByteCodeOp::IntrinsicPrintF64:
    {
        g_Log.lock();
        swag_runtime_print_f64(registersRC[ip->a.u32].f64);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintS64:
    {
        g_Log.lock();
        swag_runtime_print_i64(registersRC[ip->a.u32].s64);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintString:
    {
        g_Log.lock();
        swag_runtime_print_n(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].u32);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicInterfaceOf:
    {
        registersRC[ip->c.u32].pointer = (uint8_t*) swag_runtime_interfaceof(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].pointer);
        break;
    }

    case ByteCodeOp::SetAtPointer8:
    {
        auto ptr                      = registersRC[ip->a.u32].pointer;
        *(uint8_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::SetAtPointer16:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint16_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::SetAtPointer32:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint32_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::SetAtPointer64:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint64_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::CompareOpEqual8:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u8 == registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::CompareOpEqual16:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u16 == registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::CompareOpEqual32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u32 == registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CompareOpEqual64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u64 == registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::CompareOpEqualString:
    {
        registersRC[ip->c.u32].b = swag_runtime_compareString(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].pointer, registersRC[ip->c.u32].u32, registersRC[ip->d.u32].u32);
        break;
    }
    case ByteCodeOp::CompareOpEqualTypeInfo:
    {
        registersRC[ip->c.u32].b = swag_runtime_compareType(registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].pointer);
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

    case ByteCodeOp::CompareOpLowerS32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s32 < registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::CompareOpLowerS64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s64 < registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::CompareOpLowerU32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u32 < registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CompareOpLowerU64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u64 < registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::CompareOpLowerF32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f32 < registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::CompareOpLowerF64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f64 < registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::CompareOpGreaterS32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s32 > registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::CompareOpGreaterS64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s64 > registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::CompareOpGreaterU32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u32 > registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CompareOpGreaterU64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u64 > registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::CompareOpGreaterF32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f32 > registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::CompareOpGreaterF64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f64 > registersRC[ip->b.u32].f64;
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

    case ByteCodeOp::BinOpAnd:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].b && registersRC[ip->b.u32].b;
        break;
    }
    case ByteCodeOp::BinOpOr:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].b || registersRC[ip->b.u32].b;
        break;
    }

    case ByteCodeOp::BinOpBitmaskAndS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 & registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpBitmaskAndS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 & registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOrS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 | registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOrS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 | registersRC[ip->b.u32].s64;
        break;
    }

    case ByteCodeOp::BinOpShiftLeftU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 << registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpShiftLeftU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 << registersRC[ip->b.u32].u32;
        break;
    }

    case ByteCodeOp::BinOpShiftRightS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 >> registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpShiftRightS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 >> registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpShiftRightU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 >> registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpShiftRightU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 >> registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpShiftRightU64VB:
    {
        registersRC[ip->a.u32].u64 >>= ip->b.u32;
        break;
    }

    case ByteCodeOp::BinOpXorU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 ^ registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpXorU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 ^ registersRC[ip->b.u32].u64;
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
        *(int8_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].f64;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqPointer:
    {
        *(uint8_t**) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqPointer:
    {
        *(uint8_t**) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s32;
        break;
    }

    case ByteCodeOp::AffectOpMinusEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::AffectOpMulEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpMulEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpMulEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpMulEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpMulEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::AffectOpMulEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::AffectOpDivEqS8:
    {
        auto val = registersRC[ip->b.u32].s8;
        *(int8_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqS16:
    {
        auto val = registersRC[ip->b.u32].s16;
        *(int16_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqS32:
    {
        auto val = registersRC[ip->b.u32].s32;
        *(int32_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqS64:
    {
        auto val = registersRC[ip->b.u32].s64;
        *(int64_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU8:
    {
        auto val = registersRC[ip->b.u32].u8;
        *(uint8_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU16:
    {
        auto val = registersRC[ip->b.u32].u16;
        *(uint16_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU32:
    {
        auto val = registersRC[ip->b.u32].u32;
        *(uint32_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU64:
    {
        auto val = registersRC[ip->b.u32].u64;
        *(uint64_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqF32:
    {
        auto val = registersRC[ip->b.u32].f32;
        *(float*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqF64:
    {
        auto val = registersRC[ip->b.u32].f64;
        *(double*) registersRC[ip->a.u32].pointer /= val;
        break;
    }

    case ByteCodeOp::AffectOpAndEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpAndEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpAndEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpAndEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s64;
        break;
    }

    case ByteCodeOp::AffectOpOrEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpOrEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpOrEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpOrEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s64;
        break;
    }

    case ByteCodeOp::AffectOpXOrEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s64;
        break;
    }

    case ByteCodeOp::AffectOpShiftLeftEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }

    case ByteCodeOp::AffectOpShiftRightEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }

    case ByteCodeOp::AffectOpModuloEqS8:
    {
        auto val2 = registersRC[ip->b.u32].s8;
        *(int8_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS16:
    {
        auto val2 = registersRC[ip->b.u32].s16;
        *(int16_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS32:
    {
        auto val2 = registersRC[ip->b.u32].s32;
        *(int32_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS64:
    {
        auto val2 = registersRC[ip->b.u32].s64;
        *(int64_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU8:
    {
        auto val2 = registersRC[ip->b.u32].u8;
        *(uint8_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU16:
    {
        auto val2 = registersRC[ip->b.u32].u16;
        *(uint16_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU32:
    {
        auto val2 = registersRC[ip->b.u32].u32;
        *(uint32_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU64:
    {
        auto val2 = registersRC[ip->b.u32].u64;
        *(uint64_t*) registersRC[ip->a.u32].pointer %= val2;
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
            if (context->errorLoc)
            {
                SourceLocation start = {(int) context->errorLoc->lineStart - 1, (int) context->errorLoc->colStart - 1};
                SourceLocation end   = {(int) context->errorLoc->lineEnd - 1, (int) context->errorLoc->colEnd - 1};
                Diagnostic     diag{ip->node->sourceFile, start, end, context->errorMsg};
                diag.criticalError = true;
                context->report(diag);
            }
            else
            {
                auto location = ip->getLocation(context->bc);
                if (location)
                {
                    Diagnostic diag{ip->node->sourceFile, *location, "error during bytecode execution, " + context->errorMsg};
                    diag.criticalError = true;
                    context->report(diag);
                }
                else
                {
                    Diagnostic diag{ip->node, ip->node->token, "error during bytecode execution, " + context->errorMsg};
                    diag.criticalError = true;
                    context->report(diag);
                }
            }

            return false;
        }
    }

    return true;
}

static int exceptionHandler(ByteCodeRunContext* runContext)
{
    auto       ip = runContext->ip - 1;
    Diagnostic diag{ip->node, ip->node->token, "exception during bytecode execution !"};
    diag.criticalError  = true;
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
    __except (exceptionHandler(runContext))
    {
        return false;
    }

    return true;
}

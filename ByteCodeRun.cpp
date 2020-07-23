#include "pch.h"
#include "ByteCodeRun.h"
#include "Diagnostic.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Workspace.h"
#include "Context.h"
#include "Diagnostic.h"
#include "DiagnosticInfos.h"
#include "SwagRuntime.h"

ByteCodeRun g_Run;

inline bool ByteCodeRun::executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->bc->registersRC[context->bc->curRC];
    auto registersRR = context->registersRR;

    switch (ip->op)
    {
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
    case ByteCodeOp::JumpIfNotTrue:
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
            if (!context->bc)
            {
                context->error("lambda call, dereferencing a null pointer");
                break;
            }

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

    case ByteCodeOp::IncPointerVB32:
    {
        registersRC[ip->a.u32].pointer += ip->b.s32;
        break;
    }
    case ByteCodeOp::IncPointer32:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer + registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::DecPointer32:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer - registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::DeRef8:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint8_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef16:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint16_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef32:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint32_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef64:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint64_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRefPointer:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->b.u32].pointer = *(uint8_t**) (ptr + ip->c.u32);
        break;
    }
    case ByteCodeOp::DeRefStringSlice:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
        {
            uint64_t** ptrptr              = (uint64_t**) ptr;
            registersRC[ip->a.u32].pointer = (uint8_t*) ptrptr[0];
            registersRC[ip->b.u32].u64     = (uint64_t) ptrptr[1];
        }
        break;
    }

    case ByteCodeOp::BoundCheckString:
    {
        uint32_t curOffset = registersRC[ip->a.u32].u32;
        uint32_t maxOffset = registersRC[ip->b.u32].u32;
        if (curOffset > maxOffset)
            context->error(format("index out of range (index is '%u', maximum index is '%u')", curOffset, maxOffset));
        break;
    }
    case ByteCodeOp::BoundCheck:
    {
        uint32_t curOffset = registersRC[ip->a.u32].u32;
        uint32_t maxOffset = registersRC[ip->b.u32].u32;
        if (curOffset >= maxOffset)
            context->error(format("index out of range (index is '%u', maximum index is '%u')", curOffset, maxOffset - 1));
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
    case ByteCodeOp::MemCpyVC32:
    {
        void*    dst  = registersRC[ip->a.u32].pointer;
        void*    src  = registersRC[ip->b.u32].pointer;
        uint32_t size = ip->c.u32;
        swag_runtime_memcpy(dst, src, size);
        break;
    }
    case ByteCodeOp::MemCpy:
    {
        void*    dst  = registersRC[ip->a.u32].pointer;
        void*    src  = registersRC[ip->b.u32].pointer;
        uint32_t size = registersRC[ip->c.u32].u32;

        if (!dst)
        {
            context->error("destination pointer of @memcpy is null");
            break;
        }

        if (!src)
        {
            context->error("source pointer of @memcpy is null");
            break;
        }

        memcpy(dst, src, size);
        break;
    }
    case ByteCodeOp::MemSet:
    {
        void*    dst   = registersRC[ip->a.u32].pointer;
        uint32_t value = registersRC[ip->b.u32].u8;
        uint32_t size  = registersRC[ip->c.u32].u32;

        if (!dst)
        {
            context->error("destination pointer of @memset is null");
            break;
        }

        memset(dst, value, size);
        break;
    }
    case ByteCodeOp::MemCmp:
    {
        void*    dst  = registersRC[ip->b.u32].pointer;
        void*    src  = registersRC[ip->c.u32].pointer;
        uint32_t size = registersRC[ip->d.u32].u32;

        if (!dst)
        {
            context->error("destination pointer of @memcmp is null");
            break;
        }

        if (!dst)
        {
            context->error("source pointer of @memcmp is null");
            break;
        }

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
    case ByteCodeOp::CopyVBtoRA32:
    {
        registersRC[ip->a.u32].u32 = ip->b.u32;
        break;
    }
    case ByteCodeOp::CopyVBtoRA64:
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
        context->push(registersRR[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::PopRR:
    {
        registersRR[ip->a.u32].u64 = context->pop<uint64_t>();
        break;
    }

    case ByteCodeOp::CopyRCtoRR:
    case ByteCodeOp::CopyRCtoRRCall:
    {
        registersRR[ip->a.u32] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRRtoRC:
    case ByteCodeOp::CopyRRtoRCCall:
    {
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

    case ByteCodeOp::GetFromStack8:
        registersRC[ip->a.u32].u8 = *(uint8_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack16:
        registersRC[ip->a.u32].u16 = *(uint16_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack32:
        registersRC[ip->a.u32].u32 = *(uint32_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack64:
    case ByteCodeOp::GetFromStackParam64:
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::MakePointerToStack:
    case ByteCodeOp::MakePointerToStackParam:
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
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint8_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointer16:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint16_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointer32:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint32_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointer64:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint64_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::SetZeroAtPointerX:
        memset(registersRC[ip->a.u32].pointer, 0, ip->b.u32);
        break;
    case ByteCodeOp::ClearXVar:
        memset(registersRC[ip->a.u32].pointer, 0, registersRC[ip->b.u32].u32 * ip->c.u32);
        break;

    case ByteCodeOp::GetFromDataSeg8:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u8 = *(uint8_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromDataSeg16:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u16 = *(uint16_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromDataSeg32:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u32 = *(uint32_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromDataSeg64:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        break;
    }

    case ByteCodeOp::GetFromBssSeg8:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->bssSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u8 = *(uint8_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromBssSeg16:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->bssSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u16 = *(uint16_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromBssSeg32:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->bssSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u32 = *(uint32_t*) (ip->d.pointer);
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

    case ByteCodeOp::MakeDataSegPointer:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }
    case ByteCodeOp::MakeBssSegPointer:
    {
        auto module = context->sourceFile->module;
        if (!ip->d.pointer)
            ip->d.pointer = module->bssSegment.address(ip->b.u32);
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
    case ByteCodeOp::MakeConstantSegPointerOC:
    {
        auto module = context->sourceFile->module;
        auto offset = (uint32_t)(ip->c.u64 >> 32);
        auto count  = (uint32_t)(ip->c.u64 & 0xFFFFFFFF);
        if (!ip->d.pointer)
            ip->d.pointer = module->constantSegment.address(offset);
        registersRC[ip->a.u32].pointer = ip->d.pointer;
        registersRC[ip->b.u32].u64     = count;
        break;
    }

    case ByteCodeOp::BinOpModuloS32:
    {
        auto val1 = registersRC[ip->a.u32].s32;
        auto val2 = registersRC[ip->b.u32].s32;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            registersRC[ip->c.u32].s32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloS64:
    {
        auto val1 = registersRC[ip->a.u32].s64;
        auto val2 = registersRC[ip->b.u32].s64;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            registersRC[ip->c.u32].s64 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloU32:
    {
        auto val1 = registersRC[ip->a.u32].u32;
        auto val2 = registersRC[ip->b.u32].u32;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            registersRC[ip->c.u32].u32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloU64:
    {
        auto val1 = registersRC[ip->a.u32].u64;
        auto val2 = registersRC[ip->b.u32].u64;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
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
    case ByteCodeOp::DivRAVB:
    {
        if (ip->b.u32 == 0)
            context->error("division by zero");
        else
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
        auto val1 = registersRC[ip->a.u32].s32;
        auto val2 = registersRC[ip->b.u32].s32;
        if (val2 == 0)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].s32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivS64:
    {
        auto val1 = registersRC[ip->a.u32].s64;
        auto val2 = registersRC[ip->b.u32].s64;
        if (val2 == 0)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].s64 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivU32:
    {
        auto val1 = registersRC[ip->a.u32].u32;
        auto val2 = registersRC[ip->b.u32].u32;
        if (val2 == 0)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].u32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivU64:
    {
        auto val1 = registersRC[ip->a.u32].u64;
        auto val2 = registersRC[ip->b.u32].u64;
        if (val2 == 0)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].u64 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivF32:
    {
        auto val1 = registersRC[ip->a.u32].f32;
        auto val2 = registersRC[ip->b.u32].f32;
        if (val2 == 0.0f)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].f32 = val1 / val2;
        break;
    }

    case ByteCodeOp::BinOpDivF64:
    {
        auto val1 = registersRC[ip->a.u32].f64;
        auto val2 = registersRC[ip->b.u32].f64;
        if (val2 == 0.0)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].f64 = val1 / val2;
        break;
    }

    case ByteCodeOp::IntrinsicArguments:
        registersRC[ip->a.u32].pointer = (uint8_t*) g_CommandLine.userArgumentsSlice.first;
        registersRC[ip->b.u32].u64     = (uint64_t) g_CommandLine.userArgumentsSlice.second;
        break;

    case ByteCodeOp::IntrinsicTarget:
        registersRC[ip->a.u32].pointer = (uint8_t*) &context->sourceFile->module->buildParameters.target;
        break;
    case ByteCodeOp::IntrinsicIsByteCode:
        registersRC[ip->a.u32].b = true;
        break;

    case ByteCodeOp::IntrinsicAssert:
        if (!registersRC[ip->a.u32].b)
            context->error("assertion failed");
        break;
    case ByteCodeOp::IntrinsicAssertCastAny:
    {
        if (!registersRC[ip->a.u32].b)
        {
            // Get the names from the 2 typeinfos.
            // Name must be the first pointer of the TypeInfo struct for that to work !
            Utf8 msg;
            SWAG_ASSERT(registersRC[ip->b.u32].pointer);
            SWAG_ASSERT(registersRC[ip->c.u32].pointer);
            const char* msg1 = *(char**) registersRC[ip->b.u32].pointer;
            const char* msg2 = *(char**) registersRC[ip->c.u32].pointer;
            SWAG_ASSERT(msg1);
            SWAG_ASSERT(msg2);
            context->error(format("type '%s' does not match the 'any' type name '%s'", msg1, msg2));
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
        registersRC[ip->a.u32].pointer = (uint8_t*) OS::tlsGetValue(g_tlsContextId);
        break;
    }
    case ByteCodeOp::IntrinsicSetContext:
    {
        OS::tlsSetValue(g_tlsContextId, registersRC[ip->a.u32].pointer);
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
        swag_runtime_print_n((const char*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].u32);
        g_Log.unlock();
        break;
    }

    case ByteCodeOp::SetAtPointer8:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint8_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::SetAtPointer16:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint16_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::SetAtPointer32:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint32_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::SetAtPointer64:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
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
    case ByteCodeOp::CompareOpEqualInterface:
    {
        registersRC[ip->c.u32].b = (((void**) registersRC[ip->a.u32].pointer)[0] == ((void**) registersRC[ip->b.u32].pointer)[0]) &&
                                   (((void**) registersRC[ip->a.u32].pointer)[1] == ((void**) registersRC[ip->b.u32].pointer)[1]);
        break;
    }
    case ByteCodeOp::CompareOpEqualString:
    {
        if (!registersRC[ip->a.u32].pointer || !registersRC[ip->b.u32].pointer)
            registersRC[ip->c.u32].b = registersRC[ip->a.u32].pointer == registersRC[ip->b.u32].pointer;
        else
        {
            auto length              = registersRC[ip->c.u32].u32;
            registersRC[ip->c.u32].b = !strncmp((const char*) registersRC[ip->a.u32].pointer, (const char*) registersRC[ip->b.u32].pointer, length);
        }
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
    case ByteCodeOp::CastU64F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].u64);
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
        if (val == 0)
            context->error("division by zero");
        else
            *(int8_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqS16:
    {
        auto val = registersRC[ip->b.u32].s16;
        if (val == 0)
            context->error("division by zero");
        else
            *(int16_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqS32:
    {
        auto val = registersRC[ip->b.u32].s32;
        if (val == 0)
            context->error("division by zero");
        else
            *(int32_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqS64:
    {
        auto val = registersRC[ip->b.u32].s64;
        if (val == 0)
            context->error("division by zero");
        else
            *(int64_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU8:
    {
        auto val = registersRC[ip->b.u32].u8;
        if (val == 0)
            context->error("division by zero");
        else
            *(uint8_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU16:
    {
        auto val = registersRC[ip->b.u32].u16;
        if (val == 0)
            context->error("division by zero");
        else
            *(uint16_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU32:
    {
        auto val = registersRC[ip->b.u32].u32;
        if (val == 0)
            context->error("division by zero");
        else
            *(uint32_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqU64:
    {
        auto val = registersRC[ip->b.u32].u64;
        if (val == 0)
            context->error("division by zero");
        else
            *(uint64_t*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqF32:
    {
        auto val = registersRC[ip->b.u32].f32;
        if (val == 0.0f)
            context->error("division by zero");
        else
            *(float*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqF64:
    {
        auto val = registersRC[ip->b.u32].f64;
        if (val == 0.0)
            context->error("division by zero");
        else
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
    case ByteCodeOp::AffectOpOrEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpOrEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpOrEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpOrEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u64;
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
    case ByteCodeOp::AffectOpXOrEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u64;
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
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(int8_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS16:
    {
        auto val2 = registersRC[ip->b.u32].s16;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(int16_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS32:
    {
        auto val2 = registersRC[ip->b.u32].s32;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(int32_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqS64:
    {
        auto val2 = registersRC[ip->b.u32].s64;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(int64_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU8:
    {
        auto val2 = registersRC[ip->b.u32].u8;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(uint8_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU16:
    {
        auto val2 = registersRC[ip->b.u32].u16;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(uint16_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU32:
    {
        auto val2 = registersRC[ip->b.u32].u32;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(uint32_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }
    case ByteCodeOp::AffectOpModuloEqU64:
    {
        auto val2 = registersRC[ip->b.u32].u64;
        if (val2 == 0)
            context->error("modulo operand is zero");
        else
            *(uint64_t*) registersRC[ip->a.u32].pointer %= val2;
        break;
    }

    case ByteCodeOp::MinusToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 < 0 ? true : false;
        break;
    case ByteCodeOp::MinusZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 <= 0 ? true : false;
        break;
    case ByteCodeOp::PlusToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 > 0 ? true : false;
        break;
    case ByteCodeOp::PlusZeroToTrue:
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
            Diagnostic diag{ip->node, ip->node->token, "error during bytecode execution, " + context->errorMsg};
            diag.criticalError = true;
            context->report(diag);
            return false;
        }
    }

    return true;
}

static int exceptionHandler(ByteCodeRunContext* runContext)
{
    auto       ip = runContext->ip - 1;
    Diagnostic diag{ip->node, ip->node->token, "exception during bytecode execution !"};
    diag.criticalError = true;
    runContext->bc->sourceFile->report(diag);
    return EXCEPTION_EXECUTE_HANDLER;
}

bool ByteCodeRun::run(ByteCodeRunContext* runContext)
{
    __try
    {
        if (!g_Run.runLoop(runContext))
            return false;
    }
    __except (exceptionHandler(runContext))
    {
        return false;
    }

    return true;
}

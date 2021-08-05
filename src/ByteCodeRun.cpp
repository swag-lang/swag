#include "pch.h"
#include "ByteCodeRun.h"
#include "Diagnostic.h"
#include "ByteCode.h"
#include "Workspace.h"
#include "Context.h"
#include "CompilerItf.h"
#include "ByteCodeStack.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Math.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

#define IMMA_B(ip) ((ip->flags & BCI_IMM_A) ? ip->a.b : registersRC[ip->a.u32].b)
#define IMMB_B(ip) ((ip->flags & BCI_IMM_B) ? ip->b.b : registersRC[ip->b.u32].b)

#define IMMA_U8(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u8 : registersRC[ip->a.u32].u8)
#define IMMA_U16(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u16 : registersRC[ip->a.u32].u16)
#define IMMA_U32(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u32 : registersRC[ip->a.u32].u32)
#define IMMA_U64(ip) ((ip->flags & BCI_IMM_A) ? ip->a.u64 : registersRC[ip->a.u32].u64)
#define IMMA_S8(ip) ((ip->flags & BCI_IMM_A) ? ip->a.s8 : registersRC[ip->a.u32].s8)
#define IMMA_S16(ip) ((ip->flags & BCI_IMM_A) ? ip->a.s16 : registersRC[ip->a.u32].s16)
#define IMMA_S32(ip) ((ip->flags & BCI_IMM_A) ? ip->a.s32 : registersRC[ip->a.u32].s32)
#define IMMA_S64(ip) ((ip->flags & BCI_IMM_A) ? ip->a.s64 : registersRC[ip->a.u32].s64)
#define IMMA_F32(ip) ((ip->flags & BCI_IMM_A) ? ip->a.f32 : registersRC[ip->a.u32].f32)
#define IMMA_F64(ip) ((ip->flags & BCI_IMM_A) ? ip->a.f64 : registersRC[ip->a.u32].f64)

#define IMMB_U8(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u8 : registersRC[ip->b.u32].u8)
#define IMMB_U16(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u16 : registersRC[ip->b.u32].u16)
#define IMMB_U32(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u32 : registersRC[ip->b.u32].u32)
#define IMMB_U64(ip) ((ip->flags & BCI_IMM_B) ? ip->b.u64 : registersRC[ip->b.u32].u64)
#define IMMB_S8(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s8 : registersRC[ip->b.u32].s8)
#define IMMB_S16(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s16 : registersRC[ip->b.u32].s16)
#define IMMB_S32(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s32 : registersRC[ip->b.u32].s32)
#define IMMB_S64(ip) ((ip->flags & BCI_IMM_B) ? ip->b.s64 : registersRC[ip->b.u32].s64)
#define IMMB_F32(ip) ((ip->flags & BCI_IMM_B) ? ip->b.f32 : registersRC[ip->b.u32].f32)
#define IMMB_F64(ip) ((ip->flags & BCI_IMM_B) ? ip->b.f64 : registersRC[ip->b.u32].f64)

#define IMMC_U8(ip) ((ip->flags & BCI_IMM_C) ? ip->c.u8 : registersRC[ip->c.u32].u8)
#define IMMC_U16(ip) ((ip->flags & BCI_IMM_C) ? ip->c.u16 : registersRC[ip->c.u32].u16)
#define IMMC_U32(ip) ((ip->flags & BCI_IMM_C) ? ip->c.u32 : registersRC[ip->c.u32].u32)
#define IMMC_U64(ip) ((ip->flags & BCI_IMM_C) ? ip->c.u64 : registersRC[ip->c.u32].u64)
#define IMMC_S8(ip) ((ip->flags & BCI_IMM_C) ? ip->c.s8 : registersRC[ip->c.u32].s8)
#define IMMC_S16(ip) ((ip->flags & BCI_IMM_C) ? ip->c.s16 : registersRC[ip->c.u32].s16)
#define IMMC_S32(ip) ((ip->flags & BCI_IMM_C) ? ip->c.s32 : registersRC[ip->c.u32].s32)
#define IMMC_S64(ip) ((ip->flags & BCI_IMM_C) ? ip->c.s64 : registersRC[ip->c.u32].s64)
#define IMMC_F32(ip) ((ip->flags & BCI_IMM_C) ? ip->c.f32 : registersRC[ip->c.u32].f32)
#define IMMC_F64(ip) ((ip->flags & BCI_IMM_C) ? ip->c.f64 : registersRC[ip->c.u32].f64)

#define IMMD_U8(ip) ((ip->flags & BCI_IMM_D) ? ip->d.u8 : registersRC[ip->d.u32].u8)
#define IMMD_U16(ip) ((ip->flags & BCI_IMM_D) ? ip->d.u16 : registersRC[ip->d.u32].u16)
#define IMMD_U32(ip) ((ip->flags & BCI_IMM_D) ? ip->d.u32 : registersRC[ip->d.u32].u32)
#define IMMD_U64(ip) ((ip->flags & BCI_IMM_D) ? ip->d.u64 : registersRC[ip->d.u32].u64)

void ByteCodeRun::localCall(ByteCodeRunContext* context, ByteCode* bc, uint32_t popParamsOnRet, uint32_t returnReg)
{
    context->bc->addCallStack(context);
    context->push(context->bp);
    context->push(context->bc);
    context->push(context->ip);
    context->bc = bc;
    SWAG_ASSERT(context->bc);
    SWAG_ASSERT(context->bc->out);
    context->ip = context->bc->out;
    SWAG_ASSERT(context->ip);
    context->bp = context->sp;
    context->bc->enterByteCode(context, popParamsOnRet, returnReg);
}

void ByteCodeRun::callInternalPanic(ByteCodeRunContext* context, ByteCodeInstruction* ip, const char* msg)
{
    auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name__panic);

    SourceFile*     sourceFile;
    SourceLocation* location;
    ByteCode::getLocation(context->bc, ip, &sourceFile, &location, true);

    context->push(msg);
    context->push<uint64_t>(location->column);
    context->push<uint64_t>(location->line);
    context->push(sourceFile->path.c_str());
    localCall(context, bc, 4);
}

void* ByteCodeRun::makeLambda(JobContext* context, AstFuncDecl* funcNode, ByteCode* bc)
{
    if (funcNode && funcNode->isForeign())
    {
        auto funcPtr = ffiGetFuncAddress(context, funcNode);
        if (!funcPtr)
            return nullptr;

        // If this assert, then Houston we have a problem. At one point in time, i was using the lower bit to determine if a lambda is bytecode or native.
        // But thanks to clang, it seems that the function pointer could have it's lower bit set (optim level 1).
        // So now its the highest bit.
        SWAG_ASSERT(!ByteCode::isByteCodeLambda(funcPtr));

        return ByteCode::doForeignLambda(funcPtr);
    }
    else
    {
        SWAG_ASSERT(bc);
        return ByteCode::doByteCodeLambda(bc);
    }
}

inline bool ByteCodeRun::executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->registersRC[context->curRC]->buffer;
    auto registersRR = context->registersRR;

    switch (ip->op)
    {
    case ByteCodeOp::Nop:
    case ByteCodeOp::DebugNop:
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

    case ByteCodeOp::IntrinsicS8x2:
    case ByteCodeOp::IntrinsicS16x2:
    case ByteCodeOp::IntrinsicS32x2:
    case ByteCodeOp::IntrinsicS64x2:
    case ByteCodeOp::IntrinsicU8x2:
    case ByteCodeOp::IntrinsicU16x2:
    case ByteCodeOp::IntrinsicU32x2:
    case ByteCodeOp::IntrinsicU64x2:
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

    case ByteCodeOp::JumpIfZero8:
    {
        if (!IMMA_U8(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfZero16:
    {
        if (!IMMA_U16(ip))
            context->ip += ip->b.s32;
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
        if (!IMMA_U64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotZero8:
    {
        if (IMMA_U8(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotZero16:
    {
        if (IMMA_U16(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotZero32:
    {
        if (IMMA_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotZero64:
    {
        if (IMMA_U64(ip))
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
    case ByteCodeOp::JumpIfNotEqual8:
    {
        if (IMMA_U8(ip) != IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotEqual16:
    {
        if (IMMA_U16(ip) != IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotEqual32:
    {
        if (IMMA_U32(ip) != IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNotEqual64:
    {
        if (IMMA_U64(ip) != IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfEqual8:
    {
        if (IMMA_U8(ip) == IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfEqual16:
    {
        if (IMMA_U16(ip) == IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfEqual32:
    {
        if (IMMA_U32(ip) == IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfEqual64:
    {
        if (IMMA_U64(ip) == IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    }

    case ByteCodeOp::JumpIfLowerU32:
    {
        if (IMMA_U32(ip) < IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerU64:
    {
        if (IMMA_U64(ip) < IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerS32:
    {
        if (IMMA_S32(ip) < IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerS64:
    {
        if (IMMA_S64(ip) < IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerF32:
    {
        if (IMMA_F32(ip) < IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerF64:
    {
        if (IMMA_F64(ip) < IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;
    }

    case ByteCodeOp::JumpIfLowerEqU32:
    {
        if (IMMA_U32(ip) <= IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerEqU64:
    {
        if (IMMA_U64(ip) <= IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerEqS32:
    {
        if (IMMA_S32(ip) <= IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerEqS64:
    {
        if (IMMA_S64(ip) <= IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerEqF32:
    {
        if (IMMA_F32(ip) <= IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfLowerEqF64:
    {
        if (IMMA_F64(ip) <= IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;
    }

    case ByteCodeOp::JumpIfGreaterU32:
    {
        if (IMMA_U32(ip) > IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterU64:
    {
        if (IMMA_U64(ip) > IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterS32:
    {
        if (IMMA_S32(ip) > IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterS64:
    {
        if (IMMA_S64(ip) > IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterF32:
    {
        if (IMMA_F32(ip) > IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterF64:
    {
        if (IMMA_F64(ip) > IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;
    }

    case ByteCodeOp::JumpIfGreaterEqU32:
    {
        if (IMMA_U32(ip) >= IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterEqU64:
    {
        if (IMMA_U64(ip) >= IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterEqS32:
    {
        if (IMMA_S32(ip) >= IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterEqS64:
    {
        if (IMMA_S64(ip) >= IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterEqF32:
    {
        if (IMMA_F32(ip) >= IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfGreaterEqF64:
    {
        if (IMMA_F64(ip) >= IMMC_F64(ip))
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
        if (ip->a.u32)
            context->incSP(ip->a.u32);
        if (context->sp == context->stack + g_CommandLine.stackSizeBC)
            return false;
        context->bc->leaveByteCode(context);
        context->ip = context->pop<ByteCodeInstruction*>();
        context->bc = context->pop<ByteCode*>();
        context->bp = context->pop<uint8_t*>();
        if (context->curRC == context->firstRC)
            return false;

        // Need to pop some parameters, by increasing the stack pointer
        auto popP = context->popParamsOnRet.back();
        context->popParamsOnRet.pop_back();
        context->incSP(popP * sizeof(void*));

        // A register needs to be initialized with the result register
        auto popR = context->returnRegOnRet.back();
        context->returnRegOnRet.pop_back();
        if (popR != UINT32_MAX)
        {
            context->registersRC[context->curRC]->buffer[popR].u64 = context->registersRR[0].u64;

            // Restore RR register to its previous value
            context->registersRR[0].u64 = context->returnRegOnRetRR.back();
            context->returnRegOnRetRR.pop_back();
        }

        break;
    }
    case ByteCodeOp::LocalCall:
    {
        localCall(context, (ByteCode*) ip->a.pointer);
        break;
    }

    case ByteCodeOp::IntrinsicMakeCallback:
    {
        auto ptr = (void*) registersRC[ip->a.u32].pointer;
        if (ByteCode::isByteCodeLambda(ptr))
            registersRC[ip->a.u32].pointer = (uint8_t*) makeCallback(ptr);
        else
            registersRC[ip->a.u32].pointer = (uint8_t*) ByteCode::undoForeignLambda(ptr);
        break;
    }

    case ByteCodeOp::IntrinsicMakeForeign:
    {
        auto ptr                       = (void*) registersRC[ip->a.u32].pointer;
        registersRC[ip->a.u32].pointer = (uint8_t*) ByteCode::doForeignLambda(ptr);
        break;
    }

    case ByteCodeOp::LambdaCall:
    {
        auto ptr = registersRC[ip->a.u32].u64;
        if (ByteCode::isByteCodeLambda((void*) ptr))
        {
            context->bc->addCallStack(context);
            context->push(context->bp);
            context->push(context->bc);
            context->push(context->ip);

            context->bc = (ByteCode*) ByteCode::undoByteCodeLambda((void*) ptr);
            SWAG_ASSERT(context->bc);
            context->ip = context->bc->out;
            SWAG_ASSERT(context->ip);
            context->bp = context->sp;
            context->bc->enterByteCode(context);
        }

        // Marked as foreign, need to resolve address
        else if (ByteCode::isForeignLambda((void*) ptr))
        {
            auto funcPtr = ByteCode::undoForeignLambda((void*) ptr);
            SWAG_ASSERT(funcPtr);
            auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::Lambda);
            ffiCall(context, funcPtr, typeInfoFunc);
        }

        // Normal lambda
        else
        {
            auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::Lambda);
            ffiCall(context, (void*) registersRC[ip->a.u32].pointer, typeInfoFunc);
        }

        break;
    }
    case ByteCodeOp::MakeLambda:
    {
        auto funcNode                  = (AstFuncDecl*) ip->b.pointer;
        registersRC[ip->a.u32].pointer = (uint8_t*) makeLambda(context, funcNode, (ByteCode*) ip->c.pointer);
        break;
    }
    case ByteCodeOp::ForeignCall:
    {
        context->bc->addCallStack(context);
        ffiCall(context, ip);
        g_ByteCodeStack.pop();
        break;
    }

    case ByteCodeOp::IncPointer64:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer + IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::DecPointer64:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer - IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::DeRef8:
    {
        registersRC[ip->a.u32].u64 = *(uint8_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
    }
    case ByteCodeOp::DeRef16:
    {
        registersRC[ip->a.u32].u64 = *(uint16_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
    }
    case ByteCodeOp::DeRef32:
    {
        registersRC[ip->a.u32].u64 = *(uint32_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
    }
    case ByteCodeOp::DeRef64:
    {
        registersRC[ip->a.u32].u64 = *(uint64_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
    }
    case ByteCodeOp::DeRefStringSlice:
    {
        auto       ptr                 = registersRC[ip->a.u32].pointer + ip->c.s64;
        uint64_t** ptrptr              = (uint64_t**) ptr;
        registersRC[ip->a.u32].pointer = (uint8_t*) ptrptr[0];
        registersRC[ip->b.u32].u64     = (uint64_t) ptrptr[1];
        break;
    }

    case ByteCodeOp::PushRVParam:
        switch (ip->b.u32)
        {
        case 1:
            context->push(registersRC[ip->a.u32].u8);
            break;
        case 2:
            context->push(registersRC[ip->a.u32].u16);
            break;
        case 4:
            context->push(registersRC[ip->a.u32].u32);
            break;
        case 8:
            context->push(registersRC[ip->a.u32].u64);
            break;
        }
        break;
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

    case ByteCodeOp::IncSPPostCall:
    {
        context->incSP(ip->a.u32);
        break;
    }
    case ByteCodeOp::DecSPBP:
    {
        context->decSP(ip->a.u32);
        context->bp = context->sp;
        break;
    }

    case ByteCodeOp::MemCpy8:
    {
        auto dst = (uint8_t*) registersRC[ip->a.u32].pointer;
        auto src = (uint8_t*) registersRC[ip->b.u32].pointer;
        *dst     = *src;
        break;
    }
    case ByteCodeOp::MemCpy16:
    {
        auto dst = (uint16_t*) registersRC[ip->a.u32].pointer;
        auto src = (uint16_t*) registersRC[ip->b.u32].pointer;
        *dst     = *src;
        break;
    }
    case ByteCodeOp::MemCpy32:
    {
        auto dst = (uint32_t*) registersRC[ip->a.u32].pointer;
        auto src = (uint32_t*) registersRC[ip->b.u32].pointer;
        *dst     = *src;
        break;
    }
    case ByteCodeOp::MemCpy64:
    {
        auto dst = (uint64_t*) registersRC[ip->a.u32].pointer;
        auto src = (uint64_t*) registersRC[ip->b.u32].pointer;
        *dst     = *src;
        break;
    }
    case ByteCodeOp::MemCpyX:
    {
        void*  dst  = (void*) registersRC[ip->a.u32].pointer;
        void*  src  = (void*) registersRC[ip->b.u32].pointer;
        size_t size = ip->c.u64;
        memcpy(dst, src, size);
        break;
    }

    case ByteCodeOp::IntrinsicMemCpy:
    {
        void*  dst  = (void*) registersRC[ip->a.u32].pointer;
        void*  src  = (void*) registersRC[ip->b.u32].pointer;
        size_t size = registersRC[ip->c.u32].u64;
        memcpy(dst, src, size);
        break;
    }

    case ByteCodeOp::IntrinsicMemMove:
    {
        void*  dst  = (void*) registersRC[ip->a.u32].pointer;
        void*  src  = (void*) registersRC[ip->b.u32].pointer;
        size_t size = IMMC_U64(ip);
        memmove(dst, src, size);
        break;
    }

    case ByteCodeOp::IntrinsicMemSet:
    {
        void*    dst   = (void*) registersRC[ip->a.u32].pointer;
        uint32_t value = registersRC[ip->b.u32].u8;
        size_t   size  = registersRC[ip->c.u32].u64;
        memset(dst, value, size);
        break;
    }

    case ByteCodeOp::IntrinsicMemCmp:
    {
        void*  dst                 = (void*) registersRC[ip->b.u32].pointer;
        void*  src                 = (void*) registersRC[ip->c.u32].pointer;
        size_t size                = registersRC[ip->d.u32].u64;
        registersRC[ip->a.u32].s32 = memcmp(dst, src, size);
        break;
    }

    case ByteCodeOp::IntrinsicCStrLen:
    {
        void* src                  = (void*) registersRC[ip->b.u32].pointer;
        registersRC[ip->a.u32].u64 = strlen((const char*) src);
        break;
    }

    case ByteCodeOp::CopyRBtoRA8:
    {
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::CopyRBtoRA16:
    {
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::CopyRBtoRA32:
    {
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CopyRBtoRA64:
    {
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u64;
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
        registersRC[ip->a.u32].u64 = ip->b.u32;
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

    case ByteCodeOp::IncrementRA32:
    {
        registersRC[ip->a.u32].u32++;
        break;
    }
    case ByteCodeOp::DecrementRA32:
    {
        registersRC[ip->a.u32].u32--;
        break;
    }
    case ByteCodeOp::IncrementRA64:
    {
        registersRC[ip->a.u32].u64++;
        break;
    }
    case ByteCodeOp::DecrementRA64:
    {
        registersRC[ip->a.u32].u64--;
        break;
    }

    case ByteCodeOp::Add32byVB32:
    {
        registersRC[ip->a.u32].u32 += ip->b.u32;
        break;
    }
    case ByteCodeOp::Add64byVB64:
    {
        registersRC[ip->a.u32].u64 += ip->b.u64;
        break;
    }

    case ByteCodeOp::PushRR:
    {
        context->push(registersRR[0].u64);
        context->push(registersRR[1].u64);
        break;
    }
    case ByteCodeOp::PopRR:
    {
        registersRR[1].u64 = context->pop<uint64_t>();
        registersRR[0].u64 = context->pop<uint64_t>();
        break;
    }

    case ByteCodeOp::CopyRCtoRT:
    {
        registersRR[0] = registersRC[ip->a.u32];
        break;
    }
    case ByteCodeOp::CopyRCtoRR:
    {
        registersRR[0].u64 = IMMA_U64(ip);
        break;
    }
    case ByteCodeOp::CopyRCtoRR2:
    {
        registersRR[0] = registersRC[ip->a.u32];
        registersRR[1] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRRtoRC:
    {
        registersRC[ip->a.u32] = registersRR[0];
        break;
    }
    case ByteCodeOp::CopyRTtoRC:
    {
        registersRC[ip->a.u32] = registersRR[0];
        break;
    }
    case ByteCodeOp::CopyRTtoRC2:
    {
        registersRC[ip->a.u32] = registersRR[0];
        registersRC[ip->b.u32] = registersRR[1];
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
        registersRC[ip->a.u32].pointer = context->sp - ip->b.u32;
        break;

    case ByteCodeOp::CopySPVaargs:
        registersRC[ip->a.u32].pointer = context->sp + ip->b.u32;
        break;

    case ByteCodeOp::IntrinsicIsConstExprSI:
    {
        registersRC[ip->a.u32].b = executeIsConstExprSI(context, ip);
        break;
    }

    case ByteCodeOp::GetFromStackParam64SI:
        executeGetFromStackSI(context, ip);
        break;

    case ByteCodeOp::GetFromStack8:
        registersRC[ip->a.u32].u64 = 0;
        registersRC[ip->a.u32].u8  = *(uint8_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack16:
        registersRC[ip->a.u32].u64 = 0;
        registersRC[ip->a.u32].u16 = *(uint16_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack32:
        registersRC[ip->a.u32].u64 = 0;
        registersRC[ip->a.u32].u32 = *(uint32_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack64:
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack64x2:
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        registersRC[ip->c.u32].u64 = *(uint64_t*) (context->bp + ip->d.u32);
        break;
    case ByteCodeOp::GetFromStackParam64:
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::MakeStackPointer:
    case ByteCodeOp::MakeStackPointerParam:
        registersRC[ip->a.u32].pointer = context->bp + ip->b.u32;
        break;
    case ByteCodeOp::MakeStackPointerRT:
        registersRR[0].pointer = context->bp + ip->a.u32;
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
        memset((void*) (registersRC[ip->a.u32].pointer + ip->c.u32), 0, ip->b.u64);
        break;
    case ByteCodeOp::SetZeroAtPointerXRB:
        memset((void*) registersRC[ip->a.u32].pointer, 0, registersRC[ip->b.u32].u64 * ip->c.u64);
        break;

    case ByteCodeOp::GetFromMutableSeg64:
    {
        auto module = context->sourceFile->module;
        if (OS::atomicTestNull((void**) &ip->d.pointer))
            OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->mutableSegment.address(ip->b.u32));
        registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromBssSeg64:
    {
        auto module = context->sourceFile->module;
        if (OS::atomicTestNull((void**) &ip->d.pointer))
            OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->bssSegment.address(ip->b.u32));
        registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        break;
    }
    case ByteCodeOp::GetFromCompilerSeg64:
    {
        auto module = context->sourceFile->module;
        if (OS::atomicTestNull((void**) &ip->d.pointer))
            OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->compilerSegment.address(ip->b.u32));
        registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        break;
    }

    case ByteCodeOp::MakeMutableSegPointer:
    {
        auto module = context->sourceFile->module;
        if (OS::atomicTestNull((void**) &ip->d.pointer))
        {
            OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->mutableSegment.address(ip->b.u32));
            if (module->saveMutableValues && ip->c.pointer)
            {
                SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                module->mutableSegment.saveValue((void*) ip->d.pointer, over->typeInfo->sizeOf, false);
            }
        }

        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }
    case ByteCodeOp::MakeBssSegPointer:
    {
        auto module = context->sourceFile->module;
        if (OS::atomicTestNull((void**) &ip->d.pointer))
        {
            OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->bssSegment.address(ip->b.u32));
            if (ip->c.pointer)
            {
                if (module->saveBssValues)
                {
                    SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                    module->mutableSegment.saveValue((void*) ip->d.pointer, over->typeInfo->sizeOf, true);
                }
                else if (module->bssCannotChange)
                {
                    SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                    context->error(Utf8::format(Msg0431, over->node->token.text.c_str()));
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
        if (OS::atomicTestNull((void**) &ip->d.pointer))
            OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->constantSegment.address(offset));
        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }
    case ByteCodeOp::MakeCompilerSegPointer:
    {
        auto module = context->sourceFile->module;
        auto offset = ip->b.u32;
        if (OS::atomicTestNull((void**) &ip->d.pointer))
            OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->compilerSegment.address(offset));
        registersRC[ip->a.u32].pointer = ip->d.pointer;
        break;
    }

    case ByteCodeOp::Mul64byVB64:
    {
        registersRC[ip->a.u32].s64 *= ip->b.u64;
        break;
    }
    case ByteCodeOp::Div64byVB64:
    {
        registersRC[ip->a.u32].s64 /= ip->b.u64;
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
        auto val1 = IMMA_S32(ip);
        auto val2 = IMMB_S32(ip);
        if (addOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr.typeInfoS32));
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU32:
    {
        auto val1 = (uint32_t) IMMA_S32(ip);
        auto val2 = (uint32_t) IMMB_S32(ip);
        if (addOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr.typeInfoU32));
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS64:
    {
        auto val1 = IMMA_S64(ip);
        auto val2 = IMMB_S64(ip);
        if (addOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr.typeInfoS64));
        registersRC[ip->c.u32].s64 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU64:
    {
        auto val1 = (uint64_t) IMMA_S64(ip);
        auto val2 = (uint64_t) IMMB_S64(ip);
        if (addOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr.typeInfoU64));
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
        auto val1 = IMMA_S32(ip);
        auto val2 = IMMB_S32(ip);
        if (subOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr.typeInfoS32));
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU32:
    {
        auto val1 = (uint32_t) IMMA_S32(ip);
        auto val2 = (uint32_t) IMMB_S32(ip);
        if (subOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr.typeInfoU32));
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS64:
    {
        auto val1 = IMMA_S64(ip);
        auto val2 = IMMB_S64(ip);
        if (subOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr.typeInfoS64));
        registersRC[ip->c.u32].s64 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU64:
    {
        auto val1 = (uint64_t) IMMA_S64(ip);
        auto val2 = (uint64_t) IMMB_S64(ip);
        if (subOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr.typeInfoU64));
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
        auto val1 = IMMA_S32(ip);
        auto val2 = IMMB_S32(ip);
        if (mulOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr.typeInfoS32));
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU32:
    {
        auto val1 = (uint32_t) IMMA_S32(ip);
        auto val2 = (uint32_t) IMMB_S32(ip);
        if (mulOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr.typeInfoU32));
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS64:
    {
        auto val1 = IMMA_S64(ip);
        auto val2 = IMMB_S64(ip);
        if (mulOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr.typeInfoS64));
        registersRC[ip->c.u32].s64 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU64:
    {
        auto val1 = (uint64_t) IMMA_S64(ip);
        auto val2 = (uint64_t) IMMB_S64(ip);
        if (mulOverflow(ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr.typeInfoU64));
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

    case ByteCodeOp::BinOpBitmaskAnd8:
    {
        auto val1                 = IMMA_U8(ip);
        auto val2                 = IMMB_U8(ip);
        registersRC[ip->c.u32].u8 = val1 & val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskAnd16:
    {
        auto val1                  = IMMA_U16(ip);
        auto val2                  = IMMB_U16(ip);
        registersRC[ip->c.u32].u16 = val1 & val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskAnd32:
    {
        auto val1                  = IMMA_U32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u32 = val1 & val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskAnd64:
    {
        auto val1                  = IMMA_U64(ip);
        auto val2                  = IMMB_U64(ip);
        registersRC[ip->c.u32].u64 = val1 & val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOr8:
    {
        auto val1                 = IMMA_U8(ip);
        auto val2                 = IMMB_U8(ip);
        registersRC[ip->c.u32].u8 = val1 | val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOr16:
    {
        auto val1                  = IMMA_U16(ip);
        auto val2                  = IMMB_U16(ip);
        registersRC[ip->c.u32].u16 = val1 | val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOr32:
    {
        auto val1                  = IMMA_U32(ip);
        auto val2                  = IMMB_U32(ip);
        registersRC[ip->c.u32].u32 = val1 | val2;
        break;
    }
    case ByteCodeOp::BinOpBitmaskOr64:
    {
        auto val1                  = IMMA_U64(ip);
        auto val2                  = IMMB_U64(ip);
        registersRC[ip->c.u32].u64 = val1 | val2;
        break;
    }

    case ByteCodeOp::BinOpShiftLeftU8:
    {
        Register r1, r2;
        r1.u8  = IMMA_U8(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftLeft(context, registersRC + ip->c.u32, r1, r2, 8, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftLeftU16:
    {
        Register r1, r2;
        r1.u16 = IMMA_U16(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftLeft(context, registersRC + ip->c.u32, r1, r2, 16, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftLeftU32:
    {
        Register r1, r2;
        r1.u32 = IMMA_U32(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftLeft(context, registersRC + ip->c.u32, r1, r2, 32, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftLeftU64:
    {
        Register r1, r2;
        r1.u64 = IMMA_U64(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftLeft(context, registersRC + ip->c.u32, r1, r2, 64, ip->flags & BCI_SHIFT_SMALL);
        break;
    }

    case ByteCodeOp::BinOpShiftRightS8:
    {
        Register r1, r2;
        r1.s8  = IMMA_S8(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 8, true, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftRightS16:
    {
        Register r1, r2;
        r1.s16 = IMMA_S16(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 16, true, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftRightS32:
    {
        Register r1, r2;
        r1.s32 = IMMA_S32(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 32, true, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftRightS64:
    {
        Register r1, r2;
        r1.s64 = IMMA_S64(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 64, true, ip->flags & BCI_SHIFT_SMALL);
        break;
    }

    case ByteCodeOp::BinOpShiftRightU8:
    {
        Register r1, r2;
        r1.u8  = IMMA_U8(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 8, false, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftRightU16:
    {
        Register r1, r2;
        r1.u16 = IMMA_U16(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 16, false, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftRightU32:
    {
        Register r1, r2;
        r1.u32 = IMMA_U32(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 32, false, ip->flags & BCI_SHIFT_SMALL);
        break;
    }
    case ByteCodeOp::BinOpShiftRightU64:
    {
        Register r1, r2;
        r1.u64 = IMMA_U64(ip);
        r2.u32 = IMMB_U32(ip);
        executeShiftRight(context, registersRC + ip->c.u32, r1, r2, 64, false, ip->flags & BCI_SHIFT_SMALL);
        break;
    }

    case ByteCodeOp::BinOpXorU8:
    {
        auto val1                 = IMMA_U8(ip);
        auto val2                 = IMMB_U8(ip);
        registersRC[ip->c.u32].u8 = val1 ^ val2;
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

    case ByteCodeOp::IntrinsicErrorMsg:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name_aterrormsg);
        context->push(registersRC[ip->c.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].u64);
        localCall(context, bc, 3);
        break;
    }
    case ByteCodeOp::InternalPanic:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name__panic);

        SourceFile*     sourceFile;
        SourceLocation* location;
        ByteCode::getLocation(context->bc, ip, &sourceFile, &location, true);

        context->push(ip->d.pointer);
        context->push<uint64_t>(location->column);
        context->push<uint64_t>(location->line);
        context->push(sourceFile->path.c_str());
        localCall(context, bc, 4);
        break;
    }
    case ByteCodeOp::IntrinsicPanic:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name_atpanic);
        context->push(registersRC[ip->c.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].u64);
        localCall(context, bc, 3);
        break;
    }

    case ByteCodeOp::InternalInitStackTrace:
    {
        auto cxt        = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
        cxt->traceIndex = 0;
        break;
    }
    case ByteCodeOp::InternalStackTrace:
    {
        auto cxt = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
        if (cxt->traceIndex == MAX_TRACE)
            break;
        cxt->trace[cxt->traceIndex] = (SwagCompilerSourceLocation*) registersRC[ip->a.u32].pointer;
        cxt->traceIndex++;
        break;
    }

    case ByteCodeOp::IntrinsicAlloc:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) malloc(registersRC[ip->b.u32].u64);
        break;
    }
    case ByteCodeOp::IntrinsicRealloc:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) realloc((void*) registersRC[ip->b.u32].pointer, registersRC[ip->c.u32].u64);
        break;
    }
    case ByteCodeOp::IntrinsicFree:
    {
        free((void*) registersRC[ip->a.u32].pointer);
        break;
    }
    case ByteCodeOp::InternalGetTlsPtr:
    {
        auto module = context->sourceFile->module;
        auto bc     = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name__tlsGetPtr);
        module->tlsSegment.makeLinear(); // be sure init segment is not divided in chunks
        context->push(module->tlsSegment.address(0));
        context->push((uint64_t) module->tlsSegment.totalCount);
        context->push(g_TlsThreadLocalId);
        localCall(context, bc, 3, ip->a.u32);
        break;
    }
    case ByteCodeOp::IntrinsicGetContext:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) OS::tlsGetValue(g_TlsContextId);
        break;
    }
    case ByteCodeOp::IntrinsicSetContext:
    {
        OS::tlsSetValue(g_TlsContextId, (void*) registersRC[ip->a.u32].pointer);
        break;
    }

    case ByteCodeOp::IntrinsicSetErr:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name_atseterr);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].pointer);
        localCall(context, bc, 2);
        break;
    }
    case ByteCodeOp::IntrinsicGetErr:
    {
        auto cxt = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
        if (!cxt->errorMsgLen)
        {
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = 0;
        }
        else
        {
            registersRC[ip->a.u32].pointer = cxt->errorMsg + cxt->errorMsgStart;
            registersRC[ip->b.u32].u64     = cxt->errorMsgLen;
        }

        break;
    }
    case ByteCodeOp::InternalClearErr:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name__clearerr);
        localCall(context, bc, 0);
        break;
    }
    case ByteCodeOp::InternalPushErr:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name__pusherr);
        localCall(context, bc, 0);
        break;
    }
    case ByteCodeOp::InternalPopErr:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name__poperr);
        localCall(context, bc, 0);
        break;
    }

    case ByteCodeOp::IntrinsicInterfaceOf:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name_atinterfaceof);
        context->push(registersRC[ip->b.u32].pointer);
        context->push(registersRC[ip->a.u32].pointer);
        localCall(context, bc, 2, ip->c.u32);
        break;
    }

    case ByteCodeOp::SetAtPointer8:
    {
        auto ptr                      = registersRC[ip->a.u32].pointer;
        *(uint8_t*) (ptr + ip->c.u32) = IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::SetAtPointer16:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint16_t*) (ptr + ip->c.u32) = IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::SetAtPointer32:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint32_t*) (ptr + ip->c.u32) = IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::SetAtPointer64:
    {
        auto ptr                       = registersRC[ip->a.u32].pointer;
        *(uint64_t*) (ptr + ip->c.u32) = IMMB_U64(ip);
        break;
    }

    case ByteCodeOp::SetAtStackPointer8:
    {
        auto ptr        = context->bp + ip->a.u32;
        *(uint8_t*) ptr = IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::SetAtStackPointer16:
    {
        auto ptr         = context->bp + ip->a.u32;
        *(uint16_t*) ptr = IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::SetAtStackPointer32:
    {
        auto ptr         = context->bp + ip->a.u32;
        *(uint32_t*) ptr = IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::SetAtStackPointer64:
    {
        auto ptr         = context->bp + ip->a.u32;
        *(uint64_t*) ptr = IMMB_U64(ip);
        break;
    }

    case ByteCodeOp::SetAtStackPointer8x2:
    {
        {
            auto ptr        = context->bp + ip->a.u32;
            *(uint8_t*) ptr = IMMB_U8(ip);
        }
        {
            auto ptr        = context->bp + ip->c.u32;
            *(uint8_t*) ptr = IMMD_U8(ip);
        }
        break;
    }
    case ByteCodeOp::SetAtStackPointer16x2:
    {
        {
            auto ptr         = context->bp + ip->a.u32;
            *(uint16_t*) ptr = IMMB_U16(ip);
        }
        {
            auto ptr         = context->bp + ip->c.u32;
            *(uint16_t*) ptr = IMMD_U16(ip);
        }
        break;
    }
    case ByteCodeOp::SetAtStackPointer32x2:
    {
        {
            auto ptr         = context->bp + ip->a.u32;
            *(uint32_t*) ptr = IMMB_U32(ip);
        }
        {
            auto ptr         = context->bp + ip->c.u32;
            *(uint32_t*) ptr = IMMD_U32(ip);
        }
        break;
    }
    case ByteCodeOp::SetAtStackPointer64x2:
    {
        {
            auto ptr         = context->bp + ip->a.u32;
            *(uint64_t*) ptr = IMMB_U64(ip);
        }
        {
            auto ptr         = context->bp + ip->c.u32;
            *(uint64_t*) ptr = IMMD_U64(ip);
        }
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

    case ByteCodeOp::CompareOpNotEqual8:
    {
        registersRC[ip->c.u32].b = IMMA_U8(ip) != IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::CompareOpNotEqual16:
    {
        registersRC[ip->c.u32].b = IMMA_U16(ip) != IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::CompareOpNotEqual32:
    {
        registersRC[ip->c.u32].b = IMMA_U32(ip) != IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::CompareOpNotEqual64:
    {
        registersRC[ip->c.u32].b = IMMA_U64(ip) != IMMB_U64(ip);
        break;
    }

    case ByteCodeOp::IntrinsicStrCmp:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name_atstrcmp);
        context->push(registersRC[ip->d.u32].u64);
        context->push(registersRC[ip->c.u32].pointer);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].pointer);
        localCall(context, bc, 4, ip->d.u32);
        break;
    }
    case ByteCodeOp::IntrinsicTypeCmp:
    {
        auto bc = g_Workspace.runtimeModule->getRuntimeFct(g_LangSpec.name_attypecmp);
        context->push<uint64_t>(ip->c.u32);
        context->push(registersRC[ip->b.u32].pointer);
        context->push(registersRC[ip->a.u32].pointer);
        localCall(context, bc, 3, ip->d.u32);
        break;
    }
    case ByteCodeOp::CloneString:
    {
        char*    ptr   = (char*) registersRC[ip->a.u32].pointer;
        uint32_t count = registersRC[ip->b.u32].u32;
        if (!count)
            break;
        auto size                      = Allocator::alignSize(count + 1);
        registersRC[ip->a.u32].pointer = (uint8_t*) g_Allocator.alloc(size);
        context->bc->autoFree.push_back({(void*) registersRC[ip->a.u32].pointer, size});
        memcpy((void*) registersRC[ip->a.u32].pointer, ptr, count + 1);
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

    case ByteCodeOp::CompareOpLowerEqS32:
    {
        registersRC[ip->c.u32].b = IMMA_S32(ip) <= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerEqS64:
    {
        registersRC[ip->c.u32].b = IMMA_S64(ip) <= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerEqU32:
    {
        registersRC[ip->c.u32].b = IMMA_U32(ip) <= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerEqU64:
    {
        registersRC[ip->c.u32].b = IMMA_U64(ip) <= IMMB_U64(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerEqF32:
    {
        registersRC[ip->c.u32].b = IMMA_F32(ip) <= IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerEqF64:
    {
        registersRC[ip->c.u32].b = IMMA_F64(ip) <= IMMB_F64(ip);
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

    case ByteCodeOp::CompareOpGreaterEqS32:
    {
        registersRC[ip->c.u32].b = IMMA_S32(ip) >= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterEqS64:
    {
        registersRC[ip->c.u32].b = IMMA_S64(ip) >= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterEqU32:
    {
        registersRC[ip->c.u32].b = IMMA_U32(ip) >= IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterEqU64:
    {
        registersRC[ip->c.u32].b = IMMA_U64(ip) >= IMMB_U64(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterEqF32:
    {
        registersRC[ip->c.u32].b = IMMA_F32(ip) >= IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterEqF64:
    {
        registersRC[ip->c.u32].b = IMMA_F64(ip) >= IMMB_F64(ip);
        break;
    }

    case ByteCodeOp::NegBool:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].b ^ 1;
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

    case ByteCodeOp::InvertU8:
    {
        registersRC[ip->a.u32].u8 = ~registersRC[ip->a.u32].u8;
        break;
    }
    case ByteCodeOp::InvertU16:
    {
        registersRC[ip->a.u32].u16 = ~registersRC[ip->a.u32].u16;
        break;
    }
    case ByteCodeOp::InvertU32:
    {
        registersRC[ip->a.u32].u32 = ~registersRC[ip->a.u32].u32;
        break;
    }
    case ByteCodeOp::InvertU64:
    {
        registersRC[ip->a.u32].u64 = ~registersRC[ip->a.u32].u64;
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
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u8 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool16:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u16 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool32:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u32 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool64:
    {
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].u64 ? true : false;
        break;
    }

    case ByteCodeOp::CastS16S32:
    {
        registersRC[ip->a.u32].s32 = registersRC[ip->a.u32].s16;
        break;
    }
    case ByteCodeOp::CastS16S64:
    {
        registersRC[ip->a.u32].s64 = registersRC[ip->a.u32].s16;
        break;
    }

    case ByteCodeOp::CastF64F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].f64;
        break;
    }
    case ByteCodeOp::CastS8F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].s8;
        break;
    }
    case ByteCodeOp::CastS16F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].s16;
        break;
    }
    case ByteCodeOp::CastS32F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::CastS64F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].s64;
        break;
    }
    case ByteCodeOp::CastU8F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].u8;
        break;
    }
    case ByteCodeOp::CastU16F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].u16;
        break;
    }
    case ByteCodeOp::CastU32F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].u32;
        break;
    }
    case ByteCodeOp::CastU64F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->a.u32].u64;
        break;
    }

    case ByteCodeOp::CastF32F64:
    {
        registersRC[ip->a.u32].f64 = registersRC[ip->a.u32].f32;
        break;
    }
    case ByteCodeOp::CastU8F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].u8;
        break;
    }
    case ByteCodeOp::CastU16F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].u16;
        break;
    }
    case ByteCodeOp::CastU32F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].u32;
        break;
    }
    case ByteCodeOp::CastU64F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].u64;
        break;
    }
    case ByteCodeOp::CastS8F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].s8;
        break;
    }
    case ByteCodeOp::CastS16F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].s16;
        break;
    }
    case ByteCodeOp::CastS32F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::CastS64F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->a.u32].s64;
        break;
    }
    case ByteCodeOp::CastF32S32:
    {
        registersRC[ip->a.u32].s32 = (int32_t) registersRC[ip->a.u32].f32;
        break;
    }
    case ByteCodeOp::CastS8S16:
    {
        registersRC[ip->a.u32].s16 = registersRC[ip->a.u32].s8;
        break;
    }
    case ByteCodeOp::CastS8S32:
    {
        registersRC[ip->a.u32].s32 = registersRC[ip->a.u32].s8;
        break;
    }
    case ByteCodeOp::CastS8S64:
    {
        registersRC[ip->a.u32].s64 = registersRC[ip->a.u32].s8;
        break;
    }
    case ByteCodeOp::CastS32S64:
    {
        registersRC[ip->a.u32].s64 = registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::CastF64S64:
    {
        registersRC[ip->a.u32].s64 = (int64_t) registersRC[ip->a.u32].f64;
        break;
    }

    case ByteCodeOp::AffectOpPlusEqS8:
    {
        if (addOverflow(ip->node, *(int8_t*) registersRC[ip->a.u32].pointer, IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoS8));
        *(int8_t*) registersRC[ip->a.u32].pointer += IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU8:
    {
        if (addOverflow(ip->node, *(uint8_t*) registersRC[ip->a.u32].pointer, (uint8_t) IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoU8));
        *(int8_t*) registersRC[ip->a.u32].pointer += IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS16:
    {
        if (addOverflow(ip->node, *(int16_t*) registersRC[ip->a.u32].pointer, IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoS16));
        *(int16_t*) registersRC[ip->a.u32].pointer += IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU16:
    {
        if (addOverflow(ip->node, *(uint16_t*) registersRC[ip->a.u32].pointer, (uint16_t) IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoU16));
        *(int16_t*) registersRC[ip->a.u32].pointer += IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS32:
    {
        if (addOverflow(ip->node, *(int32_t*) registersRC[ip->a.u32].pointer, IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoS32));
        *(int32_t*) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU32:
    {
        if (addOverflow(ip->node, *(uint32_t*) registersRC[ip->a.u32].pointer, (uint32_t) IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoU32));
        *(int32_t*) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS64:
    {
        if (addOverflow(ip->node, *(int64_t*) registersRC[ip->a.u32].pointer, IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoS64));
        *(int64_t*) registersRC[ip->a.u32].pointer += IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU64:
    {
        if (addOverflow(ip->node, *(uint64_t*) registersRC[ip->a.u32].pointer, (uint64_t) IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlusEq, g_TypeMgr.typeInfoU64));
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

    case ByteCodeOp::AffectOpMinusEqS8:
    {
        if (subOverflow(ip->node, *(int8_t*) registersRC[ip->a.u32].pointer, IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr.typeInfoS8));
        *(int8_t*) registersRC[ip->a.u32].pointer -= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU8:
    {
        if (subOverflow(ip->node, *(uint8_t*) registersRC[ip->a.u32].pointer, (uint8_t) IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr.typeInfoU8));
        *(int8_t*) registersRC[ip->a.u32].pointer -= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS16:
    {
        if (subOverflow(ip->node, *(int16_t*) registersRC[ip->a.u32].pointer, IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr.typeInfoS16));
        *(int16_t*) registersRC[ip->a.u32].pointer -= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU16:
    {
        if (subOverflow(ip->node, *(uint16_t*) registersRC[ip->a.u32].pointer, (uint16_t) IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr.typeInfoU16));
        *(int16_t*) registersRC[ip->a.u32].pointer -= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS32:
    {
        if (subOverflow(ip->node, *(int32_t*) registersRC[ip->a.u32].pointer, IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr.typeInfoS32));
        *(int32_t*) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU32:
    {
        if (subOverflow(ip->node, *(uint32_t*) registersRC[ip->a.u32].pointer, (uint32_t) IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr.typeInfoU32));
        *(int32_t*) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS64:
    {
        if (subOverflow(ip->node, *(int64_t*) registersRC[ip->a.u32].pointer, IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinusEq, g_TypeMgr.typeInfoS64));
        *(int64_t*) registersRC[ip->a.u32].pointer -= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU64:
    {
        if (subOverflow(ip->node, *(uint64_t*) registersRC[ip->a.u32].pointer, (uint64_t) IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr.typeInfoU64));
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
        if (mulOverflow(ip->node, *(int8_t*) registersRC[ip->a.u32].pointer, IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoS8));
        *(int8_t*) registersRC[ip->a.u32].pointer *= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqU8:
    {
        if (mulOverflow(ip->node, *(uint8_t*) registersRC[ip->a.u32].pointer, (uint8_t) IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoU8));
        *(int8_t*) registersRC[ip->a.u32].pointer *= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqS16:
    {
        if (mulOverflow(ip->node, *(int16_t*) registersRC[ip->a.u32].pointer, IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoS16));
        *(int16_t*) registersRC[ip->a.u32].pointer *= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqU16:
    {
        if (mulOverflow(ip->node, *(uint16_t*) registersRC[ip->a.u32].pointer, (uint16_t) IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoU16));
        *(int16_t*) registersRC[ip->a.u32].pointer *= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqS32:
    {
        if (mulOverflow(ip->node, *(int32_t*) registersRC[ip->a.u32].pointer, IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoS32));
        *(int32_t*) registersRC[ip->a.u32].pointer *= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqU32:
    {
        if (mulOverflow(ip->node, *(uint32_t*) registersRC[ip->a.u32].pointer, (uint32_t) IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoU32));
        *(int32_t*) registersRC[ip->a.u32].pointer *= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqS64:
    {
        if (mulOverflow(ip->node, *(int64_t*) registersRC[ip->a.u32].pointer, IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoS64));
        *(int64_t*) registersRC[ip->a.u32].pointer *= IMMB_S64(ip);
        break;
    }
    case ByteCodeOp::AffectOpMulEqU64:
    {
        if (mulOverflow(ip->node, *(uint64_t*) registersRC[ip->a.u32].pointer, (uint64_t) IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMulEq, g_TypeMgr.typeInfoU64));
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

    case ByteCodeOp::AffectOpXorEqU8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer ^= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::AffectOpXorEqU16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer ^= IMMB_S16(ip);
        break;
    }
    case ByteCodeOp::AffectOpXorEqU32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer ^= IMMB_S32(ip);
        break;
    }
    case ByteCodeOp::AffectOpXorEqU64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer ^= IMMB_S64(ip);
        break;
    }

    case ByteCodeOp::AffectOpShiftLeftEqU8:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u8        = *(uint8_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftLeft(context, &rr, rr, r1, 8, ip->flags & BCI_SHIFT_SMALL);
        *(uint8_t*) ptr = rr.u8;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqU16:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u16       = *(uint16_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftLeft(context, &rr, rr, r1, 16, ip->flags & BCI_SHIFT_SMALL);
        *(uint16_t*) ptr = rr.u16;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqU32:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u32       = *(uint32_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftLeft(context, &rr, rr, r1, 32, ip->flags & BCI_SHIFT_SMALL);
        *(uint32_t*) ptr = rr.u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqU64:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u64       = *(uint64_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftLeft(context, &rr, rr, r1, 64, ip->flags & BCI_SHIFT_SMALL);
        *(uint64_t*) ptr = rr.u64;
        break;
    }

    case ByteCodeOp::AffectOpShiftRightEqS8:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u8        = *(uint8_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 8, true, ip->flags & BCI_SHIFT_SMALL);
        *(uint8_t*) ptr = rr.u8;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS16:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u16       = *(uint16_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 16, true, ip->flags & BCI_SHIFT_SMALL);
        *(uint16_t*) ptr = rr.u16;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS32:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u32       = *(uint32_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 32, true, ip->flags & BCI_SHIFT_SMALL);
        *(uint32_t*) ptr = rr.u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS64:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u64       = *(uint64_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 64, true, ip->flags & BCI_SHIFT_SMALL);
        *(uint64_t*) ptr = rr.u64;
        break;
    }

    case ByteCodeOp::AffectOpShiftRightEqU8:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u8        = *(uint8_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 8, false, ip->flags & BCI_SHIFT_SMALL);
        *(uint8_t*) ptr = rr.u8;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU16:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u16       = *(uint16_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 16, false, ip->flags & BCI_SHIFT_SMALL);
        *(uint16_t*) ptr = rr.u16;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU32:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u32       = *(uint32_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 32, false, ip->flags & BCI_SHIFT_SMALL);
        *(uint32_t*) ptr = rr.u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU64:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u64       = *(uint64_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        executeShiftRight(context, &rr, rr, r1, 64, false, ip->flags & BCI_SHIFT_SMALL);
        *(uint64_t*) ptr = rr.u64;
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

    case ByteCodeOp::IntrinsicAtomicAddS8:
        registersRC[ip->c.u32].s8 = OS::atomicAdd((int8_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s8);
        break;
    case ByteCodeOp::IntrinsicAtomicAddS16:
        registersRC[ip->c.u32].s16 = OS::atomicAdd((int16_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s16);
        break;
    case ByteCodeOp::IntrinsicAtomicAddS32:
        registersRC[ip->c.u32].s32 = OS::atomicAdd((int32_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s32);
        break;
    case ByteCodeOp::IntrinsicAtomicAddS64:
        registersRC[ip->c.u32].s64 = OS::atomicAdd((int64_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s64);
        break;

    case ByteCodeOp::IntrinsicAtomicAndS8:
        registersRC[ip->c.u32].s8 = OS::atomicAnd((int8_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s8);
        break;
    case ByteCodeOp::IntrinsicAtomicAndS16:
        registersRC[ip->c.u32].s16 = OS::atomicAnd((int16_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s16);
        break;
    case ByteCodeOp::IntrinsicAtomicAndS32:
        registersRC[ip->c.u32].s32 = OS::atomicAnd((int32_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s32);
        break;
    case ByteCodeOp::IntrinsicAtomicAndS64:
        registersRC[ip->c.u32].s64 = OS::atomicAnd((int64_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s64);
        break;

    case ByteCodeOp::IntrinsicAtomicOrS8:
        registersRC[ip->c.u32].s8 = OS::atomicOr((int8_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s8);
        break;
    case ByteCodeOp::IntrinsicAtomicOrS16:
        registersRC[ip->c.u32].s16 = OS::atomicOr((int16_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s16);
        break;
    case ByteCodeOp::IntrinsicAtomicOrS32:
        registersRC[ip->c.u32].s32 = OS::atomicOr((int32_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s32);
        break;
    case ByteCodeOp::IntrinsicAtomicOrS64:
        registersRC[ip->c.u32].s64 = OS::atomicOr((int64_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s64);
        break;

    case ByteCodeOp::IntrinsicAtomicXorS8:
        registersRC[ip->c.u32].s8 = OS::atomicXor((int8_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s8);
        break;
    case ByteCodeOp::IntrinsicAtomicXorS16:
        registersRC[ip->c.u32].s16 = OS::atomicXor((int16_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s16);
        break;
    case ByteCodeOp::IntrinsicAtomicXorS32:
        registersRC[ip->c.u32].s32 = OS::atomicXor((int32_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s32);
        break;
    case ByteCodeOp::IntrinsicAtomicXorS64:
        registersRC[ip->c.u32].s64 = OS::atomicXor((int64_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s64);
        break;

    case ByteCodeOp::IntrinsicAtomicXchgS8:
        registersRC[ip->c.u32].s8 = OS::atomicXchg((int8_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s8);
        break;
    case ByteCodeOp::IntrinsicAtomicXchgS16:
        registersRC[ip->c.u32].s16 = OS::atomicXchg((int16_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s16);
        break;
    case ByteCodeOp::IntrinsicAtomicXchgS32:
        registersRC[ip->c.u32].s32 = OS::atomicXchg((int32_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s32);
        break;
    case ByteCodeOp::IntrinsicAtomicXchgS64:
        registersRC[ip->c.u32].s64 = OS::atomicXchg((int64_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s64);
        break;

    case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
        registersRC[ip->d.u32].s8 = OS::atomicCmpXchg((int8_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s8, registersRC[ip->c.u32].s8);
        break;
    case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
        registersRC[ip->d.u32].s16 = OS::atomicCmpXchg((int16_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s16, registersRC[ip->c.u32].s16);
        break;
    case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
        registersRC[ip->d.u32].s32 = OS::atomicCmpXchg((int32_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s32, registersRC[ip->c.u32].s32);
        break;
    case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
        registersRC[ip->d.u32].s64 = OS::atomicCmpXchg((int64_t*) registersRC[ip->a.u32].pointer, registersRC[ip->b.u32].s64, registersRC[ip->c.u32].s64);
        break;
    case ByteCodeOp::IntrinsicBcDbg:
        context->debugEntry = !context->debugOn;
        context->debugOn    = true;
        break;

    default:
        if (ip->op < ByteCodeOp::End)
            context->internalError(Utf8::format("unknown bytecode instruction '%s'", g_ByteCodeOpDesc[(int) ip->op].name), ip->node);
        break;
    }

    return true;
}

bool ByteCodeRun::runLoop(ByteCodeRunContext* context)
{
    while (context->ip)
    {
        // Debug
        if (context->debugOn && !debugger(context))
            OS::exit(0);

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
            context->hasError = false;

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
                SourceFile*     sourceFile;
                SourceLocation* location;
                ByteCode::getLocation(context->bc, ip, &sourceFile, &location);
                if (location || !ip->node)
                {
                    Diagnostic diag{sourceFile, *location, Msg0434 + context->errorMsg};
                    errorContext->sourceFile = sourceFile;
                    errorContext->report(diag);
                }
                else
                {
                    Diagnostic diag{ip->node, ip->node->token, Msg0434 + context->errorMsg};
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
    // This is called by panic too, in certain conditions (if we do not want dialog boxes, when running tests for example)
    if (args->ExceptionRecord->ExceptionCode == 666)
    {
        runContext->canCatchError = false;

        auto location = (SwagCompilerSourceLocation*) args->ExceptionRecord->ExceptionInformation[0];
        SWAG_ASSERT(location);

        Utf8 fileName;
        fileName.append((const char*) location->fileName.buffer, (uint32_t) location->fileName.count);

        Utf8 userMsg;
        if (args->ExceptionRecord->ExceptionInformation[1] && args->ExceptionRecord->ExceptionInformation[2])
            userMsg.append((const char*) args->ExceptionRecord->ExceptionInformation[1], (uint32_t) args->ExceptionRecord->ExceptionInformation[2]);
        else
            userMsg.append("panic");

        SourceFile     dummyFile;
        SourceLocation sourceLocation;
        dummyFile.path        = fileName;
        sourceLocation.line   = location->lineStart;
        sourceLocation.column = location->colStart;
        Diagnostic diag{&dummyFile, sourceLocation, userMsg};

        // Retreive calling context, like current expension
        vector<const Diagnostic*> notes;
        if (runContext->callerContext)
        {
            runContext->callerContext->setErrorContext(diag, notes);

            // If we have an expansion, and the first expansion requests test error, then raise
            // in its context to dismiss the error (like an error during a #selectif for example)
            if (runContext->callerContext->expansionNode.size())
            {
                auto firstSrcFile = runContext->callerContext->expansionNode[0].first->sourceFile;
                if (firstSrcFile->numTestErrors || firstSrcFile->numTestWarnings)
                {
                    runContext->ip--;
                    g_ByteCodeStack.currentContext = runContext;
                    firstSrcFile->report(diag, notes);
                    g_ByteCodeStack.currentContext = nullptr;
                    runContext->ip++;
                    return EXCEPTION_EXECUTE_HANDLER;
                }
            }
        }

        SourceFile* sourceFile;
        if (g_ByteCodeStack.steps.size())
            sourceFile = g_ByteCodeStack.steps[0].bc->sourceFile;
        else
            sourceFile = runContext->bc->sourceFile;

        runContext->ip--;
        g_ByteCodeStack.currentContext = runContext;
        sourceFile->report(diag, notes);
        g_ByteCodeStack.currentContext = nullptr;
        runContext->ip++;
        return EXCEPTION_EXECUTE_HANDLER;
    }

// Hardware exception
#ifdef SWAG_DEV_MODE
    OS::errorBox("[Developer Mode]", "Exception raised !");
#endif

    runContext->ip--;
    auto       ip = runContext->ip;
    Diagnostic diag{ip->node, ip->node->token, Msg0435};
    Diagnostic note1{Msg0436, DiagnosticLevel::Note};
    Diagnostic note2{Note009, DiagnosticLevel::Note};
    diag.exceptionError            = true;
    g_ByteCodeStack.currentContext = runContext;
    runContext->bc->sourceFile->report(diag, &note1, &note2);
    g_ByteCodeStack.currentContext = nullptr;
    runContext->ip++;
#ifdef SWAG_DEV_MODE
    return EXCEPTION_CONTINUE_EXECUTION;
#else
    return EXCEPTION_EXECUTE_HANDLER;
#endif
}

bool ByteCodeRun::run(ByteCodeRunContext* runContext)
{
    auto module = runContext->sourceFile->module;

    while (true)
    {
        __try
        {
            return module->runner.runLoop(runContext);
        }
        __except (exceptionHandler(runContext, GetExceptionInformation()))
        {
            if (g_CommandLine.dbgCatch && runContext->canCatchError)
            {
                runContext->ip--;
                runContext->debugOn    = true;
                runContext->debugEntry = true;
                continue;
            }

            g_ByteCodeStack.clear();
            return false;
        }
    }

    return true;
}

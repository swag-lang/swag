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
#include "Report.h"
#include "LanguageSpec.h"
#include "SemanticJob.h"
#include "Module.h"
#include "ByteCodeDebugger.h"

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

SWAG_FORCE_INLINE void ByteCodeRun::localCallNoTrace(ByteCodeRunContext* context, ByteCode* bc, uint32_t popParamsOnRet, uint32_t returnRegOnRet, uint32_t incSPPostCall)
{
    SWAG_ASSERT(!bc->node || bc->node->semFlags & SEMFLAG_BYTECODE_GENERATED);

    context->push(context->bp);
    context->push(context->bc);
    context->push(context->ip);
    context->oldBc = context->bc;
    context->bc    = bc;
    SWAG_ASSERT(context->bc);
    SWAG_ASSERT(context->bc->out);
    context->ip = context->bc->out;
    context->bp = context->sp;
    context->bc->enterByteCode(context, popParamsOnRet, returnRegOnRet, incSPPostCall);
}

SWAG_FORCE_INLINE void ByteCodeRun::localCall(ByteCodeRunContext* context, ByteCode* bc, uint32_t popParamsOnRet, uint32_t returnRegOnRet, uint32_t incSPPostCall)
{
    g_ByteCodeStackTrace->push(context);
    localCallNoTrace(context, bc, popParamsOnRet, returnRegOnRet, incSPPostCall);
}

void ByteCodeRun::callInternalCompilerError(ByteCodeRunContext* context, ByteCodeInstruction* ip, const char* msg)
{
    msg      = _strdup(msg); // Leak and slow, but only for messages
    auto bc  = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__compilererror);
    auto loc = ByteCode::getLocation(context->bc, ip);
    context->push(msg);
    context->push<uint64_t>(loc.location->column);
    context->push<uint64_t>(loc.location->line);
    context->push(_strdup(loc.file->path.string().c_str()));
    localCall(context, bc, 4);
}

void ByteCodeRun::callInternalPanic(ByteCodeRunContext* context, ByteCodeInstruction* ip, const char* msg)
{
    msg      = _strdup(msg); // Leak and slow, but only for messages
    auto bc  = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__panic);
    auto loc = ByteCode::getLocation(context->bc, ip);
    context->push(msg);
    context->push<uint64_t>(loc.location->column);
    context->push<uint64_t>(loc.location->line);
    context->push(_strdup(loc.file->path.string().c_str()));
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

        return funcPtr;
    }
    else
    {
        SWAG_ASSERT(bc);
        return ByteCode::doByteCodeLambda(bc);
    }
}

SWAG_FORCE_INLINE bool ByteCodeRun::executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->curRegistersRC;

    SWAG_ASSERT(ip->op <= ByteCodeOp::End);
    switch (ip->op)
    {
    case ByteCodeOp::End:
        return false;

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
        SWAG_CHECK(executeMathIntrinsic(&context->jc, ip, registersRC[ip->a.u32], rb, {}, {}, !context->forConstExpr));
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
        SWAG_CHECK(executeMathIntrinsic(&context->jc, ip, registersRC[ip->a.u32], rb, rc, {}, !context->forConstExpr));
        break;
    }

    case ByteCodeOp::IntrinsicMulAddF32:
    case ByteCodeOp::IntrinsicMulAddF64:
    {
        auto& rb = (ip->flags & BCI_IMM_B) ? ip->b : registersRC[ip->b.u32];
        auto& rc = (ip->flags & BCI_IMM_C) ? ip->c : registersRC[ip->c.u32];
        auto& rd = (ip->flags & BCI_IMM_D) ? ip->d : registersRC[ip->d.u32];
        SWAG_CHECK(executeMathIntrinsic(&context->jc, ip, registersRC[ip->a.u32], rb, rc, rd, !context->forConstExpr));
        break;
    }

    case ByteCodeOp::TestNotZero8:
        registersRC[ip->a.u32].b = IMMB_U8(ip) != 0;
        break;
    case ByteCodeOp::TestNotZero16:
        registersRC[ip->a.u32].b = IMMB_U16(ip) != 0;
        break;
    case ByteCodeOp::TestNotZero32:
        registersRC[ip->a.u32].b = IMMB_U32(ip) != 0;
        break;
    case ByteCodeOp::TestNotZero64:
        registersRC[ip->a.u32].b = IMMB_U64(ip) != 0;
        break;

    case ByteCodeOp::JumpDyn8:
    {
        int32_t* table = (int32_t*) context->bc->sourceFile->module->compilerSegment.address(ip->d.u32);
        auto     val   = (uint64_t) (registersRC[ip->a.u32].s8 - (ip->b.s8 - 1));
        if (val >= ip->c.u64)
            context->ip += table[0];
        else
            context->ip += table[val];
        break;
    }

    case ByteCodeOp::JumpDyn16:
    {
        int32_t* table = (int32_t*) context->bc->sourceFile->module->compilerSegment.address(ip->d.u32);
        auto     val   = (uint64_t) (registersRC[ip->a.u32].s16 - (ip->b.s16 - 1));
        if (val >= ip->c.u64)
            context->ip += table[0];
        else
            context->ip += table[val];
        break;
    }

    case ByteCodeOp::JumpDyn32:
    {
        int32_t* table = (int32_t*) context->bc->sourceFile->module->compilerSegment.address(ip->d.u32);
        auto     val   = (uint64_t) (registersRC[ip->a.u32].s32 - (ip->b.s32 - 1));
        if (val >= ip->c.u64)
            context->ip += table[0];
        else
            context->ip += table[val];
        break;
    }

    case ByteCodeOp::JumpDyn64:
    {
        int32_t* table = (int32_t*) context->bc->sourceFile->module->compilerSegment.address(ip->d.u32);
        auto     val   = (uint64_t) (registersRC[ip->a.u32].s64 - (ip->b.s64 - 1));
        if (val >= ip->c.u64)
            context->ip += table[0];
        else
            context->ip += table[val];
        break;
    }

    case ByteCodeOp::JumpIfZero8:
        if (!IMMA_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfZero16:
        if (!IMMA_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfZero32:
        if (!IMMA_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfZero64:
        if (!IMMA_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotZero8:
        if (IMMA_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotZero16:
        if (IMMA_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotZero32:
        if (IMMA_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotZero64:
        if (IMMA_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfFalse:
        if (!registersRC[ip->a.u32].b)
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfTrue:
        if (registersRC[ip->a.u32].b)
            context->ip += ip->b.s32;
        break;

    case ByteCodeOp::JumpIfNotEqual8:
        if (IMMA_U8(ip) != IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotEqual16:
        if (IMMA_U16(ip) != IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotEqual32:
        if (IMMA_U32(ip) != IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotEqual64:
        if (IMMA_U64(ip) != IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotEqualF32:
        if (IMMA_F32(ip) != IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfNotEqualF64:
        if (IMMA_F64(ip) != IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;

    case ByteCodeOp::JumpIfEqual8:
        if (IMMA_U8(ip) == IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfEqual16:
        if (IMMA_U16(ip) == IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfEqual32:
        if (IMMA_U32(ip) == IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::IncJumpIfEqual64:
        if (++registersRC[ip->a.u32].u64 == IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfEqual64:
        if (IMMA_U64(ip) == IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfEqualF32:
        if (IMMA_F32(ip) == IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfEqualF64:
        if (IMMA_F64(ip) == IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;

    case ByteCodeOp::JumpIfLowerS8:
        if (IMMA_S8(ip) < IMMC_S8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerS16:
        if (IMMA_S16(ip) < IMMC_S16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerS32:
        if (IMMA_S32(ip) < IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerS64:
        if (IMMA_S64(ip) < IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerU8:
        if (IMMA_U8(ip) < IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerU16:
        if (IMMA_U16(ip) < IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerU32:
        if (IMMA_U32(ip) < IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerU64:
        if (IMMA_U64(ip) < IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerF32:
        if (IMMA_F32(ip) < IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerF64:
        if (IMMA_F64(ip) < IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;

    case ByteCodeOp::JumpIfLowerEqU8:
        if (IMMA_U8(ip) <= IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqU16:
        if (IMMA_U16(ip) <= IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqU32:
        if (IMMA_U32(ip) <= IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqU64:
        if (IMMA_U64(ip) <= IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqS8:
        if (IMMA_S8(ip) <= IMMC_S8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqS16:
        if (IMMA_S16(ip) <= IMMC_S16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqS32:
        if (IMMA_S32(ip) <= IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqS64:
        if (IMMA_S64(ip) <= IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqF32:
        if (IMMA_F32(ip) <= IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfLowerEqF64:
        if (IMMA_F64(ip) <= IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;

    case ByteCodeOp::JumpIfGreaterU8:
        if (IMMA_U8(ip) > IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterU16:
        if (IMMA_U16(ip) > IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterU32:
        if (IMMA_U32(ip) > IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterU64:
        if (IMMA_U64(ip) > IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterS8:
        if (IMMA_S8(ip) > IMMC_S8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterS16:
        if (IMMA_S16(ip) > IMMC_S16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterS32:
        if (IMMA_S32(ip) > IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterS64:
        if (IMMA_S64(ip) > IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterF32:
        if (IMMA_F32(ip) > IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterF64:
        if (IMMA_F64(ip) > IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;

    case ByteCodeOp::JumpIfGreaterEqU8:
        if (IMMA_U8(ip) >= IMMC_U8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqU16:
        if (IMMA_U16(ip) >= IMMC_U16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqU32:
        if (IMMA_U32(ip) >= IMMC_U32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqU64:
        if (IMMA_U64(ip) >= IMMC_U64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqS8:
        if (IMMA_S8(ip) >= IMMC_S8(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqS16:
        if (IMMA_S16(ip) >= IMMC_S16(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqS32:
        if (IMMA_S32(ip) >= IMMC_S32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqS64:
        if (IMMA_S64(ip) >= IMMC_S64(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqF32:
        if (IMMA_F32(ip) >= IMMC_F32(ip))
            context->ip += ip->b.s32;
        break;
    case ByteCodeOp::JumpIfGreaterEqF64:
        if (IMMA_F64(ip) >= IMMC_F64(ip))
            context->ip += ip->b.s32;
        break;

    case ByteCodeOp::Jump:
        context->ip += ip->b.s32;
        break;
    case ByteCodeOp::CopyRCtoRRRet:
        context->registersRR[0].u64 = IMMB_U64(ip);
        // fallthrough
    case ByteCodeOp::Ret:
    {
        context->incSP(ip->a.u32);
        if (context->sp == context->stack + g_CommandLine.stackSizeBC)
            return false;
        context->bc->leaveByteCode(context);
        g_ByteCodeStackTrace->pop();

        context->ip = context->pop<ByteCodeInstruction*>();
        context->bc = context->pop<ByteCode*>();
        context->bp = context->pop<uint8_t*>();
        if (context->curRC == context->firstRC)
        {
            context->popAlt<uint32_t>();
            auto popR = context->popAlt<uint32_t>();
            if (popR != UINT32_MAX)
                context->popAlt<uint64_t>();
            return false;
        }

        // Need to pop some parameters, by increasing the stack pointer
        auto popP = context->popAlt<uint32_t>();
        context->incSP(popP);

        // A register needs to be initialized with the result register
        auto popR = context->popAlt<uint32_t>();
        if (popR != UINT32_MAX)
        {
            context->getRegBuffer(context->curRC)[popR].u64 = context->registersRR[0].u64;

            // Restore RR register to its previous value
            context->registersRR[0].u64 = context->popAlt<uint64_t>();
        }

        break;
    }

    case ByteCodeOp::IntrinsicMakeCallback:
    {
        auto ptr = (void*) registersRC[ip->a.u32].pointer;
        if (ByteCode::isByteCodeLambda(ptr))
            registersRC[ip->a.u32].pointer = (uint8_t*) makeCallback(ptr);
        else
            registersRC[ip->a.u32].pointer = (uint8_t*) ptr;
        break;
    }

    case ByteCodeOp::LocalCallPopParam:
        context->push(registersRC[ip->d.u32].u64);
        localCall(context, (ByteCode*) ip->a.pointer, 0, UINT32_MAX, ip->c.u32);
        break;

    case ByteCodeOp::LocalCall:
        localCall(context, (ByteCode*) ip->a.pointer);
        break;
    case ByteCodeOp::LocalCallPop:
        localCall(context, (ByteCode*) ip->a.pointer, 0, UINT32_MAX, ip->c.u32);
        break;
    case ByteCodeOp::LocalCallPopRC:
        localCall(context, (ByteCode*) ip->a.pointer, 0, ip->d.u32, ip->c.u32);
        break;
    case ByteCodeOp::ForeignCall:
        g_ByteCodeStackTrace->push(context);
        ffiCall(context, ip);
        g_ByteCodeStackTrace->pop();
        break;
    case ByteCodeOp::ForeignCallPop:
        g_ByteCodeStackTrace->push(context);
        ffiCall(context, ip);
        g_ByteCodeStackTrace->pop();
        context->incSP(ip->c.u32);
        break;

    case ByteCodeOp::LambdaCall:
    case ByteCodeOp::LambdaCallPop:
    {
        auto ptr = registersRC[ip->a.u32].u64;
        SWAG_ASSERT(ptr);

        // Bytecode lambda
        if (ByteCode::isByteCodeLambda((void*) ptr))
        {
            g_ByteCodeStackTrace->push(context);
            context->push(context->bp);
            context->push(context->bc);
            context->push(context->ip);

            context->oldBc = context->bc;
            context->bc    = (ByteCode*) ByteCode::undoByteCodeLambda((void*) ptr);
            SWAG_ASSERT(context->bc);
            context->ip = context->bc->out;
            SWAG_ASSERT(context->ip);
            context->bp = context->sp;
            if (ip->op == ByteCodeOp::LambdaCallPop)
                context->bc->enterByteCode(context, 0, UINT32_MAX, ip->c.u32);
            else
                context->bc->enterByteCode(context);
        }

        // Native lambda
        else
        {
            auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::LambdaClosure);
            ffiCall(context, ip, (void*) ptr, typeInfoFunc);
            if (ip->op == ByteCodeOp::LambdaCallPop)
                context->incSP(ip->c.u32);
        }

        break;
    }
    case ByteCodeOp::MakeLambda:
    {
        auto funcNode                  = (AstFuncDecl*) ip->b.pointer;
        registersRC[ip->a.u32].pointer = (uint8_t*) makeLambda(&context->jc, funcNode, (ByteCode*) ip->c.pointer);
        break;
    }

    case ByteCodeOp::IncPointer64:
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer + IMMB_S64(ip);
        break;
    case ByteCodeOp::DecPointer64:
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer - IMMB_S64(ip);
        break;
    case ByteCodeOp::IncMulPointer64:
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer + (IMMB_S64(ip) * ip->d.u64);
        break;

    case ByteCodeOp::DeRef8:
        registersRC[ip->a.u32].u64 = *(uint8_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
    case ByteCodeOp::DeRef16:
        registersRC[ip->a.u32].u64 = *(uint16_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
    case ByteCodeOp::DeRef32:
        registersRC[ip->a.u32].u64 = *(uint32_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
    case ByteCodeOp::DeRef64:
        registersRC[ip->a.u32].u64 = *(uint64_t*) (registersRC[ip->b.u32].pointer + ip->c.s64);
        break;
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
    case ByteCodeOp::PushRAParamCond:
        if (registersRC[ip->a.u32].u64)
            context->push(registersRC[ip->b.u32].u64);
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

    case ByteCodeOp::IncSPPostCallCond:
        if (registersRC[ip->a.u32].u64)
            context->incSP(ip->b.u32);
        break;
    case ByteCodeOp::IncSPPostCall:

        context->incSP(ip->a.u32);
        break;

    case ByteCodeOp::SetBP:
        context->bp = context->sp;
        break;
    case ByteCodeOp::DecSPBP:
        context->decSP(ip->a.u32);
        context->bp = context->sp;
        break;

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

    case ByteCodeOp::IntrinsicMemCpy:
    {
        void*  dst  = (void*) registersRC[ip->a.u32].pointer;
        void*  src  = (void*) registersRC[ip->b.u32].pointer;
        size_t size = IMMC_U64(ip);
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
        uint32_t value = IMMB_U8(ip);
        size_t   size  = IMMC_U64(ip);
        memset(dst, value, size);
        break;
    }

    case ByteCodeOp::IntrinsicMemCmp:
    {
        void*  dst                 = (void*) registersRC[ip->b.u32].pointer;
        void*  src                 = (void*) registersRC[ip->c.u32].pointer;
        size_t size                = IMMD_U64(ip);
        registersRC[ip->a.u32].s32 = memcmp(dst, src, size);
        break;
    }

    case ByteCodeOp::IntrinsicStrLen:
    {
        auto src                   = (const char*) registersRC[ip->b.u32].pointer;
        registersRC[ip->a.u32].u64 = strlen(src);
        break;
    }
    case ByteCodeOp::IntrinsicStrCmp:
    {
        auto src0                  = (const char*) registersRC[ip->b.u32].pointer;
        auto src1                  = (const char*) registersRC[ip->c.u32].pointer;
        registersRC[ip->a.u32].u64 = strcmp(src0, src1);
        break;
    }

    case ByteCodeOp::CopyRBtoRA8:
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u8;
        break;
    case ByteCodeOp::CopyRBtoRA16:
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u16;
        break;
    case ByteCodeOp::CopyRBtoRA32:
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u32;
        break;
    case ByteCodeOp::CopyRBtoRA64:
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u64;
        break;

    case ByteCodeOp::CopyRBtoRA64x2:
        registersRC[ip->a.u32].u64 = registersRC[ip->b.u32].u64;
        registersRC[ip->c.u32].u64 = registersRC[ip->d.u32].u64;
        break;
    case ByteCodeOp::CopyRBAddrToRA:
        registersRC[ip->a.u32].pointer = (uint8_t*) (registersRC + ip->b.u32);
        break;

    case ByteCodeOp::SetImmediate32:
        registersRC[ip->a.u32].u64 = ip->b.u32;
        break;
    case ByteCodeOp::SetImmediate64:
        registersRC[ip->a.u32].u64 = ip->b.u64;
        break;
    case ByteCodeOp::ClearRA:
        registersRC[ip->a.u32].u64 = 0;
        break;
    case ByteCodeOp::ClearRAx2:
        registersRC[ip->a.u32].u64 = 0;
        registersRC[ip->b.u32].u64 = 0;
        break;
    case ByteCodeOp::ClearRAx3:
        registersRC[ip->a.u32].u64 = 0;
        registersRC[ip->b.u32].u64 = 0;
        registersRC[ip->c.u32].u64 = 0;
        break;
    case ByteCodeOp::ClearRAx4:
        registersRC[ip->a.u32].u64 = 0;
        registersRC[ip->b.u32].u64 = 0;
        registersRC[ip->c.u32].u64 = 0;
        registersRC[ip->d.u32].u64 = 0;
        break;

    case ByteCodeOp::DecrementRA32:
        registersRC[ip->a.u32].u32--;
        break;
    case ByteCodeOp::IncrementRA64:
        registersRC[ip->a.u32].u64++;
        break;
    case ByteCodeOp::DecrementRA64:
        registersRC[ip->a.u32].u64--;
        break;

    case ByteCodeOp::Add32byVB32:
        registersRC[ip->a.u32].u32 += ip->b.u32;
        break;
    case ByteCodeOp::Add64byVB64:
        registersRC[ip->a.u32].u64 += ip->b.u64;
        break;

    case ByteCodeOp::PushRR:
        context->push(context->registersRR[0].u64);
        context->push(context->registersRR[1].u64);
        break;
    case ByteCodeOp::PopRR:
        context->registersRR[1].u64 = context->pop<uint64_t>();
        context->registersRR[0].u64 = context->pop<uint64_t>();
        break;

    case ByteCodeOp::CopyRCtoRT:
        context->registersRR[0] = registersRC[ip->a.u32];
        break;
    case ByteCodeOp::CopyRTtoRC:
        registersRC[ip->a.u32] = context->registersRR[0];
        break;

    case ByteCodeOp::CopyRCtoRR:
        context->registersRR[0].u64 = IMMA_U64(ip);
        break;
    case ByteCodeOp::CopyRCtoRR2:
        context->registersRR[0] = registersRC[ip->a.u32];
        context->registersRR[1] = registersRC[ip->b.u32];
        break;
    case ByteCodeOp::CopyRRtoRC:
        registersRC[ip->a.u32].pointer = context->registersRR[0].pointer + ip->b.u64;
        break;
    case ByteCodeOp::CopyRTtoRC2:
        registersRC[ip->a.u32] = context->registersRR[0];
        registersRC[ip->b.u32] = context->registersRR[1];
        break;

    case ByteCodeOp::PushBP:
        context->push(context->bp);
        break;
    case ByteCodeOp::PopBP:
        context->bp = context->pop<uint8_t*>();
        break;
    case ByteCodeOp::CopySP:
        registersRC[ip->a.u32].pointer = context->sp - ip->b.u32;
        break;

    case ByteCodeOp::CopySPVaargs:
        registersRC[ip->a.u32].pointer = context->sp + ip->b.u32;
        break;

    case ByteCodeOp::IntrinsicLocationSI:
        registersRC[ip->a.u32].pointer = (uint8_t*) executeLocationSI(context, ip);
        break;
    case ByteCodeOp::IntrinsicIsConstExprSI:
        registersRC[ip->a.u32].b = executeIsConstExprSI(context, ip);
        break;

    case ByteCodeOp::GetParam64SI:
        executeGetFromStackSI(context, ip);
        break;

    case ByteCodeOp::GetFromStack8:
        SWAG_ASSERT(ip->b.u32 + 1 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 1);
        registersRC[ip->a.u32].u64 = *(uint8_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack16:
        SWAG_ASSERT(ip->b.u32 + 2 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 2);
        registersRC[ip->a.u32].u64 = *(uint16_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack32:
        SWAG_ASSERT(ip->b.u32 + 4 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 4);
        registersRC[ip->a.u32].u64 = *(uint32_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::GetFromStack64:
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::GetFromStack8x2:
        SWAG_ASSERT(ip->b.u32 + 1 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->d.u32 + 1 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 1);
        SWAG_ASSERT(context->bp + ip->d.u32 <= context->stack + g_CommandLine.stackSizeBC - 1);
        registersRC[ip->a.u32].u64 = *(uint8_t*) (context->bp + ip->b.u32);
        registersRC[ip->c.u32].u64 = *(uint8_t*) (context->bp + ip->d.u32);
        break;
    case ByteCodeOp::GetFromStack16x2:
        SWAG_ASSERT(ip->b.u32 + 2 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->d.u32 + 2 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 2);
        SWAG_ASSERT(context->bp + ip->d.u32 <= context->stack + g_CommandLine.stackSizeBC - 2);
        registersRC[ip->a.u32].u64 = *(uint16_t*) (context->bp + ip->b.u32);
        registersRC[ip->c.u32].u64 = *(uint16_t*) (context->bp + ip->d.u32);
        break;
    case ByteCodeOp::GetFromStack32x2:
        SWAG_ASSERT(ip->b.u32 + 4 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->d.u32 + 4 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 4);
        SWAG_ASSERT(context->bp + ip->d.u32 <= context->stack + g_CommandLine.stackSizeBC - 4);
        registersRC[ip->a.u32].u64 = *(uint32_t*) (context->bp + ip->b.u32);
        registersRC[ip->c.u32].u64 = *(uint32_t*) (context->bp + ip->d.u32);
        break;
    case ByteCodeOp::GetFromStack64x2:
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->d.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        SWAG_ASSERT(context->bp + ip->d.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        registersRC[ip->c.u32].u64 = *(uint64_t*) (context->bp + ip->d.u32);
        break;

    case ByteCodeOp::GetIncFromStack64:
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        registersRC[ip->a.u32].u64 += ip->c.u64;
        break;
    case ByteCodeOp::GetIncFromStack64DeRef8:
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        registersRC[ip->a.u32].u64 += ip->c.u64;
        registersRC[ip->a.u32].u64 = *(uint8_t*) registersRC[ip->a.u32].pointer;
        break;
    case ByteCodeOp::GetIncFromStack64DeRef16:
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        registersRC[ip->a.u32].u64 += ip->c.u64;
        registersRC[ip->a.u32].u64 = *(uint16_t*) registersRC[ip->a.u32].pointer;
        break;
    case ByteCodeOp::GetIncFromStack64DeRef32:
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        registersRC[ip->a.u32].u64 += ip->c.u64;
        registersRC[ip->a.u32].u64 = *(uint32_t*) registersRC[ip->a.u32].pointer;
        break;
    case ByteCodeOp::GetIncFromStack64DeRef64:
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        registersRC[ip->a.u32].u64 += ip->c.u64;
        registersRC[ip->a.u32].u64 = *(uint64_t*) registersRC[ip->a.u32].pointer;
        break;

    case ByteCodeOp::CopyStack8:
        SWAG_ASSERT(ip->a.u32 + 1 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->b.u32 + 1 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 1);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 1);
        *(uint8_t*) (context->bp + ip->a.u32) = *(uint8_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::CopyStack16:
        SWAG_ASSERT(ip->a.u32 + 2 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->b.u32 + 2 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 2);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 2);
        *(uint16_t*) (context->bp + ip->a.u32) = *(uint16_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::CopyStack32:
        SWAG_ASSERT(ip->a.u32 + 4 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->b.u32 + 4 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 4);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 4);
        *(uint32_t*) (context->bp + ip->a.u32) = *(uint32_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::CopyStack64:
        SWAG_ASSERT(ip->a.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(ip->b.u32 + 8 <= context->bc->dynStackSize);
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        SWAG_ASSERT(context->bp + ip->b.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        *(uint64_t*) (context->bp + ip->a.u32) = *(uint64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::GetParam8:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 1);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low) & 0xFF;
        break;
    case ByteCodeOp::GetParam16:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 2);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low) & 0xFFFF;
        break;
    case ByteCodeOp::GetParam32:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 4);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low) & 0xFFFFFFFF;
        break;

    case ByteCodeOp::GetParam64:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        break;
    case ByteCodeOp::GetParam64x2:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        SWAG_ASSERT(context->bp + ip->d.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->c.u32].u64 = *(uint64_t*) (context->bp + ip->d.u64u32.low);
        break;
    case ByteCodeOp::GetIncParam64:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 += ip->d.u64;
        break;

    case ByteCodeOp::GetParam64DeRef8:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint8_t*) (registersRC[ip->a.u32].pointer);
        break;
    case ByteCodeOp::GetParam64DeRef16:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint16_t*) (registersRC[ip->a.u32].pointer);
        break;
    case ByteCodeOp::GetParam64DeRef32:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint32_t*) (registersRC[ip->a.u32].pointer);
        break;
    case ByteCodeOp::GetParam64DeRef64:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (registersRC[ip->a.u32].pointer);
        break;

    case ByteCodeOp::GetIncParam64DeRef8:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint8_t*) (registersRC[ip->a.u32].pointer + ip->d.u64);
        break;
    case ByteCodeOp::GetIncParam64DeRef16:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint16_t*) (registersRC[ip->a.u32].pointer + ip->d.u64);
        break;
    case ByteCodeOp::GetIncParam64DeRef32:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint32_t*) (registersRC[ip->a.u32].pointer + ip->d.u64);
        break;
    case ByteCodeOp::GetIncParam64DeRef64:
        SWAG_ASSERT(context->bp + ip->b.u64u32.low <= context->stack + g_CommandLine.stackSizeBC - 8);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u64u32.low);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (registersRC[ip->a.u32].pointer + ip->d.u64);
        break;

    case ByteCodeOp::MakeStackPointer:
        SWAG_ASSERT(context->bp + ip->b.u32 < context->stack + g_CommandLine.stackSizeBC);
        registersRC[ip->a.u32].pointer = context->bp + ip->b.u32;
        break;
    case ByteCodeOp::MakeStackPointerx2:
        SWAG_ASSERT(context->bp + ip->b.u32 < context->stack + g_CommandLine.stackSizeBC);
        registersRC[ip->a.u32].pointer = context->bp + ip->b.u32;
        SWAG_ASSERT(context->bp + ip->d.u32 < context->stack + g_CommandLine.stackSizeBC);
        registersRC[ip->c.u32].pointer = context->bp + ip->d.u32;
        break;
    case ByteCodeOp::MakeStackPointerRT:
        SWAG_ASSERT(context->bp + ip->a.u32 < context->stack + g_CommandLine.stackSizeBC);
        context->registersRR[0].pointer = context->bp + ip->a.u32;
        break;

    case ByteCodeOp::SetZeroStack8:
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 1);
        *(uint8_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStack16:
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 2);
        *(uint16_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStack32:
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 4);
        *(uint32_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStack64:
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - 8);
        *(uint64_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::SetZeroStackX:
        SWAG_ASSERT(context->bp + ip->a.u32 <= context->stack + g_CommandLine.stackSizeBC - ip->b.u32);
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

    case ByteCodeOp::ClearRR8:
    {
        auto ptr                      = context->registersRR[0].pointer;
        *(uint8_t*) (ptr + ip->c.u32) = 0;
        break;
    }
    case ByteCodeOp::ClearRR16:
    {
        auto ptr                       = context->registersRR[0].pointer;
        *(uint16_t*) (ptr + ip->c.u32) = 0;
        break;
    }
    case ByteCodeOp::ClearRR32:
    {
        auto ptr                       = context->registersRR[0].pointer;
        *(uint32_t*) (ptr + ip->c.u32) = 0;
        break;
    }
    case ByteCodeOp::ClearRR64:
    {
        auto ptr                       = context->registersRR[0].pointer;
        *(uint64_t*) (ptr + ip->c.u32) = 0;
        break;
    }
    case ByteCodeOp::ClearRRX:
    {
        auto ptr = context->registersRR[0].pointer;
        memset(ptr, 0, ip->b.u64);
        break;
    }

    case ByteCodeOp::GetFromMutableSeg8:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        // As code in runtime is shared between modules, we cannot cache the pointer, because we could have
        // the cache initiliazed by one module, and used by another one, which is bad (because the first module
        // could be destroyed)
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint8_t*) module->mutableSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->mutableSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint8_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromMutableSeg16:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        // As code in runtime is shared between modules, we cannot cache the pointer, because we could have
        // the cache initiliazed by one module, and used by another one, which is bad (because the first module
        // could be destroyed)
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint16_t*) module->mutableSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->mutableSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint16_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromMutableSeg32:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        // As code in runtime is shared between modules, we cannot cache the pointer, because we could have
        // the cache initiliazed by one module, and used by another one, which is bad (because the first module
        // could be destroyed)
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint32_t*) module->mutableSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->mutableSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint32_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromMutableSeg64:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        // As code in runtime is shared between modules, we cannot cache the pointer, because we could have
        // the cache initiliazed by one module, and used by another one, which is bad (because the first module
        // could be destroyed)
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint64_t*) module->mutableSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->mutableSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        }
        break;
    }

    case ByteCodeOp::GetFromBssSeg8:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint8_t*) module->bssSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->bssSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint8_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromBssSeg16:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint16_t*) module->bssSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->bssSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint16_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromBssSeg32:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint32_t*) module->bssSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->bssSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint32_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromBssSeg64:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint64_t*) module->bssSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->bssSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        }
        break;
    }

    case ByteCodeOp::GetFromCompilerSeg8:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint8_t*) module->compilerSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->compilerSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint8_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromCompilerSeg16:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint16_t*) module->compilerSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->compilerSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint16_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromCompilerSeg32:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint32_t*) module->compilerSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->compilerSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint32_t*) (ip->d.pointer);
        }
        break;
    }
    case ByteCodeOp::GetFromCompilerSeg64:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].u64 = *(uint64_t*) module->compilerSegment.address(ip->b.u32);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->compilerSegment.address(ip->b.u32));
            registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->d.pointer);
        }
        break;
    }

    case ByteCodeOp::MakeMutableSegPointer:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
        {
            auto ptr = module->mutableSegment.address(ip->b.u32);
            if (module->saveMutableValues && ip->c.pointer)
            {
                SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                module->mutableSegment.saveValue((void*) ptr, over->typeInfo->sizeOf, false);
            }

            registersRC[ip->a.u32].pointer = ptr;
        }
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
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
        }
        break;
    }
    case ByteCodeOp::MakeBssSegPointer:
    {
        auto module = context->jc.sourceFile->module;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
        {
            auto ptr = module->bssSegment.address(ip->b.u32);
            if (ip->c.pointer)
            {
                if (module->saveBssValues)
                {
                    SymbolOverload* over = (SymbolOverload*) ip->c.pointer;
                    module->mutableSegment.saveValue((void*) ptr, over->typeInfo->sizeOf, true);
                }
                else if (module->bssCannotChange)
                {
                    SymbolOverload* over         = (SymbolOverload*) ip->c.pointer;
                    context->internalPanicSymbol = over;
                    context->internalPanicHint   = Nte(Nte1005);
                    callInternalPanic(context, ip, Fmt(Err(Err0431), over->node->token.ctext()));
                }
            }
            registersRC[ip->a.u32].pointer = ptr;
        }
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
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
                        SymbolOverload* over         = (SymbolOverload*) ip->c.pointer;
                        context->internalPanicSymbol = over;
                        context->internalPanicHint   = Nte(Nte1005);
                        callInternalPanic(context, ip, Fmt(Err(Err0431), over->node->token.ctext()));
                    }
                }
            }
            registersRC[ip->a.u32].pointer = ip->d.pointer;
        }
        break;
    }
    case ByteCodeOp::MakeConstantSegPointer:
    {
        auto module = context->jc.sourceFile->module;
        auto offset = ip->b.u32;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].pointer = module->constantSegment.address(offset);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->constantSegment.address(offset));
            registersRC[ip->a.u32].pointer = ip->d.pointer;
        }
        break;
    }
    case ByteCodeOp::MakeCompilerSegPointer:
    {
        auto module = context->jc.sourceFile->module;
        auto offset = ip->b.u32;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].pointer = module->compilerSegment.address(offset);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->compilerSegment.address(offset));
            registersRC[ip->a.u32].pointer = ip->d.pointer;
        }
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

    case ByteCodeOp::BinOpModuloS8:
    {
        auto val1                  = IMMA_S8(ip);
        auto val2                  = IMMB_S8(ip);
        registersRC[ip->c.u32].s32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloS16:
    {
        auto val1                  = IMMA_S16(ip);
        auto val2                  = IMMB_S16(ip);
        registersRC[ip->c.u32].s32 = val1 % val2;
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
    case ByteCodeOp::BinOpModuloU8:
    {
        auto val1                  = IMMA_U8(ip);
        auto val2                  = IMMB_U8(ip);
        registersRC[ip->c.u32].u32 = val1 % val2;
        break;
    }
    case ByteCodeOp::BinOpModuloU16:
    {
        auto val1                  = IMMA_U16(ip);
        auto val2                  = IMMB_U16(ip);
        registersRC[ip->c.u32].u32 = val1 % val2;
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

    case ByteCodeOp::BinOpPlusS8:
    {
        auto val1 = IMMA_S8(ip);
        auto val2 = IMMB_S8(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS8));
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS16:
    {
        auto val1 = IMMA_S16(ip);
        auto val2 = IMMB_S16(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS16));
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS32:
    {
        auto val1 = IMMA_S32(ip);
        auto val2 = IMMB_S32(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS32));
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS64:
    {
        auto val1 = IMMA_S64(ip);
        auto val2 = IMMB_S64(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS64));
        registersRC[ip->c.u32].s64 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU8:
    {
        auto val1 = (uint8_t) IMMA_S8(ip);
        auto val2 = (uint8_t) IMMB_S8(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU8));
        registersRC[ip->c.u32].u32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU16:
    {
        auto val1 = (uint16_t) IMMA_S16(ip);
        auto val2 = (uint16_t) IMMB_S16(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU16));
        registersRC[ip->c.u32].u32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU32:
    {
        auto val1 = (uint32_t) IMMA_S32(ip);
        auto val2 = (uint32_t) IMMB_S32(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU32));
        registersRC[ip->c.u32].u32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU64:
    {
        auto val1 = (uint64_t) IMMA_S64(ip);
        auto val2 = (uint64_t) IMMB_S64(ip);
        if (addWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU64));
        registersRC[ip->c.u32].u64 = val1 + val2;
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

    case ByteCodeOp::BinOpPlusS8_Safe:
    {
        auto val1                  = IMMA_S8(ip);
        auto val2                  = IMMB_S8(ip);
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS16_Safe:
    {
        auto val1                  = IMMA_S16(ip);
        auto val2                  = IMMB_S16(ip);
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS32_Safe:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusS64_Safe:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU8_Safe:
    {
        auto val1                  = (uint8_t) IMMA_S8(ip);
        auto val2                  = (uint8_t) IMMB_S8(ip);
        registersRC[ip->c.u32].u32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU16_Safe:
    {
        auto val1                  = (uint16_t) IMMA_S16(ip);
        auto val2                  = (uint16_t) IMMB_S16(ip);
        registersRC[ip->c.u32].u32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU32_Safe:
    {
        auto val1                  = (uint32_t) IMMA_S32(ip);
        auto val2                  = (uint32_t) IMMB_S32(ip);
        registersRC[ip->c.u32].u32 = val1 + val2;
        break;
    }
    case ByteCodeOp::BinOpPlusU64_Safe:
    {
        auto val1                  = (uint64_t) IMMA_S64(ip);
        auto val2                  = (uint64_t) IMMB_S64(ip);
        registersRC[ip->c.u32].u64 = val1 + val2;
        break;
    }

    case ByteCodeOp::BinOpMinusS8:
    {
        auto val1 = IMMA_S8(ip);
        auto val2 = IMMB_S8(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS8));
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS16:
    {
        auto val1 = IMMA_S16(ip);
        auto val2 = IMMB_S16(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS16));
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS32:
    {
        auto val1 = IMMA_S32(ip);
        auto val2 = IMMB_S32(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS32));
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS64:
    {
        auto val1 = IMMA_S64(ip);
        auto val2 = IMMB_S64(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS64));
        registersRC[ip->c.u32].s64 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU8:
    {
        auto val1 = (uint8_t) IMMA_S8(ip);
        auto val2 = (uint8_t) IMMB_S8(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU8));
        registersRC[ip->c.u32].u32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU16:
    {
        auto val1 = (uint16_t) IMMA_S16(ip);
        auto val2 = (uint16_t) IMMB_S16(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU16));
        registersRC[ip->c.u32].u16 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU32:
    {
        auto val1 = (uint32_t) IMMA_S32(ip);
        auto val2 = (uint32_t) IMMB_S32(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU32));
        registersRC[ip->c.u32].u32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU64:
    {
        auto val1 = (uint64_t) IMMA_S64(ip);
        auto val2 = (uint64_t) IMMB_S64(ip);
        if (subWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64));
        registersRC[ip->c.u32].u64 = val1 - val2;
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

    case ByteCodeOp::BinOpMinusS8_Safe:
    {
        auto val1                  = IMMA_S8(ip);
        auto val2                  = IMMB_S8(ip);
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS16_Safe:
    {
        auto val1                  = IMMA_S16(ip);
        auto val2                  = IMMB_S16(ip);
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS32_Safe:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusS64_Safe:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].s64 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU8_Safe:
    {
        auto val1                  = (uint8_t) IMMA_S8(ip);
        auto val2                  = (uint8_t) IMMB_S8(ip);
        registersRC[ip->c.u32].u32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU16_Safe:
    {
        auto val1                  = (uint16_t) IMMA_S16(ip);
        auto val2                  = (uint16_t) IMMB_S16(ip);
        registersRC[ip->c.u32].u32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU32_Safe:
    {
        auto val1                  = (uint32_t) IMMA_S32(ip);
        auto val2                  = (uint32_t) IMMB_S32(ip);
        registersRC[ip->c.u32].u32 = val1 - val2;
        break;
    }
    case ByteCodeOp::BinOpMinusU64_Safe:
    {
        auto val1                  = (uint64_t) IMMA_S64(ip);
        auto val2                  = (uint64_t) IMMB_S64(ip);
        registersRC[ip->c.u32].u64 = val1 - val2;
        break;
    }

    case ByteCodeOp::BinOpMulS8:
    {
        auto val1 = IMMA_S8(ip);
        auto val2 = IMMB_S8(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS8));
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS16:
    {
        auto val1 = IMMA_S16(ip);
        auto val2 = IMMB_S16(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS16));
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS32:
    {
        auto val1 = IMMA_S32(ip);
        auto val2 = IMMB_S32(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS32));
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS64:
    {
        auto val1 = IMMA_S64(ip);
        auto val2 = IMMB_S64(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS64));
        registersRC[ip->c.u32].s64 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU8:
    {
        auto val1 = (uint8_t) IMMA_S8(ip);
        auto val2 = (uint8_t) IMMB_S8(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU8));
        registersRC[ip->c.u32].u32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU16:
    {
        auto val1 = (uint16_t) IMMA_S16(ip);
        auto val2 = (uint16_t) IMMB_S16(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU16));
        registersRC[ip->c.u32].u32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU32:
    {
        auto val1 = (uint32_t) IMMA_S32(ip);
        auto val2 = (uint32_t) IMMB_S32(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU32));
        registersRC[ip->c.u32].u32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU64:
    {
        auto val1 = (uint64_t) IMMA_S64(ip);
        auto val2 = (uint64_t) IMMB_S64(ip);
        if (mulWillOverflow(ip, ip->node, val1, val2))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU64));
        registersRC[ip->c.u32].u64 = val1 * val2;
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

    case ByteCodeOp::BinOpMulS8_Safe:
    {
        auto val1                  = IMMA_S8(ip);
        auto val2                  = IMMB_S8(ip);
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS16_Safe:
    {
        auto val1                  = IMMA_S16(ip);
        auto val2                  = IMMB_S16(ip);
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS32_Safe:
    {
        auto val1                  = IMMA_S32(ip);
        auto val2                  = IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulS64_Safe:
    {
        auto val1                  = IMMA_S64(ip);
        auto val2                  = IMMB_S64(ip);
        registersRC[ip->c.u32].u64 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU8_Safe:
    {
        auto val1                  = (uint8_t) IMMA_S8(ip);
        auto val2                  = (uint8_t) IMMB_S8(ip);
        registersRC[ip->c.u32].u32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU16_Safe:
    {
        auto val1                  = (uint16_t) IMMA_S16(ip);
        auto val2                  = (uint16_t) IMMB_S16(ip);
        registersRC[ip->c.u32].u32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU32_Safe:
    {
        auto val1                  = (uint32_t) IMMA_S32(ip);
        auto val2                  = (uint32_t) IMMB_S32(ip);
        registersRC[ip->c.u32].u32 = val1 * val2;
        break;
    }
    case ByteCodeOp::BinOpMulU64_Safe:
    {
        auto val1                  = (uint64_t) IMMA_S64(ip);
        auto val2                  = (uint64_t) IMMB_S64(ip);
        registersRC[ip->c.u32].u64 = val1 * val2;
        break;
    }

    case ByteCodeOp::BinOpDivS8:
    {
        auto val1                  = IMMA_S8(ip);
        auto val2                  = IMMB_S8(ip);
        registersRC[ip->c.u32].s32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivS16:
    {
        auto val1                  = IMMA_S16(ip);
        auto val2                  = IMMB_S16(ip);
        registersRC[ip->c.u32].s32 = val1 / val2;
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
    case ByteCodeOp::BinOpDivU8:
    {
        auto val1                  = IMMA_U8(ip);
        auto val2                  = IMMB_U8(ip);
        registersRC[ip->c.u32].u32 = val1 / val2;
        break;
    }
    case ByteCodeOp::BinOpDivU16:
    {
        auto val1                  = IMMA_U16(ip);
        auto val2                  = IMMB_U16(ip);
        registersRC[ip->c.u32].u32 = val1 / val2;
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

    case ByteCodeOp::BinOpShiftLeftS8:
    case ByteCodeOp::BinOpShiftLeftU8:
    {
        Register r1, r2;
        r1.u8  = IMMA_U8(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(registersRC + ip->c.u32, r1, r2, 8, false);
        break;
    }
    case ByteCodeOp::BinOpShiftLeftS16:
    case ByteCodeOp::BinOpShiftLeftU16:
    {
        Register r1, r2;
        r1.u16 = IMMA_U16(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(registersRC + ip->c.u32, r1, r2, 16, false);
        break;
    }
    case ByteCodeOp::BinOpShiftLeftS32:
    case ByteCodeOp::BinOpShiftLeftU32:
    {
        Register r1, r2;
        r1.u32 = IMMA_U32(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(registersRC + ip->c.u32, r1, r2, 32, false);
        break;
    }
    case ByteCodeOp::BinOpShiftLeftS64:
    case ByteCodeOp::BinOpShiftLeftU64:
    {
        Register r1, r2;
        r1.u64 = IMMA_U64(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(registersRC + ip->c.u32, r1, r2, 64, false);
        break;
    }

    case ByteCodeOp::BinOpShiftRightS8:
    {
        Register r1, r2;
        r1.s8  = IMMA_S8(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 8, true);
        break;
    }
    case ByteCodeOp::BinOpShiftRightS16:
    {
        Register r1, r2;
        r1.s16 = IMMA_S16(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 16, true);
        break;
    }
    case ByteCodeOp::BinOpShiftRightS32:
    {
        Register r1, r2;
        r1.s32 = IMMA_S32(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 32, true);
        break;
    }
    case ByteCodeOp::BinOpShiftRightS64:
    {
        Register r1, r2;
        r1.s64 = IMMA_S64(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 64, true);
        break;
    }

    case ByteCodeOp::BinOpShiftRightU8:
    {
        Register r1, r2;
        r1.u8  = IMMA_U8(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 8, false);
        break;
    }
    case ByteCodeOp::BinOpShiftRightU16:
    {
        Register r1, r2;
        r1.u16 = IMMA_U16(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 16, false);
        break;
    }
    case ByteCodeOp::BinOpShiftRightU32:
    {
        Register r1, r2;
        r1.u32 = IMMA_U32(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 32, false);
        break;
    }
    case ByteCodeOp::BinOpShiftRightU64:
    {
        Register r1, r2;
        r1.u64 = IMMA_U64(ip);
        r2.u32 = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(registersRC + ip->c.u32, r1, r2, 64, false);
        break;
    }

    case ByteCodeOp::BinOpXorU8:
    {
        auto val1                 = IMMA_U8(ip);
        auto val2                 = IMMB_U8(ip);
        registersRC[ip->c.u32].u8 = val1 ^ val2;
        break;
    }
    case ByteCodeOp::BinOpXorU16:
    {
        auto val1                  = IMMA_U16(ip);
        auto val2                  = IMMB_U16(ip);
        registersRC[ip->c.u32].u16 = val1 ^ val2;
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
    case ByteCodeOp::IntrinsicModules:
    {
        auto module = context->jc.sourceFile->module;
        if (module->modulesSliceOffset == UINT32_MAX)
        {
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = 0;
        }
        else
        {
            registersRC[ip->a.u32].pointer = (uint8_t*) module->modulesSlice;
            registersRC[ip->b.u32].u64     = module->moduleDependencies.count + 1;
        }
        break;
    }
    case ByteCodeOp::IntrinsicGvtd:
    {
        auto module = context->jc.sourceFile->module;
        if (module->globalVarsToDropSliceOffset == UINT32_MAX)
        {
            registersRC[ip->a.u32].pointer = nullptr;
            registersRC[ip->b.u32].u64     = 0;
        }
        else
        {
            registersRC[ip->a.u32].pointer = (uint8_t*) module->globalVarsToDropSlice;
            registersRC[ip->b.u32].u64     = module->globalVarsToDrop.count + 1;
        }
        break;
    }
    case ByteCodeOp::IntrinsicCompiler:
    {
        auto itf                       = (uint8_t**) getCompilerItf(context->jc.sourceFile->module);
        registersRC[ip->a.u32].pointer = itf[0];
        registersRC[ip->b.u32].pointer = itf[1];
        break;
    }
    case ByteCodeOp::IntrinsicIsByteCode:
    {
        registersRC[ip->a.u32].b = true;
        break;
    }

    case ByteCodeOp::IntrinsicCompilerError:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atcompilererror);
        context->push(registersRC[ip->c.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].u64);
        localCall(context, bc, 3);
        break;
    }
    case ByteCodeOp::IntrinsicCompilerWarning:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atcompilerwarning);
        context->push(registersRC[ip->c.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].u64);
        localCall(context, bc, 3);
        break;
    }
    case ByteCodeOp::InternalPanic:
    {
        auto bc  = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__panic);
        auto loc = ByteCode::getLocation(context->bc, ip);

        context->push(ip->d.pointer);
        context->push<uint64_t>(loc.location->column);
        context->push<uint64_t>(loc.location->line);
        context->push(_strdup(loc.file->path.string().c_str()));
        localCall(context, bc, 4);
        break;
    }
    case ByteCodeOp::IntrinsicPanic:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atpanic);
        context->push(registersRC[ip->c.u32].u64);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].u64);
        localCall(context, bc, 3);
        break;
    }

    case ByteCodeOp::InternalUnreachable:
        break;
    case ByteCodeOp::Unreachable:
    {
        auto bc  = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__panic);
        auto loc = ByteCode::getLocation(context->bc, ip);

        context->push((const uint8_t*) "executing unreachable code");
        context->push<uint64_t>(loc.location->column);
        context->push<uint64_t>(loc.location->line);
        context->push(_strdup(loc.file->path.string().c_str()));
        localCall(context, bc, 4);
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
        auto module = context->jc.sourceFile->module;
        auto bc     = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__tlsGetPtr);
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
    case ByteCodeOp::IntrinsicGetProcessInfos:
    {
        auto module                    = context->jc.sourceFile->module;
        registersRC[ip->a.u32].pointer = (uint8_t*) &module->processInfos;
        break;
    }

    case ByteCodeOp::IntrinsicCVaStart:
    {
        auto ptr = (void**) registersRC[ip->a.u32].pointer;
        *ptr     = context->bp + ip->b.u32;
        break;
    }
    case ByteCodeOp::IntrinsicCVaEnd:
        break;
    case ByteCodeOp::IntrinsicCVaArg:
    {
        auto ptr = *(uint64_t**) registersRC[ip->a.u32].pointer;
        switch (ip->c.u32)
        {
        case 1:
            registersRC[ip->b.u32].u64 = *(uint8_t*) ptr;
            break;
        case 2:
            registersRC[ip->b.u32].u64 = *(uint16_t*) ptr;
            break;
        case 4:
            registersRC[ip->b.u32].u64 = *(uint32_t*) ptr;
            break;
        case 8:
            registersRC[ip->b.u32].u64 = *ptr;
            break;
        }
        ptr += 1;
        *(uint64_t**) registersRC[ip->a.u32].pointer = ptr;
        break;
    }

    case ByteCodeOp::InternalFailedAssume:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__failedAssume);
        context->push(registersRC[ip->a.u32].pointer);
        localCall(context, bc, 1);
        break;
    }
    case ByteCodeOp::InternalSetErr:
    {
        // PushRR
        context->push(context->registersRR[0].u64);
        context->push(context->registersRR[1].u64);

        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__seterr);
        context->push(registersRC[ip->b.u32].pointer);
        context->push(registersRC[ip->a.u32].pointer);
        localCall(context, bc, 2);
        break;
    }
    case ByteCodeOp::IntrinsicGetErr:
    {
        auto cxt                       = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
        registersRC[ip->a.u32].pointer = (uint8_t*) cxt->curError.value;
        registersRC[ip->b.u32].pointer = (uint8_t*) cxt->curError.type;
        break;
    }
    case ByteCodeOp::InternalHasErr:
    {
        SWAG_ASSERT(context->bc->registerGetContext != UINT32_MAX);
        auto cxt                   = (SwagContext*) registersRC[ip->b.u32].pointer;
        registersRC[ip->a.u32].u64 = cxt->hasError != 0;
        break;
    }
    case ByteCodeOp::JumpIfError:
    {
        SWAG_ASSERT(context->bc->registerGetContext != UINT32_MAX);
        auto cxt = (SwagContext*) registersRC[ip->a.u32].pointer;
        if (cxt->hasError)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpIfNoError:
    {
        SWAG_ASSERT(context->bc->registerGetContext != UINT32_MAX);
        auto cxt = (SwagContext*) registersRC[ip->a.u32].pointer;
        if (!cxt->hasError)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::InternalPushErr:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__pusherr);
        localCall(context, bc, 0);
        break;
    }
    case ByteCodeOp::InternalPopErr:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__poperr);
        localCall(context, bc, 0);
        break;
    }
    case ByteCodeOp::InternalCatchErr:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name__catcherr);
        localCall(context, bc, 0);
        break;
    }
    case ByteCodeOp::InternalInitStackTrace:
    {
        SWAG_ASSERT(context->bc->registerGetContext != UINT32_MAX);
        auto cxt = (SwagContext*) registersRC[ip->a.u32].pointer;
        SWAG_ASSERT(cxt != nullptr);
        cxt->traceIndex = 0;
        break;
    }
    case ByteCodeOp::InternalStackTraceConst:
    {
        auto cxt = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
        if (cxt->traceIndex == SWAG_MAX_TRACES)
            break;
        cxt->traces[cxt->traceIndex] = (SwagSourceCodeLocation*) registersRC[ip->a.u32].pointer;
        cxt->traceIndex++;
        break;
    }
    case ByteCodeOp::InternalStackTrace:
    {
        auto cxt = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
        if (cxt->traceIndex == SWAG_MAX_TRACES)
            break;

        auto module = context->jc.sourceFile->module;
        auto offset = ip->b.u32;

        // :SharedRuntimeBC
        if (ip->node && ip->node->sourceFile && ip->node->sourceFile->isRuntimeFile)
            registersRC[ip->a.u32].pointer = module->constantSegment.address(offset);
        else
        {
            SWAG_ASSERT(!ip->node || !ip->node->sourceFile || !ip->node->sourceFile->isBootstrapFile);
            if (OS::atomicTestNull((void**) &ip->d.pointer))
                OS::atomicSetIfNotNull((void**) &ip->d.pointer, module->constantSegment.address(offset));
            registersRC[ip->a.u32].pointer = ip->d.pointer;
        }

        cxt->traces[cxt->traceIndex] = (SwagSourceCodeLocation*) registersRC[ip->a.u32].pointer;
        cxt->traceIndex++;
        break;
    }

    case ByteCodeOp::IntrinsicItfTableOf:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atitftableof);
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
        SWAG_ASSERT(ip->a.u32 + 1 <= context->bc->dynStackSize);
        auto ptr        = context->bp + ip->a.u32;
        *(uint8_t*) ptr = IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::SetAtStackPointer16:
    {
        SWAG_ASSERT(ip->a.u32 + 2 <= context->bc->dynStackSize);
        auto ptr         = context->bp + ip->a.u32;
        *(uint16_t*) ptr = IMMB_U16(ip);
        break;
    }
    case ByteCodeOp::SetAtStackPointer32:
    {
        SWAG_ASSERT(ip->a.u32 + 4 <= context->bc->dynStackSize);
        auto ptr         = context->bp + ip->a.u32;
        *(uint32_t*) ptr = IMMB_U32(ip);
        break;
    }
    case ByteCodeOp::SetAtStackPointer64:
    {
        SWAG_ASSERT(ip->a.u32 + 8 <= context->bc->dynStackSize);
        auto ptr         = context->bp + ip->a.u32;
        *(uint64_t*) ptr = IMMB_U64(ip);
        break;
    }

    case ByteCodeOp::SetAtStackPointer8x2:
    {
        {
            SWAG_ASSERT(ip->a.u32 + 1 <= context->bc->dynStackSize);
            auto ptr        = context->bp + ip->a.u32;
            *(uint8_t*) ptr = IMMB_U8(ip);
        }
        {
            SWAG_ASSERT(ip->c.u32 + 1 <= context->bc->dynStackSize);
            auto ptr        = context->bp + ip->c.u32;
            *(uint8_t*) ptr = IMMD_U8(ip);
        }
        break;
    }
    case ByteCodeOp::SetAtStackPointer16x2:
    {
        {
            SWAG_ASSERT(ip->a.u32 + 2 <= context->bc->dynStackSize);
            auto ptr         = context->bp + ip->a.u32;
            *(uint16_t*) ptr = IMMB_U16(ip);
        }
        {
            SWAG_ASSERT(ip->c.u32 + 2 <= context->bc->dynStackSize);
            auto ptr         = context->bp + ip->c.u32;
            *(uint16_t*) ptr = IMMD_U16(ip);
        }
        break;
    }
    case ByteCodeOp::SetAtStackPointer32x2:
    {
        {
            SWAG_ASSERT(ip->a.u32 + 4 <= context->bc->dynStackSize);
            auto ptr         = context->bp + ip->a.u32;
            *(uint32_t*) ptr = IMMB_U32(ip);
        }
        {
            SWAG_ASSERT(ip->c.u32 + 4 <= context->bc->dynStackSize);
            auto ptr         = context->bp + ip->c.u32;
            *(uint32_t*) ptr = IMMD_U32(ip);
        }
        break;
    }
    case ByteCodeOp::SetAtStackPointer64x2:
    {
        {
            SWAG_ASSERT(ip->a.u32 + 8 <= context->bc->dynStackSize);
            auto ptr         = context->bp + ip->a.u32;
            *(uint64_t*) ptr = IMMB_U64(ip);
        }
        {
            SWAG_ASSERT(ip->c.u32 + 8 <= context->bc->dynStackSize);
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
    case ByteCodeOp::CompareOpEqualF32:
    {
        registersRC[ip->c.u32].b = IMMA_F32(ip) == IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::CompareOpEqualF64:
    {
        registersRC[ip->c.u32].b = IMMA_F64(ip) == IMMB_F64(ip);
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
    case ByteCodeOp::CompareOpNotEqualF32:
    {
        registersRC[ip->c.u32].b = IMMA_F32(ip) != IMMB_F32(ip);
        break;
    }
    case ByteCodeOp::CompareOpNotEqualF64:
    {
        registersRC[ip->c.u32].b = IMMA_F64(ip) != IMMB_F64(ip);
        break;
    }

    case ByteCodeOp::IntrinsicDbgAlloc:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atdbgalloc);
        localCall(context, bc, 0, ip->a.u32);
        break;
    }
    case ByteCodeOp::IntrinsicSysAlloc:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atsysalloc);
        localCall(context, bc, 0, ip->a.u32);
        break;
    }
    case ByteCodeOp::IntrinsicRtFlags:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atrtflags);
        localCall(context, bc, 0, ip->a.u32);
        break;
    }
    case ByteCodeOp::IntrinsicStringCmp:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_atstrcmp);
        context->push(registersRC[ip->d.u32].u64);
        context->push(registersRC[ip->c.u32].pointer);
        context->push(registersRC[ip->b.u32].u64);
        context->push(registersRC[ip->a.u32].pointer);
        localCall(context, bc, 4, ip->d.u32);
        break;
    }
    case ByteCodeOp::IntrinsicTypeCmp:
    {
        auto bc = g_Workspace->runtimeModule->getRuntimeFct(g_LangSpec->name_attypecmp);
        context->push(registersRC[ip->c.u32].u64);
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
        registersRC[ip->a.u32].pointer = (uint8_t*) Allocator::alloc(size);
        context->bc->autoFree.push_back({(void*) registersRC[ip->a.u32].pointer, size});
        memcpy((void*) registersRC[ip->a.u32].pointer, ptr, count + 1);
        registersRC[ip->a.u32].pointer[count] = 0;
        break;
    }

    case ByteCodeOp::MulAddVC64:
    {
        registersRC[ip->a.u32].u64 = registersRC[ip->a.u32].u64 * (registersRC[ip->b.u32].u64 + ip->c.u32);
        break;
    }

    case ByteCodeOp::CompareOp3Way8:
    {
        auto sub                   = IMMA_S8(ip) - IMMB_S8(ip);
        registersRC[ip->c.u32].s32 = (int32_t) ((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3Way16:
    {
        auto sub                   = IMMA_S16(ip) - IMMB_S16(ip);
        registersRC[ip->c.u32].s32 = (int32_t) ((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3Way32:
    {
        auto sub                   = IMMA_S32(ip) - IMMB_S32(ip);
        registersRC[ip->c.u32].s32 = (int32_t) ((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3Way64:
    {
        auto sub                   = IMMA_S64(ip) - IMMB_S64(ip);
        registersRC[ip->c.u32].s32 = (int32_t) ((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3WayF32:
    {
        auto sub                   = IMMA_F32(ip) - IMMB_F32(ip);
        registersRC[ip->c.u32].s32 = (int32_t) ((sub > 0) - (sub < 0));
        break;
    }
    case ByteCodeOp::CompareOp3WayF64:
    {
        auto sub                   = IMMA_F64(ip) - IMMB_F64(ip);
        registersRC[ip->c.u32].s32 = (int32_t) ((sub > 0) - (sub < 0));
        break;
    }

    case ByteCodeOp::CompareOpLowerS8:
    {
        registersRC[ip->c.u32].b = IMMA_S8(ip) < IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerS16:
    {
        registersRC[ip->c.u32].b = IMMA_S16(ip) < IMMB_S16(ip);
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
    case ByteCodeOp::CompareOpLowerU8:
    {
        registersRC[ip->c.u32].b = IMMA_U8(ip) < IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerU16:
    {
        registersRC[ip->c.u32].b = IMMA_U16(ip) < IMMB_U16(ip);
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

    case ByteCodeOp::CompareOpLowerEqS8:
    {
        registersRC[ip->c.u32].b = IMMA_S8(ip) <= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerEqS16:
    {
        registersRC[ip->c.u32].b = IMMA_S16(ip) <= IMMB_S16(ip);
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
    case ByteCodeOp::CompareOpLowerEqU8:
    {
        registersRC[ip->c.u32].b = IMMA_U8(ip) <= IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::CompareOpLowerEqU16:
    {
        registersRC[ip->c.u32].b = IMMA_U16(ip) <= IMMB_U16(ip);
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

    case ByteCodeOp::CompareOpGreaterS8:
    {
        registersRC[ip->c.u32].b = IMMA_S8(ip) > IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterS16:
    {
        registersRC[ip->c.u32].b = IMMA_S16(ip) > IMMB_S16(ip);
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
    case ByteCodeOp::CompareOpGreaterU8:
    {
        registersRC[ip->c.u32].b = IMMA_U8(ip) > IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterU16:
    {
        registersRC[ip->c.u32].b = IMMA_U16(ip) > IMMB_U16(ip);
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

    case ByteCodeOp::CompareOpGreaterEqS8:
    {
        registersRC[ip->c.u32].b = IMMA_S8(ip) >= IMMB_S8(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterEqS16:
    {
        registersRC[ip->c.u32].b = IMMA_S16(ip) >= IMMB_S16(ip);
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
    case ByteCodeOp::CompareOpGreaterEqU8:
    {
        registersRC[ip->c.u32].b = IMMA_U8(ip) >= IMMB_U8(ip);
        break;
    }
    case ByteCodeOp::CompareOpGreaterEqU16:
    {
        registersRC[ip->c.u32].b = IMMA_U16(ip) >= IMMB_U16(ip);
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
        registersRC[ip->a.u32].b = registersRC[ip->b.u32].b ^ 1;
        break;
    case ByteCodeOp::NegS32:
        registersRC[ip->a.u32].s32 = -registersRC[ip->b.u32].s32;
        break;
    case ByteCodeOp::NegS64:
        registersRC[ip->a.u32].s64 = -registersRC[ip->b.u32].s64;
        break;
    case ByteCodeOp::NegF32:
        registersRC[ip->a.u32].f32 = -registersRC[ip->b.u32].f32;
        break;
    case ByteCodeOp::NegF64:
        registersRC[ip->a.u32].f64 = -registersRC[ip->b.u32].f64;
        break;

    case ByteCodeOp::InvertU8:
        registersRC[ip->a.u32].u8 = ~registersRC[ip->b.u32].u8;
        break;
    case ByteCodeOp::InvertU16:
        registersRC[ip->a.u32].u16 = ~registersRC[ip->b.u32].u16;
        break;
    case ByteCodeOp::InvertU32:
        registersRC[ip->a.u32].u32 = ~registersRC[ip->b.u32].u32;
        break;
    case ByteCodeOp::InvertU64:
        registersRC[ip->a.u32].u64 = ~registersRC[ip->b.u32].u64;
        break;

    case ByteCodeOp::ClearMaskU32:
        registersRC[ip->a.u32].u32 &= ip->b.u32;
        break;
    case ByteCodeOp::ClearMaskU64:
        registersRC[ip->a.u32].u64 &= ip->b.u64;
        break;

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
        registersRC[ip->a.u32].s32 = registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::CastS16S64:
    {
        registersRC[ip->a.u32].s64 = registersRC[ip->b.u32].s16;
        break;
    }

    case ByteCodeOp::CastF64F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].f64;
        break;
    }
    case ByteCodeOp::CastS8F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::CastS16F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::CastS32F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::CastS64F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::CastU8F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::CastU16F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::CastU32F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CastU64F32:
    {
        registersRC[ip->a.u32].f32 = (float) registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::CastF32F64:
    {
        registersRC[ip->a.u32].f64 = registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::CastU8F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::CastU16F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::CastU32F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CastU64F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::CastS8F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::CastS16F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::CastS32F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::CastS64F64:
    {
        registersRC[ip->a.u32].f64 = (double) registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::CastF32S32:
    {
        registersRC[ip->a.u32].s32 = (int32_t) registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::CastS8S16:
    {
        registersRC[ip->a.u32].s16 = registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::CastS8S32:
    {
        registersRC[ip->a.u32].s32 = registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::CastS8S64:
    {
        registersRC[ip->a.u32].s64 = registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::CastS32S64:
    {
        registersRC[ip->a.u32].s64 = registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::CastF64S64:
    {
        registersRC[ip->a.u32].s64 = (int64_t) registersRC[ip->b.u32].f64;
        break;
    }

        /////////////////////////////////////

    case ByteCodeOp::AffectOpPlusEqS8:
        if (addWillOverflow(ip, ip->node, *(int8_t*) registersRC[ip->a.u32].pointer, IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS8));
        *(int8_t*) registersRC[ip->a.u32].pointer += IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS8_Safe:
        *(int8_t*) registersRC[ip->a.u32].pointer += IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS8_SSafe:
        *(int8_t*) (context->bp + ip->a.u32) += IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS8_SSSafe:
        *(int8_t*) (context->bp + ip->a.u32) += *(int8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqS16:
        if (addWillOverflow(ip, ip->node, *(int16_t*) registersRC[ip->a.u32].pointer, IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS16));
        *(int16_t*) registersRC[ip->a.u32].pointer += IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS16_Safe:
        *(int16_t*) registersRC[ip->a.u32].pointer += IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS16_SSafe:
        *(int16_t*) (context->bp + ip->a.u32) += IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS16_SSSafe:
        *(int16_t*) (context->bp + ip->a.u32) += *(int16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqS32:
        if (addWillOverflow(ip, ip->node, *(int32_t*) registersRC[ip->a.u32].pointer, IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS32));
        *(int32_t*) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS32_Safe:
        *(int32_t*) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS32_SSafe:
        *(int32_t*) (context->bp + ip->a.u32) += IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS32_SSSafe:
        *(int32_t*) (context->bp + ip->a.u32) += *(int32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqS64:
        if (addWillOverflow(ip, ip->node, *(int64_t*) registersRC[ip->a.u32].pointer, IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoS64));
        *(int64_t*) registersRC[ip->a.u32].pointer += IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS64_Safe:
        *(int64_t*) registersRC[ip->a.u32].pointer += IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS64_SSafe:
        *(int64_t*) (context->bp + ip->a.u32) += IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqS64_SSSafe:
        *(int64_t*) (context->bp + ip->a.u32) += *(int64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqU8:
        if (addWillOverflow(ip, ip->node, *(uint8_t*) registersRC[ip->a.u32].pointer, (uint8_t) IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU8));
        *(uint8_t*) registersRC[ip->a.u32].pointer += IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU8_Safe:
        *(uint8_t*) registersRC[ip->a.u32].pointer += IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU8_SSafe:
        *(uint8_t*) (context->bp + ip->a.u32) += IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU8_SSSafe:
        *(uint8_t*) (context->bp + ip->a.u32) += *(uint8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqU16:
        if (addWillOverflow(ip, ip->node, *(uint16_t*) registersRC[ip->a.u32].pointer, (uint16_t) IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU16));
        *(uint16_t*) registersRC[ip->a.u32].pointer += IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU16_Safe:
        *(uint16_t*) registersRC[ip->a.u32].pointer += IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU16_SSafe:
        *(uint16_t*) (context->bp + ip->a.u32) += IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU16_SSSafe:
        *(uint16_t*) (context->bp + ip->a.u32) += *(uint16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqU32:
        if (addWillOverflow(ip, ip->node, *(uint32_t*) registersRC[ip->a.u32].pointer, (uint32_t) IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU32));
        *(uint32_t*) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU32_Safe:
        *(uint32_t*) registersRC[ip->a.u32].pointer += IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU32_SSafe:
        *(uint32_t*) (context->bp + ip->a.u32) += IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU32_SSSafe:
        *(uint32_t*) (context->bp + ip->a.u32) += *(uint32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqU64:
        if (addWillOverflow(ip, ip->node, *(uint64_t*) registersRC[ip->a.u32].pointer, (uint64_t) IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::PlusEq, g_TypeMgr->typeInfoU64));
        *(uint64_t*) registersRC[ip->a.u32].pointer += IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU64_Safe:
        *(uint64_t*) registersRC[ip->a.u32].pointer += IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU64_SSafe:
        *(uint64_t*) (context->bp + ip->a.u32) += IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqU64_SSSafe:
        *(uint64_t*) (context->bp + ip->a.u32) += *(uint64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqF32:
        *(float*) registersRC[ip->a.u32].pointer += IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqF32_S:
        *(float*) (context->bp + ip->a.u32) += IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqF32_SS:
        *(float*) (context->bp + ip->a.u32) += *(float*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpPlusEqF64:
        *(double*) registersRC[ip->a.u32].pointer += IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqF64_S:
        *(double*) (context->bp + ip->a.u32) += IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpPlusEqF64_SS:
        *(double*) (context->bp + ip->a.u32) += *(double*) (context->bp + ip->b.u32);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpMinusEqS8:
        if (subWillOverflow(ip, ip->node, *(int8_t*) registersRC[ip->a.u32].pointer, IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS8));
        *(int8_t*) registersRC[ip->a.u32].pointer -= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS8_Safe:
        *(int8_t*) registersRC[ip->a.u32].pointer -= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS8_SSafe:
        *(int8_t*) (context->bp + ip->a.u32) -= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS8_SSSafe:
        *(int8_t*) (context->bp + ip->a.u32) -= *(int8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqS16:
        if (subWillOverflow(ip, ip->node, *(int16_t*) registersRC[ip->a.u32].pointer, IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS16));
        *(int16_t*) registersRC[ip->a.u32].pointer -= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS16_Safe:
        *(int16_t*) registersRC[ip->a.u32].pointer -= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS16_SSafe:
        *(int16_t*) (context->bp + ip->a.u32) -= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS16_SSSafe:
        *(int16_t*) (context->bp + ip->a.u32) -= *(int16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqS32:
        if (subWillOverflow(ip, ip->node, *(int32_t*) registersRC[ip->a.u32].pointer, IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS32));
        *(int32_t*) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS32_Safe:
        *(int32_t*) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS32_SSafe:
        *(int32_t*) (context->bp + ip->a.u32) -= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS32_SSSafe:
        *(int32_t*) (context->bp + ip->a.u32) -= *(int32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqS64:
        if (subWillOverflow(ip, ip->node, *(int64_t*) registersRC[ip->a.u32].pointer, IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoS64));
        *(int64_t*) registersRC[ip->a.u32].pointer -= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS64_Safe:
        *(int64_t*) registersRC[ip->a.u32].pointer -= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS64_SSafe:
        *(int64_t*) (context->bp + ip->a.u32) -= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqS64_SSSafe:
        *(int64_t*) (context->bp + ip->a.u32) -= *(int64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqU8:
        if (subWillOverflow(ip, ip->node, *(uint8_t*) registersRC[ip->a.u32].pointer, (uint8_t) IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU8));
        *(uint8_t*) registersRC[ip->a.u32].pointer -= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU8_Safe:
        *(uint8_t*) registersRC[ip->a.u32].pointer -= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU8_SSafe:
        *(uint8_t*) (context->bp + ip->a.u32) -= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU8_SSSafe:
        *(uint8_t*) (context->bp + ip->a.u32) -= *(uint8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqU16:
        if (subWillOverflow(ip, ip->node, *(uint16_t*) registersRC[ip->a.u32].pointer, (uint16_t) IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU16));
        *(uint16_t*) registersRC[ip->a.u32].pointer -= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU16_Safe:
        *(uint16_t*) registersRC[ip->a.u32].pointer -= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU16_SSafe:
        *(uint16_t*) (context->bp + ip->a.u32) -= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU16_SSSafe:
        *(uint16_t*) (context->bp + ip->a.u32) -= *(uint16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqU32:
        if (subWillOverflow(ip, ip->node, *(uint32_t*) registersRC[ip->a.u32].pointer, (uint32_t) IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MinusEq, g_TypeMgr->typeInfoU32));
        *(uint32_t*) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU32_Safe:
        *(uint32_t*) registersRC[ip->a.u32].pointer -= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU32_SSafe:
        *(uint32_t*) (context->bp + ip->a.u32) -= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU32_SSSafe:
        *(uint32_t*) (context->bp + ip->a.u32) -= *(uint32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqU64:
        if (subWillOverflow(ip, ip->node, *(uint64_t*) registersRC[ip->a.u32].pointer, (uint64_t) IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64));
        *(uint64_t*) registersRC[ip->a.u32].pointer -= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU64_Safe:
        *(uint64_t*) registersRC[ip->a.u32].pointer -= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU64_SSafe:
        *(uint64_t*) (context->bp + ip->a.u32) -= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqU64_SSSafe:
        *(uint64_t*) (context->bp + ip->a.u32) -= *(uint64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqF32:
        *(float*) registersRC[ip->a.u32].pointer -= IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqF32_S:
        *(float*) (context->bp + ip->a.u32) -= IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqF32_SS:
        *(float*) (context->bp + ip->a.u32) -= *(float*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMinusEqF64:
        *(double*) registersRC[ip->a.u32].pointer -= IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqF64_S:
        *(double*) (context->bp + ip->a.u32) -= IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpMinusEqF64_SS:
        *(double*) (context->bp + ip->a.u32) -= *(double*) (context->bp + ip->b.u32);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpMulEqS8:
        if (mulWillOverflow(ip, ip->node, *(int8_t*) registersRC[ip->a.u32].pointer, IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS8));
        *(int8_t*) registersRC[ip->a.u32].pointer *= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS8_Safe:
        *(int8_t*) registersRC[ip->a.u32].pointer *= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS8_SSafe:
        *(int8_t*) (context->bp + ip->a.u32) *= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS8_SSSafe:
        *(int8_t*) (context->bp + ip->a.u32) *= *(int8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqS16:
        if (mulWillOverflow(ip, ip->node, *(int16_t*) registersRC[ip->a.u32].pointer, IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS16));
        *(int16_t*) registersRC[ip->a.u32].pointer *= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS16_Safe:
        *(int16_t*) registersRC[ip->a.u32].pointer *= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS16_SSafe:
        *(int16_t*) (context->bp + ip->a.u32) *= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS16_SSSafe:
        *(int16_t*) (context->bp + ip->a.u32) *= *(int16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqS32:
        if (mulWillOverflow(ip, ip->node, *(int32_t*) registersRC[ip->a.u32].pointer, IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS32));
        *(int32_t*) registersRC[ip->a.u32].pointer *= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS32_Safe:
        *(int32_t*) registersRC[ip->a.u32].pointer *= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS32_SSafe:
        *(int32_t*) (context->bp + ip->a.u32) *= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS32_SSSafe:
        *(int32_t*) (context->bp + ip->a.u32) *= *(int32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqS64:
        if (mulWillOverflow(ip, ip->node, *(int64_t*) registersRC[ip->a.u32].pointer, IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoS64));
        *(int64_t*) registersRC[ip->a.u32].pointer *= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS64_Safe:
        *(int64_t*) registersRC[ip->a.u32].pointer *= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS64_SSafe:
        *(int64_t*) (context->bp + ip->a.u32) *= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqS64_SSSafe:
        *(int64_t*) (context->bp + ip->a.u32) *= *(int64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqU8:
        if (mulWillOverflow(ip, ip->node, *(uint8_t*) registersRC[ip->a.u32].pointer, (uint8_t) IMMB_S8(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU8));
        *(uint8_t*) registersRC[ip->a.u32].pointer *= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU8_Safe:
        *(uint8_t*) registersRC[ip->a.u32].pointer *= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU8_SSafe:
        *(uint8_t*) (context->bp + ip->a.u32) *= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU8_SSSafe:
        *(uint8_t*) (context->bp + ip->a.u32) *= *(uint8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqU16:
        if (mulWillOverflow(ip, ip->node, *(uint16_t*) registersRC[ip->a.u32].pointer, (uint16_t) IMMB_S16(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU16));
        *(uint16_t*) registersRC[ip->a.u32].pointer *= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU16_Safe:
        *(uint16_t*) registersRC[ip->a.u32].pointer *= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU16_SSafe:
        *(uint16_t*) (context->bp + ip->a.u32) *= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU16_SSSafe:
        *(uint16_t*) (context->bp + ip->a.u32) *= *(uint16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqU32:
        if (mulWillOverflow(ip, ip->node, *(uint32_t*) registersRC[ip->a.u32].pointer, (uint32_t) IMMB_S32(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU32));
        *(uint32_t*) registersRC[ip->a.u32].pointer *= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU32_Safe:
        *(uint32_t*) registersRC[ip->a.u32].pointer *= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU32_SSafe:
        *(uint32_t*) (context->bp + ip->a.u32) *= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU32_SSSafe:
        *(uint32_t*) (context->bp + ip->a.u32) *= *(uint32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqU64:
        if (mulWillOverflow(ip, ip->node, *(uint64_t*) registersRC[ip->a.u32].pointer, (uint64_t) IMMB_S64(ip)))
            callInternalPanic(context, ip, ByteCodeGenJob::safetyMsg(SafetyMsg::MulEq, g_TypeMgr->typeInfoU64));
        *(uint64_t*) registersRC[ip->a.u32].pointer *= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU64_Safe:
        *(uint64_t*) registersRC[ip->a.u32].pointer *= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU64_SSafe:
        *(uint64_t*) (context->bp + ip->a.u32) *= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqU64_SSSafe:
        *(uint64_t*) (context->bp + ip->a.u32) *= *(uint64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqF32:
        *(float*) registersRC[ip->a.u32].pointer *= IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqF32_S:
        *(float*) (context->bp + ip->a.u32) *= IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpMulEqF32_SS:
        *(float*) (context->bp + ip->a.u32) *= *(float*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpMulEqF64:
        *(double*) registersRC[ip->a.u32].pointer *= IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqF64_S:
        *(double*) (context->bp + ip->a.u32) *= IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpMulEqF64_SS:
        *(double*) (context->bp + ip->a.u32) *= *(double*) (context->bp + ip->b.u32);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpDivEqS8:
        *(int8_t*) registersRC[ip->a.u32].pointer /= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS8_S:
        *(int8_t*) (context->bp + ip->a.u32) /= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS8_SS:
        *(int8_t*) (context->bp + ip->a.u32) /= *(int8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqS16:
        *(int16_t*) registersRC[ip->a.u32].pointer /= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS16_S:
        *(int16_t*) (context->bp + ip->a.u32) /= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS16_SS:
        *(int16_t*) (context->bp + ip->a.u32) /= *(int16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqS32:
        *(int32_t*) registersRC[ip->a.u32].pointer /= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS32_S:
        *(int32_t*) (context->bp + ip->a.u32) /= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS32_SS:
        *(int32_t*) (context->bp + ip->a.u32) /= *(int32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqS64:
        *(int64_t*) registersRC[ip->a.u32].pointer /= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS64_S:
        *(int64_t*) (context->bp + ip->a.u32) /= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpDivEqS64_SS:
        *(int64_t*) (context->bp + ip->a.u32) /= *(int64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqU8:
        *(uint8_t*) registersRC[ip->a.u32].pointer /= IMMB_U8(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU8_S:
        *(uint8_t*) (context->bp + ip->a.u32) /= IMMB_U8(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU8_SS:
        *(uint8_t*) (context->bp + ip->a.u32) /= *(uint8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqU16:
        *(uint16_t*) registersRC[ip->a.u32].pointer /= IMMB_U16(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU16_S:
        *(uint16_t*) (context->bp + ip->a.u32) /= IMMB_U16(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU16_SS:
        *(uint16_t*) (context->bp + ip->a.u32) /= *(uint16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqU32:
        *(uint32_t*) registersRC[ip->a.u32].pointer /= IMMB_U32(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU32_S:
        *(uint32_t*) (context->bp + ip->a.u32) /= IMMB_U32(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU32_SS:
        *(uint32_t*) (context->bp + ip->a.u32) /= *(uint32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqU64:
        *(uint64_t*) registersRC[ip->a.u32].pointer /= IMMB_U64(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU64_S:
        *(uint64_t*) (context->bp + ip->a.u32) /= IMMB_U64(ip);
        break;
    case ByteCodeOp::AffectOpDivEqU64_SS:
        *(uint64_t*) (context->bp + ip->a.u32) /= *(uint64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqF32:
        *(float*) registersRC[ip->a.u32].pointer /= IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpDivEqF32_S:
        *(float*) (context->bp + ip->a.u32) /= IMMB_F32(ip);
        break;
    case ByteCodeOp::AffectOpDivEqF32_SS:
        *(float*) (context->bp + ip->a.u32) /= *(float*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpDivEqF64:
        *(double*) registersRC[ip->a.u32].pointer /= IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpDivEqF64_S:
        *(double*) (context->bp + ip->a.u32) /= IMMB_F64(ip);
        break;
    case ByteCodeOp::AffectOpDivEqF64_SS:
        *(double*) (context->bp + ip->a.u32) /= *(double*) (context->bp + ip->b.u32);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpModuloEqS8:
        *(int8_t*) registersRC[ip->a.u32].pointer %= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS8_S:
        *(int8_t*) (context->bp + ip->a.u32) %= IMMB_S8(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS8_SS:
        *(int8_t*) (context->bp + ip->a.u32) %= *(int8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpModuloEqS16:
        *(int16_t*) registersRC[ip->a.u32].pointer %= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS16_S:
        *(int16_t*) (context->bp + ip->a.u32) %= IMMB_S16(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS16_SS:
        *(int16_t*) (context->bp + ip->a.u32) %= *(int16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpModuloEqS32:
        *(int32_t*) registersRC[ip->a.u32].pointer %= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS32_S:
        *(int32_t*) (context->bp + ip->a.u32) %= IMMB_S32(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS32_SS:
        *(int32_t*) (context->bp + ip->a.u32) %= *(int32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpModuloEqS64:
        *(int64_t*) registersRC[ip->a.u32].pointer %= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS64_S:
        *(int64_t*) (context->bp + ip->a.u32) %= IMMB_S64(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqS64_SS:
        *(int64_t*) (context->bp + ip->a.u32) %= *(int64_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpModuloEqU8:
        *(uint8_t*) registersRC[ip->a.u32].pointer %= IMMB_U8(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU8_S:
        *(uint8_t*) (context->bp + ip->a.u32) %= IMMB_U8(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU8_SS:
        *(uint8_t*) (context->bp + ip->a.u32) %= *(uint8_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpModuloEqU16:
        *(uint16_t*) registersRC[ip->a.u32].pointer %= IMMB_U16(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU16_S:
        *(uint16_t*) (context->bp + ip->a.u32) %= IMMB_U16(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU16_SS:
        *(uint16_t*) (context->bp + ip->a.u32) %= *(uint16_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpModuloEqU32:
        *(uint32_t*) registersRC[ip->a.u32].pointer %= IMMB_U32(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU32_S:
        *(uint32_t*) (context->bp + ip->a.u32) %= IMMB_U32(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU32_SS:
        *(uint32_t*) (context->bp + ip->a.u32) %= *(uint32_t*) (context->bp + ip->b.u32);
        break;

    case ByteCodeOp::AffectOpModuloEqU64:
        *(uint64_t*) registersRC[ip->a.u32].pointer %= IMMB_U64(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU64_S:
        *(uint64_t*) (context->bp + ip->a.u32) %= IMMB_U64(ip);
        break;
    case ByteCodeOp::AffectOpModuloEqU64_SS:
        *(uint64_t*) (context->bp + ip->a.u32) %= *(uint64_t*) (context->bp + ip->b.u32);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpAndEqU8:
        *(int8_t*) registersRC[ip->a.u32].pointer &= IMMB_S8(ip);
        break;

    case ByteCodeOp::AffectOpAndEqU16:
        *(int16_t*) registersRC[ip->a.u32].pointer &= IMMB_S16(ip);
        break;

    case ByteCodeOp::AffectOpAndEqU32:
        *(int32_t*) registersRC[ip->a.u32].pointer &= IMMB_S32(ip);
        break;

    case ByteCodeOp::AffectOpAndEqU64:
        *(int64_t*) registersRC[ip->a.u32].pointer &= IMMB_S64(ip);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpOrEqU8:
        *(uint8_t*) registersRC[ip->a.u32].pointer |= IMMB_S8(ip);
        break;

    case ByteCodeOp::AffectOpOrEqU16:
        *(uint16_t*) registersRC[ip->a.u32].pointer |= IMMB_S16(ip);
        break;

    case ByteCodeOp::AffectOpOrEqU32:
        *(uint32_t*) registersRC[ip->a.u32].pointer |= IMMB_S32(ip);
        break;

    case ByteCodeOp::AffectOpOrEqU64:
        *(uint64_t*) registersRC[ip->a.u32].pointer |= IMMB_S64(ip);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpXorEqU8:
        *(uint8_t*) registersRC[ip->a.u32].pointer ^= IMMB_S8(ip);
        break;

    case ByteCodeOp::AffectOpXorEqU16:
        *(uint16_t*) registersRC[ip->a.u32].pointer ^= IMMB_S16(ip);
        break;

    case ByteCodeOp::AffectOpXorEqU32:
        *(uint32_t*) registersRC[ip->a.u32].pointer ^= IMMB_S32(ip);
        break;

    case ByteCodeOp::AffectOpXorEqU64:
        *(uint64_t*) registersRC[ip->a.u32].pointer ^= IMMB_S64(ip);
        break;

        /////////////////////////////////////

    case ByteCodeOp::AffectOpShiftLeftEqS8:
    case ByteCodeOp::AffectOpShiftLeftEqU8:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u8        = *(uint8_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(&rr, rr, r1, 8, false);
        *(uint8_t*) ptr = rr.u8;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS16:
    case ByteCodeOp::AffectOpShiftLeftEqU16:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u16       = *(uint16_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(&rr, rr, r1, 16, false);
        *(uint16_t*) ptr = rr.u16;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS32:
    case ByteCodeOp::AffectOpShiftLeftEqU32:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u32       = *(uint32_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(&rr, rr, r1, 32, false);
        *(uint32_t*) ptr = rr.u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS64:
    case ByteCodeOp::AffectOpShiftLeftEqU64:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u64       = *(uint64_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeLeftShift(&rr, rr, r1, 64, false);
        *(uint64_t*) ptr = rr.u64;
        break;
    }

        /////////////////////////////////////

    case ByteCodeOp::AffectOpShiftRightEqS8:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u8        = *(uint8_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 8, true);
        *(uint8_t*) ptr = rr.u8;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS16:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u16       = *(uint16_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 16, true);
        *(uint16_t*) ptr = rr.u16;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS32:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u32       = *(uint32_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 32, true);
        *(uint32_t*) ptr = rr.u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS64:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u64       = *(uint64_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 64, true);
        *(uint64_t*) ptr = rr.u64;
        break;
    }

    case ByteCodeOp::AffectOpShiftRightEqU8:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u8        = *(uint8_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 8, false);
        *(uint8_t*) ptr = rr.u8;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU16:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u16       = *(uint16_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 16, false);
        *(uint16_t*) ptr = rr.u16;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU32:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u32       = *(uint32_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 32, false);
        *(uint32_t*) ptr = rr.u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU64:
    {
        Register r1, rr;
        auto     ptr = registersRC[ip->a.u32].pointer;
        rr.u64       = *(uint64_t*) ptr;
        r1.u32       = IMMB_U32(ip);
        ByteCodeRun::executeRightShift(&rr, rr, r1, 64, false);
        *(uint64_t*) ptr = rr.u64;
        break;
    }

        /////////////////////////////////////

    case ByteCodeOp::ZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 == 0 ? true : false;
        break;
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
    case ByteCodeOp::IntrinsicBcBreakpoint:
        if (!g_CommandLine.dbgOff)
        {
            context->debugRaiseStart         = true;
            context->debugEntry              = true;
            g_ByteCodeDebugger.debugStepMode = ByteCodeDebugger::DebugStepMode::None;
            throw "start debug";
        }
        break;

    default:
        SWAG_ASSERT(false);
        SWAG_UNREACHABLE;
        break;
    }

    return true;
}

void ByteCodeRun::runLoopNoDbg(ByteCodeRunContext* context)
{
    while (executeInstruction(context, context->ip++))
        ;
}

bool ByteCodeRun::runLoop(ByteCodeRunContext* context)
{
    if ((context->debugOn || !g_ByteCodeDebugger.debugBreakpoints.empty()) && context->debugAccept)
    {
        while (true)
        {
            if (!g_ByteCodeDebugger.step(context))
                OS::exit(0);
            if (!executeInstruction(context, context->ip++))
                break;
            if (!context->debugOn && g_ByteCodeDebugger.debugBreakpoints.empty())
            {
                runLoopNoDbg(context);
                break;
            }
        }
    }
    else
    {
        runLoopNoDbg(context);
    }

    return true;
}

static int exceptionHandler(ByteCodeRunContext* runContext, LPEXCEPTION_POINTERS args)
{
    // @breakpoint()
    if (runContext->debugRaiseStart)
        return SWAG_EXCEPTION_EXECUTE_HANDLER;

    // Exception already processed. Need to pass the hand to the previous handle
    // This happens when bytecode executed foreign code, which executes bytecode again.
    if (args->ExceptionRecord->ExceptionCode == SWAG_EXCEPTION_TO_PREV_HANDLER)
        return SWAG_EXCEPTION_EXECUTE_HANDLER;

    Diagnostic*               diag = nullptr;
    Vector<const Diagnostic*> notes;
    SwagSourceCodeLocation*   location = nullptr;
    SwagSourceCodeLocation    tmpLoc;
    DiagnosticLevel           level = DiagnosticLevel::Error;
    Utf8                      userMsg;
    int                       returnValue = SWAG_EXCEPTION_EXECUTE_HANDLER;

    if (runContext->ip != runContext->bc->out)
        runContext->ip--;
    auto loc = ByteCode::getLocation(runContext->bc, runContext->ip);
    if (runContext->ip != runContext->bc->out)
        runContext->ip++;

    tmpLoc.fileName.buffer = (void*) _strdup(loc.file->path.string().c_str());
    tmpLoc.fileName.count  = loc.file->path.string().length();
    tmpLoc.lineStart = tmpLoc.lineEnd = loc.location->line;
    tmpLoc.colStart = tmpLoc.colEnd = loc.location->column;
    location                        = &tmpLoc;

    // Exception 666 raised during bytecode execution
    /////////////////////////////////////////////////
    if (args->ExceptionRecord->ExceptionCode == SWAG_EXCEPTION_TO_COMPILER_HANDLER)
    {
        // Source code location
        if (args->ExceptionRecord->ExceptionInformation[0])
            location = (SwagSourceCodeLocation*) args->ExceptionRecord->ExceptionInformation[0];

        // User messsage
        auto txt = Utf8{(const char*) args->ExceptionRecord->ExceptionInformation[1], (uint32_t) args->ExceptionRecord->ExceptionInformation[2]};

        // Kind of exception
        auto exceptionKind = (SwagExceptionKind) args->ExceptionRecord->ExceptionInformation[3];
        switch (exceptionKind)
        {
        case SwagExceptionKind::Error:
        default:
            level = DiagnosticLevel::Error;
            if (!Diagnostic::hastErrorId(txt))
                userMsg = Fmt(Err(Err0567), txt.c_str());
            else
                userMsg = txt;
            break;
        case SwagExceptionKind::Warning:
            level = DiagnosticLevel::Warning;
            if (!Diagnostic::hastErrorId(txt))
                userMsg = Fmt(Err(Wrn0011), txt.c_str());
            else
                userMsg = txt;
            break;
        case SwagExceptionKind::Panic:
            level = DiagnosticLevel::Panic;
            if (!Diagnostic::hastErrorId(txt))
                userMsg = Fmt(Err(Err0401), txt.c_str());
            else
                userMsg = txt;

            // Additional panic infos
            if (runContext->internalPanicSymbol)
            {
                notes.push_back(Diagnostic::hereIs(runContext->internalPanicSymbol->node, false, true));
                if (!runContext->internalPanicHint.empty())
                    notes.push_back(Diagnostic::note(runContext->internalPanicHint));
            }

            break;
        }
    }

    // Hardware exception
    /////////////////////
    else
    {
#ifdef SWAG_DEV_MODE
        returnValue = SWAG_EXCEPTION_CONTINUE_EXECUTION;
#endif

        level   = DiagnosticLevel::Exception;
        userMsg = Err(Err0435);
        notes.push_back(Diagnostic::note(Nte(Nte0022)));
        notes.push_back(Diagnostic::note(Nte(Nte0009)));
    }

    // Message
    /////////////////////

    if (runContext->forDebugger)
    {
        g_SilentErrorMsg = userMsg;
        return SWAG_EXCEPTION_EXECUTE_HANDLER;
    }

    SourceFile dummyFile;
    dummyFile.path = Utf8{location->fileName};

    SourceLocation startLocation, endLocation;
    startLocation.line   = location->lineStart;
    startLocation.column = location->colStart;
    endLocation.line     = location->lineEnd;
    endLocation.column   = location->colEnd;

    diag = new Diagnostic{&dummyFile, startLocation, endLocation, userMsg, level};

    // Get the correct source file to raise the error in the correct context
    //
    // If we have an expansion, and the first expansion requests test error, then raise
    // in its context to dismiss the error (like an error during a #validif for example)
    if (runContext->callerContext->errCxtSteps.size() && runContext->callerContext->errCxtSteps[0].node->sourceFile->shouldHaveError)
        diag->contextFile = runContext->callerContext->errCxtSteps[0].node->sourceFile;
    else if (runContext->callerContext->errCxtSteps.size() && runContext->callerContext->errCxtSteps[0].node->sourceFile->shouldHaveWarning)
        diag->contextFile = runContext->callerContext->errCxtSteps[0].node->sourceFile;
    // Otherwise get the source file from the top of the bytecode stack if possible
    else if (g_ByteCodeStackTrace->steps.size() && g_ByteCodeStackTrace->steps[0].bc)
        diag->contextFile = g_ByteCodeStackTrace->steps[0].bc->sourceFile;
    else if (g_ByteCodeStackTrace->steps.size() > 1 && g_ByteCodeStackTrace->steps[1].bc)
        diag->contextFile = g_ByteCodeStackTrace->steps[1].bc->sourceFile;
    // Otherwise take the current bytecode source file
    else
        diag->contextFile = runContext->bc->sourceFile;

    // Get error context
    runContext->callerContext->extract(*diag, notes);

    if (runContext->ip != runContext->bc->out)
        runContext->ip--;

    if (!g_CommandLine.dbgCallStack)
        notes.push_back(Diagnostic::note(Nte(Nte0087)));

    Report::report(*diag, notes, runContext);

    if (runContext->ip != runContext->bc->out)
        runContext->ip++;

    return returnValue;
}

bool ByteCodeRun::run(ByteCodeRunContext* runContext)
{
    auto module = runContext->jc.sourceFile->module;

    while (true)
    {
        SWAG_TRY
        {
            return module->runner.runLoop(runContext);
        }
        SWAG_EXCEPT(exceptionHandler(runContext, SWAG_GET_EXCEPTION_INFOS()))
        {
            if (runContext->forDebugger)
                continue;

            if (runContext->debugRaiseStart)
            {
                runContext->debugRaiseStart = false;
                runContext->debugOn         = true;
                continue;
            }

            if (g_CommandLine.dbgCatch && runContext->debugOnFirstError)
            {
                runContext->ip--;
                runContext->debugOnFirstError = false;
                runContext->debugOn           = true;
                runContext->debugEntry        = true;
                continue;
            }

            if (g_CommandLine.scriptCommand)
                OS::exit(-1);

            g_ByteCodeStackTrace->clear();
            return false;
        }
    }

    return true;
}

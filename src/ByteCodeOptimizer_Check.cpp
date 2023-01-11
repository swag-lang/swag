#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "Log.h"
#include "Ast.h"
#pragma optimize("", off)

#define BINOPEQ(__cast, __op, __reg)                                    \
    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                        \
    if (ra->kind == ValueKind::Constant)                                \
        SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));                     \
    if (ra->kind != ValueKind::StackAddr)                               \
        break;                                                          \
    SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32));                \
    SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg))); \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                             \
    *(__cast*) addr __op vb.reg.__reg;

#define CMPOP(__op, __reg)                                                                                                  \
    SWAG_CHECK(getImmediateA(va, cxt, ip));                                                                                 \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                                 \
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                                            \
    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown; \
    if (rc->kind == ValueKind::Constant)                                                                                    \
        rc->reg.b = va.reg.__reg __op vb.reg.__reg;

#define BINOP(__op, __reg)                                                                                                  \
    SWAG_CHECK(getImmediateA(va, cxt, ip));                                                                                 \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                                 \
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                                            \
    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown; \
    if (rc->kind == ValueKind::Constant)                                                                                    \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;

enum class RefKind
{
    Invalid,
    Register,
    Stack,
};

enum class ValueKind : uint8_t
{
    Invalid,
    StackAddr,
    Constant,
    Unknown,
};

struct Value
{
    ValueKind kind = ValueKind::Invalid;
    Register  reg;
};

struct Context
{
    ByteCodeOptContext*   context = nullptr;
    ByteCode*             bc      = nullptr;
    ByteCodeInstruction*  ip      = nullptr;
    VectorNative<uint8_t> stack;
    VectorNative<uint8_t> stackState;
    VectorNative<Value>   regs;
    bool                  incomplete = false;
};

static SymbolOverload* getLocalVar(Context& cxt, uint32_t stackOffset)
{
    for (auto n : cxt.bc->localVars)
    {
        if (n->resolvedSymbolOverload &&
            stackOffset >= n->resolvedSymbolOverload->computedValue.storageOffset &&
            stackOffset <= n->resolvedSymbolOverload->computedValue.storageOffset + n->resolvedSymbolOverload->typeInfo->sizeOf)
        {
            return n->resolvedSymbolOverload;
        }
    }

    return nullptr;
}

static bool raiseError(Context& cxt, Utf8 msg, Diagnostic* note = nullptr)
{
    SourceFile*     file;
    SourceLocation* loc;
    ByteCode::getLocation(cxt.bc, cxt.ip, &file, &loc);

    Diagnostic diag({file, *loc, msg});
    return cxt.context->report(diag, note);
}

static bool checkEscapeFrame(Context& cxt, uint64_t stackOffset)
{
    SWAG_ASSERT(stackOffset >= 0 && stackOffset < UINT32_MAX);
    auto sym = getLocalVar(cxt, (uint32_t) stackOffset);

    Diagnostic* note = nullptr;
    if (sym)
        note = new Diagnostic({sym->node, sym->node->token, Nte(Nte0003), DiagnosticLevel::Note});
    return raiseError(cxt, Err(San0001), note);
}

static bool checkStackOffset(Context& cxt, uint64_t stackOffset)
{
    if (stackOffset >= cxt.stack.count)
        return raiseError(cxt, Fmt(Err(San0002), stackOffset, cxt.stack.count));
    return true;
}

static ValueKind getStackState(Context& cxt, void* addr, uint32_t sizeOf)
{
    auto offset = (uint8_t*) addr - cxt.stack.buffer;
    SWAG_ASSERT(sizeOf <= 8);
    SWAG_ASSERT(offset + sizeOf <= cxt.stackState.count);
    auto addrState = cxt.stackState.buffer + offset;

    auto oldState = (ValueKind) *addrState++;
    for (uint32_t i = 1; i < sizeOf; i++)
    {
        auto state = (ValueKind) *addrState++;
        if (state != oldState)
            return ValueKind::Unknown;
        oldState = state;
    }

    return oldState;
}

static bool checkNotNull(Context& cxt, uint64_t value)
{
    if (!value)
        return raiseError(cxt, Err(San0005));
    return true;
}

static bool checkStackInitialized(Context& cxt, void* addr, uint32_t sizeOf)
{
    auto state = getStackState(cxt, addr, sizeOf);
    if (state == ValueKind::Invalid)
    {
        auto offset = (uint8_t*) addr - cxt.stack.buffer;
        auto sym    = getLocalVar(cxt, (uint32_t) offset);
        if (sym)
            return raiseError(cxt, Fmt(Err(San0004), sym->symbol->name.c_str()));
        return raiseError(cxt, Err(San0003));
    }

    return true;
}

static bool getRegister(Value*& result, Context& cxt, uint32_t reg)
{
    SWAG_ASSERT(reg < (uint32_t) cxt.regs.count);
    result = &cxt.regs[reg];
    return true;
}

static bool getImmediateA(Value& result, Context& cxt, ByteCodeInstruction* ip)
{
    if (ip->flags & BCI_IMM_A)
    {
        result.kind = ValueKind::Constant;
        result.reg  = ip->b;
        return true;
    }

    Value* ra = nullptr;
    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
    result = *ra;
    return true;
}

static bool getImmediateB(Value& result, Context& cxt, ByteCodeInstruction* ip)
{
    if (ip->flags & BCI_IMM_B)
    {
        result.kind = ValueKind::Constant;
        result.reg  = ip->b;
        return true;
    }

    Value* rb = nullptr;
    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
    result = *rb;
    return true;
}

static bool getImmediateC(Value& result, Context& cxt, ByteCodeInstruction* ip)
{
    if (ip->flags & BCI_IMM_C)
    {
        result.kind = ValueKind::Constant;
        result.reg  = ip->c;
        return true;
    }

    Value* rc = nullptr;
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
    result = *rc;
    return true;
}

static bool getStackAddress(uint8_t*& result, Context& cxt, uint64_t stackOffset)
{
    SWAG_CHECK(checkStackOffset(cxt, stackOffset));
    result = cxt.stack.buffer + stackOffset;
    return true;
}

static void setStackState(Context& cxt, void* addr, uint32_t sizeOf, ValueKind state)
{
    auto offset = (uint8_t*) addr - cxt.stack.buffer;
    SWAG_ASSERT(offset + sizeOf <= cxt.stackState.count);
    memset(cxt.stackState.buffer + offset, (uint8_t) state, sizeOf);
}

bool ByteCodeOptimizer::optimizePassCheckStack(ByteCodeOptContext* context, uint32_t curNode)
{
    auto&                cxt  = *(Context*) context->checkContext;
    ByteCodeOptTreeNode* node = &context->tree[curNode];
    auto                 ip   = node->start;
    Value*               ra   = nullptr;
    Value*               rb   = nullptr;
    Value*               rc   = nullptr;
    uint8_t*             addr = nullptr;
    Value                va, vb, vc;

    while (true)
    {
        if (!(ip->flags & BCI_SAFETY))
        {
            cxt.ip = ip;
            switch (ip->op)
            {
            case ByteCodeOp::SetBP:
            case ByteCodeOp::DecSPBP:
            case ByteCodeOp::IncSPPostCall:
            case ByteCodeOp::Ret:
            case ByteCodeOp::PushRAParam:
            case ByteCodeOp::InternalPanic:
            case ByteCodeOp::Jump:
                break;

            case ByteCodeOp::JumpIfFalse:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (ra->kind == ValueKind::Constant)
                {
                    if (ra->reg.b)
                        context->tree[node->next[0]].flags |= 1;
                    else
                        context->tree[node->next[1]].flags |= 1;
                }
                break;
            case ByteCodeOp::JumpIfTrue:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (ra->kind == ValueKind::Constant)
                {
                    if (!ra->reg.b)
                        context->tree[node->next[0]].flags |= 1;
                    else
                        context->tree[node->next[1]].flags |= 1;
                }
                break;
            /*case ByteCodeOp::JumpIfEqual64:
                SWAG_CHECK(getImmediateA(va, cxt, ip));
                SWAG_CHECK(getImmediateC(vc, cxt, ip));
                if (va.kind == ValueKind::Constant && vc.kind == ValueKind::Constant)
                {
                    if (va.reg.u64 == vc.reg.u64)
                        context->tree[node->next[0]].flags |= 1;
                    else
                        context->tree[node->next[1]].flags |= 1;
                }
                break;*/

            case ByteCodeOp::IncrementRA64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.u64++;
                break;

            case ByteCodeOp::LambdaCall:
            case ByteCodeOp::LocalCall:
            case ByteCodeOp::ForeignCall:
                // We don't know the side effects on the stack of the call, so consider
                // everything is initialized
                setStackState(cxt, cxt.stack.buffer, cxt.stack.count, ValueKind::Unknown);
                break;

            // Fake value
            case ByteCodeOp::IntrinsicGvtd:
            case ByteCodeOp::DeRefStringSlice:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                ra->kind = ValueKind::Unknown;
                rb->kind = ValueKind::Unknown;
                break;

            case ByteCodeOp::IntrinsicGetContext:
            case ByteCodeOp::CopyRTtoRC:
            case ByteCodeOp::GetParam64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->kind = ValueKind::Unknown;
                break;

            case ByteCodeOp::MakeStackPointer:
                SWAG_CHECK(checkStackOffset(cxt, ip->b.u32));
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->kind    = ValueKind::StackAddr;
                ra->reg.u64 = ip->b.u32;
                break;

            case ByteCodeOp::SetZeroStackX:
                SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
                memset(addr, 0, ip->b.u32);
                setStackState(cxt, addr, ip->b.u32, ValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack8:
                SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
                *(uint8_t*) addr = 0;
                setStackState(cxt, addr, 1, ValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack16:
                SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
                *(uint16_t*) addr = 0;
                setStackState(cxt, addr, 2, ValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack32:
                SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
                *(uint32_t*) addr = 0;
                setStackState(cxt, addr, 4, ValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack64:
                SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
                *(uint64_t*) addr = 0;
                setStackState(cxt, addr, 8, ValueKind::Constant);
                break;

            case ByteCodeOp::SetAtStackPointer64:
                SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                *(uint64_t*) addr = vb.reg.u64;
                setStackState(cxt, addr, 8, vb.kind);
                break;

            case ByteCodeOp::SetAtPointer32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (ra->kind == ValueKind::Constant)
                    SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
                if (ra->kind != ValueKind::StackAddr)
                    break;
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                *(uint32_t*) addr = vb.reg.u32;
                setStackState(cxt, addr, 4, ValueKind::Constant);
                break;
            case ByteCodeOp::SetAtPointer64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (ra->kind == ValueKind::Constant)
                    SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
                if (ra->kind != ValueKind::StackAddr)
                    break;

                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                *(uint64_t*) addr = vb.reg.u64;
                setStackState(cxt, addr, 8, vb.kind);
                break;

            case ByteCodeOp::SetImmediate32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->kind    = ValueKind::Constant;
                ra->reg.u64 = ip->b.u32;
                break;
            case ByteCodeOp::SetImmediate64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->kind    = ValueKind::Constant;
                ra->reg.u64 = ip->b.u64;
                break;
            case ByteCodeOp::ClearRA:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->kind    = ValueKind::Constant;
                ra->reg.u64 = 0;
                break;

            case ByteCodeOp::Mul64byVB64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s64 *= ip->b.u64;
                break;

            case ByteCodeOp::GetFromStack8:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 1));
                ra->kind    = getStackState(cxt, addr, 1);
                ra->reg.u64 = *(uint8_t*) addr;
                break;
            case ByteCodeOp::GetFromStack16:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 2));
                ra->kind    = getStackState(cxt, addr, 2);
                ra->reg.u64 = *(uint16_t*) addr;
                break;
            case ByteCodeOp::GetFromStack32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 4));
                ra->kind    = getStackState(cxt, addr, 4);
                ra->reg.u64 = *(uint32_t*) addr;
                break;
            case ByteCodeOp::GetFromStack64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 8));
                ra->kind    = getStackState(cxt, addr, 8);
                ra->reg.u64 = *(uint64_t*) addr;
                break;

            case ByteCodeOp::IncPointer64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                *rc = *ra;
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                rc->reg.u64 += vb.reg.s64;
                break;
            case ByteCodeOp::DecPointer64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                *rc = *ra;
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                rc->reg.u64 -= vb.reg.s64;
                break;

            case ByteCodeOp::CopyRCtoRR:
                if (ip->flags & BCI_IMM_A)
                    break;
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (ra->kind == ValueKind::StackAddr)
                    return checkEscapeFrame(cxt, ra->reg.u32);
                break;

            case ByteCodeOp::CopyRCtoRR2:
                if (ip->flags & BCI_IMM_A)
                    break;
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                if (ra->kind == ValueKind::StackAddr)
                    return checkEscapeFrame(cxt, ra->reg.u32);
                break;

            case ByteCodeOp::DeRef8:
                SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                if (rb->kind == ValueKind::Constant)
                    SWAG_CHECK(checkNotNull(cxt, rb->reg.u64));

                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (rb->kind == ValueKind::StackAddr)
                {
                    SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 1));
                    ra->kind    = getStackState(cxt, addr, 1);
                    ra->reg.u64 = *(uint8_t*) addr;
                    break;
                }
                ra->kind = ValueKind::Unknown;
                break;
            case ByteCodeOp::DeRef16:
                SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                if (rb->kind == ValueKind::Constant)
                    SWAG_CHECK(checkNotNull(cxt, rb->reg.u64));

                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (rb->kind == ValueKind::StackAddr)
                {
                    SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 2));
                    ra->kind    = getStackState(cxt, addr, 2);
                    ra->reg.u64 = *(uint16_t*) addr;
                    break;
                }
                ra->kind = ValueKind::Unknown;
                break;
            case ByteCodeOp::DeRef32:
                SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                if (rb->kind == ValueKind::Constant)
                    SWAG_CHECK(checkNotNull(cxt, rb->reg.u64));

                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (rb->kind == ValueKind::StackAddr)
                {
                    SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 4));
                    ra->kind    = getStackState(cxt, addr, 4);
                    ra->reg.u64 = *(uint32_t*) addr;
                    break;
                }
                ra->kind = ValueKind::Unknown;
                break;
            case ByteCodeOp::DeRef64:
                SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                if (rb->kind == ValueKind::Constant)
                    SWAG_CHECK(checkNotNull(cxt, rb->reg.u64));

                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                if (rb->kind == ValueKind::StackAddr)
                {
                    SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 8));
                    ra->kind    = getStackState(cxt, addr, 8);
                    ra->reg.u64 = *(uint64_t*) addr;
                    break;
                }
                ra->kind = ValueKind::Unknown;
                break;

            case ByteCodeOp::AffectOpPlusEqS8:
                BINOPEQ(uint8_t, +=, s8);
                break;
            case ByteCodeOp::AffectOpPlusEqS16:
                BINOPEQ(uint16_t, +=, s16);
                break;
            case ByteCodeOp::AffectOpPlusEqS32:
                BINOPEQ(uint32_t, +=, s32);
                break;
            case ByteCodeOp::AffectOpPlusEqS64:
                BINOPEQ(uint64_t, +=, s64);
                break;
            case ByteCodeOp::AffectOpPlusEqU8:
                BINOPEQ(uint8_t, +=, u8);
                break;
            case ByteCodeOp::AffectOpPlusEqU16:
                BINOPEQ(uint16_t, +=, u16);
                break;
            case ByteCodeOp::AffectOpPlusEqU32:
                BINOPEQ(uint32_t, +=, u32);
                break;
            case ByteCodeOp::AffectOpPlusEqU64:
                BINOPEQ(uint64_t, +=, u64);
                break;
            case ByteCodeOp::AffectOpPlusEqF32:
                BINOPEQ(float, +=, f32);
                break;
            case ByteCodeOp::AffectOpPlusEqF64:
                BINOPEQ(double, +=, f64);
                break;

            case ByteCodeOp::AffectOpMinusEqS8:
                BINOPEQ(uint8_t, -=, s8);
                break;
            case ByteCodeOp::AffectOpMinusEqS16:
                BINOPEQ(uint16_t, -=, s16);
                break;
            case ByteCodeOp::AffectOpMinusEqS32:
                BINOPEQ(uint32_t, -=, s32);
                break;
            case ByteCodeOp::AffectOpMinusEqS64:
                BINOPEQ(uint64_t, -=, s64);
                break;
            case ByteCodeOp::AffectOpMinusEqU8:
                BINOPEQ(uint8_t, -=, u8);
                break;
            case ByteCodeOp::AffectOpMinusEqU16:
                BINOPEQ(uint16_t, -=, u16);
                break;
            case ByteCodeOp::AffectOpMinusEqU32:
                BINOPEQ(uint32_t, -=, u32);
                break;
            case ByteCodeOp::AffectOpMinusEqU64:
                BINOPEQ(uint64_t, -=, u64);
                break;
            case ByteCodeOp::AffectOpMinusEqF32:
                BINOPEQ(float, -=, f32);
                break;
            case ByteCodeOp::AffectOpMinusEqF64:
                BINOPEQ(double, -=, f64);
                break;

            case ByteCodeOp::AffectOpMulEqS8:
                BINOPEQ(uint8_t, *=, s8);
                break;
            case ByteCodeOp::AffectOpMulEqS16:
                BINOPEQ(uint16_t, *=, s16);
                break;
            case ByteCodeOp::AffectOpMulEqS32:
                BINOPEQ(uint32_t, *=, s32);
                break;
            case ByteCodeOp::AffectOpMulEqS64:
                BINOPEQ(uint64_t, *=, s64);
                break;
            case ByteCodeOp::AffectOpMulEqU8:
                BINOPEQ(uint8_t, *=, u8);
                break;
            case ByteCodeOp::AffectOpMulEqU16:
                BINOPEQ(uint16_t, *=, u16);
                break;
            case ByteCodeOp::AffectOpMulEqU32:
                BINOPEQ(uint32_t, *=, u32);
                break;
            case ByteCodeOp::AffectOpMulEqU64:
                BINOPEQ(uint64_t, *=, u64);
                break;
            case ByteCodeOp::AffectOpMulEqF32:
                BINOPEQ(float, *=, f32);
                break;
            case ByteCodeOp::AffectOpMulEqF64:
                BINOPEQ(double, *=, f64);
                break;

            case ByteCodeOp::AffectOpDivEqS8:
                BINOPEQ(uint8_t, /=, s8);
                break;
            case ByteCodeOp::AffectOpDivEqS16:
                BINOPEQ(uint16_t, /=, s16);
                break;
            case ByteCodeOp::AffectOpDivEqS32:
                BINOPEQ(uint32_t, /=, s32);
                break;
            case ByteCodeOp::AffectOpDivEqS64:
                BINOPEQ(uint64_t, /=, s64);
                break;
            case ByteCodeOp::AffectOpDivEqU8:
                BINOPEQ(uint8_t, /=, u8);
                break;
            case ByteCodeOp::AffectOpDivEqU16:
                BINOPEQ(uint16_t, /=, u16);
                break;
            case ByteCodeOp::AffectOpDivEqU32:
                BINOPEQ(uint32_t, /=, u32);
                break;
            case ByteCodeOp::AffectOpDivEqU64:
                BINOPEQ(uint64_t, /=, u64);
                break;
            case ByteCodeOp::AffectOpDivEqF32:
                BINOPEQ(float, /=, f32);
                break;
            case ByteCodeOp::AffectOpDivEqF64:
                BINOPEQ(double, /=, f64);
                break;

            case ByteCodeOp::AffectOpAndEqU8:
                BINOPEQ(uint8_t, &=, u8);
                break;
            case ByteCodeOp::AffectOpAndEqU16:
                BINOPEQ(uint16_t, &=, u16);
                break;
            case ByteCodeOp::AffectOpAndEqU32:
                BINOPEQ(uint32_t, &=, u32);
                break;
            case ByteCodeOp::AffectOpAndEqU64:
                BINOPEQ(uint64_t, &=, u64);
                break;

            case ByteCodeOp::AffectOpOrEqU8:
                BINOPEQ(uint8_t, |=, u8);
                break;
            case ByteCodeOp::AffectOpOrEqU16:
                BINOPEQ(uint16_t, |=, u16);
                break;
            case ByteCodeOp::AffectOpOrEqU32:
                BINOPEQ(uint32_t, |=, u32);
                break;
            case ByteCodeOp::AffectOpOrEqU64:
                BINOPEQ(uint64_t, |=, u64);
                break;

            case ByteCodeOp::AffectOpXorEqU8:
                BINOPEQ(uint8_t, ^=, u8);
                break;
            case ByteCodeOp::AffectOpXorEqU16:
                BINOPEQ(uint16_t, ^=, u16);
                break;
            case ByteCodeOp::AffectOpXorEqU32:
                BINOPEQ(uint32_t, ^=, u32);
                break;
            case ByteCodeOp::AffectOpXorEqU64:
                BINOPEQ(uint64_t, ^=, u64);
                break;

            case ByteCodeOp::NegBool:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                ra->kind  = vb.kind;
                ra->reg.b = !vb.reg.b;
                break;

            case ByteCodeOp::CastBool8:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                ra->kind  = vb.kind;
                ra->reg.b = vb.reg.u8 ? true : false;
                break;
            case ByteCodeOp::CastBool16:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                ra->kind  = vb.kind;
                ra->reg.b = vb.reg.u16 ? true : false;
                break;
            case ByteCodeOp::CastBool32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                ra->kind  = vb.kind;
                ra->reg.b = vb.reg.u32 ? true : false;
                break;
            case ByteCodeOp::CastBool64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                ra->kind  = vb.kind;
                ra->reg.b = vb.reg.u64 ? true : false;
                break;

            case ByteCodeOp::CastS8S16:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s16 = (int16_t) ra->reg.s8;
                break;

            case ByteCodeOp::CastS8S32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s32 = (int32_t) ra->reg.s8;
                break;
            case ByteCodeOp::CastS16S32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s32 = (int32_t) ra->reg.s16;
                break;
            case ByteCodeOp::CastF32S32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s32 = (int32_t) ra->reg.f32;
                break;

            case ByteCodeOp::CastS8S64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s64 = (int64_t) ra->reg.s8;
                break;
            case ByteCodeOp::CastS16S64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s64 = (int64_t) ra->reg.s16;
                break;
            case ByteCodeOp::CastS32S64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s64 = (int64_t) ra->reg.s32;
                break;
            case ByteCodeOp::CastF64S64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.s64 = (int64_t) ra->reg.f64;
                break;

            case ByteCodeOp::CastS8F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.s8;
                break;
            case ByteCodeOp::CastS16F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.s16;
                break;
            case ByteCodeOp::CastS32F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.s32;
                break;
            case ByteCodeOp::CastS64F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.s64;
                break;
            case ByteCodeOp::CastU8F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.u8;
                break;
            case ByteCodeOp::CastU16F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.u16;
                break;
            case ByteCodeOp::CastU32F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.u32;
                break;
            case ByteCodeOp::CastU64F32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f32 = (float) ra->reg.u64;
                break;

            case ByteCodeOp::CastS8F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.s8;
                break;
            case ByteCodeOp::CastS16F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.s16;
                break;
            case ByteCodeOp::CastS32F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.s32;
                break;
            case ByteCodeOp::CastS64F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.s64;
                break;
            case ByteCodeOp::CastU8F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.u8;
                break;
            case ByteCodeOp::CastU16F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.u16;
                break;
            case ByteCodeOp::CastU32F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.u32;
                break;
            case ByteCodeOp::CastU64F64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.f64 = (double) ra->reg.u64;
                break;

            case ByteCodeOp::CompareOpEqual8:
                CMPOP(==, u8);
                break;
            case ByteCodeOp::CompareOpEqual16:
                CMPOP(==, u16);
                break;
            case ByteCodeOp::CompareOpEqual32:
                CMPOP(==, u32);
                break;
            case ByteCodeOp::CompareOpEqual64:
                CMPOP(==, u64);
                break;

            case ByteCodeOp::CompareOpNotEqual8:
                CMPOP(!=, u8);
                break;
            case ByteCodeOp::CompareOpNotEqual16:
                CMPOP(!=, u16);
                break;
            case ByteCodeOp::CompareOpNotEqual32:
                CMPOP(!=, u32);
                break;
            case ByteCodeOp::CompareOpNotEqual64:
                CMPOP(!=, u64);
                break;

            case ByteCodeOp::CompareOpGreaterEqS32:
                CMPOP(>=, s32);
                break;
            case ByteCodeOp::CompareOpGreaterEqS64:
                CMPOP(>=, s64);
                break;
            case ByteCodeOp::CompareOpGreaterEqU32:
                CMPOP(>=, u32);
                break;
            case ByteCodeOp::CompareOpGreaterEqU64:
                CMPOP(>=, u64);
                break;
            case ByteCodeOp::CompareOpGreaterEqF32:
                CMPOP(>=, f32);
                break;
            case ByteCodeOp::CompareOpGreaterEqF64:
                CMPOP(>=, f64);
                break;

            case ByteCodeOp::CompareOpGreaterS32:
                CMPOP(>, s32);
                break;
            case ByteCodeOp::CompareOpGreaterS64:
                CMPOP(>, s64);
                break;
            case ByteCodeOp::CompareOpGreaterU32:
                CMPOP(>, u32);
                break;
            case ByteCodeOp::CompareOpGreaterU64:
                CMPOP(>, u64);
                break;
            case ByteCodeOp::CompareOpGreaterF32:
                CMPOP(>, f32);
                break;
            case ByteCodeOp::CompareOpGreaterF64:
                CMPOP(>, f64);
                break;

            case ByteCodeOp::CompareOpLowerEqS32:
                CMPOP(<=, s32);
                break;
            case ByteCodeOp::CompareOpLowerEqS64:
                CMPOP(<=, s64);
                break;
            case ByteCodeOp::CompareOpLowerEqU32:
                CMPOP(<=, u32);
                break;
            case ByteCodeOp::CompareOpLowerEqU64:
                CMPOP(<=, u64);
                break;
            case ByteCodeOp::CompareOpLowerEqF32:
                CMPOP(<=, f32);
                break;
            case ByteCodeOp::CompareOpLowerEqF64:
                CMPOP(<=, f64);
                break;

            case ByteCodeOp::CompareOpLowerS32:
                CMPOP(<, s32);
                break;
            case ByteCodeOp::CompareOpLowerS64:
                CMPOP(<, s64);
                break;
            case ByteCodeOp::CompareOpLowerU32:
                CMPOP(<, u32);
                break;
            case ByteCodeOp::CompareOpLowerU64:
                CMPOP(<, u64);
                break;
            case ByteCodeOp::CompareOpLowerF32:
                CMPOP(<, f32);
                break;
            case ByteCodeOp::CompareOpLowerF64:
                CMPOP(<, f64);
                break;

            case ByteCodeOp::ClearMaskU32:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.u32 &= ip->b.u32;
                break;
            case ByteCodeOp::ClearMaskU64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                ra->reg.u64 &= ip->b.u64;
                break;

            case ByteCodeOp::BinOpBitmaskAnd8:
                BINOP(&, u8);
                break;
            case ByteCodeOp::BinOpBitmaskAnd16:
                BINOP(&, u16);
                break;
            case ByteCodeOp::BinOpBitmaskAnd32:
                BINOP(&, u32);
                break;
            case ByteCodeOp::BinOpBitmaskAnd64:
                BINOP(&, u64);
                break;

            case ByteCodeOp::BinOpBitmaskOr8:
                BINOP(|, u8);
                break;
            case ByteCodeOp::BinOpBitmaskOr16:
                BINOP(|, u16);
                break;
            case ByteCodeOp::BinOpBitmaskOr32:
                BINOP(|, u32);
                break;
            case ByteCodeOp::BinOpBitmaskOr64:
                BINOP(|, u64);
                break;

            case ByteCodeOp::BinOpPlusS32:
                BINOP(+, s32);
                break;
            case ByteCodeOp::BinOpPlusS64:
                BINOP(+, s64);
                break;
            case ByteCodeOp::BinOpPlusU32:
                BINOP(+, u32);
                break;
            case ByteCodeOp::BinOpPlusU64:
                BINOP(+, u64);
                break;
            case ByteCodeOp::BinOpPlusF32:
                BINOP(+, f32);
                break;
            case ByteCodeOp::BinOpPlusF64:
                BINOP(+, f64);
                break;

            case ByteCodeOp::BinOpMinusS32:
                BINOP(-, s32);
                break;
            case ByteCodeOp::BinOpMinusS64:
                BINOP(-, s64);
                break;
            case ByteCodeOp::BinOpMinusU32:
                BINOP(-, u32);
                break;
            case ByteCodeOp::BinOpMinusU64:
                BINOP(-, u64);
                break;
            case ByteCodeOp::BinOpMinusF32:
                BINOP(-, f32);
                break;
            case ByteCodeOp::BinOpMinusF64:
                BINOP(-, f64);
                break;

            case ByteCodeOp::BinOpMulS32:
                BINOP(*, s32);
                break;
            case ByteCodeOp::BinOpMulS64:
                BINOP(*, s64);
                break;
            case ByteCodeOp::BinOpMulU32:
                BINOP(*, u32);
                break;
            case ByteCodeOp::BinOpMulU64:
                BINOP(*, u64);
                break;
            case ByteCodeOp::BinOpMulF32:
                BINOP(*, f32);
                break;
            case ByteCodeOp::BinOpMulF64:
                BINOP(*, f64);
                break;

            case ByteCodeOp::BinOpDivS32:
                BINOP(/, s32);
                break;
            case ByteCodeOp::BinOpDivS64:
                BINOP(/, s64);
                break;
            case ByteCodeOp::BinOpDivU32:
                BINOP(/, u32);
                break;
            case ByteCodeOp::BinOpDivU64:
                BINOP(/, u64);
                break;
            case ByteCodeOp::BinOpDivF32:
                BINOP(/, f32);
                break;
            case ByteCodeOp::BinOpDivF64:
                BINOP(/, f64);
                break;

            case ByteCodeOp::BinOpModuloS32:
                BINOP(%, s32);
                break;
            case ByteCodeOp::BinOpModuloS64:
                BINOP(%, s64);
                break;
            case ByteCodeOp::BinOpModuloU32:
                BINOP(%, u32);
                break;
            case ByteCodeOp::BinOpModuloU64:
                BINOP(%, u64);
                break;

            case ByteCodeOp::CopyRBtoRA64:
                SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                *ra = *rb;
                break;

            default:
                cxt.incomplete = true;
                return true;
            }
        }

        if (ip == node->end)
            break;
        ip++;
    }

    for (auto n : node->next)
    {
        SWAG_ASSERT(n < context->nextTreeNode);
        if (!(context->tree[n].flags & 1))
        {
            context->tree[n].flags |= 1;
            auto saveCxt = cxt;
            if (!optimizePassCheckStack(context, n))
                return false;
            cxt = saveCxt;
        }
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassCheck(ByteCodeOptContext* context)
{
#if 0
    if (context->bc->name != "Core.StrConv.convertAny")
        return true;
    context->bc->print();
#endif

    if (!context->bc->node || context->bc->isCompilerGenerated)
        return true;

    Context cxt;

    cxt.bc         = context->bc;
    cxt.ip         = nullptr;
    cxt.context    = context;
    cxt.incomplete = false;

    auto funcDecl = CastAst<AstFuncDecl>(cxt.bc->node, AstNodeKind::FuncDecl);

    cxt.stack.reserve(funcDecl->stackSize);
    memset(cxt.stack.buffer, 0, funcDecl->stackSize * sizeof(uint8_t));
    cxt.stack.count = funcDecl->stackSize;

    cxt.stackState.reserve(funcDecl->stackSize);
    memset(cxt.stackState.buffer, (uint8_t) ValueKind::Invalid, funcDecl->stackSize * sizeof(uint8_t));
    cxt.stackState.count = funcDecl->stackSize;

    cxt.regs.reserve(context->bc->maxReservedRegisterRC);
    memset(cxt.regs.buffer, 0, context->bc->maxReservedRegisterRC * sizeof(Value));
    cxt.regs.count = context->bc->maxReservedRegisterRC;

    context->checkContext = &cxt;
    SWAG_CHECK(optimizePassCheckStack(context, 0));
    if (g_CommandLine.stats && !cxt.incomplete)
        g_Stats.numSanDone++;
    else if (g_CommandLine.stats && cxt.incomplete)
        g_Stats.numSanIncomplete++;

    return true;
}
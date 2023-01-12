#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "Log.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"

#pragma optimize("", off)

#define STATE() cxt.states[cxt.state]

#define BINOPEQ(__cast, __op, __reg)                                                      \
    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                                          \
    if (ra->kind == ValueKind::Constant)                                                  \
        SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));                                       \
    else if (ra->kind == ValueKind::StackAddr)                                            \
    {                                                                                     \
        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, ra->overload));                \
        SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg), ra->overload)); \
        SWAG_CHECK(getImmediateB(vb, cxt, ip));                                           \
        if (vb.kind == ValueKind::Unknown)                                                \
            ra->kind = ValueKind::Unknown;                                                \
        else                                                                              \
            *(__cast*) addr __op vb.reg.__reg;                                            \
    }

#define BINOPEQ2(__cast, __op, __reg1, __reg2)                                             \
    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                                           \
    if (ra->kind == ValueKind::Constant)                                                   \
        SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));                                        \
    else if (ra->kind == ValueKind::StackAddr)                                             \
    {                                                                                      \
        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, ra->overload));                 \
        SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg1), ra->overload)); \
        SWAG_CHECK(getImmediateB(vb, cxt, ip));                                            \
        if (vb.kind == ValueKind::Unknown)                                                 \
            ra->kind = ValueKind::Unknown;                                                 \
        else                                                                               \
            *(__cast*) addr __op vb.reg.__reg2;                                            \
    }

#define BINOPEQDIV(__cast, __op, __reg)                      \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                  \
    SWAG_CHECK(checkDivZero(cxt, vb, vb.reg.__reg == 0));    \
    if (vb.kind == ValueKind::Constant && vb.reg.__reg == 0) \
    {                                                        \
        SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));         \
        rc->kind = ValueKind::Unknown;                       \
        break;                                               \
    }                                                        \
    BINOPEQ(__cast, __op, __reg);

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

#define BINOP2(__op, __reg1, __reg2)                                                                                        \
    SWAG_CHECK(getImmediateA(va, cxt, ip));                                                                                 \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                                 \
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                                            \
    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown; \
    if (rc->kind == ValueKind::Constant)                                                                                    \
        rc->reg.__reg1 = va.reg.__reg1 __op vb.reg.__reg2;

#define BINOPDIV(__op, __reg)                                \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                  \
    SWAG_CHECK(checkDivZero(cxt, vb, vb.reg.__reg == 0));    \
    if (vb.kind == ValueKind::Constant && vb.reg.__reg == 0) \
    {                                                        \
        SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));         \
        rc->kind = ValueKind::Unknown;                       \
        break;                                               \
    }                                                        \
    BINOP(__op, __reg)

#define JUMP1(__expr)                                    \
    SWAG_CHECK(getImmediateA(va, cxt, ip));              \
    if (va.kind == ValueKind::Constant)                  \
    {                                                    \
        if (__expr)                                      \
        {                                                \
            ip->dynFlags |= BCID_SAN_PASS;               \
            ip += ip->b.s32 + 1;                         \
            continue;                                    \
        }                                                \
    }                                                    \
    else                                                 \
    {                                                    \
        if (cxt.statesHere.contains(ip + ip->b.s32 + 1)) \
            return true;                                 \
        cxt.statesHere.insert(ip + ip->b.s32 + 1);       \
        cxt.states.push_back(STATE());                   \
        cxt.states.back().branchIp = ip;                 \
        cxt.states.back().ip       = ip + ip->b.s32 + 1; \
        cxt.states.back().parent   = cxt.state;          \
    }

#define JUMP2(__expr)                                    \
    SWAG_CHECK(getImmediateA(va, cxt, ip));              \
    SWAG_CHECK(getImmediateC(va, cxt, ip));              \
    if (va.kind == ValueKind::Constant)                  \
    {                                                    \
        if (__expr)                                      \
        {                                                \
            ip->dynFlags |= BCID_SAN_PASS;               \
            ip += ip->b.s32 + 1;                         \
            continue;                                    \
        }                                                \
    }                                                    \
    else                                                 \
    {                                                    \
        if (cxt.statesHere.contains(ip + ip->b.s32 + 1)) \
            return true;                                 \
        cxt.statesHere.insert(ip + ip->b.s32 + 1);       \
        cxt.states.push_back(STATE());                   \
        cxt.states.back().branchIp = ip;                 \
        cxt.states.back().ip       = ip + ip->b.s32 + 1; \
        cxt.states.back().parent   = cxt.state;          \
    }

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
    ValueKind       kind = ValueKind::Invalid;
    Register        reg;
    SymbolOverload* overload = nullptr;
};

struct State
{
    VectorNative<uint8_t> stack;
    VectorNative<Value>   stackValue;
    VectorNative<Value>   regs;
    ByteCodeInstruction*  branchIp = nullptr;
    ByteCodeInstruction*  ip       = nullptr;
    int                   parent   = -1;
};

struct Context
{
    ByteCodeOptContext*       context = nullptr;
    ByteCode*                 bc      = nullptr;
    int                       state   = 0;
    set<ByteCodeInstruction*> statesHere;
    vector<State>             states;
    bool                      incomplete = false;
};

static SymbolOverload* getLocalVar(Context& cxt, uint32_t stackOffset)
{
    for (auto n : cxt.bc->localVars)
    {
        if (n->resolvedSymbolOverload &&
            stackOffset >= n->resolvedSymbolOverload->computedValue.storageOffset &&
            stackOffset < n->resolvedSymbolOverload->computedValue.storageOffset + n->resolvedSymbolOverload->typeInfo->sizeOf)
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
    ByteCode::getLocation(cxt.bc, cxt.states[cxt.state].ip, &file, &loc);

    if (!file->module->mustEmitSafety(STATE().ip->node, SAFETY_SANITY))
        return true;

    Diagnostic diag({file, *loc, msg});
    return cxt.context->report(diag, note);
}

static bool checkDivZero(Context& cxt, Value& value, bool isZero)
{
    if (value.kind != ValueKind::Constant)
        return true;
    if (isZero)
        return raiseError(cxt, Err(San0006));
    return true;
}

static bool checkEscapeFrame(Context& cxt, uint64_t stackOffset, SymbolOverload* overload = nullptr)
{
    SWAG_ASSERT(stackOffset >= 0 && stackOffset < UINT32_MAX);

    auto sym = overload;
    if (!sym)
        sym = getLocalVar(cxt, (uint32_t) stackOffset);

    Diagnostic* note = nullptr;
    if (sym)
        note = new Diagnostic({sym->node, sym->node->token, Nte(Nte0003), DiagnosticLevel::Note});
    return raiseError(cxt, Err(San0001), note);
}

static bool checkStackOffset(Context& cxt, uint64_t stackOffset, SymbolOverload* overload = nullptr)
{
    if (overload && stackOffset >= overload->computedValue.storageOffset + overload->typeInfo->sizeOf)
        return raiseError(cxt, Fmt(Err(San0007), SymTable::getNakedKindName(overload).c_str(), overload->symbol->name.c_str()));
    if (stackOffset >= STATE().stack.count)
        return raiseError(cxt, Fmt(Err(San0002), stackOffset, STATE().stack.count));
    return true;
}

static bool getStackValue(Value& result, Context& cxt, void* addr, uint32_t sizeOf)
{
    auto offset = (uint8_t*) addr - STATE().stack.buffer;
    SWAG_ASSERT(sizeOf <= 8);
    SWAG_ASSERT(offset + sizeOf <= cxt.state.stackValue.count);
    auto addrValue = STATE().stackValue.buffer + offset;

    result.kind = addrValue->kind;
    if (result.kind == ValueKind::Invalid)
    {
        result.overload = nullptr;
        return true;
    }

    result.overload = addrValue->overload;
    addrValue++;

    for (uint32_t i = 1; i < sizeOf; i++)
    {
        auto value = *addrValue;
        addrValue++;

        if (value.kind == ValueKind::Invalid)
        {
            result.kind     = ValueKind::Invalid;
            result.overload = nullptr;
            return true;
        }

        if (value.kind != result.kind)
        {
            result.kind     = ValueKind::Unknown;
            result.overload = nullptr;
        }

        result = value;
    }

    return true;
}

static bool checkNotNull(Context& cxt, uint64_t value)
{
    if (!value)
        return raiseError(cxt, Err(San0005));
    return true;
}

static bool checkStackInitialized(Context& cxt, void* addr, uint32_t sizeOf, SymbolOverload* overload = nullptr)
{
    Value value;
    SWAG_CHECK(getStackValue(value, cxt, addr, sizeOf));
    if (value.kind == ValueKind::Invalid)
    {
        auto offset = (uint8_t*) addr - STATE().stack.buffer;
        auto sym    = getLocalVar(cxt, (uint32_t) offset);
        if (sym)
            return raiseError(cxt, Fmt(Err(San0004), sym->symbol->name.c_str()));
        return raiseError(cxt, Err(San0003));
    }

    return true;
}

static bool getRegister(Value*& result, Context& cxt, uint32_t reg)
{
    SWAG_ASSERT(reg < (uint32_t) STATE().regs.count);
    result = &STATE().regs[reg];
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

static bool getStackAddress(uint8_t*& result, Context& cxt, uint64_t stackOffset, SymbolOverload* overload = nullptr)
{
    SWAG_CHECK(checkStackOffset(cxt, stackOffset, overload));
    result = STATE().stack.buffer + stackOffset;
    return true;
}

static void setStackValue(Context& cxt, void* addr, uint32_t sizeOf, const Value& value)
{
    auto offset = (uint32_t) ((uint8_t*) addr - STATE().stack.buffer);
    SWAG_ASSERT(offset + sizeOf <= STATE().stackValue.count);

    for (uint32_t i = offset; i < offset + sizeOf; i++)
    {
        STATE().stackValue[i].kind     = value.kind;
        STATE().stackValue[i].overload = value.overload;
    }
}

bool ByteCodeOptimizer::optimizePassSanityStack(ByteCodeOptContext* context)
{
    auto&    cxt  = *(Context*) context->checkContext;
    Value*   ra   = nullptr;
    Value*   rb   = nullptr;
    Value*   rc   = nullptr;
    Value*   rd   = nullptr;
    uint8_t* addr = nullptr;
    Value    va, vb, vc;

    auto ip = STATE().ip;
    while (ip->op != ByteCodeOp::End)
    {
        if (ip->dynFlags & BCID_SAN_PASS)
            return true;

        if (ip->flags & BCI_SAFETY)
        {
            ip->dynFlags |= BCID_SAN_PASS;
            ip++;
            continue;
        }

        STATE().ip = ip;
        switch (ip->op)
        {
        case ByteCodeOp::Ret:
        case ByteCodeOp::InternalPanic:
        case ByteCodeOp::IntrinsicPanic:
            return true;

        case ByteCodeOp::SetBP:
        case ByteCodeOp::DecSPBP:
        case ByteCodeOp::IncSPPostCall:
        case ByteCodeOp::IncSPPostCallCond:
        case ByteCodeOp::PushRAParam:
        case ByteCodeOp::PushRAParam2:
        case ByteCodeOp::PushRAParam3:
        case ByteCodeOp::PushRAParam4:
        case ByteCodeOp::PushRAParamCond:
        case ByteCodeOp::PushRVParam:
        case ByteCodeOp::InternalInitStackTrace:
        case ByteCodeOp::InternalStackTrace:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
        case ByteCodeOp::InternalClearErr:
        case ByteCodeOp::InternalSetErr:
        case ByteCodeOp::InternalPushErr:
        case ByteCodeOp::InternalPopErr:
        case ByteCodeOp::CopyRCtoRT:
        case ByteCodeOp::IntrinsicErrorMsg:
            break;

            // Fake 1 value
        case ByteCodeOp::InternalGetTlsPtr:
        case ByteCodeOp::IntrinsicGetContext:
        case ByteCodeOp::IntrinsicGetProcessInfos:
        case ByteCodeOp::InternalHasErr:
        case ByteCodeOp::GetParam64:
        case ByteCodeOp::GetParam64SI:
        case ByteCodeOp::MakeBssSegPointer:
        case ByteCodeOp::MakeConstantSegPointer:
        case ByteCodeOp::MakeMutableSegPointer:
        case ByteCodeOp::MakeCompilerSegPointer:
        case ByteCodeOp::GetFromMutableSeg64:
        case ByteCodeOp::GetFromBssSeg64:
        case ByteCodeOp::GetFromCompilerSeg64:
        case ByteCodeOp::CopySP:
        case ByteCodeOp::CopyRTtoRC:
        case ByteCodeOp::CopyRRtoRC:
        case ByteCodeOp::CopySPVaargs:
        case ByteCodeOp::MakeLambda:
        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::IntrinsicStrLen:
        case ByteCodeOp::IntrinsicStrCmp:
        case ByteCodeOp::IntrinsicMemCmp:
        case ByteCodeOp::IntrinsicMakeCallback:
        case ByteCodeOp::CloneString:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->kind = ValueKind::Unknown;
            break;

        case ByteCodeOp::IntrinsicTypeCmp:
        case ByteCodeOp::IntrinsicStringCmp:
            SWAG_CHECK(getRegister(rd, cxt, ip->d.u32));
            rd->kind = ValueKind::Unknown;
            break;

            // Fake 2 values
        case ByteCodeOp::IntrinsicGvtd:
        case ByteCodeOp::IntrinsicGetErr:
        case ByteCodeOp::IntrinsicModules:
        case ByteCodeOp::DeRefStringSlice:
        case ByteCodeOp::CopyRTtoRC2:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
            ra->kind = ValueKind::Unknown;
            rb->kind = ValueKind::Unknown;
            break;

        case ByteCodeOp::Jump:
            ip->dynFlags |= BCID_SAN_PASS;
            ip += ip->b.s32 + 1;
            continue;

        case ByteCodeOp::JumpIfFalse:
            JUMP1(!va.reg.b);
            break;
        case ByteCodeOp::JumpIfTrue:
            JUMP1(va.reg.b);
            break;

        case ByteCodeOp::JumpIfEqual64:
            JUMP2(va.reg.u64 == vc.reg.u64);
            break;
        case ByteCodeOp::JumpIfLowerEqS64:
            JUMP2(va.reg.s64 <= vc.reg.s64);
            break;

        case ByteCodeOp::JumpIfZero8:
            JUMP1(va.reg.u8 == 0);
            break;
        case ByteCodeOp::JumpIfZero16:
            JUMP1(va.reg.u16 == 0);
            break;
        case ByteCodeOp::JumpIfZero32:
            JUMP1(va.reg.u32 == 0);
            break;
        case ByteCodeOp::JumpIfZero64:
            JUMP1(va.reg.u64 == 0);
            break;

        case ByteCodeOp::JumpIfNotZero8:
            JUMP1(va.reg.u8 != 0);
            break;
        case ByteCodeOp::JumpIfNotZero16:
            SWAG_CHECK(getImmediateA(va, cxt, ip));
            JUMP1(va.reg.u16 != 0);
            break;
        case ByteCodeOp::JumpIfNotZero32:
            SWAG_CHECK(getImmediateA(va, cxt, ip));
            JUMP1(va.reg.u32 != 0);
            break;
        case ByteCodeOp::JumpIfNotZero64:
            SWAG_CHECK(getImmediateA(va, cxt, ip));
            JUMP1(va.reg.u64 != 0);
            break;

        case ByteCodeOp::CopyRBtoRA64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
            *ra = *rb;
            break;

        case ByteCodeOp::CompareOp3WayS32:
        case ByteCodeOp::CompareOp3WayU32:
            SWAG_CHECK(getImmediateA(va, cxt, ip));
            SWAG_CHECK(getImmediateB(vb, cxt, ip));
            SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
            rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
            if (rc->kind == ValueKind::Constant)
            {
                auto sub    = va.reg.s32 - vb.reg.s32;
                rc->reg.s32 = (int32_t) ((sub > 0) - (sub < 0));
            }
            break;
        case ByteCodeOp::CompareOp3WayS64:
        case ByteCodeOp::CompareOp3WayU64:
            SWAG_CHECK(getImmediateA(va, cxt, ip));
            SWAG_CHECK(getImmediateB(vb, cxt, ip));
            SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
            rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
            if (rc->kind == ValueKind::Constant)
            {
                auto sub    = va.reg.s64 - vb.reg.s64;
                rc->reg.s32 = (int32_t) ((sub > 0) - (sub < 0));
            }
            break;

        case ByteCodeOp::ZeroToTrue:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.b = ra->reg.s32 == 0 ? true : false;
            break;
        case ByteCodeOp::GreaterEqZeroToTrue:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.b = ra->reg.s32 >= 0 ? true : false;
            break;
        case ByteCodeOp::GreaterZeroToTrue:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.b = ra->reg.s32 > 0 ? true : false;
            break;
        case ByteCodeOp::LowerEqZeroToTrue:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.b = ra->reg.s32 <= 0 ? true : false;
            break;
        case ByteCodeOp::LowerZeroToTrue:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.b = ra->reg.s32 < 0 ? true : false;
            break;

        case ByteCodeOp::IncrementRA64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.u64++;
            break;
        case ByteCodeOp::DecrementRA64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.u64--;
            break;

        case ByteCodeOp::MakeStackPointer:
            SWAG_CHECK(checkStackOffset(cxt, ip->b.u32));
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->kind     = ValueKind::StackAddr;
            ra->reg.u64  = ip->b.u32;
            ra->overload = (SymbolOverload*) ip->c.pointer;
            break;

        case ByteCodeOp::SetZeroStackX:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            memset(addr, 0, ip->b.u32);
            setStackValue(cxt, addr, ip->b.u32, {ValueKind::Constant});
            break;
        case ByteCodeOp::SetZeroStack8:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            *(uint8_t*) addr = 0;
            setStackValue(cxt, addr, 1, {ValueKind::Constant});
            break;
        case ByteCodeOp::SetZeroStack16:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            *(uint16_t*) addr = 0;
            setStackValue(cxt, addr, 2, {ValueKind::Constant});
            break;
        case ByteCodeOp::SetZeroStack32:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            *(uint32_t*) addr = 0;
            setStackValue(cxt, addr, 4, {ValueKind::Constant});
            break;
        case ByteCodeOp::SetZeroStack64:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            *(uint64_t*) addr = 0;
            setStackValue(cxt, addr, 8, {ValueKind::Constant});
            break;

        case ByteCodeOp::SetAtStackPointer64:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            SWAG_CHECK(getImmediateB(vb, cxt, ip));
            *(uint64_t*) addr = vb.reg.u64;
            setStackValue(cxt, addr, 8, vb);
            break;

        case ByteCodeOp::SetZeroAtPointerX:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64 + ip->c.u32));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
                memset(addr, 0, ip->b.u64);
                setStackValue(cxt, addr, (uint32_t) ip->b.u64, {ValueKind::Constant});
            }
            break;
        case ByteCodeOp::SetZeroAtPointer8:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32));
                *(uint8_t*) addr = 0;
                setStackValue(cxt, addr, 1, {ValueKind::Constant});
            }
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32));
                *(uint16_t*) addr = 0;
                setStackValue(cxt, addr, 2, {ValueKind::Constant});
            }
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32));
                *(uint32_t*) addr = 0;
                setStackValue(cxt, addr, 4, {ValueKind::Constant});
            }
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32));
                *(uint64_t*) addr = 0;
                setStackValue(cxt, addr, 8, {ValueKind::Constant});
            }
            break;

        case ByteCodeOp::SetAtPointer8:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                *(uint8_t*) addr = vb.reg.u8;
                setStackValue(cxt, addr, 1, vb);
            }
            break;
        case ByteCodeOp::SetAtPointer16:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                *(uint16_t*) addr = vb.reg.u16;
                setStackValue(cxt, addr, 2, vb);
            }
            break;
        case ByteCodeOp::SetAtPointer32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                *(uint32_t*) addr = vb.reg.u32;
                setStackValue(cxt, addr, 4, vb);
            }
            break;
        case ByteCodeOp::SetAtPointer64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, ra->reg.u64));
            if (ra->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
                SWAG_CHECK(getImmediateB(vb, cxt, ip));
                *(uint64_t*) addr = vb.reg.u64;
                setStackValue(cxt, addr, 8, vb);
            }
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

        case ByteCodeOp::Add64byVB64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.s64 += ip->b.u64;
            break;
        case ByteCodeOp::Mul64byVB64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.s64 *= ip->b.u64;
            break;
        case ByteCodeOp::Div64byVB64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.s64 /= ip->b.u64;
            break;
        case ByteCodeOp::MulAddVC64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
            if (rb->kind == ValueKind::Unknown)
                ra->kind = ValueKind::Unknown;
            else
                ra->reg.u64 = ra->reg.u64 * (rb->reg.u64 + ip->c.u32);
            break;

        case ByteCodeOp::GetFromStack8:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
            SWAG_CHECK(checkStackInitialized(cxt, addr, 1));
            SWAG_CHECK(getStackValue(*ra, cxt, addr, 1));
            ra->reg.u64 = *(uint8_t*) addr;
            break;
        case ByteCodeOp::GetFromStack16:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
            SWAG_CHECK(checkStackInitialized(cxt, addr, 2));
            SWAG_CHECK(getStackValue(*ra, cxt, addr, 2));
            ra->reg.u64 = *(uint16_t*) addr;
            break;
        case ByteCodeOp::GetFromStack32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
            SWAG_CHECK(checkStackInitialized(cxt, addr, 4));
            SWAG_CHECK(getStackValue(*ra, cxt, addr, 4));
            ra->reg.u64 = *(uint32_t*) addr;
            break;
        case ByteCodeOp::GetFromStack64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
            SWAG_CHECK(checkStackInitialized(cxt, addr, 8));
            SWAG_CHECK(getStackValue(*ra, cxt, addr, 8));
            ra->reg.u64 = *(uint64_t*) addr;
            break;

        case ByteCodeOp::IncPointer64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
            *rc = *ra;
            SWAG_CHECK(getImmediateB(vb, cxt, ip));
            if (vb.kind == ValueKind::Unknown)
                rc->kind = ValueKind::Unknown;
            else
                rc->reg.u64 += vb.reg.s64;
            break;
        case ByteCodeOp::DecPointer64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
            *rc = *ra;
            SWAG_CHECK(getImmediateB(vb, cxt, ip));
            if (vb.kind == ValueKind::Unknown)
                rc->kind = ValueKind::Unknown;
            else
                rc->reg.u64 -= vb.reg.s64;
            break;

        case ByteCodeOp::CopyRCtoRR:
            if (ip->flags & BCI_IMM_A)
                break;
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::StackAddr)
                return checkEscapeFrame(cxt, ra->reg.u32, ra->overload);
            break;

        case ByteCodeOp::CopyRCtoRR2:
            if (ip->flags & BCI_IMM_A)
                break;
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
            if (ra->kind == ValueKind::StackAddr)
                return checkEscapeFrame(cxt, ra->reg.u32, ra->overload);
            break;

        case ByteCodeOp::DeRef8:
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
            if (rb->kind == ValueKind::Constant)
                SWAG_CHECK(checkNotNull(cxt, rb->reg.u64));

            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (rb->kind == ValueKind::StackAddr)
            {
                SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, rb->overload));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 1));
                SWAG_CHECK(getStackValue(*ra, cxt, addr, 1));
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
                SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, rb->overload));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 2));
                SWAG_CHECK(getStackValue(*ra, cxt, addr, 2));
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
                SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, rb->overload));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 4));
                SWAG_CHECK(getStackValue(*ra, cxt, addr, 4));
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
                SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, rb->overload));
                SWAG_CHECK(checkStackInitialized(cxt, addr, 8));
                SWAG_CHECK(getStackValue(*ra, cxt, addr, 8));
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
            BINOPEQDIV(uint8_t, /=, s8);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            BINOPEQDIV(uint16_t, /=, s16);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            BINOPEQDIV(uint32_t, /=, s32);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            BINOPEQDIV(uint64_t, /=, s64);
            break;
        case ByteCodeOp::AffectOpDivEqU8:
            BINOPEQDIV(uint8_t, /=, u8);
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            BINOPEQDIV(uint16_t, /=, u16);
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            BINOPEQDIV(uint32_t, /=, u32);
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            BINOPEQDIV(uint64_t, /=, u64);
            break;
        case ByteCodeOp::AffectOpDivEqF32:
            BINOPEQDIV(float, /=, f32);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            BINOPEQDIV(double, /=, f64);
            break;

        case ByteCodeOp::AffectOpModuloEqS8:
            BINOPEQDIV(uint8_t, %=, s8);
            break;
        case ByteCodeOp::AffectOpModuloEqS16:
            BINOPEQDIV(uint16_t, %=, s16);
            break;
        case ByteCodeOp::AffectOpModuloEqS32:
            BINOPEQDIV(uint32_t, %=, s32);
            break;
        case ByteCodeOp::AffectOpModuloEqS64:
            BINOPEQDIV(uint64_t, %=, s64);
            break;
        case ByteCodeOp::AffectOpModuloEqU8:
            BINOPEQDIV(uint8_t, %=, u8);
            break;
        case ByteCodeOp::AffectOpModuloEqU16:
            BINOPEQDIV(uint16_t, %=, u16);
            break;
        case ByteCodeOp::AffectOpModuloEqU32:
            BINOPEQDIV(uint32_t, %=, u32);
            break;
        case ByteCodeOp::AffectOpModuloEqU64:
            BINOPEQDIV(uint64_t, %=, u64);
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

        case ByteCodeOp::AffectOpShiftLeftEqU8:
            BINOPEQ2(uint8_t, <<=, u8, u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            BINOPEQ2(uint8_t, <<=, u16, u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            BINOPEQ2(uint8_t, <<=, u32, u32);
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            BINOPEQ2(uint8_t, <<=, u64, u32);
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            BINOPEQ2(uint8_t, >>=, s8, u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            BINOPEQ2(uint8_t, >>=, s16, u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            BINOPEQ2(uint8_t, >>=, s32, u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            BINOPEQ2(uint8_t, >>=, s64, u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            BINOPEQ2(uint8_t, >>=, u8, u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            BINOPEQ2(uint8_t, >>=, u16, u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            BINOPEQ2(uint8_t, >>=, u32, u32);
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            BINOPEQ2(uint8_t, >>=, u64, u32);
            break;

        case ByteCodeOp::NegBool:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getImmediateB(vb, cxt, ip));
            ra->kind  = vb.kind;
            ra->reg.b = !vb.reg.b;
            break;
        case ByteCodeOp::NegS32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.s32 = -vb.reg.s32;
            break;
        case ByteCodeOp::NegS64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.s64 = -vb.reg.s64;
            break;
        case ByteCodeOp::NegF32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.f32 = -vb.reg.f32;
            break;
        case ByteCodeOp::NegF64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.f64 = -vb.reg.f64;
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
        case ByteCodeOp::CastF32F64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.f64 = (double) ra->reg.f32;
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
        case ByteCodeOp::CompareOpEqualF32:
            CMPOP(==, f32);
            break;
        case ByteCodeOp::CompareOpEqualF64:
            CMPOP(==, f64);
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
        case ByteCodeOp::CompareOpNotEqualF32:
            CMPOP(!=, f32);
            break;
        case ByteCodeOp::CompareOpNotEqualF64:
            CMPOP(!=, f64);
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

        case ByteCodeOp::BinOpXorU8:
            BINOP(^, u8);
            break;
        case ByteCodeOp::BinOpXorU16:
            BINOP(^, u16);
            break;
        case ByteCodeOp::BinOpXorU32:
            BINOP(^, u32);
            break;
        case ByteCodeOp::BinOpXorU64:
            BINOP(^, u64);
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
            BINOPDIV(/, s32);
            break;
        case ByteCodeOp::BinOpDivS64:
            BINOPDIV(/, s64);
            break;
        case ByteCodeOp::BinOpDivU32:
            BINOPDIV(/, u32);
            break;
        case ByteCodeOp::BinOpDivU64:
            BINOPDIV(/, u64);
            break;
        case ByteCodeOp::BinOpDivF32:
            BINOPDIV(/, f32);
            break;
        case ByteCodeOp::BinOpDivF64:
            BINOPDIV(/, f64);
            break;

        case ByteCodeOp::BinOpModuloS32:
            BINOPDIV(%, s32);
            break;
        case ByteCodeOp::BinOpModuloS64:
            BINOPDIV(%, s64);
            break;
        case ByteCodeOp::BinOpModuloU32:
            BINOPDIV(%, u32);
            break;
        case ByteCodeOp::BinOpModuloU64:
            BINOPDIV(%, u64);
            break;

        case ByteCodeOp::BinOpShiftLeftU8:
            BINOP2(<<, u8, u32);
            break;
        case ByteCodeOp::BinOpShiftLeftU16:
            BINOP2(<<, u16, u32);
            break;
        case ByteCodeOp::BinOpShiftLeftU32:
            BINOP2(<<, u32, u32);
            break;
        case ByteCodeOp::BinOpShiftLeftU64:
            BINOP2(<<, u64, u32);
            break;

        case ByteCodeOp::BinOpShiftRightS8:
            BINOP2(>>, s8, u32);
            break;
        case ByteCodeOp::BinOpShiftRightS16:
            BINOP2(>>, s16, u32);
            break;
        case ByteCodeOp::BinOpShiftRightS32:
            BINOP2(>>, s32, u32);
            break;
        case ByteCodeOp::BinOpShiftRightS64:
            BINOP2(>>, s64, u32);
            break;
        case ByteCodeOp::BinOpShiftRightU8:
            BINOP2(>>, u8, u32);
            break;
        case ByteCodeOp::BinOpShiftRightU16:
            BINOP2(>>, u16, u32);
            break;
        case ByteCodeOp::BinOpShiftRightU32:
            BINOP2(>>, u32, u32);
            break;
        case ByteCodeOp::BinOpShiftRightU64:
            BINOP2(>>, u64, u32);
            break;

        case ByteCodeOp::InvertU8:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.u8 = ~ra->reg.u8;
            break;
        case ByteCodeOp::InvertU16:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.u16 = ~ra->reg.u16;
            break;
        case ByteCodeOp::InvertU32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.u32 = ~ra->reg.u32;
            break;
        case ByteCodeOp::InvertU64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.u64 = ~ra->reg.u64;
            break;

        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::LocalCall:
        case ByteCodeOp::ForeignCall:
            setStackValue(cxt, STATE().stack.buffer, STATE().stack.count, {ValueKind::Unknown});
            break;

        default:
            for (int i = 0; i < STATE().regs.count; i++)
                STATE().regs[i] = {ValueKind::Unknown};
            setStackValue(cxt, STATE().stack.buffer, STATE().stack.count, {ValueKind::Unknown});

            // printf("%s\n", g_ByteCodeOpDesc[(int) ip->op].name);
            cxt.incomplete = true;
            break;
        }

        ip->dynFlags |= BCID_SAN_PASS;
        ip++;
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassSanity(ByteCodeOptContext* context)
{
#if 0
    if (context->bc->name != "__compiler4176.toto")
        return true;
    context->bc->print();
#endif

    if (!context->bc->node || context->bc->isCompilerGenerated)
        return true;

    Context cxt;
    State   state;

    cxt.bc         = context->bc;
    state.ip       = nullptr;
    cxt.context    = context;
    cxt.incomplete = false;

    auto funcDecl = CastAst<AstFuncDecl>(cxt.bc->node, AstNodeKind::FuncDecl);

    state.stack.reserve(funcDecl->stackSize);
    memset(state.stack.buffer, 0, funcDecl->stackSize * sizeof(uint8_t));
    state.stack.count = funcDecl->stackSize;

    state.stackValue.reserve(funcDecl->stackSize);
    memset(state.stackValue.buffer, (uint8_t) ValueKind::Invalid, funcDecl->stackSize * sizeof(uint8_t));
    state.stackValue.count = funcDecl->stackSize;

    state.regs.reserve(context->bc->maxReservedRegisterRC);
    memset(state.regs.buffer, 0, context->bc->maxReservedRegisterRC * sizeof(Value));
    state.regs.count = context->bc->maxReservedRegisterRC;

    context->checkContext = &cxt;

    state.ip = cxt.bc->out;
    cxt.states.emplace_back(state);

    for (int i = 0; i < cxt.states.size(); i++)
    {
        cxt.state = i;
        SWAG_CHECK(optimizePassSanityStack(context));
        if (i == cxt.states.size() - 1)
            break;
        for (uint32_t j = 0; j < cxt.bc->numInstructions; j++)
            cxt.bc->out[j].dynFlags &= ~BCID_SAN_PASS;
    }

    if (g_CommandLine.stats && !cxt.incomplete)
        g_Stats.numSanDone++;
    else if (g_CommandLine.stats && cxt.incomplete)
        g_Stats.numSanIncomplete++;

    return true;
}
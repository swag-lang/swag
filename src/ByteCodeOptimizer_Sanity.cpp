#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeOptimizer.h"
#include "ByteCode_Math.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Report.h"
#include "TypeManager.h"

#define STATE() cxt.states[cxt.state]

#define MEMCPY(__cast, __sizeof)                                               \
    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                               \
    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));                               \
    if (ra->kind == ValueKind::StackAddr && rb->kind == ValueKind::StackAddr)  \
    {                                                                          \
        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, __sizeof));         \
        SWAG_CHECK(getStackAddress(addr2, cxt, rb->reg.u32, __sizeof));        \
        SWAG_CHECK(checkStackInitialized(cxt, addr2, __sizeof, rb->overload)); \
        SWAG_CHECK(getStackValue(vb, cxt, addr2, __sizeof));                   \
        setStackValue(cxt, addr, __sizeof, vb.kind);                           \
        *(__cast*) addr = *(__cast*) addr2;                                    \
        break;                                                                 \
    }                                                                          \
    invalidateCurStateStack(cxt)

#define BINOP_EQ(__cast, __op, __reg)                                                         \
    do                                                                                        \
    {                                                                                         \
        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                                          \
        SWAG_CHECK(checkNotNull(cxt, ra));                                                    \
        if (ra->kind == ValueKind::StackAddr)                                                 \
        {                                                                                     \
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, sizeof(vb.reg.__reg)));        \
            SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg), ra->overload)); \
            SWAG_CHECK(getStackValue(va, cxt, addr, sizeof(vb.reg.__reg)));                   \
            SWAG_CHECK(getImmediateB(vb, cxt, ip));                                           \
            if (va.kind == ValueKind::Unknown || vb.kind == ValueKind::Unknown)               \
                setStackValue(cxt, addr, sizeof(vb.reg.__reg), ValueKind::Unknown);           \
            else                                                                              \
            {                                                                                 \
                *(__cast*) addr __op vb.reg.__reg;                                            \
                setStackConstant(cxt, va.kind, ip, ra->reg.u32, addr, sizeof(vb.reg.__reg));  \
            }                                                                                 \
        }                                                                                     \
    } while(0)

#define BINOP_EQ_OVF(__cast, __op, __reg, __ovf, __msg, __type)                                                              \
    do                                                                                                                       \
    {                                                                                                                        \
        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                                                                         \
        SWAG_CHECK(checkNotNull(cxt, ra));                                                                                   \
        if (ra->kind == ValueKind::StackAddr)                                                                                \
        {                                                                                                                    \
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, sizeof(vb.reg.__reg)));                                       \
            SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg), ra->overload));                                \
            SWAG_CHECK(getStackValue(va, cxt, addr, sizeof(vb.reg.__reg)));                                                  \
            SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                          \
            if (va.kind == ValueKind::Unknown || vb.kind == ValueKind::Unknown)                                              \
                setStackValue(cxt, addr, sizeof(vb.reg.__reg), ValueKind::Unknown);                                          \
            else                                                                                                             \
            {                                                                                                                \
                SWAG_CHECK(checkOverflow(cxt, !__ovf(ip, ip->node, *(__cast*) addr, (__cast) vb.reg.__reg), __msg, __type)); \
                *(__cast*) addr __op vb.reg.__reg;                                                                           \
                setStackConstant(cxt, va.kind, ip, ra->reg.u32, addr, sizeof(vb.reg.__reg));                                 \
            }                                                                                                                \
        }                                                                                                                    \
    } while(0)

#define ATOM_EQ(__cast, __op, __reg)                                                          \
    do                                                                                        \
    {                                                                                         \
        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                                          \
        SWAG_CHECK(checkNotNull(cxt, ra));                                                    \
        SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                          \
        rc->kind = ValueKind::Unknown;                                                        \
        if (ra->kind == ValueKind::StackAddr)                                                 \
        {                                                                                     \
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, sizeof(vb.reg.__reg)));        \
            SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg), ra->overload)); \
            SWAG_CHECK(getStackValue(*rc, cxt, addr, sizeof(vb.reg.__reg)));                  \
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));                                      \
            if (rc->kind == ValueKind::Unknown || rb->kind == ValueKind::Unknown)             \
                setStackValue(cxt, addr, sizeof(vb.reg.__reg), ValueKind::Unknown);           \
            else                                                                              \
            {                                                                                 \
                rc->reg.__reg = *(__cast*) addr;                                              \
                *(__cast*) addr __op rb->reg.__reg;                                           \
            }                                                                                 \
        }                                                                                     \
    } while(0)

#define ATOM_EQ_XCHG(__cast, __reg)                                                                                 \
    do                                                                                                              \
    {                                                                                                               \
        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                                                                \
        SWAG_CHECK(checkNotNull(cxt, ra));                                                                          \
        SWAG_CHECK(getRegister(rd, cxt, ip->d.u32));                                                                \
        rd->kind = ValueKind::Unknown;                                                                              \
        if (ra->kind == ValueKind::StackAddr)                                                                       \
        {                                                                                                           \
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, sizeof(vb.reg.__reg)));                              \
            SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg), ra->overload));                       \
            SWAG_CHECK(getStackValue(*rd, cxt, addr, sizeof(vb.reg.__reg)));                                        \
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));                                                            \
            SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                            \
            if (rd->kind == ValueKind::Unknown || rb->kind == ValueKind::Unknown || rc->kind == ValueKind::Unknown) \
                setStackValue(cxt, addr, sizeof(vb.reg.__reg), ValueKind::Unknown);                                 \
            else                                                                                                    \
            {                                                                                                       \
                rd->reg.__reg = *(__cast*) addr;                                                                    \
                if (rd->reg.__reg == rb->reg.__reg)                                                                 \
                    *(__cast*) addr = rc->reg.__reg;                                                                \
            }                                                                                                       \
        }                                                                                                           \
    } while(0)

#define BINOP_EQ_DIV(__cast, __op, __reg)                               \
    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                        \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                             \
    SWAG_CHECK(checkDivZero(cxt, vb, vb.reg.__reg == 0, ra->overload)); \
    if (vb.kind == ValueKind::Constant && vb.reg.__reg == 0)            \
    {                                                                   \
        SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                    \
        rc->kind = ValueKind::Unknown;                                  \
        break;                                                          \
    }                                                                   \
    BINOP_EQ(__cast, __op, __reg)

#define BINOP_EQ_SHIFT(__cast, __reg, __func, __isSigned)                                     \
    do                                                                                        \
    {                                                                                         \
        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                                          \
        SWAG_CHECK(checkNotNull(cxt, ra));                                                    \
        if (ra->kind == ValueKind::StackAddr)                                                 \
        {                                                                                     \
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, sizeof(vb.reg.__reg)));        \
            SWAG_CHECK(checkStackInitialized(cxt, addr, sizeof(vb.reg.__reg), ra->overload)); \
            SWAG_CHECK(getStackValue(va, cxt, addr, sizeof(vb.reg.__reg)));                   \
            SWAG_CHECK(getImmediateB(vb, cxt, ip));                                           \
            if (va.kind == ValueKind::Unknown || vb.kind == ValueKind::Unknown)               \
                setStackValue(cxt, addr, sizeof(vb.reg.__reg), ValueKind::Unknown);           \
            else                                                                              \
            {                                                                                 \
                Register r;                                                                   \
                r.__reg = *(__cast*) addr;                                                    \
                __func(&r, r, vb.reg, sizeof(vb.reg.__reg) * 8, __isSigned);                  \
                *(__cast*) addr = r.__reg;                                                    \
            }                                                                                 \
        }                                                                                     \
    } while(0)

#define BINOP_SHIFT(__cast, __reg, __func, __isSigned)                                                                      \
    SWAG_CHECK(getImmediateA(va, cxt, ip));                                                                                 \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                                 \
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                                            \
    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown; \
    if (rc->kind == ValueKind::Constant)                                                                                    \
    {                                                                                                                       \
        __func(&rc->reg, va.reg, vb.reg, sizeof(va.reg.__reg) * 8, __isSigned);                                             \
    }                                                                                                                       \
    setConstant(cxt, rc->kind, ip, rc->reg.u64, ConstantKind::SetImmediateC)

#define BINOP(__op, __reg)                                                                                                  \
    SWAG_CHECK(getImmediateA(va, cxt, ip));                                                                                 \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                                 \
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                                            \
    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown; \
    if (rc->kind == ValueKind::Constant)                                                                                    \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;                                                                     \
    setConstant(cxt, rc->kind, ip, rc->reg.u64, ConstantKind::SetImmediateC)

#define BINOP_OVF(__op, __reg, __ovf, __msg, __type)                                                                        \
    SWAG_CHECK(getImmediateA(va, cxt, ip));                                                                                 \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                                 \
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                                            \
    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown; \
    if (rc->kind == ValueKind::Constant)                                                                                    \
    {                                                                                                                       \
        SWAG_CHECK(checkOverflow(cxt, !__ovf(ip, ip->node, va.reg.__reg, vb.reg.__reg), __msg, __type));                    \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;                                                                     \
    }                                                                                                                       \
    setConstant(cxt, rc->kind, ip, rc->reg.u64, ConstantKind::SetImmediateC)

#define BINOP_DIV(__op, __reg)                                          \
    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));                        \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                             \
    SWAG_CHECK(checkDivZero(cxt, vb, vb.reg.__reg == 0, ra->overload)); \
    if (vb.kind == ValueKind::Constant && vb.reg.__reg == 0)            \
    {                                                                   \
        SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                    \
        rc->kind = ValueKind::Unknown;                                  \
        break;                                                          \
    }                                                                   \
    BINOP(__op, __reg)

#define CMP_OP(__op, __reg)                                                                                                 \
    SWAG_CHECK(getImmediateA(va, cxt, ip));                                                                                 \
    SWAG_CHECK(getImmediateB(vb, cxt, ip));                                                                                 \
    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));                                                                            \
    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown; \
    if (rc->kind == ValueKind::Constant)                                                                                    \
        rc->reg.b = va.reg.__reg __op vb.reg.__reg;                                                                         \
    setConstant(cxt, rc->kind, ip, rc->reg.u64, ConstantKind::SetImmediateC)

#define JUMPT(__expr0, __expr1)                       \
    jmpAddState = nullptr;                            \
    if (__expr0)                                      \
    {                                                 \
        ip->dynFlags |= BCID_SAN_PASS;                \
        if (__expr1)                                  \
            ip += ip->b.s32 + 1;                      \
        else                                          \
            ip = ip + 1;                              \
        continue;                                     \
    }                                                 \
    if (cxt.statesHere.contains(ip + ip->b.s32 + 1))  \
        return true;                                  \
    cxt.statesHere.insert(ip + ip->b.s32 + 1);        \
    cxt.states.push_back(new State);                  \
    jmpAddState                 = cxt.states.back();  \
    *cxt.states.back()          = *STATE();           \
    cxt.states.back()->branchIp = ip;                 \
    cxt.states.back()->ip       = ip + ip->b.s32 + 1; \
    cxt.states.back()->parent   = cxt.state

#define JUMP1(__expr)                       \
    SWAG_CHECK(getImmediateA(va, cxt, ip)); \
    JUMPT(va.kind == ValueKind::Constant, __expr)

#define JUMP2(__expr)                       \
    SWAG_CHECK(getImmediateA(va, cxt, ip)); \
    SWAG_CHECK(getImmediateC(vc, cxt, ip)); \
    JUMPT(va.kind == ValueKind::Constant && vc.kind == ValueKind::Constant, __expr)

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

enum class ConstantKind
{
    SetImmediateA,
    SetImmediateC,
    SetImmediateD,
};

struct Context
{
    ByteCodeOptContext*       context = nullptr;
    ByteCode*                 bc      = nullptr;
    int                       state   = 0;
    Set<ByteCodeInstruction*> statesHere;
    Vector<State*>            states;
    bool                      canSetConstants = true;
};

namespace
{
    void setStackConstant(const Context& cxt, ValueKind kind, ByteCodeInstruction* ip, uint32_t offset, uint8_t* value, int sizeOf)
    {
        if (!cxt.canSetConstants)
            return;
        if (kind != ValueKind::Constant)
            return;
        if (!cxt.bc->sourceFile->module->mustOptimizeBytecode(cxt.bc->node))
            return;

        const auto context = cxt.context;
        switch (sizeOf)
        {
            case 1:
                SET_OP(ip, ByteCodeOp::SetAtStackPointer8);
                ip->addFlag(BCI_IMM_B);
                ip->a.u32 = offset;
                ip->b.u64 = *value;
                break;
            case 2:
                SET_OP(ip, ByteCodeOp::SetAtStackPointer16);
                ip->addFlag(BCI_IMM_B);
                ip->a.u32 = offset;
                ip->b.u64 = *reinterpret_cast<uint16_t*>(value);
                break;
            case 4:
                SET_OP(ip, ByteCodeOp::SetAtStackPointer32);
                ip->addFlag(BCI_IMM_B);
                ip->a.u32 = offset;
                ip->b.u64 = *reinterpret_cast<uint32_t*>(value);
                break;
            case 8:
                SET_OP(ip, ByteCodeOp::SetAtStackPointer64);
                ip->addFlag(BCI_IMM_B);
                ip->a.u32 = offset;
                ip->b.u64 = *reinterpret_cast<uint64_t*>(value);
                break;
            default:
                break;
        }
    }

    void setConstant(const Context& cxt, ValueKind kind, ByteCodeInstruction* ip, uint64_t value, ConstantKind cstKind)
    {
        if (!cxt.canSetConstants)
            return;
        if (kind != ValueKind::Constant)
            return;
        if (!cxt.bc->sourceFile->module->mustOptimizeBytecode(cxt.bc->node))
            return;

        const auto context = cxt.context;
        switch (cstKind)
        {
            case ConstantKind::SetImmediateA:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->b.u64 = value;
                break;
            case ConstantKind::SetImmediateC:
                SET_OP(ip, ByteCodeOp::SetImmediate64);
                ip->a.u32 = ip->c.u32;
                ip->b.u64 = value;
                break;
            default:
                break;
        }
    }

    bool getStackValue(Value& result, const Context& cxt, void* addr, uint32_t sizeOf)
    {
        const auto offset = static_cast<uint8_t*>(addr) - STATE()->stack.buffer;
        SWAG_ASSERT(offset + sizeOf <= STATE()->stackValue.count);
        auto addrValue = STATE()->stackValue.buffer + offset;

        result.kind     = addrValue->kind;
        result.overload = addrValue->overload;
        addrValue++;

        for (uint32_t i = 1; i < sizeOf; i++)
        {
            const auto value = *addrValue;
            addrValue++;

            if (value.kind != result.kind)
            {
                result.kind     = ValueKind::Unknown;
                result.overload = nullptr;
                return true;
            }

            result = value;
        }

        return true;
    }
}

/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////

namespace
{
    bool raiseError(const Context& cxt, const Utf8& msg, const SymbolOverload* overload = nullptr)
    {
        if (!cxt.bc->sourceFile->module->mustEmitSafety(STATE()->ip->node, SAFETY_SANITY))
            return true;

        AstNode*   nodeLoc = nullptr;
        const auto ip      = cxt.states[cxt.state]->ip;
        if (overload && ip->node)
        {
            Ast::visit(ip->node, [&](AstNode* n)
            {
                if (n->resolvedSymbolOverload == overload && !nodeLoc)
                {
                    nodeLoc = n;
                    return;
                }
            });
        }

        if (nodeLoc)
        {
            const Diagnostic err({nodeLoc, nodeLoc->token, msg});
            return cxt.context->report(err);
        }

        const auto loc = ByteCode::getLocation(cxt.bc, cxt.states[cxt.state]->ip);

        const Diagnostic err({loc.file, *loc.location, msg});
        return cxt.context->report(err);
    }

    bool checkOverflow(const Context& cxt, bool isValid, const char* msgKind, TypeInfo* type)
    {
        if (isValid)
            return true;
        return raiseError(cxt, FMT(Err(San0010), msgKind, type->getDisplayNameC()));
    }

    bool checkDivZero(const Context& cxt, const Value& value, bool isZero, const SymbolOverload* overload = nullptr)
    {
        if (value.kind != ValueKind::Constant)
            return true;
        if (!isZero)
            return true;
        if (overload)
            return raiseError(cxt, FMT(Err(San0002), Naming::kindName(overload).c_str(), overload->symbol->name.c_str()), overload);
        return raiseError(cxt, Err(San0001));
    }

    bool checkEscapeFrame(const Context& cxt, uint64_t stackOffset, const SymbolOverload* overload = nullptr)
    {
        SWAG_ASSERT(stackOffset < UINT32_MAX);
        if (overload)
            return raiseError(cxt, FMT(Err(San0004), Naming::kindName(overload).c_str(), overload->symbol->name.c_str()), overload);
        return raiseError(cxt, Err(San0003));
    }

    bool checkStackOffset(const Context& cxt, uint64_t stackOffset, uint32_t sizeOf = 0)
    {
        if (stackOffset + sizeOf > static_cast<size_t>(STATE()->stack.count))
            return raiseError(cxt, FMT(Err(San0007), stackOffset + sizeOf, STATE()->stack.count));
        return true;
    }

    bool checkNotNull(const Context& cxt, const Value* value)
    {
        if (value->kind != ValueKind::Constant)
            return true;
        if (value->reg.u64)
            return true;
        if (value->overload)
            return raiseError(cxt, FMT(Err(San0006), Naming::kindName(value->overload).c_str(), value->overload->symbol->name.c_str()), value->overload);
        return raiseError(cxt, Err(San0005));
    }

    bool checkStackInitialized(const Context& cxt, void* addr, uint32_t sizeOf, const SymbolOverload* overload = nullptr)
    {
        Value value;
        SWAG_CHECK(getStackValue(value, cxt, addr, sizeOf));
        if (value.kind == ValueKind::Invalid)
        {
            if (overload)
                return raiseError(cxt, FMT(Err(San0008), Naming::kindName(overload).c_str(), overload->symbol->name.c_str()), overload);
            return raiseError(cxt, Err(San0009));
        }

        return true;
    }
}

/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////

namespace
{
    bool getRegister(Value*& result, const Context& cxt, uint32_t reg)
    {
        SWAG_ASSERT(reg < STATE()->regs.count);
        result = &STATE()->regs[reg];
        return true;
    }

    bool getImmediateA(Value& result, const Context& cxt, const ByteCodeInstruction* ip)
    {
        if (ip->hasFlag(BCI_IMM_A))
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

    bool getImmediateB(Value& result, const Context& cxt, const ByteCodeInstruction* ip)
    {
        if (ip->hasFlag(BCI_IMM_B))
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

    bool getImmediateC(Value& result, const Context& cxt, const ByteCodeInstruction* ip)
    {
        if (ip->hasFlag(BCI_IMM_C))
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

    bool getImmediateD(Value& result, const Context& cxt, const ByteCodeInstruction* ip)
    {
        if (ip->hasFlag(BCI_IMM_D))
        {
            result.kind = ValueKind::Constant;
            result.reg  = ip->d;
            return true;
        }

        Value* rd = nullptr;
        SWAG_CHECK(getRegister(rd, cxt, ip->d.u32));
        result = *rd;
        return true;
    }

    bool getStackAddress(uint8_t*& result, const Context& cxt, uint64_t stackOffset, uint32_t sizeOf = 0)
    {
        SWAG_CHECK(checkStackOffset(cxt, stackOffset, sizeOf));
        result = STATE()->stack.buffer + stackOffset;
        return true;
    }

    void setStackValue(const Context& cxt, void* addr, uint32_t sizeOf, ValueKind kind)
    {
        const auto offset = static_cast<uint32_t>(static_cast<uint8_t*>(addr) - STATE()->stack.buffer);
        SWAG_ASSERT(offset + sizeOf <= STATE()->stackValue.count);
        for (uint32_t i                 = offset; i < offset + sizeOf; i++)
            STATE()->stackValue[i].kind = kind;
    }

    void invalidateCurStateStack(const Context& cxt)
    {
        setStackValue(cxt, STATE()->stack.buffer, STATE()->stack.count, ValueKind::Unknown);
    }

    bool optimizePassSanityStack(ByteCodeOptContext* context, Context& cxt)
    {
        Value*   ra    = nullptr;
        Value*   rb    = nullptr;
        Value*   rc    = nullptr;
        Value*   rd    = nullptr;
        uint8_t* addr  = nullptr;
        uint8_t* addr2 = nullptr;
        Value    va, vb, vc, vd;
        State*   jmpAddState = nullptr;

        auto ip = STATE()->ip;
        while (ip->op != ByteCodeOp::End)
        {
            if (ip->dynFlags & BCID_SAN_PASS)
                return true;

            if (ip->hasFlag(BCI_SAFETY))
            {
                ip->dynFlags |= BCID_SAN_PASS;
                ip++;
                continue;
            }

            if (ip->hasFlag(BCI_START_STMT_N))
                cxt.canSetConstants = false;

            STATE()->ip = ip;
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
                case ByteCodeOp::InternalStackTraceConst:
                case ByteCodeOp::PushRR:
                case ByteCodeOp::PopRR:
                case ByteCodeOp::InternalSetErr:
                case ByteCodeOp::InternalPushErr:
                case ByteCodeOp::InternalPopErr:
                case ByteCodeOp::InternalCatchErr:
                case ByteCodeOp::InternalFailedAssume:
                case ByteCodeOp::IntrinsicCompilerError:
                case ByteCodeOp::IntrinsicCompilerWarning:
                case ByteCodeOp::IntrinsicSetContext:
                case ByteCodeOp::IntrinsicCVaEnd:
                case ByteCodeOp::IntrinsicFree:
                case ByteCodeOp::DebugNop:
                case ByteCodeOp::Unreachable:
                case ByteCodeOp::InternalUnreachable:
                case ByteCodeOp::IntrinsicBcBreakpoint:
                case ByteCodeOp::ClearRR8:
                case ByteCodeOp::ClearRR16:
                case ByteCodeOp::ClearRR32:
                case ByteCodeOp::ClearRR64:
                case ByteCodeOp::ClearRRX:
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
                case ByteCodeOp::GetFromMutableSeg8:
                case ByteCodeOp::GetFromMutableSeg16:
                case ByteCodeOp::GetFromMutableSeg32:
                case ByteCodeOp::GetFromMutableSeg64:
                case ByteCodeOp::GetFromBssSeg8:
                case ByteCodeOp::GetFromBssSeg16:
                case ByteCodeOp::GetFromBssSeg32:
                case ByteCodeOp::GetFromBssSeg64:
                case ByteCodeOp::GetFromCompilerSeg8:
                case ByteCodeOp::GetFromCompilerSeg16:
                case ByteCodeOp::GetFromCompilerSeg32:
                case ByteCodeOp::GetFromCompilerSeg64:
                case ByteCodeOp::CopySP:
                case ByteCodeOp::CopyRTtoRA:
                case ByteCodeOp::SaveRRtoRA:
                case ByteCodeOp::CopyRRtoRA:
                case ByteCodeOp::CopySPVaargs:
                case ByteCodeOp::MakeLambda:
                case ByteCodeOp::CopyRBAddrToRA:
                case ByteCodeOp::IntrinsicStrLen:
                case ByteCodeOp::IntrinsicStrCmp:
                case ByteCodeOp::IntrinsicMemCmp:
                case ByteCodeOp::IntrinsicMakeCallback:
                case ByteCodeOp::CloneString:
                case ByteCodeOp::IntrinsicIsConstExprSI:
                case ByteCodeOp::IntrinsicLocationSI:
                case ByteCodeOp::IntrinsicAlloc:
                case ByteCodeOp::IntrinsicRealloc:
                case ByteCodeOp::IntrinsicSysAlloc:
                case ByteCodeOp::IntrinsicDbgAlloc:
                case ByteCodeOp::IntrinsicRtFlags:
                case ByteCodeOp::IntrinsicIsByteCode:
                case ByteCodeOp::JumpIfError:
                case ByteCodeOp::JumpIfNoError:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->kind = ValueKind::Unknown;
                    break;

                case ByteCodeOp::CopyRAtoRT:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    if (ra->kind == ValueKind::StackAddr)
                        invalidateCurStateStack(cxt);
                    break;

                case ByteCodeOp::IntrinsicCVaStart:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, 8));
                        setStackValue(cxt, addr, 8, ValueKind::Unknown);
                    }
                    break;

                case ByteCodeOp::IntrinsicCVaArg:
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    rb->kind = ValueKind::Unknown;
                    break;

                case ByteCodeOp::IntrinsicAtomicAddS8:
                    ATOM_EQ(int8_t, +=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicAddS16:
                    ATOM_EQ(int16_t, +=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicAddS32:
                    ATOM_EQ(int32_t, +=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicAddS64:
                    ATOM_EQ(int64_t, +=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicAndS8:
                    ATOM_EQ(int8_t, &=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicAndS16:
                    ATOM_EQ(int16_t, &=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicAndS32:
                    ATOM_EQ(int32_t, &=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicAndS64:
                    ATOM_EQ(int64_t, &=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicOrS8:
                    ATOM_EQ(int8_t, |=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicOrS16:
                    ATOM_EQ(int16_t, |=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicOrS32:
                    ATOM_EQ(int32_t, |=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicOrS64:
                    ATOM_EQ(int64_t, |=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicXorS8:
                    ATOM_EQ(int8_t, ^=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicXorS16:
                    ATOM_EQ(int16_t, ^=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicXorS32:
                    ATOM_EQ(int32_t, ^=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicXorS64:
                    ATOM_EQ(int64_t, ^=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicXchgS8:
                    ATOM_EQ(int8_t, =, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicXchgS16:
                    ATOM_EQ(int16_t, =, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicXchgS32:
                    ATOM_EQ(int32_t, =, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicXchgS64:
                    ATOM_EQ(int64_t, =, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
                    ATOM_EQ_XCHG(int8_t, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
                    ATOM_EQ_XCHG(int16_t, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
                    ATOM_EQ_XCHG(int32_t, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
                    ATOM_EQ_XCHG(int64_t, s64);
                    break;

                case ByteCodeOp::IntrinsicItfTableOf:
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    rc->kind = ValueKind::Unknown;
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
                case ByteCodeOp::CopyRT2toRARB:
                case ByteCodeOp::IntrinsicArguments:
                case ByteCodeOp::IntrinsicCompiler:
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
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = ValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                    }
                    break;
                case ByteCodeOp::JumpIfTrue:
                    JUMP1(va.reg.b);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = ValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 1;
                    }
                    break;

                case ByteCodeOp::JumpIfEqual64:
                    JUMP2(va.reg.u64 == vc.reg.u64);
                    if (jmpAddState && vc.kind == ValueKind::Constant)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = ValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = vc.reg.u64;
                    }
                    break;

                case ByteCodeOp::JumpIfZero8:
                    JUMP1(va.reg.u8 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = ValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                    }
                    break;
                case ByteCodeOp::JumpIfZero16:
                    JUMP1(va.reg.u16 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = ValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                    }
                    break;
                case ByteCodeOp::JumpIfZero32:
                    JUMP1(va.reg.u32 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = ValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                    }
                    break;
                case ByteCodeOp::JumpIfZero64:
                    JUMP1(va.reg.u64 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = ValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                    }
                    break;

                case ByteCodeOp::JumpIfLowerEqS64:
                    JUMP2(va.reg.s64 <= vc.reg.s64);
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

                case ByteCodeOp::CompareOp3Way8:
                    SWAG_CHECK(getImmediateA(va, cxt, ip));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (rc->kind == ValueKind::Constant)
                    {
                        auto sub    = va.reg.u8 - vb.reg.u8;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3Way16:
                    SWAG_CHECK(getImmediateA(va, cxt, ip));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (rc->kind == ValueKind::Constant)
                    {
                        auto sub    = va.reg.u16 - vb.reg.u16;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3Way32:
                    SWAG_CHECK(getImmediateA(va, cxt, ip));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (rc->kind == ValueKind::Constant)
                    {
                        auto sub    = va.reg.u32 - vb.reg.u32;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3Way64:
                    SWAG_CHECK(getImmediateA(va, cxt, ip));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (rc->kind == ValueKind::Constant)
                    {
                        auto sub    = va.reg.u64 - vb.reg.u64;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3WayF32:
                    SWAG_CHECK(getImmediateA(va, cxt, ip));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (rc->kind == ValueKind::Constant)
                    {
                        auto sub    = va.reg.f32 - vb.reg.f32;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3WayF64:
                    SWAG_CHECK(getImmediateA(va, cxt, ip));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    rc->kind = va.kind == ValueKind::Constant && vb.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (rc->kind == ValueKind::Constant)
                    {
                        auto sub    = va.reg.f64 - vb.reg.f64;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;

                case ByteCodeOp::ZeroToTrue:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->reg.b = ra->reg.s32 == 0;
                    break;
                case ByteCodeOp::GreaterEqZeroToTrue:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->reg.b = ra->reg.s32 >= 0;
                    break;
                case ByteCodeOp::GreaterZeroToTrue:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->reg.b = ra->reg.s32 > 0;
                    break;
                case ByteCodeOp::LowerEqZeroToTrue:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->reg.b = ra->reg.s32 <= 0;
                    break;
                case ByteCodeOp::LowerZeroToTrue:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->reg.b = ra->reg.s32 < 0;
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
                    ra->overload = reinterpret_cast<SymbolOverload*>(ip->c.pointer);
                    break;

                case ByteCodeOp::SetZeroStackX:
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32, ip->b.u32));
                    memset(addr, 0, ip->b.u32);
                    setStackValue(cxt, addr, ip->b.u32, ValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack8:
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32, 1));
                    *addr = 0;
                    setStackValue(cxt, addr, 1, ValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack16:
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32, 2));
                    *reinterpret_cast<uint16_t*>(addr) = 0;
                    setStackValue(cxt, addr, 2, ValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack32:
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32, 4));
                    *reinterpret_cast<uint32_t*>(addr) = 0;
                    setStackValue(cxt, addr, 4, ValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack64:
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32, 8));
                    *reinterpret_cast<uint64_t*>(addr) = 0;
                    setStackValue(cxt, addr, 8, ValueKind::Constant);
                    break;

                case ByteCodeOp::SetAtStackPointer64:
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32, 8));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                    setStackValue(cxt, addr, 8, vb.kind);
                    break;

                case ByteCodeOp::SetZeroAtPointerXRB:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_ASSERT(rb->reg.u64 * ip->c.u64 <= UINT32_MAX);
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64)));
                        memset(addr, 0, rb->reg.u64 * ip->c.u64);
                        setStackValue(cxt, addr, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64), ValueKind::Constant);
                    }
                    break;

                case ByteCodeOp::SetZeroAtPointerX:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_ASSERT(ip->b.u64 <= UINT32_MAX);
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32, static_cast<uint32_t>(ip->b.u64)));
                        memset(addr, 0, ip->b.u64);
                        setStackValue(cxt, addr, static_cast<uint32_t>(ip->b.u64), ValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer8:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32, 1));
                        *addr = 0;
                        setStackValue(cxt, addr, 1, ValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer16:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32, 2));
                        *reinterpret_cast<uint16_t*>(addr) = 0;
                        setStackValue(cxt, addr, 2, ValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32, 4));
                        *reinterpret_cast<uint32_t*>(addr) = 0;
                        setStackValue(cxt, addr, 4, ValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->b.u32, 8));
                        *reinterpret_cast<uint64_t*>(addr) = 0;
                        setStackValue(cxt, addr, 8, ValueKind::Constant);
                    }
                    break;

                case ByteCodeOp::SetAtPointer8:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32, 1));
                        SWAG_CHECK(getImmediateB(vb, cxt, ip));
                        *addr = vb.reg.u8;
                        setStackValue(cxt, addr, 1, vb.kind);
                    }
                    break;
                case ByteCodeOp::SetAtPointer16:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32, 2));
                        SWAG_CHECK(getImmediateB(vb, cxt, ip));
                        *reinterpret_cast<uint16_t*>(addr) = vb.reg.u16;
                        setStackValue(cxt, addr, 2, vb.kind);
                    }
                    break;
                case ByteCodeOp::SetAtPointer32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32, 4));
                        SWAG_CHECK(getImmediateB(vb, cxt, ip));
                        *reinterpret_cast<uint32_t*>(addr) = vb.reg.u32;
                        setStackValue(cxt, addr, 4, vb.kind);
                    }
                    break;
                case ByteCodeOp::SetAtPointer64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32, 8));
                        SWAG_CHECK(getImmediateB(vb, cxt, ip));
                        *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                        setStackValue(cxt, addr, 8, vb.kind);
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
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32, 1));
                    SWAG_CHECK(getStackValue(*ra, cxt, addr, 1));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 1, ra->overload));
                    ra->reg.u64 = *addr;
                    setConstant(cxt, ra->kind, ip, *addr, ConstantKind::SetImmediateA);
                    break;
                case ByteCodeOp::GetFromStack16:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32, 2));
                    SWAG_CHECK(getStackValue(*ra, cxt, addr, 2));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 2, ra->overload));
                    ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                    setConstant(cxt, ra->kind, ip, *reinterpret_cast<uint16_t*>(addr), ConstantKind::SetImmediateA);
                    break;
                case ByteCodeOp::GetFromStack32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32, 4));
                    SWAG_CHECK(getStackValue(*ra, cxt, addr, 4));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 4, ra->overload));
                    ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                    setConstant(cxt, ra->kind, ip, *reinterpret_cast<uint32_t*>(addr), ConstantKind::SetImmediateA);
                    break;
                case ByteCodeOp::GetFromStack64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32, 8));
                    SWAG_CHECK(getStackValue(*ra, cxt, addr, 8));
                    SWAG_CHECK(checkStackInitialized(cxt, addr, 8, ra->overload));
                    ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                    setConstant(cxt, ra->kind, ip, *reinterpret_cast<uint64_t*>(addr), ConstantKind::SetImmediateA);
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
                case ByteCodeOp::IncMulPointer64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
                    *rc = *ra;
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    if (vb.kind == ValueKind::Unknown)
                        rc->kind = ValueKind::Unknown;
                    else
                        rc->reg.u64 += vb.reg.s64 * ip->d.u64;
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

                case ByteCodeOp::CopyRAtoRR:
                    if (ip->hasFlag(BCI_IMM_A))
                        break;
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    if (ra->kind == ValueKind::StackAddr)
                        return checkEscapeFrame(cxt, ra->reg.u32, ra->overload);
                    break;

                case ByteCodeOp::CopyRARBtoRR2:
                    if (ip->hasFlag(BCI_IMM_A))
                        break;
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));

                    if (ra->kind == ValueKind::StackAddr)
                    {
                        // Legit in #run block, as we will make a copy
                        if (context->bc->node && context->bc->node->hasAstFlag(AST_IN_RUN_BLOCK))
                            break;

                        return checkEscapeFrame(cxt, ra->reg.u32, ra->overload);
                    }

                    break;

                case ByteCodeOp::DeRef8:
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    SWAG_CHECK(checkNotNull(cxt, rb));
                    if (rb->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, 1));
                        SWAG_CHECK(checkStackInitialized(cxt, addr, 1));
                        SWAG_CHECK(getStackValue(*ra, cxt, addr, 1));
                        ra->reg.u64 = *addr;
                        break;
                    }
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->kind = ValueKind::Unknown;
                    break;
                case ByteCodeOp::DeRef16:
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    SWAG_CHECK(checkNotNull(cxt, rb));
                    if (rb->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, 2));
                        SWAG_CHECK(checkStackInitialized(cxt, addr, 2));
                        SWAG_CHECK(getStackValue(*ra, cxt, addr, 2));
                        ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                        break;
                    }
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->kind = ValueKind::Unknown;
                    break;
                case ByteCodeOp::DeRef32:
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    SWAG_CHECK(checkNotNull(cxt, rb));
                    if (rb->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, 4));
                        SWAG_CHECK(checkStackInitialized(cxt, addr, 4));
                        SWAG_CHECK(getStackValue(*ra, cxt, addr, 4));
                        ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                        break;
                    }
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->kind = ValueKind::Unknown;
                    break;
                case ByteCodeOp::DeRef64:
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    SWAG_CHECK(checkNotNull(cxt, rb));
                    if (rb->kind == ValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64, 8));
                        SWAG_CHECK(checkStackInitialized(cxt, addr, 8));
                        SWAG_CHECK(getStackValue(*ra, cxt, addr, 8));
                        ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                        if (ip->d.pointer)
                            ra->overload = reinterpret_cast<SymbolOverload*>(ip->d.pointer);
                        break;
                    }
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    ra->kind     = ValueKind::Unknown;
                    ra->overload = nullptr;
                    break;

                case ByteCodeOp::AffectOpPlusEqS8:
                    BINOP_EQ_OVF(int8_t, +=, s8, addWillOverflow, "+=", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::AffectOpPlusEqS16:
                    BINOP_EQ_OVF(int16_t, +=, s16, addWillOverflow, "+=", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::AffectOpPlusEqS32:
                    BINOP_EQ_OVF(int32_t, +=, s32, addWillOverflow, "+=", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::AffectOpPlusEqS64:
                    BINOP_EQ_OVF(int64_t, +=, s64, addWillOverflow, "+=", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::AffectOpPlusEqU8:
                    BINOP_EQ_OVF(uint8_t, +=, u8, addWillOverflow, "+=", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::AffectOpPlusEqU16:
                    BINOP_EQ_OVF(uint16_t, +=, u16, addWillOverflow, "+=", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::AffectOpPlusEqU32:
                    BINOP_EQ_OVF(uint32_t, +=, u32, addWillOverflow, "+=", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::AffectOpPlusEqU64:
                    BINOP_EQ_OVF(uint64_t, +=, u64, addWillOverflow, "+=", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::AffectOpPlusEqF32:
                    BINOP_EQ(float, +=, f32);
                    break;
                case ByteCodeOp::AffectOpPlusEqF64:
                    BINOP_EQ(double, +=, f64);
                    break;

                case ByteCodeOp::AffectOpMinusEqS8:
                    BINOP_EQ_OVF(int8_t, -=, s8, subWillOverflow, "-=", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::AffectOpMinusEqS16:
                    BINOP_EQ_OVF(int16_t, -=, s16, subWillOverflow, "-=", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::AffectOpMinusEqS32:
                    BINOP_EQ_OVF(int32_t, -=, s32, subWillOverflow, "-=", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::AffectOpMinusEqS64:
                    BINOP_EQ_OVF(int64_t, -=, s64, subWillOverflow, "-=", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::AffectOpMinusEqU8:
                    BINOP_EQ_OVF(uint8_t, -=, u8, subWillOverflow, "-=", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::AffectOpMinusEqU16:
                    BINOP_EQ_OVF(uint16_t, -=, u16, subWillOverflow, "-=", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::AffectOpMinusEqU32:
                    BINOP_EQ_OVF(uint32_t, -=, u32, subWillOverflow, "-=", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::AffectOpMinusEqU64:
                    BINOP_EQ_OVF(uint64_t, -=, u64, subWillOverflow, "-=", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::AffectOpMinusEqF32:
                    BINOP_EQ(float, -=, f32);
                    break;
                case ByteCodeOp::AffectOpMinusEqF64:
                    BINOP_EQ(double, -=, f64);
                    break;

                case ByteCodeOp::AffectOpMulEqS8:
                    BINOP_EQ_OVF(int8_t, *=, s8, mulWillOverflow, "*=", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::AffectOpMulEqS16:
                    BINOP_EQ_OVF(int16_t, *=, s16, mulWillOverflow, "*=", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::AffectOpMulEqS32:
                    BINOP_EQ_OVF(int32_t, *=, s32, mulWillOverflow, "*=", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::AffectOpMulEqS64:
                    BINOP_EQ_OVF(int64_t, *=, s64, mulWillOverflow, "*=", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::AffectOpMulEqU8:
                    BINOP_EQ_OVF(uint8_t, *=, u8, mulWillOverflow, "*=", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::AffectOpMulEqU16:
                    BINOP_EQ_OVF(uint16_t, *=, u16, mulWillOverflow, "*=", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::AffectOpMulEqU32:
                    BINOP_EQ_OVF(uint32_t, *=, u32, mulWillOverflow, "*=", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::AffectOpMulEqU64:
                    BINOP_EQ_OVF(uint64_t, *=, u64, mulWillOverflow, "*=", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::AffectOpMulEqF32:
                    BINOP_EQ(float, *=, f32);
                    break;
                case ByteCodeOp::AffectOpMulEqF64:
                    BINOP_EQ(double, *=, f64);
                    break;

                case ByteCodeOp::AffectOpDivEqS8:
                    BINOP_EQ_DIV(int8_t, /=, s8);
                    break;
                case ByteCodeOp::AffectOpDivEqS16:
                    BINOP_EQ_DIV(int16_t, /=, s16);
                    break;
                case ByteCodeOp::AffectOpDivEqS32:
                    BINOP_EQ_DIV(int32_t, /=, s32);
                    break;
                case ByteCodeOp::AffectOpDivEqS64:
                    BINOP_EQ_DIV(int64_t, /=, s64);
                    break;
                case ByteCodeOp::AffectOpDivEqU8:
                    BINOP_EQ_DIV(uint8_t, /=, u8);
                    break;
                case ByteCodeOp::AffectOpDivEqU16:
                    BINOP_EQ_DIV(uint16_t, /=, u16);
                    break;
                case ByteCodeOp::AffectOpDivEqU32:
                    BINOP_EQ_DIV(uint32_t, /=, u32);
                    break;
                case ByteCodeOp::AffectOpDivEqU64:
                    BINOP_EQ_DIV(uint64_t, /=, u64);
                    break;
                case ByteCodeOp::AffectOpDivEqF32:
                    BINOP_EQ_DIV(float, /=, f32);
                    break;
                case ByteCodeOp::AffectOpDivEqF64:
                    BINOP_EQ_DIV(double, /=, f64);
                    break;

                case ByteCodeOp::AffectOpModuloEqS8:
                    BINOP_EQ_DIV(int8_t, %=, s8);
                    break;
                case ByteCodeOp::AffectOpModuloEqS16:
                    BINOP_EQ_DIV(int16_t, %=, s16);
                    break;
                case ByteCodeOp::AffectOpModuloEqS32:
                    BINOP_EQ_DIV(int32_t, %=, s32);
                    break;
                case ByteCodeOp::AffectOpModuloEqS64:
                    BINOP_EQ_DIV(int64_t, %=, s64);
                    break;
                case ByteCodeOp::AffectOpModuloEqU8:
                    BINOP_EQ_DIV(uint8_t, %=, u8);
                    break;
                case ByteCodeOp::AffectOpModuloEqU16:
                    BINOP_EQ_DIV(uint16_t, %=, u16);
                    break;
                case ByteCodeOp::AffectOpModuloEqU32:
                    BINOP_EQ_DIV(uint32_t, %=, u32);
                    break;
                case ByteCodeOp::AffectOpModuloEqU64:
                    BINOP_EQ_DIV(uint64_t, %=, u64);
                    break;

                case ByteCodeOp::AffectOpAndEqU8:
                    BINOP_EQ(uint8_t, &=, u8);
                    break;
                case ByteCodeOp::AffectOpAndEqU16:
                    BINOP_EQ(uint16_t, &=, u16);
                    break;
                case ByteCodeOp::AffectOpAndEqU32:
                    BINOP_EQ(uint32_t, &=, u32);
                    break;
                case ByteCodeOp::AffectOpAndEqU64:
                    BINOP_EQ(uint64_t, &=, u64);
                    break;

                case ByteCodeOp::AffectOpOrEqU8:
                    BINOP_EQ(uint8_t, |=, u8);
                    break;
                case ByteCodeOp::AffectOpOrEqU16:
                    BINOP_EQ(uint16_t, |=, u16);
                    break;
                case ByteCodeOp::AffectOpOrEqU32:
                    BINOP_EQ(uint32_t, |=, u32);
                    break;
                case ByteCodeOp::AffectOpOrEqU64:
                    BINOP_EQ(uint64_t, |=, u64);
                    break;

                case ByteCodeOp::AffectOpXorEqU8:
                    BINOP_EQ(uint8_t, ^=, u8);
                    break;
                case ByteCodeOp::AffectOpXorEqU16:
                    BINOP_EQ(uint16_t, ^=, u16);
                    break;
                case ByteCodeOp::AffectOpXorEqU32:
                    BINOP_EQ(uint32_t, ^=, u32);
                    break;
                case ByteCodeOp::AffectOpXorEqU64:
                    BINOP_EQ(uint64_t, ^=, u64);
                    break;

                case ByteCodeOp::AffectOpShiftLeftEqS8:
                    BINOP_EQ_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqS16:
                    BINOP_EQ_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqS32:
                    BINOP_EQ_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqS64:
                    BINOP_EQ_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU8:
                    BINOP_EQ_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU16:
                    BINOP_EQ_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU32:
                    BINOP_EQ_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU64:
                    BINOP_EQ_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, false);
                    break;

                case ByteCodeOp::AffectOpShiftRightEqS8:
                    BINOP_EQ_SHIFT(int8_t, s8, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqS16:
                    BINOP_EQ_SHIFT(int16_t, s16, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqS32:
                    BINOP_EQ_SHIFT(int32_t, s32, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqS64:
                    BINOP_EQ_SHIFT(int64_t, s64, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU8:
                    BINOP_EQ_SHIFT(uint8_t, u8, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU16:
                    BINOP_EQ_SHIFT(uint16_t, u16, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU32:
                    BINOP_EQ_SHIFT(uint32_t, u32, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU64:
                    BINOP_EQ_SHIFT(uint64_t, u64, ByteCodeRun::executeRightShift, false);
                    break;

                case ByteCodeOp::NegBool:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind  = rb->kind;
                    ra->reg.b = rb->reg.b ^ 1;
                    break;
                case ByteCodeOp::NegS32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.s32 = -rb->reg.s32;
                    break;
                case ByteCodeOp::NegS64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.s64 = -rb->reg.s64;
                    break;
                case ByteCodeOp::NegF32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.f32 = -rb->reg.f32;
                    break;
                case ByteCodeOp::NegF64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.f64 = -rb->reg.f64;
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
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s16 = static_cast<int16_t>(vb.reg.s8);
                    break;

                case ByteCodeOp::CastS8S32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s32 = static_cast<int32_t>(vb.reg.s8);
                    break;
                case ByteCodeOp::CastS16S32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s32 = static_cast<int32_t>(vb.reg.s16);
                    break;
                case ByteCodeOp::CastF32S32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s32 = static_cast<int32_t>(vb.reg.f32);
                    break;

                case ByteCodeOp::CastS8S64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.s8);
                    break;
                case ByteCodeOp::CastS16S64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.s16);
                    break;
                case ByteCodeOp::CastS32S64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.s32);
                    break;
                case ByteCodeOp::CastF64S64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.f64);
                    break;

                case ByteCodeOp::CastS8F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s8);
                    break;
                case ByteCodeOp::CastS16F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s16);
                    break;
                case ByteCodeOp::CastS32F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s32);
                    break;
                case ByteCodeOp::CastS64F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s64);
                    break;
                case ByteCodeOp::CastU8F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u8);
                    break;
                case ByteCodeOp::CastU16F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u16);
                    break;
                case ByteCodeOp::CastU32F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u32);
                    break;
                case ByteCodeOp::CastU64F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u64);
                    break;

                case ByteCodeOp::CastS8F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s8);
                    break;
                case ByteCodeOp::CastS16F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s16);
                    break;
                case ByteCodeOp::CastS32F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s32);
                    break;
                case ByteCodeOp::CastS64F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s64);
                    break;
                case ByteCodeOp::CastU8F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u8);
                    break;
                case ByteCodeOp::CastU16F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u16);
                    break;
                case ByteCodeOp::CastU32F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u32);
                    break;
                case ByteCodeOp::CastU64F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u64);
                    break;
                case ByteCodeOp::CastF32F64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.f32);
                    break;

                case ByteCodeOp::CastF64F32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.f64);
                    break;

                case ByteCodeOp::CompareOpEqual8:
                    CMP_OP(==, u8);
                    break;
                case ByteCodeOp::CompareOpEqual16:
                    CMP_OP(==, u16);
                    break;
                case ByteCodeOp::CompareOpEqual32:
                    CMP_OP(==, u32);
                    break;
                case ByteCodeOp::CompareOpEqual64:
                    CMP_OP(==, u64);
                    break;
                case ByteCodeOp::CompareOpEqualF32:
                    CMP_OP(==, f32);
                    break;
                case ByteCodeOp::CompareOpEqualF64:
                    CMP_OP(==, f64);
                    break;

                case ByteCodeOp::CompareOpNotEqual8:
                    CMP_OP(!=, u8);
                    break;
                case ByteCodeOp::CompareOpNotEqual16:
                    CMP_OP(!=, u16);
                    break;
                case ByteCodeOp::CompareOpNotEqual32:
                    CMP_OP(!=, u32);
                    break;
                case ByteCodeOp::CompareOpNotEqual64:
                    CMP_OP(!=, u64);
                    break;
                case ByteCodeOp::CompareOpNotEqualF32:
                    CMP_OP(!=, f32);
                    break;
                case ByteCodeOp::CompareOpNotEqualF64:
                    CMP_OP(!=, f64);
                    break;

                case ByteCodeOp::CompareOpGreaterEqS8:
                    CMP_OP(>=, s8);
                    break;
                case ByteCodeOp::CompareOpGreaterEqS16:
                    CMP_OP(>=, s16);
                    break;
                case ByteCodeOp::CompareOpGreaterEqS32:
                    CMP_OP(>=, s32);
                    break;
                case ByteCodeOp::CompareOpGreaterEqS64:
                    CMP_OP(>=, s64);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU8:
                    CMP_OP(>=, u8);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU16:
                    CMP_OP(>=, u16);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU32:
                    CMP_OP(>=, u32);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU64:
                    CMP_OP(>=, u64);
                    break;
                case ByteCodeOp::CompareOpGreaterEqF32:
                    CMP_OP(>=, f32);
                    break;
                case ByteCodeOp::CompareOpGreaterEqF64:
                    CMP_OP(>=, f64);
                    break;

                case ByteCodeOp::CompareOpGreaterS8:
                    CMP_OP(>, s8);
                    break;
                case ByteCodeOp::CompareOpGreaterS16:
                    CMP_OP(>, s16);
                    break;
                case ByteCodeOp::CompareOpGreaterS32:
                    CMP_OP(>, s32);
                    break;
                case ByteCodeOp::CompareOpGreaterS64:
                    CMP_OP(>, s64);
                    break;
                case ByteCodeOp::CompareOpGreaterU8:
                    CMP_OP(>, u8);
                    break;
                case ByteCodeOp::CompareOpGreaterU16:
                    CMP_OP(>, u16);
                    break;
                case ByteCodeOp::CompareOpGreaterU32:
                    CMP_OP(>, u32);
                    break;
                case ByteCodeOp::CompareOpGreaterU64:
                    CMP_OP(>, u64);
                    break;
                case ByteCodeOp::CompareOpGreaterF32:
                    CMP_OP(>, f32);
                    break;
                case ByteCodeOp::CompareOpGreaterF64:
                    CMP_OP(>, f64);
                    break;

                case ByteCodeOp::CompareOpLowerEqS8:
                    CMP_OP(<=, s8);
                    break;
                case ByteCodeOp::CompareOpLowerEqS16:
                    CMP_OP(<=, s16);
                    break;
                case ByteCodeOp::CompareOpLowerEqS32:
                    CMP_OP(<=, s32);
                    break;
                case ByteCodeOp::CompareOpLowerEqS64:
                    CMP_OP(<=, s64);
                    break;
                case ByteCodeOp::CompareOpLowerEqU8:
                    CMP_OP(<=, u8);
                    break;
                case ByteCodeOp::CompareOpLowerEqU16:
                    CMP_OP(<=, u16);
                    break;
                case ByteCodeOp::CompareOpLowerEqU32:
                    CMP_OP(<=, u32);
                    break;
                case ByteCodeOp::CompareOpLowerEqU64:
                    CMP_OP(<=, u64);
                    break;
                case ByteCodeOp::CompareOpLowerEqF32:
                    CMP_OP(<=, f32);
                    break;
                case ByteCodeOp::CompareOpLowerEqF64:
                    CMP_OP(<=, f64);
                    break;

                case ByteCodeOp::CompareOpLowerS8:
                    CMP_OP(<, s8);
                    break;
                case ByteCodeOp::CompareOpLowerS16:
                    CMP_OP(<, s16);
                    break;
                case ByteCodeOp::CompareOpLowerS32:
                    CMP_OP(<, s32);
                    break;
                case ByteCodeOp::CompareOpLowerS64:
                    CMP_OP(<, s64);
                    break;
                case ByteCodeOp::CompareOpLowerU8:
                    CMP_OP(<, u8);
                    break;
                case ByteCodeOp::CompareOpLowerU16:
                    CMP_OP(<, u16);
                    break;
                case ByteCodeOp::CompareOpLowerU32:
                    CMP_OP(<, u32);
                    break;
                case ByteCodeOp::CompareOpLowerU64:
                    CMP_OP(<, u64);
                    break;
                case ByteCodeOp::CompareOpLowerF32:
                    CMP_OP(<, f32);
                    break;
                case ByteCodeOp::CompareOpLowerF64:
                    CMP_OP(<, f64);
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

                case ByteCodeOp::BinOpPlusS8:
                    BINOP_OVF(+, s8, addWillOverflow, "+", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::BinOpPlusS16:
                    BINOP_OVF(+, s16, addWillOverflow, "+", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::BinOpPlusS32:
                    BINOP_OVF(+, s32, addWillOverflow, "+", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::BinOpPlusS64:
                    BINOP_OVF(+, s64, addWillOverflow, "+", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::BinOpPlusU8:
                    BINOP_OVF(+, u8, addWillOverflow, "+", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::BinOpPlusU16:
                    BINOP_OVF(+, u16, addWillOverflow, "+", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::BinOpPlusU32:
                    BINOP_OVF(+, u32, addWillOverflow, "+", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::BinOpPlusU64:
                    BINOP_OVF(+, u64, addWillOverflow, "+", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::BinOpPlusF32:
                    BINOP(+, f32);
                    break;
                case ByteCodeOp::BinOpPlusF64:
                    BINOP(+, f64);
                    break;

                case ByteCodeOp::BinOpMinusS8:
                    BINOP_OVF(-, s8, subWillOverflow, "-", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::BinOpMinusS16:
                    BINOP_OVF(-, s16, subWillOverflow, "-", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::BinOpMinusS32:
                    BINOP_OVF(-, s32, subWillOverflow, "-", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::BinOpMinusS64:
                    BINOP_OVF(-, s64, subWillOverflow, "-", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::BinOpMinusU8:
                    BINOP_OVF(-, u8, subWillOverflow, "-", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::BinOpMinusU16:
                    BINOP_OVF(-, u16, subWillOverflow, "-", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::BinOpMinusU32:
                    BINOP_OVF(-, u32, subWillOverflow, "-", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::BinOpMinusU64:
                    BINOP_OVF(-, u64, subWillOverflow, "-", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::BinOpMinusF32:
                    BINOP(-, f32);
                    break;
                case ByteCodeOp::BinOpMinusF64:
                    BINOP(-, f64);
                    break;

                case ByteCodeOp::BinOpMulS8:
                    BINOP_OVF(*, s8, mulWillOverflow, "*", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::BinOpMulS16:
                    BINOP_OVF(*, s16, mulWillOverflow, "*", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::BinOpMulS32:
                    BINOP_OVF(*, s32, mulWillOverflow, "*", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::BinOpMulS64:
                    BINOP_OVF(*, s64, mulWillOverflow, "*", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::BinOpMulU8:
                    BINOP_OVF(*, u8, mulWillOverflow, "*", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::BinOpMulU16:
                    BINOP_OVF(*, u16, mulWillOverflow, "*", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::BinOpMulU32:
                    BINOP_OVF(*, u32, mulWillOverflow, "*", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::BinOpMulU64:
                    BINOP_OVF(*, u64, mulWillOverflow, "*", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::BinOpMulF32:
                    BINOP(*, f32);
                    break;
                case ByteCodeOp::BinOpMulF64:
                    BINOP(*, f64);
                    break;

                case ByteCodeOp::BinOpDivS8:
                    BINOP_DIV(/, s8);
                    break;
                case ByteCodeOp::BinOpDivS16:
                    BINOP_DIV(/, s16);
                    break;
                case ByteCodeOp::BinOpDivS32:
                    BINOP_DIV(/, s32);
                    break;
                case ByteCodeOp::BinOpDivS64:
                    BINOP_DIV(/, s64);
                    break;
                case ByteCodeOp::BinOpDivU8:
                    BINOP_DIV(/, u8);
                    break;
                case ByteCodeOp::BinOpDivU16:
                    BINOP_DIV(/, u16);
                    break;
                case ByteCodeOp::BinOpDivU32:
                    BINOP_DIV(/, u32);
                    break;
                case ByteCodeOp::BinOpDivU64:
                    BINOP_DIV(/, u64);
                    break;
                case ByteCodeOp::BinOpDivF32:
                    BINOP_DIV(/, f32);
                    break;
                case ByteCodeOp::BinOpDivF64:
                    BINOP_DIV(/, f64);
                    break;

                case ByteCodeOp::BinOpModuloS8:
                    BINOP_DIV(%, s8);
                    break;
                case ByteCodeOp::BinOpModuloS16:
                    BINOP_DIV(%, s16);
                    break;
                case ByteCodeOp::BinOpModuloS32:
                    BINOP_DIV(%, s32);
                    break;
                case ByteCodeOp::BinOpModuloS64:
                    BINOP_DIV(%, s64);
                    break;
                case ByteCodeOp::BinOpModuloU8:
                    BINOP_DIV(%, u8);
                    break;
                case ByteCodeOp::BinOpModuloU16:
                    BINOP_DIV(%, u16);
                    break;
                case ByteCodeOp::BinOpModuloU32:
                    BINOP_DIV(%, u32);
                    break;
                case ByteCodeOp::BinOpModuloU64:
                    BINOP_DIV(%, u64);
                    break;

                case ByteCodeOp::BinOpShiftLeftS8:
                    BINOP_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftS16:
                    BINOP_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftS32:
                    BINOP_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftS64:
                    BINOP_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftU8:
                    BINOP_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::BinOpShiftLeftU16:
                    BINOP_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::BinOpShiftLeftU32:
                    BINOP_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::BinOpShiftLeftU64:
                    BINOP_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, false);
                    break;

                case ByteCodeOp::BinOpShiftRightS8:
                    BINOP_SHIFT(int8_t, s8, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightS16:
                    BINOP_SHIFT(int16_t, s16, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightS32:
                    BINOP_SHIFT(int32_t, s32, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightS64:
                    BINOP_SHIFT(int64_t, s64, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightU8:
                    BINOP_SHIFT(uint8_t, u8, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::BinOpShiftRightU16:
                    BINOP_SHIFT(uint16_t, u16, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::BinOpShiftRightU32:
                    BINOP_SHIFT(uint32_t, u32, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::BinOpShiftRightU64:
                    BINOP_SHIFT(uint64_t, u64, ByteCodeRun::executeRightShift, false);
                    break;

                case ByteCodeOp::InvertU8:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind   = rb->kind;
                    ra->reg.u8 = ~rb->reg.u8;
                    break;
                case ByteCodeOp::InvertU16:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.u16 = ~rb->reg.u16;
                    break;
                case ByteCodeOp::InvertU32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.u32 = ~rb->reg.u32;
                    break;
                case ByteCodeOp::InvertU64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.u64 = ~rb->reg.u64;
                    break;

                case ByteCodeOp::MemCpy8:
                    MEMCPY(uint8_t, 1);
                    break;
                case ByteCodeOp::MemCpy16:
                    MEMCPY(uint16_t, 2);
                    break;
                case ByteCodeOp::MemCpy32:
                    MEMCPY(uint32_t, 4);
                    break;
                case ByteCodeOp::MemCpy64:
                    MEMCPY(uint64_t, 8);
                    break;
                case ByteCodeOp::IntrinsicMemCpy:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    SWAG_CHECK(getImmediateC(vc, cxt, ip));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    SWAG_CHECK(checkNotNull(cxt, rb));
                    if (ra->kind == ValueKind::StackAddr && rb->kind == ValueKind::StackAddr && vc.kind == ValueKind::Constant)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, vc.reg.u32));
                        SWAG_CHECK(getStackAddress(addr2, cxt, rb->reg.u32, vc.reg.u32));
                        SWAG_CHECK(checkStackInitialized(cxt, addr2, vc.reg.u32, rb->overload));
                        SWAG_CHECK(getStackValue(va, cxt, addr2, vc.reg.u32));
                        setStackValue(cxt, addr, vc.reg.u32, va.kind);
                        memcpy(addr, addr2, vc.reg.u32);
                        break;
                    }
                    invalidateCurStateStack(cxt);
                    break;
                case ByteCodeOp::IntrinsicMemMove:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
                    SWAG_CHECK(getImmediateC(vc, cxt, ip));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    SWAG_CHECK(checkNotNull(cxt, rb));
                    if (ra->kind == ValueKind::StackAddr && rb->kind == ValueKind::StackAddr && vc.kind == ValueKind::Constant)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, vc.reg.u32));
                        SWAG_CHECK(getStackAddress(addr2, cxt, rb->reg.u32, vc.reg.u32));
                        SWAG_CHECK(checkStackInitialized(cxt, addr2, vc.reg.u32, rb->overload));
                        SWAG_CHECK(getStackValue(va, cxt, addr2, vc.reg.u32));
                        setStackValue(cxt, addr, vc.reg.u32, va.kind);
                        memmove(addr, addr2, vc.reg.u32);
                        break;
                    }
                    invalidateCurStateStack(cxt);
                    break;
                case ByteCodeOp::IntrinsicMemSet:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getImmediateC(vc, cxt, ip));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    if (ra->kind == ValueKind::StackAddr && vb.kind == ValueKind::Constant && vc.kind == ValueKind::Constant)
                    {
                        SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32, vc.reg.u32));
                        setStackValue(cxt, addr, vc.reg.u32, ValueKind::Constant);
                        memset(addr, vb.reg.u8, vc.reg.u32);
                        break;
                    }
                    invalidateCurStateStack(cxt);
                    break;

                case ByteCodeOp::LambdaCall:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(checkNotNull(cxt, ra));
                    invalidateCurStateStack(cxt);
                    break;

                case ByteCodeOp::LocalCall:
                case ByteCodeOp::ForeignCall:
                    invalidateCurStateStack(cxt);
                    break;

                case ByteCodeOp::IntrinsicMulAddF32:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getImmediateC(vc, cxt, ip));
                    SWAG_CHECK(getImmediateD(vd, cxt, ip));
                    ra->kind = vb.kind == ValueKind::Constant && vc.kind == ValueKind::Constant && vd.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (ra->kind == ValueKind::Constant)
                        ra->reg.f32 = vb.reg.f32 * vc.reg.f32 + vd.reg.f32;
                    setConstant(cxt, ra->kind, ip, ra->reg.u32, ConstantKind::SetImmediateA);
                    break;
                case ByteCodeOp::IntrinsicMulAddF64:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getImmediateC(vc, cxt, ip));
                    SWAG_CHECK(getImmediateD(vd, cxt, ip));
                    ra->kind = vb.kind == ValueKind::Constant && vc.kind == ValueKind::Constant && vd.kind == ValueKind::Constant ? ValueKind::Constant : ValueKind::Unknown;
                    if (ra->kind == ValueKind::Constant)
                        ra->reg.f64 = vb.reg.f64 * vc.reg.f64 + vd.reg.f64;
                    setConstant(cxt, ra->kind, ip, ra->reg.u64, ConstantKind::SetImmediateA);
                    break;

                case ByteCodeOp::IntrinsicS8x1:
                case ByteCodeOp::IntrinsicS16x1:
                case ByteCodeOp::IntrinsicS32x1:
                case ByteCodeOp::IntrinsicS64x1:
                case ByteCodeOp::IntrinsicF32x1:
                case ByteCodeOp::IntrinsicF64x1:
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    ra->kind = ValueKind::Unknown;
                    if (vb.kind == ValueKind::Constant)
                    {
                        ra->kind = ValueKind::Constant;
                        SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, ra->reg, vb.reg, {}, {}, false));
                    }
                    break;

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
                    SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
                    SWAG_CHECK(getImmediateB(vb, cxt, ip));
                    SWAG_CHECK(getImmediateC(vc, cxt, ip));
                    ra->kind = ValueKind::Unknown;
                    if (vb.kind == ValueKind::Constant && vc.kind == ValueKind::Constant)
                    {
                        ra->kind = ValueKind::Constant;
                        SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, ra->reg, vb.reg, vc.reg, {}, false));
                    }
                    break;

                default:
                    Report::internalError(cxt.bc->sourceFile->module, FMT("unknown instruction [[%s]] during sanity check", g_ByteCodeOpDesc[static_cast<int>(ip->op)].name));
                    return false;
            }

            ip->dynFlags |= BCID_SAN_PASS;
            ip++;
        }

        return true;
    }
}

bool ByteCodeOptimizer::optimizePassSanity(ByteCodeOptContext* context)
{
    if (!context->bc->node || context->bc->isCompilerGenerated)
        return true;

    Context cxt;
    auto    state = new State;

    cxt.bc      = context->bc;
    state->ip   = nullptr;
    cxt.context = context;

    const auto funcDecl = castAst<AstFuncDecl>(cxt.bc->node, AstNodeKind::FuncDecl);

    state->stack.reserve(funcDecl->stackSize);
    state->stack.count = funcDecl->stackSize;
    memset(state->stack.buffer, 0, state->stack.count * sizeof(uint8_t));

    state->stackValue.reserve(funcDecl->stackSize);
    state->stackValue.count = funcDecl->stackSize;
    memset(state->stackValue.buffer, static_cast<uint8_t>(ValueKind::Invalid), state->stackValue.count * sizeof(Value));

    state->regs.reserve(context->bc->maxReservedRegisterRC);
    state->regs.count = context->bc->maxReservedRegisterRC;
    memset(state->regs.buffer, 0, state->regs.count * sizeof(Value));

    state->ip = cxt.bc->out;

    // Initialize information of stack
    for (const auto l : cxt.bc->localVars)
    {
        const auto over = l->resolvedSymbolOverload;
        SWAG_ASSERT(over);
        if (over->computedValue.storageOffset == UINT32_MAX)
            continue;
        if (over->computedValue.storageOffset + over->typeInfo->sizeOf > state->stackValue.count)
            continue;

        for (uint32_t i                                                       = 0; i < over->typeInfo->sizeOf; i++)
            state->stackValue[i + over->computedValue.storageOffset].overload = over;
    }

    cxt.states.emplace_back(state);

    for (size_t i = 0; i < cxt.states.size(); i++)
    {
        cxt.state = static_cast<int>(i);
        SWAG_CHECK(optimizePassSanityStack(context, cxt));
        if (i == cxt.states.size() - 1)
            break;
        for (uint32_t j = 0; j < cxt.bc->numInstructions; j++)
            cxt.bc->out[j].dynFlags &= ~BCID_SAN_PASS;
    }

    for (const auto s : cxt.states)
        delete s;

    return true;
}

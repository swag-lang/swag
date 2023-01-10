#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "Log.h"
#include "Ast.h"
#pragma optimize("", off)

enum class RefKind
{
    Unknown,
    Register,
    Stack,
};

enum class ValueKind : uint8_t
{
    Unknown,
    StackAddr,
    Constant,
};

static const uint8_t STATE_INVALID   = 0x00;
static const uint8_t STATE_CONSTANT  = 0x01;
static const uint8_t STATE_STACKADDR = 0x02;

struct Value
{
    ValueKind kind = ValueKind::Unknown;
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
    bool                  mustStop = false;
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

bool checkStackOffset(Context& cxt, uint64_t stackOffset)
{
    if (stackOffset >= cxt.stack.count)
        return raiseError(cxt, Fmt(Err(San0002), stackOffset, cxt.stack.count));
    return true;
}

static bool checkStackState(Context& cxt, void* addr, uint32_t sizeOf, ValueKind kind, bool is)
{
    auto offset = (uint8_t*) addr - cxt.stack.buffer;
    SWAG_ASSERT(sizeOf <= 8);
    SWAG_ASSERT(offset + sizeOf <= cxt.stackState.count);
    auto addrState = cxt.stackState.buffer + offset;

    for (uint32_t i = 0; i < sizeOf; i++)
    {
        auto state = (ValueKind) *addrState++;
        if (state == kind && !is)
            return false;
        if (state != kind && is)
            return false;
    }

    return true;
}

static bool checkStackInitialized(Context& cxt, void* addr, uint32_t sizeOf)
{
    auto ok = checkStackState(cxt, addr, sizeOf, ValueKind::Unknown, false);
    if (!ok)
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
    SWAG_ASSERT(reg < cxt.regs.count);
    result = &cxt.regs[reg];
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
    Value                imm;

    while (true)
    {
        if (ip->flags & 1)
            break;
        ip->flags |= 1;

        cxt.ip = ip;
        switch (ip->op)
        {
        case ByteCodeOp::DecSPBP:
        case ByteCodeOp::Ret:
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
        case ByteCodeOp::SetZeroStack64:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            *(uint64_t*) addr = 0;
            setStackState(cxt, addr, 8, ValueKind::Constant);
            break;

        case ByteCodeOp::SetAtStackPointer64:
            SWAG_CHECK(getStackAddress(addr, cxt, ip->a.u32));
            SWAG_CHECK(getImmediateB(imm, cxt, ip));
            *(uint64_t*) addr = imm.reg.u64;
            setStackState(cxt, addr, 8, imm.kind);
            break;

        case ByteCodeOp::SetAtPointer32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind != ValueKind::StackAddr)
                break;
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
            SWAG_CHECK(getImmediateB(imm, cxt, ip));
            *(uint32_t*) addr = imm.reg.u32;
            setStackState(cxt, addr, 4, ValueKind::Constant);
            break;
        case ByteCodeOp::SetAtPointer64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind != ValueKind::StackAddr)
                break;
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u64 + ip->c.u32));
            SWAG_CHECK(getImmediateB(imm, cxt, ip));
            *(uint64_t*) addr = imm.reg.u64;
            setStackState(cxt, addr, 8, imm.kind);
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

        case ByteCodeOp::Mul64byVB64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            ra->reg.s64 *= ip->b.u64;
            break;

        case ByteCodeOp::GetFromStack32:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
            SWAG_CHECK(checkStackInitialized(cxt, addr, 4));
            ra->reg.u64 = *(uint32_t*) addr;
            break;
        case ByteCodeOp::GetFromStack64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, ip->b.u32));
            SWAG_CHECK(checkStackInitialized(cxt, addr, 8));
            ra->reg.u64 = *(uint64_t*) addr;
            break;

        case ByteCodeOp::IncPointer64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getRegister(rc, cxt, ip->c.u32));
            *rc = *ra;
            SWAG_CHECK(getImmediateB(imm, cxt, ip));
            rc->reg.u64 += imm.reg.s64;
            break;

        case ByteCodeOp::CopyRCtoRR:
            if (ip->flags & BCI_IMM_A)
                break;
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            if (ra->kind == ValueKind::StackAddr)
                return checkEscapeFrame(cxt, ra->reg.u32);
            break;

        case ByteCodeOp::DeRef64:
            SWAG_CHECK(getRegister(rb, cxt, ip->b.u32));
            if (rb->kind != ValueKind::StackAddr)
                break;
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, rb->reg.u64 + ip->c.s64));
            SWAG_CHECK(checkStackInitialized(cxt, addr, 8));
            if (!checkStackState(cxt, addr, 8, ValueKind::StackAddr, true))
                ra->kind = ValueKind::Constant;
            ra->reg.u64 = *(uint64_t*) addr;
            break;

        case ByteCodeOp::AffectOpPlusEqS64:
            SWAG_CHECK(getRegister(ra, cxt, ip->a.u32));
            SWAG_CHECK(getStackAddress(addr, cxt, ra->reg.u32));
            SWAG_CHECK(getImmediateB(imm, cxt, ip));
            *(uint64_t*) addr += imm.reg.s64;
            break;

        default:
            cxt.mustStop = true;
            return true;
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
            if (cxt.mustStop)
                return true;
            cxt = saveCxt;
        }
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassCheck(ByteCodeOptContext* context)
{
#if 0
     if (context->bc->name != "__compiler4160.toto")
         return true;
     context->bc->print();
#endif

    if (!context->bc->node)
        return true;

    Context cxt;

    cxt.bc       = context->bc;
    cxt.ip       = nullptr;
    cxt.context  = context;
    cxt.mustStop = false;

    auto funcDecl = CastAst<AstFuncDecl>(cxt.bc->node, AstNodeKind::FuncDecl);

    cxt.stack.reserve(funcDecl->stackSize);
    memset(cxt.stack.buffer, 0, funcDecl->stackSize * sizeof(uint8_t));
    cxt.stack.count = funcDecl->stackSize;

    cxt.stackState.reserve(funcDecl->stackSize);
    memset(cxt.stackState.buffer, (uint8_t) ValueKind::Unknown, funcDecl->stackSize * sizeof(uint8_t));
    cxt.stackState.count = funcDecl->stackSize;

    cxt.regs.reserve(context->bc->maxReservedRegisterRC);
    memset(cxt.regs.buffer, 0, context->bc->maxReservedRegisterRC * sizeof(Value));
    cxt.regs.count = context->bc->maxReservedRegisterRC;

    context->checkContext = &cxt;
    SWAG_CHECK(optimizePassCheckStack(context, 0));
    if (g_CommandLine.stats && !cxt.mustStop)
        g_Stats.numSanPassed++;
    else if (g_CommandLine.stats && cxt.mustStop)
        g_Stats.numSanSkipped++;

    return true;
}
#include "pch.h"
#include "ByteCodeOptimizer.h"
#include "TypeInfo.h"
#include "Log.h"
#pragma optimize("", off)

enum class RefKind
{
    Unknown,
    Register,
    Stack
};

enum class ValueKind
{
    StackAddr,
    Unknown,
};

struct Value
{
    ValueKind kind            = ValueKind::Unknown;
    int64_t   stackAddrOffset = 0;
};

struct RefContext
{
    RefKind  kind = RefKind::Unknown;
    uint32_t who  = 0;
    Value    value;
};

struct Context
{
    VectorNative<RefContext> stack;
};

static RefContext* getRefContext(Context& cxt, RefKind kind, uint32_t who)
{
    for (auto& c : cxt.stack)
    {
        if (c.kind == kind && c.who == who)
            return &c;
    }

    return nullptr;
}

static RefContext* getOrUseRefContext(Context& cxt, RefKind kind, uint32_t who)
{
    auto c = getRefContext(cxt, kind, who);
    if (c)
        return c;
    cxt.stack.push_back({kind, who});
    return &cxt.stack.back();
}

static SymbolOverload* getLocalVar(ByteCodeOptContext* context, uint32_t stackOffset)
{
    for (auto n : context->bc->localVars)
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

static bool errEscapeFrame(ByteCodeOptContext* context, ByteCodeInstruction* ip, int64_t stackOffset)
{
    SWAG_ASSERT(stackOffset >= 0 && stackOffset < UINT32_MAX);
    auto sym = getLocalVar(context, (uint32_t) stackOffset);
    if (!sym)
    {
        Diagnostic diag({ip->node, ip->node->token, Err(Err0578)});
        context->report(diag);
    }
    else
    {
        Diagnostic diag({ip->node, ip->node->token, Err(Err0578)});
        Diagnostic note({sym->node, sym->node->token, Nte(Nte0003), DiagnosticLevel::Note});
        context->report(diag, &note);
    }

    return false;
}

bool ByteCodeOptimizer::optimizePassCheckRetStack(ByteCodeOptContext* context, uint32_t curNode)
{
    auto&                cxt  = *(Context*) context->checkContext;
    ByteCodeOptTreeNode* node = &context->tree[curNode];
    auto                 ip   = node->start;
    while (true)
    {
        if (ip->flags & 1)
            break;
        ip->flags |= 1;

        RefContext* na = nullptr;
        RefContext* nb = nullptr;
        RefContext* nc = nullptr;
        RefContext* nd = nullptr;

        switch (ip->op)
        {
        case ByteCodeOp::MakeStackPointer:
            na                        = getOrUseRefContext(cxt, RefKind::Register, ip->a.u32);
            na->value.kind            = ValueKind::StackAddr;
            na->value.stackAddrOffset = ip->b.u32;
            break;

        case ByteCodeOp::SetAtStackPointer64:
            na        = getOrUseRefContext(cxt, RefKind::Stack, ip->a.u32);
            nb        = getOrUseRefContext(cxt, RefKind::Register, ip->b.u32);
            na->value = nb->value;
            break;

        case ByteCodeOp::SetAtPointer64:
            if (!(ip->flags & BCI_IMM_B))
            {
                na = getOrUseRefContext(cxt, RefKind::Register, ip->a.u32);
                if (na->value.kind == ValueKind::StackAddr)
                {
                    nc        = getOrUseRefContext(cxt, RefKind::Stack, (uint32_t) (na->value.stackAddrOffset + ip->c.u64));
                    nb        = getOrUseRefContext(cxt, RefKind::Register, ip->b.u32);
                    nc->value = nb->value;
                }
            }
            break;

        case ByteCodeOp::GetFromStack64:
            na        = getOrUseRefContext(cxt, RefKind::Register, ip->a.u32);
            nb        = getOrUseRefContext(cxt, RefKind::Stack, ip->b.u32);
            na->value = nb->value;
            break;

        case ByteCodeOp::IncPointer64:
            nc        = getOrUseRefContext(cxt, RefKind::Register, ip->c.u32);
            na        = getOrUseRefContext(cxt, RefKind::Register, ip->a.u32);
            nc->value = na->value;
            if (nc->value.kind == ValueKind::StackAddr)
            {
                if (ip->flags & BCI_IMM_B)
                    nc->value.stackAddrOffset += ip->b.s64;
                else
                {
                    nb = getOrUseRefContext(cxt, RefKind::Register, ip->b.u32);
                }
            }

            break;

        case ByteCodeOp::CopyRCtoRR:
            if (ip->flags & BCI_IMM_A)
                break;

            na = getRefContext(cxt, RefKind::Register, ip->a.u32);
            if (na && na->value.kind == ValueKind::StackAddr)
                return errEscapeFrame(context, ip, na->value.stackAddrOffset);
            break;

        case ByteCodeOp::CopyRCtoRRRet:
            if (ip->flags & BCI_IMM_B)
                break;

            nb = getRefContext(cxt, RefKind::Register, ip->b.u32);
            if (nb && nb->value.kind == ValueKind::StackAddr)
                return errEscapeFrame(context, ip, nb->value.stackAddrOffset);
            break;

        default:
            if (ByteCode::hasWriteRegInA(ip))
            {
                na             = getOrUseRefContext(cxt, RefKind::Register, ip->a.u32);
                na->value.kind = ValueKind::Unknown;
            }
            if (ByteCode::hasWriteRegInB(ip))
            {
                nb             = getOrUseRefContext(cxt, RefKind::Register, ip->b.u32);
                nb->value.kind = ValueKind::Unknown;
            }
            if (ByteCode::hasWriteRegInC(ip))
            {
                nc             = getOrUseRefContext(cxt, RefKind::Register, ip->c.u32);
                nc->value.kind = ValueKind::Unknown;
            }
            if (ByteCode::hasWriteRegInD(ip))
            {
                nd             = getOrUseRefContext(cxt, RefKind::Register, ip->d.u32);
                nd->value.kind = ValueKind::Unknown;
            }
            break;
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
            auto saveCount = cxt.stack.count;
            optimizePassCheckRetStack(context, n);
            cxt.stack.count = saveCount;
        }
    }

    return true;
}

bool ByteCodeOptimizer::optimizePassCheck(ByteCodeOptContext* context)
{
    Context cxt;
    context->checkContext = &cxt;
    SWAG_CHECK(optimizePassCheckRetStack(context, 0));
    return true;
}
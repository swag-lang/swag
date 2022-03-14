#include "pch.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ByteCodeStack.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Diagnostic.h"
#include "Backend.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags, __dis, __nump) {#__op, (int) strlen(#__op), __flags, __dis, __nump},
ByteCodeOpDesc g_ByteCodeOpDesc[] = {
#include "ByteCodeOpList.h"
};

void ByteCode::releaseOut()
{
    if (substitution)
        return;
    auto s = Allocator::alignSize(maxInstructions * sizeof(ByteCodeInstruction));
    g_Allocator.free(out, s);
}

void ByteCode::getLocation(ByteCode* bc, ByteCodeInstruction* ip, SourceFile** file, SourceLocation** location, bool force)
{
    *file = ip && ip->node && ip->node->sourceFile ? ip->node->sourceFile : bc->sourceFile;

    // When generated private code, without logging to generated file, then we must take the original source file
    if ((*file) && (*file)->fileForSourceLocation)
        *file = (*file)->fileForSourceLocation;

    *location = force ? ip->location : nullptr;

    if (!ip || !ip->node || !ip->node->ownerScope || ip->node->kind == AstNodeKind::FuncDecl)
        return;

    // When inside an inline block (and not a mixin), zap all inline chain to the caller
    if (!bc->sourceFile || !bc->sourceFile->module->buildCfg.byteCodeDebugInline)
    {
        if (ip->node->ownerInline && !(ip->node->flags & AST_IN_MIXIN) && ip->node->ownerInline->ownerFct == ip->node->ownerFct)
        {
            auto n = ip->node;
            while (n->ownerInline && !(n->flags & AST_IN_MIXIN) && (n->ownerInline->ownerFct == n->ownerFct))
                n = n->ownerInline;
            *location = &n->token.startLocation;
            *file     = n->sourceFile;
            return;
        }
    }

    *location = ip->location;
}

const Utf8& ByteCode::getCallName()
{
    ScopedLock lk(mutexCallName);
    if (!callName.empty())
        return callName;

    // If this is an intrinsic that can be called by the compiler itself, it should not
    // have overloads, and the name will be the name alone (without the node address which is
    // used to differentiate overloads)
    if (node && node->sourceFile->isRuntimeFile && !(node->attributeFlags & ATTRIBUTE_CALLBACK) && !node->ownerStructScope)
    {
        if (node->resolvedSymbolName && node->resolvedSymbolName->cptOverloadsInit == 1)
        {
            callName = name;
            return callName;
        }
    }

    if (name.empty())
        callName = node->getScopedName();
    else
        callName = name;
    callName += Fmt("_%lX", (uint64_t) this);
    return callName;
}

TypeInfoFuncAttr* ByteCode::getCallType()
{
    if (node && node->typeInfo->kind == TypeInfoKind::FuncAttr)
        return CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    return typeInfoFunc;
}

void ByteCode::addCallStack(ByteCodeRunContext* context)
{
    if (context->bc && context->bc->node && context->bc->node->flags & AST_NO_CALLSTACK)
        return;

    ByteCodeStackStep stackStep;
    stackStep.bc = context->bc;
    stackStep.ip = context->ip - 1;
    stackStep.bp = context->bp;
    g_ByteCodeStack.push(stackStep);
}

void ByteCode::enterByteCode(ByteCodeRunContext* context, uint32_t popParamsOnRet, uint32_t returnReg)
{
    if (g_CommandLine->maxRecurse && context->curRC == (int) g_CommandLine->maxRecurse)
    {
        context->hasError = true;
        context->errorMsg = Fmt(Err(Err0076), g_CommandLine->maxRecurse);
        return;
    }

    context->curRC++;
    if (context->curRC >= context->registersRC.size())
        context->registersRC.emplace_back(new VectorNative<Register>());

    context->registersRC[context->curRC]->reserve(maxReservedRegisterRC, false);
    context->registersRC[context->curRC]->count = maxReservedRegisterRC;

    context->popParamsOnRet.push_back(popParamsOnRet);
    context->returnRegOnRet.push_back(returnReg);
    if (returnReg != UINT32_MAX)
        context->returnRegOnRetRR.push_back(context->registersRR[0].u64);
}

void ByteCode::leaveByteCode(ByteCodeRunContext* context, bool popCallStack)
{
    if (popCallStack)
        g_ByteCodeStack.pop();
    context->curRC--;
}

void ByteCode::markLabels()
{
    uint32_t count = numJumps;
    auto     ip    = out;
    for (uint32_t i = 0; i < numInstructions && count; i++, ip++)
    {
        if (ByteCode::isJump(ip))
        {
            ip[ip->b.s32 + 1].flags |= BCI_JUMP_DEST;
            ip[1].flags |= BCI_JUMP_DEST;
            count--;
        }
    }
}

bool ByteCode::isDoingNothing()
{
    if (getCallType()->returnType != g_TypeMgr->typeInfoVoid)
        return false;

    if (numInstructions == 2)
    {
        if (out[0].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::GetFromStackParam64 &&
            out[1].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::DecSPBP &&
            out[1].op == ByteCodeOp::Ret)
        {
            return true;
        }
    }

    if (numInstructions == 4)
    {
        if (out[0].op == ByteCodeOp::ClearRA &&
            out[1].op == ByteCodeOp::IntrinsicSetErr &&
            out[2].op == ByteCodeOp::Ret)
        {
            return true;
        }
    }

    return false;
}

void* ByteCode::doForeignLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    u |= SWAG_LAMBDA_FOREIGN_MARKER;
    return (void*) u;
}

void* ByteCode::undoForeignLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    SWAG_ASSERT(u & SWAG_LAMBDA_FOREIGN_MARKER);
    u ^= SWAG_LAMBDA_FOREIGN_MARKER;
    return (void*) u;
}

bool ByteCode::isForeignLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    return u & SWAG_LAMBDA_FOREIGN_MARKER;
}

void* ByteCode::doByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    u |= SWAG_LAMBDA_BC_MARKER;
    return (void*) u;
}

void* ByteCode::undoByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    SWAG_ASSERT(u & SWAG_LAMBDA_BC_MARKER);
    u ^= SWAG_LAMBDA_BC_MARKER;
    return (void*) u;
}

bool ByteCode::isByteCodeLambda(void* ptr)
{
    uint64_t u = (uint64_t) ptr;
    return u & SWAG_LAMBDA_BC_MARKER;
}

void ByteCode::computeCrc()
{
    if (!sourceFile)
        return;
    if (!Backend::canEmitFunction(this))
        return;

    auto module = sourceFile->module;

    // Compute hash content
    uint32_t hash = 0;
    for (uint32_t i = 0; i < numInstructions; i++)
    {
#define ADDH(__a)           \
    hash += (uint32_t) __a; \
    hash ^= hash << 10;     \
    hash += hash >> 1;      \
    hash ^= hash << 3;      \
    hash += hash >> 5;      \
    hash ^= hash << 4;      \
    hash += hash >> 17;     \
    hash ^= hash << 25;     \
    hash += hash >> 6;

        auto& ins = out[i];
        ADDH(ins.op);
        ADDH(ins.flags);
        ADDH(ins.a.u32);
        ADDH(ins.b.u32);
        ADDH(ins.c.u32);
        ADDH(ins.d.u32);
    }

    contentCrc = hash;

    ScopedLock lk(module->mutexMapCrcBc);
    auto       it = module->mapCrcBc.find(hash);
    if (it == module->mapCrcBc.end())
    {
        module->mapCrcBc[hash] = this;
    }
    else if (it->second->numInstructions != numInstructions)
    {
        nextCrc                = it->second->nextCrc;
        module->mapCrcBc[hash] = this;
    }
    else
    {
        auto      other  = it->second;
        ByteCode* sameAs = nullptr;
        while (other)
        {
            bool isSame = true;
            for (uint32_t i = 0; i < numInstructions; i++)
            {
                if (other->out[i].op != out[i].op ||
                    other->out[i].flags != out[i].flags ||
                    other->out[i].a.u64 != out[i].a.u64 ||
                    other->out[i].b.u64 != out[i].b.u64 ||
                    other->out[i].c.u64 != out[i].c.u64 ||
                    other->out[i].d.u64 != out[i].d.u64)
                {
                    isSame = false;
                    break;
                }
            }

            if (isSame)
            {
                sameAs = other;
                break;
            }

            other = other->nextCrc;
        }

        if (!sameAs)
        {
            nextCrc                = it->second->nextCrc;
            module->mapCrcBc[hash] = this;
        }
        else
        {
            substitution   = sameAs;
            sameAs->isUsed = true;
        }
    }
}

ByteCode* ByteCode::getSubstitution()
{
    if (!substitution)
        return this;

    auto res = substitution;
    while (res->substitution)
        res = res->substitution;

    return res;
}
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

void ByteCode::setCallName(const Utf8& n)
{
    ScopedLock lk(mutexCallName);
    callName = n;
}

Utf8 ByteCode::getCallName()
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
    {
        ScopedLock lock(node->mutex);
        callName = node->getScopedName();
    }
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

void ByteCode::enterByteCode(ByteCodeRunContext* context, uint32_t popParamsOnRet, uint32_t returnRegOnRet, uint32_t incSPPostCall)
{
    if (g_CommandLine->maxRecurse && context->curRC == (int) g_CommandLine->maxRecurse)
    {
        context->raiseError(Fmt(Err(Err0076), g_CommandLine->maxRecurse));
        return;
    }

    context->curRC++;
    context->registersRC.push_back(context->registers.count);
    context->registers.reserve(context->registers.count + maxReservedRegisterRC);
    context->curRegistersRC = context->registers.buffer + context->registers.count;
    context->registers.count += maxReservedRegisterRC;

    if (returnRegOnRet != UINT32_MAX)
        context->popOnRet.push_back(context->registersRR[0].u64);
    context->popOnRet.push_back(returnRegOnRet);
    context->popOnRet.push_back((popParamsOnRet * sizeof(void*)) + incSPPostCall);
}

void ByteCode::leaveByteCode(ByteCodeRunContext* context, bool popCallStack)
{
    if (popCallStack)
        g_ByteCodeStack.pop();
    context->curRC--;
    if (context->curRC >= 0)
    {
        auto newCount            = context->registersRC.get_pop_back();
        context->registers.count = newCount;
        context->curRegistersRC  = context->registers.buffer + context->registersRC.back();
    }
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
        else if (ByteCode::isJumpDyn(ip))
        {
            int32_t* table = (int32_t*) sourceFile->module->compilerSegment.address(ip->d.u32);
            for (uint32_t idx = 0; idx < ip->c.u32; idx++)
            {
                ip[table[idx] + 1].flags |= BCI_JUMP_DEST;
            }

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
        if (out[0].op == ByteCodeOp::GetParam64 &&
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
            out[1].op == ByteCodeOp::InternalSetErr &&
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
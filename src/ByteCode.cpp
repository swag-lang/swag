#include "pch.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ByteCodeStack.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op, __flags, __dis) {#__op, (int) strlen(#__op), __flags, __dis},
ByteCodeOpDesc g_ByteCodeOpDesc[] = {
#include "ByteCodeOpList.h"
};

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

Utf8 ByteCode::callName()
{
    // If this is an intrinsic that can be called by the compiler itself, it should not
    // have overloads, and the name will be the name alone (without the node address which is
    // used to differentiate overloads)
    if (node && node->sourceFile->isRuntimeFile && !(node->attributeFlags & ATTRIBUTE_CALLBACK) && !node->ownerStructScope)
    {
        SWAG_ASSERT(node->resolvedSymbolName);
        if (node->resolvedSymbolName->cptOverloadsInit == 1)
            return name;
    }

    Utf8 callName;
    if (name.empty())
        callName = node->getScopedName();
    else
        callName = name;
    callName += Utf8::format("_%lX", (uint64_t) this);
    return callName;
}

TypeInfoFuncAttr* ByteCode::callType()
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
    if (g_CommandLine.maxRecurse && context->curRC == (int) g_CommandLine.maxRecurse)
    {
        context->hasError = true;
        context->errorMsg = Utf8::format(Msg0076, g_CommandLine.maxRecurse);
        return;
    }

    context->curRC++;
    if (context->curRC >= context->registersRC.size())
        context->registersRC.emplace_back(new VectorNative<Register>());

    context->registersRC[context->curRC]->reserve(maxReservedRegisterRC, false);
    context->registersRC[context->curRC]->count = maxReservedRegisterRC;

    context->popParamsOnRet.push_back(popParamsOnRet);
    context->returnRegOnRet.push_back(returnReg);
    if (returnReg != UINT8_MAX)
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
    if (callType()->returnType != g_TypeMgr.typeInfoVoid)
        return false;

    if (numInstructions == 2)
    {
        if (out[0].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::GetFromStackParam64 && out[1].op == ByteCodeOp::Ret)
            return true;
    }

    if (numInstructions == 3)
    {
        if (out[0].op == ByteCodeOp::DecSPBP &&
            out[2].op == ByteCodeOp::Ret)
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

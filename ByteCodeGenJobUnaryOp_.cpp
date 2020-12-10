#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"

bool ByteCodeGenJob::emitUnaryOpMinus(ByteCodeGenContext* context, uint32_t r0)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteReferenceType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitUnaryOpMinus, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::NegS32, r0);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::NegS64, r0);
        return true;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::NegF32, r0);
        return true;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::NegF64, r0);
        return true;
    default:
        return internalError(context, "emitUnaryOpMinus, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOpInvert(ByteCodeGenContext* context, uint32_t r0)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteReferenceType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitUnaryOpInvert, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::InvertS8, r0);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::InvertS16, r0);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::InvertS32, r0);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::InvertS64, r0);
        return true;
    default:
        return internalError(context, "emitUnaryOpInvert, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOp(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    auto     r0            = node->childs[0]->resultRegisterRC;
    node->resultRegisterRC = r0;

    if (!(node->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST1;
    }

    if (node->resolvedUserOpSymbolOverload && node->resolvedUserOpSymbolOverload->symbol->kind == SymbolKind::Function)
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result != ContextResult::Done)
            return true;
        return true;
    }

    switch (node->token.id)
    {
    case TokenId::SymExclam:
        emitInstruction(context, ByteCodeOp::NegBool, r0);
        return true;
    case TokenId::SymMinus:
        SWAG_CHECK(emitUnaryOpMinus(context, r0));
        return true;
    case TokenId::SymTilde:
        SWAG_CHECK(emitUnaryOpInvert(context, r0));
        return true;
    default:
        return internalError(context, "emitUnaryOp, invalid token op");
    }
}

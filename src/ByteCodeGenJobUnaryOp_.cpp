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
        emitInstruction(context, ByteCodeOp::InvertU8, r0);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::InvertU16, r0);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::InvertU32, r0);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::InvertU64, r0);
        return true;
    default:
        return internalError(context, "emitUnaryOpInvert, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOp(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs[0]->resultRegisterRC;

    if (!(node->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST1;
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result != ContextResult::Done)
            return true;
        return true;
    }

    switch (node->token.id)
    {
    case TokenId::SymExclam:
    {
        auto rt = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::NegBool, rt, node->resultRegisterRC);
        freeRegisterRC(context, node->resultRegisterRC);
        node->resultRegisterRC = rt;
        return true;
    }
    case TokenId::SymMinus:
        ensureCanBeChangedRC(context, node->resultRegisterRC);
        SWAG_CHECK(emitUnaryOpMinus(context, node->resultRegisterRC));
        return true;
    case TokenId::SymTilde:
        ensureCanBeChangedRC(context, node->resultRegisterRC);
        SWAG_CHECK(emitUnaryOpInvert(context, node->resultRegisterRC));
        return true;
    default:
        return internalError(context, "emitUnaryOp, invalid token op");
    }
}

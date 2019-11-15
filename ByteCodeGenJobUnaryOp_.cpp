#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "SymTable.h"

bool ByteCodeGenJob::emitUnaryOpMinus(ByteCodeGenContext* context, uint32_t r0)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitUnaryOpMinus, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::NegS32, r0);
        return true;
    case NativeTypeKind::S64:
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
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitUnaryOpTilde, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::InvertS32, r0);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::InvertS32, r0);
        return true;
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::InvertU32, r0);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::InvertU32, r0);
        return true;
    default:
        return internalError(context, "emitUnaryOpTilde, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOp(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    auto     r0            = node->childs[0]->resultRegisterRC;
    node->resultRegisterRC = r0;

	if (node->byteCodePass == 0)
	{
		SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
	}

    if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
    {
        SWAG_CHECK(emitUserOp(context));
		if (context->result == ContextResult::Pending)
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

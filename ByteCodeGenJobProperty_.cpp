#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitKindOfProperty(ByteCodeGenContext* context)
{
    auto node                      = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    node->resultRegisterRC         = node->childs.front()->resultRegisterRC[1];
    node->parent->resultRegisterRC = node->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitCountOfProperty(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto expr     = node->childs.back();
    auto typeInfo = TypeManager::concreteType(expr->typeInfo);

    if (node->resolvedUserOpSymbolName)
    {
        SWAG_CHECK(emitUserOp(context));
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::String) || typeInfo->kind == TypeInfoKind::Slice)
    {
        node->resultRegisterRC = expr->resultRegisterRC[1];
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        node->resultRegisterRC = expr->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::DeRef32, node->resultRegisterRC);
        return true;
    }

    return internalError(context, "emitCountProperty, type not supported");
}

bool ByteCodeGenJob::emitDataOfProperty(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);

    if (node->resolvedUserOpSymbolName)
    {
        SWAG_CHECK(emitUserOp(context));
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::String) ||
        typeInfo->isNative(NativeTypeKind::Any) ||
        typeInfo->kind == TypeInfoKind::Slice ||
        typeInfo->kind == TypeInfoKind::Array)
    {
        node->resultRegisterRC         = node->childs.front()->resultRegisterRC[0];
        node->parent->resultRegisterRC = node->resultRegisterRC;
        return true;
    }

    return internalError(context, "emitDataProperty, type not supported");
}

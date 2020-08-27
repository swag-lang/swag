#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitSliceOfProperty(ByteCodeGenContext* context)
{
    auto node              = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    node->resultRegisterRC = node->childs.front()->resultRegisterRC;
    node->resultRegisterRC += node->childs.back()->resultRegisterRC;
    transformResultToLinear2(context, node);
    return true;
}

bool ByteCodeGenJob::IntrinsicMkInterface(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto params = node->childs.front();

    reserveLinearRegisterRC(context, node->resultRegisterRC, 3);

    // Result will be a pointer to two contiguous registers
    emitInstruction(context, ByteCodeOp::CopyRBAddrToRA, node->resultRegisterRC[0], node->resultRegisterRC[1]);

    // Reference to the interface concrete type info
    auto childItf = params->childs[2];
    SWAG_ASSERT(childItf->computedValue.reg.u32 != UINT32_MAX);
    auto r0 = reserveRegisterRC(context);

    emitInstruction(context, ByteCodeOp::MakeTypeSegPointer, r0)->b.u32 = childItf->computedValue.reg.u32;

    // Copy object pointer to first result register
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[1], params->childs[0]->resultRegisterRC);

    // Get interface itable pointer in the second result register
    emitInstruction(context, ByteCodeOp::IntrinsicMkInterface, params->childs[1]->resultRegisterRC, r0, node->resultRegisterRC[2]);

    freeRegisterRC(context, params->childs[0]);
    freeRegisterRC(context, params->childs[1]);
    freeRegisterRC(context, r0);
    return true;
}

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

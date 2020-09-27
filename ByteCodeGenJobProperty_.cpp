#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitIntrinsicMakeAny(ByteCodeGenContext* context)
{
    auto node              = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    node->resultRegisterRC = node->childs.front()->resultRegisterRC;
    node->resultRegisterRC += node->childs.back()->resultRegisterRC;
    transformResultToLinear2(context, node);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicMakeSlice(ByteCodeGenContext* context)
{
    auto node              = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    node->resultRegisterRC = node->childs.front()->resultRegisterRC;
    node->resultRegisterRC += node->childs.back()->resultRegisterRC;
    transformResultToLinear2(context, node);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicMakeInterface(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto params = node->childs.front();

    reserveLinearRegisterRC2(context, node->resultRegisterRC);

    // Reference to the interface concrete type info
    auto childItf = params->childs[2];
    SWAG_ASSERT(childItf->computedValue.reg.u32 != UINT32_MAX);

    auto r0                                                             = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::MakeTypeSegPointer, r0)->b.u32 = childItf->computedValue.reg.u32;

    // Copy object pointer to first result register
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[0], params->childs[0]->resultRegisterRC);

    // Get interface itable pointer in the second result register
    emitInstruction(context, ByteCodeOp::IntrinsicInterfaceOf, params->childs[1]->resultRegisterRC, r0, node->resultRegisterRC[1]);

    freeRegisterRC(context, params->childs[0]);
    freeRegisterRC(context, params->childs[1]);
    freeRegisterRC(context, params->childs[2]);
    freeRegisterRC(context, r0);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicKindOf(ByteCodeGenContext* context)
{
    auto node  = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto front = node->childs.front();
    SWAG_ASSERT(front->resultRegisterRC.size() <= 2);
    node->resultRegisterRC         = front->resultRegisterRC[1];
    node->parent->resultRegisterRC = node->resultRegisterRC;
    freeRegisterRC(context, front->resultRegisterRC[0]);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicCountOf(ByteCodeGenContext* context)
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
        SWAG_ASSERT(expr->resultRegisterRC.size() <= 2);
        freeRegisterRC(context, expr->resultRegisterRC[0]);
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

bool ByteCodeGenJob::emitIntrinsicDataOf(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto front    = node->childs.front();
    auto typeInfo = TypeManager::concreteType(front->typeInfo);

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
        truncRegisterRC(context, front->resultRegisterRC, 1);
        node->resultRegisterRC         = front->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        return true;
    }

    return internalError(context, "emitDataProperty, type not supported");
}

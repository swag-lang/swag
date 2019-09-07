#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "ByteCode.h"

bool ByteCodeGenJob::emitCountProperty(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto expr     = node->childs.front();
    auto typeInfo = TypeManager::concreteType(expr->typeInfo);

    if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
    {
        node->resultRegisterRC = expr->resultRegisterRC[1];
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic)
    {
        node->resultRegisterRC = expr->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::DeRef32, node->resultRegisterRC);
    }
    else
    {
        return internalError(context, "emitCountProperty, type not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitDataProperty(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto typeInfo = TypeManager::concreteType(node->expression->typeInfo);

    if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
    {
        node->resultRegisterRC = node->expression->resultRegisterRC[0];
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        node->resultRegisterRC = node->expression->resultRegisterRC[0];
    }
    else
    {
        return internalError(context, "emitDataProperty, type not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitTypeOfProperty(ByteCodeGenContext* context)
{
    auto node = context->node;

    reserveRegisterRC(context, node->resultRegisterRC, 2);
    auto inst   = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, node->resultRegisterRC[0], node->resultRegisterRC[1]);
    inst->c.u64 = ((uint64_t) node->computedValue.reg.u32 << 32) | (uint32_t) 1;
    return true;
}
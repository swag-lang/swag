#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Report.h"

bool ByteCodeGenJob::emitIntrinsicMakeAny(ByteCodeGenContext* context)
{
    auto node  = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto front = node->childs.front();
    auto back  = node->childs.back();
    reserveRegisterRC(context, node->resultRegisterRC, 2);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[0], front->resultRegisterRC);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[1], back->resultRegisterRC);
    freeRegisterRC(context, front);
    freeRegisterRC(context, back);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicMakeCallback(ByteCodeGenContext* context)
{
    auto node    = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto ptrNode = node->childs.front();
    EMIT_INST1(context, ByteCodeOp::IntrinsicMakeCallback, ptrNode->resultRegisterRC);
    node->resultRegisterRC = ptrNode->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitIntrinsicMakeSlice(ByteCodeGenContext* context)
{
    auto node      = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto ptrNode   = node->childs.front();
    auto countNode = node->childs.back();

    SWAG_CHECK(emitCast(context, countNode, countNode->typeInfo, countNode->castedTypeInfo));
    reserveRegisterRC(context, node->resultRegisterRC, 2);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[0], ptrNode->resultRegisterRC);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[1], countNode->resultRegisterRC);
    freeRegisterRC(context, ptrNode);
    freeRegisterRC(context, countNode);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicMakeInterface(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto params = node->childs.front();

    reserveLinearRegisterRC2(context, node->resultRegisterRC);

    // Reference to the interface concrete type info
    auto childItf = params->childs[2];
    SWAG_ASSERT(childItf->computedValue);
    SWAG_ASSERT(childItf->computedValue->storageSegment);
    SWAG_ASSERT(childItf->computedValue->storageOffset != UINT32_MAX);

    auto constSegment = childItf->computedValue->storageSegment;
    auto r0           = reserveRegisterRC(context);
    emitMakeSegPointer(context, constSegment, childItf->computedValue->storageOffset, r0);

    // Copy object pointer to first result register
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[0], params->childs[0]->resultRegisterRC);

    // Get interface itable pointer in the second result register
    EMIT_INST3(context, ByteCodeOp::IntrinsicItfTableOf, params->childs[1]->resultRegisterRC, r0, node->resultRegisterRC[1]);

    freeRegisterRC(context, params->childs[0]);
    freeRegisterRC(context, params->childs[1]);
    freeRegisterRC(context, params->childs[2]);
    freeRegisterRC(context, r0);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicSpread(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto expr     = node->childs.back();
    auto typeInfo = TypeManager::concreteType(expr->typeInfo);

    if (typeInfo->isArray())
    {
        transformResultToLinear2(context, expr);
        node->resultRegisterRC = expr->resultRegisterRC;
        auto typeArr           = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto inst              = EMIT_INST1(context, ByteCodeOp::SetImmediate64, expr->resultRegisterRC[1]);
        inst->b.u64            = typeArr->count;
    }
    else if (typeInfo->isListArray() || typeInfo->isSlice())
    {
        node->resultRegisterRC = expr->resultRegisterRC;
    }
    else
    {
        return Report::internalError(context->node, "emitIntrinsicSpread, type not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitIntrinsicLocationSI(ByteCodeGenContext* context)
{
    auto node  = CastAst<AstNode>(context->node, AstNodeKind::IntrinsicLocation);
    auto front = node->childs.front();

    node->resultRegisterRC = reserveRegisterRC(context);
    auto inst              = EMIT_INST1(context, ByteCodeOp::IntrinsicLocationSI, node->resultRegisterRC);
    SWAG_ASSERT(front->resolvedSymbolOverload);
    SWAG_ASSERT(front->resolvedSymbolOverload->node->ownerFct);
    auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(front->resolvedSymbolOverload->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32     = typeFunc->registerIdxToParamIdx(front->resolvedSymbolOverload->storageIndex);
    inst->d.pointer = (uint8_t*) front->resolvedSymbolOverload;

    return true;
}

bool ByteCodeGenJob::emitIntrinsicIsConstExprSI(ByteCodeGenContext* context)
{
    auto node  = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto front = node->childs.front();

    node->resultRegisterRC = reserveRegisterRC(context);
    auto inst              = EMIT_INST1(context, ByteCodeOp::IntrinsicIsConstExprSI, node->resultRegisterRC);
    SWAG_ASSERT(front->resolvedSymbolOverload);
    SWAG_ASSERT(front->resolvedSymbolOverload->node->ownerFct);
    auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(front->resolvedSymbolOverload->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32     = typeFunc->registerIdxToParamIdx(front->resolvedSymbolOverload->storageIndex);
    inst->d.pointer = (uint8_t*) front->resolvedSymbolOverload;

    return true;
}

bool ByteCodeGenJob::emitKindOf(ByteCodeGenContext* context, AstNode* node, TypeInfoKind from)
{
    SWAG_ASSERT(node->resultRegisterRC.size() == 2);
    ensureCanBeChangedRC(context, node->resultRegisterRC);
    auto rc = node->resultRegisterRC[1];
    freeRegisterRC(context, node->resultRegisterRC[0]);
    node->resultRegisterRC = rc;

    // Deref the type from the itable
    if (from == TypeInfoKind::Interface)
    {
        EMIT_INST2(context, ByteCodeOp::JumpIfZero64, node->resultRegisterRC, 2);
        auto inst   = EMIT_INST3(context, ByteCodeOp::DecPointer64, node->resultRegisterRC, 0, node->resultRegisterRC);
        inst->b.u64 = sizeof(void*);
        inst->flags |= BCI_IMM_B;
        EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
    }

    return true;
}

bool ByteCodeGenJob::emitImplicitKindOfAny(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitKindOf(context, context->node, TypeInfoKind::Native));
    return true;
}

bool ByteCodeGenJob::emitImplicitKindOfInterface(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitKindOf(context, context->node, TypeInfoKind::Interface));
    return true;
}

bool ByteCodeGenJob::emitIntrinsicKindOf(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto front    = node->childs.front();
    auto typeInfo = TypeManager::concretePtrRefType(front->typeInfo);
    SWAG_CHECK(emitKindOf(context, front, typeInfo->kind));
    node->resultRegisterRC         = front->resultRegisterRC;
    node->parent->resultRegisterRC = node->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitIntrinsicCountOf(ByteCodeGenContext* context, AstNode* node, AstNode* expr)
{
    auto typeInfo = TypeManager::concretePtrRefType(expr->typeInfo);

    if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result != ContextResult::Done)
            return true;
        return true;
    }

    if (typeInfo->isString() ||
        typeInfo->isSlice() ||
        typeInfo->isVariadic() ||
        typeInfo->isTypedVariadic())
    {
        ensureCanBeChangedRC(context, expr->resultRegisterRC);
        node->resultRegisterRC = expr->resultRegisterRC[1];
        SWAG_ASSERT(expr->resultRegisterRC.size() <= 2);
        freeRegisterRC(context, expr->resultRegisterRC[0]);
        return true;
    }

    return Report::internalError(context->node, "emitIntrinsicCountOf, type not supported");
}

bool ByteCodeGenJob::emitIntrinsicCountOf(ByteCodeGenContext* context)
{
    auto node = context->node;
    auto expr = node->childs.back();
    SWAG_CHECK(emitIntrinsicCountOf(context, node, expr));
    return true;
}

bool ByteCodeGenJob::emitIntrinsicDataOf(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto front    = node->childs.front();
    auto typeInfo = TypeManager::concretePtrRefType(front->typeInfo);

    if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result != ContextResult::Done)
            return true;
        return true;
    }

    if (typeInfo->isString() ||
        typeInfo->isAny() ||
        typeInfo->isSlice() ||
        typeInfo->isInterface() ||
        typeInfo->isArray() ||
        typeInfo->isListArray())
    {
        ensureCanBeChangedRC(context, front->resultRegisterRC);
        truncRegisterRC(context, front->resultRegisterRC, 1);
        node->resultRegisterRC         = front->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        return true;
    }

    return Report::internalError(context->node, "emitIntrinsicDataOf, type not supported");
}

#include "pch.h"
#include "Ast.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "ByteCodeGenContext.h"
#include "Report.h"
#include "Symbol.h"
#include "TypeManager.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicMakeAny(ByteCodeGenContext* context)
{
    const auto node  = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front = node->childs.front();
    const auto back  = node->childs.back();
    reserveRegisterRC(context, node->resultRegisterRc, 2);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[0], front->resultRegisterRc);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[1], back->resultRegisterRc);
    freeRegisterRC(context, front);
    freeRegisterRC(context, back);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicMakeCallback(ByteCodeGenContext* context)
{
    const auto node    = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto ptrNode = node->childs.front();
    EMIT_INST1(context, ByteCodeOp::IntrinsicMakeCallback, ptrNode->resultRegisterRc);
    node->resultRegisterRc = ptrNode->resultRegisterRc;
    return true;
}

bool ByteCodeGen::emitIntrinsicMakeSlice(ByteCodeGenContext* context)
{
    const auto node      = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto ptrNode   = node->childs.front();
    const auto countNode = node->childs.back();

    SWAG_CHECK(emitCast(context, countNode, countNode->typeInfo, countNode->castedTypeInfo));
    reserveRegisterRC(context, node->resultRegisterRc, 2);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[0], ptrNode->resultRegisterRc);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[1], countNode->resultRegisterRc);
    freeRegisterRC(context, ptrNode);
    freeRegisterRC(context, countNode);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicMakeInterface(ByteCodeGenContext* context)
{
    const auto node   = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto params = node->childs.front();

    reserveLinearRegisterRC2(context, node->resultRegisterRc);

    // Reference to the interface concrete type info
    const auto childItf = params->childs[2];
    SWAG_ASSERT(childItf->computedValue);
    SWAG_ASSERT(childItf->computedValue->storageSegment);
    SWAG_ASSERT(childItf->computedValue->storageOffset != UINT32_MAX);

    const auto constSegment = childItf->computedValue->storageSegment;
    const auto r0           = reserveRegisterRC(context);
    emitMakeSegPointer(context, constSegment, childItf->computedValue->storageOffset, r0);

    // Copy object pointer to first result register
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[0], params->childs[0]->resultRegisterRc);

    // Get interface itable pointer in the second result register
    EMIT_INST3(context, ByteCodeOp::IntrinsicItfTableOf, params->childs[1]->resultRegisterRc, r0, node->resultRegisterRc[1]);

    freeRegisterRC(context, params->childs[0]);
    freeRegisterRC(context, params->childs[1]);
    freeRegisterRC(context, params->childs[2]);
    freeRegisterRC(context, r0);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicSpread(ByteCodeGenContext* context)
{
    const auto node     = context->node;
    const auto expr     = node->childs.back();
    const auto typeInfo = TypeManager::concreteType(expr->typeInfo);

    if (typeInfo->isArray())
    {
        transformResultToLinear2(context, expr->resultRegisterRc);
        node->resultRegisterRc = expr->resultRegisterRc;
        const auto typeArr     = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        const auto inst        = EMIT_INST1(context, ByteCodeOp::SetImmediate64, expr->resultRegisterRc[1]);
        inst->b.u64            = typeArr->count;
    }
    else if (typeInfo->isListArray() || typeInfo->isSlice())
    {
        node->resultRegisterRc = expr->resultRegisterRc;
    }
    else
    {
        return Report::internalError(context->node, "emitIntrinsicSpread, type not supported");
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicLocationSI(ByteCodeGenContext* context)
{
    const auto node  = castAst<AstNode>(context->node, AstNodeKind::IntrinsicLocation);
    const auto front = node->childs.front();

    node->resultRegisterRc = reserveRegisterRC(context);
    const auto inst        = EMIT_INST1(context, ByteCodeOp::IntrinsicLocationSI, node->resultRegisterRc);
    SWAG_ASSERT(front->resolvedSymbolOverload);
    SWAG_ASSERT(front->resolvedSymbolOverload->node->ownerFct);
    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(front->resolvedSymbolOverload->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32         = typeFunc->registerIdxToParamIdx(front->resolvedSymbolOverload->storageIndex);
    inst->d.pointer     = (uint8_t*) front->resolvedSymbolOverload;

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicIsConstExprSI(ByteCodeGenContext* context)
{
    const auto node  = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front = node->childs.front();

    node->resultRegisterRc = reserveRegisterRC(context);
    const auto inst        = EMIT_INST1(context, ByteCodeOp::IntrinsicIsConstExprSI, node->resultRegisterRc);
    SWAG_ASSERT(front->resolvedSymbolOverload);
    SWAG_ASSERT(front->resolvedSymbolOverload->node->ownerFct);
    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(front->resolvedSymbolOverload->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32         = typeFunc->registerIdxToParamIdx(front->resolvedSymbolOverload->storageIndex);
    inst->d.pointer     = (uint8_t*) front->resolvedSymbolOverload;

    return true;
}

bool ByteCodeGen::emitKindOf(ByteCodeGenContext* context, AstNode* node, TypeInfoKind from)
{
    SWAG_ASSERT(node->resultRegisterRc.size() == 2);
    ensureCanBeChangedRC(context, node->resultRegisterRc);
    const auto rc = node->resultRegisterRc[1];
    freeRegisterRC(context, node->resultRegisterRc[0]);
    node->resultRegisterRc = rc;

    // Deref the type from the itable
    if (from == TypeInfoKind::Interface)
    {
        EMIT_INST2(context, ByteCodeOp::JumpIfZero64, node->resultRegisterRc, 2);
        const auto inst = EMIT_INST3(context, ByteCodeOp::DecPointer64, node->resultRegisterRc, 0, node->resultRegisterRc);
        inst->b.u64     = sizeof(void*);
        inst->flags |= BCI_IMM_B;
        EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
    }

    return true;
}

bool ByteCodeGen::emitImplicitKindOfAny(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitKindOf(context, context->node, TypeInfoKind::Native));
    return true;
}

bool ByteCodeGen::emitImplicitKindOfInterface(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitKindOf(context, context->node, TypeInfoKind::Interface));
    return true;
}

bool ByteCodeGen::emitIntrinsicKindOf(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front    = node->childs.front();
    const auto typeInfo = TypeManager::concretePtrRefType(front->typeInfo);
    SWAG_CHECK(emitKindOf(context, front, typeInfo->kind));
    node->resultRegisterRc         = front->resultRegisterRc;
    node->parent->resultRegisterRc = node->resultRegisterRc;
    return true;
}

bool ByteCodeGen::emitIntrinsicCountOf(ByteCodeGenContext* context, AstNode* node, AstNode* expr)
{
    const auto typeInfo = TypeManager::concretePtrRefType(expr->typeInfo);

    if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
    {
        SWAG_CHECK(emitUserOp(context));
        YIELD();
        return true;
    }

    if (typeInfo->isString() ||
        typeInfo->isSlice() ||
        typeInfo->isVariadic() ||
        typeInfo->isTypedVariadic())
    {
        ensureCanBeChangedRC(context, expr->resultRegisterRc);
        node->resultRegisterRc = expr->resultRegisterRc[1];
        SWAG_ASSERT(expr->resultRegisterRc.size() <= 2);
        freeRegisterRC(context, expr->resultRegisterRc[0]);
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::U64))
    {
        node->resultRegisterRc = expr->resultRegisterRc;
        return true;
    }

    return Report::internalError(context->node, "emitIntrinsicCountOf, type not supported");
}

bool ByteCodeGen::emitIntrinsicCountOf(ByteCodeGenContext* context)
{
    const auto node = context->node;
    const auto expr = node->childs.back();
    SWAG_CHECK(emitIntrinsicCountOf(context, node, expr));
    return true;
}

bool ByteCodeGen::emitIntrinsicDataOf(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front    = node->childs.front();
    const auto typeInfo = TypeManager::concretePtrRefType(front->typeInfo);

    if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
    {
        SWAG_CHECK(emitUserOp(context));
        YIELD();
        return true;
    }

    if (typeInfo->isString() ||
        typeInfo->isAny() ||
        typeInfo->isSlice() ||
        typeInfo->isInterface() ||
        typeInfo->isArray() ||
        typeInfo->isListArray())
    {
        ensureCanBeChangedRC(context, front->resultRegisterRc);
        truncRegisterRC(context, front->resultRegisterRc, 1);
        node->resultRegisterRc         = front->resultRegisterRc;
        node->parent->resultRegisterRc = node->resultRegisterRc;
        return true;
    }

    return Report::internalError(context->node, "emitIntrinsicDataOf, type not supported");
}

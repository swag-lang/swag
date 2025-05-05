#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Report.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicMakeAny(ByteCodeGenContext* context)
{
    const auto node  = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front = node->firstChild();
    const auto back  = node->lastChild();
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
    const auto ptrNode = node->firstChild();
    EMIT_INST1(context, ByteCodeOp::IntrinsicMakeCallback, ptrNode->resultRegisterRc);
    node->resultRegisterRc = ptrNode->resultRegisterRc;
    return true;
}

bool ByteCodeGen::emitIntrinsicMakeSlice(ByteCodeGenContext* context)
{
    const auto node      = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto ptrNode   = node->firstChild();
    const auto countNode = node->lastChild();

    SWAG_CHECK(emitCast(context, countNode, countNode->typeInfo, countNode->typeInfoCast));
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
    const auto params = node->firstChild();

    reserveLinearRegisterRC2(context, node->resultRegisterRc);

    // Reference to the interface concrete type info
    const auto childItf = params->children[2];
    SWAG_ASSERT(childItf->computedValue());
    SWAG_ASSERT(childItf->computedValue()->storageSegment);
    SWAG_ASSERT(childItf->computedValue()->storageOffset != UINT32_MAX);

    const auto constSegment = childItf->computedValue()->storageSegment;
    const auto r0           = reserveRegisterRC(context);
    emitMakeSegPointer(context, constSegment, childItf->computedValue()->storageOffset, r0);

    // Copy object pointer to first result register
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[0], params->firstChild()->resultRegisterRc);

    // Get interface itable pointer in the second result register
    EMIT_INST3(context, ByteCodeOp::IntrinsicItfTableOf, params->secondChild()->resultRegisterRc, r0, node->resultRegisterRc[1]);

    freeRegisterRC(context, params->firstChild());
    freeRegisterRC(context, params->secondChild());
    freeRegisterRC(context, params->children[2]);
    freeRegisterRC(context, r0);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicLocationSI(ByteCodeGenContext* context)
{
    const auto node  = castAst<AstNode>(context->node, AstNodeKind::CompilerLocation);
    const auto front = node->firstChild();

    node->resultRegisterRc = reserveRegisterRC(context);
    const auto inst        = EMIT_INST1(context, ByteCodeOp::IntrinsicLocationSI, node->resultRegisterRc);
    SWAG_ASSERT(front->resolvedSymbolOverload());
    SWAG_ASSERT(front->resolvedSymbolOverload()->node->ownerFct);
    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(front->resolvedSymbolOverload()->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32         = typeFunc->registerIdxToParamIdx(front->resolvedSymbolOverload()->storageIndex);
    inst->d.pointer     = reinterpret_cast<uint8_t*>(front->resolvedSymbolOverload());

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicIsConstExprSI(ByteCodeGenContext* context)
{
    const auto node  = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front = node->firstChild();

    node->resultRegisterRc = reserveRegisterRC(context);
    const auto inst        = EMIT_INST1(context, ByteCodeOp::IntrinsicIsConstExprSI, node->resultRegisterRc);
    SWAG_ASSERT(front->resolvedSymbolOverload());
    SWAG_ASSERT(front->resolvedSymbolOverload()->node->ownerFct);
    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(front->resolvedSymbolOverload()->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32         = typeFunc->registerIdxToParamIdx(front->resolvedSymbolOverload()->storageIndex);
    inst->d.pointer     = reinterpret_cast<uint8_t*>(front->resolvedSymbolOverload());

    return true;
}

bool ByteCodeGen::emitKindOfAny(const ByteCodeGenContext* context, AstNode* node)
{
    SWAG_ASSERT(node->resultRegisterRc.size() == 2);
    ensureCanBeChangedRC(context, node->resultRegisterRc);
    const auto rc = node->resultRegisterRc[1];
    freeRegisterRC(context, node->resultRegisterRc[0]);
    node->resultRegisterRc = rc;
    return true;
}

bool ByteCodeGen::emitKindOfInterface(ByteCodeGenContext* context, AstNode* node)
{
    SWAG_ASSERT(node->resultRegisterRc.size() == 2);
    ensureCanBeChangedRC(context, node->resultRegisterRc);
    const auto rc = node->resultRegisterRc[1];
    freeRegisterRC(context, node->resultRegisterRc[0]);
    node->resultRegisterRc = rc;
    emitSafetyNullCheck(context, node->resultRegisterRc);

    // Deref the type from the itable
    const auto inst = EMIT_INST3(context, ByteCodeOp::DecPointer64, node->resultRegisterRc, 0, node->resultRegisterRc);
    inst->b.u64     = sizeof(void*);
    inst->addFlag(BCI_IMM_B);
    EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitKindOfAny(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitKindOfAny(context, context->node));
    emitSafetyNullCheck(context, context->node->resultRegisterRc);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitKindOfInterface(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitKindOfInterface(context, context->node));
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicKindOf(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front    = node->firstChild();
    const auto typeInfo = TypeManager::concretePtrRefType(front->typeInfo);

    if (typeInfo->isInterface())
        SWAG_CHECK(emitKindOfInterface(context, front));
    else
    {
        SWAG_ASSERT(typeInfo->isAny());
        SWAG_CHECK(emitKindOfAny(context, front));
    }

    node->resultRegisterRc         = front->resultRegisterRc;
    node->parent->resultRegisterRc = node->resultRegisterRc;
    return true;
}

bool ByteCodeGen::emitIntrinsicCountOf(ByteCodeGenContext* context, AstNode* node, AstNode* expr)
{
    const auto typeInfo = TypeManager::concretePtrRefType(expr->typeInfo);

    if (node->hasExtraPointer(ExtraPointerKind::UserOp))
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
    const auto expr = node->lastChild();
    SWAG_CHECK(emitIntrinsicCountOf(context, node, expr));
    return true;
}

bool ByteCodeGen::emitIntrinsicDataOf(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto front    = node->firstChild();
    const auto typeInfo = TypeManager::concretePtrRefType(front->typeInfo);

    if (node->hasExtraPointer(ExtraPointerKind::UserOp))
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

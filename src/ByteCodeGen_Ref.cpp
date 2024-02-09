#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "ByteCodeGenContext.h"
#include "Report.h"
#include "Symbol.h"
#include "TypeManager.h"

bool ByteCodeGen::emitPointerRef(ByteCodeGenContext* context)
{
    const auto node = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!node->access->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->addSemFlag(SEMFLAG_CAST1);
    }

    // In case of a deref, no need to increment pointer because we are sure that index is 0
    if (!(node->hasSpecFlag(AstArrayPointerIndex::SPECFLAG_IS_DEREF)))
    {
        const auto sizeOf = node->typeInfo->sizeOf;
        if (sizeOf > 1)
        {
            ensureCanBeChangedRC(context, node->access->resultRegisterRc);
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = sizeOf;
        }

        ensureCanBeChangedRC(context, node->array->resultRegisterRc);
        EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
    }

    node->resultRegisterRc         = node->array->resultRegisterRc;
    node->parent->resultRegisterRc = node->resultRegisterRc;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitStringRef(ByteCodeGenContext* context)
{
    const auto node = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!node->access->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->addSemFlag(SEMFLAG_CAST1);
    }

    emitSafetyBoundCheckString(context, node->access->resultRegisterRc, node->array->resultRegisterRc[1]);

    ensureCanBeChangedRC(context, node->array->resultRegisterRc);
    EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
    node->resultRegisterRc = node->array->resultRegisterRc;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitArrayRef(ByteCodeGenContext* context)
{
    const auto node          = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    const auto typeArray     = TypeManager::concreteType(node->array->typeInfo, CONCRETE_FORCE_ALIAS);
    const auto typeInfoArray = castTypeInfo<TypeInfoArray>(typeArray, TypeInfoKind::Array);

    if (!node->access->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->addSemFlag(SEMFLAG_CAST1);
    }

    if (!node->access->hasComputedValue())
        emitSafetyBoundCheckArray(context, node->access->resultRegisterRc, typeInfoArray);

    // Pointer increment
    const auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
    {
        ensureCanBeChangedRC(context, node->access->resultRegisterRc);
        EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = sizeOf;
    }

    ensureCanBeChangedRC(context, node->array->resultRegisterRc);
    EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
    node->resultRegisterRc = node->array->resultRegisterRc;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitSliceRef(ByteCodeGenContext* context)
{
    const auto node = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!node->access->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->addSemFlag(SEMFLAG_CAST1);
    }

    // Slice is already dereferenced ? (from function parameter)
    if (node->array->resultRegisterRc.size() != 2)
    {
        ensureCanBeChangedRC(context, node->array->resultRegisterRc);
        node->array->resultRegisterRc += reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, node->array->resultRegisterRc[0], node->array->resultRegisterRc[1]);
    }

    emitSafetyBoundCheckSlice(context, node->access->resultRegisterRc, node->array->resultRegisterRc[1]);

    // Pointer increment
    const auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
    {
        ensureCanBeChangedRC(context, node->access->resultRegisterRc);
        EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = sizeOf;
    }

    ensureCanBeChangedRC(context, node->array->resultRegisterRc);
    EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
    node->resultRegisterRc = node->array->resultRegisterRc;
    truncRegisterRC(context, node->resultRegisterRc, 1);

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitStructDeRef(ByteCodeGenContext* context, TypeInfo* typeInfo)
{
    const auto node = context->node;

    ensureCanBeChangedRC(context, node->resultRegisterRc);

    if (typeInfo->isInterface() && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !node->hasAstFlag(AST_UFCS_FCT))
    {
        if (node->hasAstFlag(AST_FROM_UFCS)) // Get the ITable pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = sizeof(void*);
        else if (node->hasAstFlag(AST_TO_UFCS)) // Get the structure pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        return true;
    }

    if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !node->hasAstFlag(AST_UFCS_FCT))
    {
        EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        if (node->hasAstFlag(AST_FROM_UFCS)) // Get the ITable pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = sizeof(void*);
        else if (node->hasAstFlag(AST_TO_UFCS)) // Get the structure pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        return true;
    }

    if (typeInfo->isSlice())
    {
        transformResultToLinear2(context, node->resultRegisterRc);
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRc[0], node->resultRegisterRc[1]);
        return true;
    }

    if (typeInfo->isInterface())
    {
        transformResultToLinear2(context, node->resultRegisterRc);
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRc[0], node->resultRegisterRc[1]);
        return true;
    }

    if (typeInfo->isStruct() || typeInfo->isArray())
    {
        return true;
    }

    if (typeInfo->isClosure())
    {
        truncRegisterRC(context, node->resultRegisterRc, 1);
        return true;
    }

    if (typeInfo->isFuncAttr())
    {
        truncRegisterRC(context, node->resultRegisterRc, 1);
        const auto inst = EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        inst->d.pointer = (uint8_t*) node->resolvedSymbolOverload;
        return true;
    }

    if (typeInfo->isPointer())
    {
        truncRegisterRC(context, node->resultRegisterRc, 1);
        const auto inst = EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        inst->d.pointer = (uint8_t*) node->resolvedSymbolOverload;
        return true;
    }

    SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRc, typeInfo));
    return true;
}

bool ByteCodeGen::emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo)
{
    ensureCanBeChangedRC(context, r0);

    typeInfo = TypeManager::concretePtrRefType(typeInfo, CONCRETE_FORCE_ALIAS);

    if (typeInfo->numRegisters() == 2)
    {
        transformResultToLinear2(context, r0);
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, r0[0], r0[1]);
        return true;
    }

    if (typeInfo->isListTuple() ||
        typeInfo->isListArray() ||
        typeInfo->isStruct() ||
        typeInfo->isArray() ||
        typeInfo->isClosure())
    {
        return true;
    }

    // We now only need one register
    truncRegisterRC(context, r0, 1);

    // Register list can be stored in the parent, so we need to update it, otherwise we
    // will free one register twice
    context->node->parent->resultRegisterRc = r0;

    switch (typeInfo->sizeOf)
    {
    case 1:
        EMIT_INST2(context, ByteCodeOp::DeRef8, r0, r0);
        break;
    case 2:
        EMIT_INST2(context, ByteCodeOp::DeRef16, r0, r0);
        break;
    case 4:
        EMIT_INST2(context, ByteCodeOp::DeRef32, r0, r0);
        break;
    case 8:
        EMIT_INST2(context, ByteCodeOp::DeRef64, r0, r0);
        break;
    default:
        return Report::internalError(context->node, "emitTypeDeRef, size not supported");
    }

    SWAG_CHECK(emitSafetyValue(context, r0, typeInfo));
    return true;
}

bool ByteCodeGen::emitPointerDeRef(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    const auto typeInfo = TypeManager::concretePtrRefType(node->array->typeInfo);
    const auto castInfo = node->array->castedTypeInfo ? node->array->castedTypeInfo : nullptr;

    if (!node->access->hasSemFlag(SEMFLAG_CAST3))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->addSemFlag(SEMFLAG_CAST3);
    }

    // Dereference of a string constant
    if (typeInfo->isString())
    {
        emitSafetyBoundCheckString(context, node->access->resultRegisterRc, node->array->resultRegisterRc[1]);

        ensureCanBeChangedRC(context, node->array->resultRegisterRc);
        EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
        EMIT_INST2(context, ByteCodeOp::DeRef8, node->array->resultRegisterRc, node->array->resultRegisterRc);
        node->resultRegisterRc         = node->array->resultRegisterRc;
        node->parent->resultRegisterRc = node->resultRegisterRc;
        freeRegisterRC(context, node->access);
        ensureCanBeChangedRC(context, node->resultRegisterRc);
        truncRegisterRC(context, node->resultRegisterRc, 1);
    }

    // Dereference of a slice
    else if (typeInfo->isSlice())
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRc, node->array->resultRegisterRc[1]);

        const auto typeInfoSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            const auto sizeOf = typeInfoSlice->pointedType->sizeOf;
            if (sizeOf > 1)
            {
                ensureCanBeChangedRC(context, node->access->resultRegisterRc);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = sizeOf;
            }

            ensureCanBeChangedRC(context, node->array->resultRegisterRc);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
        }

        if (typeInfoSlice->pointedType->isString())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, typeInfoSlice->pointedType));
        else if (!(node->isForceTakeAddress()) || typeInfoSlice->pointedType->isPointer())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, typeInfoSlice->pointedType));

        node->resultRegisterRc         = node->array->resultRegisterRc;
        node->parent->resultRegisterRc = node->resultRegisterRc;
        freeRegisterRC(context, node->access);
    }

    // Dereference a struct
    else if (typeInfo->isStruct() ||
             (typeInfo->isPointerTo(TypeInfoKind::Struct) && castInfo && castInfo->isStruct()) ||
             node->hasSemFlag(SEMFLAG_FORCE_CAST_PTR_STRUCT))
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            context->allocateTempCallParams();
            context->allParamsTmp->childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, context->allParamsTmp));
            if (context->result != ContextResult::Done)
            {
                node->addSemFlag(SEMFLAG_FORCE_CAST_PTR_STRUCT);
                return true;
            }

            if (node->hasSemFlag(SEMFLAG_FROM_PTR_REF) && !node->hasSemFlag(SEMFLAG_FROM_REF))
            {
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
            }
        }
    }

    // Dereference of a pointer
    else if (typeInfo->isPointer())
    {
        const auto typeInfoPointer = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            const auto sizeOf = typeInfoPointer->pointedType->sizeOf;
            if (sizeOf > 1)
            {
                ensureCanBeChangedRC(context, node->access->resultRegisterRc);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = sizeOf;
            }
            ensureCanBeChangedRC(context, node->array->resultRegisterRc);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
        }

        if (typeInfoPointer->pointedType->isString())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, typeInfoPointer->pointedType));
        else if (!node->isForceTakeAddress())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, typeInfoPointer->pointedType));

        node->resultRegisterRc         = node->array->resultRegisterRc;
        node->parent->resultRegisterRc = node->resultRegisterRc;
        freeRegisterRC(context, node->access);
    }

    // Dereference of an array
    else if (typeInfo->isArray())
    {
        const auto typeInfoArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);

        if (!node->access->hasComputedValue())
            emitSafetyBoundCheckArray(context, node->access->resultRegisterRc, typeInfoArray);
        truncRegisterRC(context, node->array->resultRegisterRc, 1);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            const auto sizeOf = typeInfoArray->pointedType->sizeOf;
            if (sizeOf > 1)
            {
                ensureCanBeChangedRC(context, node->access->resultRegisterRc);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = sizeOf;
            }
            ensureCanBeChangedRC(context, node->array->resultRegisterRc);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
        }

        const auto pointedType = TypeManager::concreteType(typeInfoArray->pointedType, CONCRETE_FORCE_ALIAS);

        if (pointedType->isString())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, pointedType));
        else if (pointedType->isPointer())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, pointedType));
        else if (!node->isForceTakeAddress() && !pointedType->isArray())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, pointedType));

        node->resultRegisterRc         = node->array->resultRegisterRc;
        node->parent->resultRegisterRc = node->resultRegisterRc;
        freeRegisterRC(context, node->access);
    }

    // Dereference a variadic parameter
    else if (typeInfo->isVariadic())
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRc, node->array->resultRegisterRc[1]);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            ensureCanBeChangedRC(context, node->access->resultRegisterRc);
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = 2 * sizeof(Register); // 2 is sizeof(any)
            ensureCanBeChangedRC(context, node->array->resultRegisterRc);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
        }

        SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, g_TypeMgr->typeInfoAny));
        node->resultRegisterRc         = node->array->resultRegisterRc;
        node->parent->resultRegisterRc = node->resultRegisterRc;
        freeRegisterRC(context, node->access);
    }

    // Dereference a typed variadic parameter
    else if (typeInfo->isTypedVariadic())
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRc, node->array->resultRegisterRc[1]);

        const auto rawType = ((TypeInfoVariadic*) typeInfo)->rawType;

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            ensureCanBeChangedRC(context, node->access->resultRegisterRc);
            if (rawType->isNative() && rawType->sizeOf < sizeof(Register))
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = rawType->sizeOf;
            else
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRc)->b.u64 = rawType->numRegisters() * sizeof(Register);
            ensureCanBeChangedRC(context, node->array->resultRegisterRc);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRc, node->access->resultRegisterRc, node->array->resultRegisterRc);
        }

        SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRc, rawType));
        node->resultRegisterRc         = node->array->resultRegisterRc;
        node->parent->resultRegisterRc = node->resultRegisterRc;
        freeRegisterRC(context, node->access);
    }
    else
    {
        return Report::internalError(context->node, "emitPointerDeRef, type not supported");
    }

    return true;
}

bool ByteCodeGen::emitMakeLambda(ByteCodeGenContext* context)
{
    const auto node = castAst<AstMakePointer>(context->node, AstNodeKind::MakePointerLambda, AstNodeKind::MakePointer);

    AstNode* front;
    if (node->lambda && node->lambda->captureParameters)
        front = node->childs[1];
    else
        front = node->childs.front();

    const auto funcNode = castAst<AstFuncDecl>(front->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);

    if (!funcNode->isForeign())
    {
        // Need to generate bytecode, if not already done or running
        askForByteCode(context->baseJob, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        YIELD();
    }

    freeRegisterRC(context, front);
    node->resultRegisterRc = reserveRegisterRC(context);

    const auto inst = EMIT_INST1(context, ByteCodeOp::MakeLambda, node->resultRegisterRc);
    inst->b.pointer = (uint8_t*) funcNode;
    inst->c.pointer = nullptr;
    if (funcNode->hasExtByteCode() && funcNode->extByteCode()->bc)
    {
        inst->c.pointer                        = (uint8_t*) funcNode->extByteCode()->bc;
        funcNode->extByteCode()->bc->isUsed    = true;
        funcNode->extByteCode()->bc->forceEmit = true;
    }

    // :CaptureBlock
    // Block capture
    if (node->typeInfo->isClosure())
    {
        node->resultRegisterRc += node->childs.back()->resultRegisterRc[0];
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitMakePointer(ByteCodeGenContext* context)
{
    const auto node        = context->node;
    const auto front       = node->childs.front();
    node->resultRegisterRc = front->resultRegisterRc;
    return true;
}

bool ByteCodeGen::emitMakeArrayPointerSlicingUpperBound(ByteCodeGenContext* context)
{
    const auto upperNode = context->node;
    const auto slicing   = castAst<AstArrayPointerSlicing>(context->node->parent, AstNodeKind::ArrayPointerSlicing);
    const auto arrayNode = slicing->array;

    if (upperNode->hasExtMisc() && upperNode->extMisc()->resolvedUserOpSymbolOverload)
    {
        context->allocateTempCallParams();
        context->allParamsTmp->childs.push_back(arrayNode);
        SWAG_CHECK(emitUserOp(context, context->allParamsTmp, nullptr, false));
        YIELD();
        EMIT_INST1(context, ByteCodeOp::Add64byVB64, upperNode->resultRegisterRc)->b.s64 = -1;
        return true;
    }

    const auto typeInfo = TypeManager::concretePtrRefType(arrayNode->typeInfo);
    if (typeInfo->isString() ||
        typeInfo->isSlice() ||
        typeInfo->isVariadic() ||
        typeInfo->isTypedVariadic())
    {
        upperNode->resultRegisterRc = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, upperNode->resultRegisterRc, arrayNode->resultRegisterRc[1]);
        EMIT_INST1(context, ByteCodeOp::Add64byVB64, upperNode->resultRegisterRc)->b.s64 = -1;
        return true;
    }

    return Report::internalError(context->node, "emitMakeArrayPointerSlicingUpperBound, type not supported");
}

bool ByteCodeGen::emitMakeArrayPointerSlicing(ByteCodeGenContext* context)
{
    const auto node    = castAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    const auto typeVar = TypeManager::concretePtrRefType(node->array->typeInfo);

    if (!node->lowerBound->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->lowerBound, node->lowerBound->typeInfo, node->lowerBound->castedTypeInfo));
        YIELD();
        node->lowerBound->addSemFlag(SEMFLAG_CAST1);
    }

    if (!node->upperBound->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->upperBound, node->upperBound->typeInfo, node->upperBound->castedTypeInfo));
        YIELD();
        node->upperBound->addSemFlag(SEMFLAG_CAST1);
    }

    // Exclude upper bound limit
    if (node->hasSpecFlag(AstArrayPointerSlicing::SPECFLAG_EXCLUDE_UP) && !node->upperBound->hasComputedValue())
    {
        ensureCanBeChangedRC(context, node->upperBound->resultRegisterRc);
        EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->upperBound->resultRegisterRc)->b.s64 = -1;
    }

    // Slicing of a structure, with a special function
    if (typeVar->isStruct())
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            context->allocateTempCallParams();
            context->allParamsTmp->childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, context->allParamsTmp));
            YIELD();
            return true;
        }

        return Report::internalError(context->node, "emitMakeArrayPointerSlicing, type not supported");
    }

    uint64_t sizeOf;
    if (typeVar->isArray())
        sizeOf = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->finalType->sizeOf;
    else if (typeVar->isString())
        sizeOf = 1;
    else if (typeVar->isSlice())
        sizeOf = castTypeInfo<TypeInfoSlice>(typeVar, TypeInfoKind::Slice)->pointedType->sizeOf;
    else if (typeVar->isPointer())
        sizeOf = castTypeInfo<TypeInfoPointer>(typeVar, TypeInfoKind::Pointer)->pointedType->sizeOf;
    else
        return Report::internalError(context->node, "emitMakeArrayPointerSlicing, type not supported");

    emitSafetyArrayPointerSlicing(context, node);

    RegisterList r0;
    reserveLinearRegisterRC2(context, r0);

    // Compute size of slice
    ensureCanBeChangedRC(context, node->upperBound->resultRegisterRc);
    if (node->lowerBound)
        EMIT_INST3(context, ByteCodeOp::BinOpMinusS64, node->upperBound->resultRegisterRc, node->lowerBound->resultRegisterRc, node->upperBound->resultRegisterRc);
    EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->upperBound->resultRegisterRc)->b.u64 = 1;
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0[1], node->upperBound->resultRegisterRc);

    // Increment start pointer
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0[0], node->array->resultRegisterRc);
    if (node->lowerBound)
    {
        if (sizeOf > 1)
        {
            ensureCanBeChangedRC(context, node->lowerBound->resultRegisterRc);
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->lowerBound->resultRegisterRc)->b.u64 = sizeOf;
        }
        EMIT_INST3(context, ByteCodeOp::IncPointer64, r0[0], node->lowerBound->resultRegisterRc, r0[0]);
    }

    freeRegisterRC(context, node->array);
    freeRegisterRC(context, node->lowerBound);
    freeRegisterRC(context, node->upperBound);
    node->resultRegisterRc = r0;

    return true;
}

void ByteCodeGen::emitSetZeroAtPointer(const ByteCodeGenContext* context, uint64_t sizeOf, uint32_t registerIndex)
{
    switch (sizeOf)
    {
    case 1:
        EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer8, registerIndex);
        break;
    case 2:
        EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer16, registerIndex);
        break;
    case 4:
        EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer32, registerIndex);
        break;
    case 8:
        EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, registerIndex);
        break;
    default:
        EMIT_INST1(context, ByteCodeOp::SetZeroAtPointerX, registerIndex)->b.u64 = sizeOf;
        break;
    }
}

void ByteCodeGen::emitSetZeroStack(const ByteCodeGenContext* context, uint32_t offset, uint32_t sizeOf)
{
    switch (sizeOf)
    {
    case 1:
        EMIT_INST1(context, ByteCodeOp::SetZeroStack8, offset);
        break;
    case 2:
        EMIT_INST1(context, ByteCodeOp::SetZeroStack16, offset);
        break;
    case 4:
        EMIT_INST1(context, ByteCodeOp::SetZeroStack32, offset);
        break;
    case 8:
        EMIT_INST1(context, ByteCodeOp::SetZeroStack64, offset);
        break;
    default:
        EMIT_INST1(context, ByteCodeOp::SetZeroStackX, offset)->b.u64 = sizeOf;
        break;
    }
}

void ByteCodeGen::emitMemCpy(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint64_t sizeOf)
{
    switch (sizeOf)
    {
    case 1:
        EMIT_INST2(context, ByteCodeOp::MemCpy8, r0, r1);
        break;
    case 2:
        EMIT_INST2(context, ByteCodeOp::MemCpy16, r0, r1);
        break;
    case 4:
        EMIT_INST2(context, ByteCodeOp::MemCpy32, r0, r1);
        break;
    case 8:
        EMIT_INST2(context, ByteCodeOp::MemCpy64, r0, r1);
        break;
    default:
    {
        const auto inst = EMIT_INST2(context, ByteCodeOp::IntrinsicMemCpy, r0, r1);
        inst->flags |= BCI_IMM_C;
        inst->c.u64 = sizeOf;
        break;
    }
    }
}

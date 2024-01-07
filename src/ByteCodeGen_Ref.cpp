#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Report.h"

bool ByteCodeGen::emitPointerRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!(node->access->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->semFlags |= SEMFLAG_CAST1;
    }

    // In case of a deref, no need to increment pointer because we are sure that index is 0
    if (!(node->specFlags & AstArrayPointerIndex::SPECFLAG_IS_DREF))
    {
        auto sizeOf = node->typeInfo->sizeOf;
        if (sizeOf > 1)
        {
            ensureCanBeChangedRC(context, node->access->resultRegisterRC);
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
        }

        ensureCanBeChangedRC(context, node->array->resultRegisterRC);
        EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    }

    node->resultRegisterRC         = node->array->resultRegisterRC;
    node->parent->resultRegisterRC = node->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitStringRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!(node->access->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->semFlags |= SEMFLAG_CAST1;
    }

    emitSafetyBoundCheckString(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

    ensureCanBeChangedRC(context, node->array->resultRegisterRC);
    EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitArrayRef(ByteCodeGenContext* context)
{
    auto node          = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto typeArray     = TypeManager::concreteType(node->array->typeInfo, CONCRETE_FORCEALIAS);
    auto typeInfoArray = CastTypeInfo<TypeInfoArray>(typeArray, TypeInfoKind::Array);

    if (!(node->access->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->semFlags |= SEMFLAG_CAST1;
    }

    if (!node->access->hasComputedValue())
        emitSafetyBoundCheckArray(context, node->access->resultRegisterRC, typeInfoArray);

    // Pointer increment
    auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
    {
        ensureCanBeChangedRC(context, node->access->resultRegisterRC);
        EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
    }

    ensureCanBeChangedRC(context, node->array->resultRegisterRC);
    EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitSliceRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!(node->access->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->semFlags |= SEMFLAG_CAST1;
    }

    // Slice is already dereferenced ? (from function parameter)
    if (node->array->resultRegisterRC.size() != 2)
    {
        ensureCanBeChangedRC(context, node->array->resultRegisterRC);
        node->array->resultRegisterRC += reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);
    }

    emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

    // Pointer increment
    auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
    {
        ensureCanBeChangedRC(context, node->access->resultRegisterRC);
        EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
    }

    ensureCanBeChangedRC(context, node->array->resultRegisterRC);
    EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;
    truncRegisterRC(context, node->resultRegisterRC, 1);

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGen::emitStructDeRef(ByteCodeGenContext* context, TypeInfo* typeInfo)
{
    auto node = context->node;

    ensureCanBeChangedRC(context, node->resultRegisterRC);

    if (typeInfo->isInterface() && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
    {
        if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
        else if (node->flags & AST_TO_UFCS) // Get the structure pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
    {
        EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
        else if (node->flags & AST_TO_UFCS) // Get the structure pointer
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->isSlice())
    {
        transformResultToLinear2(context, node);
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    if (typeInfo->isInterface())
    {
        transformResultToLinear2(context, node);
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    if (typeInfo->isStruct() || typeInfo->isArray())
    {
        return true;
    }

    if (typeInfo->isClosure())
    {
        truncRegisterRC(context, node->resultRegisterRC, 1);
        return true;
    }

    if (typeInfo->isFuncAttr())
    {
        truncRegisterRC(context, node->resultRegisterRC, 1);
        auto inst       = EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        inst->d.pointer = (uint8_t*) node->resolvedSymbolOverload;
        return true;
    }

    if (typeInfo->isPointer())
    {
        truncRegisterRC(context, node->resultRegisterRC, 1);
        auto inst       = EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        inst->d.pointer = (uint8_t*) node->resolvedSymbolOverload;
        return true;
    }

    SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, typeInfo));
    return true;
}

bool ByteCodeGen::emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo)
{
    ensureCanBeChangedRC(context, r0);

    typeInfo = TypeManager::concretePtrRefType(typeInfo, CONCRETE_FORCEALIAS);

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
    context->node->parent->resultRegisterRC = r0;

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
    auto node     = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto typeInfo = TypeManager::concretePtrRefType(node->array->typeInfo);
    auto castInfo = node->array->castedTypeInfo ? node->array->castedTypeInfo : nullptr;

    if (!(node->access->semFlags & SEMFLAG_CAST3))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        YIELD();
        node->access->semFlags |= SEMFLAG_CAST3;
    }

    // Dereference of a string constant
    if (typeInfo->isString())
    {
        emitSafetyBoundCheckString(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        ensureCanBeChangedRC(context, node->array->resultRegisterRC);
        EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        EMIT_INST2(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC, node->array->resultRegisterRC);
        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        freeRegisterRC(context, node->access);
        ensureCanBeChangedRC(context, node->resultRegisterRC);
        truncRegisterRC(context, node->resultRegisterRC, 1);
    }

    // Dereference of a slice
    else if (typeInfo->isSlice())
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        auto typeInfoSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            auto sizeOf = typeInfoSlice->pointedType->sizeOf;
            if (sizeOf > 1)
            {
                ensureCanBeChangedRC(context, node->access->resultRegisterRC);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            }

            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfoSlice->pointedType->isString())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoSlice->pointedType));
        else if (!(node->forceTakeAddress()) || typeInfoSlice->pointedType->isPointer())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoSlice->pointedType));

        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference a struct
    else if (typeInfo->isStruct() ||
             (typeInfo->isPointerTo(TypeInfoKind::Struct) && castInfo && castInfo->isStruct()) ||
             node->semFlags & SEMFLAG_FORCE_CAST_PTR_STRUCT)
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            if (!context->allParamsTmp)
                context->allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
            context->allParamsTmp->childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, context->allParamsTmp));
            if (context->result != ContextResult::Done)
            {
                node->semFlags |= SEMFLAG_FORCE_CAST_PTR_STRUCT;
                return true;
            }
        }
    }

    // Dereference of a pointer
    else if (typeInfo->isPointer())
    {
        auto typeInfoPointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            auto sizeOf = typeInfoPointer->pointedType->sizeOf;
            if (sizeOf > 1)
            {
                ensureCanBeChangedRC(context, node->access->resultRegisterRC);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            }
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfoPointer->pointedType->isString())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoPointer->pointedType));
        else if (!(node->forceTakeAddress()))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoPointer->pointedType));

        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference of an array
    else if (typeInfo->isArray())
    {
        auto typeInfoArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);

        if (!node->access->hasComputedValue())
            emitSafetyBoundCheckArray(context, node->access->resultRegisterRC, typeInfoArray);
        truncRegisterRC(context, node->array->resultRegisterRC, 1);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            auto sizeOf = typeInfoArray->pointedType->sizeOf;
            if (sizeOf > 1)
            {
                ensureCanBeChangedRC(context, node->access->resultRegisterRC);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            }
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        auto pointedType = TypeManager::concreteType(typeInfoArray->pointedType, CONCRETE_FORCEALIAS);

        if (pointedType->isString())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, pointedType));
        else if (pointedType->isPointer())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, pointedType));
        else if (!node->forceTakeAddress() && !pointedType->isArray())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, pointedType));

        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference a variadic parameter
    else if (typeInfo->isVariadic())
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            ensureCanBeChangedRC(context, node->access->resultRegisterRC);
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = 2 * sizeof(Register); // 2 is sizeof(any)
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, g_TypeMgr->typeInfoAny));
        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference a typed variadic parameter
    else if (typeInfo->isTypedVariadic())
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        auto rawType = ((TypeInfoVariadic*) typeInfo)->rawType;

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            ensureCanBeChangedRC(context, node->access->resultRegisterRC);
            if (rawType->isNative() && rawType->sizeOf < sizeof(Register))
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = rawType->sizeOf;
            else
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = rawType->numRegisters() * sizeof(Register);
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            EMIT_INST3(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, rawType));
        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
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
    auto node = CastAst<AstMakePointer>(context->node, AstNodeKind::MakePointerLambda, AstNodeKind::MakePointer);

    AstNode* front;
    if (node->lambda && node->lambda->captureParameters)
        front = node->childs[1];
    else
        front = node->childs.front();

    auto funcNode = CastAst<AstFuncDecl>(front->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);

    if (!funcNode->isForeign())
    {
        // Need to generate bytecode, if not already done or running
        askForByteCode(context->baseJob, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        YIELD();
    }

    freeRegisterRC(context, front);
    node->resultRegisterRC = reserveRegisterRC(context);

    auto inst       = EMIT_INST1(context, ByteCodeOp::MakeLambda, node->resultRegisterRC);
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
        node->resultRegisterRC += node->childs.back()->resultRegisterRC[0];
    }

    return true;
}

bool ByteCodeGen::emitMakePointer(ByteCodeGenContext* context)
{
    auto node              = context->node;
    auto front             = node->childs.front();
    node->resultRegisterRC = front->resultRegisterRC;
    return true;
}

bool ByteCodeGen::emitMakeArrayPointerSlicingUpperBound(ByteCodeGenContext* context)
{
    auto upperNode = context->node;
    auto slicing   = CastAst<AstArrayPointerSlicing>(context->node->parent, AstNodeKind::ArrayPointerSlicing);
    auto arrayNode = slicing->array;

    if (upperNode->hasExtMisc() && upperNode->extMisc()->resolvedUserOpSymbolOverload)
    {
        context->allocateTempCallParams();
        context->allParamsTmp->childs.push_back(arrayNode);
        SWAG_CHECK(emitUserOp(context, context->allParamsTmp, nullptr, false));
        YIELD();
        EMIT_INST1(context, ByteCodeOp::Add64byVB64, upperNode->resultRegisterRC)->b.s64 = -1;
        return true;
    }

    auto typeInfo = TypeManager::concretePtrRefType(arrayNode->typeInfo);
    if (typeInfo->isString() ||
        typeInfo->isSlice() ||
        typeInfo->isVariadic() ||
        typeInfo->isTypedVariadic())
    {
        upperNode->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, upperNode->resultRegisterRC, arrayNode->resultRegisterRC[1]);
        EMIT_INST1(context, ByteCodeOp::Add64byVB64, upperNode->resultRegisterRC)->b.s64 = -1;
        return true;
    }

    return Report::internalError(context->node, "emitMakeArrayPointerSlicingUpperBound, type not supported");
}

bool ByteCodeGen::emitMakeArrayPointerSlicing(ByteCodeGenContext* context)
{
    auto node    = CastAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto typeVar = TypeManager::concretePtrRefType(node->array->typeInfo);

    if (!(node->lowerBound->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->lowerBound, node->lowerBound->typeInfo, node->lowerBound->castedTypeInfo));
        YIELD();
        node->lowerBound->semFlags |= SEMFLAG_CAST1;
    }

    if (!(node->upperBound->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->upperBound, node->upperBound->typeInfo, node->upperBound->castedTypeInfo));
        YIELD();
        node->upperBound->semFlags |= SEMFLAG_CAST1;
    }

    // Exclude upper bound limit
    if (node->specFlags & AstArrayPointerSlicing::SPECFLAG_EXCLUDE_UP && !node->upperBound->hasComputedValue())
    {
        ensureCanBeChangedRC(context, node->upperBound->resultRegisterRC);
        EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->upperBound->resultRegisterRC)->b.s64 = -1;
    }

    // Slicing of a structure, with a special function
    if (typeVar->isStruct())
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            if (!context->allParamsTmp)
                context->allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
            context->allParamsTmp->childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, context->allParamsTmp));
            YIELD();
            return true;
        }

        return Report::internalError(context->node, "emitMakeArrayPointerSlicing, type not supported");
    }

    uint64_t sizeOf = 1;
    if (typeVar->isArray())
        sizeOf = CastTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->finalType->sizeOf;
    else if (typeVar->isString())
        sizeOf = 1;
    else if (typeVar->isSlice())
        sizeOf = CastTypeInfo<TypeInfoSlice>(typeVar, TypeInfoKind::Slice)->pointedType->sizeOf;
    else if (typeVar->isPointer())
        sizeOf = CastTypeInfo<TypeInfoPointer>(typeVar, TypeInfoKind::Pointer)->pointedType->sizeOf;
    else
        return Report::internalError(context->node, "emitMakeArrayPointerSlicing, type not supported");

    emitSafetyArrayPointerSlicing(context, node);

    RegisterList r0;
    reserveLinearRegisterRC2(context, r0);

    // Compute size of slice
    ensureCanBeChangedRC(context, node->upperBound->resultRegisterRC);
    if (node->lowerBound)
        EMIT_INST3(context, ByteCodeOp::BinOpMinusS64, node->upperBound->resultRegisterRC, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC);
    EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->upperBound->resultRegisterRC)->b.u64 = 1;
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0[1], node->upperBound->resultRegisterRC);

    // Increment start pointer
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0[0], node->array->resultRegisterRC);
    if (node->lowerBound)
    {
        if (sizeOf > 1)
        {
            ensureCanBeChangedRC(context, node->lowerBound->resultRegisterRC);
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, node->lowerBound->resultRegisterRC)->b.u64 = sizeOf;
        }
        EMIT_INST3(context, ByteCodeOp::IncPointer64, r0[0], node->lowerBound->resultRegisterRC, r0[0]);
    }

    freeRegisterRC(context, node->array);
    freeRegisterRC(context, node->lowerBound);
    freeRegisterRC(context, node->upperBound);
    node->resultRegisterRC = r0;

    return true;
}

void ByteCodeGen::emitSetZeroAtPointer(ByteCodeGenContext* context, uint64_t sizeOf, uint32_t registerIndex)
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

void ByteCodeGen::emitSetZeroStack(ByteCodeGenContext* context, uint32_t offset, uint32_t sizeOf)
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

void ByteCodeGen::emitMemCpy(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint64_t sizeOf)
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
        auto inst = EMIT_INST2(context, ByteCodeOp::IntrinsicMemCpy, r0, r1);
        inst->flags |= BCI_IMM_C;
        inst->c.u64 = sizeOf;
        break;
    }
    }
}
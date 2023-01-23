#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"

bool ByteCodeGenJob::emitPointerRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!(node->access->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    // In case of a deref, no need to increment pointer because we are sure that index is 0
    if (!(node->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF))
    {
        auto sizeOf = node->typeInfo->sizeOf;
        if (sizeOf > 1)
        {
            ensureCanBeChangedRC(context, node->access->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
        }

        ensureCanBeChangedRC(context, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    }

    node->resultRegisterRC         = node->array->resultRegisterRC;
    node->parent->resultRegisterRC = node->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitStringRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!(node->access->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    emitSafetyBoundCheckString(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

    ensureCanBeChangedRC(context, node->array->resultRegisterRC);
    emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitArrayRef(ByteCodeGenContext* context)
{
    auto node          = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto typeArray     = TypeManager::concreteType(node->array->typeInfo, CONCRETE_ALIAS);
    auto typeInfoArray = CastTypeInfo<TypeInfoArray>(typeArray, TypeInfoKind::Array);

    if (!(node->access->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    if (!(node->access->flags & AST_VALUE_COMPUTED))
        emitSafetyBoundCheckArray(context, node->access->resultRegisterRC, typeInfoArray);

    // Pointer increment
    auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
    {
        ensureCanBeChangedRC(context, node->access->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
    }

    ensureCanBeChangedRC(context, node->array->resultRegisterRC);
    emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitSliceRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (!(node->access->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    // Slice is already dereferenced ? (from function parameter)
    if (node->array->resultRegisterRC.size() != 2)
    {
        node->array->resultRegisterRC += reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);
    }

    emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

    // Pointer increment
    auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
    {
        ensureCanBeChangedRC(context, node->access->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
    }

    ensureCanBeChangedRC(context, node->array->resultRegisterRC);
    emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitStructDeRef(ByteCodeGenContext* context, TypeInfo* typeInfo)
{
    auto node = context->node;

    ensureCanBeChangedRC(context, node->resultRegisterRC);

    if (typeInfo->isInterface() && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
    {
        if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
        else if (node->flags & AST_TO_UFCS) // Get the structure pointer
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
    {
        emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
        else if (node->flags & AST_TO_UFCS) // Get the structure pointer
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->isSlice())
    {
        transformResultToLinear2(context, node);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    if (typeInfo->isInterface())
    {
        transformResultToLinear2(context, node);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
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
        emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->isPointer())
    {
        truncRegisterRC(context, node->resultRegisterRC, 1);
        emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, typeInfo));
    return true;
}

bool ByteCodeGenJob::emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo)
{
    ensureCanBeChangedRC(context, r0);

    typeInfo = TypeManager::concretePtrRefType(typeInfo, CONCRETE_ALIAS);

    if (typeInfo->numRegisters() == 2)
    {
        transformResultToLinear2(context, r0);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, r0[0], r0[1]);
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
        emitInstruction(context, ByteCodeOp::DeRef8, r0, r0);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::DeRef16, r0, r0);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::DeRef32, r0, r0);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::DeRef64, r0, r0);
        break;
    default:
        return Report::internalError(context->node, "emitTypeDeRef, size not supported");
    }

    SWAG_CHECK(emitSafetyValue(context, r0, typeInfo));
    return true;
}

bool ByteCodeGenJob::emitPointerDeRef(ByteCodeGenContext* context)
{
    auto job      = context->job;
    auto node     = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto typeInfo = TypeManager::concretePtrRefType(node->array->typeInfo);
    auto castInfo = node->array->castedTypeInfo ? node->array->castedTypeInfo : nullptr;

    if (!(node->access->doneFlags & AST_DONE_CAST3))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->access->doneFlags |= AST_DONE_CAST3;
    }

    // Dereference of a string constant
    if (typeInfo->isString())
    {
        emitSafetyBoundCheckString(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        ensureCanBeChangedRC(context, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC, node->array->resultRegisterRC);
        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        freeRegisterRC(context, node->access);
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
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            }

            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
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
             node->doneFlags & AST_DONE_FORCE_CAST_PTR_STRUCT)
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            if (!job->allParamsTmp)
                job->allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
            job->allParamsTmp->childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, job->allParamsTmp));
            if (context->result != ContextResult::Done)
            {
                node->doneFlags |= AST_DONE_FORCE_CAST_PTR_STRUCT;
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
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            }
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
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

        if (!(node->access->flags & AST_VALUE_COMPUTED))
            emitSafetyBoundCheckArray(context, node->access->resultRegisterRC, typeInfoArray);
        truncRegisterRC(context, node->array->resultRegisterRC, 1);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            auto sizeOf = typeInfoArray->pointedType->sizeOf;
            if (sizeOf > 1)
            {
                ensureCanBeChangedRC(context, node->access->resultRegisterRC);
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            }
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfoArray->pointedType->isString())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoArray->pointedType));
        else if (typeInfoArray->pointedType->isPointer())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoArray->pointedType));
        else if (!node->forceTakeAddress() && !typeInfoArray->pointedType->isArray())
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoArray->pointedType));

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
            emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = 2 * sizeof(Register); // 2 is sizeof(any)
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
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
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = rawType->sizeOf;
            else
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = rawType->numRegisters() * sizeof(Register);
            ensureCanBeChangedRC(context, node->array->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
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

bool ByteCodeGenJob::emitMakeLambda(ByteCodeGenContext* context)
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
        askForByteCode(context->job, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        if (context->result == ContextResult::Pending)
            return true;
    }

    freeRegisterRC(context, front);
    node->resultRegisterRC = reserveRegisterRC(context);

    auto inst       = emitInstruction(context, ByteCodeOp::MakeLambda, node->resultRegisterRC);
    inst->b.pointer = (uint8_t*) funcNode;
    inst->c.pointer = nullptr;
    if (funcNode->extension && funcNode->extension->bytecode && funcNode->extension->bytecode->bc)
    {
        inst->c.pointer                              = (uint8_t*) funcNode->extension->bytecode->bc;
        funcNode->extension->bytecode->bc->isUsed    = true;
        funcNode->extension->bytecode->bc->forceEmit = true;
    }

    // :CaptureBlock
    // Block capture
    if (node->typeInfo->isClosure())
    {
        node->resultRegisterRC += node->childs.back()->resultRegisterRC[0];
    }

    return true;
}

bool ByteCodeGenJob::emitMakePointer(ByteCodeGenContext* context)
{
    auto node              = context->node;
    auto front             = node->childs.front();
    node->resultRegisterRC = front->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitMakeArrayPointerSlicingUpperBound(ByteCodeGenContext* context)
{
    auto upperNode = context->node;
    auto slicing   = CastAst<AstArrayPointerSlicing>(context->node->parent, AstNodeKind::ArrayPointerSlicing);
    auto arrayNode = slicing->array;

    if (upperNode->extension && upperNode->extension->misc && upperNode->extension->misc->resolvedUserOpSymbolOverload)
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result != ContextResult::Done)
            return true;
        return true;
    }

    auto typeInfo = TypeManager::concretePtrRefType(arrayNode->typeInfo);
    if (typeInfo->isString() ||
        typeInfo->isSlice() ||
        typeInfo->isVariadic() ||
        typeInfo->isTypedVariadic())
    {
        upperNode->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, upperNode->resultRegisterRC, arrayNode->resultRegisterRC[1]);
        emitInstruction(context, ByteCodeOp::Add64byVB64, upperNode->resultRegisterRC)->b.s64 = -1;
        return true;
    }

    return Report::internalError(context->node, "emitMakeArrayPointerSlicingUpperBound, type not supported");
}

bool ByteCodeGenJob::emitMakeArrayPointerSlicing(ByteCodeGenContext* context)
{
    auto job     = context->job;
    auto node    = CastAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto typeVar = TypeManager::concreteType(node->array->typeInfo);

    if (!(node->lowerBound->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->lowerBound, node->lowerBound->typeInfo, node->lowerBound->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->lowerBound->doneFlags |= AST_DONE_CAST1;
    }

    if (!(node->upperBound->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->upperBound, node->upperBound->typeInfo, node->upperBound->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->upperBound->doneFlags |= AST_DONE_CAST1;
    }

    // Exclude upper bound limit
    if (node->specFlags & AST_SPEC_RANGE_EXCLUDE_UP && !(node->upperBound->flags & AST_VALUE_COMPUTED))
    {
        ensureCanBeChangedRC(context, node->upperBound->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::Add64byVB64, node->upperBound->resultRegisterRC)->b.s64 = -1;
    }

    // Slicing of a structure, with a special function
    if (typeVar->isStruct())
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            if (!job->allParamsTmp)
                job->allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
            job->allParamsTmp->childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, job->allParamsTmp));
            if (context->result != ContextResult::Done)
                return true;
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
        emitInstruction(context, ByteCodeOp::BinOpMinusS64, node->upperBound->resultRegisterRC, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC);
    emitInstruction(context, ByteCodeOp::Add64byVB64, node->upperBound->resultRegisterRC)->b.u64 = 1;
    emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r0[1], node->upperBound->resultRegisterRC);

    // Increment start pointer
    emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r0[0], node->array->resultRegisterRC);
    if (node->lowerBound)
    {
        if (sizeOf > 1)
        {
            ensureCanBeChangedRC(context, node->lowerBound->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::Mul64byVB64, node->lowerBound->resultRegisterRC)->b.u64 = sizeOf;
        }
        emitInstruction(context, ByteCodeOp::IncPointer64, r0[0], node->lowerBound->resultRegisterRC, r0[0]);
    }

    freeRegisterRC(context, node->array);
    freeRegisterRC(context, node->lowerBound);
    freeRegisterRC(context, node->upperBound);
    node->resultRegisterRC = r0;

    return true;
}

void ByteCodeGenJob::emitSetZeroAtPointer(ByteCodeGenContext* context, uint64_t sizeOf, uint32_t registerIndex)
{
    switch (sizeOf)
    {
    case 1:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer8, registerIndex);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer16, registerIndex);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer32, registerIndex);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, registerIndex);
        break;
    default:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointerX, registerIndex)->b.u64 = sizeOf;
        break;
    }
}

void ByteCodeGenJob::emitMemCpy(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint64_t sizeOf)
{
    switch (sizeOf)
    {
    case 1:
        emitInstruction(context, ByteCodeOp::MemCpy8, r0, r1);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::MemCpy16, r0, r1);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::MemCpy32, r0, r1);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::MemCpy64, r0, r1);
        break;
    default:
    {
        auto inst = emitInstruction(context, ByteCodeOp::IntrinsicMemCpy, r0, r1);
        inst->flags |= BCI_IMM_C;
        inst->c.u64 = sizeOf;
        break;
    }
    }
}
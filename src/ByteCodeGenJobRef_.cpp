#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitPointerRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    emitSafetyNullPointer(context, node->array->resultRegisterRC);

    if (!(node->access->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    // In case of a deref, no need to increment pointer because we are sure that index is 0
    if (!node->isDeref)
    {
        auto sizeOf = node->typeInfo->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
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
    emitSafetyNullPointer(context, node->array->resultRegisterRC[0]);

    if (!(node->access->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    emitSafetyBoundCheckString(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

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
        if (context->result == ContextResult::Pending)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    if (!node->access->hasComputedValue())
        emitSafetyBoundCheckArray(context, node->access->resultRegisterRC, typeInfoArray);

    // Pointer increment
    auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
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
        if (context->result == ContextResult::Pending)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    SWAG_ASSERT(node->array->resultRegisterRC.size() != 2);
    node->array->resultRegisterRC += reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);

    emitSafetyNullPointer(context, node->array->resultRegisterRC);
    emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

    // Pointer increment
    auto sizeOf = node->typeInfo->sizeOf;
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitStructDeRef(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto typeInfo = node->typeInfo;

    if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
    {
        if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
        else if (node->flags & AST_TO_UFCS) // Get the structure pointer
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
    {
        emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
        else if (node->flags & AST_TO_UFCS) // Get the structure pointer
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice || typeInfo->kind == TypeInfoKind::Interface)
    {
        transformResultToLinear2(context, node);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Array)
    {
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        truncRegisterRC(context, node->resultRegisterRC, 1);
        emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        truncRegisterRC(context, node->resultRegisterRC, 1);
        if (typeInfo->flags & TYPEINFO_RELATIVE)
            SWAG_CHECK(emitUnwrapRelativePointer(context, node->resultRegisterRC, typeInfo));
        else
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, typeInfo));
    return true;
}

bool ByteCodeGenJob::emitWrapRelativePointer(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
    if (fromTypeInfo == g_TypeMgr.typeInfoNull)
    {
        emitSetZeroAtPointer(context, typePtr->sizeOf, r0);
    }
    else
    {
        auto jumpToDo = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::JumpIfNotZero64, r1);
        auto seekJumpNotZero = context->bc->numInstructions;
        emitSetZeroAtPointer(context, typePtr->sizeOf, r0);
        auto jumpAfter = context->bc->numInstructions; 
        emitInstruction(context, ByteCodeOp::Jump);
        auto seekJump  = context->bc->numInstructions;

        context->bc->out[jumpToDo].b.s32 = context->bc->numInstructions - seekJumpNotZero;
        emitInstruction(context, ByteCodeOp::BinOpMinusS64, r1, r0, r1);
        switch (typePtr->sizeOf)
        {
        case 1:
            emitSafetyRelativePointerS64(context, r1, 1);
            emitInstruction(context, ByteCodeOp::SetAtPointer8, r0, r1);
            break;
        case 2:
            emitSafetyRelativePointerS64(context, r1, 2);
            emitInstruction(context, ByteCodeOp::SetAtPointer16, r0, r1);
            break;
        case 4:
            emitSafetyRelativePointerS64(context, r1, 4);
            emitInstruction(context, ByteCodeOp::SetAtPointer32, r0, r1);
            break;
        case 8:
            emitSafetyRelativePointerS64(context, r1, 8);
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
            break;
        }

        context->bc->out[jumpAfter].b.s32 = context->bc->numInstructions - seekJump;
    }

    return true;
}

bool ByteCodeGenJob::emitUnwrapRelativePointer(ByteCodeGenContext* context, uint32_t rr, TypeInfo* typeInfo)
{
    auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
    auto r0      = reserveRegisterRC(context);
    switch (typePtr->sizeOf)
    {
    case 1:
        emitInstruction(context, ByteCodeOp::DeRef8, r0, rr);
        emitInstruction(context, ByteCodeOp::CastS8S64, r0);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::DeRef16, r0, rr);
        emitInstruction(context, ByteCodeOp::CastS16S64, r0);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::DeRef32, r0, rr);
        emitInstruction(context, ByteCodeOp::CastS32S64, r0);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::DeRef64, r0, rr);
        break;
    }

    emitInstruction(context, ByteCodeOp::JumpIfNotZero64, r0)->b.s32 = 1;
    emitInstruction(context, ByteCodeOp::ClearRA, rr);
    emitInstruction(context, ByteCodeOp::BinOpPlusS64, r0, rr, rr);
    return true;
}

bool ByteCodeGenJob::emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo, bool safety)
{
    if (typeInfo->kind == TypeInfoKind::Reference)
    {
        if (safety)
            emitSafetyNullPointer(context, r0);
        emitInstruction(context, ByteCodeOp::DeRef64, r0, r0);
        return true;
    }

    typeInfo = TypeManager::concreteReference(typeInfo);
    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);

    if (typeInfo->numRegisters() == 2)
    {
        if (safety)
            emitSafetyNullPointer(context, r0);
        transformResultToLinear2(context, r0);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, r0[0], r0[1]);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::TypeListTuple ||
        typeInfo->kind == TypeInfoKind::TypeListArray ||
        typeInfo->kind == TypeInfoKind::Struct ||
        typeInfo->kind == TypeInfoKind::Array)
    {
        return true;
    }

    if (safety)
        emitSafetyNullPointer(context, r0);

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
        return internalError(context, "emitTypeDeRef, size not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitPointerDeRef(ByteCodeGenContext* context)
{
    auto job      = context->job;
    auto node     = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto typeInfo = TypeManager::concreteType(node->array->typeInfo);

    if (!(node->access->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->access, node->access->typeInfo, node->access->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->access->doneFlags |= AST_DONE_CAST1;
    }

    // Dereference of a string constant
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        emitSafetyNullPointer(context, node->array->resultRegisterRC);
        emitSafetyBoundCheckString(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC, node->array->resultRegisterRC);
        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
        truncRegisterRC(context, node->resultRegisterRC, 1);
    }

    // Dereference of a slice
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        emitSafetyNullPointer(context, node->array->resultRegisterRC);
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        auto typeInfoSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            auto sizeOf = typeInfoSlice->pointedType->sizeOf;
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfoSlice->pointedType->isNative(NativeTypeKind::String))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoSlice->pointedType, false));
        else if (!(node->forceTakeAddress()) || typeInfoSlice->pointedType->kind == TypeInfoKind::Pointer)
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoSlice->pointedType, false));

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference of a pointer
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typeInfoPointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);

        emitSafetyNullPointer(context, node->array->resultRegisterRC);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            auto sizeOf = typeInfoPointer->pointedType->sizeOf;
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfoPointer->finalType->isNative(NativeTypeKind::String))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoPointer->finalType, false));
        else if (!(node->forceTakeAddress()))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoPointer->pointedType, false));

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference of an array
    else if (typeInfo->kind == TypeInfoKind::Array)
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
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfoArray->pointedType->isNative(NativeTypeKind::String))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoArray->pointedType, false));
        else if ((!(node->forceTakeAddress()) && typeInfoArray->pointedType->kind != TypeInfoKind::Array) || (typeInfoArray->pointedType->kind == TypeInfoKind::Pointer))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfoArray->pointedType, false));

        node->resultRegisterRC         = node->array->resultRegisterRC;
        node->parent->resultRegisterRC = node->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference a variadic parameter
    else if (typeInfo->kind == TypeInfoKind::Variadic)
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = 2 * sizeof(Register); // 2 is sizeof(any)
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, g_TypeMgr.typeInfoAny, false));
        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference a typed variadic parameter
    else if (typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        emitSafetyBoundCheckSlice(context, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        auto rawType = ((TypeInfoVariadic*) typeInfo)->rawType;

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstant0())
        {
            if (rawType->kind == TypeInfoKind::Native && rawType->sizeOf < sizeof(Register))
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = rawType->sizeOf;
            else
                emitInstruction(context, ByteCodeOp::Mul64byVB64, node->access->resultRegisterRC)->b.u64 = rawType->numRegisters() * sizeof(Register);
            emitInstruction(context, ByteCodeOp::IncPointer64, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, rawType, false));
        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference a struct
    else if (typeInfo->kind == TypeInfoKind::Struct)
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
        }
    }
    else
    {
        return internalError(context, "emitPointerDeRef, type not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitMakeLambda(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto front    = node->childs.front();
    auto funcNode = CastAst<AstFuncDecl>(front->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);

    if (!(funcNode->attributeFlags & ATTRIBUTE_FOREIGN))
    {
        // Need to generate bytecode, if not already done or running
        askForByteCode(context->job, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    freeRegisterRC(context, front);
    node->resultRegisterRC = reserveRegisterRC(context);

    auto inst       = emitInstruction(context, ByteCodeOp::MakeLambda, node->resultRegisterRC);
    inst->b.pointer = (uint8_t*) funcNode;
    SWAG_ASSERT(funcNode->extension);
    inst->c.pointer = (uint8_t*) funcNode->extension->bc;

    return true;
}

bool ByteCodeGenJob::emitMakePointer(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = node->childs.front()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitMakeArrayPointerSlicing(ByteCodeGenContext* context)
{
    auto job     = context->job;
    auto node    = CastAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto typeVar = TypeManager::concreteReferenceType(node->array->typeInfo);

    if (!(node->lowerBound->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->lowerBound, node->lowerBound->typeInfo, node->lowerBound->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->lowerBound->doneFlags |= AST_DONE_CAST1;
    }

    if (!(node->lowerBound->doneFlags & AST_DONE_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->upperBound, node->upperBound->typeInfo, node->upperBound->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->upperBound->doneFlags |= AST_DONE_CAST2;
    }

    // Slicing of a structure, with a special function
    if (typeVar->kind == TypeInfoKind::Struct)
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

        return internalError(context, "emitMakeArrayPointerSlicing, type not supported");
    }

    uint64_t sizeOf = 1;
    if (typeVar->kind == TypeInfoKind::Array)
        sizeOf = CastTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->finalType->sizeOf;
    else if (typeVar->isNative(NativeTypeKind::String))
        sizeOf = 1;
    else if (typeVar->kind == TypeInfoKind::Slice)
        sizeOf = CastTypeInfo<TypeInfoSlice>(typeVar, TypeInfoKind::Slice)->pointedType->sizeOf;
    else if (typeVar->kind == TypeInfoKind::Pointer)
        sizeOf = CastTypeInfo<TypeInfoPointer>(typeVar, TypeInfoKind::Pointer)->pointedType->sizeOf;
    else
        return internalError(context, "emitMakeArrayPointerSlicing, type not supported");

    emitSafetyArrayPointerSlicing(context, node);

    RegisterList r0;
    reserveLinearRegisterRC2(context, r0);

    // Compute size of slice
    if (node->lowerBound)
        emitInstruction(context, ByteCodeOp::BinOpMinusS64, node->upperBound->resultRegisterRC, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC);
    emitInstruction(context, ByteCodeOp::Add64byVB64, node->upperBound->resultRegisterRC)->b.u64 = 1;
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0[1], node->upperBound->resultRegisterRC);

    // Increment start pointer
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0[0], node->array->resultRegisterRC);
    if (node->lowerBound)
    {
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB64, node->lowerBound->resultRegisterRC)->b.u64 = sizeOf;
        emitInstruction(context, ByteCodeOp::IncPointer64, r0[0], node->lowerBound->resultRegisterRC, r0[0]);
    }

    freeRegisterRC(context, node->array);
    freeRegisterRC(context, node->lowerBound);
    freeRegisterRC(context, node->upperBound);
    node->resultRegisterRC = r0;

    return true;
}

bool ByteCodeGenJob::emitInit(ByteCodeGenContext* context)
{
    auto node           = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto typeExpression = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);

    // Determine if we just need to clear the memory
    bool justClear = true;
    if (node->parameters)
    {
        for (auto child : node->parameters->childs)
        {
            if (!(child->flags & AST_VALUE_COMPUTED))
            {
                justClear = false;
                break;
            }

            if (child->computedValue.reg.u64)
            {
                justClear = false;
                break;
            }
        }
    }

    TypeInfoStruct* typeStruct = nullptr;
    if (typeExpression->pointedType->kind == TypeInfoKind::Struct)
    {
        typeStruct = CastTypeInfo<TypeInfoStruct>(typeExpression->pointedType, TypeInfoKind::Struct);
        if (typeStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            justClear = false;
    }

    // Number of elements to init. If 0, then this is dynamic
    uint64_t numToInit = 0;
    if (!node->count)
        numToInit = 1;
    else if (node->count->flags & AST_VALUE_COMPUTED)
        numToInit = node->count->computedValue.reg.u64;
    else if (!(node->count->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->count, node->count->typeInfo, node->count->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->count->doneFlags |= AST_DONE_CAST1;
    }

    if (justClear)
    {
        uint64_t sizeToClear = typeExpression->pointedType->sizeOf;
        if (sizeToClear)
        {
            if (numToInit)
            {
                sizeToClear *= numToInit;
                emitSetZeroAtPointer(context, sizeToClear, node->expression->resultRegisterRC);
            }
            else
            {
                SWAG_ASSERT(node->count);
                emitInstruction(context, ByteCodeOp::SetZeroAtPointerXRB, node->expression->resultRegisterRC, node->count->resultRegisterRC)->c.u64 = sizeToClear;
            }
        }
    }
    else if (!node->parameters || node->parameters->childs.empty())
    {
        SWAG_ASSERT(typeStruct);
        if (!(typeStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        {
            context->job->waitStructGenerated(typeStruct);
            if (context->result != ContextResult::Done)
                return true;

            SWAG_ASSERT(typeStruct->opInit);
            if (!generateStruct_opInit(context, typeStruct))
                return false;

            auto startLoop = context->bc->numInstructions;
            emitInstruction(context, ByteCodeOp::PushRAParam, node->expression->resultRegisterRC);
            SWAG_ASSERT(typeStruct->opInit);
            emitOpCallUser(context, nullptr, typeStruct->opInit, false);

            if (numToInit != 1)
            {
                auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, node->expression->resultRegisterRC, 0, node->expression->resultRegisterRC);
                inst->flags |= BCI_IMM_B;
                inst->b.u64 = typeStruct->sizeOf;
                emitInstruction(context, ByteCodeOp::DecrementRA64, node->count->resultRegisterRC);
                auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, node->count->resultRegisterRC);
                instJump->b.s32 = startLoop - context->bc->numInstructions;
            }
        }
    }
    else if (!typeStruct)
    {
        auto child     = node->parameters->childs.front();
        auto startLoop = context->bc->numInstructions;
        SWAG_CHECK(emitAffectEqual(context, node->expression->resultRegisterRC, child->resultRegisterRC, child->typeInfo, child));
        if (numToInit != 1)
        {
            auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, node->expression->resultRegisterRC, 0, node->expression->resultRegisterRC);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = typeExpression->pointedType->sizeOf;
            emitInstruction(context, ByteCodeOp::DecrementRA64, node->count->resultRegisterRC);
            auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, node->count->resultRegisterRC);
            instJump->b.s32 = startLoop - context->bc->numInstructions;
        }
    }
    else
    {
        RegisterList r1;
        reserveRegisterRC(context, r1, 1);

        auto startLoop = context->bc->numInstructions;
        for (auto child : node->parameters->childs)
        {
            auto param     = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            auto typeParam = CastTypeInfo<TypeInfoParam>(param->resolvedParameter, TypeInfoKind::Param);
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, r1, node->expression->resultRegisterRC);
            if (typeParam->offset)
                emitInstruction(context, ByteCodeOp::Add64byVB64, r1)->b.u64 = typeParam->offset;
            emitAffectEqual(context, r1, child->resultRegisterRC, child->typeInfo, child);
        }

        if (numToInit != 1)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::IncPointer64, node->expression->resultRegisterRC, 0, node->expression->resultRegisterRC);
            inst->b.u64 = typeExpression->pointedType->sizeOf;
            inst->flags |= BCI_IMM_B;

            emitInstruction(context, ByteCodeOp::DecrementRA64, node->count->resultRegisterRC);
            auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, node->count->resultRegisterRC);
            instJump->b.s32 = startLoop - context->bc->numInstructions;
        }

        for (auto child : node->parameters->childs)
            freeRegisterRC(context, child);
        freeRegisterRC(context, r1);
    }

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
    freeRegisterRC(context, node->parameters);
    return true;
}

bool ByteCodeGenJob::emitDropCopyMove(ByteCodeGenContext* context)
{
    auto node           = CastAst<AstDropCopyMove>(context->node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    auto typeExpression = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);

    if (typeExpression->pointedType->kind != TypeInfoKind::Struct)
    {
        freeRegisterRC(context, node->expression);
        freeRegisterRC(context, node->count);
        return true;
    }

    // Number of elements to deal with. If 0, then this is dynamic
    uint64_t numToDo = 0;
    if (!node->count)
        numToDo = 1;
    else if (node->count->flags & AST_VALUE_COMPUTED)
        numToDo = node->count->computedValue.reg.u64;
    else if (!(node->count->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->count, node->count->typeInfo, node->count->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->count->doneFlags |= AST_DONE_CAST1;
    }

    auto typeStruct    = CastTypeInfo<TypeInfoStruct>(typeExpression->pointedType, TypeInfoKind::Struct);
    bool somethingToDo = false;
    switch (node->kind)
    {
    case AstNodeKind::Drop:
        generateStruct_opDrop(context, typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
        somethingToDo = typeStruct->opDrop;
        break;
    case AstNodeKind::PostCopy:
        generateStruct_opPostCopy(context, typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
        somethingToDo = typeStruct->opPostCopy;
        break;
    case AstNodeKind::PostMove:
        generateStruct_opPostMove(context, typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
        somethingToDo = typeStruct->opPostMove;
        break;
    }

    if (somethingToDo)
    {
        auto startLoop = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::PushRAParam, node->expression->resultRegisterRC);
        auto inst = emitInstruction(context, ByteCodeOp::LocalCall);

        switch (node->kind)
        {
        case AstNodeKind::Drop:
            inst->a.pointer = (uint8_t*) typeStruct->opDrop;
            inst->b.pointer = (uint8_t*) typeStruct->opDrop->typeInfoFunc;
            break;
        case AstNodeKind::PostCopy:
            inst->a.pointer = (uint8_t*) typeStruct->opPostCopy;
            inst->b.pointer = (uint8_t*) typeStruct->opPostCopy->typeInfoFunc;
            break;
        case AstNodeKind::PostMove:
            inst->a.pointer = (uint8_t*) typeStruct->opPostMove;
            inst->b.pointer = (uint8_t*) typeStruct->opPostMove->typeInfoFunc;
            break;
        }

        emitInstruction(context, ByteCodeOp::IncSPPostCall, 8);

        if (numToDo != 1)
        {
            inst        = emitInstruction(context, ByteCodeOp::IncPointer64, node->expression->resultRegisterRC, 0, node->expression->resultRegisterRC);
            inst->b.u64 = typeExpression->pointedType->sizeOf;
            inst->flags |= BCI_IMM_B;
            emitInstruction(context, ByteCodeOp::DecrementRA64, node->count->resultRegisterRC);
            auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, node->count->resultRegisterRC);
            instJump->b.s32 = startLoop - context->bc->numInstructions;
        }
    }

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
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
        auto inst = emitInstruction(context, ByteCodeOp::MemCpyX, r0, r1);
        inst->flags |= BCI_IMM_C;
        inst->c.u64 = sizeOf;
        break;
    }
    }
}
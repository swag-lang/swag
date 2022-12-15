#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"

bool SemanticJob::boundCheck(SemanticContext* context, AstNode* arrayAccess, uint64_t maxCount)
{
    if (!(arrayAccess->flags & AST_VALUE_COMPUTED))
        return true;
    auto idx = arrayAccess->computedValue->reg.u64;
    if (idx >= maxCount)
        return context->report({arrayAccess, Fmt(Err(Err0468), idx, maxCount - 1)});
    return true;
}

bool SemanticJob::checkCanMakeFuncPointer(SemanticContext* context, AstFuncDecl* funcNode, AstNode* node)
{
    Utf8 msg;
    Utf8 msg1;

    if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
    {
        msg  = Err(Err0471);
        msg1 = Hnt(Hnt0015);
    }
    else if (funcNode->attributeFlags & ATTRIBUTE_MIXIN)
    {
        msg  = Err(Err0472);
        msg1 = Hnt(Hnt0016);
    }
    else if (funcNode->attributeFlags & ATTRIBUTE_INLINE)
    {
        msg  = Err(Err0473);
        msg1 = Hnt(Hnt0017);
    }

    if (!msg.empty())
    {
        Diagnostic diag{node, msg};
        diag.hint = msg1;
        return context->report(diag, Diagnostic::hereIs(funcNode->resolvedSymbolOverload));
    }

    return true;
}

bool SemanticJob::checkCanTakeAddress(SemanticContext* context, AstNode* node)
{
    if (node->kind != AstNodeKind::IdentifierRef && node->kind != AstNodeKind::ArrayPointerIndex)
        return context->report({node, Err(Err0470)});

    if (!(node->flags & AST_L_VALUE))
    {
        if (node->resolvedSymbolName->kind != SymbolKind::Variable)
        {
            Diagnostic diag{node, Fmt(Err(Err0465), SymTable::getArticleKindName(node->resolvedSymbolName->kind))};
            diag.hint = Hnt(Hnt0054);
            return context->report(diag);
        }

        Diagnostic note{Hlp(Hlp0022), DiagnosticLevel::Help};
        return context->report({node, Fmt(Err(Err0469), node->typeInfo->getDisplayNameC()), Diagnostic::isType(node->typeInfo)}, &note);
    }

    return true;
}

bool SemanticJob::resolveMakePointerLambda(SemanticContext* context)
{
    auto     node  = CastAst<AstMakePointer>(context->node, AstNodeKind::MakePointerLambda, AstNodeKind::MakePointer);
    AstNode* child = nullptr;

    // When this is a closure, we have /capture params/ref to the function/capture block/ref to the capture block
    if (node->lambda && node->lambda->captureParameters)
        child = node->childs[1];
    else
        child = node->childs.front();

    SWAG_CHECK(checkCanTakeAddress(context, child));
    SWAG_CHECK(checkIsConcrete(context, child));
    node->flags |= AST_R_VALUE;
    node->resolvedSymbolName = child->resolvedSymbolName;

    auto funcNode = child->resolvedSymbolOverload->node;
    SWAG_CHECK(checkCanMakeFuncPointer(context, (AstFuncDecl*) funcNode, child));

    auto lambdaType  = child->typeInfo->clone();
    lambdaType->kind = TypeInfoKind::LambdaClosure;
    if (child->typeInfo->isClosure())
    {
        lambdaType->flags |= TYPEINFO_RETURN_BY_COPY;
        lambdaType->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
    }
    else
        lambdaType->sizeOf = sizeof(void*);
    node->typeInfo    = lambdaType;
    node->byteCodeFct = ByteCodeGenJob::emitMakeLambda;

    // :CaptureBlock
    // Block capture
    if (node->lambda && node->lambda->captureParameters)
    {
        auto       typeBlock = CastTypeInfo<TypeInfoStruct>(node->childs.back()->typeInfo, TypeInfoKind::Struct);
        const auto MaxSize   = SWAG_LIMIT_CLOSURE_SIZEOF - 2 * sizeof(void*);
        SWAG_VERIFY(typeBlock->sizeOf <= MaxSize, context->report({node->childs.back(), Fmt(Err(Err0882), typeBlock->sizeOf, MaxSize)}));
    }

    return true;
}

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node     = CastAst<AstMakePointer>(context->node, AstNodeKind::MakePointer);
    auto child    = node->childs.front();
    auto typeInfo = child->typeInfo;

    SWAG_ASSERT(child->resolvedSymbolName);
    if (child->resolvedSymbolName->kind == SymbolKind::Function)
    {
        // For a function, if no parameters, then this is for a lambda
        auto back = child->childs.back();
        if (back->kind != AstNodeKind::FuncCall)
            return resolveMakePointerLambda(context);

        // Otherwise we want to take the address of the return value
        // Only for a reference
        typeInfo = TypeManager::concreteType(typeInfo);
        if (!typeInfo->isPointerRef())
            return context->report({node, Fmt(Err(Err0114), typeInfo->getDisplayNameC())});
    }

    SWAG_CHECK(checkCanTakeAddress(context, child));
    SWAG_CHECK(checkIsConcrete(context, child));
    node->flags |= AST_R_VALUE;
    node->resolvedSymbolName = child->resolvedSymbolName;
    node->byteCodeFct        = ByteCodeGenJob::emitMakePointer;

    // A new pointer
    bool             forceConst = false;
    TypeInfoPointer* ptrType    = allocType<TypeInfoPointer>();

    // Transform a reference pointer to a pointer to the pointed value
    if (typeInfo->isPointerRef())
    {
        forceConst = typeInfo->isConst();
        typeInfo   = TypeManager::concretePtrRef(typeInfo);
        child->semFlags |= AST_SEM_FORCE_NO_TAKE_ADDRESS;
        child->childs.back()->semFlags |= AST_SEM_FORCE_NO_TAKE_ADDRESS;
        node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
    }

    // If this is an array, then this is legit, the pointer will address the first
    // element : need to find it's type
    else if (typeInfo->isArray())
    {
        while (typeInfo->isArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo       = typeArray->pointedType;
        }

        ptrType = CastTypeInfo<TypeInfoPointer>(g_TypeMgr->asPointerArithmetic(ptrType), TypeInfoKind::Pointer);
    }

    ptrType->pointedType = typeInfo;
    ptrType->sizeOf      = sizeof(void*);

    // :PointerArithmetic
    // Taking the address of an array element is ok for pointer arithmetic
    if (child->kind == AstNodeKind::IdentifierRef)
    {
        auto last = child->childs.back();
        if (last->kind == AstNodeKind::ArrayPointerIndex)
            ptrType = (TypeInfoPointer*) g_TypeMgr->asPointerArithmetic(ptrType);
    }

    ptrType->computeName();

    // Taking the address of a const is const
    if (child->flags & AST_IS_CONST || forceConst)
        ptrType->setConst();

    // Type is constant if we take address of a readonly variable
    if (child->resolvedSymbolOverload && !child->typeInfo->isPointerRef())
    {
        auto typeResolved = TypeManager::concreteType(child->resolvedSymbolOverload->typeInfo, CONCRETE_ALIAS);

        if ((child->resolvedSymbolOverload->flags & OVERLOAD_CONST_ASSIGN) &&
            !typeResolved->isArray() &&
            !typeResolved->isSlice())
        {
            ptrType->setConst();
        }

        if (typeResolved->isConst() && typeResolved->isSlice())
            ptrType->setConst();
        else if (node->flags & AST_IS_CONST)
            ptrType->setConst();
    }

    node->typeInfo = ptrType;

    // Transform pointer to a reference
    if (node->specFlags & AST_SPEC_MAKEPOINTER_TOREF)
    {
        ptrType->flags |= TYPEINFO_POINTER_REF;
        ptrType->forceComputeName();
    }

    return true;
}

bool SemanticJob::resolveArrayPointerSlicing(SemanticContext* context)
{
    auto     node     = CastAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto     typeVar  = TypeManager::concreteType(node->array->typeInfo);
    uint64_t maxBound = 0;

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, node->lowerBound, CASTFLAG_TRY_COERCE));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, node->upperBound, CASTFLAG_TRY_COERCE));

    // Slicing of an array
    if (typeVar->isArray())
    {
        auto typeInfoArray = CastTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);
        if (typeInfoArray->totalCount != typeInfoArray->count)
        {
            Diagnostic diag{node, node->token, Err(Err0474)};
            diag.hint = Hnt(Hnt0061);
            diag.setRange2(node->array, Diagnostic::isType(typeInfoArray));
            return context->report(diag);
        }

        auto ptrSlice         = allocType<TypeInfoSlice>();
        ptrSlice->pointedType = typeInfoArray->finalType;
        if (typeInfoArray->isConst())
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->computeName();
        node->typeInfo = ptrSlice;
        maxBound       = typeInfoArray->count - 1;
    }

    // Slicing of a string
    else if (typeVar->isString())
    {
        node->typeInfo = typeVar;
        if (node->array->flags & AST_VALUE_COMPUTED)
            maxBound = node->array->computedValue->text.length();
    }

    // Slicing of a pointer
    else if (typeVar->isPointer())
    {
        if (!(typeVar->flags & TYPEINFO_POINTER_ARITHMETIC))
        {
            Diagnostic diag{node, node->token, Err(Err0193)};
            diag.hint = Hnt(Hnt0061);
            diag.setRange2(node->array, Diagnostic::isType(typeVar));
            return context->report(diag);
        }

        auto typeInfoPointer  = CastTypeInfo<TypeInfoPointer>(node->array->typeInfo, TypeInfoKind::Pointer);
        auto ptrSlice         = allocType<TypeInfoSlice>();
        ptrSlice->pointedType = typeInfoPointer->pointedType;
        if (typeInfoPointer->isConst())
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->computeName();
        node->typeInfo = ptrSlice;
    }

    // Slicing of a... slice
    else if (typeVar->isSlice())
    {
        node->typeInfo = typeVar;
    }

    // Slicing of a struct with a special function
    else if (typeVar->isStruct())
    {
        // Flatten all operator parameters : self, then all indices
        node->structFlatParams.clear();
        node->structFlatParams.push_back(node->lowerBound);
        node->structFlatParams.push_back(node->upperBound);

        // Self in first position
        node->structFlatParams.push_front(node->array);

        // Resolve call
        auto        typeInfo = node->array->typeInfo;
        SymbolName* symbol   = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opSlice, node->array, &symbol));
        if (!symbol)
        {
            if (context->result != ContextResult::Done)
                return true;

            Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0320), node->array->token.ctext(), typeInfo->getDisplayNameC())};
            diag.hint = Fmt(Hnt(Hnt0047), g_LangSpec->name_opSlice.c_str());
            diag.setRange2(node->array, Diagnostic::isType(typeInfo));
            return context->report(diag);
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opSlice, nullptr, nullptr, node->array, node->structFlatParams));
    }
    else
    {
        return context->report({node->array, Fmt(Err(Err0475), node->array->typeInfo->getDisplayNameC())});
    }

    // startBound <= endBound
    if ((node->lowerBound->flags & AST_VALUE_COMPUTED) && (node->upperBound->flags & AST_VALUE_COMPUTED))
    {
        if (node->lowerBound->computedValue->reg.u64 > node->upperBound->computedValue->reg.u64)
        {
            Diagnostic diag{node->lowerBound, Fmt(Err(Err0476), node->lowerBound->computedValue->reg.u64, node->upperBound->computedValue->reg.u64)};
            diag.hint = Hnt(Hnt0076);
            diag.setRange2(node->upperBound, Hnt(Hnt0077));
            return context->report(diag);
        }
    }

    // endBound < maxBound
    if (maxBound && (node->upperBound->flags & AST_VALUE_COMPUTED))
    {
        if (node->upperBound->computedValue->reg.u64 > maxBound)
        {
            Diagnostic diag{node->upperBound, Fmt(Err(Err0477), node->upperBound->computedValue->reg.u64, maxBound)};
            return context->report(diag);
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitMakeArrayPointerSlicing;
    return true;
}

bool SemanticJob::resolveKeepRef(SemanticContext* context)
{
    auto node  = context->node;
    auto front = node->childs.front();
    node->inheritAndFlag1(AST_CONST_EXPR);

    auto typeInfo = TypeManager::concreteType(front->typeInfo);
    if (!typeInfo->isPointerRef() && !typeInfo->isPointer())
        return context->report({node, Fmt(Err(Err0517), typeInfo->getDisplayNameC())});

    if (!typeInfo->isPointerRef())
    {
        typeInfo = typeInfo->clone();
        typeInfo->flags |= TYPEINFO_POINTER_REF;
        typeInfo->forceComputeName();
    }

    node->typeInfo    = typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
    return true;
}

bool SemanticJob::resolveArrayPointerIndex(SemanticContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (node->forceTakeAddress())
        SWAG_CHECK(resolveArrayPointerRef(context));
    else
        SWAG_CHECK(resolveArrayPointerDeRef(context));
    if (context->result == ContextResult::Pending)
        return true;

    if (!(node->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF))
        node->inheritAndFlag1(AST_CONST_EXPR);

    // If this is not the last child of the IdentifierRef, then this is a reference, and
    // we must take the address and not dereference that identifier
    if (node->parent->kind == AstNodeKind::IdentifierRef)
    {
        auto parent = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);
        if (node != parent->childs.back())
        {
            // The last ArrayPointerIndex in a list [0, 0, 0] must dereference
            if (node->childs[0]->kind != AstNodeKind::ArrayPointerIndex)
                node->semFlags |= AST_SEM_FORCE_TAKE_ADDRESS;

            // In order to resolve what's next, we need to fill the startScope of the identifier ref
            auto typeReturn = node->array->typeInfo;

            // There's a 'opIndex' function
            if (node->extension && node->extension->misc && node->extension->misc->resolvedUserOpSymbolOverload)
                typeReturn = TypeManager::concretePtrRefType(node->extension->misc->resolvedUserOpSymbolOverload->typeInfo);

            // Get the pointed type if we have a pointer
            if (typeReturn->isPointer())
            {
                auto typePointer = CastTypeInfo<TypeInfoPointer>(typeReturn, TypeInfoKind::Pointer);
                typeReturn       = typePointer->pointedType;
            }

            // And if this is a struct or an interface, we fill the startScope
            if (typeReturn->isStruct() || typeReturn->isInterface())
            {
                auto typeStruct    = CastTypeInfo<TypeInfoStruct>(typeReturn, TypeInfoKind::Struct, TypeInfoKind::Interface);
                parent->startScope = typeStruct->scope;
            }
        }

        parent->previousResolvedNode = node;
    }

    return true;
}

bool SemanticJob::resolveArrayPointerRef(SemanticContext* context)
{
    auto arrayNode                    = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    arrayNode->resolvedSymbolName     = arrayNode->array->resolvedSymbolName;
    arrayNode->resolvedSymbolOverload = arrayNode->array->resolvedSymbolOverload;
    arrayNode->inheritOrFlag(arrayNode->array, AST_L_VALUE);

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));
    arrayNode->flags |= AST_R_VALUE;

    auto arrayType = TypeManager::concretePtrRefType(arrayNode->array->typeInfo, CONCRETE_ALIAS | CONCRETE_FUNC);

    // When we are building a pointer, this is fine to be const, because in fact we do no generate an address to modify the content
    // (or it will be done later on a pointer, and it will be const too)
    if (arrayNode->parent->parent->kind != AstNodeKind::MakePointer)
    {
        if (arrayType->isConst())
            return context->report({arrayNode->array, Fmt(Err(Err0564), arrayType->getDisplayNameC()), Diagnostic::isType(arrayType)});
    }
    else
    {
        // If array is const, inform the make pointer that it need to make a const pointer
        if (arrayType->isConst())
            arrayNode->parent->parent->flags |= AST_IS_CONST;
    }

    auto accessType = TypeManager::concreteType(arrayNode->access->typeInfo);
    if (!(accessType->isNativeInteger()) && !(accessType->flags & TYPEINFO_ENUM_INDEX))
        return context->report({arrayNode->access, Fmt(Err(Err0485), arrayNode->access->typeInfo->getDisplayNameC())});

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        if (!(arrayType->flags & TYPEINFO_POINTER_ARITHMETIC) && !(arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF))
        {
            Diagnostic diag{arrayNode->array, Fmt(Err(Err0194), arrayNode->resolvedSymbolName->name.c_str()), Diagnostic::isType(arrayType)};
            return context->report(diag);
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePtr->pointedType != g_TypeMgr->typeInfoVoid, context->report({arrayNode->access, Err(Err0486)}));
        arrayNode->typeInfo = typePtr->pointedType;
        arrayNode->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->array->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerRef;
        break;
    }

    case TypeInfoKind::Native:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        if (arrayType->nativeType == NativeTypeKind::String)
        {
            arrayNode->typeInfo    = g_TypeMgr->typeInfoU8;
            arrayNode->byteCodeFct = ByteCodeGenJob::emitStringRef;
            arrayNode->flags |= AST_IS_CONST;
        }
        else
        {
            Diagnostic diag{arrayNode->array, Fmt(Err(Err0481), arrayType->getDisplayNameC()), Diagnostic::isType(arrayType)};
            if (arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF)
                diag.setRange2(arrayNode->token.startLocation, arrayNode->token.endLocation, Hnt(Hnt0060));
            return context->report(diag);
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr           = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitArrayRef;

        // Try to dereference as a constant if we can
        uint32_t     storageOffset  = UINT32_MAX;
        DataSegment* storageSegment = nullptr;
        SWAG_CHECK(getConstantArrayPtr(context, &storageOffset, &storageSegment));
        if (storageSegment)
        {
            arrayNode->setFlagsValueIsComputed();
            arrayNode->computedValue->storageOffset  = storageOffset;
            arrayNode->computedValue->storageSegment = storageSegment;
            if (arrayNode->resolvedSymbolOverload)
            {
                SWAG_ASSERT(arrayNode->resolvedSymbolOverload->computedValue.storageSegment == storageSegment);
                arrayNode->resolvedSymbolOverload->computedValue.storageOffset = storageOffset;
            }
        }
        else
        {
            SWAG_CHECK(boundCheck(context, arrayNode->access, typePtr->count));
        }
        break;
    }

    case TypeInfoKind::Slice:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr           = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitSliceRef;
        break;
    }

    case TypeInfoKind::Struct:
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));

        if (arrayType->isTuple())
        {
            if (arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF)
            {
                Diagnostic diag{arrayNode->access, Err(Err0482), Diagnostic::isType(arrayType)};
                diag.setRange2(arrayNode->token.startLocation, arrayNode->token.endLocation, Hnt(Hnt0060));
                return context->report(diag);
            }
            else
            {
                Diagnostic diag{arrayNode->access, Err(Err0482), Hnt(Hnt0061)};
                diag.setRange2(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(diag);
            }
        }

        arrayNode->typeInfo = arrayType;

        // In fact we are taking the address of an operator [] call result
        if (arrayNode->parent->parent->kind == AstNodeKind::MakePointer)
        {
            SWAG_CHECK(resolveArrayPointerDeRef(context));
            if (context->result != ContextResult::Done)
                return true;
            arrayNode->typeInfo = TypeManager::concreteType(arrayNode->typeInfo);
        }

        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind != AstNodeKind::ArrayPointerIndex)
        {
            // Flatten all indices. self and value will be set before the call later
            // Can be already done, so do not overwrite
            // :StructFlatParamsDone
            if (arrayNode->structFlatParams.empty())
            {
                arrayNode->structFlatParams.push_back(arrayNode->access);

                AstNode* child = arrayNode->array;
                while (child->kind == AstNodeKind::ArrayPointerIndex)
                {
                    auto arrayChild = CastAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
                    arrayNode->structFlatParams.push_front(arrayChild->access);
                    child = arrayChild->array;
                }
            }
        }
        break;

    default:
    {
        return context->report({arrayNode->array, Fmt(Err(Err0481), arrayType->getDisplayNameC())});
    }
    }

    return true;
}

bool SemanticJob::getConstantArrayPtr(SemanticContext* context, uint32_t* storageOffset, DataSegment** storageSegment)
{
    auto arrayNode = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayType = TypeManager::concreteType(arrayNode->array->typeInfo);
    auto typePtr   = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);

    if (!arrayNode->typeInfo->isArray() && arrayNode->access->flags & AST_VALUE_COMPUTED)
    {
        bool     isConstAccess = true;
        uint64_t offsetAccess  = arrayNode->access->computedValue->reg.u64 * typePtr->finalType->sizeOf;
        SWAG_CHECK(boundCheck(context, arrayNode->access, typePtr->count));

        // Deal with array of array
        auto subArray = arrayNode;
        while (isConstAccess && subArray->array->kind == AstNodeKind::ArrayPointerIndex)
        {
            subArray      = CastAst<AstArrayPointerIndex>(subArray->array, AstNodeKind::ArrayPointerIndex);
            isConstAccess = isConstAccess && (subArray->access->flags & AST_VALUE_COMPUTED);
            if (isConstAccess)
            {
                auto subTypePtr = CastTypeInfo<TypeInfoArray>(subArray->array->typeInfo, TypeInfoKind::Array);
                SWAG_CHECK(boundCheck(context, subArray->access, subTypePtr->count));
                offsetAccess += subArray->access->computedValue->reg.u64 * subTypePtr->pointedType->sizeOf;
                typePtr = subTypePtr;
            }
        }

        if (isConstAccess)
        {
            auto overload = subArray->array->resolvedSymbolOverload;
            if (overload && (overload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                SWAG_ASSERT(overload->computedValue.storageOffset != UINT32_MAX);
                SWAG_ASSERT(overload->computedValue.storageSegment);
                *storageOffset  = overload->computedValue.storageOffset + (uint32_t) offsetAccess;
                *storageSegment = overload->computedValue.storageSegment;
                return true;
            }

            if (subArray->array->flags & AST_VALUE_COMPUTED)
            {
                SWAG_ASSERT(subArray->array->computedValue);
                SWAG_ASSERT(subArray->array->computedValue->storageOffset != UINT32_MAX);
                SWAG_ASSERT(subArray->array->computedValue->storageSegment);
                *storageOffset  = subArray->array->computedValue->storageOffset + (uint32_t) offsetAccess;
                *storageSegment = subArray->array->computedValue->storageSegment;
                return true;
            }
        }
    }

    return true;
}

bool SemanticJob::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto arrayNode         = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayAccess       = arrayNode->access;
    auto arrayType         = TypeManager::concretePtrRefType(arrayNode->array->typeInfo);
    arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerDeRef;

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));

    if (arrayType->isTuple())
    {
        if (arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF)
        {
            Diagnostic diag{arrayNode->access, Err(Err0482), Diagnostic::isType(arrayType)};
            diag.setRange2(arrayNode->token.startLocation, arrayNode->token.endLocation, Hnt(Hnt0060));
            return context->report(diag);
        }
        else
        {
            Diagnostic diag{arrayNode->access, Err(Err0482), Hnt(Hnt0061)};
            diag.setRange2(arrayNode->array, Diagnostic::isType(arrayType));
            return context->report(diag);
        }
    }

    arrayNode->flags |= AST_R_VALUE;

    auto accessType = TypeManager::concreteType(arrayNode->access->typeInfo);
    if (!(accessType->isNativeInteger()) && !(accessType->flags & TYPEINFO_ENUM_INDEX))
    {
        Diagnostic diag{arrayNode->access, Fmt(Err(Err0485), arrayNode->access->typeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0058);
        return context->report(diag);
    }

    // Do not set resolvedSymbolOverload !
    arrayNode->resolvedSymbolName = arrayNode->array->resolvedSymbolName;

    // Can we dereference at compile time ?
    if (arrayType->isString())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        if (arrayNode->access->flags & AST_VALUE_COMPUTED)
        {
            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                arrayNode->setFlagsValueIsComputed();
                auto& text = arrayNode->array->resolvedSymbolOverload->computedValue.text;
                SWAG_CHECK(boundCheck(context, arrayNode->access, text.length()));
                auto idx                         = arrayAccess->computedValue->reg.u32;
                arrayNode->computedValue->reg.u8 = text[idx];
            }
        }

        arrayNode->typeInfo = g_TypeMgr->typeInfoU8;
        return true;
    }

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        if (!(arrayType->flags & TYPEINFO_POINTER_ARITHMETIC) && !(arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF))
        {
            Diagnostic diag{arrayNode->array, Fmt(Err(Err0194), arrayNode->resolvedSymbolName->name.c_str()), Diagnostic::isType(arrayType)};
            return context->report(diag);
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePtr->pointedType != g_TypeMgr->typeInfoVoid, context->report({arrayNode->access, Err(Err0486)}));
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Array:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr        = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);

        // Try to dereference as a constant if we can
        uint32_t     storageOffset  = UINT32_MAX;
        DataSegment* storageSegment = nullptr;
        SWAG_CHECK(getConstantArrayPtr(context, &storageOffset, &storageSegment));
        if (storageSegment)
        {
            auto ptr = storageSegment->address(storageOffset);
            if (derefConstantValue(context, arrayNode, typePtr->finalType, storageSegment, ptr))
                arrayNode->setFlagsValueIsComputed();
            else if (typePtr->finalType->isStruct())
            {
                arrayNode->setFlagsValueIsComputed();
                arrayNode->computedValue->storageSegment = storageSegment;
                arrayNode->computedValue->storageOffset  = storageOffset;
                arrayNode->typeInfo                      = typePtr->finalType;
                if (arrayNode->resolvedSymbolOverload)
                {
                    SWAG_ASSERT(arrayNode->resolvedSymbolOverload->computedValue.storageSegment == storageSegment);
                    arrayNode->resolvedSymbolOverload->computedValue.storageOffset = storageOffset;
                }
            }
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typeSlice      = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo = typeSlice->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);

        // Try to dereference as a constant if we can
        if (arrayNode->access->flags & AST_VALUE_COMPUTED)
        {
            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                SWAG_CHECK(boundCheck(context, arrayNode->access, arrayNode->array->computedValue->reg.u32));
                auto& computedValue = arrayNode->array->resolvedSymbolOverload->computedValue;
                SWAG_ASSERT(computedValue.storageOffset != UINT32_MAX);
                SWAG_ASSERT(computedValue.storageSegment != nullptr);
                auto ptr = computedValue.storageSegment->address(computedValue.storageOffset);
                ptr += arrayNode->access->computedValue->reg.u64 * typeSlice->pointedType->sizeOf;
                if (derefConstantValue(context, arrayNode, typeSlice->pointedType, computedValue.storageSegment, ptr))
                    arrayNode->setFlagsValueIsComputed();
            }
        }

        break;
    }

    case TypeInfoKind::Variadic:
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        arrayNode->typeInfo = g_TypeMgr->typeInfoAny;
        break;

    case TypeInfoKind::TypedVariadic:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typeVariadic   = CastTypeInfo<TypeInfoVariadic>(arrayType, TypeInfoKind::TypedVariadic);
        arrayNode->typeInfo = typeVariadic->rawType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Struct:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));

        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex)
        {
            arrayNode->typeInfo = arrayType;
            break;
        }

        // Flatten all operator parameters : self, then all indices
        // :StructFlatParamsDone
        arrayNode->structFlatParams.clear();
        arrayNode->structFlatParams.push_back(arrayNode->access);

        AstNode* child = arrayNode->array;
        while (child->kind == AstNodeKind::ArrayPointerIndex)
        {
            auto arrayChild = CastAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
            arrayNode->structFlatParams.push_front(arrayChild->access);
            child = arrayChild->array;
        }

        // Self in first position
        arrayNode->structFlatParams.push_front(arrayNode->array);

        // Resolve call
        SymbolName* symbol = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opIndex, arrayNode->array, &symbol));
        if (!symbol)
        {
            if (context->result != ContextResult::Done)
                return true;

            if (arrayNode->array->token.text.empty())
            {
                Diagnostic diag{arrayNode->access, Fmt(Err(Err0226), arrayType->getDisplayNameC())};
                diag.hint = Fmt(Hnt(Hnt0047), g_LangSpec->name_opIndex.c_str());
                diag.setRange2(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(diag);
            }
            else
            {
                Diagnostic diag{arrayNode->access, Fmt(Err(Err0227), arrayNode->array->token.ctext(), arrayType->getDisplayNameC())};
                diag.hint = Fmt(Hnt(Hnt0047), g_LangSpec->name_opIndex.c_str());
                diag.setRange2(arrayNode->array, Diagnostic::isType(arrayType));
                return context->report(diag);
            }
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndex, nullptr, nullptr, arrayNode->array, arrayNode->structFlatParams));
        break;
    }

    default:
    {
        Diagnostic diag{arrayNode->array, Fmt(Err(Err0488), arrayNode->array->typeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0021);
        if (arrayNode->specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF)
            diag.setRange2(arrayNode->token.startLocation, arrayNode->token.endLocation, Hnt(Hnt0060));
        return context->report(diag);
    }
    }

    return true;
}

bool SemanticJob::resolveInit(SemanticContext* context)
{
    auto job                = context->job;
    auto node               = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->isPointer(), context->report({node->expression, Fmt(Err(Err0489), expressionTypeInfo->getDisplayNameC())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->isNativeInteger(), context->report({node->count, Fmt(Err(Err0490), countTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, node->count, CASTFLAG_TRY_COERCE));
    }

    if (node->parameters)
    {
        auto typeinfoPointer = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        auto pointedType     = typeinfoPointer->pointedType;

        if (pointedType->isNative() || pointedType->isPointer())
        {
            SWAG_VERIFY(node->parameters->childs.size() == 1, context->report({node->count, Fmt(Err(Err0491), pointedType->getDisplayNameC())}));
            auto child = node->parameters->childs.front();
            SWAG_CHECK(TypeManager::makeCompatibles(context, pointedType, child->typeInfo, nullptr, child));
        }
        else if (pointedType->isStruct())
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(pointedType, TypeInfoKind::Struct);

            SymbolMatchContext symMatchContext;
            symMatchContext.reset();
            for (auto child : node->parameters->childs)
                symMatchContext.parameters.push_back(child);

            auto& listTryMatch = job->cacheListTryMatch;
            while (true)
            {
                job->clearTryMatch();
                auto symbol = typeStruct->declNode->resolvedSymbolName;

                {
                    SharedLock lk(symbol->mutex);
                    for (auto overload : symbol->overloads)
                    {
                        auto t               = job->getTryMatch();
                        t->symMatchContext   = symMatchContext;
                        t->overload          = overload;
                        t->genericParameters = nullptr;
                        t->callParameters    = node->parameters;
                        t->dependentVar      = nullptr;
                        t->cptOverloads      = (uint32_t) symbol->overloads.size();
                        listTryMatch.push_back(t);
                    }
                }

                SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, node));
                if (context->result == ContextResult::Pending)
                    return true;
                if (context->result != ContextResult::NewChilds)
                    break;
                context->result = ContextResult::Done;
            }
        }
        else
        {
            return Report::internalError(context->node, "resolveInit, invalid type");
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitInit;
    return true;
}

bool SemanticJob::resolveDropCopyMove(SemanticContext* context)
{
    auto node               = CastAst<AstDropCopyMove>(context->node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->isPointer(), context->report({node->expression, Fmt(Err(Err0495), node->token.ctext(), expressionTypeInfo->getDisplayNameC())}));

    // Be sure struct if not marked as nocopy
    if (node->kind == AstNodeKind::PostCopy)
    {
        auto ptrType     = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        auto pointedType = TypeManager::concreteType(ptrType->pointedType);
        if (pointedType->flags & TYPEINFO_STRUCT_NO_COPY)
        {
            return context->report({node->expression, Fmt(Err(Err0493), pointedType->getDisplayNameC())});
        }
    }

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->isNativeInteger(), context->report({node->count, Fmt(Err(Err0498), node->token.ctext(), countTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, nullptr, node->count, CASTFLAG_TRY_COERCE));
    }

    node->byteCodeFct = ByteCodeGenJob::emitDropCopyMove;
    return true;
}

bool SemanticJob::derefConstantValue(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, DataSegment* storageSegment, void* ptr)
{
    // Dereferencing a type descriptor. Convert it to a literal.
    if (typeInfo->isPointerToTypeInfo())
    {
        auto value = *(uint8_t**) ptr;
        node->setFlagsValueIsComputed();
        node->computedValue->storageSegment = storageSegment;
        node->computedValue->storageOffset  = storageSegment->offset(value);
        node->computedValue->reg.pointer    = (uint8_t*) typeInfo;
        node->flags |= AST_VALUE_IS_TYPEINFO;
        return true;
    }

    return derefConstantValue(context, node, typeInfo->kind, typeInfo->nativeType, ptr);
}

bool SemanticJob::derefConstantValue(SemanticContext* context, AstNode* node, TypeInfoKind kind, NativeTypeKind nativeKind, void* ptr)
{
    if (kind != TypeInfoKind::Native)
        return false;

    node->allocateComputedValue();
    switch (nativeKind)
    {
    case NativeTypeKind::String:
        node->computedValue->text = *(const char**) ptr;
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoString;
        break;

    case NativeTypeKind::S8:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS8;
        node->computedValue->reg.s8 = *(int8_t*) ptr;
        break;
    case NativeTypeKind::U8:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU8;
        node->computedValue->reg.u8 = *(uint8_t*) ptr;
        break;
    case NativeTypeKind::S16:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS16;
        node->computedValue->reg.s16 = *(int16_t*) ptr;
        break;
    case NativeTypeKind::U16:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU16;
        node->computedValue->reg.u16 = *(uint16_t*) ptr;
        break;
    case NativeTypeKind::S32:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS32;
        node->computedValue->reg.s32 = *(int32_t*) ptr;
        break;
    case NativeTypeKind::U32:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU32;
        node->computedValue->reg.u32 = *(uint32_t*) ptr;
        break;
    case NativeTypeKind::F32:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoF32;
        node->computedValue->reg.f32 = *(float*) ptr;
        break;
    case NativeTypeKind::Rune:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoRune;
        node->computedValue->reg.ch = *(uint32_t*) ptr;
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS64;
        node->computedValue->reg.s64 = *(int64_t*) ptr;
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        node->computedValue->reg.u64 = *(uint64_t*) ptr;
        break;
    case NativeTypeKind::F64:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoF64;
        node->computedValue->reg.f64 = *(double*) ptr;
        break;
    case NativeTypeKind::Bool:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoBool;
        node->computedValue->reg.b = *(bool*) ptr;
        break;
    case NativeTypeKind::Any:
    {
        void** ppt         = (void**) ptr;
        void*  anyValue    = ppt[0];
        auto   anyTypeInfo = (ConcreteTypeInfo*) ppt[1];
        if (anyTypeInfo->kind == TypeInfoKind::Native)
        {
            auto anyNative = (ConcreteTypeInfoNative*) anyTypeInfo;
            node->typeInfo = nullptr;
            return derefConstantValue(context, node, anyNative->base.kind, anyNative->nativeKind, anyValue);
        }

        return false;
    }

    default:
        return false;
    }

    return true;
}

bool SemanticJob::derefLiteralStruct(SemanticContext* context, uint8_t* ptr, SymbolOverload* overload, DataSegment* storageSegment)
{
    auto node = context->node;

    ptr += overload->computedValue.storageOffset;
    node->typeInfo = overload->typeInfo;

    auto concreteType = TypeManager::concreteType(overload->typeInfo);
    if (concreteType->isPointer())
    {
        // :BackPtrOffset
        node->allocateComputedValue();
        node->computedValue->storageOffset  = storageSegment->offset(*(uint8_t**) ptr);
        node->computedValue->storageSegment = storageSegment;
        node->computedValue->reg.pointer    = (uint8_t*) overload->typeInfo;
        node->flags |= AST_VALUE_IS_TYPEINFO;
    }
    else if (concreteType->isArray())
    {
        node->allocateComputedValue();
        node->computedValue->storageOffset  = storageSegment->offset(ptr);
        node->computedValue->storageSegment = storageSegment;
        node->typeInfo                      = concreteType;
    }
    else if (concreteType->isSlice())
    {
        // Convert slice to a static constant array
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(concreteType, TypeInfoKind::Slice);
        auto ptrSlice  = (SwagSlice*) ptr;
        node->allocateComputedValue();
        node->computedValue->storageOffset  = storageSegment->offset((uint8_t*) ptrSlice->buffer);
        node->computedValue->storageSegment = storageSegment;
        node->computedValue->reg.u64        = ptrSlice->count;
        auto typeArray                      = allocType<TypeInfoArray>();
        typeArray->count                    = (uint32_t) ((SwagSlice*) ptr)->count;
        typeArray->totalCount               = typeArray->count;
        typeArray->pointedType              = typeSlice->pointedType;
        typeArray->finalType                = typeSlice->pointedType;
        typeArray->sizeOf                   = typeArray->totalCount * typeArray->pointedType->sizeOf;
        typeArray->computeName();
        node->typeInfo = typeArray;
    }
    else if (concreteType->isStruct())
    {
        node->allocateComputedValue();
        node->computedValue->storageOffset  = storageSegment->offset(ptr);
        node->computedValue->storageSegment = storageSegment;
        node->typeInfo                      = concreteType;
        setupIdentifierRef(context, node, concreteType);
    }
    else if (!derefConstantValue(context, node, concreteType, storageSegment, ptr))
    {
        return false;
    }

    node->setFlagsValueIsComputed();
    return true;
}

bool SemanticJob::derefLiteralStruct(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload)
{
    DataSegment* storageSegment;
    uint32_t     storageOffset = UINT32_MAX;

    auto prevNode = parent->previousResolvedNode;
    SWAG_ASSERT(prevNode->computedValue);
    SWAG_ASSERT(prevNode->computedValue->storageSegment);
    SWAG_ASSERT(prevNode->computedValue->storageOffset != UINT32_MAX);
    storageOffset  = prevNode->computedValue->storageOffset;
    storageSegment = prevNode->computedValue->storageSegment;
    SWAG_CHECK(derefLiteralStruct(context, storageSegment->address(storageOffset), overload, storageSegment));

    return true;
}
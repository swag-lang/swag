#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"

bool SemanticJob::boundCheck(SemanticContext* context, AstNode* arrayAccess, uint64_t maxCount)
{
    if (!(arrayAccess->flags & AST_VALUE_COMPUTED))
        return true;
    auto idx = arrayAccess->computedValue.reg.u64;
    if (idx >= maxCount)
        return context->report({arrayAccess, format(Msg0468, idx, maxCount - 1)});
    return true;
}

bool SemanticJob::checkCanMakeFuncPointer(SemanticContext* context, AstFuncDecl* funcNode, AstNode* node)
{
    const char* msg  = nullptr;
    const char* msg1 = nullptr;

    if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
    {
        msg  = Msg0471;
        msg1 = Hnt0015;
    }
    else if (funcNode->attributeFlags & ATTRIBUTE_MIXIN)
    {
        msg  = Msg0472;
        msg1 = Hnt0016;
    }
    else if (funcNode->attributeFlags & ATTRIBUTE_INLINE)
    {
        msg  = Msg0473;
        msg1 = Hnt0017;
    }

    if (msg)
    {
        PushErrHint errh(msg1);
        Diagnostic  diag{node, msg};
        Diagnostic  note{funcNode, funcNode->token, format(Msg0018, funcNode->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    return true;
}

bool SemanticJob::checkCanTakeAddress(SemanticContext* context, AstNode* node)
{
    if (node->kind != AstNodeKind::IdentifierRef && node->kind != AstNodeKind::ArrayPointerIndex)
        return context->report({node, Msg0470});

    if (!(node->flags & AST_L_VALUE))
    {
        if (node->resolvedSymbolName->kind != SymbolKind::Variable)
            return context->report({node, format(Msg0465, SymTable::getArticleKindName(node->resolvedSymbolName->kind))});
        return context->report({node, Msg0469});
    }

    return true;
}

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node     = context->node;
    auto child    = node->childs.front();
    auto typeInfo = child->typeInfo;

    SWAG_CHECK(checkCanTakeAddress(context, child));
    SWAG_CHECK(checkIsConcrete(context, child));
    node->flags |= AST_R_VALUE;

    // Lambda
    if (child->resolvedSymbolName->kind == SymbolKind::Function)
    {
        auto funcNode = child->resolvedSymbolOverload->node;
        SWAG_CHECK(checkCanMakeFuncPointer(context, (AstFuncDecl*) funcNode, child));

        auto lambdaType    = child->typeInfo->clone();
        lambdaType->kind   = TypeInfoKind::Lambda;
        lambdaType->sizeOf = sizeof(void*);
        node->typeInfo     = lambdaType;
        node->byteCodeFct  = ByteCodeGenJob::emitMakeLambda;
    }

    // Expression
    else
    {
        node->byteCodeFct = ByteCodeGenJob::emitMakePointer;

        // A new pointer
        TypeInfoPointer* ptrType = allocType<TypeInfoPointer>();

        // If this is a reference (struct as parameter), then pointer is just a const pointer
        // to the original type, and we do not have to generate specific bytecode.
        if (typeInfo->kind == TypeInfoKind::Reference)
        {
            typeInfo = TypeManager::concreteReference(typeInfo);
            child->semFlags |= AST_SEM_FORCE_NO_TAKE_ADDRESS;
            child->childs.back()->semFlags |= AST_SEM_FORCE_NO_TAKE_ADDRESS;
            node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
        }

        // If this is an array, then this is legit, the pointer will address the first
        // element : need to find it's type
        else if (typeInfo->kind == TypeInfoKind::Array)
        {
            while (typeInfo->kind == TypeInfoKind::Array)
            {
                auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
                typeInfo       = typeArray->pointedType;
            }
        }

        ptrType->pointedType = typeInfo;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->computeName();

        // Type is constant if we take address of a readonly variable
        if (child->resolvedSymbolOverload)
        {
            auto typeResolved = TypeManager::concreteType(child->resolvedSymbolOverload->typeInfo, CONCRETE_ALIAS);

            if ((child->resolvedSymbolOverload->flags & OVERLOAD_CONST_ASSIGN) && (typeResolved->kind != TypeInfoKind::Array))
                ptrType->setConst();

            if (typeResolved->isNative(NativeTypeKind::String))
                ptrType->setConst();
            else if (typeResolved->isConst() && typeResolved->kind == TypeInfoKind::Slice)
                ptrType->setConst();
            else if (node->flags & AST_IS_CONST)
                ptrType->setConst();
        }

        node->typeInfo = ptrType;
    }

    return true;
}

bool SemanticJob::resolveArrayPointerSlicing(SemanticContext* context)
{
    auto     node     = CastAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto     typeVar  = TypeManager::concreteReferenceType(node->array->typeInfo);
    uint64_t maxBound = 0;

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, node->lowerBound, CASTFLAG_TRY_COERCE));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, node->upperBound, CASTFLAG_TRY_COERCE));

    // Slicing of an array
    if (typeVar->kind == TypeInfoKind::Array)
    {
        auto typeInfoArray = CastTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);
        if (typeInfoArray->totalCount != typeInfoArray->count)
            return context->report({node->array, format(Msg0474, node->array->typeInfo->getDisplayName().c_str())});

        auto ptrSlice         = allocType<TypeInfoSlice>();
        ptrSlice->pointedType = typeInfoArray->finalType;
        if (typeInfoArray->isConst())
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->computeName();
        node->typeInfo = ptrSlice;
        maxBound       = typeInfoArray->count - 1;
    }

    // Slicing of a string
    else if (typeVar->isNative(NativeTypeKind::String))
    {
        node->typeInfo = typeVar;
        if (node->array->flags & AST_VALUE_COMPUTED)
            maxBound = node->array->computedValue.text.length();
    }

    // Slicing of a pointer
    else if (typeVar->kind == TypeInfoKind::Pointer)
    {
        auto typeInfoPointer  = CastTypeInfo<TypeInfoPointer>(node->array->typeInfo, TypeInfoKind::Pointer);
        auto ptrSlice         = allocType<TypeInfoSlice>();
        ptrSlice->pointedType = typeInfoPointer->pointedType;
        if (typeInfoPointer->isConst())
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->computeName();
        node->typeInfo = ptrSlice;
    }

    // Slicing of a... slice
    else if (typeVar->kind == TypeInfoKind::Slice)
    {
        node->typeInfo = typeVar;
    }

    // Slicing of a struct with a special function
    else if (typeVar->kind == TypeInfoKind::Struct)
    {
        // Flatten all operator parameters : self, then all indices
        node->structFlatParams.clear();
        node->structFlatParams.push_back(node->lowerBound);
        node->structFlatParams.push_back(node->upperBound);

        // Self in first position
        node->structFlatParams.push_front(node->array);

        // Resolve call
        auto typeInfo = node->array->typeInfo;
        if (!hasUserOp(context, "opSlice", node->array))
        {
            if (node->array->token.text.empty())
            {
                Utf8 msg = format("cannot slice because special function 'opSlice' cannot be found in type '%s'", typeInfo->getDisplayName().c_str());
                return context->report({node->array, msg});
            }
            else
            {
                Utf8 msg = format("cannot access '%s' by index because special function 'opIndex' cannot be found in type '%s'", node->array->token.text.c_str(), typeInfo->getDisplayName().c_str());
                return context->report({node->array, msg});
            }
        }

        SWAG_CHECK(resolveUserOp(context, "opSlice", nullptr, nullptr, node->array, node->structFlatParams, false));
    }
    else
    {
        return context->report({node->array, format(Msg0475, node->array->typeInfo->getDisplayName().c_str())});
    }

    // startBound <= endBound
    if ((node->lowerBound->flags & AST_VALUE_COMPUTED) && (node->upperBound->flags & AST_VALUE_COMPUTED))
    {
        if (node->lowerBound->computedValue.reg.u64 > node->upperBound->computedValue.reg.u64)
        {
            return context->report({node->lowerBound, format(Msg0476, node->lowerBound->computedValue.reg.u64, node->upperBound->computedValue.reg.u64)});
        }
    }

    // endBound < maxBound
    if (maxBound && (node->upperBound->flags & AST_VALUE_COMPUTED))
    {
        if (node->upperBound->computedValue.reg.u64 > maxBound)
        {
            return context->report({node->upperBound, format(Msg0477, node->upperBound->computedValue.reg.u64)});
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitMakeArrayPointerSlicing;
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
            if (node->extension && node->extension->resolvedUserOpSymbolOverload)
                typeReturn = TypeManager::concreteReferenceType(node->extension->resolvedUserOpSymbolOverload->typeInfo);

            // Get the pointed type if we have a pointer
            if (typeReturn->kind == TypeInfoKind::Pointer)
            {
                auto typePointer = CastTypeInfo<TypeInfoPointer>(typeReturn, TypeInfoKind::Pointer);
                typeReturn       = typePointer->pointedType;
            }

            // And this is is a struct, we fill the startScope
            if (typeReturn->kind == TypeInfoKind::Struct)
            {
                auto typeStruct    = CastTypeInfo<TypeInfoStruct>(typeReturn, TypeInfoKind::Struct);
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

    auto arrayType = TypeManager::concreteReferenceType(arrayNode->array->typeInfo, CONCRETE_ALIAS);

    // When we are building a pointer, this is fine to be const, because in fact we do no generate an address to modify the content
    // (or it will be done later on a pointer, and it will be const too)
    if (arrayNode->parent->parent->kind != AstNodeKind::MakePointer)
    {
        SWAG_VERIFY(!arrayType->isConst(), context->report({arrayNode->access, format(Msg0478, arrayType->getDisplayName().c_str())}));
    }
    else
    {
        // If array is const, inform the make pointer that it need to make a const pointer
        if (arrayType->isConst())
            arrayNode->parent->parent->flags |= AST_IS_CONST;
    }

    auto accessType = TypeManager::concreteReferenceType(arrayNode->access->typeInfo);
    if (!(accessType->flags & TYPEINFO_INTEGER) && !(accessType->flags & TYPEINFO_ENUM_INDEX))
        return context->report({arrayNode->access, format(Msg0485, arrayNode->access->typeInfo->getDisplayName().c_str())});

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePtr->pointedType != g_TypeMgr.typeInfoVoid, context->report({arrayNode, Msg0486}));
        arrayNode->typeInfo = typePtr->pointedType;
        arrayNode->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->array->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerRef;
        break;
    }

    case TypeInfoKind::Native:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        if (arrayType->nativeType == NativeTypeKind::String)
        {
            arrayNode->typeInfo    = g_TypeMgr.typeInfoU8;
            arrayNode->byteCodeFct = ByteCodeGenJob::emitStringRef;
        }
        else
        {
            return context->report({arrayNode->array, format(Msg0481, arrayType->getDisplayName().c_str())});
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr           = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitArrayRef;

        // Try to dereference as a constant if we can
        uint32_t     storageOffset  = UINT32_MAX;
        DataSegment* storageSegment = nullptr;
        SWAG_CHECK(getConstantArrayPtr(context, &storageOffset, &storageSegment));
        if (storageSegment)
        {
            arrayNode->computedValue.storageOffset  = storageOffset;
            arrayNode->computedValue.storageSegment = storageSegment;
            if (arrayNode->resolvedSymbolOverload)
            {
                SWAG_ASSERT(arrayNode->resolvedSymbolOverload->computedValue.storageSegment == storageSegment);
                arrayNode->resolvedSymbolOverload->computedValue.storageOffset = storageOffset;
            }

            arrayNode->setFlagsValueIsComputed();
        }
        else
        {
            SWAG_CHECK(boundCheck(context, arrayNode->access, typePtr->count));
        }
        break;
    }

    case TypeInfoKind::Slice:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr           = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitSliceRef;
        break;
    }

    case TypeInfoKind::Struct:
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        if (arrayType->flags & TYPEINFO_STRUCT_IS_TUPLE)
            return context->report({arrayNode->array, Msg0482});

        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex)
        {
            arrayNode->typeInfo = arrayType;
        }
        else
        {
            // Flatten all indices. self and value will be set before the call later
            arrayNode->typeInfo = arrayType;
            arrayNode->structFlatParams.clear();
            arrayNode->structFlatParams.push_back(arrayNode->access);

            AstNode* child = arrayNode->array;
            while (child->kind == AstNodeKind::ArrayPointerIndex)
            {
                auto arrayChild = CastAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
                arrayNode->structFlatParams.push_front(arrayChild->access);
                child = arrayChild->array;
            }
        }
        break;

    default:
    {
        return context->report({arrayNode->array, format(Msg0483, arrayType->getDisplayName().c_str())});
    }
    }

    return true;
}

bool SemanticJob::getConstantArrayPtr(SemanticContext* context, uint32_t* storageOffset, DataSegment** storageSegment)
{
    auto arrayNode = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayType = TypeManager::concreteReferenceType(arrayNode->array->typeInfo);
    auto typePtr   = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);

    if (arrayNode->typeInfo->kind != TypeInfoKind::Array && arrayNode->access->flags & AST_VALUE_COMPUTED)
    {
        bool     isConstAccess = true;
        uint64_t offsetAccess  = arrayNode->access->computedValue.reg.u64 * typePtr->finalType->sizeOf;
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
                offsetAccess += subArray->access->computedValue.reg.u64 * subTypePtr->pointedType->sizeOf;
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
        }
    }

    return true;
}

bool SemanticJob::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto arrayNode         = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayAccess       = arrayNode->access;
    auto arrayType         = TypeManager::concreteReferenceType(arrayNode->array->typeInfo);
    arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerDeRef;

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));

    if (arrayType->flags & TYPEINFO_STRUCT_IS_TUPLE)
        return context->report({arrayAccess, Msg0482});

    arrayNode->flags |= AST_R_VALUE;

    auto accessType = TypeManager::concreteReferenceType(arrayNode->access->typeInfo);
    if (!(accessType->flags & TYPEINFO_INTEGER) && !(accessType->flags & TYPEINFO_ENUM_INDEX))
        return context->report({arrayNode->access, format(Msg0485, arrayNode->access->typeInfo->getDisplayName().c_str())});

    // Do not set resolvedSymbolOverload !
    arrayNode->resolvedSymbolName = arrayNode->array->resolvedSymbolName;

    // Can we dereference at compile time ?
    if (arrayType->isNative(NativeTypeKind::String))
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        if (arrayNode->access->flags & AST_VALUE_COMPUTED)
        {
            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                arrayNode->setFlagsValueIsComputed();
                auto& text = arrayNode->array->resolvedSymbolOverload->computedValue.text;
                SWAG_CHECK(boundCheck(context, arrayNode->access, text.length()));
                auto idx                        = arrayAccess->computedValue.reg.u32;
                arrayNode->computedValue.reg.u8 = text[idx];
            }
        }

        arrayNode->typeInfo = g_TypeMgr.typeInfoU8;
        return true;
    }

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePtr->pointedType != g_TypeMgr.typeInfoVoid, context->report({arrayNode, Msg0486}));
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Array:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
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
            if (derefConstantValue(context, arrayNode, typePtr->finalType->kind, typePtr->finalType->nativeType, typePtr->finalType->relative, ptr))
                arrayNode->setFlagsValueIsComputed();
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typePtr        = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);

        // Try to dereference as a constant if we can
        if (arrayNode->access->flags & AST_VALUE_COMPUTED)
        {
            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                SWAG_CHECK(boundCheck(context, arrayNode->access, arrayNode->array->computedValue.reg.u32));
                SWAG_ASSERT(arrayNode->array->resolvedSymbolOverload->computedValue.storageOffset != UINT32_MAX);
                auto ptr = context->sourceFile->module->constantSegment.address(arrayNode->array->resolvedSymbolOverload->computedValue.storageOffset);
                ptr += arrayNode->access->computedValue.reg.u64 * typePtr->pointedType->sizeOf;
                if (derefConstantValue(context, arrayNode, typePtr->pointedType->kind, typePtr->pointedType->nativeType, typePtr->pointedType->relative, ptr))
                    arrayNode->setFlagsValueIsComputed();
            }
        }

        break;
    }

    case TypeInfoKind::Variadic:
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        arrayNode->typeInfo = g_TypeMgr.typeInfoAny;
        break;

    case TypeInfoKind::TypedVariadic:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));
        auto typeVariadic   = CastTypeInfo<TypeInfoVariadic>(arrayType, TypeInfoKind::TypedVariadic);
        arrayNode->typeInfo = typeVariadic->rawType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Struct:
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, arrayNode->access, CASTFLAG_TRY_COERCE | CASTFLAG_INDEX));

        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex)
        {
            arrayNode->typeInfo = arrayType;
            break;
        }

        // Flatten all operator parameters : self, then all indices
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
        auto typeInfo = arrayNode->array->typeInfo;
        if (!hasUserOp(context, "opIndex", arrayNode->array))
        {
            if (arrayNode->array->token.text.empty())
            {
                Utf8 msg = format("cannot access by index because special function 'opIndex' cannot be found in type '%s'", typeInfo->getDisplayName().c_str());
                return context->report({arrayNode->access, msg});
            }
            else
            {
                Utf8 msg = format("cannot access '%s' by index because special function 'opIndex' cannot be found in type '%s'", arrayNode->array->token.text.c_str(), typeInfo->getDisplayName().c_str());
                return context->report({arrayNode->access, msg});
            }
        }

        SWAG_CHECK(resolveUserOp(context, "opIndex", nullptr, nullptr, arrayNode->array, arrayNode->structFlatParams, false));
        break;
    }

    default:
    {
        PushErrHint errh(Msg0487);
        return context->report({arrayNode->array, format(Msg0488, TypeInfo::getNakedKindName(arrayType), arrayType->getDisplayName().c_str())});
    }
    }

    return true;
}

bool SemanticJob::resolveInit(SemanticContext* context)
{
    auto job                = context->job;
    auto node               = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format(Msg0489, expressionTypeInfo->getDisplayName().c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format(Msg0490, countTypeInfo->getDisplayName().c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, node->count, CASTFLAG_TRY_COERCE));
    }

    if (node->parameters)
    {
        auto typeinfoPointer = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        auto pointedType     = typeinfoPointer->pointedType;

        if (pointedType->kind == TypeInfoKind::Native || pointedType->kind == TypeInfoKind::Pointer)
        {
            SWAG_VERIFY(node->parameters->childs.size() == 1, context->report({node->count, format(Msg0491, pointedType->getDisplayName().c_str())}));
            auto child = node->parameters->childs.front();
            SWAG_CHECK(TypeManager::makeCompatibles(context, pointedType, child->typeInfo, nullptr, child));
        }
        else if (pointedType->kind == TypeInfoKind::Struct)
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
                    unique_lock lk(symbol->mutex);
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
            return internalError(context, "resolveInit, invalid type");
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitInit;
    return true;
}

bool SemanticJob::resolveDropCopyMove(SemanticContext* context)
{
    auto node               = CastAst<AstDropCopyMove>(context->node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format(Msg0495, node->token.text.c_str(), expressionTypeInfo->getDisplayName().c_str())}));

    // Be sure struct if not marked as nocopy
    if (node->kind == AstNodeKind::PostCopy)
    {
        auto ptrType     = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        auto pointedType = TypeManager::concreteType(ptrType->pointedType);
        if (pointedType->flags & TYPEINFO_STRUCT_NO_COPY)
        {
            return context->report({node->expression, format(Msg0493, pointedType->getDisplayName().c_str())});
        }
    }

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format(Msg0498, node->token.text.c_str(), countTypeInfo->getDisplayName().c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, node->count, CASTFLAG_TRY_COERCE));
    }

    node->byteCodeFct = ByteCodeGenJob::emitDropCopyMove;
    return true;
}

bool SemanticJob::resolveReloc(SemanticContext* context)
{
    auto node                = CastAst<AstReloc>(context->node, AstNodeKind::Reloc);
    auto expression1TypeInfo = TypeManager::concreteType(node->expression1->typeInfo);
    auto expression2TypeInfo = TypeManager::concreteType(node->expression2->typeInfo);

    SWAG_VERIFY(expression1TypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression1, format(Msg0495, node->token.text.c_str(), expression1TypeInfo->getDisplayName().c_str())}));
    SWAG_VERIFY(expression2TypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression2, format(Msg0496, node->token.text.c_str(), expression2TypeInfo->getDisplayName().c_str())}));
    auto ptrType1 = CastTypeInfo<TypeInfoPointer>(expression1TypeInfo, TypeInfoKind::Pointer);
    auto ptrType2 = CastTypeInfo<TypeInfoPointer>(expression2TypeInfo, TypeInfoKind::Pointer);
    SWAG_VERIFY(ptrType1->pointedType == ptrType2->pointedType, context->report({node->expression2, format(Msg0497, node->token.text.c_str(), expression1TypeInfo->getDisplayName().c_str(), expression2TypeInfo->getDisplayName().c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format(Msg0498, node->token.text.c_str(), countTypeInfo->getDisplayName().c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, nullptr, node->count, CASTFLAG_TRY_COERCE));
    }

    node->byteCodeFct = ByteCodeGenJob::emitReloc;
    return true;
}

bool SemanticJob::derefConstantValue(SemanticContext* context, AstNode* node, TypeInfoKind kind, NativeTypeKind nativeKind, uint8_t relative, void* ptr)
{
    if (kind != TypeInfoKind::Native)
        return false;

    switch (nativeKind)
    {
    case NativeTypeKind::String:
        if (relative)
        {
            SWAG_ASSERT(relative == 8);
            ptr                      = RELATIVE_PTR64(ptr);
            node->computedValue.text = (const char*) ptr;
        }
        else
        {
            node->computedValue.text = *(const char**) ptr;
        }

        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoString;
        break;

    case NativeTypeKind::S8:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoS8;
        node->computedValue.reg.s8 = *(int8_t*) ptr;
        break;
    case NativeTypeKind::U8:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoU8;
        node->computedValue.reg.u8 = *(uint8_t*) ptr;
        break;
    case NativeTypeKind::S16:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoS16;
        node->computedValue.reg.s16 = *(int16_t*) ptr;
        break;
    case NativeTypeKind::U16:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoU16;
        node->computedValue.reg.u16 = *(uint16_t*) ptr;
        break;
    case NativeTypeKind::S32:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoS32;
        node->computedValue.reg.s32 = *(int32_t*) ptr;
        break;
    case NativeTypeKind::U32:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoU32;
        node->computedValue.reg.u32 = *(uint32_t*) ptr;
        break;
    case NativeTypeKind::F32:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoF32;
        node->computedValue.reg.f32 = *(float*) ptr;
        break;
    case NativeTypeKind::Rune:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoRune;
        node->computedValue.reg.ch = *(uint32_t*) ptr;
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoS64;
        node->computedValue.reg.s64 = *(int64_t*) ptr;
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoU64;
        node->computedValue.reg.u64 = *(uint64_t*) ptr;
        break;
    case NativeTypeKind::F64:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoF64;
        node->computedValue.reg.f64 = *(double*) ptr;
        break;
    case NativeTypeKind::Bool:
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr.typeInfoBool;
        node->computedValue.reg.b = *(bool*) ptr;
        break;
    case NativeTypeKind::Any:
    {
        void**            ppt         = (void**) ptr;
        void*             anyValue    = ppt[0];
        ConcreteTypeInfo* anyTypeInfo = (ConcreteTypeInfo*) ppt[1];
        if (anyTypeInfo->kind == TypeInfoKind::Native)
        {
            ConcreteTypeInfoNative* anyNative = (ConcreteTypeInfoNative*) anyTypeInfo;
            node->typeInfo                    = nullptr;
            return derefConstantValue(context, node, anyNative->base.kind, anyNative->nativeKind, 0, anyValue);
        }

        return false;
    }

    default:
        return false;
    }

    return true;
}

bool SemanticJob::derefLiteralStruct(SemanticContext* context, uint8_t* ptr, SymbolOverload* overload, DataSegment* segment)
{
    auto node = context->node;

    ptr += overload->computedValue.storageOffset;
    node->typeInfo = overload->typeInfo;

    auto concreteType = TypeManager::concreteType(overload->typeInfo);
    if (concreteType->kind == TypeInfoKind::Pointer)
    {
        auto relPtr                        = (uint8_t*) RELATIVE_PTR64(ptr);
        node->computedValue.storageOffset  = segment->offset(relPtr);
        node->computedValue.storageSegment = segment;
        node->flags |= AST_VALUE_IS_TYPEINFO;
    }
    else if (!derefConstantValue(context, node, concreteType->kind, concreteType->nativeType, concreteType->relative, ptr))
    {
        return false;
    }

    node->setFlagsValueIsComputed();
    return true;
}

bool SemanticJob::derefLiteralStruct(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload, DataSegment* segment)
{
    uint32_t storageOffset = UINT32_MAX;
    auto     prevNode      = parent->previousResolvedNode;
    if (prevNode->resolvedSymbolOverload)
        storageOffset = prevNode->resolvedSymbolOverload->computedValue.storageOffset;
    else
        storageOffset = prevNode->computedValue.storageOffset;
    SWAG_ASSERT(storageOffset != UINT32_MAX);
    SWAG_CHECK(derefLiteralStruct(context, segment->address(storageOffset), overload, segment));
    return true;
}
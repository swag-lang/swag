#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"

bool SemanticJob::boundCheck(SemanticContext* context, AstNode* arrayAccess, uint32_t maxCount)
{
    if (!(arrayAccess->flags & AST_VALUE_COMPUTED))
        return true;
    auto idx = arrayAccess->computedValue.reg.u32;
    if (idx >= (uint32_t) maxCount)
        return context->report({arrayAccess, format("index out of range (index is '%u', maximum index is '%u')", idx, maxCount - 1)});
    return true;
}

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node     = context->node;
    auto child    = node->childs.front();
    auto typeInfo = child->typeInfo;

    SWAG_VERIFY(child->flags & AST_L_VALUE, context->report({child, "cannot take address of expression"}));
    if (child->kind != AstNodeKind::IdentifierRef && child->kind != AstNodeKind::ArrayPointerIndex)
        return context->report({child, "invalid address expression"});

    SWAG_CHECK(checkIsConcrete(context, child));
    node->flags |= AST_R_VALUE;

    // Lambda
    if (child->resolvedSymbolName->kind == SymbolKind::Function)
    {
        SWAG_VERIFY(!(child->resolvedSymbolOverload->node->attributeFlags & ATTRIBUTE_INLINE), context->report({child, "cannot take address of an inline function"}));

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

        TypeInfoPointer* ptrType = nullptr;

        // Pointer on a pointer
        if (typeInfo->kind == TypeInfoKind::Pointer)
        {
            TypeInfoPointer* typeInfoPtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            ptrType                      = (TypeInfoPointer*) typeInfoPtr->clone();
            ptrType->ptrCount++;
            ptrType->computeName();
        }

        // Else new pointer
        else
        {
            ptrType           = g_Allocator.alloc<TypeInfoPointer>();
            ptrType->ptrCount = 1;

            // If this is an array, then this is legit, the pointer will address the first
            // element : need to find it's type
            while (typeInfo->kind == TypeInfoKind::Array)
            {
                auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
                typeInfo       = typeArray->pointedType;
            }

            ptrType->finalType   = typeInfo;
            ptrType->pointedType = typeInfo;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->computeName();
        }

        // Type is constant if we take address of a readonly variable
        if (child->resolvedSymbolOverload &&
            (child->resolvedSymbolOverload->flags & OVERLOAD_CONST_ASSIGN) &&
            (child->resolvedSymbolOverload->typeInfo->kind != TypeInfoKind::Array))
            ptrType->setConst();
        if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->typeInfo->isNative(NativeTypeKind::String))
            ptrType->setConst();

        node->typeInfo = ptrType;
    }

    return true;
}

bool SemanticJob::resolveArrayPointerSlicing(SemanticContext* context)
{
    auto     node     = CastAst<AstArrayPointerSlicing>(context->node, AstNodeKind::ArrayPointerSlicing);
    auto     typeVar  = node->array->typeInfo;
    uint32_t maxBound = 0;

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, nullptr, node->lowerBound));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, nullptr, node->upperBound));

    // Slicing of an array
    if (typeVar->kind == TypeInfoKind::Array)
    {
        auto typeInfoArray = CastTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);
        if (typeInfoArray->totalCount != typeInfoArray->count)
            return context->report({node->array, format("slicing operator cannot be applied on an array with multiple dimensions", node->array->typeInfo->name.c_str())});

        auto ptrSlice         = g_Allocator.alloc<TypeInfoSlice>();
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
        auto ptrSlice         = g_Allocator.alloc<TypeInfoSlice>();
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
            if (node->array->name.empty())
            {
                Utf8 msg = format("cannot slice because special function 'opSlice' cannot be found in type '%s'", typeInfo->name.c_str());
                return context->report({node->array, msg});
            }
            else
            {
                Utf8 msg = format("cannot access '%s' by index because special function 'opIndex' cannot be found in type '%s'", node->array->name.c_str(), typeInfo->name.c_str());
                return context->report({node->array, msg});
            }
        }

        SWAG_CHECK(resolveUserOp(context, "opSlice", nullptr, nullptr, node->array, node->structFlatParams, false));
    }
    else
    {
        return context->report({node->array, format("slicing operator cannot be applied on type '%s'", node->array->typeInfo->name.c_str())});
    }

    // startBound <= endBound
    if ((node->lowerBound->flags & AST_VALUE_COMPUTED) && (node->upperBound->flags & AST_VALUE_COMPUTED))
    {
        if (node->lowerBound->computedValue.reg.u32 > node->upperBound->computedValue.reg.u32)
        {
            return context->report({node->lowerBound, format("bad slicing, lower bound '%d' is greater than upper bound '%d'", node->lowerBound->computedValue.reg.u32, node->upperBound->computedValue.reg.u32)});
        }
    }

    // endBound < maxBound
    if (maxBound && (node->upperBound->flags & AST_VALUE_COMPUTED))
    {
        if (node->upperBound->computedValue.reg.u32 > maxBound)
        {
            return context->report({node->upperBound, format("bad slicing, upper bound '%d' is out of range", node->upperBound->computedValue.reg.u32)});
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitMakeSlice;
    return true;
}

bool SemanticJob::resolveArrayPointerIndex(SemanticContext* context)
{
    auto node = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);

    if (node->flags & AST_TAKE_ADDRESS)
    {
        SWAG_CHECK(resolveArrayPointerRef(context));
    }
    else
    {
        SWAG_CHECK(resolveArrayPointerDeRef(context));
    }

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
                node->flags |= AST_TAKE_ADDRESS;

            // In order to resolve what's next, we need to fill the startScope of the identifier ref
            auto typeReturn = node->array->typeInfo;

            // There's a 'opIndex' function
            if (node->resolvedUserOpSymbolOverload)
                typeReturn = TypeManager::concreteReferenceType(node->resolvedUserOpSymbolOverload->typeInfo);

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

    auto arrayType = TypeManager::concreteType(arrayNode->array->typeInfo, CONCRETE_ALIAS);

    // When we are building a pointer, this is fine to be const, because in fact we do no generate an address to modify the content
    // (or it will be done later on a pointer, and it will be const too)
    if (arrayNode->parent->parent->kind != AstNodeKind::MakePointer)
    {
        SWAG_VERIFY(!arrayType->isConst(), context->report({arrayNode->access, format("type '%s' is immutable and cannot be changed", arrayType->name.c_str())}));
    }

    // Promote to 32 or 64 bits
    SWAG_CHECK(TypeManager::promoteOne(context, arrayNode->access));

    auto accessType = TypeManager::concreteType(arrayNode->access->typeInfo);
    if (!(accessType->flags & TYPEINFO_INTEGER))
        return context->report({arrayNode->access, format("array access type should be integer ('%s' provided)", arrayNode->access->typeInfo->name.c_str())});

    switch (arrayType->kind)
    {
    case TypeInfoKind::Pointer:
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(arrayType, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePtr->ptrCount != 1 || typePtr->finalType != g_TypeMgr.typeInfoVoid, context->report({arrayNode, "cannot dereference a 'void' pointer"}));
        if (typePtr->ptrCount == 1)
            arrayNode->typeInfo = typePtr->finalType;
        else
        {
            auto newTypePtr = (TypeInfoPointer*) typePtr->clone();
            newTypePtr->ptrCount--;
            newTypePtr->computeName();
            newTypePtr->computePointedType();
            arrayNode->typeInfo = newTypePtr;
        }

        arrayNode->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->array->flags |= AST_ARRAY_POINTER_REF;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerRef;
        break;
    }

    case TypeInfoKind::Native:
    {
        if (arrayType->nativeType == NativeTypeKind::String)
        {
            arrayNode->typeInfo    = g_TypeMgr.typeInfoU8;
            arrayNode->byteCodeFct = ByteCodeGenJob::emitStringRef;
        }
        else
        {
            return context->report({arrayNode->array, format("cannot dereference type '%s'", arrayType->name.c_str())});
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        auto typePtr           = CastTypeInfo<TypeInfoArray>(arrayType, TypeInfoKind::Array);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitArrayRef;
        SWAG_CHECK(boundCheck(context, arrayNode->access, typePtr->count));
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typePtr           = CastTypeInfo<TypeInfoSlice>(arrayType, TypeInfoKind::Slice);
        arrayNode->typeInfo    = typePtr->pointedType;
        arrayNode->byteCodeFct = ByteCodeGenJob::emitSliceRef;
        break;
    }

    case TypeInfoKind::Struct:
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
        return context->report({arrayNode->array, format("cannot dereference type '%s'", arrayType->name.c_str())});
    }
    }

    return true;
}

bool SemanticJob::resolveArrayPointerDeRef(SemanticContext* context)
{
    auto arrayNode         = CastAst<AstArrayPointerIndex>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayAccess       = arrayNode->access;
    auto arrayType         = TypeManager::concreteType(arrayNode->array->typeInfo);
    arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerDeRef;

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));

    if (arrayType->flags & TYPEINFO_STRUCT_IS_TUPLE)
        return context->report({arrayAccess, "cannot dereference a tuple type"});

    arrayNode->flags |= AST_R_VALUE;

    // Promote to 32 or 64 bits
    SWAG_CHECK(TypeManager::promoteOne(context, arrayNode->access));

    auto accessType = TypeManager::concreteReferenceType(arrayNode->access->typeInfo);
    if (!(accessType->flags & TYPEINFO_INTEGER) && !(accessType->flags & TYPEINFO_ENUM_INDEX))
        return context->report({arrayNode->access, format("array access type should be integer ('%s' provided)", arrayNode->access->typeInfo->name.c_str())});

    arrayNode->resolvedSymbolName = arrayNode->array->resolvedSymbolName;

    // Can we dereference the string at compile time ?
    if (arrayType->isNative(NativeTypeKind::String))
    {
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
        auto typePtr = static_cast<TypeInfoPointer*>(arrayType);
        SWAG_VERIFY(typePtr->ptrCount != 1 || typePtr->finalType != g_TypeMgr.typeInfoVoid, context->report({arrayNode, "cannot dereference a 'void' pointer"}));
        if (typePtr->ptrCount == 1)
        {
            arrayNode->typeInfo = typePtr->finalType;
        }
        else
        {
            auto newType = static_cast<TypeInfoPointer*>(typePtr->clone());
            newType->ptrCount--;
            newType->computeName();
            newType->computePointedType();
            arrayNode->typeInfo = newType;
        }

        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Array:
    {
        auto typePtr        = static_cast<TypeInfoArray*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);

        // Try to dereference as a constant if we can
        if (arrayNode->typeInfo->kind != TypeInfoKind::Array && (arrayNode->access->flags & AST_VALUE_COMPUTED))
        {
            SWAG_CHECK(boundCheck(context, arrayNode->access, typePtr->count));
            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                SWAG_ASSERT(arrayNode->array->resolvedSymbolOverload->storageOffset != UINT32_MAX);
                auto ptr = context->sourceFile->module->constantSegment.address(arrayNode->array->resolvedSymbolOverload->storageOffset);
                ptr += arrayNode->access->computedValue.reg.u32 * typePtr->finalType->sizeOf;
                if (derefConstantValue(context, arrayNode, typePtr->finalType->kind, typePtr->finalType->nativeType, ptr))
                    arrayNode->setFlagsValueIsComputed();
            }
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typePtr        = static_cast<TypeInfoSlice*>(arrayType);
        arrayNode->typeInfo = typePtr->pointedType;
        setupIdentifierRef(context, arrayNode, arrayNode->typeInfo);
        break;
    }

    case TypeInfoKind::Variadic:
        arrayNode->typeInfo = g_TypeMgr.typeInfoAny;
        break;

    case TypeInfoKind::TypedVariadic:
    {
        auto typeVariadic   = static_cast<TypeInfoVariadic*>(arrayType);
        arrayNode->typeInfo = typeVariadic->rawType;
        break;
    }

    case TypeInfoKind::Struct:
        // Only the top level ArrayPointerIndex node will deal with the call
        if (arrayNode->parent->kind == AstNodeKind::ArrayPointerIndex)
        {
            arrayNode->typeInfo = arrayType;
        }
        else
        {
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
                if (arrayNode->array->name.empty())
                {
                    Utf8 msg = format("cannot access by index because special function 'opIndex' cannot be found in type '%s'", typeInfo->name.c_str());
                    return context->report({arrayNode->access, msg});
                }
                else
                {
                    Utf8 msg = format("cannot access '%s' by index because special function 'opIndex' cannot be found in type '%s'", arrayNode->array->name.c_str(), typeInfo->name.c_str());
                    return context->report({arrayNode->access, msg});
                }
            }

            SWAG_CHECK(resolveUserOp(context, "opIndex", nullptr, nullptr, arrayNode->array, arrayNode->structFlatParams, false));
        }
        break;

    default:
        return context->report({arrayNode->array, format("%s '%s' cannot be referenced like a pointer", TypeInfo::getNakedKindName(arrayType), arrayType->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveInit(SemanticContext* context)
{
    auto node               = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format("'@init' first parameter should be a pointer, but is '%s'", expressionTypeInfo->name.c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format("'@init' count parameter should be an integer, but is '%s'", countTypeInfo->name.c_str())}));
        SWAG_VERIFY(countTypeInfo->sizeOf <= 4, context->report({node->count, "'@init' count parameter should be 32 bits"}));
    }

    if (node->parameters)
    {
        auto typeinfoPointer = CastTypeInfo<TypeInfoPointer>(expressionTypeInfo, TypeInfoKind::Pointer);
        auto pointedType     = typeinfoPointer->pointedType;

        if (pointedType->kind == TypeInfoKind::Native || pointedType->kind == TypeInfoKind::Pointer)
        {
            SWAG_VERIFY(node->parameters->childs.size() == 1, context->report({node->count, format("too many initialization parameters for type '%s'", pointedType->name.c_str())}));
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

            while (true)
            {
                vector<OneTryMatch> listTryMatch;
                auto                symbol = typeStruct->declNode->resolvedSymbolName;

                {
                    unique_lock lk(symbol->mutex);
                    for (auto overload : symbol->overloads)
                    {
                        OneTryMatch t;
                        t.symMatchContext   = symMatchContext;
                        t.overload          = overload;
                        t.genericParameters = nullptr;
                        t.callParameters    = node->parameters;
                        t.dependentVar      = nullptr;
                        t.cptOverloads      = (uint32_t) symbol->overloads.size();
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

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format("'%s' first parameter should be a pointer to a struct, but is '%s'", node->name.c_str(), expressionTypeInfo->name.c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format("'%s' count parameter should be an integer, but is '%s'", node->name.c_str(), countTypeInfo->name.c_str())}));
        SWAG_VERIFY(countTypeInfo->sizeOf <= 4, context->report({node->count, format("'%s' count parameter should be 32 bits", node->name.c_str())}));
    }

    node->byteCodeFct = ByteCodeGenJob::emitDropCopyMove;
    return true;
}

bool SemanticJob::derefConstantValue(SemanticContext* context, AstNode* node, TypeInfoKind kind, NativeTypeKind nativeKind, void* ptr)
{
    if (kind == TypeInfoKind::Native)
    {
        switch (nativeKind)
        {
        case NativeTypeKind::String:
            node->computedValue.text = *(const char**) ptr;
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
        case NativeTypeKind::Char:
            if (!node->typeInfo)
                node->typeInfo = g_TypeMgr.typeInfoChar;
            node->computedValue.reg.ch = *(uint32_t*) ptr;
            break;
        case NativeTypeKind::S64:
            if (!node->typeInfo)
                node->typeInfo = g_TypeMgr.typeInfoS64;
            node->computedValue.reg.s64 = *(int64_t*) ptr;
            break;
        case NativeTypeKind::U64:
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
                return derefConstantValue(context, node, anyNative->base.kind, anyNative->nativeKind, anyValue);
            }

            return false;
        }

        default:
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool SemanticJob::derefLiteralStruct(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload, DataSegment* segment)
{
    auto node = context->node;

    uint32_t storageOffset = UINT32_MAX;
    if (parent->previousResolvedNode->resolvedSymbolOverload)
        storageOffset = parent->previousResolvedNode->resolvedSymbolOverload->storageOffset;
    else
        storageOffset = parent->previousResolvedNode->computedValue.reg.u32;
    SWAG_ASSERT(storageOffset != UINT32_MAX);

    uint8_t* ptr = segment->address(storageOffset);
    ptr += overload->storageOffset;
    node->typeInfo = overload->typeInfo;

    auto concreteType = TypeManager::concreteType(overload->typeInfo);
    if (concreteType->kind == TypeInfoKind::Pointer)
    {
        node->computedValue.reg.offset = segment->offset(*(uint8_t**) ptr);
        node->flags |= AST_VALUE_IS_TYPEINFO;
    }
    else if (!derefConstantValue(context, node, concreteType->kind, concreteType->nativeType, ptr))
    {
        return false;
    }

    node->setFlagsValueIsComputed();
    return true;
}
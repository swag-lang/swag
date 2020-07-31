#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "SymTable.h"
#include "Module.h"
#include "TypeManager.h"

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

bool SemanticJob::resolveArrayPointerIndex(SemanticContext* context)
{
    auto node = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);

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
                typeReturn = TypeManager::concreteType(node->resolvedUserOpSymbolOverload->typeInfo);

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
    auto arrayNode                    = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    arrayNode->resolvedSymbolName     = arrayNode->array->resolvedSymbolName;
    arrayNode->resolvedSymbolOverload = arrayNode->array->resolvedSymbolOverload;
    arrayNode->inheritOrFlag(arrayNode->array, AST_L_VALUE);

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));
    arrayNode->flags |= AST_R_VALUE;

    auto arrayType = arrayNode->array->typeInfo;

    // When we are building a pointer, this is fine to be const, be cause in fact we do no generate an address to modify the content
    // (or it will be done later on a pointer, and it will be const too)
    if (arrayNode->parent->parent->kind != AstNodeKind::MakePointer)
    {
        SWAG_VERIFY(!arrayType->isConst(), context->report({arrayNode->access, format("type '%s' is immutable and cannot be changed", arrayType->name.c_str())}));
    }

    auto accessType = TypeManager::concreteType(arrayNode->access->typeInfo);
    if (!(accessType->flags & TYPEINFO_INTEGER))
        return context->report({arrayNode->access, format("access type should be integer, not '%s'", arrayNode->access->typeInfo->name.c_str())});

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
                auto arrayChild = CastAst<AstPointerDeRef>(child, AstNodeKind::ArrayPointerIndex);
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
    auto arrayNode         = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    auto arrayAccess       = arrayNode->access;
    auto arrayType         = TypeManager::concreteType(arrayNode->array->typeInfo);
    arrayNode->byteCodeFct = ByteCodeGenJob::emitPointerDeRef;

    SWAG_CHECK(checkIsConcrete(context, arrayNode->array));
    SWAG_CHECK(checkIsConcrete(context, arrayNode->access));

    if (arrayType->flags & TYPEINFO_STRUCT_IS_TUPLE)
        return context->report({arrayAccess, "cannot dereference a tuple type"});

    arrayNode->flags |= AST_R_VALUE;

    if (!(arrayNode->access->typeInfo->flags & TYPEINFO_INTEGER))
        return context->report({arrayNode->array, format("access type should be integer, not '%s'", arrayNode->access->typeInfo->name.c_str())});

    arrayNode->resolvedSymbolName = arrayNode->array->resolvedSymbolName;

    switch (arrayType->kind)
    {
    case TypeInfoKind::Native:
        // Can we just change the computed value to a single u8 ?
        if (arrayType->nativeType == NativeTypeKind::String)
        {
            if (arrayNode->access->flags & AST_VALUE_COMPUTED)
            {
                if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
                {
                    arrayNode->setFlagsValueIsComputed();
                    auto& text = arrayNode->array->resolvedSymbolOverload->computedValue.text;
                    switch (arrayAccess->typeInfo->nativeType)
                    {
                    case NativeTypeKind::S32:
                    {
                        auto idx = arrayAccess->computedValue.reg.s32;
                        if (idx < 0)
                            return context->report({arrayNode->access, format("index is a negative value ('%d')", idx)});
                        else if (idx >= text.length())
                            return context->report({arrayNode->access, format("index out of range (index is '%d', maximum index is '%u')", idx, text.length() - 1)});
                        arrayNode->computedValue.reg.u8 = text[idx];
                        break;
                    }
                    case NativeTypeKind::U32:
                    {
                        auto idx = arrayAccess->computedValue.reg.u32;
                        if (idx >= (uint32_t) text.length())
                            return context->report({arrayNode->access, format("index out of range (index is '%u', maximum index is '%u')", idx, text.length() - 1)});
                        arrayNode->computedValue.reg.u8 = text[idx];
                        break;
                    }
                    default:
                        SWAG_ASSERT(false);
                        break;
                    }
                }
            }

            arrayNode->typeInfo = g_TypeMgr.typeInfoU8;
        }

        break;

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
        if (arrayNode->typeInfo->kind != TypeInfoKind::Array && arrayNode->access->flags & AST_VALUE_COMPUTED)
        {
            // Check access at compile time
            switch (arrayAccess->typeInfo->nativeType)
            {
            case NativeTypeKind::S32:
            {
                auto idx = arrayAccess->computedValue.reg.s32;
                if (idx < 0)
                    return context->report({arrayNode->access, format("index is a negative value ('%d')", idx)});
                else if (idx >= (int32_t) typePtr->count)
                    return context->report({arrayNode->access, format("index out of range (index is '%d', maximum index is '%u')", idx, typePtr->count - 1)});
                break;
            }
            case NativeTypeKind::U32:
            {
                auto idx = arrayAccess->computedValue.reg.u32;
                if (idx >= typePtr->count)
                    return context->report({arrayNode->access, format("index out of range (index is '%u', maximum index is '%u')", idx, typePtr->count - 1)});
                break;
            }
            default:
                SWAG_ASSERT(false);
                break;
            }

            if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
            {
                SWAG_ASSERT(arrayNode->array->resolvedSymbolOverload->storageOffset != UINT32_MAX);
                auto ptr = context->sourceFile->module->constantSegment.address(arrayNode->array->resolvedSymbolOverload->storageOffset);
                ptr += arrayNode->access->computedValue.reg.u32 * typePtr->finalType->sizeOf;
                if (derefToValue(context, arrayNode, typePtr->finalType->kind, typePtr->finalType->nativeType, ptr))
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
                auto arrayChild = CastAst<AstPointerDeRef>(child, AstNodeKind::ArrayPointerIndex);
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
        return context->report({arrayNode->array, format("%s type '%s' cannot be referenced like a pointer", TypeInfo::getNakedKindName(arrayType), arrayType->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveInit(SemanticContext* context)
{
    auto node               = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format("'init' first parameter should be a pointer, but is '%s'", expressionTypeInfo->name.c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format("'init' count parameter should be an integer, but is '%s'", countTypeInfo->name.c_str())}));
        SWAG_VERIFY(countTypeInfo->sizeOf <= 4, context->report({node->count, "'init' count parameter should be 32 bits"}));
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
            auto job        = context->job;
            job->cacheDependentSymbols.clear();
            job->cacheDependentSymbols.insert(typeStruct->declNode->resolvedSymbolName);
            job->symMatch.reset();
            for (auto child : node->parameters->childs)
                job->symMatch.parameters.push_back(child);
            SWAG_CHECK(matchIdentifierParameters(context, nullptr, node->parameters, node));
            if (context->result == ContextResult::Pending)
                return true;
        }
        else
        {
            return internalError(context, "resolveInit, invalid type");
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitInit;

    return true;
}

bool SemanticJob::resolveDrop(SemanticContext* context)
{
    auto node               = CastAst<AstDrop>(context->node, AstNodeKind::Drop);
    auto expressionTypeInfo = TypeManager::concreteType(node->expression->typeInfo);

    SWAG_VERIFY(expressionTypeInfo->kind == TypeInfoKind::Pointer, context->report({node->expression, format("'drop' first parameter should be a pointer, but is '%s'", expressionTypeInfo->name.c_str())}));

    if (node->count)
    {
        auto countTypeInfo = TypeManager::concreteType(node->count->typeInfo);
        SWAG_VERIFY(countTypeInfo->flags & TYPEINFO_INTEGER, context->report({node->count, format("'drop' count parameter should be an integer, but is '%s'", countTypeInfo->name.c_str())}));
        SWAG_VERIFY(countTypeInfo->sizeOf <= 4, context->report({node->count, "'drop' count parameter should be 32 bits"}));
    }

    node->byteCodeFct = ByteCodeGenJob::emitDrop;

    return true;
}

bool SemanticJob::derefToValue(SemanticContext* context, AstNode* node, TypeInfoKind kind, NativeTypeKind nativeKind, void* ptr)
{
    if (kind == TypeInfoKind::Native)
    {
        switch (nativeKind)
        {
        case NativeTypeKind::String:
            node->computedValue.text = *(const char**) ptr;
            node->typeInfo           = g_TypeMgr.typeInfoString;
            break;
        case NativeTypeKind::S8:
            node->typeInfo             = g_TypeMgr.typeInfoS8;
            node->computedValue.reg.s8 = *(int8_t*) ptr;
            break;
        case NativeTypeKind::U8:
            node->typeInfo             = g_TypeMgr.typeInfoU8;
            node->computedValue.reg.u8 = *(uint8_t*) ptr;
            break;
        case NativeTypeKind::S16:
            node->typeInfo              = g_TypeMgr.typeInfoS16;
            node->computedValue.reg.s16 = *(int16_t*) ptr;
            break;
        case NativeTypeKind::U16:
            node->typeInfo              = g_TypeMgr.typeInfoU16;
            node->computedValue.reg.u16 = *(uint16_t*) ptr;
            break;
        case NativeTypeKind::S32:
            node->typeInfo              = g_TypeMgr.typeInfoS32;
            node->computedValue.reg.s32 = *(int32_t*) ptr;
            break;
        case NativeTypeKind::U32:
            node->typeInfo              = g_TypeMgr.typeInfoU32;
            node->computedValue.reg.u32 = *(uint32_t*) ptr;
            break;
        case NativeTypeKind::F32:
            node->typeInfo              = g_TypeMgr.typeInfoF32;
            node->computedValue.reg.f32 = *(float*) ptr;
            break;
        case NativeTypeKind::Char:
            node->typeInfo             = g_TypeMgr.typeInfoChar;
            node->computedValue.reg.ch = *(uint32_t*) ptr;
            break;
        case NativeTypeKind::S64:
            node->typeInfo              = g_TypeMgr.typeInfoS64;
            node->computedValue.reg.s64 = *(int64_t*) ptr;
            break;
        case NativeTypeKind::U64:
            node->typeInfo              = g_TypeMgr.typeInfoU64;
            node->computedValue.reg.u64 = *(uint64_t*) ptr;
            break;
        case NativeTypeKind::F64:
            node->typeInfo              = g_TypeMgr.typeInfoF64;
            node->computedValue.reg.f64 = *(double*) ptr;
            break;
        case NativeTypeKind::Bool:
            node->typeInfo            = g_TypeMgr.typeInfoBool;
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
                return derefToValue(context, node, anyNative->base.kind, anyNative->nativeKind, anyValue);
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

bool SemanticJob::derefTypeInfo(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload)
{
    auto sourceFile = context->sourceFile;
    auto node       = context->node;

    uint8_t* ptr = sourceFile->module->constantSegment.address(parent->previousResolvedNode->computedValue.reg.u32);
    ptr += overload->storageOffset;

    auto concreteType = TypeManager::concreteType(overload->typeInfo);
    if (!derefToValue(context, node, concreteType->kind, concreteType->nativeType, ptr))
        return internalError(context, "derefTypeInfo, invalid type", node);

    node->setFlagsValueIsComputed();
    return true;
}
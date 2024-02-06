#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Report.h"
#include "Semantic.h"
#include "TypeManager.h"

bool Semantic::reserveAndStoreToSegment(JobContext* context, DataSegment* storageSegment, uint32_t& storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    storageOffset = storageSegment->reserve(max(1, typeInfo->sizeOf), nullptr, TypeManager::alignOf(typeInfo));
    return storeToSegment(context, storageSegment, storageOffset, value, typeInfo, assignment);
}

bool Semantic::storeToSegment(JobContext* context, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    uint8_t* ptrDest = storageSegment->address(storageOffset);

    if (typeInfo->isString())
    {
        if (!value->text.empty())
        {
            const auto constSeg = getConstantSegFromContext(context->node, storageSegment->kind == SegmentKind::Compiler);
            uint8_t*   resultPtr;
            const auto offset = constSeg->addString(value->text, &resultPtr);
            storageSegment->addInitPtr(storageOffset, offset, constSeg->kind);
            *reinterpret_cast<const char**>(ptrDest)              = (const char*) resultPtr;
            *reinterpret_cast<uint64_t*>(ptrDest + sizeof(void*)) = value->text.length();
        }

        return true;
    }

    if (typeInfo->isAny())
    {
        const auto ptrAny = reinterpret_cast<SwagAny*>(ptrDest);
        if (assignment && !assignment->castedTypeInfo)
        {
            const auto valueAny           = (SwagAny*) value->getStorageAddr();
            *ptrAny                       = *valueAny;
            const auto storageOffsetValue = value->storageSegment->offset((uint8_t*) valueAny->value);
            const auto storageOffsetType  = value->storageSegment->offset((uint8_t*) valueAny->type);
            value->storageSegment->addInitPtr(storageOffset, storageOffsetValue, value->storageSegment->kind);
            value->storageSegment->addInitPtr(storageOffset + 8, storageOffsetType, value->storageSegment->kind);
        }
        else if (assignment && assignment->castedTypeInfo && assignment->castedTypeInfo->isPointerNull())
        {
            ptrAny->type  = nullptr;
            ptrAny->value = nullptr;
        }
        else if (assignment)
        {
            // Store value in constant storageSegment
            auto     constSegment = getConstantSegFromContext(context->node, storageSegment->kind == SegmentKind::Compiler);
            uint32_t storageOffsetValue;
            SWAG_CHECK(reserveAndStoreToSegment(context, constSegment, storageOffsetValue, value, assignment->castedTypeInfo, assignment));

            // Then reference that value and the concrete type info
            // Pointer to the value
            const auto ptrStorage = constSegment->address(storageOffsetValue);
            ptrAny->value         = ptrStorage;
            storageSegment->addInitPtr(storageOffset, storageOffsetValue, constSegment->kind);

            // :AnyTypeSegment
            SWAG_ASSERT(assignment->hasExtMisc());
            SWAG_ASSERT(assignment->extMisc()->anyTypeSegment);
            constSegment = assignment->extMisc()->anyTypeSegment;
            ptrAny->type = reinterpret_cast<ExportedTypeInfo*>(constSegment->address(assignment->extMisc()->anyTypeOffset));
            storageSegment->addInitPtr(storageOffset + 8, assignment->extMisc()->anyTypeOffset, constSegment->kind);
        }

        return true;
    }

    if (typeInfo->isSlice())
    {
        auto       ptrSlice   = reinterpret_cast<SwagSlice*>(ptrDest);
        const auto assignType = assignment ? TypeManager::concreteType(assignment->typeInfo) : nullptr;

        if (assignment && assignment->castedTypeInfo && assignment->castedTypeInfo->isPointerNull())
        {
            ptrSlice->buffer = nullptr;
            ptrSlice->count  = 0;
        }
        else if (assignType && assignType->isSlice())
        {
            const auto slice = (SwagSlice*) value->getStorageAddr();
            *ptrSlice        = *slice;
            if (slice->buffer)
            {
                const auto storageOffsetValue = value->storageSegment->offset((uint8_t*) slice->buffer);
                value->storageSegment->addInitPtr(storageOffset, storageOffsetValue, value->storageSegment->kind);
            }
        }
        else if (assignType && assignType->isListArray())
        {
            SWAG_CHECK(checkIsConstExpr(context, assignment));

            // Store value in constant storageSegment
            uint32_t   storageOffsetValue;
            const auto constSegment = getConstantSegFromContext(assignment, storageSegment->kind == SegmentKind::Compiler);
            SWAG_CHECK(reserveAndStoreToSegment(context, constSegment, storageOffsetValue, value, assignType, assignment));

            // Then setup the pointer to that data, and the data count
            const auto ptrStorage = constSegment->address(storageOffsetValue);
            ptrSlice->buffer      = ptrStorage;
            ptrSlice->count       = assignment->childs.size();
            storageSegment->addInitPtr(storageOffset, storageOffsetValue, constSegment->kind);
        }

        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::FuncCallParams)
    {
        SWAG_CHECK(checkIsConstExpr(context, assignment));
        auto       offset = storageOffset;
        const auto result = collectLiteralsToSegment(context, storageSegment, storageOffset, offset, assignment);
        SWAG_CHECK(result);
        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::ExpressionList)
    {
        SWAG_CHECK(checkIsConstExpr(context, assignment));
        auto       offset = storageOffset;
        const auto result = collectLiteralsToSegment(context, storageSegment, storageOffset, offset, assignment);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->isStruct())
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        const auto result     = collectStructLiterals(context, storageSegment, storageOffset, typeStruct->declNode);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->isLambdaClosure())
    {
        *reinterpret_cast<uint64_t*>(ptrDest) = 0;
        const auto funcDecl                   = castAst<AstFuncDecl>(typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
        storageSegment->addPatchMethod(funcDecl, storageOffset);
        return true;
    }

    if (typeInfo->isPointerToTypeInfo())
    {
        storageSegment->addInitPtr(storageOffset, value->storageOffset, value->storageSegment->kind);
        *reinterpret_cast<void**>(ptrDest) = value->getStorageAddr();
        return true;
    }

    switch (typeInfo->sizeOf)
    {
    case 1:
        *ptrDest = value->reg.u8;
        break;
    case 2:
        *reinterpret_cast<uint16_t*>(ptrDest) = value->reg.u16;
        break;
    case 4:
        *reinterpret_cast<uint32_t*>(ptrDest) = value->reg.u32;
        break;
    case 8:
        *reinterpret_cast<uint64_t*>(ptrDest) = value->reg.u64;
        break;
    default:
        break;
    }

    return true;
}

bool Semantic::collectStructLiterals(JobContext* context, DataSegment* storageSegment, uint32_t offsetStruct, AstNode* node)
{
    const AstStruct* structNode = castAst<AstStruct>(node, AstNodeKind::StructDecl);
    const auto       typeStruct = castTypeInfo<TypeInfoStruct>(structNode->typeInfo, TypeInfoKind::Struct);

    for (const auto field : typeStruct->fields)
    {
        const auto ptrDest  = storageSegment->address(offsetStruct + field->offset);
        const auto child    = field->declNode;
        const auto varDecl  = castAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        const auto typeInfo = TypeManager::concreteType(varDecl->typeInfo);

        if (varDecl->assignment)
        {
            const auto value = varDecl->assignment->computedValue;
            if (typeInfo->isString())
            {
                SWAG_ASSERT(value);
                const auto storedV      = reinterpret_cast<Register*>(ptrDest);
                storedV[0].pointer      = (uint8_t*) value->text.buffer;
                storedV[1].u64          = value->text.length();
                const auto constSegment = getConstantSegFromContext(varDecl->assignment, storageSegment->kind == SegmentKind::Compiler);
                const auto strOffset    = constSegment->addString(value->text);
                storageSegment->addInitPtr(offsetStruct + field->offset, strOffset, constSegment->kind);
            }
            else
            {
                switch (typeInfo->sizeOf)
                {
                case 1:
                    *reinterpret_cast<uint8_t*>(ptrDest) = value ? value->reg.u8 : 0;
                    break;
                case 2:
                    *reinterpret_cast<uint16_t*>(ptrDest) = value ? value->reg.u16 : 0;
                    break;
                case 4:
                    *reinterpret_cast<uint32_t*>(ptrDest) = value ? value->reg.u32 : 0;
                    break;
                case 8:
                    *reinterpret_cast<uint64_t*>(ptrDest) = value ? value->reg.u64 : 0;
                    break;
                default:
                    return Report::internalError(context->node, "collectStructLiterals, invalid native type sizeof");
                }

                SWAG_ASSERT(typeInfo->sizeOf);
            }
        }
        else if (typeInfo->isStruct())
        {
            const auto typeSub = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(collectStructLiterals(context, storageSegment, offsetStruct + field->offset, typeSub->declNode));
        }

        if (varDecl->type && varDecl->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS)
        {
            const auto varType = varDecl->type;
            const auto srcAddr = varType->computedValue->getStorageAddr();
            memcpy(ptrDest, srcAddr, typeInfo->sizeOf);
        }
    }

    return true;
}

bool Semantic::collectLiteralsToSegment(JobContext* context, DataSegment* storageSegment, uint32_t baseOffset, uint32_t& offset, AstNode* node)
{
    // If we are collecting an expression list for a struct, then we must first collect all struct default
    // values if every fields are not covered
    if (node->typeInfo && node->typeInfo->kind == TypeInfoKind::TypeListTuple)
    {
        const auto exprNode = castAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
        if (exprNode->castToStruct)
        {
            SWAG_CHECK(storeToSegment(context, storageSegment, baseOffset, nullptr, exprNode->castToStruct, nullptr));
        }
    }

    for (const auto child : node->childs)
    {
        auto typeInfo = child->typeInfo;

        // Special type when collecting (like an array collected to a slice)
        if (child->hasExtMisc() && child->extMisc()->collectTypeInfo)
            typeInfo = child->extMisc()->collectTypeInfo;

        // In case of a struct to field match
        auto assignment = child;
        if (child->kind == AstNodeKind::FuncCallParam)
        {
            const auto param = castAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            if (param->resolvedParameter)
            {
                offset   = baseOffset + param->resolvedParameter->offset;
                typeInfo = param->resolvedParameter->typeInfo;
            }

            assignment = child->childs.front();

            // If we have an expression list in a call parameter, like = {{1}}, then we check if the expression
            // list has been converted to a variable. If that's the case, then we should have type parameters to
            // that var, and we must take them instead of the expression list, because cast has been done
            if (assignment->kind == AstNodeKind::ExpressionList &&
                child->childs.count == 3 &&
                child->childs[1]->kind == AstNodeKind::VarDecl)
            {
                const auto varDecl = castAst<AstVarDecl>(child->childs[1], AstNodeKind::VarDecl);
                SWAG_ASSERT(varDecl->type);
                const auto typeDecl = castAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
                SWAG_ASSERT(typeDecl->identifier);
                const auto idDecl = castAst<AstIdentifier>(typeDecl->identifier->childs.back(), AstNodeKind::Identifier);
                SWAG_ASSERT(idDecl->callParameters);
                SWAG_CHECK(collectLiteralsToSegment(context, storageSegment, baseOffset, offset, idDecl->callParameters));
            }
            else
            {
                SWAG_CHECK(storeToSegment(context, storageSegment, offset, child->computedValue, typeInfo, assignment));
            }

            continue;
        }

        // Offset has been forced
        // Note that offset is +1 to be sure it has been initialized
        if (child->hasExtMisc() && child->extMisc()->castOffset)
            offset = baseOffset + child->extMisc()->castOffset - 1;

        SWAG_CHECK(storeToSegment(context, storageSegment, offset, child->computedValue, typeInfo, assignment));

        offset += child->typeInfo->sizeOf;
    }

    // If we are collecting a tuple, then take the size of it to compute the "next" offset.
    // In case there's a padding here, because of the cast to struct.
    if (node->typeInfo && node->typeInfo->kind == TypeInfoKind::TypeListTuple)
        offset = baseOffset + node->typeInfo->sizeOf;

    return true;
}

bool Semantic::collectAssignment(SemanticContext* context, DataSegment* storageSegment, uint32_t& storageOffset, AstVarDecl* node, TypeInfo* typeInfo)
{
    if (!typeInfo)
        typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->sizeOf == 0)
        return true;

    ComputedValue* value;
    if (node->assignment)
    {
        node->assignment->allocateComputedValue();
        value = node->assignment->computedValue;
    }
    else
    {
        node->allocateComputedValue();
        value = node->computedValue;
    }

    if (typeInfo->isArray())
    {
        const auto typeArr = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);

        // Already computed in the constant storageSegment for an array
        if (node->assignment && node->assignment->hasComputedValue())
        {
            if (!value->storageSegment)
            {
                uint8_t* addrDst;
                storageOffset = storageSegment->reserve(typeInfo->sizeOf, &addrDst, alignOf(node));

                for (uint32_t i = 0; i < typeArr->totalCount; i++)
                {
                    switch (typeArr->finalType->sizeOf)
                    {
                    case 1:
                        *addrDst = node->assignment->computedValue->reg.u8;
                        break;
                    case 2:
                        *reinterpret_cast<uint16_t*>(addrDst) = node->assignment->computedValue->reg.u16;
                        break;
                    case 4:
                        *reinterpret_cast<uint32_t*>(addrDst) = node->assignment->computedValue->reg.u32;
                        break;
                    case 8:
                        *reinterpret_cast<uint64_t*>(addrDst) = node->assignment->computedValue->reg.u64;
                        break;
                    default:
                        return Report::internalError(node->assignment, "invalid size constant collectAssignment");
                    }
                    addrDst += typeArr->finalType->sizeOf;
                }
            }
            else if (storageSegment->kind == SegmentKind::Constant)
            {
                SWAG_ASSERT(value->storageOffset != UINT32_MAX);
                SWAG_ASSERT(value->storageSegment != nullptr);
                storageOffset = value->storageOffset;
            }
            else
            {
                uint8_t* addrDst;
                storageOffset      = storageSegment->reserve(typeInfo->sizeOf, &addrDst, alignOf(node));
                const auto addrSrc = value->getStorageAddr();
                memcpy(addrDst, addrSrc, typeInfo->sizeOf);
            }

            return true;
        }

        // Array of struct
        if (!node->assignment)
        {
            if (typeArr->finalType->isStruct() && typeArr->finalType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            {
                storageOffset = storageSegment->reserve(typeInfo->sizeOf, nullptr);

                auto offset = storageOffset;
                for (uint32_t i = 0; i < typeArr->totalCount; i++)
                {
                    SWAG_CHECK(storeToSegment(context, storageSegment, offset, value, typeArr->finalType, nullptr));
                    offset += typeArr->finalType->sizeOf;
                }

                return true;
            }
        }
    }

    if (typeInfo->isStruct())
    {
        if (node->assignment && node->assignment->kind == AstNodeKind::IdentifierRef && node->assignment->resolvedSymbolOverload)
        {
            // Do not initialize variable with type arguments, then again with an initialization
            const auto assign   = node->assignment;
            const auto overload = assign->resolvedSymbolOverload;
            if (node->type && (node->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
            {
                Diagnostic diag{assign, Err(Err0063)};
                diag.addRange(node->type, Nte(Nte0164));
                return context->report(diag);
            }

            // Copy from a constant
            SWAG_ASSERT(assign->flags & AST_CONST_EXPR);
            uint8_t* addrDst;
            storageOffset = storageSegment->reserve(typeInfo->sizeOf, &addrDst, alignOf(node));
            SWAG_ASSERT(overload->computedValue.storageSegment != storageSegment);
            const auto addrSrc = overload->computedValue.getStorageAddr();
            memcpy(addrDst, addrSrc, typeInfo->sizeOf);
        }
        else
        {
            if (node->type && (node->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
            {
                const auto typeExpression = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
                const auto identifier     = castAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

                // First collect values from the structure default initialization, except if the parameters cover
                // all the fields (in that case no need to initialize the struct twice)
                if (!(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                    SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, value, typeInfo, nullptr));
                else
                    storageOffset = storageSegment->reserve(typeInfo->sizeOf, nullptr, TypeManager::alignOf(typeInfo));

                // Then collect values from the type parameters
                SWAG_CHECK(storeToSegment(context, storageSegment, storageOffset, value, typeInfo, identifier->callParameters));
            }
            else
            {
                // Collect values from the structure default init
                SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, value, typeInfo, nullptr));
            }

            SWAG_ASSERT(!node->assignment || node->assignment->kind != AstNodeKind::ExpressionList);
        }

        return true;
    }

    SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, value, typeInfo, node->assignment));
    return true;
}

bool Semantic::collectConstantAssignment(SemanticContext* context, DataSegment** storageSegmentResult, uint32_t* storageOffsetResult, uint32_t& symbolFlags)
{
    auto       node     = static_cast<AstVarDecl*>(context->node);
    const auto typeInfo = TypeManager::concreteType(node->typeInfo);

    DataSegment* storageSegment = nullptr;
    uint32_t     storageOffset  = UINT32_MAX;

    SWAG_VERIFY(!node->typeInfo->isGeneric(), context->report({node, FMT(Err(Err0248), node->typeInfo->getDisplayNameC())}));

    // A constant array cannot be initialized with just one value (this is for variables)
    if (node->assignment && typeInfo->isArray())
    {
        const auto typeAssign = TypeManager::concreteType(node->assignment->typeInfo);
        if (!typeAssign->isArray() && !typeAssign->isListArray())
        {
            const Diagnostic diag{node->assignment, FMT(Err(Err0313), typeInfo->getDisplayNameC())};
            const auto       note = Diagnostic::note(Nte(Nte0101));
            return context->report(diag, note);
        }
    }

    storageSegment = getSegmentForVar(context, node);
    if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
    {
        storageOffset = 0;
        symbolFlags |= OVERLOAD_INCOMPLETE;
    }
    else if (node->assignment && typeInfo->isAny())
    {
        node->assignment->setFlagsValueIsComputed();
        SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, node->assignment->computedValue, node->assignment->typeInfo, node->assignment));
        node->assignment->computedValue->storageOffset  = storageOffset;
        node->assignment->computedValue->storageSegment = storageSegment;
    }

    // :SliceLiteral
    else if (node->assignment && typeInfo->isSlice() && node->assignment->castedTypeInfo && node->assignment->castedTypeInfo->isArray())
    {
        uint32_t storageOffsetValues;
        SWAG_CHECK(collectAssignment(context, storageSegment, storageOffsetValues, node, node->assignment->castedTypeInfo));

        SwagSlice* slice;
        storageOffset        = storageSegment->reserve(sizeof(SwagSlice), (uint8_t**) &slice);
        const auto typeArray = castTypeInfo<TypeInfoArray>(node->assignment->castedTypeInfo, TypeInfoKind::Array);
        slice->buffer        = storageSegment->address(storageOffsetValues);
        storageSegment->addInitPtr(storageOffset, storageOffsetValues, storageSegment->kind);
        slice->count = typeArray->totalCount;
    }
    else if (node->assignment && typeInfo->isSlice())
    {
        const auto assignNode = node->assignment;
        const auto assignType = TypeManager::concreteType(assignNode->typeInfo);
        assignNode->setFlagsValueIsComputed();
        SWAG_CHECK(collectConstantSlice(context, assignNode, assignType, storageSegment, storageOffset));
        assignNode->computedValue->storageOffset  = storageOffset;
        assignNode->computedValue->storageSegment = storageSegment;
    }
    else if (node->assignment && typeInfo->isInterface() && node->assignment->castedTypeInfo && node->assignment->castedTypeInfo->isPointerNull())
    {
        SwagInterface* itr;
        storageOffset = storageSegment->reserve(sizeof(SwagInterface), (uint8_t**) &itr);
        itr->data     = nullptr;
        itr->itable   = nullptr;
    }
    else if (node->assignment && node->assignment->hasComputedValue())
    {
        storageOffset  = node->assignment->computedValue->storageOffset;
        storageSegment = node->assignment->computedValue->storageSegment;
    }
    else if (node->hasComputedValue())
    {
        storageOffset  = node->computedValue->storageOffset;
        storageSegment = node->computedValue->storageSegment;
    }
    else if (typeInfo->isArray() || typeInfo->isStruct())
    {
        node->allocateComputedValue();
        SWAG_CHECK(collectAssignment(context, storageSegment, storageOffset, node));
    }

    node->inheritComputedValue(node->assignment);
    SWAG_ASSERT(node->computedValue);

    *storageSegmentResult = storageSegment;
    *storageOffsetResult  = storageOffset;
    return true;
}

bool Semantic::collectConstantSlice(SemanticContext* context, AstNode* assignNode, TypeInfo* assignType, DataSegment* storageSegment, uint32_t& storageOffset)
{
    // :SliceLiteral
    if (assignType->isListArray())
    {
        SwagSlice* slice;
        storageOffset = storageSegment->reserve(sizeof(SwagSlice), (uint8_t**) &slice);

        uint32_t storageOffsetValues;
        SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffsetValues, assignNode->computedValue, assignNode->typeInfo, assignNode));
        storageSegment->addInitPtr(storageOffset, storageOffsetValues, storageSegment->kind);

        const auto typeList = castTypeInfo<TypeInfoList>(assignNode->typeInfo, TypeInfoKind::TypeListArray);
        slice->buffer       = storageSegment->address(storageOffsetValues);
        slice->count        = typeList->subTypes.size();
    }
    else if (assignType->isPointerNull() || (assignNode && assignNode->castedTypeInfo && assignNode->castedTypeInfo->isPointerNull()))
    {
        SwagSlice* slice;
        storageOffset = storageSegment->reserve(sizeof(SwagSlice), (uint8_t**) &slice);
        slice->buffer = nullptr;
        slice->count  = 0;
    }
    else
    {
        SWAG_ASSERT(assignType->isSlice());
        SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, assignNode->computedValue, assignType, assignNode));
    }

    return true;
}

bool Semantic::derefConstantValue(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, DataSegment* storageSegment, uint8_t* ptr)
{
    if (typeInfo->isPointerToTypeInfo())
    {
        node->setFlagsValueIsComputed();
        if (*reinterpret_cast<uint8_t**>(ptr) == nullptr)
        {
            node->typeInfo = g_TypeMgr->typeInfoNull;
        }
        else
        {
            // :BackPtrOffset
            node->computedValue->storageOffset  = storageSegment->offset(*reinterpret_cast<uint8_t**>(ptr));
            node->computedValue->storageSegment = storageSegment;
            setupIdentifierRef(context, node);
            node->flags |= AST_VALUE_IS_GEN_TYPEINFO;
        }

        return true;
    }

    if (typeInfo->isPointer())
    {
        return false;
    }

    if (typeInfo->isArray())
    {
        node->setFlagsValueIsComputed();
        node->computedValue->storageOffset  = storageSegment->offset(ptr);
        node->computedValue->storageSegment = storageSegment;
        node->typeInfo                      = typeInfo;
        return true;
    }

    if (typeInfo->isSlice())
    {
        // Convert slice to a static constant array
        const auto typeSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        const auto ptrSlice  = reinterpret_cast<SwagSlice*>(ptr);
        node->setFlagsValueIsComputed();
        node->computedValue->storageOffset  = ptrSlice->buffer ? storageSegment->offset((uint8_t*) ptrSlice->buffer) : UINT32_MAX;
        node->computedValue->storageSegment = storageSegment;
        node->computedValue->reg.u64        = ptrSlice->count;
        const auto typeArray                = makeType<TypeInfoArray>();
        typeArray->count                    = (uint32_t) (reinterpret_cast<SwagSlice*>(ptr))->count;
        typeArray->totalCount               = typeArray->count;
        typeArray->pointedType              = typeSlice->pointedType;
        typeArray->finalType                = typeSlice->pointedType;
        typeArray->sizeOf                   = typeArray->totalCount * typeArray->pointedType->sizeOf;
        typeArray->computeName();
        node->typeInfo = typeArray;
        return true;
    }

    if (typeInfo->isStruct())
    {
        node->setFlagsValueIsComputed();
        node->computedValue->storageOffset  = storageSegment->offset(ptr);
        node->computedValue->storageSegment = storageSegment;
        node->typeInfo                      = typeInfo;
        setupIdentifierRef(context, node);
        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return false;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::String:
    {
        const auto slice = reinterpret_cast<SwagSlice*>(ptr);
        node->setFlagsValueIsComputed();
        node->computedValue->text = Utf8{(const char*) slice->buffer, (uint32_t) slice->count};
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoString;
        break;
    }

    case NativeTypeKind::Any:
        node->setFlagsValueIsComputed();
        node->computedValue->storageSegment = storageSegment;
        node->computedValue->storageOffset  = storageSegment->offset((uint8_t*) ptr);
        break;

    case NativeTypeKind::S8:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS8;
        node->computedValue->reg.s8 = *reinterpret_cast<int8_t*>(ptr);
        break;
    case NativeTypeKind::U8:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU8;
        node->computedValue->reg.u8 = *ptr;
        break;
    case NativeTypeKind::S16:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS16;
        node->computedValue->reg.s16 = *reinterpret_cast<int16_t*>(ptr);
        break;
    case NativeTypeKind::U16:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU16;
        node->computedValue->reg.u16 = *reinterpret_cast<uint16_t*>(ptr);
        break;
    case NativeTypeKind::S32:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS32;
        node->computedValue->reg.s32 = *reinterpret_cast<int32_t*>(ptr);
        break;
    case NativeTypeKind::U32:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU32;
        node->computedValue->reg.u32 = *reinterpret_cast<uint32_t*>(ptr);
        break;
    case NativeTypeKind::F32:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoF32;
        node->computedValue->reg.f32 = *reinterpret_cast<float*>(ptr);
        break;
    case NativeTypeKind::Rune:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoRune;
        node->computedValue->reg.ch = *reinterpret_cast<uint32_t*>(ptr);
        break;
    case NativeTypeKind::S64:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoS64;
        node->computedValue->reg.s64 = *reinterpret_cast<int64_t*>(ptr);
        break;
    case NativeTypeKind::U64:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        node->computedValue->reg.u64 = *reinterpret_cast<uint64_t*>(ptr);
        break;
    case NativeTypeKind::F64:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoF64;
        node->computedValue->reg.f64 = *reinterpret_cast<double*>(ptr);
        break;
    case NativeTypeKind::Bool:
        node->setFlagsValueIsComputed();
        if (!node->typeInfo)
            node->typeInfo = g_TypeMgr->typeInfoBool;
        node->computedValue->reg.b = *reinterpret_cast<bool*>(ptr);
        break;

    default:
        return false;
    }

    return true;
}

bool Semantic::derefConstant(SemanticContext* context, uint8_t* ptr, const SymbolOverload* overload, DataSegment* storageSegment)
{
    const auto node = context->node;
    ptr += overload->computedValue.storageOffset;
    node->typeInfo          = overload->typeInfo;
    const auto concreteType = TypeManager::concreteType(overload->typeInfo);
    return derefConstantValue(context, node, concreteType, storageSegment, ptr);
}

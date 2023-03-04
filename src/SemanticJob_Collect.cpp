#include "pch.h"
#include "SemanticJob.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"

bool SemanticJob::reserveAndStoreToSegment(JobContext* context, DataSegment* storageSegment, uint32_t& storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    storageOffset = storageSegment->reserve(max(1, typeInfo->sizeOf), nullptr, TypeManager::alignOf(typeInfo));
    return storeToSegment(context, storageSegment, storageOffset, value, typeInfo, assignment);
}

bool SemanticJob::storeToSegment(JobContext* context, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    uint8_t* ptrDest = storageSegment->address(storageOffset);

    if (typeInfo->isString())
    {
        if (!value->text.empty())
        {
            auto     constSeg = getConstantSegFromContext(context->node, storageSegment->kind == SegmentKind::Compiler);
            uint8_t* resultPtr;
            auto     offset = constSeg->addString(value->text, &resultPtr);
            storageSegment->addInitPtr(storageOffset, offset, constSeg->kind);
            *(const char**) ptrDest                = (const char*) resultPtr;
            *(uint64_t*) (ptrDest + sizeof(void*)) = value->text.length();
        }

        return true;
    }

    if (typeInfo->isAny())
    {
        if (!assignment->castedTypeInfo)
            return Report::internalError(context->node, "storeToSegment, cannot resolve any");

        // Store value in constant storageSegment
        auto     constSegment = SemanticJob::getConstantSegFromContext(context->node, storageSegment->kind == SegmentKind::Compiler);
        uint32_t storageOffsetValue;
        SWAG_CHECK(reserveAndStoreToSegment(context, constSegment, storageOffsetValue, value, assignment->castedTypeInfo, assignment));

        // Then reference that value and the concrete type info
        // Pointer to the value
        auto ptrStorage   = constSegment->address(storageOffsetValue);
        *(void**) ptrDest = ptrStorage;
        storageSegment->addInitPtr(storageOffset, storageOffsetValue, constSegment->kind);

        // :AnyTypeSegment
        SWAG_ASSERT(assignment->hasExtMisc());
        SWAG_ASSERT(assignment->extMisc()->anyTypeSegment);
        constSegment                        = assignment->extMisc()->anyTypeSegment;
        *(void**) (ptrDest + sizeof(void*)) = constSegment->address(assignment->extMisc()->anyTypeOffset);
        storageSegment->addInitPtr(storageOffset + 8, assignment->extMisc()->anyTypeOffset, constSegment->kind);
        return true;
    }

    if (typeInfo->isSlice())
    {
        if (assignment && assignment->kind == AstNodeKind::Literal)
        {
            *(void**) ptrDest                      = nullptr;
            *(uint64_t*) (ptrDest + sizeof(void*)) = 0;
        }
        else if (assignment)
        {
            SWAG_VERIFY(assignment->kind == AstNodeKind::ExpressionList, context->report({assignment, Err(Err0798)}));
            SWAG_CHECK(checkIsConstExpr(context, assignment));

            // Store value in constant storageSegment
            uint32_t storageOffsetValue;
            auto     constSegment = getConstantSegFromContext(assignment, storageSegment->kind == SegmentKind::Compiler);
            SWAG_CHECK(reserveAndStoreToSegment(context, constSegment, storageOffsetValue, value, assignment->typeInfo, assignment));

            // Then setup the pointer to that data, and the data count
            auto ptrStorage                        = constSegment->address(storageOffsetValue);
            *(void**) ptrDest                      = ptrStorage;
            *(uint64_t*) (ptrDest + sizeof(void*)) = assignment->childs.size();
            storageSegment->addInitPtr(storageOffset, storageOffsetValue, constSegment->kind);
        }

        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::FuncCallParams)
    {
        SWAG_CHECK(checkIsConstExpr(context, assignment));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegment(context, storageSegment, storageOffset, offset, assignment);
        SWAG_CHECK(result);
        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::ExpressionList)
    {
        SWAG_CHECK(checkIsConstExpr(context, assignment));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegment(context, storageSegment, storageOffset, offset, assignment);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->isStruct())
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto result     = collectStructLiterals(context, storageSegment, storageOffset, typeStruct->declNode);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->isLambdaClosure())
    {
        *(uint64_t*) ptrDest = 0;
        auto funcDecl        = CastAst<AstFuncDecl>(typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
        storageSegment->addPatchMethod(funcDecl, storageOffset);
        return true;
    }

    if (typeInfo->isPointerToTypeInfo())
    {
        storageSegment->addInitPtr(storageOffset, value->storageOffset, value->storageSegment->kind);
        *(void**) ptrDest = value->storageSegment->address(value->storageOffset);
        return true;
    }

    switch (typeInfo->sizeOf)
    {
    case 1:
        *(uint8_t*) ptrDest = value->reg.u8;
        break;
    case 2:
        *(uint16_t*) ptrDest = value->reg.u16;
        break;
    case 4:
        *(uint32_t*) ptrDest = value->reg.u32;
        break;
    case 8:
        *(uint64_t*) ptrDest = value->reg.u64;
        break;
    }

    return true;
}

bool SemanticJob::collectStructLiterals(JobContext* context, DataSegment* storageSegment, uint32_t offsetStruct, AstNode* node)
{
    AstStruct* structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
    auto       typeStruct = CastTypeInfo<TypeInfoStruct>(structNode->typeInfo, TypeInfoKind::Struct);

    auto cptField = 0;
    for (auto field : typeStruct->fields)
    {
        auto ptrDest  = storageSegment->address(offsetStruct + field->offset);
        auto child    = field->declNode;
        auto varDecl  = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeInfo = TypeManager::concreteType(varDecl->typeInfo);

        if (varDecl->assignment)
        {
            auto value = varDecl->assignment->computedValue;
            if (typeInfo->isString())
            {
                SWAG_ASSERT(value);
                Register* storedV  = (Register*) ptrDest;
                storedV[0].pointer = (uint8_t*) value->text.buffer;
                storedV[1].u64     = value->text.length();
                auto constSegment  = SemanticJob::getConstantSegFromContext(varDecl->assignment, storageSegment->kind == SegmentKind::Compiler);
                auto strOffset     = constSegment->addString(value->text);
                storageSegment->addInitPtr(offsetStruct + field->offset, strOffset, constSegment->kind);
            }
            else
            {
                switch (typeInfo->sizeOf)
                {
                case 1:
                    *(uint8_t*) ptrDest = value ? value->reg.u8 : 0;
                    break;
                case 2:
                    *(uint16_t*) ptrDest = value ? value->reg.u16 : 0;
                    break;
                case 4:
                    *(uint32_t*) ptrDest = value ? value->reg.u32 : 0;
                    break;
                case 8:
                    *(uint64_t*) ptrDest = value ? value->reg.u64 : 0;
                    break;
                default:
                    return Report::internalError(context->node, "collectStructLiterals, invalid native type sizeof");
                }

                SWAG_ASSERT(typeInfo->sizeOf);
            }
        }
        else if (typeInfo->isStruct())
        {
            auto typeSub = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(collectStructLiterals(context, storageSegment, offsetStruct + field->offset, typeSub->declNode));
        }

        if (varDecl->type && varDecl->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS)
        {
            auto varType = varDecl->type;
            SWAG_ASSERT(varType->computedValue->storageSegment);
            SWAG_ASSERT(varType->computedValue->storageOffset != 0xFFFFFFFF);
            auto srcAddr = varType->computedValue->storageSegment->address(varType->computedValue->storageOffset);
            memcpy(ptrDest, srcAddr, typeInfo->sizeOf);
        }

        cptField++;
    }

    return true;
}

bool SemanticJob::collectLiteralsToSegment(JobContext* context, DataSegment* storageSegment, uint32_t baseOffset, uint32_t& offset, AstNode* node)
{
    // If we are collecting an expression list for a struct, then we must first collect all struct default
    // values if every fields are not covered
    if (node->typeInfo && node->typeInfo->kind == TypeInfoKind::TypeListTuple)
    {
        auto exprNode = CastAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
        if (exprNode->castToStruct)
        {
            SWAG_CHECK(storeToSegment(context, storageSegment, baseOffset, nullptr, exprNode->castToStruct, nullptr));
        }
    }

    for (auto child : node->childs)
    {
        auto typeInfo = child->typeInfo;

        // Special type when collecting (like an array collected to a slice)
        if (child->hasExtMisc() && child->extMisc()->collectTypeInfo)
            typeInfo = child->extMisc()->collectTypeInfo;

        // In case of a struct to field match
        auto assignment = child;
        if (child->kind == AstNodeKind::FuncCallParam)
        {
            auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
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
                auto varDecl = CastAst<AstVarDecl>(child->childs[1], AstNodeKind::VarDecl);
                SWAG_ASSERT(varDecl->type);
                auto typeDecl = CastAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
                SWAG_ASSERT(typeDecl->identifier);
                auto idDecl = CastAst<AstIdentifier>(typeDecl->identifier->childs.back(), AstNodeKind::Identifier);
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

bool SemanticJob::collectAssignment(SemanticContext* context, DataSegment* storageSegment, uint32_t& storageOffset, AstVarDecl* node)
{
    auto typeInfo = TypeManager::concreteType(node->typeInfo);
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
        // Already computed in the constant storageSegment for an array
        if (node->assignment && node->assignment->flags & AST_VALUE_COMPUTED)
        {
            SWAG_ASSERT(value->storageOffset != UINT32_MAX);
            SWAG_ASSERT(value->storageSegment != nullptr);

            if (storageSegment->kind == SegmentKind::Constant)
                storageOffset = value->storageOffset;
            else
            {
                uint8_t* addrDst;
                storageOffset = storageSegment->reserve(typeInfo->sizeOf, &addrDst, SemanticJob::alignOf(node));
                auto addrSrc  = value->storageSegment->address(value->storageOffset);
                memcpy(addrDst, addrSrc, typeInfo->sizeOf);
            }

            return true;
        }

        // Array of struct
        if (!node->assignment)
        {
            auto typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
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
            auto assign   = node->assignment;
            auto overload = assign->resolvedSymbolOverload;
            if (node->type && (node->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
            {
                Diagnostic diag{assign, Err(Err0645), Hnt(Hnt0045)};
                diag.addRange(node->type, Hnt(Hnt0007));
                return context->report(diag);
            }

            // Copy from a constant
            SWAG_ASSERT(assign->flags & AST_CONST_EXPR);
            uint8_t* addrDst;
            storageOffset = storageSegment->reserve(typeInfo->sizeOf, &addrDst, SemanticJob::alignOf(node));
            SWAG_ASSERT(overload->computedValue.storageOffset != UINT32_MAX);
            SWAG_ASSERT(overload->computedValue.storageSegment);
            SWAG_ASSERT(overload->computedValue.storageSegment != storageSegment);
            auto addrSrc = overload->computedValue.storageSegment->address(overload->computedValue.storageOffset);
            memcpy(addrDst, addrSrc, typeInfo->sizeOf);
        }
        else
        {
            if (node->type && (node->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
            {
                auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
                auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

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

#include "pch.h"
#include "SemanticJob.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ErrorIds.h"

bool SemanticJob::reserveAndStoreToSegment(JobContext* context, DataSegment* storageSegment, uint32_t& storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    storageOffset = storageSegment->reserve(typeInfo->sizeOf, nullptr, TypeManager::alignOf(typeInfo));
    return storeToSegment(context, storageSegment, storageOffset, value, typeInfo, assignment);
}

bool SemanticJob::storeToSegment(JobContext* context, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    uint8_t* ptrDest = storageSegment->address(storageOffset);

    if (typeInfo->isNative(NativeTypeKind::String))
    {
        if (!value->text.empty())
        {
            *(const char**) ptrDest                = value->text.c_str();
            *(uint64_t*) (ptrDest + sizeof(void*)) = value->text.length();
            auto constSeg                          = getConstantSegFromContext(context->node, storageSegment->kind == SegmentKind::Compiler);
            auto offset                            = constSeg->addString(value->text);
            storageSegment->addInitPtr(storageOffset, offset, constSeg->kind);
        }

        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::Any))
    {
        if (!assignment->castedTypeInfo)
            return internalError(context, "storeToSegment, cannot resolve any");

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
        SWAG_ASSERT(assignment->extension);
        SWAG_ASSERT(assignment->extension->anyTypeSegment);
        constSegment                        = assignment->extension->anyTypeSegment;
        *(void**) (ptrDest + sizeof(void*)) = constSegment->address(assignment->extension->anyTypeOffset);
        storageSegment->addInitPtr(storageOffset + 8, assignment->extension->anyTypeOffset, constSegment->kind);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        if (assignment)
        {
            SWAG_VERIFY(assignment->kind == AstNodeKind::ExpressionList, context->report({assignment, Msg0798}));
            SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->report({assignment, Msg0798}));

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
        SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->report({assignment, Msg0798}));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegment(context, storageSegment, storageOffset, offset, assignment);
        SWAG_CHECK(result);
        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::ExpressionList)
    {
        SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->report({assignment, Msg0798}));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegment(context, storageSegment, storageOffset, offset, assignment);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto result     = collectStructLiterals(context, storageSegment, storageOffset, typeStruct->declNode);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Lambda)
    {
        *(uint64_t*) ptrDest = 0;
        auto funcDecl        = CastAst<AstFuncDecl>(typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
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
            if (typeInfo->isNative(NativeTypeKind::String))
            {
                SWAG_ASSERT(value);
                Register* storedV  = (Register*) ptrDest;
                storedV[0].pointer = (uint8_t*) value->text.c_str();
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
                    return internalError(context, "collectStructLiterals, invalid native type sizeof");
                }

                SWAG_ASSERT(typeInfo->sizeOf);
            }
        }
        else if (typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeSub = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(collectStructLiterals(context, storageSegment, offsetStruct + field->offset, typeSub->declNode));
        }

        if (varDecl->type && varDecl->type->flags & AST_HAS_STRUCT_PARAMETERS)
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
    for (auto child : node->childs)
    {
        auto typeInfo = child->typeInfo;

        // Special type when collecting (like an array collected to a slice)
        if (child->extension && child->extension->collectTypeInfo)
            typeInfo = child->extension->collectTypeInfo;

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
        }

        SWAG_CHECK(storeToSegment(context, storageSegment, offset, child->computedValue, typeInfo, assignment));

        // castOffset can store the padding between one field and one other, in case we collect for a struct
        if (child->extension && child->extension->castOffset)
            offset += child->extension->castOffset;
        else
            offset += child->typeInfo->sizeOf;
    }

    return true;
}

bool SemanticJob::collectAssignment(SemanticContext* context, DataSegment* storageSegment, uint32_t& storageOffset, AstVarDecl* node)
{
    auto typeInfo = TypeManager::concreteReferenceType(node->typeInfo);
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

    // Already computed in the constant storageSegment for an array
    if (typeInfo->kind == TypeInfoKind::Array)
    {
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
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (node->assignment && node->assignment->kind == AstNodeKind::IdentifierRef && node->assignment->resolvedSymbolOverload)
        {
            // Do not initialize variable with type arguments, then again with an initialization
            auto assign   = node->assignment;
            auto overload = assign->resolvedSymbolOverload;
            if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
                return context->report({assign, Msg0645});

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
            // First collect values from the structure default init
            SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, value, typeInfo, nullptr));

            // Then collect values from the type parameters
            if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
            {
                auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
                auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
                SWAG_CHECK(storeToSegment(context, storageSegment, storageOffset, value, typeInfo, identifier->callParameters));
            }

            SWAG_ASSERT(!node->assignment || node->assignment->kind != AstNodeKind::ExpressionList);
        }

        return true;
    }

    SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, value, typeInfo, node->assignment));
    return true;
}

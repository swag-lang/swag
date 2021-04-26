#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Generic.h"

bool SemanticJob::reserveAndStoreToSegment(JobContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    if (seg == &module->constantSegment || seg == &module->bssSegment || seg == &module->compilerSegment)
        seg->mutex.lock();

    // Need to lock both data and constant segments, as both can be modified if 'seg' is a data
    else
    {
        SWAG_ASSERT(seg == &module->mutableSegment);
        seg->mutex.lock();
        module->constantSegment.mutex.lock();
    }

    auto result = reserveAndStoreToSegmentNoLock(context, storageOffset, seg, value, typeInfo, assignment);

    if (seg == &module->constantSegment || seg == &module->bssSegment || seg == &module->compilerSegment)
        seg->mutex.unlock();
    else
    {
        module->constantSegment.mutex.unlock();
        seg->mutex.unlock();
    }

    return result;
}

bool SemanticJob::storeToSegment(SemanticContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    if (seg == &module->constantSegment || seg == &module->bssSegment)
        seg->mutex.lock();

    // Need to lock both data and constant segments, as both can be modified if 'seg' is a data
    else
    {
        SWAG_ASSERT(seg == &module->mutableSegment);
        seg->mutex.lock();
        module->constantSegment.mutex.lock();
    }

    auto result = storeToSegmentNoLock(context, storageOffset, seg, value, typeInfo, assignment);

    if (seg == &module->constantSegment || seg == &module->bssSegment)
        seg->mutex.unlock();
    else
    {
        module->constantSegment.mutex.unlock();
        seg->mutex.unlock();
    }

    return result;
}

bool SemanticJob::reserveAndStoreToSegmentNoLock(JobContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    storageOffset = seg->reserveNoLock(typeInfo);
    return storeToSegmentNoLock(context, storageOffset, seg, value, typeInfo, assignment);
}

bool SemanticJob::storeToSegmentNoLock(JobContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    auto     sourceFile = context->sourceFile;
    auto     module     = sourceFile->module;
    uint8_t* ptrDest    = seg->addressNoLock(storageOffset);

    if (typeInfo->isNative(NativeTypeKind::String))
    {
        if (!value->text.empty())
        {
            *(const char**) ptrDest                = value->text.c_str();
            *(uint64_t*) (ptrDest + sizeof(void*)) = value->text.length();
            auto offset                            = module->constantSegment.addStringNoLock(value->text);
            seg->addInitPtr(storageOffset, offset, SegmentKind::Constant);
        }

        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::Any))
    {
        if (!assignment->castedTypeInfo)
            return internalError(context, "storeToSegment, cannot resolve any");

        // Store value in constant segment
        uint32_t storageOffsetValue;
        SWAG_CHECK(reserveAndStoreToSegmentNoLock(context, storageOffsetValue, &module->constantSegment, value, assignment->castedTypeInfo, assignment));

        // Then reference that value and the concrete type info
        auto ptrStorage                     = module->constantSegment.addressNoLock(storageOffsetValue);
        *(void**) ptrDest                   = ptrStorage;
        *(void**) (ptrDest + sizeof(void*)) = module->typeSegment.address(assignment->concreteTypeInfoStorage);

        seg->addInitPtr(storageOffset, storageOffsetValue, SegmentKind::Constant);
        seg->addInitPtr(storageOffset + 8, assignment->concreteTypeInfoStorage, SegmentKind::Type);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        if (assignment)
        {
            SWAG_VERIFY(assignment->kind == AstNodeKind::ExpressionList, context->report({assignment, Msg0641                                         }));
            SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->report({assignment, Msg0642                                         }));

            // Store value in constant segment
            uint32_t storageOffsetValue;
            SWAG_CHECK(reserveAndStoreToSegmentNoLock(context, storageOffsetValue, &module->constantSegment, value, assignment->typeInfo, assignment));

            // Then setup the pointer to that data, and the data count
            auto ptrStorage                        = module->constantSegment.addressNoLock(storageOffsetValue);
            *(void**) ptrDest                      = ptrStorage;
            *(uint64_t*) (ptrDest + sizeof(void*)) = assignment->childs.size();
            seg->addInitPtr(storageOffset, storageOffsetValue, SegmentKind::Constant);
        }

        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::FuncCallParams)
    {
        SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->report({assignment, Msg0643                                         }));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegmentNoLock(context, storageOffset, offset, assignment, seg);
        SWAG_CHECK(result);
        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::ExpressionList)
    {
        SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->report({assignment, Msg0644                                         }));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegmentNoLock(context, storageOffset, offset, assignment, seg);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto offset     = storageOffset;
        auto result     = collectStructLiteralsNoLock(context, sourceFile, offset, typeStruct->declNode, seg);
        SWAG_CHECK(result);
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

bool SemanticJob::collectStructLiteralsNoLock(JobContext* context, SourceFile* sourceFile, uint32_t& offset, AstNode* node, DataSegment* segment)
{
    AstStruct* structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
    auto       module     = sourceFile->module;

    auto ptrDest    = segment->addressNoLock(offset);
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(structNode->typeInfo, TypeInfoKind::Struct);

    auto cptField  = 0;
    auto numFields = typeStruct->fields.size();
    for (auto field : typeStruct->fields)
    {
        auto child    = field->declNode;
        auto varDecl  = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeInfo = TypeManager::concreteType(varDecl->typeInfo);
        if (varDecl->assignment)
        {
            auto& value = varDecl->assignment->computedValue;
            if (typeInfo->isNative(NativeTypeKind::String))
            {
                Register* storedV  = (Register*) ptrDest;
                storedV[0].pointer = (uint8_t*) value.text.c_str();
                storedV[1].u64     = value.text.length();
                auto strOffset     = module->constantSegment.addStringNoLock(value.text);
                segment->addInitPtr(offset, strOffset, SegmentKind::Constant);
                ptrDest += 2 * sizeof(Register);
                offset += 2 * sizeof(Register);
            }
            else
            {
                switch (typeInfo->sizeOf)
                {
                case 1:
                    *(uint8_t*) ptrDest = value.reg.u8;
                    break;
                case 2:
                    *(uint16_t*) ptrDest = value.reg.u16;
                    break;
                case 4:
                    *(uint32_t*) ptrDest = value.reg.u32;
                    break;
                case 8:
                    *(uint64_t*) ptrDest = value.reg.u64;
                    break;
                default:
                    return internalError(context, "collectStructLiterals, invalid native type sizeof");
                }

                SWAG_ASSERT(typeInfo->sizeOf);
                ptrDest += typeInfo->sizeOf;
                offset += typeInfo->sizeOf;
            }
        }
        else if (typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeSub = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(collectStructLiteralsNoLock(context, sourceFile, offset, typeSub->declNode, segment));
            if (cptField != numFields - 1)
                ptrDest = segment->addressNoLock(offset);
        }

        cptField++;
    }

    return true;
}

bool SemanticJob::collectLiteralsToSegmentNoLock(JobContext* context, uint32_t baseOffset, uint32_t& offset, AstNode* node, DataSegment* segment)
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

        SWAG_CHECK(storeToSegmentNoLock(context, offset, segment, &child->computedValue, typeInfo, assignment));

        // castOffset can store the padding between one field and one other, in case we collect for a struct
        if (child->castOffset)
            offset += child->castOffset;
        else
            offset += child->typeInfo->sizeOf;
    }

    return true;
}

bool SemanticJob::collectAssignment(SemanticContext* context, uint32_t& storageOffset, AstVarDecl* node, DataSegment* seg)
{
    auto value    = node->assignment ? &node->assignment->computedValue : &node->computedValue;
    auto typeInfo = TypeManager::concreteReferenceType(node->typeInfo);
    if (typeInfo->sizeOf == 0)
        return true;

    // Already computed in the constant segment for an array
    if (typeInfo->kind == TypeInfoKind::Array)
    {
        if (node->assignment && node->assignment->flags & AST_VALUE_COMPUTED)
        {
            SWAG_ASSERT(node->assignment->computedValue.reg.offset != UINT32_MAX);

            // Should be stored in the compiler segment !
            storageOffset = seg->reserve(typeInfo->sizeOf, SemanticJob::alignOf(node));
            auto addrDst  = seg->address(storageOffset);
            auto addrSrc  = node->sourceFile->module->compilerSegment.address(node->assignment->computedValue.reg.offset);
            memcpy(addrDst, addrSrc, typeInfo->sizeOf);
            return true;
        }
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (node->assignment && node->assignment->kind == AstNodeKind::IdentifierRef && node->assignment->resolvedSymbolOverload)
        {
            // Do not initialize variable with type arguments, then again with an initialization
            if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
                return context->report({node->assignment, Msg0645                                                                               });

            // Copy from a constant
            SWAG_ASSERT(node->assignment->flags & AST_CONST_EXPR);
            storageOffset = seg->reserve(typeInfo->sizeOf, SemanticJob::alignOf(node));
            auto addrDst  = seg->address(storageOffset);
            auto addrSrc  = node->sourceFile->module->constantSegment.address(node->assignment->resolvedSymbolOverload->storageOffset);
            memcpy(addrDst, addrSrc, typeInfo->sizeOf);
        }
        else
        {
            // First collect values from the structure default init
            SWAG_CHECK(reserveAndStoreToSegment(context, storageOffset, seg, value, typeInfo, nullptr));

            // Then collect values from the type parameters
            if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
            {
                auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
                auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
                SWAG_CHECK(storeToSegment(context, storageOffset, seg, value, typeInfo, identifier->callParameters));
            }

            SWAG_ASSERT(!node->assignment || node->assignment->kind != AstNodeKind::ExpressionList);
        }
    }
    else
    {
        SWAG_CHECK(reserveAndStoreToSegment(context, storageOffset, seg, value, typeInfo, node->assignment));
    }

    return true;
}

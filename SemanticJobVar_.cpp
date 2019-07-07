#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Global.h"
#include "Scope.h"
#include "TypeManager.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeInfo.h"
#include "Register.h"

bool SemanticJob::collectLiterals(SourceFile* sourceFile, uint32_t& offset, AstNode* node, SegmentBuffer buffer)
{
    auto     module  = sourceFile->module;
    uint8_t* ptrDest = buffer == SegmentBuffer::Constant ? &module->constantSegment[offset] : &module->dataSegment[offset];

    for (auto child : node->childs)
    {
        if (child->kind == AstNodeKind::ExpressionList)
        {
            SWAG_CHECK(collectLiterals(sourceFile, offset, child, buffer));
            continue;
        }

        if (child->typeInfo->isNative(NativeType::String))
        {
            Register* storedV  = (Register*) ptrDest;
            storedV[0].pointer = (uint8_t*) child->computedValue.text.c_str();
            storedV[1].u64     = child->computedValue.text.length();

            auto stringIndex = module->reserveString(child->computedValue.text);
            if (buffer == SegmentBuffer::Constant)
                module->addConstantSegmentInitString(offset, stringIndex);
            else
                module->addDataSegmentInitString(offset, stringIndex);

            ptrDest += 2 * sizeof(Register);
            offset += 2 * sizeof(Register);
        }
        else
        {
            switch (child->typeInfo->sizeOf)
            {
            case 1:
                *(uint8_t*) ptrDest = child->computedValue.reg.u8;
                ptrDest += 1;
                offset += 1;
                break;
            case 2:
                *(uint16_t*) ptrDest = child->computedValue.reg.u16;
                ptrDest += 2;
                offset += 2;
                break;
            case 4:
                *(uint32_t*) ptrDest = child->computedValue.reg.u32;
                ptrDest += 4;
                offset += 4;
                break;
            case 8:
                *(uint64_t*) ptrDest = child->computedValue.reg.u64;
                ptrDest += 8;
                offset += 8;
                break;

            default:
                sourceFile->report({sourceFile, node, "collectLiterals, invalid type size"});
                return false;
            }
        }
    }

    return true;
}

bool SemanticJob::resolveConstDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = static_cast<AstVarDecl*>(context->node);

    if (!node->astAssignment)
        return sourceFile->report({sourceFile, node, "a constant must be initialized"});

    // Be sure we have a constant value
    if (node->astAssignment->flags & AST_CONST_EXPR)
    {
        SWAG_CHECK(executeNode(context, node->astAssignment, true));
        if (context->result == SemanticResult::Pending)
            return true;
    }

    node->inheritComputedValue(node->astAssignment);
    SWAG_VERIFY(node->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, node->token, format("constant value '%s' cannot be evaluated at compile time", node->name.c_str())}));
    node->flags |= AST_NO_BYTECODE;

    // Find type
    if (node->astType && node->astAssignment)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->astType->typeInfo, node->astAssignment));
        node->typeInfo = node->astType->typeInfo;
    }
    else if (node->astAssignment)
    {
        node->typeInfo = node->astAssignment->typeInfo;
    }
    else if (node->astType)
    {
        node->typeInfo = node->astType->typeInfo;
    }

    node->typeInfo = TypeManager::concreteType(node->typeInfo);
    SWAG_VERIFY(node->typeInfo, sourceFile->report({sourceFile, node->token, format("unable to deduce type of constant '%s'", node->name.c_str())}));

    // Register symbol with its type
    auto overload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Variable, &node->computedValue, 0);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Variable));
    node->resolvedSymbolOverload = overload;

    return true;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = static_cast<AstVarDecl*>(context->node);

    uint32_t symbolFlags = 0;
    if (node->kind == AstNodeKind::FuncDeclParam)
        symbolFlags |= OVERLOAD_VAR_FUNC_PARAM;
    else if (node->ownerScope->isGlobal())
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else
        symbolFlags |= OVERLOAD_VAR_LOCAL;

    // Value
    if (node->astAssignment && node->astAssignment->kind != AstNodeKind::ExpressionList)
    {
        if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_FUNC_PARAM) || (node->astAssignment->flags & AST_CONST_EXPR))
        {
            SWAG_CHECK(executeNode(context, node->astAssignment, true));
            if (context->result == SemanticResult::Pending)
                return true;
        }

        SWAG_VERIFY(node->astAssignment->typeInfo->kind != TypeInfoKind::Array, sourceFile->report({sourceFile, node->astAssignment, "affect not allowed from an array"}));
    }

    // A global variable must have its value computed at that point
    if (node->astAssignment && (symbolFlags & OVERLOAD_VAR_GLOBAL))
    {
        SWAG_VERIFY(node->astAssignment->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, node->astAssignment, "can't evaluate initialization expression at compile time"}));
    }

    // Be sure array without a size have a initializer, to deduce the size
    if (node->astType && node->astType->typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(node->astType->typeInfo, TypeInfoKind::Array);
        SWAG_VERIFY(typeArray->count != UINT32_MAX || node->astAssignment, sourceFile->report({sourceFile, node, "missing initialization expression to deduce size of array"}));
        SWAG_VERIFY(!node->astAssignment || node->astAssignment->kind == AstNodeKind::ExpressionList, sourceFile->report({sourceFile, node, "invalid initialization expression for an array"}));

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count  = (uint32_t) node->astAssignment->childs.size();
            typeArray->sizeOf = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name   = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
        }
    }

    // Find type
    if (node->astType && node->astAssignment)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->astType->typeInfo, node->astAssignment));
        node->typeInfo = node->astType->typeInfo;
    }
    else if (node->astAssignment)
    {
        node->typeInfo = node->astAssignment->typeInfo;

        // Convert from initialization list to array
        if (node->typeInfo->kind == TypeInfoKind::TypeList)
        {
            auto typeList          = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeList);
            auto typeArray         = g_Pool_typeInfoArray.alloc();
            typeArray->pointedType = typeList->childs.front();
            typeArray->sizeOf      = node->typeInfo->sizeOf;
            typeArray->count       = (uint32_t) typeList->childs.size();
            typeArray->name        = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
            node->typeInfo         = g_TypeMgr.registerType(typeArray);
            SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->typeInfo, node->astAssignment));
        }
    }
    else if (node->astType)
    {
        node->typeInfo = node->astType->typeInfo;
    }

    node->typeInfo = TypeManager::concreteType(node->typeInfo);
    SWAG_VERIFY(node->typeInfo, sourceFile->report({sourceFile, node->token, format("unable to deduce type of variable '%s'", node->name.c_str())}));

    // Register symbol with its type
    auto overload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Variable, nullptr, symbolFlags);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Variable));
    node->resolvedSymbolOverload = overload;

    // Assign value
    auto typeInfo = TypeManager::concreteType(node->typeInfo);
    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
    {
        auto     value    = node->astAssignment ? &node->astAssignment->computedValue : &node->computedValue;
        void*    ptrValue = &value->reg;
        Register storedV[2];
        if (typeInfo->isNative(NativeType::String))
        {
            // Store direct pointer to text. Not sure this is really safe
            storedV[0].pointer = (uint8_t*) value->text.c_str();
            storedV[1].u64     = value->text.length();
            ptrValue           = &storedV;
        }

        overload->storageOffset = sourceFile->module->reserveDataSegment(typeInfo->sizeOf, ptrValue);

        if (typeInfo->isNative(NativeType::String))
        {
            auto strIndex = sourceFile->module->reserveString(value->text);
            sourceFile->module->addDataSegmentInitString(overload->storageOffset, strIndex);
        }
        else if (node->astAssignment && node->astAssignment->typeInfo->kind == TypeInfoKind::TypeList)
        {
            sourceFile->module->mutexDataSeg.lock();
            auto offset = overload->storageOffset;
            auto result = collectLiterals(sourceFile, offset, node->astAssignment, SegmentBuffer::Data);
            sourceFile->module->mutexDataSeg.unlock();
            SWAG_CHECK(result);
        }
    }
    else if (symbolFlags & OVERLOAD_VAR_LOCAL)
    {
        SWAG_ASSERT(node->ownerScope);
        SWAG_ASSERT(node->ownerFct);
        overload->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        node->byteCodeFct         = &ByteCodeGenJob::emitVarDecl;
    }

    // Attributes
    if (context->node->parentAttributes)
    {
        collectAttributes(context, overload->attributes, node->parentAttributes, context->node, AstNodeKind::VarDecl, node->attributeFlags);
    }

    return true;
}

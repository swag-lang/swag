#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Scope.h"
#include "TypeManager.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Module.h"
#include "Ast.h"

bool SemanticJob::collectLiterals(SourceFile* sourceFile, uint32_t& offset, AstNode* node, vector<AstNode*>* orderedChilds, SegmentBuffer buffer)
{
    auto module = sourceFile->module;

    uint8_t* ptrDest;
    if (buffer == SegmentBuffer::Constant)
        ptrDest = &module->constantSegment[offset];
    else if (buffer == SegmentBuffer::Data)
        ptrDest = &module->dataSegment[offset];
    else
        ptrDest = nullptr;

    for (auto child : node->childs)
    {
        if (child->kind == AstNodeKind::ExpressionList)
        {
            SWAG_CHECK(collectLiterals(sourceFile, offset, child, orderedChilds, buffer));
            continue;
        }

        if (orderedChilds)
            orderedChilds->push_back(child);

        // We do not want to store the result in the constant/data buffer
        if (!ptrDest)
            continue;

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

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = static_cast<AstVarDecl*>(context->node);

    bool isConstant = node->kind == AstNodeKind::ConstDecl ? true : false;

    uint32_t symbolFlags = 0;
    if (node->kind == AstNodeKind::FuncDeclParam)
        symbolFlags |= OVERLOAD_VAR_FUNC_PARAM;
    else if (node->ownerScope->isGlobal())
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->kind == ScopeKind::Struct)
        symbolFlags |= OVERLOAD_VAR_STRUCT;
    else if (!isConstant)
        symbolFlags |= OVERLOAD_VAR_LOCAL;
    if (node->kind == AstNodeKind::LetDecl)
        symbolFlags |= OVERLOAD_CONST;

    // A constant must be initialized
    if (isConstant && !node->assignment && !(node->flags & AST_VALUE_COMPUTED))
    {
        return sourceFile->report({sourceFile, node, "a constant must be initialized"});
    }

    if ((symbolFlags & OVERLOAD_CONST) && !node->assignment)
    {
        return sourceFile->report({sourceFile, node, "a unmutable 'let' variable must be initialized"});
    }

    // Value
    if (node->assignment && node->assignment->kind != AstNodeKind::ExpressionList)
    {
        if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_FUNC_PARAM) || (node->assignment->flags & AST_CONST_EXPR))
        {
            SWAG_CHECK(executeNode(context, node->assignment, true));
            if (context->result == SemanticResult::Pending)
                return true;
        }

        if (node->type && node->type->typeInfo->kind != TypeInfoKind::Slice)
        {
            SWAG_VERIFY(node->assignment->typeInfo->kind != TypeInfoKind::Array, sourceFile->report({sourceFile, node->assignment, "affect not allowed from an array"}));
        }
    }

    // A global variable or a constant must have its value computed at that point
    if (node->assignment && (isConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL)))
    {
        SWAG_VERIFY(node->assignment->flags & AST_CONST_EXPR, sourceFile->report({sourceFile, node->assignment, "initialization expression cannot be evaluated at compile time"}));
    }

    // Be sure array without a size have a initializer, to deduce the size
    if (node->type && node->type->typeInfo && node->type->typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(node->type->typeInfo, TypeInfoKind::Array);
        SWAG_VERIFY(typeArray->count != UINT32_MAX || node->assignment, sourceFile->report({sourceFile, node, "missing initialization expression to deduce size of array"}));
        SWAG_VERIFY(!node->assignment || node->assignment->kind == AstNodeKind::ExpressionList, sourceFile->report({sourceFile, node, "invalid initialization expression for an array"}));

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count  = (uint32_t) node->assignment->childs.size();
            typeArray->sizeOf = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name   = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
        }
    }

    // Find type
    if (node->type && node->assignment)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->type->typeInfo, node->assignment));
        node->typeInfo = node->type->typeInfo;
    }
    else if (node->assignment)
    {
        node->typeInfo = g_TypeMgr.concreteType(node->assignment->typeInfo);

        // Convert from initialization list to array
        if (node->typeInfo->kind == TypeInfoKind::TypeList)
        {
            auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeList);
            if (typeList->listKind == TypeInfoListKind::Array)
            {
                auto typeArray         = g_Pool_typeInfoArray.alloc();
                typeArray->pointedType = typeList->childs.front();
                typeArray->sizeOf      = node->typeInfo->sizeOf;
                typeArray->count       = (uint32_t) typeList->childs.size();
                typeArray->name        = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
                node->typeInfo         = g_TypeMgr.registerType(typeArray);
                SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->typeInfo, node->assignment));
            }
            else if (typeList->listKind == TypeInfoListKind::Tuple)
            {
                auto typeTuple   = static_cast<TypeInfoList*>(typeList->clone());
                typeTuple->scope = Ast::newScope("", ScopeKind::TypeList, node->ownerScope);
                node->typeInfo   = g_TypeMgr.registerType(typeTuple);
                SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->typeInfo, node->assignment));
            }
            else
                return internalError(context, "resolveVarDecl, invalid typelist kind");
        }
    }
    else if (node->type)
    {
        node->typeInfo = node->type->typeInfo;
    }

    if (!(node->flags & AST_IS_GENERIC))
    {
        SWAG_VERIFY(node->typeInfo, sourceFile->report({sourceFile, node->token, format("unable to deduce type of variable '%s'", node->name.c_str())}));
        SWAG_VERIFY(node->typeInfo->kind != TypeInfoKind::VariadicValue, sourceFile->report({sourceFile, node, "declaration not allowed on a variadic value, you must cast"}));
    }
    else
    {
        symbolFlags |= OVERLOAD_GENERIC;
		if (!node->typeInfo)
		{
            node->typeInfo       = g_Pool_typeInfoGeneric.alloc();
            node->typeInfo->name = node->name;
            node->typeInfo       = g_TypeMgr.registerType(node->typeInfo);
		}
    }

    // A constant does nothing on backend, except if it can't be stored in a register
    uint32_t storageOffset = 0;
    if (isConstant)
    {
        assert(node->typeInfo);

        // Set it as const (so this is a new type)
        if (node->typeInfo->kind != TypeInfoKind::Native)
        {
            auto typeInfo = node->typeInfo->clone();
            typeInfo->setConst();
            node->typeInfo = g_TypeMgr.registerType(typeInfo);
        }

        node->flags |= AST_NO_BYTECODE;
        if (node->typeInfo->kind != TypeInfoKind::Array)
        {
            node->inheritComputedValue(node->assignment);
        }
        else
        {
            // Reserve space in constant segment
            auto module    = sourceFile->module;
            auto typeArray = CastTypeInfo<TypeInfoArray>(node->typeInfo, TypeInfoKind::Array);
            storageOffset  = module->reserveConstantSegment(typeArray->sizeOf);
            module->mutexConstantSeg.lock();
            auto offset = storageOffset;
            auto result = SemanticJob::collectLiterals(context->sourceFile, offset, node, nullptr, SegmentBuffer::Constant);
            module->mutexConstantSeg.unlock();
            SWAG_CHECK(result);
        }
    }

    // Assign value
    auto module   = sourceFile->module;
    auto typeInfo = TypeManager::concreteType(node->typeInfo);
    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
    {
        auto value    = node->assignment ? &node->assignment->computedValue : &node->computedValue;
        storageOffset = sourceFile->module->reserveDataSegment(typeInfo->sizeOf);

        module->mutexDataSeg.lock();
        if (typeInfo->isNative(NativeType::String))
        {
            uint8_t* ptrDest                       = module->dataSegment.data() + storageOffset;
            *(const char**) ptrDest                = value->text.c_str();
            *(uint64_t*) (ptrDest + sizeof(void*)) = value->text.length();
            auto stringIndex                       = module->reserveString(value->text);
            module->addDataSegmentInitString(storageOffset, stringIndex);
        }
        else if (typeInfo->kind == TypeInfoKind::Native)
        {
            uint8_t* ptrDest = module->dataSegment.data() + storageOffset;
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
            default:
                module->mutexDataSeg.unlock();
                return internalError(context, "emitVarDecl, init native, bad size");
            }
        }
        else if (node->assignment && node->assignment->typeInfo->kind == TypeInfoKind::TypeList)
        {
            SWAG_VERIFY(node->assignment->flags & AST_CONST_EXPR, sourceFile->report({sourceFile, node, "expression cannot be evaluated at compile time"}));
            auto offset = storageOffset;
            auto result = collectLiterals(sourceFile, offset, node->assignment, nullptr, SegmentBuffer::Data);
            SWAG_CHECK(result);
        }
        else if (typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            auto offset     = storageOffset;
            auto result     = collectStructLiterals(context, sourceFile, offset, typeStruct->structNode, SegmentBuffer::Data);
            SWAG_CHECK(result);
        }

        module->mutexDataSeg.unlock();
    }
    else if (symbolFlags & OVERLOAD_VAR_LOCAL)
    {
        SWAG_ASSERT(node->ownerScope);
        SWAG_ASSERT(node->ownerFct);
        storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        node->byteCodeFct         = &ByteCodeGenJob::emitVarDecl;
    }

    // Attributes
    SymbolAttributes attributes;
    if (context->node->parentAttributes)
    {
        collectAttributes(context, attributes, node->parentAttributes, context->node, AstNodeKind::VarDecl, node->attributeFlags);
    }

    // Register symbol with its type
    auto symbolKind = SymbolKind::Variable;
    if (symbolFlags & OVERLOAD_GENERIC)
        symbolKind = SymbolKind::GenericType;

    auto overload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile,
                                                                  node,
                                                                  node->typeInfo,
                                                                  symbolKind,
                                                                  isConstant ? &node->computedValue : nullptr,
                                                                  symbolFlags,
                                                                  nullptr,
                                                                  storageOffset,
                                                                  &attributes);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Variable));
    node->resolvedSymbolOverload = overload;

    return true;
}

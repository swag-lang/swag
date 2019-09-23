#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"

bool SemanticJob::collectLiterals(SourceFile* sourceFile, uint32_t& offset, AstNode* node, vector<AstNode*>* orderedChilds, DataSegment* segment)
{
    auto module = sourceFile->module;

    uint8_t* ptrDest = segment ? segment->addressNoLock(offset) : nullptr;
    for (auto child : node->childs)
    {
        if (child->kind == AstNodeKind::ExpressionList)
        {
            SWAG_CHECK(collectLiterals(sourceFile, offset, child, orderedChilds, segment));
            continue;
        }

        if (orderedChilds)
            orderedChilds->push_back(child);

        // We do not want to store the result in the constant/data buffer
        if (!ptrDest)
            continue;

        if (child->typeInfo->isNative(NativeTypeKind::String))
        {
            Register* storedV  = (Register*) ptrDest;
            storedV[0].pointer = (uint8_t*) child->computedValue.text.c_str();
            storedV[1].u64     = child->computedValue.text.length();

            auto stringIndex = module->reserveString(child->computedValue.text);
            segment->addInitString(offset, stringIndex);

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

bool SemanticJob::storeToSegment(SemanticContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    scoped_lock lock(seg->mutex);

    auto sourceFile  = context->sourceFile;
    auto module      = sourceFile->module;
    storageOffset    = seg->reserveNoLock(typeInfo->sizeOf);
    uint8_t* ptrDest = seg->addressNoLock(storageOffset);

    if (typeInfo->isNative(NativeTypeKind::String))
    {
        *(const char**) ptrDest                = value->text.c_str();
        *(uint64_t*) (ptrDest + sizeof(void*)) = value->text.length();
        auto stringIndex                       = module->reserveString(value->text);
        seg->addInitString(storageOffset, stringIndex);
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::Any))
    {
        if (!assignment->castedTypeInfo)
            return internalError(context, "storeToSegment, cannot resolve any");

        // Store value in constant segment
        uint32_t storageOffsetValue;
        SWAG_CHECK(storeToSegment(context, storageOffsetValue, &module->constantSegment, value, assignment->castedTypeInfo, assignment));

        // Then reference that value and the concrete type info
        auto ptrStorage                     = module->constantSegment.address(storageOffsetValue);
        *(void**) ptrDest                   = ptrStorage;
        *(void**) (ptrDest + sizeof(void*)) = module->constantSegment.address(assignment->concreteTypeInfoStorage);

        seg->addInitPtr(storageOffset, storageOffsetValue, SegmentKind::Constant);
        seg->addInitPtr(storageOffset + 8, assignment->concreteTypeInfoStorage, SegmentKind::Constant);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
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
            return internalError(context, "storeToSegment, init native, bad size");
        }

        return true;
    }

    if (assignment && assignment->typeInfo->kind == TypeInfoKind::TypeList)
    {
        SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->errorContext.report({sourceFile, assignment, "expression cannot be evaluated at compile time"}));
        auto offset = storageOffset;
        auto result = collectLiterals(sourceFile, offset, assignment, nullptr, seg);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto offset     = storageOffset;
        auto result     = collectStructLiterals(context, sourceFile, offset, typeStruct->structNode, seg);
        SWAG_CHECK(result);
        return true;
    }

    return true;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto  sourceFile         = context->sourceFile;
    auto& typeTable          = sourceFile->module->typeTable;
    auto  node               = static_cast<AstVarDecl*>(context->node);
    bool  isCompilerConstant = node->kind == AstNodeKind::ConstDecl ? true : false;

    uint32_t symbolFlags = 0;
    if (node->kind == AstNodeKind::FuncDeclParam)
        symbolFlags |= OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_CONST;
    else if (node->ownerScope->isGlobal())
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->kind == ScopeKind::Struct)
        symbolFlags |= OVERLOAD_VAR_STRUCT;
    else if (!isCompilerConstant)
        symbolFlags |= OVERLOAD_VAR_LOCAL;
    if (node->kind == AstNodeKind::LetDecl)
        symbolFlags |= OVERLOAD_CONST;

    // A constant must be initialized
    if (isCompilerConstant && !node->assignment && !(node->flags & AST_VALUE_COMPUTED))
        return context->errorContext.report({sourceFile, node, "a constant must be initialized"});

    if ((symbolFlags & OVERLOAD_CONST) && !node->assignment && node->kind != AstNodeKind::FuncDeclParam)
    {
        // This is ok to not have an initialization for structs, as they are initialized by default
        if (!node->type || node->type->typeInfo->kind != TypeInfoKind::Struct)
            return context->errorContext.report({sourceFile, node, "a non mutable 'let' variable must be initialized"});
    }

    // Value
    if (node->assignment && node->assignment->kind != AstNodeKind::ExpressionList)
    {
        SWAG_CHECK(checkIsConcrete(context, node->assignment));

        if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_FUNC_PARAM) || (node->assignment->flags & AST_CONST_EXPR))
        {
            SWAG_CHECK(evaluateConstExpression(context, node->assignment));
            if (context->result == SemanticResult::Pending)
                return true;
        }

        if (node->type && node->type->typeInfo->kind != TypeInfoKind::Slice)
        {
            SWAG_VERIFY(node->assignment->typeInfo->kind != TypeInfoKind::Array, context->errorContext.report({sourceFile, node->assignment, "affect not allowed from an array"}));
        }
    }

    // A global variable or a constant must have its value computed at that point
    if (node->assignment && (isCompilerConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL)))
    {
        SWAG_VERIFY(node->assignment->flags & AST_CONST_EXPR, context->errorContext.report({sourceFile, node->assignment, "initialization expression cannot be evaluated at compile time"}));
    }

    // Be sure array without a size have a initializer, to deduce the size
    if (node->type && node->type->typeInfo && node->type->typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(node->type->typeInfo, TypeInfoKind::Array);
        SWAG_VERIFY(typeArray->count != UINT32_MAX || node->assignment, context->errorContext.report({sourceFile, node, "missing initialization expression to deduce size of array"}));
        SWAG_VERIFY(!node->assignment || node->assignment->kind == AstNodeKind::ExpressionList, context->errorContext.report({sourceFile, node, "invalid initialization expression for an array"}));

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = (uint32_t) node->assignment->childs.size();
            typeArray->totalCount = typeArray->count;
            typeArray->sizeOf     = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name       = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
        }
    }

    // Find type
    if (node->type && node->assignment)
    {
        // Do not cast for structs, as we can have special assignment with different types
        if (node->type->typeInfo->kind != TypeInfoKind::Struct)
        {
            SWAG_CHECK(TypeManager::makeCompatibles(&context->errorContext, node->type->typeInfo, node->assignment, CASTFLAG_UNCONST));
        }
        else
        {
            auto rightConcreteType = TypeManager::concreteType(node->assignment->typeInfo);
            if (!rightConcreteType->isSame(node->type->typeInfo, 0))
            {
                SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, node->type, node->assignment));
                if (context->result == SemanticResult::Pending)
                    return true;
            }
        }

        node->typeInfo = node->type->typeInfo;
    }
    else if (node->assignment)
    {
        node->typeInfo = TypeManager::concreteType(node->assignment->typeInfo);
        SWAG_ASSERT(node->typeInfo);
        SWAG_VERIFY(node->typeInfo != g_TypeMgr.typeInfoVoid, context->errorContext.report({sourceFile, node->assignment, "type of expression is 'void'"}));

        // We need to decide which integer/float type it is
        if (node->typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
            node->typeInfo = g_TypeMgr.typeInfoS32;
        else if (node->typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
            node->typeInfo = g_TypeMgr.typeInfoF32;

        // Convert from initialization list to array
        if (node->typeInfo->kind == TypeInfoKind::TypeList)
        {
            auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeList);
            if (typeList->listKind == TypeInfoListKind::Array)
            {
                auto typeArray         = g_Pool_typeInfoArray.alloc();
                typeArray->pointedType = typeList->childs.front();
                typeArray->rawType     = typeArray->pointedType;
                typeArray->sizeOf      = node->typeInfo->sizeOf;
                typeArray->count       = (uint32_t) typeList->childs.size();
                typeArray->totalCount  = typeArray->count;
                typeArray->name        = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
                node->typeInfo         = typeTable.registerType(typeArray);
                SWAG_CHECK(TypeManager::makeCompatibles(&context->errorContext, node->typeInfo, node->assignment));
            }
            else if (typeList->listKind == TypeInfoListKind::Tuple)
            {
                auto typeTuple   = static_cast<TypeInfoList*>(typeList->clone());
                typeTuple->scope = Ast::newScope(nullptr, "", ScopeKind::TypeList, node->ownerScope);
                node->typeInfo   = typeTable.registerType(typeTuple);
                SWAG_CHECK(TypeManager::makeCompatibles(&context->errorContext, node->typeInfo, node->assignment));
            }
            else
                return internalError(context, "resolveVarDecl, invalid typelist kind");
        }
    }
    else if (node->type)
    {
        node->typeInfo = node->type->typeInfo;
    }

    if (node->type)
        node->inheritOrFlag(node->type, AST_IS_GENERIC);
    if (!(node->flags & AST_IS_GENERIC))
    {
        SWAG_VERIFY(node->typeInfo, context->errorContext.report({sourceFile, node->token, format("unable to deduce type of variable '%s'", node->name.c_str())}));
        SWAG_VERIFY(node->typeInfo->kind != TypeInfoKind::VariadicValue, context->errorContext.report({sourceFile, node, "declaration not allowed with a variadic type, you must cast to the wanted type"}));
    }
    else
    {
        symbolFlags |= OVERLOAD_GENERIC;
        if (!node->typeInfo)
        {
            node->typeInfo       = g_Pool_typeInfoGeneric.alloc();
            node->typeInfo->name = node->name;
            node->typeInfo       = typeTable.registerType(node->typeInfo);
        }
    }

    // No struct initialization on everything except local vars
    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
        SWAG_VERIFY(symbolFlags & OVERLOAD_VAR_LOCAL, context->errorContext.report({sourceFile, node->type, "cannot initialize a struct with parameters here (only local variables are supported)"}));

        // Determin if the call parameters cover everything (to avoid calling default initialisation)
        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

        TypeInfoStruct* typeStruct = nullptr;
        if (node->typeInfo->kind == TypeInfoKind::Struct)
            typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        else if (node->typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(node->typeInfo, TypeInfoKind::Array);
            while (typeArray->pointedType->kind == TypeInfoKind::Array)
                typeArray = CastTypeInfo<TypeInfoArray>(typeArray->pointedType, TypeInfoKind::Array);
            typeStruct = CastTypeInfo<TypeInfoStruct>(typeArray->pointedType, TypeInfoKind::Struct);
        }

        if (typeStruct && identifier->callParameters->childs.size() == typeStruct->childs.size())
            node->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
    }

    SWAG_ASSERT(node->typeInfo);

    // A constant does nothing on backend, except if it can't be stored in a register
    uint32_t storageOffset = 0;
    if (isCompilerConstant)
    {
        node->flags |= AST_NO_BYTECODE;
        if (node->typeInfo->kind == TypeInfoKind::Array || node->typeInfo->kind == TypeInfoKind::TypeList)
        {
            // Reserve space in constant segment
            auto module   = sourceFile->module;
            storageOffset = module->constantSegment.reserve(node->typeInfo->sizeOf);
            scoped_lock lock(module->constantSegment.mutex);
            auto        offset = storageOffset;
            SWAG_CHECK(SemanticJob::collectLiterals(context->sourceFile, offset, node, nullptr, &module->constantSegment));
            if (!node->typeInfo->isConst())
            {
                auto typeConst = node->typeInfo->clone();
                typeConst->setConst();
                node->typeInfo = typeTable.registerType(typeConst);
            }
        }
        else
        {
            node->inheritComputedValue(node->assignment);
        }
    }

    // An enum must be initialized
    if (!node->assignment && !(symbolFlags & OVERLOAD_VAR_FUNC_PARAM) && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        if (node->typeInfo->kind == TypeInfoKind::Enum ||
            (node->typeInfo->kind == TypeInfoKind::Array && ((TypeInfoArray*) node->typeInfo)->pointedType->kind == TypeInfoKind::Enum))
        {
            return context->errorContext.report({sourceFile, node->token, "an enum variable must be initialized"});
        }
    }

    SWAG_CHECK(dealWithAny(context, node->assignment, node->assignment));
    if (context->result == SemanticResult::Pending)
        return true;

    auto module   = sourceFile->module;
    auto typeInfo = TypeManager::concreteType(node->typeInfo);

    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
    {
        SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->errorContext.report({sourceFile, node, format("cannot instanciate variable because type '%s' is generic", node->typeInfo->name.c_str())}));
        node->flags |= AST_R_VALUE;
        auto value = node->assignment ? &node->assignment->computedValue : &node->computedValue;
        SWAG_CHECK(storeToSegment(context, storageOffset, &module->dataSegment, value, typeInfo, node->assignment));
    }
    else if (symbolFlags & OVERLOAD_VAR_LOCAL)
    {
        // For a struct, need to wait for special functions to be found
        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            SWAG_CHECK(waitForStructUserOps(context, node));
            if (context->result == SemanticResult::Pending)
                return true;
        }

        SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->errorContext.report({sourceFile, node, format("cannot instanciate variable because type '%s' is generic", node->typeInfo->name.c_str())}));
        SWAG_ASSERT(node->ownerScope);
        SWAG_ASSERT(node->ownerFct);
        storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        // Be sure we have a stack if a variable is declared, even if sizeof is null (for an empty struct for example)
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, 1);
        node->byteCodeFct         = &ByteCodeGenJob::emitVarDecl;
        node->flags |= AST_R_VALUE;
    }
    else if (symbolFlags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->flags |= AST_R_VALUE;
    }
    else if (isCompilerConstant)
    {
        node->flags |= AST_R_VALUE;
    }

    // A using on a variable
    if (node->flags & AST_DECL_USING)
        SWAG_CHECK(resolveUsingVar(context, context->node, node->typeInfo));

    // Attributes
    SymbolAttributes attributes;
    if (context->node->parentAttributes)
        collectAttributes(context, attributes, node->parentAttributes, context->node, AstNodeKind::VarDecl, node->attributeFlags);

    // Register symbol with its type
    auto overload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile,
                                                                  node,
                                                                  node->typeInfo,
                                                                  (node->type || node->assignment) ? SymbolKind::Variable : SymbolKind::GenericType,
                                                                  isCompilerConstant ? &node->computedValue : nullptr,
                                                                  symbolFlags,
                                                                  nullptr,
                                                                  storageOffset,
                                                                  &attributes);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Variable));
    node->resolvedSymbolOverload = overload;

    return true;
}

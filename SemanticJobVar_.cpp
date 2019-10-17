#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"

bool SemanticJob::reserveAndStoreToSegment(SemanticContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    auto sourceFile = context->sourceFile;

    // Need to lock both data segments, as both can be modified if 'seg' is a data
    scoped_lock lockConstant(sourceFile->module->constantSegment.mutex);
    scoped_lock lockData(sourceFile->module->mutableSegment.mutex);

    return reserveAndStoreToSegmentNoLock(context, storageOffset, seg, value, typeInfo, assignment);
}

bool SemanticJob::storeToSegment(SemanticContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    auto sourceFile = context->sourceFile;

    // Need to lock both data segments, as both can be modified if 'seg' is a data
    scoped_lock lockConstant(sourceFile->module->constantSegment.mutex);
    scoped_lock lockData(sourceFile->module->mutableSegment.mutex);

    return storeToSegmentNoLock(context, storageOffset, seg, value, typeInfo, assignment);
}

bool SemanticJob::reserveAndStoreToSegmentNoLock(SemanticContext* context, uint32_t& storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    storageOffset = seg->reserveNoLock(typeInfo->sizeOf);
    return storeToSegmentNoLock(context, storageOffset, seg, value, typeInfo, assignment);
}

bool SemanticJob::storeToSegmentNoLock(SemanticContext* context, uint32_t storageOffset, DataSegment* seg, ComputedValue* value, TypeInfo* typeInfo, AstNode* assignment)
{
    auto     sourceFile = context->sourceFile;
    auto     module     = sourceFile->module;
    uint8_t* ptrDest    = seg->addressNoLock(storageOffset);

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
        SWAG_CHECK(reserveAndStoreToSegmentNoLock(context, storageOffsetValue, &module->constantSegment, value, assignment->castedTypeInfo, assignment));
        SWAG_ASSERT(assignment->concreteTypeInfoStorage != UINT32_MAX);

        // Then reference that value and the concrete type info
        auto ptrStorage                     = module->constantSegment.addressNoLock(storageOffsetValue);
        *(void**) ptrDest                   = ptrStorage;
        *(void**) (ptrDest + sizeof(void*)) = module->constantSegment.addressNoLock(assignment->concreteTypeInfoStorage);

        seg->addInitPtr(storageOffset, storageOffsetValue, SegmentKind::Constant);
        seg->addInitPtr(storageOffset + 8, assignment->concreteTypeInfoStorage, SegmentKind::Constant);
        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::FuncCallParams)
    {
        SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->errorContext.report({sourceFile, assignment, "expression cannot be evaluated at compile time"}));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegmentNoLock(context, offset, assignment, seg);
        SWAG_CHECK(result);
        return true;
    }

    if (assignment && assignment->kind == AstNodeKind::ExpressionList)
    {
        SWAG_VERIFY(assignment->flags & AST_CONST_EXPR, context->errorContext.report({sourceFile, assignment, "expression cannot be evaluated at compile time"}));
        auto offset = storageOffset;
        auto result = collectLiteralsToSegmentNoLock(context, offset, assignment, seg);
        SWAG_CHECK(result);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        auto offset     = storageOffset;
        auto result     = collectStructLiteralsNoLock(context, sourceFile, offset, typeStruct->structNode, seg);
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

bool SemanticJob::collectStructLiteralsNoLock(SemanticContext* context, SourceFile* sourceFile, uint32_t& offset, AstNode* node, DataSegment* segment)
{
    AstStruct* structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
    auto       module     = sourceFile->module;

    auto ptrDest = segment->addressNoLock(offset);
    for (auto child : structNode->content->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        if (varDecl->assignment)
        {
            auto  typeInfo = child->typeInfo;
            auto& value    = varDecl->assignment->computedValue;

            if (typeInfo->isNative(NativeTypeKind::String))
            {
                Register* storedV  = (Register*) ptrDest;
                storedV[0].pointer = (uint8_t*) value.text.c_str();
                storedV[1].u64     = value.text.length();

                auto stringIndex = module->reserveString(value.text);
                segment->addInitString(offset, stringIndex);

                ptrDest += 2 * sizeof(Register);
                offset += 2 * sizeof(Register);
            }
            else if (typeInfo->kind == TypeInfoKind::Native)
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

                ptrDest += typeInfo->sizeOf;
                offset += typeInfo->sizeOf;
            }
            else
            {
                return internalError(context, "collectStructLiterals, invalid type");
            }
        }
        else if (varDecl->typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(varDecl->typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(collectStructLiteralsNoLock(context, sourceFile, offset, typeStruct->structNode, segment));
            ptrDest = segment->addressNoLock(offset);
        }
    }

    return true;
}

bool SemanticJob::collectLiteralsToSegmentNoLock(SemanticContext* context, uint32_t& offset, AstNode* node, DataSegment* segment)
{
    for (auto child : node->childs)
    {
        if (child->flags & AST_DONT_COLLECT)
            continue;

        if (child->kind == AstNodeKind::ExpressionList)
        {
            SWAG_CHECK(collectLiteralsToSegmentNoLock(context, offset, child, segment));
            continue;
        }

        SWAG_CHECK(storeToSegmentNoLock(context, offset, segment, &child->computedValue, child->typeInfo, child));
        offset += child->typeInfo->sizeOf;
    }

    return true;
}

bool SemanticJob::convertAssignementToStruct(SemanticContext* context, AstNode* assignment, AstStruct** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode = Ast::newStructDecl(sourceFile, nullptr);
    *result               = structNode;

    auto contentNode               = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode);
    contentNode->semanticBeforeFct = &SemanticJob::preResolveStruct;
    structNode->content            = contentNode;

    auto typeList   = CastTypeInfo<TypeInfoList>(assignment->typeInfo, TypeInfoKind::TypeList);
    Utf8 structName = "__tuple_";
    Utf8 varName;
    int  numChilds = (int) typeList->childs.size();
    for (int idx = 0; idx < numChilds; idx++)
    {
        auto childType = typeList->childs[idx];

        if (idx < typeList->names.size())
        {
            varName = typeList->names[idx];
            structName += varName;
            structName += "_";
        }
        else
        {
            varName = format("val%u", idx);
        }

        structName += childType->name;
        structName += "_";

        auto paramNode      = Ast::newVarDecl(sourceFile, varName, contentNode);
        auto typeExpression = Ast::newTypeExpression(sourceFile, paramNode);
        typeExpression->flags |= AST_NO_BYTECODE_CHILDS;
        paramNode->type = typeExpression;

        // Convert typeinfo to TypeExpression
        switch (childType->kind)
        {
        case TypeInfoKind::Native:
        {
            typeExpression->token.id          = TokenId::NativeType;
            typeExpression->token.literalType = childType;
            break;
        }
        case TypeInfoKind::Pointer:
        {
            auto typeInfoPointer     = CastTypeInfo<TypeInfoPointer>(childType, TypeInfoKind::Pointer);
            typeExpression->ptrCount = typeInfoPointer->ptrCount;
            if (typeInfoPointer->finalType->kind != TypeInfoKind::Native)
                return internalError(context, format("convertAssignementToStruct, cannot convert type '%s'", typeInfoPointer->finalType->name.c_str()).c_str(), assignment->childs[idx]);
            typeExpression->token.id          = TokenId::NativeType;
            typeExpression->token.literalType = typeInfoPointer->finalType;
            break;
        }
        case TypeInfoKind::Enum:
        {
            typeExpression->identifier = Ast::newIdentifierRef(sourceFile, childType->name, typeExpression);
            paramNode->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
            break;
        }
        case TypeInfoKind::Struct:
        {
            typeExpression->identifier = Ast::newIdentifierRef(sourceFile, childType->name, typeExpression);
            break;
        }
        case TypeInfoKind::TypeList:
        {
            AstStruct* inStructNode;
            SWAG_CHECK(convertAssignementToStruct(context, assignment->childs[idx], &inStructNode));
            typeExpression->identifier = Ast::newIdentifierRef(sourceFile, inStructNode->name, typeExpression);
            break;
        }
        default:
            return internalError(context, format("convertAssignementToStruct, cannot convert type '%s'", childType->name.c_str()).c_str(), assignment->childs[idx]);
        }
    }

    // Compute structure name
    structNode->name = move(structName);

    // Add struct type and scope
    auto rootScope = sourceFile->scopeRoot;
    rootScope->allocateSymTable();
    scoped_lock lk(rootScope->symTable->mutex);
    auto        symbol = rootScope->symTable->findNoLock(structNode->name);
    if (symbol)
    {
        // Must release struct node, it's useless
    }
    else
    {
        auto typeInfo = g_Pool_typeInfoStruct.alloc();
        auto newScope = Ast::newScope(structNode, structNode->name, ScopeKind::Struct, rootScope, true);
        newScope->allocateSymTable();
        typeInfo->name       = structNode->name;
        typeInfo->scope      = newScope;
        structNode->typeInfo = typeInfo;
        structNode->scope    = newScope;
        symbol               = rootScope->symTable->registerSymbolNameNoLock(sourceFile, structNode, SymbolKind::Struct);

        Ast::addChildBack(sourceFile->astRoot, structNode);
        structNode->inheritOwners(sourceFile->astRoot);

        Ast::visit(structNode->content, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
        });

        SemanticJob::newJob(sourceFile, structNode, true);
    }

    return true;
}

bool SemanticJob::convertAssignementToStruct(SemanticContext* context, AstNode* parent, AstNode* assignement, AstNode** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode;
    SWAG_CHECK(convertAssignementToStruct(context, assignement, &structNode));

    // Reference to that struct
    auto typeExpression = Ast::newTypeExpression(sourceFile, parent);
    typeExpression->flags |= AST_NO_BYTECODE_CHILDS;
    typeExpression->identifier = Ast::newIdentifierRef(sourceFile, structNode->name, typeExpression);
    *result                    = typeExpression;
    context->job->nodes.push_back(typeExpression);
    context->result = SemanticResult::NewChilds;
    return true;
}

bool SemanticJob::collectAssignment(SemanticContext* context, uint32_t& storageOffset, AstVarDecl* node, DataSegment* seg)
{
    auto value    = node->assignment ? &node->assignment->computedValue : &node->computedValue;
    auto typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->sizeOf == 0)
        return true;

    if (node->typeInfo->kind == TypeInfoKind::Struct)
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

        // Then collect values from the assignment
        if (node->assignment && node->assignment->kind == AstNodeKind::ExpressionList)
            SWAG_CHECK(storeToSegment(context, storageOffset, seg, value, typeInfo, node->assignment));
    }
    else
    {
        SWAG_CHECK(reserveAndStoreToSegment(context, storageOffset, seg, value, typeInfo, node->assignment));
    }

    return true;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto  sourceFile         = context->sourceFile;
    auto  module             = sourceFile->module;
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

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->type || node->type->typeInfo->kind != TypeInfoKind::Struct)
    {
        if (isCompilerConstant && !node->assignment && !(node->flags & AST_VALUE_COMPUTED))
            return context->errorContext.report({sourceFile, node, "a constant must be initialized"});
        if ((symbolFlags & OVERLOAD_CONST) && !node->assignment && node->kind != AstNodeKind::FuncDeclParam)
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

    // Be sure array without a size have an initializer, to deduce the size
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
        SWAG_ASSERT(node->type->typeInfo);

        // Do not cast for structs, as we can have special assignment with different types
        if (node->type->typeInfo->kind != TypeInfoKind::Struct || node->assignment->typeInfo->kind == TypeInfoKind::TypeList)
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, CASTFLAG_UNCONST));
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
        node->typeInfo = TypeManager::concreteType(node->assignment->typeInfo, MakeConcrete::FlagFunc);
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
            if (typeList->listKind == TypeInfoListKind::Bracket)
            {
                auto typeArray         = g_Pool_typeInfoArray.alloc();
                typeArray->pointedType = typeList->childs.front();
                typeArray->finalType   = typeArray->pointedType;
                typeArray->sizeOf      = node->typeInfo->sizeOf;
                typeArray->count       = (uint32_t) typeList->childs.size();
                typeArray->totalCount  = typeArray->count;
                if (isCompilerConstant)
                    typeArray->flags |= TYPEINFO_CONST;
                typeArray->computeName();
                node->typeInfo = typeArray;

                // For a global variable, no need to collect in the constant segment, as we will collect directly to the mutable segment
                if (symbolFlags & OVERLOAD_VAR_GLOBAL)
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, CASTFLAG_NO_COLLECT));
                else
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment));
            }
            else if (typeList->listKind == TypeInfoListKind::Curly)
            {
                SWAG_CHECK(convertAssignementToStruct(context, node, node->assignment, &node->type));
                node->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
                return true;
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
    if (node->flags & AST_IS_GENERIC)
    {
        symbolFlags |= OVERLOAD_GENERIC;
        if (!node->typeInfo)
        {
            node->typeInfo       = g_Pool_typeInfoGeneric.alloc();
            node->typeInfo->name = node->name;
        }
    }

    // We should have a type here !
    SWAG_VERIFY(node->typeInfo, context->errorContext.report({sourceFile, node->token, format("unable to deduce type of variable '%s'", node->name.c_str())}));

    // Determine if the call parameters cover everything (to avoid calling default initialization)
    // i.e. set AST_HAS_FULL_STRUCT_PARAMETERS
    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
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

        if (typeStruct && identifier->callParameters && identifier->callParameters->childs.size() == typeStruct->childs.size())
            node->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
    }

    // A constant does nothing on backend, except if it can't be stored in a register
    uint32_t storageOffset = 0;
    if (isCompilerConstant)
    {
        node->inheritComputedValue(node->assignment);
        if (symbolFlags & (OVERLOAD_VAR_GLOBAL | OVERLOAD_VAR_LOCAL))
        {
            if (node->typeInfo->kind == TypeInfoKind::Array || node->typeInfo->kind == TypeInfoKind::Struct)
            {
                // Be sure type is now constant
                if (!node->typeInfo->isConst())
                {
                    auto typeConst = node->typeInfo->clone();
                    typeConst->setConst();
                    node->typeInfo = typeConst;
                }
            }
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

    auto typeInfo = TypeManager::concreteType(node->typeInfo);

    if (isCompilerConstant)
    {
        node->flags |= AST_NO_BYTECODE | AST_R_VALUE;
        SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->constantSegment));
    }
    else if (symbolFlags & OVERLOAD_VAR_GLOBAL)
    {
        SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->errorContext.report({sourceFile, node, format("cannot instanciate variable because type '%s' is generic", node->typeInfo->name.c_str())}));
        node->flags |= AST_R_VALUE;
        SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->mutableSegment));
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
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, 1); // Be sure we have a stack if a variable is declared, even if sizeof is null (for an empty struct for example)
        node->byteCodeFct         = &ByteCodeGenJob::emitVarDecl;
        node->flags |= AST_R_VALUE;
    }
    else if (symbolFlags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->flags |= AST_R_VALUE;
    }

    // A using on a variable
    if (node->flags & AST_DECL_USING)
    {
        SWAG_CHECK(resolveUsingVar(context, context->node, node->typeInfo));
    }

    // Register symbol with its type
    auto overload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile,
                                                                  node,
                                                                  node->typeInfo,
                                                                  (node->type || node->assignment) ? SymbolKind::Variable : SymbolKind::GenericType,
                                                                  isCompilerConstant ? &node->computedValue : nullptr,
                                                                  symbolFlags,
                                                                  nullptr,
                                                                  storageOffset);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Variable));
    node->resolvedSymbolOverload = overload;

    return true;
}

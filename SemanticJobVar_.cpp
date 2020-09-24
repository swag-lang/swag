#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Generic.h"

bool SemanticJob::convertAssignementToStruct(SemanticContext* context, AstNode* assignment, AstStruct** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode = Ast::newStructDecl(sourceFile, nullptr);
    *result               = structNode;

    auto contentNode               = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode);
    contentNode->semanticBeforeFct = SemanticJob::preResolveStruct;
    structNode->content            = contentNode;

    auto typeList = CastTypeInfo<TypeInfoList>(assignment->typeInfo, TypeInfoKind::TypeListTuple);
    Utf8 varName;
    int  numChilds = (int) typeList->subTypes.size();
    for (int idx = 0; idx < numChilds; idx++)
    {
        auto typeParam = typeList->subTypes[idx];
        auto childType = typeParam->typeInfo;

        bool autoName = false;
        if (!typeParam->namedParam.empty())
            varName = typeParam->namedParam;
        else
        {
            autoName = true;
            varName  = format("item%u", idx);
        }

        auto paramNode = Ast::newVarDecl(sourceFile, varName, contentNode);
        if (autoName)
            paramNode->flags |= AST_AUTO_NAME;
        auto typeExpression = Ast::newTypeExpression(sourceFile, paramNode);
        typeExpression->flags |= AST_NO_BYTECODE_CHILDS;
        paramNode->type = typeExpression;

        // Convert typeinfo to TypeExpression
        switch (childType->kind)
        {
        case TypeInfoKind::Native:
        {
            typeExpression->token.id    = TokenId::NativeType;
            typeExpression->literalType = childType;
            break;
        }
        case TypeInfoKind::Pointer:
        {
            auto typeInfoPointer     = CastTypeInfo<TypeInfoPointer>(childType, TypeInfoKind::Pointer);
            typeExpression->ptrCount = typeInfoPointer->ptrCount;
            typeExpression->isConst  = typeInfoPointer->isConst();
            if (typeInfoPointer->finalType->kind != TypeInfoKind::Native)
                return internalError(context, format("convertAssignementToStruct, cannot convert type '%s'", typeInfoPointer->finalType->name.c_str()).c_str(), assignment->childs[idx]);
            typeExpression->token.id    = TokenId::NativeType;
            typeExpression->literalType = typeInfoPointer->finalType;
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
        case TypeInfoKind::TypeListTuple:
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
    structNode->name = move(typeList->computeTupleName(context));

    // Add struct type and scope
    Scope* rootScope;
    if (sourceFile->module->fromTestsFolder)
        rootScope = sourceFile->scopePrivate;
    else
        rootScope = sourceFile->module->scopeRoot;
    scoped_lock lk(rootScope->symTable.mutex);
    auto        symbol = rootScope->symTable.findNoLock(structNode->name);
    if (symbol)
    {
        // Must release struct node, it's useless
    }
    else
    {
        auto typeInfo        = g_Allocator.alloc<TypeInfoStruct>();
        auto newScope        = Ast::newScope(structNode, structNode->name, ScopeKind::Struct, rootScope, true);
        typeInfo->declNode   = structNode;
        typeInfo->name       = structNode->name;
        typeInfo->nakedName  = structNode->name;
        typeInfo->structName = structNode->name;
        typeInfo->scope      = newScope;
        typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE;
        structNode->typeInfo = typeInfo;
        structNode->scope    = newScope;
        symbol               = rootScope->symTable.registerSymbolNameNoLock(context, structNode, SymbolKind::Struct);

        Ast::addChildBack(sourceFile->astRoot, structNode);
        structNode->inheritOwners(sourceFile->astRoot);

        Ast::visit(structNode->content, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
        });

        SemanticJob::newJob(context->job->dependentJob, sourceFile, structNode, true);
    }

    return true;
}

bool SemanticJob::convertAssignementToStruct(SemanticContext* context, AstNode* parent, AstNode* assignement, AstNode** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode;
    SWAG_CHECK(convertAssignementToStruct(context, assignement, &structNode));

    // Reference to that generated structure
    auto typeExpression = Ast::newTypeExpression(sourceFile, parent);
    typeExpression->flags |= AST_NO_BYTECODE_CHILDS | AST_GENERATED;
    typeExpression->identifier = Ast::newIdentifierRef(sourceFile, structNode->name, typeExpression);
    *result                    = typeExpression;
    return true;
}

bool SemanticJob::resolveVarDeclAfterAssign(SemanticContext* context)
{
    auto savedNode = context->node;
    auto parent    = context->node->parent;
    while (parent && parent->kind != AstNodeKind::VarDecl && parent->kind != AstNodeKind::ConstDecl && parent->kind != AstNodeKind::LetDecl)
        parent = parent->parent;
    SWAG_ASSERT(parent);
    auto varDecl = (AstVarDecl*) parent;

    auto assign = varDecl->assignment;
    if (!assign || assign->kind != AstNodeKind::ExpressionList)
        return true;

    auto exprList = CastAst<AstExpressionList>(assign, AstNodeKind::ExpressionList);
    if (!exprList->forTuple)
        return true;

    if (!varDecl->type)
    {
        SWAG_CHECK(convertAssignementToStruct(context, varDecl, assign, &varDecl->type));
        varDecl->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;

        auto job        = context->job;
        context->result = ContextResult::Done;
        job->nodes.pop_back();
        job->nodes.push_back(varDecl->type);
        job->nodes.push_back(context->node);
    }

    auto typeExpression = CastAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
    if (!typeExpression->identifier)
        return true;

    auto identifier = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
    SWAG_VERIFY(!identifier->callParameters, context->report({assign, "structure cannot be initialized twice"}));

    auto sourceFile            = context->sourceFile;
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);

    auto numParams = assign->childs.size();
    for (int i = 0; i < numParams; i++)
    {
        auto child = assign->childs[0];

        // This happens if the structure has been generated from the assignment expression
        if (child->kind == AstNodeKind::TypeExpression)
            continue;

        auto param = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        if (child->kind == AstNodeKind::Literal)
            param->namedParam = move(child->name);
        Ast::removeFromParent(child);
        Ast::addChildBack(param, child);

        context->node = param;
        SWAG_CHECK(resolveFuncCallParam(context));
    }

    context->node = identifier->callParameters;
    SWAG_CHECK(resolveFuncCallParams(context));
    context->node = savedNode;

    identifier->callParameters->inheritOrFlag(varDecl->assignment, AST_CONST_EXPR);
    identifier->callParameters->flags |= AST_CALL_FOR_STRUCT;
    identifier->flags |= AST_IN_TYPE_VAR_DECLARATION;
    varDecl->type->flags &= ~AST_NO_BYTECODE_CHILDS;
    varDecl->type->flags |= AST_HAS_STRUCT_PARAMETERS;

    Ast::removeFromParent(varDecl->assignment);
    varDecl->assignment = nullptr;

    return true;
}

bool SemanticJob::convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, uint32_t symbolFlags)
{
    auto typeList  = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListArray);
    auto typeArray = TypeManager::convertTypeListToArray(context, typeList, isCompilerConstant);
    node->typeInfo = typeArray;

    // For a global variable, no need to collect in the constant segment, as we will collect directly to the mutable segment
    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, CASTFLAG_NO_COLLECT));
    else
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment));
    node->typeInfo->sizeOf = node->assignment->typeInfo->sizeOf;

    return true;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = static_cast<AstVarDecl*>(context->node);

    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node, SymbolKind::Variable));
    if (context->result == ContextResult::Pending)
        return true;

    // Collect all attributes for the variable
    SymbolAttributes attributes;
    if (node->parentAttributes)
        SWAG_CHECK(collectAttributes(context, attributes, node->parentAttributes, node, AstNodeKind::VarDecl, node->attributeFlags));

    bool isCompilerConstant = node->kind == AstNodeKind::ConstDecl ? true : false;
    bool isLocalConstant    = false;

    uint32_t symbolFlags = 0;
    if (node->kind == AstNodeKind::FuncDeclParam)
        symbolFlags |= OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_CONST_ASSIGN;
    else if (node->ownerScope->isGlobal() || (node->attributeFlags & ATTRIBUTE_GLOBAL))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->isGlobalOrImpl() && (node->flags & AST_INSIDE_IMPL))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->kind == ScopeKind::Struct)
        symbolFlags |= OVERLOAD_VAR_STRUCT;
    else if (!isCompilerConstant)
        symbolFlags |= OVERLOAD_VAR_LOCAL;
    else
        isLocalConstant = true;
    if (node->kind == AstNodeKind::LetDecl)
        symbolFlags |= OVERLOAD_CONST_ASSIGN;

    // Check public
    if (isCompilerConstant && (node->attributeFlags & ATTRIBUTE_PUBLIC))
    {
        if (node->ownerScope->isGlobalOrImpl())
        {
            if (node->type || node->assignment)
            {
                node->ownerScope->addPublicConst(node);
            }
        }
    }

    auto concreteNodeType = node->type && node->type->typeInfo ? TypeManager::concreteType(node->type->typeInfo, CONCRETE_ALIAS) : nullptr;

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->type || concreteNodeType->kind != TypeInfoKind::Struct)
    {
        if (isCompilerConstant && !node->assignment && !(node->flags & AST_VALUE_COMPUTED))
            return context->report({node, "a constant must be initialized"});
        if ((symbolFlags & OVERLOAD_CONST_ASSIGN) && !node->assignment && node->kind != AstNodeKind::FuncDeclParam)
            return context->report({node, "a non mutable 'let' variable must be initialized"});
    }

    // Value
    bool genericType = !node->type && !node->assignment;
    if (node->assignment && node->assignment->kind != AstNodeKind::ExpressionList)
    {
        // A template type with a default value is a generic type
        if ((node->flags & AST_IS_GENERIC) && !node->type && !(node->flags & AST_R_VALUE))
            genericType = true;
        else if (!(node->flags & AST_FROM_GENERIC))
        {
            SWAG_CHECK(checkIsConcreteOrType(context, node->assignment));
            if (context->result == ContextResult::Pending)
                return true;
            if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_FUNC_PARAM) || (node->assignment->flags & AST_CONST_EXPR))
            {
                SWAG_CHECK(evaluateConstExpression(context, node->assignment));
                if (context->result == ContextResult::Pending)
                    return true;
            }

            if (node->type &&
                concreteNodeType->kind != TypeInfoKind::Slice &&
                concreteNodeType->kind != TypeInfoKind::Pointer)
            {
                SWAG_VERIFY(node->assignment->typeInfo->kind != TypeInfoKind::Array, context->report({node->assignment, "affect not allowed from an array"}));
            }
        }
    }

    // A global variable or a constant must have its value computed at that point
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (node->assignment && (isCompilerConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL)))
        {
            SWAG_VERIFY(node->assignment->flags & AST_CONST_EXPR, context->report({node->assignment, "initialization expression cannot be evaluated at compile time"}));
        }
    }

    // Be sure that an array without a size has an initializer, to deduce its size
    if (concreteNodeType && concreteNodeType->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(concreteNodeType, TypeInfoKind::Array);
        SWAG_VERIFY(typeArray->count != UINT32_MAX || node->assignment, context->report({node, "missing initialization expression to deduce size of array"}));
        SWAG_VERIFY(!node->assignment || node->assignment->kind == AstNodeKind::ExpressionList || node->assignment->kind == AstNodeKind::ExplicitNoInit, context->report({node, "invalid initialization expression for an array"}));

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = (uint32_t) node->assignment->childs.size();
            typeArray->totalCount = typeArray->count;
            typeArray->sizeOf     = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name       = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
        }
    }

    if (node->flags & AST_EXPLICITLY_NOT_INITIALIZED)
    {
        SWAG_VERIFY(!isCompilerConstant, context->report({node, "a constant must be explicitly initialized"}));
        SWAG_VERIFY(node->kind != AstNodeKind::LetDecl, context->report({node, "a 'let' declaration must be explicitly initialized"}));
    }

    // Find type
    if (node->type && node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        SWAG_ASSERT(node->type->typeInfo);

        auto      leftConcreteType = node->type->typeInfo;
        TypeInfo* rightConcreteType;
        if (leftConcreteType->kind != TypeInfoKind::Reference)
            rightConcreteType = TypeManager::concreteReferenceType(node->assignment->typeInfo);
        else
            rightConcreteType = TypeManager::concreteType(node->assignment->typeInfo);

        // Do not cast for structs, as we can have special assignment with different types
        // Except if this is an initializer list {...}
        if (leftConcreteType->kind != TypeInfoKind::Struct || rightConcreteType->isInitializerList())
        {
            // Cast from struct to interface : need to wait for all interfaces to be registered
            if (leftConcreteType->kind == TypeInfoKind::Interface && rightConcreteType->kind == TypeInfoKind::Struct)
            {
                context->job->waitForAllStructInterfaces(rightConcreteType);
                if (context->result == ContextResult::Pending)
                    return true;
            }

            SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, CASTFLAG_UNCONST));
        }
        else
        {
            if ((leftConcreteType->kind != rightConcreteType->kind) || !rightConcreteType->isSame(leftConcreteType, ISSAME_CAST))
            {
                if (!hasUserOp(context, "opAffect", node->type))
                {
                    Utf8 msg = format("'%s = %s' is impossible because special function 'opAffect' cannot be found in '%s'", leftConcreteType->name.c_str(), rightConcreteType->name.c_str(), node->type->typeInfo->name.c_str());
                    return context->report({node, msg});
                }

                SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, nullptr, node->type, node->assignment, false));
                if (context->result == ContextResult::Pending)
                    return true;
            }
        }

        node->typeInfo = node->type->typeInfo;

        // A slice initialized with an expression list must be immutable
        if (leftConcreteType->kind == TypeInfoKind::Slice && rightConcreteType->kind == TypeInfoKind::TypeListArray && (node->assignment->flags & AST_CONST_EXPR))
        {
            SWAG_VERIFY(leftConcreteType->isConst(), context->report({node->type, "'slice' must de declared as immutable with 'const'"}));
        }
    }
    else if (node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        node->typeInfo = TypeManager::concreteType(node->assignment->typeInfo, CONCRETE_FUNC);
        SWAG_ASSERT(node->typeInfo);
        SWAG_VERIFY(node->typeInfo != g_TypeMgr.typeInfoVoid, context->report({node->assignment, "type of expression is 'void'"}));

        // We need to decide which integer/float type it is
        node->typeInfo = TypeManager::solidifyUntyped(node->typeInfo);

        // Convert from initialization list to array
        if (node->typeInfo->kind == TypeInfoKind::TypeListArray)
            SWAG_CHECK(convertTypeListToArray(context, node, isCompilerConstant, symbolFlags));
    }
    else if (node->type)
    {
        node->typeInfo = node->type->typeInfo;
    }

    // Slices can't be constant. Use array
    SWAG_VERIFY(!isCompilerConstant || node->typeInfo->kind != TypeInfoKind::Slice, context->report({node, "cannot declare slices as 'const'. Use an array instead"}));

    if (node->type)
        node->inheritOrFlag(node->type, AST_IS_GENERIC);
    if (node->flags & AST_IS_GENERIC)
    {
        symbolFlags |= OVERLOAD_GENERIC;
        if (genericType && node->assignment)
        {
            auto typeGeneric       = g_Allocator.alloc<TypeInfoGeneric>();
            typeGeneric->name      = node->name;
            typeGeneric->nakedName = node->name;
            typeGeneric->rawType   = node->typeInfo;
            node->typeInfo         = typeGeneric;
        }
        else if (!node->typeInfo)
        {
            node->typeInfo            = g_Allocator.alloc<TypeInfoGeneric>();
            node->typeInfo->name      = node->name;
            node->typeInfo->nakedName = node->name;
        }
    }

    if (!node->typeInfo || node->typeInfo == g_TypeMgr.typeInfoUndefined)
    {
        // If this is a lambda parameter in an expression, this is fine, we will try to deduce the type
        if ((node->ownerFct && node->kind == AstNodeKind::FuncDeclParam && (node->ownerFct->flags & AST_IS_LAMBDA_EXPRESSION)) ||
            node->typeInfo == g_TypeMgr.typeInfoUndefined)
        {
            node->typeInfo = g_TypeMgr.typeInfoUndefined;
            genericType    = false;

            // AST_PENDING_LAMBDA_TYPING will stop semantic, forcing to not evaluate the content of the function,
            // until types are known
            node->ownerFct->flags |= AST_PENDING_LAMBDA_TYPING;
        }
    }

    // Type should be a correct one
    SWAG_VERIFY(node->typeInfo != g_TypeMgr.typeInfoNull, context->report({node, node->token, "cannot deduce type from 'null'"}));

    // We should have a type here !
    SWAG_VERIFY(node->typeInfo, context->report({node, node->token, format("unable to deduce type of %s '%s'", AstNode::getNakedKindName(node).c_str(), node->name.c_str())}));

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

        if (typeStruct && identifier->callParameters && identifier->callParameters->childs.size() == typeStruct->fields.size())
            node->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
    }

    // Force a constant to have a constant type, to avoid modifying a type that is in fact stored in the data segment,
    // and has an address
    if (isCompilerConstant && !(node->flags & AST_FROM_GENERIC))
    {
        if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || isLocalConstant)
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
            return context->report({node, node->token, "an enum variable must be initialized"});
        }
    }

    auto     typeInfo      = TypeManager::concreteType(node->typeInfo);
    uint32_t storageOffset = UINT32_MAX;

    if (isCompilerConstant)
    {
        node->flags |= AST_NO_BYTECODE | AST_R_VALUE;

        // A constant does nothing on backend, except if it can't be stored in a ComputedValue struct
        if (typeInfo->kind == TypeInfoKind::Array || typeInfo->kind == TypeInfoKind::Struct)
        {
            // Value already stored in the node
            if (node->flags & AST_VALUE_COMPUTED)
                storageOffset = node->computedValue.reg.offset;
            else
                SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->constantSegment));
        }

        node->inheritComputedValue(node->assignment);
    }
    else if (symbolFlags & OVERLOAD_VAR_GLOBAL)
    {
        SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->report({node, format("cannot instantiate variable because type '%s' is generic", node->typeInfo->name.c_str())}));
        SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PUBLIC), context->report({node, "a global variable cannot be declared as 'public'"}));

        node->flags |= AST_R_VALUE;

        if (node->attributeFlags & AST_EXPLICITLY_NOT_INITIALIZED)
        {
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->mutableSegment));
        }
        else if (node->attributeFlags & ATTRIBUTE_NOBSS)
        {
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->mutableSegment));
        }
        else if (!node->assignment && (typeInfo->kind == TypeInfoKind::Native || typeInfo->kind == TypeInfoKind::Array))
        {
            symbolFlags |= OVERLOAD_VAR_BSS;
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->bssSegment));
        }
        else if (node->assignment && typeInfo->kind == TypeInfoKind::Native && typeInfo->sizeOf <= 8 && node->assignment->isConstant0())
        {
            symbolFlags |= OVERLOAD_VAR_BSS;
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->bssSegment));
        }
        else
        {
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->mutableSegment));
        }

        module->addGlobalVar(node, symbolFlags & OVERLOAD_VAR_BSS);
    }
    else if (symbolFlags & OVERLOAD_VAR_LOCAL)
    {
        // For a struct, need to wait for special functions to be found
        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            SWAG_CHECK(waitForStructUserOps(context, node));
            if (context->result == ContextResult::Pending)
                return true;
        }

        if (node->typeInfo->flags & TYPEINFO_GENERIC)
        {
            SWAG_CHECK(Generic::instantiateDefaultGeneric(context, node));
            if (context->result != ContextResult::Done)
                return true;
        }

        SWAG_ASSERT(node->ownerScope);
        SWAG_ASSERT(node->ownerFct);
        storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, 1); // Be sure we have a stack if a variable is declared, even if sizeof is null (for an empty struct for example)
        node->byteCodeFct         = ByteCodeGenJob::emitLocalVarDecl;
        node->flags |= AST_R_VALUE;
    }
    else if (symbolFlags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->flags |= AST_R_VALUE;

        // A struct/interface is forced to be a const reference
        if (!(node->typeInfo->flags & TYPEINFO_GENERIC))
        {
            if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Interface)
            {
                auto typeRef          = g_Allocator.alloc<TypeInfoReference>();
                typeRef->flags        = typeInfo->flags | TYPEINFO_CONST;
                typeRef->pointedType  = typeInfo;
                typeRef->originalType = node->typeInfo;
                typeRef->computeName();
                node->typeInfo = typeRef;
            }
        }
    }

    // A using on a variable
    if (node->flags & AST_DECL_USING)
    {
        SWAG_CHECK(resolveUsingVar(context, context->node, node->typeInfo));
    }

    // Register symbol with its type
    auto overload = node->ownerScope->symTable.addSymbolTypeInfo(context,
                                                                 node,
                                                                 node->typeInfo,
                                                                 genericType ? SymbolKind::GenericType : SymbolKind::Variable,
                                                                 isCompilerConstant ? &node->computedValue : nullptr,
                                                                 symbolFlags,
                                                                 nullptr,
                                                                 storageOffset);
    SWAG_CHECK(overload);
    node->resolvedSymbolOverload = overload;

    return true;
}

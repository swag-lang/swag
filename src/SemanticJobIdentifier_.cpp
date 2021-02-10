#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Workspace.h"
#include "Generic.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Module.h"

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto node                    = static_cast<AstIdentifierRef*>(context->node);
    auto childBack               = node->childs.back();
    node->resolvedSymbolName     = childBack->resolvedSymbolName;
    node->resolvedSymbolOverload = childBack->resolvedSymbolOverload;
    node->typeInfo               = childBack->typeInfo;
    node->token.text             = childBack->token.text;
    node->byteCodeFct            = ByteCodeGenJob::emitIdentifierRef;

    // Flag inheritance
    node->flags |= AST_CONST_EXPR | AST_FROM_GENERIC_REPLACE;
    for (auto child : node->childs)
    {
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_FROM_GENERIC_REPLACE))
            node->flags &= ~AST_FROM_GENERIC_REPLACE;
        if (child->flags & AST_IS_GENERIC)
            node->flags |= AST_IS_GENERIC;
        if (child->flags & AST_IS_CONST)
            node->flags |= AST_IS_CONST;
    }

    if (childBack->flags & AST_VALUE_COMPUTED)
        node->inheritComputedValue(childBack);
    node->inheritOrFlag(childBack, AST_L_VALUE | AST_R_VALUE | AST_TRANSIENT | AST_VALUE_IS_TYPEINFO);

    if (childBack->flags & AST_IS_CONST_ASSIGN)
        node->flags |= AST_IS_CONST;

    // Symbol is in fact a constant value : no need for bytecode
    if (node->resolvedSymbolOverload && (node->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
    {
        node->computedValue = node->resolvedSymbolOverload->computedValue;
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR | AST_NO_BYTECODE_CHILDS;
        node->flags &= ~AST_L_VALUE;
    }

    return true;
}

bool SemanticJob::setupIdentifierRef(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    // If type of previous one was const, then we force this node to be const (cannot change it)
    if (node->parent->typeInfo && node->parent->typeInfo->isConst())
        node->flags |= AST_IS_CONST;
    if (node->typeInfo->kind == TypeInfoKind::Reference && node->typeInfo->isConst())
        node->flags |= AST_IS_CONST;
    auto overload = node->resolvedSymbolOverload;
    if (overload && overload->flags & OVERLOAD_CONST_ASSIGN)
        node->flags |= AST_IS_CONST_ASSIGN;

    if (node->parent->kind != AstNodeKind::IdentifierRef)
        return true;

    auto identifierRef = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);

    // If we cannot assign previous, and this was AST_IS_CONST_ASSIGN_INHERIT, then we cannot assign
    // this one either
    if (identifierRef->previousResolvedNode && (identifierRef->previousResolvedNode->flags & AST_IS_CONST_ASSIGN_INHERIT))
    {
        node->flags |= AST_IS_CONST_ASSIGN;
        node->flags |= AST_IS_CONST_ASSIGN_INHERIT;
    }

    typeInfo                            = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);
    identifierRef->typeInfo             = typeInfo;
    identifierRef->previousResolvedNode = node;
    identifierRef->startScope           = nullptr;

    // Before making the type concrete
    if (node->typeInfo->kind == TypeInfoKind::Enum)
        identifierRef->startScope = CastTypeInfo<TypeInfoEnum>(node->typeInfo, node->typeInfo->kind)->scope;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Reference:
    {
        auto typeReference = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        auto subType       = TypeManager::concreteReferenceType(typeReference->pointedType);
        if (subType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        else if (subType->kind == TypeInfoKind::Interface)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->itable->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto subType     = TypeManager::concreteReferenceType(typePointer->pointedType);
        if (subType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        else if (subType->kind == TypeInfoKind::Interface)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->itable->scope;

        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::TypeListArray:
    case TypeInfoKind::TypeListTuple:
        identifierRef->startScope = CastTypeInfo<TypeInfoList>(typeInfo, typeInfo->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Interface:
    {
        auto typeStruct           = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
        identifierRef->startScope = typeStruct->itable->scope;
        node->typeInfo            = typeInfo;
        break;
    }

    case TypeInfoKind::Struct:
    case TypeInfoKind::TypeSet:
        identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto subType   = TypeManager::concreteReferenceType(typeArray->finalType);
        if (subType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto subType   = TypeManager::concreteReferenceType(typeSlice->pointedType);
        if (subType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }
    }

    return true;
}

void SemanticJob::sortParameters(AstNode* allParams)
{
    unique_lock lk(allParams->mutex);

    if (!allParams || !(allParams->flags & AST_MUST_SORT_CHILDS))
        return;
    if (allParams->childs.size() <= 1)
        return;

    sort(allParams->childs.begin(), allParams->childs.end(), [](AstNode* n1, AstNode* n2) {
        AstFuncCallParam* p1 = CastAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
        AstFuncCallParam* p2 = CastAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
        return p1->index < p2->index;
    });

    allParams->flags ^= AST_MUST_SORT_CHILDS;
}

void SemanticJob::dealWithIntrinsic(SemanticContext* context, AstIdentifier* identifier)
{
    auto module = context->sourceFile->module;

    switch (identifier->token.id)
    {
    case TokenId::IntrinsicAssert:
    {
        if (module->mustOptimizeBC(context->node))
        {
            // Remove assert(true)
            SWAG_ASSERT(identifier->callParameters && !identifier->callParameters->childs.empty());
            auto param = identifier->callParameters->childs.front();
            if ((param->flags & AST_VALUE_COMPUTED) && param->computedValue.reg.b)
                identifier->flags |= AST_NO_BYTECODE;
        }
        break;
    }
    }
}

void SemanticJob::resolvePendingLambdaTyping(AstFuncCallParam* nodeCall, OneMatch* oneMatch, int i)
{
    auto funcDecl = CastAst<AstFuncDecl>(nodeCall->typeInfo->declNode, AstNodeKind::FuncDecl);
    SWAG_ASSERT(!(funcDecl->flags & AST_IS_GENERIC));
    auto typeUndefinedFct = CastTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    auto concreteType     = TypeManager::concreteReferenceType(oneMatch->solvedParameters[i]->typeInfo);
    auto typeDefinedFct   = CastTypeInfo<TypeInfoFuncAttr>(concreteType, TypeInfoKind::Lambda);

    // Replace every parameters types
    for (int paramIdx = 0; paramIdx < typeUndefinedFct->parameters.size(); paramIdx++)
    {
        auto childType   = funcDecl->parameters->childs[paramIdx];
        auto definedType = typeDefinedFct->parameters[paramIdx]->typeInfo;

        childType->typeInfo                              = definedType;
        childType->resolvedSymbolOverload->typeInfo      = definedType;
        typeUndefinedFct->parameters[paramIdx]->typeInfo = definedType;
    }

    // Replace generic parameters, if any
    for (int paramIdx = 0; paramIdx < typeUndefinedFct->genericParameters.size(); paramIdx++)
    {
        auto undefinedType = typeUndefinedFct->genericParameters[paramIdx];
        auto it            = typeDefinedFct->replaceTypes.find(undefinedType->name);
        if (it != typeDefinedFct->replaceTypes.end())
        {
            undefinedType->name     = it->second->name;
            undefinedType->typeInfo = it->second;
        }
    }

    // Replace every types inside the function
    Ast::visit(funcDecl, [&](AstNode* p) {
        auto it = typeDefinedFct->replaceTypes.find(p->token.text);
        if (it == typeDefinedFct->replaceTypes.end())
            return;
        p->token.text = it->second->name;
        if (p->resolvedSymbolOverload)
            p->resolvedSymbolOverload->typeInfo = it->second;
        p->typeInfo = it->second;
    });

    // Set return type
    if (typeUndefinedFct->returnType->isNative(NativeTypeKind::Undefined))
    {
        typeUndefinedFct->returnType = typeDefinedFct->returnType;
        if (funcDecl->returnType)
            funcDecl->returnType->typeInfo = typeDefinedFct->returnType;
    }

    typeUndefinedFct->computeName();

    // Wake up semantic lambda job
    SWAG_ASSERT(funcDecl->pendingLambdaJob);
    funcDecl->flags &= ~AST_PENDING_LAMBDA_TYPING;
    g_ThreadMgr.addJob(funcDecl->pendingLambdaJob);
}

bool SemanticJob::createTmpVarStruct(SemanticContext* context, AstIdentifier* identifier)
{
    auto sourceFile = context->sourceFile;
    auto callP      = identifier->callParameters;
    identifier->flags |= AST_R_VALUE | AST_GENERATED | AST_NO_BYTECODE;

    // Be sure it's the NAME{} syntax
    if (!(identifier->callParameters->flags & AST_CALL_FOR_STRUCT))
        return context->report({callP, callP->token, format("struct '%s' must be initialized in place with '{}' and not parenthesis (this is reserved for function calls)", identifier->typeInfo->name.c_str())});

    auto varParent = identifier->identifierRef->parent;
    while (varParent->kind == AstNodeKind::ExpressionList)
        varParent = varParent->parent;

    // Declare a variable
    auto varNode = Ast::newVarDecl(sourceFile, format("__tmp_%d", g_Global.uniqueID.fetch_add(1)), varParent);

    // At global scope, this should be a constant declaration, not a variable, as we cannot assign a global variable to
    // another global variable at compile time
    if (identifier->ownerScope->isGlobalOrImpl())
        varNode->kind = AstNodeKind::ConstDecl;

    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
    varNode->flags |= AST_GENERATED;
    varNode->type = typeNode;
    CloneContext cloneContext;
    cloneContext.parent  = typeNode;
    typeNode->identifier = identifier->identifierRef->clone(cloneContext);
    auto back            = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;
    identifier->callParameters = back->callParameters;

    // Call parameters have already been evaluated, so do not reevaluate them again
    identifier->callParameters->flags |= AST_NO_SEMANTIC;

    // If this is in a return expression, then force the identifier type to be retval
    if (context->node->parent && context->node->parent->parent && context->node->parent->parent->kind == AstNodeKind::Return)
        typeNode->typeFlags |= TYPEFLAG_RETVAL;

    // And make a reference to that variable
    auto identifierRef = identifier->identifierRef;
    identifierRef->childs.clear();
    auto idNode = Ast::newIdentifier(sourceFile, varNode->token.text, identifierRef, identifierRef);
    idNode->flags |= AST_R_VALUE | AST_TRANSIENT;

    // Reset parsing
    identifierRef->startScope = nullptr;

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure this will always work.
    varParent->flags |= AST_REVERSE_SEMANTIC;

    // Add the 2 nodes to the semantic
    context->job->nodes.pop_back();
    context->job->nodes.push_back(idNode);
    context->job->nodes.push_back(varNode);
    context->result = ContextResult::NewChilds;

    return true;
}

bool SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, OneMatch& oneMatch)
{
    auto symbol       = oneMatch.symbolOverload->symbol;
    auto overload     = oneMatch.symbolOverload;
    auto dependentVar = oneMatch.dependentVar;
    auto sourceFile   = context->sourceFile;

    // Test x.toto with x not a struct (like a native type for example), but toto is known, so
    // no error was raised before
    if (symbol &&
        symbol->kind == SymbolKind::Variable &&
        overload->typeInfo->kind != TypeInfoKind::Lambda &&
        !parent->startScope &&
        parent->previousResolvedNode &&
        parent->previousResolvedNode->typeInfo->kind != TypeInfoKind::Pointer &&
        parent->previousResolvedNode->typeInfo->kind != TypeInfoKind::Struct)
    {
        return context->report({parent->previousResolvedNode, format("identifier '%s' cannot be dereferenced like a struct (type is '%s')", parent->previousResolvedNode->token.text.c_str(), parent->previousResolvedNode->typeInfo->name.c_str())});
    }

    // Direct reference to a constexpr typeinfo
    if (parent->previousResolvedNode &&
        (parent->previousResolvedNode->flags & AST_VALUE_IS_TYPEINFO) &&
        symbol->kind == SymbolKind::Variable)
    {
        if (derefLiteralStruct(context, parent, overload, &sourceFile->module->typeSegment))
        {
            parent->previousResolvedNode = context->node;
            return true;
        }

        identifier->flags |= AST_R_VALUE;
    }

    // Direct reference to a constexpr structure
    if (parent->previousResolvedNode &&
        (parent->previousResolvedNode->flags & AST_VALUE_COMPUTED) &&
        parent->previousResolvedNode->typeInfo->kind == TypeInfoKind::Struct &&
        symbol->kind == SymbolKind::Variable)
    {
        if (derefLiteralStruct(context, parent, overload, &sourceFile->module->constantSegment))
        {
            parent->previousResolvedNode = context->node;
            return true;
        }

        identifier->flags |= AST_R_VALUE;
    }

    // Direct reference of a struct field inside a const array
    if (parent->previousResolvedNode &&
        parent->previousResolvedNode->kind == AstNodeKind::ArrayPointerIndex &&
        parent->previousResolvedNode->typeInfo->kind == TypeInfoKind::Struct &&
        symbol->kind == SymbolKind::Variable)
    {
        auto arrayNode = CastAst<AstArrayPointerIndex>(parent->previousResolvedNode, AstNodeKind::ArrayPointerIndex);
        if (arrayNode->array->resolvedSymbolOverload && (arrayNode->array->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
        {
            if (arrayNode->access->flags & AST_VALUE_COMPUTED)
            {
                auto typePtr = CastTypeInfo<TypeInfoArray>(arrayNode->array->typeInfo, TypeInfoKind::Array);
                SWAG_ASSERT(arrayNode->array->resolvedSymbolOverload->storageOffset != UINT32_MAX);
                auto ptr = context->sourceFile->module->constantSegment.address(arrayNode->array->resolvedSymbolOverload->storageOffset);
                ptr += arrayNode->access->computedValue.reg.u64 * typePtr->finalType->sizeOf;
                if (derefLiteralStruct(context, ptr, overload, &sourceFile->module->constantSegment))
                {
                    parent->previousResolvedNode = context->node;
                    return true;
                }

                identifier->flags |= AST_R_VALUE;
            }
        }
    }

    // If this a L or R value
    if (!dependentVar && (overload->flags & OVERLOAD_VAR_STRUCT))
    {
        if (symbol->kind != SymbolKind::GenericType)
        {
            if (parent->previousResolvedNode)
            {
                if (parent->previousResolvedNode->flags & AST_R_VALUE)
                {
                    identifier->flags |= AST_L_VALUE | AST_R_VALUE;
                }
                else
                {
                    identifier->flags |= (parent->previousResolvedNode->flags & AST_L_VALUE);
                    identifier->flags |= (parent->previousResolvedNode->flags & AST_R_VALUE);
                }
            }
        }
    }
    else if (symbol->kind == SymbolKind::Variable)
    {
        identifier->flags |= AST_L_VALUE | AST_R_VALUE;
    }

    // Do not register a sub impl scope, for ufcs to use the real variable
    if (!(overload->flags & OVERLOAD_IMPL))
    {
        parent->resolvedSymbolName     = symbol;
        parent->resolvedSymbolOverload = overload;
    }

    identifier->resolvedSymbolName     = symbol;
    identifier->resolvedSymbolOverload = overload;

    if (identifier->typeInfo->flags & TYPEINFO_GENERIC)
        identifier->flags |= AST_IS_GENERIC;

    // Symbol is linked to a using var : insert the variable name before the symbol
    if (dependentVar && parent->kind == AstNodeKind::IdentifierRef && symbol->kind != SymbolKind::Function)
    {
        auto idRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
        if (dependentVar->kind == AstNodeKind::IdentifierRef)
        {
            for (int i = (int) dependentVar->childs.size() - 1; i >= 0; i--)
            {
                auto child  = dependentVar->childs[i];
                auto idNode = Ast::newIdentifier(sourceFile, child->token.text, idRef, nullptr);
                idNode->inheritTokenLocation(idRef->token);
                Ast::addChildFront(idRef, idNode);
                context->job->nodes.push_back(idNode);
                if (i == 0)
                    idNode->flags |= identifier->flags & AST_IDENTIFIER_BACKTICK;
            }
        }
        else
        {
            auto idNode = Ast::newIdentifier(sourceFile, dependentVar->token.text, idRef, nullptr);
            idNode->inheritTokenLocation(identifier->token);

            // We need to insert at the right place, but the identifier 'childParentIdx' can be the wrong one
            // if it's not a direct child of 'idRef'. So we need to find the direct child of 'idRef', which is
            // also a parent if 'identifier', in order to get the right child index, and insert the 'using'
            // just before.
            AstNode* newParent = identifier;
            while (newParent->parent != idRef)
                newParent = newParent->parent;

            idNode->flags |= identifier->flags & AST_IDENTIFIER_BACKTICK;
            Ast::insertChild(idRef, idNode, newParent->childParentIdx);
            context->job->nodes.push_back(idNode);
        }

        context->node->semanticState = AstNodeResolveState::Enter;
        context->result              = ContextResult::NewChilds;
        return true;
    }

    // If this is a typealias, find the right thing
    auto typeAlias  = identifier->typeInfo;
    auto symbolKind = symbol->kind;
    if (symbol->kind == SymbolKind::TypeAlias)
    {
        typeAlias = TypeManager::concreteType(identifier->typeInfo, CONCRETE_ALIAS);
        if (typeAlias->kind == TypeInfoKind::Struct)
            symbolKind = SymbolKind::Struct;
    }

    // Global identifier call. Must be a call to a mixin function, that's it...
    if (identifier->identifierRef && identifier->identifierRef->flags & AST_GLOBAL_IDENTIFIER)
    {
        if (symbolKind != SymbolKind::Function)
            return context->report({identifier, format("only calls to 'swag.mixin' functions can be done at global scope ('%s' is %s)", identifier->token.text.c_str(), SymTable::getArticleKindName(symbolKind))});

        auto funcDecl = CastAst<AstFuncDecl>(identifier->typeInfo->declNode, AstNodeKind::FuncDecl);
        if (!(funcDecl->attributeFlags & ATTRIBUTE_MIXIN))
        {
            Diagnostic diag{identifier, identifier->token, format("function '%s' is not declared with the 'swag.mixin' attribute, and this is mandatory for a function call at global scope", funcDecl->token.text.c_str())};
            Diagnostic note{funcDecl, funcDecl->token, "this is the function", DiagnosticLevel::Note};
            return context->report(diag, &note);
        }
    }

    switch (symbolKind)
    {
    case SymbolKind::GenericType:
        SWAG_CHECK(setupIdentifierRef(context, identifier, identifier->typeInfo));
        break;

    case SymbolKind::Namespace:
        parent->startScope = CastTypeInfo<TypeInfoNamespace>(identifier->typeInfo, identifier->typeInfo->kind)->scope;
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::Enum:
        parent->startScope = CastTypeInfo<TypeInfoEnum>(identifier->typeInfo, identifier->typeInfo->kind)->scope;
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::EnumValue:
        SWAG_CHECK(setupIdentifierRef(context, identifier, TypeManager::concreteType(identifier->typeInfo)));
        identifier->setFlagsValueIsComputed();
        identifier->flags |= AST_R_VALUE;
        identifier->computedValue = identifier->resolvedSymbolOverload->computedValue;
        break;

    case SymbolKind::Struct:
    case SymbolKind::Interface:
    case SymbolKind::TypeSet:
        if (!(overload->flags & OVERLOAD_IMPL))
            SWAG_CHECK(setupIdentifierRef(context, identifier, identifier->typeInfo));
        parent->startScope = CastTypeInfo<TypeInfoStruct>(typeAlias, typeAlias->kind)->scope;
        identifier->flags |= AST_CONST_EXPR;

        // A struct with parameters is in fact the creation of a temporary variable
        if (identifier->callParameters && !(identifier->flags & AST_GENERATED) && !(identifier->flags & AST_IN_TYPE_VAR_DECLARATION))
        {
            SWAG_CHECK(createTmpVarStruct(context, identifier));
            return true;
        }

        // Be sure it's the NAME{} syntax
        if (identifier->callParameters && !(identifier->flags & AST_GENERATED) && !(identifier->callParameters->flags & AST_CALL_FOR_STRUCT))
            return context->report({identifier, identifier->token, format("struct '%s' must be initialized in place with '{}' and not parenthesis (this is reserved for function calls)", identifier->typeInfo->name.c_str())});

        // Need to make all types compatible, in case a cast is necessary
        if (identifier->callParameters)
        {
            sortParameters(identifier->callParameters);
            auto maxParams = identifier->callParameters->childs.size();
            for (int i = 0; i < maxParams; i++)
            {
                auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
                int  idx      = nodeCall->index;
                if (idx < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[idx])
                    SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters[idx]->typeInfo, nullptr, nodeCall, CASTFLAG_COERCE_FULL));
            }
        }

        break;

    case SymbolKind::Variable:
    {
        // Transform the variable to a constant node
        if (overload->flags & OVERLOAD_COMPUTED_VALUE)
        {
            if (overload->node->flags & AST_VALUE_IS_TYPEINFO)
                identifier->flags |= AST_VALUE_IS_TYPEINFO;
            identifier->computedValue = overload->computedValue;
            identifier->setFlagsValueIsComputed();
        }

        // Setup parent if necessary
        auto typeInfo = TypeManager::concreteType(identifier->typeInfo);
        SWAG_CHECK(setupIdentifierRef(context, identifier, typeInfo));

        // Lambda call
        if (typeInfo->kind == TypeInfoKind::Lambda && identifier->callParameters)
        {
            // From now this is considered as a function, not a lambda
            auto funcType           = typeInfo->clone();
            funcType->kind          = TypeInfoKind::FuncAttr;
            identifier->typeInfo    = funcType;
            identifier->byteCodeFct = ByteCodeGenJob::emitLambdaCall;

            // Need to make all types compatible, in case a cast is necessary
            if (identifier->callParameters)
            {
                sortParameters(identifier->callParameters);
                auto maxParams = identifier->callParameters->childs.size();
                for (int i = 0; i < maxParams; i++)
                {
                    auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
                    if (i < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[i])
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters[i]->typeInfo, nullptr, nodeCall));
                    else if (oneMatch.solvedParameters.back() && oneMatch.solvedParameters.back()->typeInfo->kind == TypeInfoKind::TypedVariadic)
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters.back()->typeInfo, nullptr, nodeCall));
                }
            }

            // For a return by copy, need to reserve room on the stack for the return result
            auto returnType = TypeManager::concreteType(identifier->typeInfo);
            if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
            {
                identifier->flags |= AST_TRANSIENT;
                identifier->concreteTypeInfoStorage = identifier->ownerScope->startStackSize;
                identifier->ownerScope->startStackSize += returnType->sizeOf;
                identifier->ownerFct->stackSize = max(identifier->ownerFct->stackSize, identifier->ownerScope->startStackSize);
            }
        }

        break;
    }

    case SymbolKind::Function:
    {
        // Be sure it's () and not {}
        if (identifier->callParameters && (identifier->callParameters->flags & AST_CALL_FOR_STRUCT))
            return context->report({identifier->callParameters, identifier->callParameters->token, format("function '%s' must be called with '()' and not curlies (this is reserved for struct initialization)", identifier->token.text.c_str())});

        // Now we need to be sure that the function is now complete
        // If not, we need to wait for it
        {
            scoped_lock lk(symbol->mutex);
            if (overload->flags & OVERLOAD_INCOMPLETE)
            {
                context->job->waitForSymbolNoLock(symbol);
                return true;
            }
        }

        if (identifier->token.text == "opDrop")
            return context->report({identifier, identifier->token, "cannot reference 'opDrop' special function (use '@drop' instead)"});
        if (identifier->token.text == "opPostCopy")
            return context->report({identifier, identifier->token, "cannot reference 'opPostCopy' special function (use '@postcopy' instead)"});
        if (identifier->token.text == "opPostMove")
            return context->report({identifier, identifier->token, "cannot reference 'opPostMove' special function (use '@postmove' instead)"});

        // Error
        auto funcNode = CastAst<AstFuncDecl>(identifier->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);
        if (funcNode->funcFlags & FUNC_FLAG_RAISE_ERRORS && identifier->parent->parent->kind != AstNodeKind::Try)
        {
            //return context->report({identifier, identifier->token, format("uncatched error when calling function '%s'", identifier->token.text.c_str())});
        }

        // Be sure this is not a 'forward' decl
        if (overload->node->flags & AST_EMPTY_FCT && !(overload->node->attributeFlags & ATTRIBUTE_FOREIGN) && identifier->token.text[0] != '@')
        {
            Diagnostic diag{identifier, identifier->token, format("cannot call empty function '%s'", identifier->token.text.c_str())};
            Diagnostic note{overload->node, overload->node->token, "this is the function", DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        identifier->flags |= AST_L_VALUE | AST_R_VALUE;

        // Need to make all types compatible, in case a cast is necessary
        if (!identifier->ownerFct || !(identifier->ownerFct->flags & AST_IS_GENERIC))
        {
            if (identifier->callParameters)
            {
                sortParameters(identifier->callParameters);
                auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr);
                auto maxParams    = identifier->callParameters->childs.size();
                for (int i = 0; i < maxParams; i++)
                {
                    auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);

                    // This is a lambda that was waiting for a match to have its types, and to continue solving its content
                    if (nodeCall->typeInfo->kind == TypeInfoKind::Lambda && (nodeCall->typeInfo->declNode->flags & AST_PENDING_LAMBDA_TYPING))
                        resolvePendingLambdaTyping(nodeCall, &oneMatch, i);

                    uint32_t castFlags = CASTFLAG_AUTO_OPCAST;
                    if (i == 0 && oneMatch.ufcs)
                        castFlags |= CASTFLAG_UFCS;

                    if (i < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[i])
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters[i]->typeInfo, nullptr, nodeCall, castFlags));
                    else if (oneMatch.solvedParameters.size() && oneMatch.solvedParameters.back() && oneMatch.solvedParameters.back()->typeInfo->kind == TypeInfoKind::TypedVariadic)
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters.back()->typeInfo, nullptr, nodeCall));

                    // For a variadic parameter, we need to generate the concrete typeinfo for the corresponding 'any' type
                    if (i >= typeInfoFunc->parameters.size() - 1 && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
                    {
                        auto& typeTable    = sourceFile->module->typeTable;
                        auto  concreteType = TypeManager::concreteType(nodeCall->typeInfo, CONCRETE_FUNC);
                        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, concreteType, nullptr, &nodeCall->concreteTypeInfoStorage, CONCRETE_ZERO));
                    }
                }
            }
        }

        // Be sure the call is valid
        if ((identifier->token.text[0] != '@') && !(overload->node->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            auto ownerFct = identifier->ownerFct;
            if (ownerFct)
            {
                auto fctAttributes = ownerFct->attributeFlags;

                if (!(fctAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER) && !(identifier->flags & AST_RUN_BLOCK))
                    return context->report({identifier, identifier->token, format("cannot reference 'swag.compiler' function '%s' from '%s'", overload->node->token.text.c_str(), ownerFct->token.text.c_str())});
                if (!(fctAttributes & ATTRIBUTE_TEST_FUNC) && (overload->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
                    return context->report({identifier, identifier->token, format("cannot reference 'swag.test' function '%s' from '%s'", overload->node->token.text.c_str(), ownerFct->token.text.c_str())});
            }
        }

        // This is for a lambda
        if (identifier->forceTakeAddress())
        {
            // The makePointer will deal with the real make lambda thing
            identifier->flags |= AST_NO_BYTECODE;
            break;
        }

        // The function call is constexpr if the function is, and all parameters are
        if (identifier->resolvedSymbolOverload->node->flags & AST_CONST_EXPR)
        {
            if (identifier->callParameters)
                identifier->inheritAndFlag1(identifier->callParameters, AST_CONST_EXPR);
            else
                identifier->flags |= AST_CONST_EXPR;
        }

        auto returnType = TypeManager::concreteType(identifier->typeInfo);
        if (overload->node->mustInline())
        {
            // Expand inline function. Do not expand an inline call inside a function marked as inline.
            // The expansion will be done at the lowest level possible
            if (!identifier->ownerFct || !identifier->ownerFct->mustInline())
            {
                // Need to wait for function full semantic resolve
                auto funcDecl = static_cast<AstFuncDecl*>(overload->node);
                {
                    scoped_lock lk(funcDecl->mutex);
                    if (!(funcDecl->semFlags & AST_SEM_FULL_RESOLVE))
                    {
                        funcDecl->dependentJobs.add(context->job);
                        context->job->setPending(funcDecl->resolvedSymbolName, "AST_SEM_FULL_RESOLVE", funcDecl, nullptr);
                        return true;
                    }
                }

                // First pass, we inline the function.
                // The identifier for the function call will be reresolved later when the content
                // of the inline os done.
                if (!(identifier->doneFlags & AST_DONE_INLINED))
                {
                    identifier->doneFlags |= AST_DONE_INLINED;

                    // In case of an inline call inside an inline function, the identifier kind has been changed to
                    // AstNodeKind::FuncCall in the original function. So we restore it to be a simple identifier.
                    identifier->kind = AstNodeKind::Identifier;

                    SWAG_CHECK(makeInline(context, funcDecl, identifier));
                }
                else
                {
                    SWAG_CHECK(setupIdentifierRef(context, identifier, returnType));
                }

                identifier->byteCodeFct = ByteCodeGenJob::emitPassThrough;
                return true;
            }
        }

        identifier->kind = AstNodeKind::FuncCall;

        if (identifier->token.text[0] == '@')
        {
            dealWithIntrinsic(context, identifier);
            identifier->byteCodeFct = ByteCodeGenJob::emitIntrinsic;
        }
        else if (overload->node->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            identifier->byteCodeFct = ByteCodeGenJob::emitForeignCall;
        }
        else
            identifier->byteCodeFct = ByteCodeGenJob::emitCall;

        // Setup parent if necessary
        if (returnType->kind == TypeInfoKind::Struct)
        {
            identifier->flags |= AST_IS_CONST_ASSIGN_INHERIT;
            identifier->flags |= AST_IS_CONST_ASSIGN;
        }

        SWAG_CHECK(setupIdentifierRef(context, identifier, returnType));

        // For a return by copy, need to reserve room on the stack for the return result
        if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            identifier->flags |= AST_TRANSIENT;
            identifier->concreteTypeInfoStorage = identifier->ownerScope->startStackSize;
            identifier->ownerScope->startStackSize += returnType->sizeOf;
            if (identifier->ownerFct)
                identifier->ownerFct->stackSize = max(identifier->ownerFct->stackSize, identifier->ownerScope->startStackSize);
        }

        break;
    }
    }

    return true;
}

void SemanticJob::setupContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload)
{
    auto node = context->node;

    // Fresh start on generic types
    oneTryMatch.symMatchContext.genericReplaceTypes.clear();
    oneTryMatch.symMatchContext.mapGenericTypesIndex.clear();

    VectorNative<AstNode*> toCheck;

    // If we are inside a struct, then we can inherit the generic concrete types of that struct
    if (node->ownerStructScope)
        toCheck.push_back(node->ownerStructScope->owner);

    // If function A in a struct calls function B in the same struct, then we can inherit the match types of function A
    // when instantiating function B
    if (node->ownerFct && node->ownerStructScope && node->ownerFct->ownerStructScope == symOverload->node->ownerStructScope)
        toCheck.push_back(node->ownerFct);

    if (node->ownerFct && symOverload->typeInfo->kind != TypeInfoKind::FuncAttr)
        toCheck.push_back(node->ownerFct);

    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
        if (identifier->identifierRef->startScope)
            toCheck.push_back(identifier->identifierRef->startScope->owner);
    }

    // Collect from the owner structure
    for (auto one : toCheck)
    {
        if (one->kind == AstNodeKind::FuncDecl)
        {
            auto nodeFunc = CastAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(nodeFunc->typeInfo, TypeInfoKind::FuncAttr);
            for (auto oneReplace : typeFunc->replaceTypes)
            {
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;
            }
        }

        if (one->kind == AstNodeKind::StructDecl)
        {
            auto nodeStruct = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(nodeStruct->typeInfo, TypeInfoKind::Struct);
            for (auto oneReplace : typeStruct->replaceTypes)
            {
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;
            }
        }
    }
}

void SemanticJob::getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, vector<const Diagnostic*>& result0, vector<const Diagnostic*>& result1)
{
    auto              node              = context->node;
    BadSignatureInfos bi                = oneTry.symMatchContext.badSignatureInfos;
    auto              symbol            = oneTry.overload->symbol;
    SymbolOverload*   overload          = oneTry.overload;
    auto              genericParameters = oneTry.genericParameters;
    auto              callParameters    = oneTry.callParameters;
    auto&             match             = oneTry.symMatchContext;
    AstFuncCallParam* failedParam       = nullptr;

    // Get the call parameter that failed
    if (oneTry.callParameters && bi.badSignatureParameterIdx >= 0 && bi.badSignatureParameterIdx < callParameters->childs.size())
        failedParam = static_cast<AstFuncCallParam*>(callParameters->childs[bi.badSignatureParameterIdx]);

    // If the first parameter of the call has been generated by ufcs, then decrease the call parameter index by 1, so
    // that the message is more relevant
    int badParamIdx = bi.badSignatureParameterIdx;
    if (badParamIdx && callParameters && !callParameters->childs.empty() && callParameters->childs.front()->flags & (AST_FROM_UFCS | AST_TO_UFCS))
        badParamIdx--;
    badParamIdx += 1;

    // Nice name to reference it
    Utf8 refNiceName;
    if (overload->typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
        refNiceName = "the tuple";
    else
        refNiceName = format("%s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str());

    // Get parameters of destination symbol
    AstFuncDecl* destFuncDecl = nullptr;
    if (overload->node->kind == AstNodeKind::FuncDecl)
        destFuncDecl = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    switch (match.result)
    {
    default:
        SWAG_ASSERT(false);
        return;

    case MatchResult::SelectIfFailed:
    {
        SWAG_ASSERT(destFuncDecl);
        auto diag = new Diagnostic{node, format("cannot select function '%s' because '#selectif' has failed", destFuncDecl->token.text.c_str())};
        result0.push_back(diag);
        auto note = new Diagnostic{destFuncDecl->selectIf, destFuncDecl->selectIf->token, "this is the '#selectif'", DiagnosticLevel::Note};
        result1.push_back(note);
        return;
    }

    case MatchResult::MissingNamedParameter:
    {
        SWAG_ASSERT(failedParam);
        auto diag = new Diagnostic{failedParam, format("parameter '%d' must be named", badParamIdx)};
        result0.push_back(diag);
        return;
    }

    case MatchResult::InvalidNamedParameter:
    {
        SWAG_ASSERT(failedParam);
        auto diag = new Diagnostic{failedParam->namedParamNode ? failedParam->namedParamNode : failedParam, format("unknown named parameter '%s'", failedParam->namedParam.c_str())};
        auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::DuplicatedNamedParameter:
    {
        SWAG_ASSERT(failedParam);
        auto diag = new Diagnostic{failedParam->namedParamNode ? failedParam->namedParamNode : failedParam, format("named parameter '%s' already used", failedParam->namedParam.c_str())};
        result0.push_back(diag);
        return;
    }

    case MatchResult::NotEnoughParameters:
    {
        auto diag = new Diagnostic{callParameters ? callParameters : node, format("not enough parameters for %s", refNiceName.c_str())};
        auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::MissingParameters:
    {
        auto diag = new Diagnostic{callParameters ? callParameters : node, format("missing function call '()' to %s", refNiceName.c_str())};
        auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::TooManyParameters:
    {
        AstNode* site = failedParam;
        if (!site)
            site = callParameters;
        SWAG_ASSERT(site);
        auto diag = new Diagnostic{site, format("too many parameters for %s", refNiceName.c_str())};
        auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::NotEnoughGenericParameters:
    {
        auto errNode = genericParameters ? genericParameters : node ? node : context->node;
        auto diag    = new Diagnostic{errNode, errNode->token, format("not enough generic parameters for %s", refNiceName.c_str())};
        auto note    = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::TooManyGenericParameters:
    {
        auto errNode = genericParameters ? genericParameters : node ? node : context->node;
        auto diag    = new Diagnostic{errNode, errNode->token, format("too many generic parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
        auto note    = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::BadGenericMatch:
    {
        SWAG_ASSERT(callParameters);
        auto diag = new Diagnostic{match.parameters[bi.badSignatureParameterIdx],
                                   format("bad type of parameter '%d' for %s, generic type '%s' is assigned to '%s' ('%s' provided)",
                                          badParamIdx,
                                          refNiceName.c_str(),
                                          bi.badGenMatch.c_str(),
                                          bi.badSignatureRequestedType->name.c_str(),
                                          bi.badSignatureGivenType->name.c_str())};
        if (TypeManager::makeCompatibles(context, bi.badSignatureRequestedType, bi.badSignatureGivenType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            diag->codeComment = format("'cast(%s)' can be used in that context", bi.badSignatureRequestedType->name.c_str());
        auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::BadSignature:
    {
        SWAG_ASSERT(callParameters);
        Diagnostic* diag = nullptr;
        if (overload->typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
            diag            = new Diagnostic{match.parameters[bi.badSignatureParameterIdx],
                                  format("bad type of parameter '%d' for %s ('%s' is expected for field '%s', '%s' provided)",
                                         badParamIdx,
                                         refNiceName.c_str(),
                                         bi.badSignatureRequestedType->name.c_str(),
                                         typeStruct->fields[badParamIdx - 1]->namedParam.c_str(),
                                         bi.badSignatureGivenType->name.c_str())};
        }
        else
        {
            diag = new Diagnostic{match.parameters[bi.badSignatureParameterIdx],
                                  format("bad type of parameter '%d' for %s ('%s' expected, '%s' provided)",
                                         badParamIdx,
                                         refNiceName.c_str(),
                                         bi.badSignatureRequestedType->name.c_str(),
                                         bi.badSignatureGivenType->name.c_str())};
        }
        if (TypeManager::makeCompatibles(context, bi.badSignatureRequestedType, bi.badSignatureGivenType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            diag->codeComment = format("'cast(%s)' can be used in that context", bi.badSignatureRequestedType->name.c_str());
        Diagnostic* note;
        if (destFuncDecl && bi.badSignatureParameterIdx < destFuncDecl->parameters->childs.size())
            note = new Diagnostic{destFuncDecl->parameters->childs[bi.badSignatureParameterIdx], format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        else
            note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
        result0.push_back(diag);
        result1.push_back(note);
        return;
    }

    case MatchResult::BadGenericSignature:
    {
        SWAG_ASSERT(genericParameters);
        if (match.flags & SymbolMatchContext::MATCH_ERROR_VALUE_TYPE)
        {
            auto diag = new Diagnostic{match.genericParameters[bi.badSignatureParameterIdx],
                                       format("bad generic parameter '%d' for %s (type expected, value provided)",
                                              badParamIdx,
                                              refNiceName.c_str())};
            auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
            result0.push_back(diag);
            result1.push_back(note);
        }
        else if (match.flags & SymbolMatchContext::MATCH_ERROR_TYPE_VALUE)
        {
            auto diag = new Diagnostic{match.genericParameters[bi.badSignatureParameterIdx],
                                       format("bad generic parameter '%d' for %s (value expected, type provided)",
                                              badParamIdx,
                                              refNiceName.c_str())};
            auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
            result0.push_back(diag);
            result1.push_back(note);
        }
        else
        {
            auto diag = new Diagnostic{match.genericParameters[bi.badSignatureParameterIdx],
                                       format("bad type of generic parameter '%d' for %s ('%s' expected, '%s' provided)",
                                              badParamIdx,
                                              refNiceName.c_str(),
                                              bi.badSignatureRequestedType->name.c_str(),
                                              bi.badSignatureGivenType->name.c_str())};
            if (TypeManager::makeCompatibles(context, bi.badSignatureRequestedType, bi.badSignatureGivenType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
                diag->codeComment = format("'cast(%s)' can be used in that context", bi.badSignatureRequestedType->name.c_str());
            auto note = new Diagnostic{overload->node, overload->node->token, format("this is %s", refNiceName.c_str()), DiagnosticLevel::Note};
            result0.push_back(diag);
            result1.push_back(note);
        }

        return;
    }
    }
}

bool SemanticJob::cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node)
{
    AstIdentifier* identifier        = nullptr;
    AstNode*       genericParameters = nullptr;
    AstNode*       callParameters    = nullptr;

    // node can be null when we try to resolve a userOp
    if (node)
    {
        identifier        = CastAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
        genericParameters = identifier->genericParameters;
        callParameters    = identifier->callParameters;
    }
    else
        node = context->node;

    // Take non generic errors in priority
    {
        vector<OneTryMatch*> n;
        for (auto oneMatch : overloads)
        {
            auto& one = *oneMatch;
            switch (one.symMatchContext.result)
            {
            case MatchResult::BadSignature:
            case MatchResult::DuplicatedNamedParameter:
            case MatchResult::InvalidNamedParameter:
            case MatchResult::MissingNamedParameter:
            case MatchResult::MissingParameters:
            case MatchResult::NotEnoughParameters:
            case MatchResult::TooManyParameters:
            case MatchResult::SelectIfFailed:
                n.push_back(oneMatch);
                break;
            }
        }
        if (!n.empty())
            overloads = n;
    }

    // If we do not have generic parameters, then eliminate generic fail
    if (!genericParameters)
    {
        vector<OneTryMatch*> n;
        for (auto oneMatch : overloads)
        {
            auto& one = *oneMatch;
            if (!(one.overload->flags & OVERLOAD_GENERIC))
                n.push_back(oneMatch);
        }
        if (!n.empty())
            overloads = n;
    }

    // If we have generic parameters, then eliminate non generic fail
    if (genericParameters)
    {
        vector<OneTryMatch*> n;
        for (auto oneMatch : overloads)
        {
            auto& one = *oneMatch;
            if (one.overload->flags & OVERLOAD_GENERIC)
                n.push_back(oneMatch);
        }
        if (!n.empty())
            overloads = n;
    }

    // Take bad signature in priority
    {
        vector<OneTryMatch*> n;
        for (auto oneMatch : overloads)
        {
            auto& one = *oneMatch;
            if (one.symMatchContext.result == MatchResult::BadGenericSignature || one.symMatchContext.result == MatchResult::BadSignature)
                n.push_back(oneMatch);
        }
        if (!n.empty())
            overloads = n;
    }

    // One single overload
    if (overloads.size() == 1)
    {
        // Be sure this is not because of an invalid special function signature
        if (overloads[0]->overload->node->kind == AstNodeKind::FuncDecl)
            SWAG_CHECK(checkFuncPrototype(context, CastAst<AstFuncDecl>(overloads[0]->overload->node, AstNodeKind::FuncDecl)));

        vector<const Diagnostic*> errs0, errs1;
        getDiagnosticForMatch(context, *overloads[0], errs0, errs1);
        SWAG_ASSERT(!errs0.empty());
        return context->report(*errs0[0], errs1);
    }

    // Multiple overloads
    Diagnostic                diag{node, node->token, format("none of the %d overloads could match", overloads.size())};
    vector<const Diagnostic*> notes;
    for (auto& one : overloads)
    {
        vector<const Diagnostic*> errs0, errs1;
        getDiagnosticForMatch(context, *one, errs0, errs1);

        // Get the overload site
        if (!errs1.empty())
        {
            const_cast<Diagnostic*>(errs0[0])->sourceFile    = errs1[0]->sourceFile;
            const_cast<Diagnostic*>(errs0[0])->startLocation = errs1[0]->startLocation;
            const_cast<Diagnostic*>(errs0[0])->endLocation   = errs1[0]->startLocation;
        }

        SWAG_ASSERT(!errs0.empty());
        const_cast<Diagnostic*>(errs0[0])->errorLevel = DiagnosticLevel::Note;
        notes.push_back(errs0[0]);
    }

    return context->report(diag, notes);
}

bool SemanticJob::matchIdentifierParameters(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node, bool justCheck)
{
    auto  job              = context->job;
    auto& matches          = job->cacheMatches;
    auto& genericMatches   = job->cacheGenericMatches;
    auto& genericMatchesSI = job->cacheGenericMatchesSI;

    job->clearMatch();
    job->clearGenericMatch();
    job->bestSignatureInfos.clear();

    bool forStruct = false;

    for (auto oneMatch : overloads)
    {
        auto&       oneOverload       = *oneMatch;
        auto        genericParameters = oneOverload.genericParameters;
        auto        callParameters    = oneOverload.callParameters;
        auto        dependentVar      = oneOverload.dependentVar;
        auto        overload          = oneOverload.overload;
        auto        symbol            = overload->symbol;
        scoped_lock lock(symbol->mutex);

        if (oneOverload.symMatchContext.parameters.empty() && oneOverload.symMatchContext.genericParameters.empty())
        {
            auto symbolKind = symbol->kind;

            // For everything except functions/attributes/structs (which have overloads), this is a match
            if (symbolKind != SymbolKind::Attribute &&
                symbolKind != SymbolKind::Function &&
                symbolKind != SymbolKind::Struct &&
                symbolKind != SymbolKind::TypeSet &&
                symbolKind != SymbolKind::Interface &&
                overload->typeInfo->kind != TypeInfoKind::Lambda)
            {
                SWAG_ASSERT(symbol->overloads.size() == 1);

                auto match              = job->getOneMatch();
                match->symbolOverload   = overload;
                match->solvedParameters = move(oneOverload.symMatchContext.solvedParameters);
                match->dependentVar     = dependentVar;
                match->ufcs             = oneOverload.ufcs;
                matches.push_back(match);
                continue;
            }
        }

        if (symbol->kind == SymbolKind::Function && symbol->cptOverloadsInit == symbol->overloads.size())
        {
            // This is enough to resolve
        }
        else if (symbol->cptOverloads)
        {
            job->waitForSymbolNoLock(symbol);
            return true;
        }

        auto rawTypeInfo = overload->typeInfo;

        // If this is a type alias that already has a generic instance, accept to not have generic
        // parameters on the source symbol
        if (rawTypeInfo->kind == TypeInfoKind::Alias)
        {
            rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_ALIAS);
            if (rawTypeInfo->kind == TypeInfoKind::Struct)
            {
                auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                if (!(typeInfo->flags & TYPEINFO_GENERIC))
                    oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC;
            }
        }

        // This way, a special cast can be done for the first parameter of a function
        if (oneOverload.ufcs)
            oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_UFCS;

        // We collect type replacements depending on where the identifier is
        setupContextualGenericTypeReplacement(context, oneOverload, overload);

        oneOverload.symMatchContext.semContext = context;
        if (rawTypeInfo->kind == TypeInfoKind::Struct)
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->kind == TypeInfoKind::Interface)
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Interface);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->kind == TypeInfoKind::TypeSet)
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::TypeSet);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->kind == TypeInfoKind::FuncAttr)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::FuncAttr);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->kind == TypeInfoKind::Lambda)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::Lambda);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else
        {
            SWAG_ASSERT(false);
        }

        if (context->result != ContextResult::Done)
            return true;

        // For a function, sometime, we do not want call parameters
        bool forcedFine = false;
        if (node && node->parent && node->parent->parent && symbol->kind == SymbolKind::Function)
        {
            // Be sure this is not because of a generic error
            if (oneOverload.symMatchContext.result != MatchResult::NotEnoughGenericParameters &&
                oneOverload.symMatchContext.result != MatchResult::TooManyGenericParameters &&
                oneOverload.symMatchContext.result != MatchResult::BadGenericSignature)
            {
                auto grandParent = node->parent->parent;
                if (grandParent->kind == AstNodeKind::MakePointer ||
                    grandParent->kind == AstNodeKind::MakePointerLambda ||
                    grandParent->kind == AstNodeKind::Alias ||
                    (grandParent->kind == AstNodeKind::CompilerSpecialFunction && grandParent->token.id == TokenId::CompilerLocation) ||
                    (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicTypeOf) ||
                    (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicKindOf))
                {
                    if (callParameters)
                        return context->report({callParameters, "cannot take the address of a function with call parameters"});
                    oneOverload.symMatchContext.result = MatchResult::Ok;
                    forcedFine                         = true;
                }
            }
        }

        // We need () for a function call !
        bool emptyParams = oneOverload.symMatchContext.parameters.empty() && !callParameters;
        if (!forcedFine && emptyParams && oneOverload.symMatchContext.result == MatchResult::Ok && symbol->kind == SymbolKind::Function)
        {
            oneOverload.symMatchContext.result = MatchResult::MissingParameters;
        }

        // Function type without call parameters
        if (emptyParams && oneOverload.symMatchContext.result == MatchResult::NotEnoughParameters)
        {
            if (symbol->kind == SymbolKind::Variable)
                oneOverload.symMatchContext.result = MatchResult::Ok;
        }

        // In case of errors, remember the 'best' signature in order to generate the best possible
        // accurate error. We find the longest match (the one that failed on the right most parameter)
        if (oneOverload.symMatchContext.result != MatchResult::Ok)
        {
            if (job->bestSignatureInfos.badSignatureParameterIdx == -1 ||
                oneOverload.symMatchContext.badSignatureInfos.badSignatureParameterIdx > job->bestSignatureInfos.badSignatureParameterIdx ||
                oneOverload.symMatchContext.result == MatchResult::BadGenericMatch && job->bestMatchResult == MatchResult::BadSignature)
            {
                job->bestMatchResult    = oneOverload.symMatchContext.result;
                job->bestSignatureInfos = oneOverload.symMatchContext.badSignatureInfos;
                job->bestOverload       = overload;
            }
        }

        if (oneOverload.symMatchContext.result == MatchResult::Ok)
        {
            if (overload->flags & OVERLOAD_GENERIC)
            {
                auto* match                         = job->getOneGenericMatch();
                match->flags                        = oneOverload.symMatchContext.flags;
                match->symbolName                   = symbol;
                match->symbolOverload               = overload;
                match->genericParametersCallTypes   = move(oneOverload.symMatchContext.genericParametersCallTypes);
                match->genericParametersGenTypes    = move(oneOverload.symMatchContext.genericParametersGenTypes);
                match->genericReplaceTypes          = move(oneOverload.symMatchContext.genericReplaceTypes);
                match->genericParameters            = genericParameters;
                match->matchNumOverloadsWhenChecked = oneOverload.cptOverloads;
                if (overload->node->flags & AST_HAS_SELECT_IF)
                    genericMatchesSI.push_back(match);
                else
                    genericMatches.push_back(match);
            }
            else
            {
                auto match              = job->getOneMatch();
                match->symbolOverload   = overload;
                match->solvedParameters = move(oneOverload.symMatchContext.solvedParameters);
                match->dependentVar     = dependentVar;
                match->ufcs             = oneOverload.ufcs;
                match->oneOverload      = &oneOverload;
                matches.push_back(match);
            }
        }
    }

    auto prevMatchesCount = matches.size();
    SWAG_CHECK(filterMatches(context, matches));
    if (context->result != ContextResult::Done)
        return true;

    // All choices were removed
    if (!genericMatches.size() && genericMatchesSI.size() && matches.empty() && prevMatchesCount)
    {
        if (justCheck)
            return false;
        return cannotMatchIdentifierError(context, overloads, node);
    }

    // Multi instantation in case of #selectif
    if (genericMatchesSI.size() && matches.empty() && !prevMatchesCount)
    {
        if (justCheck)
            return true;

        for (auto& g : genericMatchesSI)
        {
            SWAG_CHECK(Generic::instantiateFunction(context, g->genericParameters, *g, true));
        }

        return true;
    }

    // This is a generic
    if (genericMatches.size() == 1 && matches.empty())
    {
        if (justCheck)
            return true;

        SWAG_CHECK(instantiateGenericSymbol(context, *genericMatches[0], forStruct));
        return true;
    }

    // Done !!!
    if (matches.size() == 1)
        return true;

    // Ambiguity with generics
    if (genericMatches.size() > 1)
    {
        if (justCheck)
            return false;

        auto                      symbol = overloads[0]->overload->symbol;
        Diagnostic                diag{node, node->token, format("ambiguous resolution of generic %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
        vector<const Diagnostic*> notes;
        for (auto match : genericMatches)
        {
            auto overload = match->symbolOverload;
            auto couldBe  = "could be: " + Ast::computeTypeDisplay(overload->node->token.text, overload->typeInfo);
            auto note     = new Diagnostic{overload->node, overload->node->token, couldBe, DiagnosticLevel::Note};

            Utf8 width;
            for (auto og : match->genericReplaceTypes)
            {
                if (!width.empty())
                    width += " and ";
                else
                    width = "widh ";
                width += format("%s = %s", og.first.c_str(), og.second->name.c_str());
            }

            note->codeComment           = width;
            note->showRange             = false;
            note->showMultipleCodeLines = false;
            notes.push_back(note);
        }

        context->report(diag, notes);
        return false;
    }

    // We remove all generated nodes, because if they exist, they do not participate to the
    // error
    auto oneTry = overloads[0];
    for (int i = 0; i < overloads.size(); i++)
    {
        if (overloads[i]->overload->node->flags & AST_FROM_GENERIC)
        {
            overloads[i] = overloads.back();
            overloads.pop_back();
            i--;
        }
    }

    // Be sure to have something. This should raise in case of internal error only, because
    // we must have at least the generic symbol
    if (overloads.empty())
        overloads.push_back(oneTry);

    // There's no match at all
    if (matches.size() == 0)
    {
        if (justCheck)
            return false;
        return cannotMatchIdentifierError(context, overloads, node);
    }

    // There is more than one possible match
    if (matches.size() > 1)
    {
        if (justCheck)
            return false;

        if (!node)
            node = context->node;
        auto                      symbol = overloads[0]->overload->symbol;
        Diagnostic                diag{node, node->token, format("ambiguous resolution of %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
        vector<const Diagnostic*> notes;
        for (auto match : matches)
        {
            auto overload               = match->symbolOverload;
            auto couldBe                = "could be: " + Ast::computeTypeDisplay(overload->node->token.text, overload->typeInfo);
            auto note                   = new Diagnostic{overload->node, overload->node->token, couldBe, DiagnosticLevel::Note};
            note->showRange             = false;
            note->showMultipleCodeLines = false;

            if (overload->typeInfo->kind == TypeInfoKind::FuncAttr)
            {
                auto typeFunc     = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
                note->codeComment = Ast::computeGenericParametersReplacement(typeFunc->genericParameters);
            }
            else if (overload->typeInfo->kind == TypeInfoKind::Struct)
            {
                auto typeStruct   = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
                note->codeComment = Ast::computeGenericParametersReplacement(typeStruct->genericParameters);
            }

            notes.push_back(note);
        }

        context->report(diag, notes);
        return false;
    }

    return true;
}

bool SemanticJob::instantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch, bool forStruct)
{
    auto        job               = context->job;
    auto        node              = context->node;
    auto&       matches           = job->cacheMatches;
    auto        symbol            = firstMatch.symbolName;
    auto        genericParameters = firstMatch.genericParameters;
    unique_lock lk(symbol->mutex);

    // Be sure we don't have more overloads waiting to be solved
    if (symbol->cptOverloads)
    {
        if (context->result == ContextResult::Done)
            job->waitForSymbolNoLock(symbol);
        return true;
    }

    // Be sure number of overloads has not changed since then
    if (firstMatch.matchNumOverloadsWhenChecked != symbol->overloads.size())
    {
        context->result = ContextResult::NewChilds;
        return true;
    }

    if (forStruct)
    {
        // Be sure we have generic parameters if there's an automatic match
        if (!genericParameters && (firstMatch.flags & SymbolMatchContext::MATCH_GENERIC_AUTO))
        {
            SWAG_ASSERT(!firstMatch.genericParametersCallTypes.empty());
            auto identifier               = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
            identifier->genericParameters = Ast::newFuncCallParams(node->sourceFile, node);
            genericParameters             = identifier->genericParameters;
            for (auto param : firstMatch.genericParametersCallTypes)
            {
                auto callParam      = Ast::newFuncCallParam(node->sourceFile, genericParameters);
                callParam->typeInfo = param;
            }
        }

        if (!(node->flags & AST_IS_GENERIC) && genericParameters)
        {
            // If we are inside a #back instruction, then setup
            SWAG_CHECK(Generic::instantiateStruct(context, genericParameters, firstMatch));
        }
        else
        {
            auto oneMatch            = job->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->flags |= AST_IS_GENERIC;
        }
    }
    else
    {
        SWAG_CHECK(Generic::instantiateFunction(context, genericParameters, firstMatch));
    }

    return true;
}

bool SemanticJob::ufcsSetLastParam(SemanticContext* context, AstIdentifierRef* identifierRef, SymbolName* symbol)
{
    auto node = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    if (node->callParameters)
        return true;
    if (node->identifierRef->parent->kind != AstNodeKind::AffectOp)
        return true;
    if (node->identifierRef->parent->token.id != TokenId::SymEqual)
        return true;

    auto rightAffect = node->identifierRef->parent->childs[1];

    auto fctCallParam = Ast::newNode<AstFuncCallParam>(nullptr, AstNodeKind::FuncCallParam, node->sourceFile, nullptr);
    if (!node->callParameters)
        node->callParameters = Ast::newFuncCallParams(context->sourceFile, node);
    node->callParameters->childs.push_back(fctCallParam);
    fctCallParam->parent   = node->callParameters;
    fctCallParam->typeInfo = rightAffect->typeInfo;
    SWAG_ASSERT(fctCallParam->typeInfo);
    fctCallParam->token       = rightAffect->token;
    fctCallParam->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;
    fctCallParam->inheritComputedValue(rightAffect);
    fctCallParam->inheritOwners(node->callParameters);
    Ast::removeFromParent(rightAffect);
    Ast::addChildBack(fctCallParam, rightAffect);

    node->semFlags |= AST_SEM_FORCE_NO_TAKE_ADDRESS;
    node->identifierRef->parent->byteCodeFct = &ByteCodeGenJob::emitPassThrough;
    node->identifierRef->parent->semanticFct = nullptr;

    return true;
}

bool SemanticJob::ufcsSetFirstParam(SemanticContext* context, AstIdentifierRef* identifierRef, OneMatch& match)
{
    auto symbol       = match.symbolOverload->symbol;
    auto dependentVar = match.dependentVar;
    auto node         = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);

    auto fctCallParam = Ast::newNode<AstFuncCallParam>(nullptr, AstNodeKind::FuncCallParam, node->sourceFile, nullptr);
    if (!node->callParameters)
        node->callParameters = Ast::newFuncCallParams(context->sourceFile, node);

    SWAG_CHECK(checkIsConcrete(context, identifierRef->previousResolvedNode));

    node->callParameters->childs.push_front(fctCallParam);
    fctCallParam->parent   = node->callParameters;
    fctCallParam->typeInfo = identifierRef->previousResolvedNode->typeInfo;
    fctCallParam->token    = identifierRef->previousResolvedNode->token;
    fctCallParam->inheritTokenLocation(node->token);
    fctCallParam->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;
    fctCallParam->inheritOwners(node->callParameters);
    fctCallParam->flags |= AST_TO_UFCS;

    SWAG_ASSERT(match.solvedParameters.size() > 0);
    SWAG_ASSERT(match.solvedParameters[0]->index == 0);
    fctCallParam->resolvedParameter = match.solvedParameters[0];

    auto idRef = Ast::newIdentifierRef(node->sourceFile, fctCallParam);
    if (symbol->kind == SymbolKind::Variable)
    {
        // Call from a lambda, on a variable : we need to keep the original variable, and put the UFCS one in its own identifierref
        // Copy all previous references to the one we want to pass as parameter
        // X.Y.call(...) => X.Y.call(X.Y, ...)
        for (auto child : identifierRef->childs)
        {
            auto copyChild = Ast::cloneRaw(child, idRef);
            if (child == identifierRef->previousResolvedNode)
            {
                copyChild->flags |= AST_TO_UFCS;
                break;
            }
        }
    }
    else
    {
        // If ufcs comes from a using var, then we must make a reference to the using var in
        // the first call parameter
        if (dependentVar == identifierRef->previousResolvedNode)
        {
            auto copyChild = Ast::newIdentifier(node->sourceFile, dependentVar->token.text, idRef, idRef);
            copyChild->inheritOwners(fctCallParam);
            copyChild->resolvedSymbolName     = dependentVar->resolvedSymbolOverload->symbol;
            copyChild->resolvedSymbolOverload = dependentVar->resolvedSymbolOverload;
            copyChild->typeInfo               = dependentVar->typeInfo;
            copyChild->byteCodeFct            = ByteCodeGenJob::emitIdentifier;
            copyChild->flags |= AST_TO_UFCS | AST_L_VALUE;
        }
        else
        {
            // Copy all previous references to the one we want to pass as parameter
            // X.Y.call(...) => X.Y.call(X.Y, ...)
            // We copy instead of moving in case this will be evaluated another time (inline)
            for (auto child : identifierRef->childs)
            {
                auto copyChild = Ast::cloneRaw(child, idRef);
                child->flags |= AST_NO_BYTECODE;
                if (child == identifierRef->previousResolvedNode)
                {
                    copyChild->flags |= AST_TO_UFCS;
                    break;
                }
            }
        }

        Ast::visit(idRef, [&](AstNode* n) {
            if (n->kind == AstNodeKind::Identifier || n->kind == AstNodeKind::FuncCall)
                ((AstIdentifier*) n)->identifierRef = idRef;
        });
    }

    identifierRef->previousResolvedNode->flags |= AST_FROM_UFCS;
    return true;
}

bool SemanticJob::findIdentifierInScopes(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    auto  job              = context->job;
    auto& dependentSymbols = job->cacheDependentSymbols;

    // When this is "retval" type, no need to do fancy things, we take the corresponding function
    // return symbol. This will avoid some ambiguous resolutions with multiple tuples/structs.
    if (node->token.text == "retval")
    {
        // Be sure this is correct
        SWAG_CHECK(resolveRetVal(context));
        auto fctDecl = node->ownerInline ? node->ownerInline->func : node->ownerFct;
        SWAG_ASSERT(fctDecl);
        auto typeFct = CastTypeInfo<TypeInfoFuncAttr>(fctDecl->typeInfo, TypeInfoKind::FuncAttr);
        SWAG_ASSERT(typeFct->returnType->kind == TypeInfoKind::Struct);
        dependentSymbols.push_back(typeFct->returnType->declNode->resolvedSymbolName);
        return true;
    }

    auto& scopeHierarchy     = job->cacheScopeHierarchy;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;

    // We make 2 tries at max : one try with the previous symbol scope (A.B), and one try with the collected scope
    // hierarchy. We need this because even if A.B does not resolve (B is not in A), B(A) can be a match because of UFCS
    for (int oneTry = 0; oneTry < 2; oneTry++)
    {
        auto startScope = identifierRef->startScope;
        if (!startScope || oneTry == 1)
        {
            uint32_t collectFlags = COLLECT_ALL;

            // Pass through the first inline if there's a back tick before the name
            if (node->flags & AST_IDENTIFIER_BACKTICK)
                collectFlags = COLLECT_BACKTICK;

            startScope = node->ownerScope;
            SWAG_CHECK(collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, collectFlags));

            // Be sure this is the last try
            oneTry++;
        }
        else
        {
            scopeHierarchy.insert(startScope);

            // Only deal with previous scope if the previous node wants to
            bool addAlternative = true;
            if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->semFlags & AST_SEM_FORCE_SCOPE)
                addAlternative = false;

            if (addAlternative)
            {
                // Add private scope
                auto it = startScope->privateScopes.find(context->sourceFile);
                if (it != startScope->privateScopes.end())
                    scopeHierarchy.insert(it->second);

                // A namespace scope can in fact be shared between multiple nodes, so the 'owner' is not
                // relevant and we should not use it
                if (startScope->kind != ScopeKind::Namespace && startScope->owner->extension)
                {
                    for (auto s : startScope->owner->extension->alternativeScopes)
                        scopeHierarchy.insert(s);
                    scopeHierarchyVars.append(startScope->owner->extension->alternativeScopesVars);
                }
            }
        }

        // Search symbol in all the scopes of the hierarchy
        auto crc = node->token.text.hash();
        for (auto scope : scopeHierarchy)
        {
            auto symbol = scope->symTable.find(node->token.text, crc);
            if (symbol)
                dependentSymbols.insert(symbol);
        }

        if (!dependentSymbols.empty())
            break;

        // We raise an error if we have tried to resolve with the normal scope hierarchy, and not just the scope
        // from the previous symbol
        if (oneTry || !identifierRef->startScope)
        {
            if (identifierRef->flags & AST_SILENT_CHECK)
                return true;

            if (identifierRef->startScope)
            {
                if (identifierRef->typeInfo && identifierRef->typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
                    return context->report({node, node->token, format("identifier '%s' cannot be found in tuple", node->token.text.c_str())});
                Utf8 displayName;
                if (!(identifierRef->startScope->flags & SCOPE_PRIVATE))
                    displayName = identifierRef->startScope->getFullName();
                if (displayName.empty() && identifierRef->typeInfo)
                    displayName = identifierRef->typeInfo->name;
                if (!displayName.empty())
                    return context->report({node, node->token, format("identifier '%s' cannot be found in %s '%s'", node->token.text.c_str(), Scope::getNakedKindName(identifierRef->startScope->kind), displayName.c_str())});
            }

            return context->report({node, node->token, format("unknown identifier '%s'", node->token.text.c_str())});
        }

        node->flags |= AST_FORCE_UFCS;
    }

    return true;
}

bool SemanticJob::getUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** result)
{
    auto     job                = context->job;
    auto     symbol             = overload->symbol;
    auto&    scopeHierarchyVars = job->cacheScopeHierarchyVars;
    AstNode* dependentVar       = nullptr;

    auto symScope = symbol->ownerTable->scope;
    for (auto& dep : scopeHierarchyVars)
    {
        bool getIt = false;

        // Exact smae scope
        if (dep.scope->getFullName() == symScope->getFullName())
            getIt = true;

        // From the normal scope, use something in the private scope
        else if (symScope->parentScope && dep.scope->getFullName() == symScope->parentScope->getFullName() && symScope->flags & SCOPE_ROOT_PRIVATE)
            getIt = true;

        if (getIt)
        {
            if (dependentVar)
            {
                Diagnostic diag{dep.node, "cannot use 'using' on two variables with the same type"};
                Diagnostic note{dependentVar, "this is the other definition", DiagnosticLevel::Note};
                return context->report(diag, &note);
            }

            dependentVar = dep.node;

            // This way the ufcs can trigger even with an implicit 'using' var (typically for a 'using self')
            if (!identifierRef->previousResolvedNode)
            {
                if (symbol->kind == SymbolKind::Function)
                {
                    // Be sure we have a missing parameter in order to try ufcs
                    auto typeFunc  = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
                    auto numParams = node->callParameters ? node->callParameters->childs.size() : 0;
                    if (numParams < typeFunc->parameters.size())
                    {
                        identifierRef->resolvedSymbolOverload = dependentVar->resolvedSymbolOverload;
                        identifierRef->resolvedSymbolName     = dependentVar->resolvedSymbolOverload->symbol;
                        identifierRef->previousResolvedNode   = dependentVar;
                    }
                }
            }
        }
    }

    *result = dependentVar;
    return true;
}

bool SemanticJob::getUfcs(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** ufcsFirstParam, AstNode** ufcsLastParam)
{
    auto symbol = overload->symbol;

    bool canDoUfcs = false;
    if (symbol->kind == SymbolKind::Function)
        canDoUfcs = true;
    if (symbol->kind == SymbolKind::Variable && overload->typeInfo->kind == TypeInfoKind::Lambda)
        canDoUfcs = node->callParameters;

    if (canDoUfcs)
    {
        // If a variable is defined just before a function call, then this can be an UFCS (unified function call system)
        if (identifierRef->resolvedSymbolName)
        {
            if (identifierRef->resolvedSymbolName->kind == SymbolKind::Variable ||
                identifierRef->resolvedSymbolName->kind == SymbolKind::EnumValue)
            {
                SWAG_ASSERT(identifierRef->previousResolvedNode);

                // Be sure the identifier we want to use in ufcs emits code, otherwise we cannot use it.
                // This can happen if we have already changed the ast, but the nodes are reavaluated later
                // (because of an inline for example)
                if (identifierRef->previousResolvedNode->flags & AST_NO_BYTECODE)
                    return true;

                auto typeFunc  = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
                auto numParams = node->callParameters ? node->callParameters->childs.size() : 0;

                if (typeFunc->parameters.size())
                {
                    // If we have missing parameters, we will try ufcs
                    if (numParams < typeFunc->parameters.size())
                        *ufcsFirstParam = identifierRef->previousResolvedNode;

                    // In case of variadic functions, make ufcs if the first parameter is correct
                    else if (typeFunc->flags & TYPEINFO_VARIADIC)
                    {
                        bool cmp = TypeManager::makeCompatibles(context, typeFunc->parameters[0]->typeInfo, identifierRef->previousResolvedNode->typeInfo, nullptr, identifierRef->previousResolvedNode, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR | CASTFLAG_UFCS);
                        if (cmp)
                            *ufcsFirstParam = identifierRef->previousResolvedNode;
                    }
                }

                // If we do not have parenthesis (call parameters), then this must be a function marked with 'swag.property'
                if (!node->callParameters)
                {
                    SWAG_VERIFY(symbol->kind == SymbolKind::Function, context->report({node, "missing function call parameters"}));
                    SWAG_VERIFY(symbol->overloads.size() <= 2, context->report({node, "too many overloads for a property (only one set and one get should exist)"}));
                    SWAG_VERIFY(symbol->overloads.front()->node->attributeFlags & ATTRIBUTE_PROPERTY, context->report({node, format("missing function call parameters because symbol '%s' is not marked as 'swag.property'", symbol->name.c_str())}));

                    // Affectation of a property is in fact a function call, with the last parameter being the right sight of the affectation
                    if (node->identifierRef->parent->kind == AstNodeKind::AffectOp &&
                        node->identifierRef->parent->token.id == TokenId::SymEqual)
                    {
                        *ufcsLastParam = node->identifierRef->parent->childs[1];
                    }
                }
            }
        }
    }

    // We want to force the ufcs
    if (!*ufcsFirstParam && !*ufcsLastParam && (node->flags & AST_FORCE_UFCS))
    {
        if (identifierRef->startScope)
        {
            auto displayName = identifierRef->startScope->getFullName();
            if (identifierRef->startScope->name.empty() && identifierRef->typeInfo)
                displayName = identifierRef->typeInfo->name;
            if (!displayName.empty())
                return context->report({node, node->token, format("identifier '%s' cannot be found in %s '%s'", node->token.text.c_str(), Scope::getNakedKindName(identifierRef->startScope->kind), displayName.c_str())});
        }

        return context->report({node, node->token, format("unknown identifier '%s'", node->token.text.c_str())});
    }

    if (canDoUfcs && (symbol->kind == SymbolKind::Variable))
    {
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind == SymbolKind::Struct)
            return context->report({node, node->token, format("invalid lambda call, cannot reference structure member '%s'", symbol->name.c_str())});
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind != SymbolKind::Variable)
            return context->report({node, format("invalid lambda call, '%s' is not a variable", identifierRef->resolvedSymbolName->name.c_str())});
    }

    return true;
}

bool SemanticJob::appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, SymbolOverload* overload)
{
    auto symbol = overload->symbol;
    if (!(node->doneFlags & AST_DONE_LAST_PARAM_CODE) && (symbol->kind == SymbolKind::Function))
    {
        node->doneFlags |= AST_DONE_LAST_PARAM_CODE;

        // If last parameter is of type code, and the call last parameter is not, then take the next statement
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeFunc->parameters.empty() && typeFunc->parameters.back()->typeInfo->kind == TypeInfoKind::Code)
        {
            if (node->callParameters && node->callParameters->childs.size() < typeFunc->parameters.size())
            {
                if (node->parent->childParentIdx != node->parent->parent->childs.size() - 1)
                {
                    auto brother = node->parent->parent->childs[node->parent->childParentIdx + 1];
                    if (brother->kind == AstNodeKind::Statement)
                    {
                        auto fctCallParam = Ast::newFuncCallParam(context->sourceFile, node->callParameters);
                        auto codeNode     = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerCode, node->sourceFile, fctCallParam);
                        codeNode->flags |= AST_NO_BYTECODE;
                        Ast::removeFromParent(brother);
                        Ast::addChildBack(codeNode, brother);
                        auto typeCode     = allocType<TypeInfoCode>();
                        typeCode->content = brother;
                        brother->flags |= AST_NO_SEMANTIC;
                        fctCallParam->typeInfo = typeCode;
                        codeNode->typeInfo     = typeCode;
                    }
                }
            }
        }
    }

    return true;
}

bool SemanticJob::fillMatchContextCallParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload, AstNode* ufcsFirstParam, AstNode* ufcsLastParam)
{
    auto symbol         = overload->symbol;
    auto symbolKind     = symbol->kind;
    auto callParameters = node->callParameters;

    if (ufcsFirstParam)
        symMatchContext.parameters.push_back(ufcsFirstParam);

    if (callParameters || ufcsFirstParam || ufcsLastParam)
    {
        if (symbolKind != SymbolKind::Attribute &&
            symbolKind != SymbolKind::Function &&
            symbolKind != SymbolKind::Struct &&
            symbolKind != SymbolKind::Interface &&
            symbolKind != SymbolKind::TypeAlias &&
            TypeManager::concreteType(symbol->overloads[0]->typeInfo, CONCRETE_ALIAS)->kind != TypeInfoKind::Lambda)
        {
            if (symbolKind == SymbolKind::Variable)
            {
                Diagnostic diag{node, node->token, format("identifier '%s' has call parameters, but is a variable of type '%s' and not a function", node->token.text.c_str(), symbol->overloads[0]->typeInfo->name.c_str())};
                Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is '%s'", node->token.text.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            else
            {
                Diagnostic diag{node, node->token, format("identifier '%s' has call parameters, but is %s and not a function", node->token.text.c_str(), SymTable::getArticleKindName(symbol->kind))};
                Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is '%s'", node->token.text.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
        }
    }

    if (callParameters)
    {
        auto childCount = callParameters->childs.size();
        for (int i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(callParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatchContext.parameters.push_back(oneParam);

            // Be sure all interfaces of the structure has been solved, in case a cast to an interface is necessary to match
            // a function
            if (oneParam->typeInfo->kind == TypeInfoKind::Struct || oneParam->typeInfo->isPointerTo(TypeInfoKind::Struct))
            {
                context->job->waitForAllStructInterfaces(oneParam->typeInfo);
                if (context->result == ContextResult::Pending)
                    return true;
            }

            // Variadic parameter must be the last one
            if (i != childCount - 1)
            {
                if (oneParam->typeInfo->kind == TypeInfoKind::Variadic || oneParam->typeInfo->kind == TypeInfoKind::TypedVariadic)
                {
                    return context->report({oneParam, "variadic argument must be the last one"});
                }
            }
        }
    }

    if (ufcsLastParam)
        symMatchContext.parameters.push_back(ufcsLastParam);

    return true;
}

bool SemanticJob::fillMatchContextGenericParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload)
{
    auto symbol            = overload->symbol;
    auto symbolKind        = symbol->kind;
    auto callParameters    = node->callParameters;
    auto genericParameters = node->genericParameters;

    if (!genericParameters)
        return true;

    node->inheritOrFlag(genericParameters, AST_IS_GENERIC);
    if (symbolKind != SymbolKind::Function &&
        symbolKind != SymbolKind::Struct &&
        symbolKind != SymbolKind::Interface &&
        symbolKind != SymbolKind::TypeAlias)
    {
        Diagnostic diag{callParameters, callParameters->token, format("invalid generic parameters, identifier '%s' is %s and not a function or a structure", node->token.text.c_str(), SymTable::getArticleKindName(symbol->kind))};
        Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is '%s'", node->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    auto childCount = genericParameters->childs.size();
    for (int i = 0; i < childCount; i++)
    {
        auto oneParam = CastAst<AstFuncCallParam>(genericParameters->childs[i], AstNodeKind::FuncCallParam, AstNodeKind::IdentifierRef);
        symMatchContext.genericParameters.push_back(oneParam);
        symMatchContext.genericParametersCallTypes.push_back(oneParam->typeInfo);
    }

    return true;
}

bool SemanticJob::filterMatches(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    auto node = context->node;
    for (int i = 0; i < matches.size(); i++)
    {
        auto over    = matches[i]->symbolOverload;
        auto overSym = over->symbol;

        // Take care of #selectif
        if (overSym->kind == SymbolKind::Function &&
            !(context->node->flags & AST_IN_SELECTIF) &&
            !(context->node->attributeFlags & ATTRIBUTE_SELECTIF_OFF))
        {
            auto funcDecl = CastAst<AstFuncDecl>(over->node, AstNodeKind::FuncDecl);
            if (funcDecl->selectIf)
            {
                // Be sure selectIf block has been solved
                {
                    scoped_lock lk(funcDecl->mutex);
                    if (!(funcDecl->semFlags & AST_SEM_PARTIAL_RESOLVE))
                    {
                        funcDecl->dependentJobs.add(context->job);
                        context->job->setPending(funcDecl->resolvedSymbolName, "AST_SEM_PARTIAL_RESOLVE", funcDecl, nullptr);
                        return true;
                    }
                }

                // Execute #selectif block
                auto expr = funcDecl->selectIf->childs.back();

                expr->flags &= ~AST_VALUE_COMPUTED;
                context->selectIfParameters = matches[i]->oneOverload->callParameters;
                context->expansionNode.push_back({context->selectIfParameters->parent, JobContext::ExpansionType::SelectIf});

                auto result = executeNode(context, expr, true);

                context->selectIfParameters = nullptr;
                context->expansionNode.pop_back();

                if (!result)
                    return false;
                if (context->result != ContextResult::Done)
                    return true;

                // Result
                if (!expr->computedValue.reg.b)
                {
                    matches[i]->remove                              = true;
                    matches[i]->oneOverload->symMatchContext.result = MatchResult::SelectIfFailed;
                    continue;
                }
                else if (funcDecl->content && funcDecl->content->flags & AST_NO_SEMANTIC)
                {
                    scoped_lock lk(funcDecl->mutex);
                    funcDecl->content->flags &= ~AST_NO_SEMANTIC;

                    // Need to restart semantic on instantiated function, because the #selectif has passed
                    auto job                = g_Pool_semanticJob.alloc();
                    job->sourceFile         = context->sourceFile;
                    job->module             = context->sourceFile->module;
                    job->dependentJob       = context->job->dependentJob;
                    funcDecl->semanticState = AstNodeResolveState::ProcessingChilds;
                    job->nodes.push_back(funcDecl);
                    job->nodes.push_back(funcDecl->content);
                    g_ThreadMgr.addJob(job);
                }
            }
        }

        if (matches.size() == 1)
            return true;

        // In case of an alias, we take the first one, which should be the 'closest' one.
        // Not sure this is true, perhaps one day will have to change the way we find it.
        if (strstr(overSym->name.c_str(), "@alias") == overSym->name.c_str())
        {
            for (int j = 0; j < matches.size(); j++)
            {
                if (i != j)
                    matches[j]->remove = true;
            }

            break;
        }

        // Priority to a non empty function
        if (over->node->flags & AST_EMPTY_FCT)
        {
            for (int j = 0; j < matches.size(); j++)
            {
                if (!(matches[j]->symbolOverload->node->flags & AST_EMPTY_FCT) &&
                    matches[j]->symbolOverload->symbol == matches[i]->symbolOverload->symbol)
                {
                    matches[i]->remove = true;
                    break;
                }
            }
        }

        // Priority to a local var/parameter versus a function
        if (over->typeInfo->kind == TypeInfoKind::FuncAttr)
        {
            for (int j = 0; j < matches.size(); j++)
            {
                if (matches[j]->symbolOverload->flags & (OVERLOAD_VAR_LOCAL | OVERLOAD_VAR_FUNC_PARAM))
                {
                    matches[i]->remove = true;
                    break;
                }
            }
        }

        // Priority to a concrete type versus a generic one
        auto lastOverloadType = overSym->ownerTable->scope->owner->typeInfo;
        if (lastOverloadType && lastOverloadType->flags & TYPEINFO_GENERIC)
        {
            for (int j = 0; j < matches.size(); j++)
            {
                auto newOverloadType = matches[j]->symbolOverload->symbol->ownerTable->scope->owner->typeInfo;
                if (newOverloadType && !(newOverloadType->flags & TYPEINFO_GENERIC))
                {
                    matches[i]->remove = true;
                    break;
                }
            }
        }

        // Priority to a non IMPL symbol
        if (over->flags & OVERLOAD_IMPL)
        {
            for (int j = 0; j < matches.size(); j++)
            {
                if (!(matches[j]->symbolOverload->flags & OVERLOAD_IMPL))
                {
                    matches[i]->remove = true;
                    break;
                }
            }
        }

        // Priority to a user generic parameters, instead of a copy one
        if (over->node->flags & AST_GENERATED_GENERIC_PARAM)
        {
            for (int j = 0; j < matches.size(); j++)
            {
                if (!(matches[j]->symbolOverload->node->flags & AST_GENERATED_GENERIC_PARAM))
                {
                    matches[i]->remove = true;
                    break;
                }
            }
        }

        // Priority to the same stack frame
        if (!node->isSameStackFrame(over))
        {
            for (int j = 0; j < matches.size(); j++)
            {
                if (node->isSameStackFrame(matches[j]->symbolOverload))
                {
                    matches[i]->remove = true;
                    break;
                }
            }
        }

        // Priority to the same inline scope
        if (node->ownerInline)
        {
            if (!node->ownerInline->scope->isParentOf(over->node->ownerScope))
            {
                for (int j = 0; j < matches.size(); j++)
                {
                    if (node->ownerInline->scope->isParentOf(matches[j]->symbolOverload->node->ownerScope))
                    {
                        matches[i]->remove = true;
                        break;
                    }
                }
            }
        }

        // If we didn't match with ufcs, then priority to a match that do not start with 'self'
        if (!matches[i]->ufcs && over->typeInfo->kind == TypeInfoKind::FuncAttr)
        {
            auto typeFunc0 = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeFunc0->parameters.empty() && typeFunc0->parameters[0]->typeInfo->flags & TYPEINFO_SELF)
            {
                for (int j = 0; j < matches.size(); j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->kind == TypeInfoKind::FuncAttr)
                    {
                        auto typeFunc1 = CastTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (typeFunc1->parameters.empty() || !(typeFunc1->parameters[0]->typeInfo->flags & TYPEINFO_SELF))
                        {
                            matches[i]->remove = true;
                            break;
                        }
                    }
                }
            }
        }

        // If we did match with ufcs, then priority to a match that starts with 'self'
        if (matches[i]->ufcs && over->typeInfo->kind == TypeInfoKind::FuncAttr)
        {
            auto typeFunc0 = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc0->parameters.empty() || !(typeFunc0->parameters[0]->typeInfo->flags & TYPEINFO_SELF))
            {
                for (int j = 0; j < matches.size(); j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->kind == TypeInfoKind::FuncAttr)
                    {
                        auto typeFunc1 = CastTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (!typeFunc1->parameters.empty() && (typeFunc1->parameters[0]->typeInfo->flags & TYPEINFO_SELF))
                        {
                            matches[i]->remove = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Eliminate all matches tag as 'remove'
    for (int i = 0; i < matches.size(); i++)
    {
        if (matches[i]->remove)
        {
            matches[i] = matches.back();
            matches.pop_back();
            i--;
        }
    }

    return true;
}

bool SemanticJob::filterSymbols(SemanticContext* context, AstIdentifier* node)
{
    auto  job              = context->job;
    auto  identifierRef    = node->identifierRef;
    auto& dependentSymbols = job->cacheDependentSymbols;

    if (dependentSymbols.size() == 1)
        return true;

    auto& toAddSymbol = job->cacheToAddSymbols;
    toAddSymbol.clear();
    for (auto oneSymbol : dependentSymbols)
    {
        if (node->callParameters && oneSymbol->kind == SymbolKind::Variable)
            continue;
        if (!node->callParameters && oneSymbol->kind == SymbolKind::Function)
            continue;

        // Reference to a variable inside a struct, without a direct explicit reference
        bool isValid = true;
        if (oneSymbol->kind != SymbolKind::Function &&
            oneSymbol->kind != SymbolKind::GenericType &&
            oneSymbol->kind != SymbolKind::Struct &&
            oneSymbol->kind != SymbolKind::Enum &&
            oneSymbol->kind != SymbolKind::TypeSet &&
            (oneSymbol->overloads.size() != 1 || !(oneSymbol->overloads[0]->flags & OVERLOAD_COMPUTED_VALUE)) &&
            oneSymbol->ownerTable->scope->kind == ScopeKind::Struct &&
            !identifierRef->startScope)
        {
            isValid                  = false;
            auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;
            for (auto& dep : scopeHierarchyVars)
            {
                if (dep.scope->getFullName() == oneSymbol->ownerTable->scope->getFullName())
                {
                    isValid = true;
                    break;
                }
            }
        }

        if (!isValid)
            continue;

        toAddSymbol.insert(oneSymbol);
    }

    // Register back all valid symbols
    dependentSymbols.clear();
    for (auto s : toAddSymbol)
        dependentSymbols.insert(s);

    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto  node               = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    auto  job                = context->job;
    auto& scopeHierarchy     = job->cacheScopeHierarchy;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;
    auto& dependentSymbols   = job->cacheDependentSymbols;
    auto  identifierRef      = node->identifierRef;

    node->byteCodeFct = ByteCodeGenJob::emitIdentifier;

    // Current private scope
    if (context->sourceFile && context->sourceFile->scopePrivate && node->token.text == context->sourceFile->scopePrivate->name)
    {
        SWAG_VERIFY(node == identifierRef->childs.front(), context->report({node, node->token, "invalid reference to private scope"}));
        identifierRef->startScope = context->sourceFile->scopePrivate;
        return true;
    }

    // If previous identifier was generic, then stop evaluation
    if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->typeInfo->kind == TypeInfoKind::Generic)
    {
        // Just take the generic type for now
        node->typeInfo          = g_TypeMgr.typeInfoUndefined;
        identifierRef->typeInfo = identifierRef->previousResolvedNode->typeInfo;
        return true;
    }

    // Already solved
    if ((node->flags & AST_FROM_GENERIC) && node->typeInfo && !node->typeInfo->isNative(NativeTypeKind::Undefined))
    {
        if (node->resolvedSymbolOverload)
        {
            OneMatch oneMatch;
            oneMatch.symbolOverload = node->resolvedSymbolOverload;
            SWAG_CHECK(setSymbolMatch(context, identifierRef, node, oneMatch));
        }

        return true;
    }

    // Compute dependencies if not already done
    if (node->semanticState == AstNodeResolveState::ProcessingChilds)
    {
        scopeHierarchy.clear();
        scopeHierarchyVars.clear();
        dependentSymbols.clear();
    }

    if (dependentSymbols.empty())
    {
        SWAG_CHECK(findIdentifierInScopes(context, identifierRef, node));
        if (dependentSymbols.empty())
        {
            SWAG_ASSERT(identifierRef->flags & AST_SILENT_CHECK);
            return true;
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto symbol : dependentSymbols)
    {
        scoped_lock lkn(symbol->mutex);
        if (!symbol->cptOverloads)
            continue;

        // This is enough to resolve, as we just need parameters, and that case means that some functions
        // do not know their return type yet (short lambdas)
        if (symbol->kind == SymbolKind::Function && symbol->overloads.size() == symbol->cptOverloadsInit)
            continue;

        // If a structure is referencing itself, we will match the incomplete symbol for now
        bool newToWait = true;
        if ((symbol->kind == SymbolKind::Struct || symbol->kind == SymbolKind::Interface || symbol->kind == SymbolKind::TypeSet) &&
            node->ownerMainNode &&
            (node->ownerMainNode->kind != AstNodeKind::Impl || (node->flags & AST_CAN_MATCH_INCOMPLETE)) &&
            node->ownerMainNode->token.text == symbol->name)
        {
            SWAG_VERIFY(!node->callParameters, context->report({node->callParameters, "cannot auto reference a struct with parameters"}));
            SWAG_VERIFY(!node->genericParameters, context->report({node->genericParameters, "cannot auto reference a struct with generic parameters"}));
            if (symbol->overloads.size() == 1 && (symbol->overloads[0]->flags & OVERLOAD_INCOMPLETE))
            {
                newToWait                    = false;
                node->resolvedSymbolName     = symbol;
                node->resolvedSymbolOverload = symbol->overloads[0];
                node->typeInfo               = node->resolvedSymbolOverload->typeInfo;
            }
        }

        if (newToWait)
            job->waitForSymbolNoLock(symbol);
        return true;
    }

    // Filter symbols
    SWAG_CHECK(filterSymbols(context, node));
    if (dependentSymbols.empty())
        return context->report({node, node->token, format("cannot resolve identifier '%s'", node->token.text.c_str())});

    auto orgResolvedSymbolOverload = identifierRef->resolvedSymbolOverload;
    auto orgResolvedSymbolName     = identifierRef->resolvedSymbolName;
    auto orgPreviousResolvedNode   = identifierRef->previousResolvedNode;

    while (true)
    {
        // Collect all overloads to solve. We collect also the number of overloads when the collect is done, in
        // case that number changes (other thread) during the resolution. Because if the number of overloads differs
        // at one point in the process (for a given symbol), then this will invalidate the resolution
        // (number of overloads can change when instantiating a generic)
        auto& toSolveOverload = job->cacheToSolveOverload;
        toSolveOverload.clear();
        for (auto symbol : dependentSymbols)
        {
            unique_lock lk(symbol->mutex);
            for (auto over : symbol->overloads)
            {
                OneOverload t;
                t.overload     = over;
                t.cptOverloads = (uint32_t) symbol->overloads.size();
                toSolveOverload.push_back(t);
            }
        }

        // Can happen if a symbol is inside a disabled #if for example
        if (toSolveOverload.empty())
            return context->report({node, node->token, format("cannot resolve identifier '%s'", node->token.text.c_str())});

        auto& listTryMatch = job->cacheListTryMatch;
        job->clearTryMatch();

        for (auto& oneOver : toSolveOverload)
        {
            auto symbolOverload                   = oneOver.overload;
            identifierRef->resolvedSymbolOverload = orgResolvedSymbolOverload;
            identifierRef->resolvedSymbolName     = orgResolvedSymbolName;
            identifierRef->previousResolvedNode   = orgPreviousResolvedNode;

            // Is there a using variable associated with the symbol to solve ?
            AstNode* dependentVar = nullptr;
            SWAG_CHECK(getUsingVar(context, identifierRef, node, symbolOverload, &dependentVar));

            // Get the ufcs first parameter, and last parameter, if we can
            AstNode* ufcsFirstParam = nullptr;
            AstNode* ufcsLastParam  = nullptr;
            SWAG_CHECK(getUfcs(context, identifierRef, node, symbolOverload, &ufcsFirstParam, &ufcsLastParam));

            // If the last parameter of a function is of type code, and the call last parameter is not,
            // then we take the next statement, after the function, and put it as the last parameter
            SWAG_CHECK(appendLastCodeStatement(context, node, symbolOverload));

            auto  tryMatch        = job->getTryMatch();
            auto& symMatchContext = tryMatch->symMatchContext;

            tryMatch->genericParameters = node->genericParameters;
            tryMatch->callParameters    = node->callParameters;
            tryMatch->dependentVar      = dependentVar;
            tryMatch->overload          = symbolOverload;
            tryMatch->ufcs              = ufcsFirstParam || ufcsLastParam;
            tryMatch->cptOverloads      = oneOver.cptOverloads;

            SWAG_CHECK(fillMatchContextCallParameters(context, symMatchContext, node, symbolOverload, ufcsFirstParam, ufcsLastParam));
            if (context->result == ContextResult::Pending)
                return true;
            SWAG_CHECK(fillMatchContextGenericParameters(context, symMatchContext, node, symbolOverload));

            if ((node->forceTakeAddress()) && !ufcsLastParam)
                symMatchContext.flags |= SymbolMatchContext::MATCH_FOR_LAMBDA;

            listTryMatch.push_back(tryMatch);
        }

        SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, node));
        if (context->result == ContextResult::Pending)
            return true;

        // The number of overloads for a given symbol has been changed by another thread, which
        // means that we need to restart everything...
        if (context->result != ContextResult::NewChilds)
            break;
        context->result = ContextResult::Done;
    }

    if (job->cacheMatches.empty())
        return false;

    // Deal with ufcs. Now that the match is done, we will change the ast in order to
    // add the ufcs parameters to the function call parameters
    auto& match = job->cacheMatches[0];
    if (match->ufcs)
    {
        // Do not change AST if this is code inside a generic function
        if (!node->ownerFct || !(node->ownerFct->flags & AST_IS_GENERIC))
        {
            if (match->dependentVar)
            {
                identifierRef->resolvedSymbolOverload = match->dependentVar->resolvedSymbolOverload;
                identifierRef->resolvedSymbolName     = match->dependentVar->resolvedSymbolOverload->symbol;
                identifierRef->previousResolvedNode   = match->dependentVar;
            }

            SWAG_CHECK(ufcsSetLastParam(context, identifierRef, match->symbolOverload->symbol));
            SWAG_CHECK(ufcsSetFirstParam(context, identifierRef, *match));
        }
    }

    node->typeInfo = match->symbolOverload->typeInfo;
    SWAG_CHECK(setSymbolMatch(context, identifierRef, node, *match));
    return true;
}

void SemanticJob::collectAlternativeScopeHierarchy(SemanticContext* context, VectorNative<Scope*>& scopes, VectorNative<AlternativeScope>& scopesVars, AstNode* startNode, uint32_t flags)
{
    if (startNode->extension && !startNode->extension->alternativeScopes.empty())
    {
        auto  job  = context->job;
        auto& here = job->scopesHere;

        {
            for (auto p : startNode->extension->alternativeScopes)
            {
                if (!scopes.contains(p))
                {
                    scopes.push_back(p);
                    here.push_back(p);
                }
            }

            scopesVars.append(startNode->extension->alternativeScopesVars);
        }
    }

    // An inline block contains a specific scope that contains the parameters.
    // That scope does not have a parent, so the hierarchy scan will stop at it.
    if (startNode->kind == AstNodeKind::Inline)
    {
        auto inlineNode = CastAst<AstInline>(startNode, AstNodeKind::Inline);
        SWAG_ASSERT(inlineNode->parametersScope);
        scopes.push_back(inlineNode->parametersScope);
    }

    if (startNode->kind == AstNodeKind::CompilerMacro)
    {
        if (!(flags & COLLECT_BACKTICK))
        {
            while (startNode->kind != AstNodeKind::Inline && startNode->kind != AstNodeKind::CompilerInline)
                startNode = startNode->parent;
        }

        flags &= ~COLLECT_BACKTICK;
    }

    // If we are in an inline block, jump right to the function parent
    else if (startNode->kind == AstNodeKind::CompilerInline)
    {
        if (!(flags & COLLECT_BACKTICK))
        {
            while (startNode && startNode->kind != AstNodeKind::FuncDecl)
                startNode = startNode->parent;
        }

        flags &= ~COLLECT_BACKTICK;
    }

    // If we are in an inline block, jump right to the function parent
    // Not that the function parent can be null in case of inlined expression in a global for example (compile time execution)
    else if (startNode->kind == AstNodeKind::Inline)
    {
        auto inlineBlock = CastAst<AstInline>(startNode, AstNodeKind::Inline);
        if (!(inlineBlock->func->attributeFlags & ATTRIBUTE_MIXIN))
        {
            if (!(flags & COLLECT_BACKTICK))
            {
                while (startNode && startNode->kind != AstNodeKind::FuncDecl)
                    startNode = startNode->parent;
            }

            flags &= ~COLLECT_BACKTICK;
        }
    }

    if (startNode && startNode->parent)
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode->parent, flags);
}

bool SemanticJob::collectScopeHierarchy(SemanticContext* context, VectorNative<Scope*>& scopes, VectorNative<AlternativeScope>& scopesVars, AstNode* startNode, uint32_t flags)
{
    auto  job        = context->job;
    auto& here       = job->scopesHere;
    auto  sourceFile = context->sourceFile;

    here.clear();
    scopes.clear();

    // Get alternative scopes from the node hierarchy
    collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode, flags);

    auto startScope = startNode->ownerScope;
    if (startScope)
    {
        // For a backtick, do not collect scope until we find an inline block
        if (flags & COLLECT_BACKTICK)
        {
            while (startScope && startScope->kind != ScopeKind::Inline && startScope->kind != ScopeKind::Macro)
                startScope = startScope->parentScope;
            SWAG_VERIFY(startScope, context->report({context->node, "backtick can only be used inside an inline/macro block"}));
            startScope = startScope->parentScope;
            flags &= ~COLLECT_BACKTICK;
        }

        scopes.insert(startScope);
        here.push_back(startScope);
    }

    // Add current file private scope
    scopes.insert(context->sourceFile->scopePrivate);
    here.push_back(context->sourceFile->scopePrivate);

    // Add bootstrap
    SWAG_ASSERT(g_Workspace.bootstrapModule);
    scopes.insert(g_Workspace.bootstrapModule->scopeRoot);
    here.push_back(g_Workspace.bootstrapModule->scopeRoot);

    // Add runtime, except for the bootstrap
    if (!sourceFile->isBootstrapFile)
    {
        SWAG_ASSERT(g_Workspace.runtimeModule);
        scopes.insert(g_Workspace.runtimeModule->scopeRoot);
        here.push_back(g_Workspace.runtimeModule->scopeRoot);
    }

    for (int i = 0; i < here.size(); i++)
    {
        auto scope = here[i];

        // Add private scope
        auto it = scope->privateScopes.find(context->sourceFile);
        if (it != scope->privateScopes.end())
        {
            scopes.insert(it->second);
            here.push_back(it->second);
        }

        // For an inline scope, jump right to the function
        if (scope->kind == ScopeKind::Inline)
        {
            while (scope && scope->kind != ScopeKind::Function)
                scope = scope->parentScope;
            if (!scope)
                continue;
        }

        // For a macro scope, jump right to the inline
        else if (scope->kind == ScopeKind::Macro)
        {
            while (scope && scope->parentScope->kind != ScopeKind::Inline)
                scope = scope->parentScope;
            if (!scope)
                continue;
            scope = scope->parentScope;
        }

        // Add parent scope
        if (scope->parentScope)
        {
            if (scope->parentScope->kind == ScopeKind::Struct && (flags & COLLECT_NO_STRUCT))
                continue;

            if (!scopes.contains(scope->parentScope))
            {
                scopes.push_back(scope->parentScope);
                here.push_back(scope->parentScope);
            }
        }
    }

    SWAG_VERIFY(!(flags & COLLECT_BACKTICK), context->report({startNode, startNode->token, "back ticked identifier can only be used inside a 'swag.macro' function or an '#inline' block"}));
    return true;
}

bool SemanticJob::checkSymbolGhosting(SemanticContext* context, AstNode* node, SymbolKind kind)
{
    auto job = context->job;

    // No ghosting from struct
    auto startScope = node->ownerScope;
    if (startScope->kind == ScopeKind::Struct)
        return true;

    // Ghosting is authorized for @alias, that way we can have a visit inside a visit even if the
    // alias name has not been redefined by the user
    if (strstr(node->token.text.c_str(), "@alias") == node->token.text.c_str())
        return true;

    // Zap structures. Seems to work to relax the test, as if the definition is done multiple
    // times, it will be catch anyway
    uint32_t collectFlags = COLLECT_NO_STRUCT;

    job->cacheScopeHierarchy.clear();
    job->cacheScopeHierarchyVars.clear();

    collectScopeHierarchy(context, job->cacheScopeHierarchy, job->cacheScopeHierarchyVars, node, collectFlags);

    auto crc = node->token.text.hash();
    for (auto scope : job->cacheScopeHierarchy)
    {
        // Do not check if this is the same scope
        if (scope == startScope)
            continue;

        // Be sure that symbol is fully resolved, otherwise we cannot check for a ghosting
        {
            auto symbol = scope->symTable.find(node->token.text, crc);
            if (!symbol)
                continue;

            scoped_lock lock(symbol->mutex);
            if (symbol->cptOverloadsInit != symbol->overloads.size())
            {
                if (symbol->cptOverloads)
                {
                    job->waitForSymbolNoLock(symbol);
                    return true;
                }
            }

            // If symbol is not in the same stackframe, we ignore the ghosting, as the priority will be given to the
            // one in the same stack frame (if any). And if there's no symbol in the same stack frame, then error will be
            // raised later during bytecode generation
            if (!node->isSameStackFrame(symbol->overloads[0]))
                continue;
        }

        SWAG_CHECK(scope->symTable.checkHiddenSymbol(context, node, node->typeInfo, kind));
    }

    return true;
}

bool SemanticJob::resolveTry(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatch>(context->node, AstNodeKind::Try);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_VERIFY(lastChild->resolvedSymbolName->kind == SymbolKind::Function, context->report({node, format("'try' can only be used after a function call, and '%s' is %s", lastChild->token.text.c_str(), SymTable::getArticleKindName(lastChild->resolvedSymbolName->kind))}));

    auto funcNode = CastAst<AstFuncDecl>(lastChild->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);
    SWAG_VERIFY(funcNode->funcFlags & FUNC_FLAG_RAISE_ERRORS, context->report({node, format("'try' can only be used after a function call that can raise errors, and '%s' does not", lastChild->token.text.c_str())}));

    node->byteCodeFct = ByteCodeGenJob::emitTry;
    node->typeInfo    = lastChild->typeInfo;
    return true;
}

bool SemanticJob::resolveCatch(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatch>(context->node, AstNodeKind::Catch);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_VERIFY(lastChild->resolvedSymbolName->kind == SymbolKind::Function, context->report({node, format("'catch' can only be used after a function call, and '%s' is %s", lastChild->token.text.c_str(), SymTable::getArticleKindName(lastChild->resolvedSymbolName->kind))}));

    auto funcNode = CastAst<AstFuncDecl>(lastChild->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);
    SWAG_VERIFY(funcNode->funcFlags & FUNC_FLAG_RAISE_ERRORS, context->report({node, format("'catch' can only be used after a function call that can raise errors, and '%s' does not", lastChild->token.text.c_str())}));

    node->byteCodeFct = ByteCodeGenJob::emitCatch;
    node->typeInfo    = lastChild->typeInfo;

    auto stmt = node->childs.back();
    stmt->allocateExtension();
    stmt->extension->byteCodeBeforeFct = ByteCodeGenJob::emitCatchBeforeStmt;

    return true;
}
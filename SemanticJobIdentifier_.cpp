#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Workspace.h"
#include "Generic.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto node                    = static_cast<AstIdentifierRef*>(context->node);
    auto childBack               = node->childs.back();
    node->resolvedSymbolName     = childBack->resolvedSymbolName;
    node->resolvedSymbolOverload = childBack->resolvedSymbolOverload;
    node->typeInfo               = childBack->typeInfo;
    node->name                   = childBack->name;
    node->byteCodeFct            = ByteCodeGenJob::emitIdentifierRef;

    // Flag inheritance
    node->flags |= AST_CONST_EXPR;
    for (auto child : node->childs)
    {
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (child->flags & AST_IS_GENERIC)
            node->flags |= AST_IS_GENERIC;
        if (child->flags & AST_IS_CONST)
            node->flags |= AST_IS_CONST;
        if (child->flags & AST_GENERIC_MATCH_WAS_PARTIAL)
            node->flags |= AST_GENERIC_MATCH_WAS_PARTIAL;
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

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Reference:
    {
        auto typeReference = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        if (typeReference->pointedType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = static_cast<TypeInfoStruct*>(typeReference->pointedType)->scope;
        else if (typeReference->pointedType->kind == TypeInfoKind::Interface)
            identifierRef->startScope = static_cast<TypeInfoStruct*>(typeReference->pointedType)->itable->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        if (typePointer->ptrCount == 1)
        {
            if (typePointer->finalType->kind == TypeInfoKind::Struct)
                identifierRef->startScope = static_cast<TypeInfoStruct*>(typePointer->finalType)->scope;
            else if (typePointer->finalType->kind == TypeInfoKind::Interface)
                identifierRef->startScope = static_cast<TypeInfoStruct*>(typePointer->finalType)->itable->scope;
        }

        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::TypeList:
        identifierRef->startScope = static_cast<TypeInfoList*>(typeInfo)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Interface:
    {
        auto typeStruct           = static_cast<TypeInfoStruct*>(typeInfo);
        identifierRef->startScope = typeStruct->itable->scope;
        node->typeInfo            = typeInfo;
        break;
    }

    case TypeInfoKind::Struct:
        identifierRef->startScope = static_cast<TypeInfoStruct*>(typeInfo)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArray->finalType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = static_cast<TypeInfoStruct*>(typeArray->finalType)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        if (typeSlice->pointedType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = static_cast<TypeInfoStruct*>(typeSlice->pointedType)->scope;
        node->typeInfo = typeInfo;
        break;
    }
    }

    return true;
}

bool SemanticJob::derefTypeInfo(SemanticContext* context, AstIdentifierRef* parent, SymbolOverload* overload)
{
    auto sourceFile = context->sourceFile;
    auto node       = context->node;

    uint8_t* ptr = sourceFile->module->constantSegment.address(parent->previousResolvedNode->computedValue.reg.u32);
    ptr += overload->storageOffset;

    auto concreteType = TypeManager::concreteType(overload->typeInfo);
    if (concreteType->kind == TypeInfoKind::Native)
    {
        switch (concreteType->nativeType)
        {
        case NativeTypeKind::String:
            node->computedValue.text = *(const char**) ptr;
            break;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue.reg.u8 = *(uint8_t*) ptr;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue.reg.u16 = *(uint16_t*) ptr;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::F32:
        case NativeTypeKind::Char:
            node->computedValue.reg.u32 = *(uint32_t*) ptr;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::F64:
            node->computedValue.reg.u64 = *(uint64_t*) ptr;
            break;
        case NativeTypeKind::Bool:
            node->computedValue.reg.b = *(bool*) ptr;
            break;
        default:
            return internalError(context, "derefTypeInfo, invalid type", node);
        }
    }
    else
    {
        return internalError(context, "derefTypeInfo, invalid type", node);
    }

    node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
    return true;
}

bool SemanticJob::makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    CloneContext cloneContext;

    // The content will be inline in its separated syntax block
    auto inlineNode                   = Ast::newInline(context->sourceFile, identifier);
    inlineNode->attributeFlags        = funcDecl->attributeFlags;
    inlineNode->func                  = funcDecl;
    inlineNode->scope                 = identifier->ownerScope;
    inlineNode->alternativeScopes     = funcDecl->alternativeScopes;
    inlineNode->alternativeScopesVars = funcDecl->alternativeScopesVars;

    // We need to add the parent scope of the inline function (the global one), in order for
    // the inline content to be resolved in the same context as the original function
    auto globalScope = funcDecl->ownerScope;
    while (!globalScope->isGlobal())
        globalScope = globalScope->parentScope;
    inlineNode->alternativeScopes.push_back(globalScope);

    // If function has generic parameters, then the block resolution of identifiers needs to be able to find the generic parameters
    // so we register all those generic parameters in a special scope (we cannot just register the scope of the function because
    // they are other stuff here)
    if (funcDecl->genericParameters)
    {
        Scope* scope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);
        inlineNode->alternativeScopes.push_back(scope);
        for (auto child : funcDecl->genericParameters->childs)
        {
            auto symName = scope->symTable.registerSymbolNameNoLock(context, child, SymbolKind::Variable);
            symName->addOverloadNoLock(child, child->typeInfo, &child->resolvedSymbolOverload->computedValue);
            symName->cptOverloads = 0; // Simulate a done resolution
        }
    }

    Scope* newScope = identifier->ownerScope;
    if (!(funcDecl->attributeFlags & ATTRIBUTE_MIXIN))
    {
        newScope          = Ast::newScope(inlineNode, format("__inline%d", identifier->ownerScope->childScopes.size()), ScopeKind::Inline, identifier->ownerScope);
        inlineNode->scope = newScope;
    }

    // Clone the function body
    cloneContext.parent         = inlineNode;
    cloneContext.ownerInline    = inlineNode;
    cloneContext.ownerFct       = identifier->ownerFct;
    cloneContext.ownerBreakable = identifier->ownerBreakable;
    cloneContext.parentScope    = newScope;

    // Register all aliases
    if (identifier->kind == AstNodeKind::Identifier)
    {
        auto id  = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
        int  idx = 0;
        for (const auto& alias : id->aliasNames)
        {
            cloneContext.replaceNames[format("@alias%d", idx++)] = alias;
        }
    }

    cloneContext.forceFlags |= identifier->flags & AST_NO_BACKEND;
    cloneContext.forceFlags |= identifier->flags & AST_RUN_BLOCK;

    auto newContent               = funcDecl->content->clone(cloneContext);
    newContent->byteCodeBeforeFct = nullptr;
    newContent->flags &= ~AST_NO_SEMANTIC;

    identifier->semanticState = AstNodeResolveState::Enter;
    identifier->bytecodeState = AstNodeResolveState::Enter;
    return true;
}

bool SemanticJob::makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(makeInline((JobContext*) context, funcDecl, identifier));
    context->result = ContextResult::NewChilds;
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

bool SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, SymbolName* symbol, SymbolOverload* overload, OneMatch* oneMatch, AstNode* dependentVar)
{
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
        return context->report({parent->previousResolvedNode, format("identifier '%s' cannot be dereferenced like a struct (type is '%s')", parent->previousResolvedNode->name.c_str(), parent->previousResolvedNode->typeInfo->name.c_str())});
    }

    // Direct reference to a constexpr typeinfo
    if (parent->previousResolvedNode &&
        parent->previousResolvedNode->flags & AST_VALUE_IS_TYPEINFO &&
        symbol->kind == SymbolKind::Variable)
    {
        SWAG_CHECK(derefTypeInfo(context, parent, overload));
        return true;
    }

    // Can access a private member of a struct only from the same scope (impl or struct)
    if ((overload->flags & OVERLOAD_VAR_STRUCT) && (overload->node->ownerScope != identifier->ownerStructScope))
    {
        if (overload->attributeFlags & ATTRIBUTE_INTERNAL)
            return context->report({identifier, identifier->token, format("member '%s' of structure '%s' is 'internal' and cannot be accessed", overload->node->name.c_str(), overload->node->ownerScope->owner->name.c_str())});
        if ((overload->attributeFlags & ATTRIBUTE_READONLY) && (identifier->flags & AST_TAKE_ADDRESS) && !(overload->attributeFlags & ATTRIBUTE_READWRITE))
            return context->report({identifier, identifier->token, format("member '%s' of structure '%s' is 'readonly'", overload->node->name.c_str(), overload->node->ownerScope->owner->name.c_str())});
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

    auto sourceFile                    = context->sourceFile;
    parent->resolvedSymbolName         = symbol;
    parent->resolvedSymbolOverload     = overload;
    identifier->resolvedSymbolName     = symbol;
    identifier->resolvedSymbolOverload = overload;

    if (identifier->typeInfo->flags & TYPEINFO_GENERIC)
        identifier->flags |= AST_IS_GENERIC;

    // Symbol is linked to a using var : insert the variable name before the symbol
    if (dependentVar && !parent->typeInfo && parent->kind == AstNodeKind::IdentifierRef && symbol->kind != SymbolKind::Function)
    {
        auto idRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
        if (dependentVar->kind == AstNodeKind::IdentifierRef)
        {
            for (int i = (int) dependentVar->childs.size() - 1; i >= 0; i--)
            {
                auto child  = dependentVar->childs[i];
                auto idNode = Ast::newIdentifier(sourceFile, child->name, idRef, nullptr);
                idNode->inheritTokenLocation(child->token);
                Ast::addChildFront(idRef, idNode);
                context->job->nodes.push_back(idNode);

                // Determine if the added identifier is out scope, and must be backticked to be retrieved in the
                // following semantic pass
                if (idNode->ownerInline &&
                    (idNode->ownerInline->attributeFlags & ATTRIBUTE_INLINE | ATTRIBUTE_MACRO) &&
                    !(idNode->ownerInline->attributeFlags & ATTRIBUTE_MIXIN) &&
                    (idNode->ownerInline != dependentVar->ownerInline))
                {
                    idNode->flags |= AST_IDENTIFIER_BACKTICK;
                }
            }
        }
        else
        {
            auto idNode = Ast::newIdentifier(sourceFile, dependentVar->name, idRef, nullptr);
            idNode->inheritTokenLocation(identifier->token);
            Ast::addChildFront(idRef, idNode);
            context->job->nodes.push_back(idNode);

            // Determine if the added identifier is out scope, and must be backticked to be retrieved in the
            // following semantic pass
            if (idNode->ownerInline &&
                (idNode->ownerInline->attributeFlags & ATTRIBUTE_INLINE | ATTRIBUTE_MACRO) &&
                !(idNode->ownerInline->attributeFlags & ATTRIBUTE_MIXIN) &&
                (idNode->ownerInline != dependentVar->ownerInline))
            {
                idNode->flags |= AST_IDENTIFIER_BACKTICK;
            }
        }

        context->node->semanticState = AstNodeResolveState::Enter;
        context->result              = ContextResult::NewChilds;
        return true;
    }

    switch (symbol->kind)
    {
    case SymbolKind::Namespace:
        parent->startScope = static_cast<TypeInfoNamespace*>(identifier->typeInfo)->scope;
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::Enum:
        parent->startScope = static_cast<TypeInfoEnum*>(identifier->typeInfo)->scope;
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::EnumValue:
        identifier->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE;
        identifier->computedValue = identifier->resolvedSymbolOverload->computedValue;
        break;

    case SymbolKind::Struct:
    case SymbolKind::Interface:
        parent->startScope = static_cast<TypeInfoStruct*>(identifier->typeInfo)->scope;
        identifier->flags |= AST_CONST_EXPR;

        // A struct with parameters is in fact the creation of a temporary local variable
        if (identifier->callParameters && !(identifier->flags & AST_GENERATED) && !(identifier->flags & AST_IN_TYPE_VAR_DECLARATION))
        {
            identifier->flags |= AST_R_VALUE | AST_GENERATED | AST_NO_BYTECODE;

            auto varParent = identifier->identifierRef->parent;
            while (varParent->kind == AstNodeKind::ExpressionList)
                varParent = varParent->parent;

            // Declare a variable
            auto varNode  = Ast::newVarDecl(sourceFile, format("__tmp_%d", g_Global.uniqueID.fetch_add(1)), varParent);
            auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
            typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
            varNode->type        = typeNode;
            typeNode->identifier = Ast::clone(identifier->identifierRef, typeNode);
            auto back            = typeNode->identifier->childs.back();
            back->flags &= ~AST_NO_BYTECODE;
            back->flags |= AST_IN_TYPE_VAR_DECLARATION;

            // And make a reference to that variable
            auto identifierRef = identifier->identifierRef;
            identifierRef->childs.clear();
            auto idNode = Ast::newIdentifier(sourceFile, varNode->name, identifierRef, identifierRef);
            idNode->flags |= AST_R_VALUE | AST_TRANSIENT;

            // Reset parsing
            identifierRef->startScope = nullptr;

            // Add the 2 nodes to the semantic
            context->job->nodes.pop_back();
            context->job->nodes.push_back(idNode);
            context->job->nodes.push_back(varNode);
            context->job->nodes.push_back(identifier);
        }

        // Need to make all types compatible, in case a cast is necessary
        if (identifier->callParameters && oneMatch)
        {
            sortParameters(identifier->callParameters);
            auto maxParams = identifier->callParameters->childs.size();
            for (int i = 0; i < maxParams; i++)
            {
                auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
                if (i < oneMatch->solvedParameters.size() && oneMatch->solvedParameters[i])
                    SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch->solvedParameters[i]->typeInfo, nullptr, nodeCall));
            }
        }

        break;

    case SymbolKind::Variable:
    {
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
            if (identifier->callParameters && oneMatch)
            {
                sortParameters(identifier->callParameters);
                auto maxParams = identifier->callParameters->childs.size();
                for (int i = 0; i < maxParams; i++)
                {
                    auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
                    if (i < oneMatch->solvedParameters.size() && oneMatch->solvedParameters[i])
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch->solvedParameters[i]->typeInfo, nullptr, nodeCall));
                    else if (oneMatch->solvedParameters.back() && oneMatch->solvedParameters.back()->typeInfo->kind == TypeInfoKind::TypedVariadic)
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch->solvedParameters.back()->typeInfo, nullptr, nodeCall));
                }
            }

            // For a return by copy, need to reserve room on the stack for the return result
            auto returnType = TypeManager::concreteType(identifier->typeInfo);
            if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
            {
                identifier->flags |= AST_TRANSIENT;
                identifier->fctCallStorageOffset = identifier->ownerScope->startStackSize;
                identifier->ownerScope->startStackSize += returnType->sizeOf;
                identifier->ownerFct->stackSize = max(identifier->ownerFct->stackSize, identifier->ownerScope->startStackSize);
            }
        }

        break;
    }

    case SymbolKind::Function:
    {
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

        identifier->flags |= AST_L_VALUE | AST_R_VALUE;

        // Need to make all types compatible, in case a cast is necessary
        if (!identifier->ownerFct || !(identifier->ownerFct->flags & AST_IS_GENERIC))
        {
            if (identifier->callParameters && oneMatch)
            {
                sortParameters(identifier->callParameters);
                auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr);
                auto maxParams    = identifier->callParameters->childs.size();
                for (int i = 0; i < maxParams; i++)
                {
                    auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);

                    if (i < oneMatch->solvedParameters.size() && oneMatch->solvedParameters[i])
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch->solvedParameters[i]->typeInfo, nullptr, nodeCall));
                    else if (oneMatch->solvedParameters.size() && oneMatch->solvedParameters.back() && oneMatch->solvedParameters.back()->typeInfo->kind == TypeInfoKind::TypedVariadic)
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch->solvedParameters.back()->typeInfo, nullptr, nodeCall));

                    // For a variadic parameter, we need to generate the concrete typeinfo for the corresponding 'any' type
                    if (i >= typeInfoFunc->parameters.size() - 1 && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
                    {
                        auto& typeTable = sourceFile->module->typeTable;
                        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, nodeCall->typeInfo, &nodeCall->concreteTypeInfo, &nodeCall->concreteTypeInfoStorage));
                    }
                }
            }
        }

        // Be sure the call is valid
        if ((identifier->token.id != TokenId::Intrinsic) && !(overload->node->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            auto ownerFct = identifier->ownerFct;
            if (ownerFct)
            {
                auto fctAttributes = ownerFct->attributeFlags;

                if (!(fctAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER) && !(identifier->flags & AST_RUN_BLOCK))
                    return context->report({identifier, identifier->token, format("cannot reference 'swag.compiler' function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
                if (!(fctAttributes & ATTRIBUTE_TEST_FUNC) && (overload->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
                    return context->report({identifier, identifier->token, format("cannot reference 'swag.test' function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
            }
        }

        // This is for a lambda
        if (identifier->flags & AST_TAKE_ADDRESS)
        {
            // The makePointer will deal with the real make lambda thing
            identifier->flags |= AST_NO_BYTECODE;
            break;
        }

        // Expand inline function. Do not expand an inline call inside a function marked as inline.
        // The expansion will be done at the lowest level possible
        if (identifier->ownerFct && !(identifier->ownerFct->attributeFlags & ATTRIBUTE_INLINE))
        {
            if (overload->node->attributeFlags & ATTRIBUTE_INLINE)
            {
                // Need to wait for function full semantic resolve
                auto funcDecl = static_cast<AstFuncDecl*>(overload->node);
                {
                    scoped_lock lk(funcDecl->mutex);
                    if (!(funcDecl->flags & AST_FULL_RESOLVE))
                    {
                        funcDecl->dependentJobs.add(context->job);
                        context->job->setPending(funcDecl->resolvedSymbolName);
                        return true;
                    }
                }

                if (!(identifier->doneFlags & AST_DONE_INLINED))
                {
                    identifier->doneFlags |= AST_DONE_INLINED;
                    SWAG_CHECK(makeInline(context, funcDecl, identifier));
                }

                identifier->byteCodeFct = ByteCodeGenJob::emitPassThrough;
                return true;
            }
        }

        identifier->kind = AstNodeKind::FuncCall;

        // The function call is constexpr if the function is, and all parameters are
        if (identifier->resolvedSymbolOverload->node->flags & AST_CONST_EXPR)
        {
            if (identifier->callParameters)
                identifier->inheritAndFlag(identifier->callParameters, AST_CONST_EXPR);
            else
                identifier->flags |= AST_CONST_EXPR;
        }

        if (identifier->token.id == TokenId::Intrinsic)
            identifier->byteCodeFct = ByteCodeGenJob::emitIntrinsic;
        else if (overload->node->attributeFlags & ATTRIBUTE_FOREIGN)
            identifier->byteCodeFct = ByteCodeGenJob::emitForeignCall;
        else
            identifier->byteCodeFct = ByteCodeGenJob::emitCall;

        // Setup parent if necessary
        auto returnType = TypeManager::concreteType(identifier->typeInfo);
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
            identifier->fctCallStorageOffset = identifier->ownerScope->startStackSize;
            identifier->ownerScope->startStackSize += returnType->sizeOf;
            identifier->ownerFct->stackSize = max(identifier->ownerFct->stackSize, identifier->ownerScope->startStackSize);
        }

        break;
    }
    }

    return true;
}

bool SemanticJob::matchIdentifierParameters(SemanticContext* context, AstNode* genericParameters, AstNode* callParameters, AstNode* node)
{
    auto              job                 = context->job;
    auto&             matches             = job->cacheMatches;
    auto&             genericMatches      = job->cacheGenericMatches;
    auto&             badSignature        = job->cacheBadSignature;
    auto&             badGenericSignature = job->cacheBadGenericSignature;
    auto&             dependentSymbols    = job->cacheDependentSymbols;
    BadSignatureInfos bestSignatureInfos;

anotherTry:
    matches.clear();
    genericMatches.clear();
    badSignature.clear();
    badGenericSignature.clear();
    bestSignatureInfos.clear();

    bool forStruct               = false;
    bool hasGenericErrors        = false;
    int  numOverloads            = 0;
    int  numOverloadsWhenChecked = 0;
    for (auto symbol : dependentSymbols)
    {
        scoped_lock lock(symbol->mutex);
        if (symbol->kind == SymbolKind::Function && symbol->cptOverloadsInit == symbol->overloads.size())
        {
            // This is enough to resolve
        }
        else if (symbol->cptOverloads)
        {
            job->waitForSymbolNoLock(symbol);
            return true;
        }

        numOverloadsWhenChecked = (int) symbol->overloads.size();
        for (auto overload : symbol->overloads)
        {
            numOverloads++;

            auto rawTypeInfo = overload->typeInfo;

            // If this is a type alias that already has a generic instance, accept to not have generic
            // parameters on the source symbol
            if (overload->typeInfo->kind == TypeInfoKind::Alias)
            {
                rawTypeInfo = TypeManager::concreteType(overload->typeInfo, CONCRETE_ALIAS);
                if (rawTypeInfo->kind == TypeInfoKind::Struct)
                {
                    auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                    if (!(typeInfo->flags & TYPEINFO_GENERIC))
                        job->symMatch.flags |= SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC;
                }
            }

            if (rawTypeInfo->kind == TypeInfoKind::Struct)
            {
                forStruct     = true;
                auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                typeInfo->match(job->symMatch);
            }
            else if (rawTypeInfo->kind == TypeInfoKind::Interface)
            {
                forStruct     = true;
                auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Interface);
                typeInfo->match(job->symMatch);
            }
            else if (rawTypeInfo->kind == TypeInfoKind::FuncAttr)
            {
                auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::FuncAttr);
                typeInfo->match(job->symMatch);
            }
            else if (rawTypeInfo->kind == TypeInfoKind::Lambda)
            {
                auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::Lambda);
                typeInfo->match(job->symMatch);
            }
            else
            {
                SWAG_ASSERT(false);
            }

            // We need () for a function call !
            if (!callParameters && job->symMatch.result == MatchResult::Ok && symbol->kind == SymbolKind::Function)
            {
                auto grandParent = node->parent->parent;
                if (grandParent->kind != AstNodeKind::MakePointer)
                {
                    job->symMatch.result = MatchResult::MissingParameters;
                }
            }

            // Function type without call parameters
            if (!callParameters && job->symMatch.result == MatchResult::NotEnoughParameters)
            {
                if (symbol->kind == SymbolKind::Variable)
                    job->symMatch.result = MatchResult::Ok;
                else if (symbol->kind == SymbolKind::Function && node->parent->childs.size() == 1)
                {
                    // We can make @typeof(function), without adding the 'function' parameters
                    auto grandParent = node->parent->parent;
                    if (grandParent->kind == AstNodeKind::IntrinsicProp)
                    {
                        auto prop = CastAst<AstProperty>(grandParent, AstNodeKind::IntrinsicProp);
                        if (prop->prop == Property::TypeOf)
                            job->symMatch.result = MatchResult::Ok;
                    }
                }
            }

            // Remember the signature with the longest match (for error reporting)
            if (job->symMatch.result != MatchResult::Ok)
            {
                if (bestSignatureInfos.badSignatureParameterIdx == -1 ||
                    job->symMatch.badSignatureInfos.badSignatureParameterIdx > bestSignatureInfos.badSignatureParameterIdx)
                    bestSignatureInfos = job->symMatch.badSignatureInfos;
            }

            switch (job->symMatch.result)
            {
            case MatchResult::Ok:
                if (overload->flags & OVERLOAD_GENERIC)
                {
                    OneGenericMatch match;
                    match.flags                      = job->symMatch.flags;
                    match.symbolName                 = symbol;
                    match.symbolOverload             = overload;
                    match.genericParametersCallTypes = move(job->symMatch.genericParametersCallTypes);
                    match.genericParametersGenTypes  = move(job->symMatch.genericParametersGenTypes);
                    match.genericReplaceTypes        = move(job->symMatch.genericReplaceTypes);
                    genericMatches.emplace_back(match);
                }
                else
                {
                    OneMatch match;
                    match.symbolName       = symbol;
                    match.symbolOverload   = overload;
                    match.solvedParameters = move(job->symMatch.solvedParameters);
                    matches.emplace_back(match);
                }
                break;

            case MatchResult::BadGenericSignature:
                if (overload->typeInfo->flags & TYPEINFO_GENERIC)
                {
                    badGenericSignature.push_back(overload);
                    hasGenericErrors = true;
                }
                break;

            case MatchResult::BadSignature:
                badSignature.push_back(overload);
                break;

            case MatchResult::TooManyGenericParameters:
            case MatchResult::NotEnoughGenericParameters:
                hasGenericErrors = true;
                break;
            }
        }
    }

    // This is a generic
    if (genericMatches.size() == 1 && matches.size() == 0)
    {
        auto symbol = *dependentSymbols.begin();
        symbol->mutex.lock();

        // Be sure we don't have more overloads waiting to be solved
        if (symbol->cptOverloads)
        {
            job->waitForSymbolNoLock(symbol);
            symbol->mutex.unlock();
            return true;
        }

        // Be sure number of overloads has not changed since then
        if (numOverloadsWhenChecked != symbol->overloads.size())
        {
            symbol->mutex.unlock();
            goto anotherTry;
        }

        auto& firstMatch = genericMatches[0];
        if (forStruct)
        {
            if ((node->flags & AST_CAN_INSTANCIATE_TYPE) && !(node->flags & AST_IS_GENERIC) && genericParameters)
            {
                SWAG_CHECK(Generic::instanciateStruct(context, genericParameters, firstMatch, true));
            }
            else
            {
                OneMatch oneMatch;
                oneMatch.symbolName     = firstMatch.symbolName;
                oneMatch.symbolOverload = firstMatch.symbolOverload;
                matches.emplace_back(oneMatch);
                node->flags |= AST_IS_GENERIC;
                if (firstMatch.flags & SymbolMatchContext::MATCH_WAS_PARTIAL)
                    node->flags |= AST_GENERIC_MATCH_WAS_PARTIAL;
            }
        }
        else
        {
            SWAG_CHECK(Generic::instanciateFunction(context, genericParameters, firstMatch));
        }

        symbol->mutex.unlock();
        return true;
    }

    auto symbol = *dependentSymbols.begin();
    if (matches.size() == 0)
    {
        BadSignatureInfos badSignatureInfos = job->symMatch.badSignatureInfos;

        // If there's a generic in the overloads list, then we need to raise on error
        // with that symbol only (even if some other overloads have already been instantiated).
        // Because in the end, this is the generic that didn't watch
        SymbolOverload* overload = nullptr;
        {
            shared_lock lk(symbol->mutex);
            for (auto one : symbol->overloads)
            {
                if (one->flags & OVERLOAD_GENERIC)
                {
                    overload          = one;
                    badSignatureInfos = bestSignatureInfos;
                    break;
                }
            }
        }

        if (numOverloads == 1 || overload)
        {
            auto& match = job->symMatch;

            if (!overload)
            {
                shared_lock lk(symbol->mutex);
                overload = symbol->overloads[0];
            }

            // Be sure this is not because of an invalid special function signature
            if (overload->node->kind == AstNodeKind::FuncDecl)
                SWAG_CHECK(checkFuncPrototype(context, CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl)));

            switch (match.result)
            {
            case MatchResult::MissingNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[badSignatureInfos.badSignatureParameterIdx]);
                Diagnostic diag{param, format("parameter '%d' must be named", badSignatureInfos.badSignatureParameterIdx + 1)};
                return context->report(diag);
            }
            case MatchResult::InvalidNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[badSignatureInfos.badSignatureParameterIdx]);
                Diagnostic diag{param->namedParamNode ? param->namedParamNode : param, format("unknown named parameter '%s'", param->namedParam.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            case MatchResult::DuplicatedNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[badSignatureInfos.badSignatureParameterIdx]);
                Diagnostic diag{param->namedParamNode, format("named parameter '%s' already used", param->namedParam.c_str())};
                return context->report(diag);
            }
            case MatchResult::NotEnoughParameters:
            {
                Diagnostic diag{callParameters ? callParameters : node, format("not enough parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            case MatchResult::MissingParameters:
            {
                Diagnostic diag{callParameters ? callParameters : node, format("missing function call '()' to %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            case MatchResult::TooManyParameters:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(match.parameters[badSignatureInfos.badSignatureParameterIdx]);
                Diagnostic diag{param, format("too many parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            case MatchResult::NotEnoughGenericParameters:
            {
                auto       errNode = genericParameters ? genericParameters : node ? node : context->node;
                Diagnostic diag{errNode, errNode->token, format("not enough generic parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            case MatchResult::TooManyGenericParameters:
            {
                auto       errNode = genericParameters ? genericParameters : node ? node : context->node;
                Diagnostic diag{errNode, errNode->token, format("too many generic parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            case MatchResult::BadSignature:
            {
                SWAG_ASSERT(callParameters);
                Diagnostic diag{match.parameters[badSignatureInfos.badSignatureParameterIdx],
                                format("bad type of parameter '%d' for %s '%s' ('%s' expected, '%s' provided)",
                                       badSignatureInfos.badSignatureParameterIdx + 1,
                                       SymTable::getNakedKindName(symbol->kind),
                                       symbol->name.c_str(),
                                       badSignatureInfos.badSignatureRequestedType->name.c_str(),
                                       badSignatureInfos.badSignatureGivenType->name.c_str())};
                if (TypeManager::makeCompatibles(context, badSignatureInfos.badSignatureRequestedType, badSignatureInfos.badSignatureGivenType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
                    diag.codeComment = format("'cast(%s)' can be used in that context", badSignatureInfos.badSignatureRequestedType->name.c_str());
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            case MatchResult::BadGenericSignature:
            {
                SWAG_ASSERT(genericParameters);
                if (match.flags & SymbolMatchContext::MATCH_ERROR_VALUE_TYPE)
                {
                    Diagnostic diag{match.genericParameters[badSignatureInfos.badSignatureParameterIdx],
                                    format("bad generic parameter '%d' for %s '%s' (type expected, value provided)",
                                           badSignatureInfos.badSignatureParameterIdx + 1,
                                           SymTable::getNakedKindName(symbol->kind),
                                           symbol->name.c_str())};
                    Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }
                else if (match.flags & SymbolMatchContext::MATCH_ERROR_TYPE_VALUE)
                {
                    Diagnostic diag{match.genericParameters[badSignatureInfos.badSignatureParameterIdx],
                                    format("bad generic parameter '%d' for %s '%s' (value expected, type provided)",
                                           badSignatureInfos.badSignatureParameterIdx + 1,
                                           SymTable::getNakedKindName(symbol->kind),
                                           symbol->name.c_str())};
                    Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }
                else
                {
                    Diagnostic diag{match.genericParameters[badSignatureInfos.badSignatureParameterIdx],
                                    format("bad type of generic parameter '%d' for %s '%s' ('%s' expected, '%s' provided)",
                                           badSignatureInfos.badSignatureParameterIdx + 1,
                                           SymTable::getNakedKindName(symbol->kind),
                                           symbol->name.c_str(),
                                           badSignatureInfos.badSignatureRequestedType->name.c_str(),
                                           badSignatureInfos.badSignatureGivenType->name.c_str())};
                    if (TypeManager::makeCompatibles(context, badSignatureInfos.badSignatureRequestedType, badSignatureInfos.badSignatureGivenType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
                        diag.codeComment = format("'cast(%s)' can be used in that context", badSignatureInfos.badSignatureRequestedType->name.c_str());
                    Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }
            }
            }
        }
        else
        {
            if (badSignature.size())
            {
                Diagnostic                diag{callParameters ? callParameters : node, format("none of the %d overloads could convert all the parameters types", numOverloads)};
                vector<const Diagnostic*> notes;
                for (auto one : badSignature)
                {
                    auto couldBe                = "cast has failed for: " + Ast::computeTypeDisplay(one->node->name, one->typeInfo);
                    auto note                   = new Diagnostic{one->node, one->node->token, couldBe, DiagnosticLevel::Note};
                    note->showRange             = false;
                    note->showMultipleCodeLines = false;
                    notes.push_back(note);
                }

                return context->report(diag, notes);
            }
            else if (badGenericSignature.size())
            {
                Diagnostic                diag{genericParameters ? genericParameters : node, format("none of the %d overloads could convert all the generic parameters types", numOverloads)};
                vector<const Diagnostic*> notes;
                for (auto one : badGenericSignature)
                {
                    auto couldBe                = "cast has failed for: " + Ast::computeTypeDisplay(one->node->name, one->typeInfo);
                    auto note                   = new Diagnostic{one->node, one->node->token, couldBe, DiagnosticLevel::Note};
                    note->showRange             = false;
                    note->showMultipleCodeLines = false;
                    notes.push_back(note);
                }

                return context->report(diag, notes);
            }
            else if (hasGenericErrors)
            {
                int         numParams = genericParameters ? (int) genericParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "generic parameter" : "generic parameters";
                Diagnostic  diag{genericParameters ? genericParameters : node, format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return context->report(diag);
            }
            else
            {
                int         numParams = callParameters ? (int) callParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "parameter" : "parameters";
                Diagnostic  diag{callParameters ? callParameters : node, format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return context->report(diag);
            }
        }
    }

    if (matches.size() > 1)
    {
        Diagnostic                diag{node, node->token, format("ambiguous resolution of %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
        vector<const Diagnostic*> notes;
        for (auto match : matches)
        {
            auto overload = match.symbolOverload;
            auto couldBe  = "could be: " + Ast::computeTypeDisplay(overload->node->name, overload->typeInfo);
            auto note     = new Diagnostic{overload->node, overload->node->token, couldBe, DiagnosticLevel::Note};

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

bool SemanticJob::pickSymbol(SemanticContext* context, AstIdentifier* node, SymbolName** result)
{
    auto  job                = context->job;
    auto  identifierRef      = node->identifierRef;
    auto& dependentSymbols   = job->cacheDependentSymbols;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;

    if (dependentSymbols.size() == 1)
    {
        *result = *dependentSymbols.begin();
        return true;
    }

    SymbolName* symbol = nullptr;
    for (auto oneSymbol : dependentSymbols)
    {
        if (node->callParameters && oneSymbol->kind == SymbolKind::Variable)
            continue;
        if (!node->callParameters && oneSymbol->kind == SymbolKind::Function)
            continue;

        // Reference to a variable inside a struct, without a direct explicit reference
        bool isValid = true;
        if (oneSymbol->ownerTable->scope->kind == ScopeKind::Struct && !identifierRef->startScope)
        {
            isValid = false;
            for (auto& dep : scopeHierarchyVars)
            {
                if (dep.scope->fullname == oneSymbol->ownerTable->scope->fullname)
                {
                    isValid = true;
                    break;
                }
            }
        }

        if (!isValid)
            continue;

        if (!symbol)
        {
            symbol = oneSymbol;
            continue;
        }

        // Priority to a concrete type versus a generic one
        auto lastOverload = symbol->ownerTable->scope->owner->typeInfo;
        auto newOverload  = oneSymbol->ownerTable->scope->owner->typeInfo;
        if (lastOverload && newOverload)
        {
            if (!(lastOverload->flags & TYPEINFO_GENERIC) && (newOverload->flags & TYPEINFO_GENERIC))
            {
                continue;
            }

            if ((lastOverload->flags & TYPEINFO_GENERIC) && !(newOverload->flags & TYPEINFO_GENERIC))
            {
                symbol = oneSymbol;
                continue;
            }
        }

        // Priority to the same inline scope
        if (node->ownerInline && symbol->overloads.size() == 1 && oneSymbol->overloads.size() == 1)
        {
            if ((oneSymbol->overloads[0]->node->ownerScope != node->ownerInline->ownerScope) &&
                (symbol->overloads[0]->node->ownerScope == node->ownerInline->ownerScope))
            {
                continue;
            }

            if ((oneSymbol->overloads[0]->node->ownerScope == node->ownerInline->ownerScope) &&
                (symbol->overloads[0]->node->ownerScope != node->ownerInline->ownerScope))
            {
                symbol = oneSymbol;
                continue;
            }
        }

        // Error this is ambiguous
        if (symbol->kind != oneSymbol->kind || oneSymbol->kind != SymbolKind::Function)
        {
            Diagnostic diag{node, node->token, format("ambiguous resolution of '%s'", symbol->name.c_str())};
            Diagnostic note1{symbol->overloads[0]->node, symbol->overloads[0]->node->token, "could be", DiagnosticLevel::Note};
            Diagnostic note2{oneSymbol->overloads[0]->node, oneSymbol->overloads[0]->node->token, "could be", DiagnosticLevel::Note};
            context->report(diag, &note1, &note2);
            return false;
        }
    }

    *result = symbol;
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
    Ast::removeFromParent(rightAffect);
    Ast::addChildBack(fctCallParam, rightAffect);

    node->flags &= ~AST_TAKE_ADDRESS;
    node->identifierRef->parent->byteCodeFct = &ByteCodeGenJob::emitPassThrough;
    node->identifierRef->parent->semanticFct = nullptr;

    return true;
}

bool SemanticJob::ufcsSetFirstParam(SemanticContext* context, AstIdentifierRef* identifierRef, SymbolName* symbol)
{
    auto node = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);

    auto fctCallParam = Ast::newNode<AstFuncCallParam>(nullptr, AstNodeKind::FuncCallParam, node->sourceFile, nullptr);
    if (!node->callParameters)
        node->callParameters = Ast::newFuncCallParams(context->sourceFile, node);
    node->callParameters->childs.push_front(fctCallParam);
    fctCallParam->parent      = node->callParameters;
    fctCallParam->typeInfo    = identifierRef->previousResolvedNode->typeInfo;
    fctCallParam->token       = identifierRef->previousResolvedNode->token;
    fctCallParam->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;

    auto prevIdRef = CastAst<AstIdentifierRef>(identifierRef->previousResolvedNode->parent, AstNodeKind::IdentifierRef);
    auto idRef     = Ast::newIdentifierRef(node->sourceFile, fctCallParam);
    if (symbol->kind == SymbolKind::Variable)
    {
        // Call from a lambda, on a variable : we need to keep the original variable, and put the UFCS one in its own identifierref
        // Copy all previous references to the one we want to pass as parameter
        // X.Y.call(...) => X.Y.call(X.Y, ...)
        for (auto child : prevIdRef->childs)
        {
            auto copyChild = Ast::clone(child, idRef);
            if (child == identifierRef->previousResolvedNode)
            {
                copyChild->flags |= AST_TO_UFCS;
                break;
            }
        }
    }
    else
    {
        // Move all previous references to the one we want to pass as parameter
        // X.Y.call(...) => call(X.Y, ...)
        while (prevIdRef->childs.size())
        {
            auto copyChild = prevIdRef->childs.front();
            Ast::removeFromParent(copyChild);
            Ast::addChildBack(idRef, copyChild);
            if (copyChild == identifierRef->previousResolvedNode)
            {
                copyChild->flags |= AST_TO_UFCS;
                break;
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

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto node         = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    node->byteCodeFct = ByteCodeGenJob::emitIdentifier;

    auto  job                = context->job;
    auto& scopeHierarchy     = job->cacheScopeHierarchy;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;
    auto& dependentSymbols   = job->cacheDependentSymbols;
    auto  identifierRef      = node->identifierRef;

    // Already solved
    if ((node->flags & AST_FROM_GENERIC) && node->typeInfo)
    {
        SWAG_CHECK(setSymbolMatch(context, identifierRef, node, node->resolvedSymbolName, node->resolvedSymbolOverload, nullptr, nullptr));
        return true;
    }

    if (node->name == "Self")
    {
        SWAG_VERIFY(node->ownerStructScope, context->report({node, node->token, "type 'Self' cannot be used outside an 'impl' block"}));
        node->name = node->ownerStructScope->name;
    }

    if (node->semanticState == AstNodeResolveState::ProcessingChilds)
    {
        scopeHierarchy.clear();
        scopeHierarchyVars.clear();
        dependentSymbols.clear();
    }

    // Compute dependencies if not already done
    if (dependentSymbols.empty())
    {
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
                    collectFlags = COLLECT_PASS_INLINE;

                startScope = node->ownerScope;
                SWAG_CHECK(collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, collectFlags));

                // Be sure this is the last try
                oneTry++;
            }
            else
            {
                scopeHierarchy.insert(startScope);

                // A namespace scope can in fact be shared between multiple nodes, so the 'owner' is not
                // relevant and we should not use it
                if (startScope->kind != ScopeKind::Namespace)
                {
                    for (auto s : startScope->owner->alternativeScopes)
                        scopeHierarchy.insert(s);
                    scopeHierarchyVars.insert(scopeHierarchyVars.end(), startScope->owner->alternativeScopesVars.begin(), startScope->owner->alternativeScopesVars.end());
                }
            }

            // Search symbol in all the scopes of the hierarchy
            for (auto scope : scopeHierarchy)
            {
                auto symbol = scope->symTable.find(node->name);
                if (symbol)
                {
                    dependentSymbols.insert(symbol);

                    // Tentative to have a better error message in the case of local variables problem (a local variable is referencing another one
                    // defined later).
                    if (!scope->isGlobal() && symbol->kind == SymbolKind::Variable)
                    {
                        scoped_lock lkn(symbol->mutex);
                        if (symbol->cptOverloads && node->ownerFct)
                        {
                            auto testScope = identifierRef->ownerScope;
                            while (testScope && testScope != node->ownerFct->scope && testScope != scope)
                                testScope = testScope->parentScope;
                            if (testScope == scope)
                            {
                                return context->report({node, node->token, format("unknown identifier '%s' (seems to be defined later)", node->name.c_str())});
                            }
                        }
                    }
                }
            }

            if (!dependentSymbols.empty())
                break;

            // We raise an error if we have tried to resolve with the normal scope hierarchy, and not just the scope
            // from the previous symbol
            if (oneTry)
            {
                if (identifierRef->startScope)
                {
                    auto displayName = identifierRef->startScope->fullname;
                    if (identifierRef->startScope->name.empty() && identifierRef->typeInfo)
                        displayName = identifierRef->typeInfo->name;
                    if (!displayName.empty())
                        return context->report({node, node->token, format("identifier '%s' cannot be found in %s '%s'", node->name.c_str(), Scope::getNakedKindName(identifierRef->startScope->kind), displayName.c_str())});
                }

                if (identifierRef->typeInfo)
                {
                    return context->report({node, node->token, format("identifier '%s' cannot be found in type '%s'", node->name.c_str(), identifierRef->typeInfo->name.c_str())});
                }

                return context->report({node, node->token, format("unknown identifier '%s'", node->name.c_str())});
            }

            node->flags |= AST_FORCE_UFCS;
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto symbol : dependentSymbols)
    {
        scoped_lock lkn(symbol->mutex);
        if (symbol->cptOverloads)
        {
            if (symbol->kind == SymbolKind::Function && symbol->overloads.size() == symbol->cptOverloadsInit)
            {
                // This is enough to resolve, as we just need parameters, and that case means that some functions
                // do not know their return type yet (short lambdas)
            }
            else
            {
                // If a structure is referencing itself, we will match the incomplete symbol for now
                if ((symbol->kind == SymbolKind::Struct || symbol->kind == SymbolKind::Interface) &&
                    node->ownerMainNode &&
                    node->ownerMainNode->kind != AstNodeKind::Impl &&
                    node->ownerMainNode->name == symbol->name)
                {
                    SWAG_VERIFY(!node->callParameters, internalError(context, "resolveIdentifier, struct auto ref, has parameters"));
                    SWAG_VERIFY(!node->genericParameters, internalError(context, "resolveIdentifier, struct auto ref, has generic parameters"));
                    SWAG_ASSERT(symbol->overloads.size() == 1 && (symbol->overloads[0]->flags & OVERLOAD_INCOMPLETE));
                    node->resolvedSymbolName     = symbol;
                    node->resolvedSymbolOverload = symbol->overloads[0];
                    node->typeInfo               = node->resolvedSymbolOverload->typeInfo;
                }
                else
                {
                    job->waitForSymbolNoLock(symbol);
                }

                return true;
            }
        }
    }

    // If we have multiple symbols, we need to choose one
    SymbolName* symbol = nullptr;
    SWAG_CHECK(pickSymbol(context, node, &symbol));

    AstNode* ufcsParam = nullptr;
    bool     canDoUfcs = false;
    if (symbol->kind == SymbolKind::Function)
        canDoUfcs = true;
    if (symbol->kind == SymbolKind::Variable && symbol->overloads.size() == 1 && symbol->overloads.front()->typeInfo->kind == TypeInfoKind::Lambda)
        canDoUfcs = node->callParameters;

    if (!(node->doneFlags & AST_DONE_UFCS) && canDoUfcs)
    {
        // If a variable is defined just before a function call, then this can be an UFCS (unified function call system)
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind == SymbolKind::Variable)
        {
            // If we do not have parenthesis (call parameters), then this must be a function marked with 'swag.property'
            if (!node->callParameters)
            {
                SWAG_VERIFY(symbol->kind == SymbolKind::Function, context->report({node, "missing function call parameters"}));
                SWAG_VERIFY(symbol->overloads.size() <= 2, context->report({node, "too many overloads for a property (only one set and one get should exist)"}));
                SWAG_VERIFY(symbol->overloads.front()->node->attributeFlags & ATTRIBUTE_PROPERTY, context->report({node, format("missing function call parameters because symbol '%s' is not marked as 'swag.property'", symbol->name.c_str())}));
            }

            if (node->ownerFct && (node->ownerFct->flags & AST_IS_GENERIC))
            {
                SWAG_ASSERT(identifierRef->previousResolvedNode);
                ufcsParam = identifierRef->previousResolvedNode;
            }
            else
            {
                node->doneFlags |= AST_DONE_UFCS;
                SWAG_CHECK(ufcsSetLastParam(context, identifierRef, symbol));
                SWAG_CHECK(ufcsSetFirstParam(context, identifierRef, symbol));
            }
        }
    }

    // We want to force the ufcs
    if (!ufcsParam && (node->flags & AST_FORCE_UFCS) && !(node->doneFlags & AST_DONE_UFCS))
    {
        if (identifierRef->startScope)
        {
            auto displayName = identifierRef->startScope->fullname;
            if (identifierRef->startScope->name.empty() && identifierRef->typeInfo)
                displayName = identifierRef->typeInfo->name;
            if (!displayName.empty())
                return context->report({node, node->token, format("identifier '%s' cannot be found in %s '%s'", node->name.c_str(), Scope::getNakedKindName(identifierRef->startScope->kind), displayName.c_str())});
        }

        return context->report({node, node->token, format("unknown identifier '%s'", node->name.c_str())});
    }

    if (canDoUfcs && (symbol->kind == SymbolKind::Variable))
    {
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind == SymbolKind::Struct)
            return context->report({node, node->token, format("invalid lambda call, cannot reference structure member '%s'", symbol->name.c_str())});
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind != SymbolKind::Variable)
            return context->report({node, format("invalid lambda call because '%s' is not a variable", identifierRef->resolvedSymbolName->name.c_str())});
    }

    // Fill specified parameters
    job->symMatch.reset();
    if (node->flags & AST_TAKE_ADDRESS)
        job->symMatch.flags |= SymbolMatchContext::MATCH_FOR_LAMBDA;

    if (!(node->doneFlags & AST_DONE_LAST_PARAM_CODE) && (symbol->kind == SymbolKind::Function))
    {
        node->doneFlags |= AST_DONE_LAST_PARAM_CODE;

        // If last parameter is of type code, and the call last parameter is not, then take the next statement
        if (symbol->overloads.size() > 0)
        {
            bool lastIsCode = false;
            for (auto overload : symbol->overloads)
            {
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
                if (!typeFunc->parameters.empty() && typeFunc->parameters.back()->typeInfo->kind == TypeInfoKind::Code)
                    lastIsCode = true;
                else
                {
                    lastIsCode = false;
                    break;
                }
            }

            if (lastIsCode)
            {
                auto overload = symbol->overloads[0];
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
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
                            auto typeCode     = g_Allocator.alloc<TypeInfoCode>();
                            typeCode->content = brother;
                            brother->flags |= AST_NO_SEMANTIC;
                            fctCallParam->typeInfo = typeCode;
                            codeNode->typeInfo     = typeCode;
                        }
                    }
                }
            }
        }
    }

    auto  genericParameters = node->genericParameters;
    auto  callParameters    = node->callParameters;
    auto& symMatch          = job->symMatch;

    if (callParameters)
    {
        if (symbol->kind != SymbolKind::Attribute &&
            symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            symbol->kind != SymbolKind::Interface &&
            symbol->kind != SymbolKind::TypeAlias &&
            TypeManager::concreteType(symbol->overloads[0]->typeInfo, CONCRETE_ALIAS)->kind != TypeInfoKind::Lambda)
        {
            if (symbol->kind == SymbolKind::Variable)
            {
                Diagnostic diag{node, node->token, format("identifier '%s' has call parameters, but is a variable of type '%s' and not a function", node->name.c_str(), symbol->overloads[0]->typeInfo->name.c_str())};
                Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            else
            {
                Diagnostic diag{node, node->token, format("identifier '%s' has call parameters, but is %s and not a function", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
                Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
        }

        if (ufcsParam)
            symMatch.parameters.push_back(ufcsParam);

        auto childCount = callParameters->childs.size();
        for (int i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(callParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatch.parameters.push_back(oneParam);

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

    if (genericParameters)
    {
        node->inheritOrFlag(genericParameters, AST_IS_GENERIC);
        if (symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            symbol->kind != SymbolKind::Interface &&
            symbol->kind != SymbolKind::TypeAlias)
        {
            Diagnostic diag{callParameters, callParameters->token, format("invalid generic parameters, identifier '%s' is %s and not a function or a structure", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
            Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        auto childCount = genericParameters->childs.size();
        for (int i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(genericParameters->childs[i], AstNodeKind::FuncCallParam, AstNodeKind::IdentifierRef);
            symMatch.genericParameters.push_back(oneParam);
            symMatch.genericParametersCallTypes.push_back(oneParam->typeInfo);
        }
    }

    // If there a using variable associated with the resolved symbol ?
    AstNode* dependentVar = nullptr;
    for (auto& dep : scopeHierarchyVars)
    {
        if (dep.scope->fullname == symbol->ownerTable->scope->fullname)
        {
            if (dependentVar)
            {
                Diagnostic diag{dep.node, "cannot use 'using' on two variables with the same type"};
                Diagnostic note{dependentVar, "this is the other definition", DiagnosticLevel::Note};
                return context->report(diag, &note);
            }

            dependentVar = dep.node;
        }
    }

    // Can happen if a symbol is inside a disabled #if for example
    if (symbol->overloads.empty())
    {
        return context->report({node, format("cannot resolve identifier '%s'", symbol->name.c_str())});
    }

    auto overload = symbol->overloads[0];
    if (symMatch.parameters.empty() && symMatch.genericParameters.empty())
    {
        // For everything except functions/attributes/structs (which have overloads), this is a match
        if (symbol->kind != SymbolKind::Attribute &&
            symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            symbol->kind != SymbolKind::Interface &&
            overload->typeInfo->kind != TypeInfoKind::Lambda)
        {
            SWAG_ASSERT(symbol->overloads.size() == 1);
            node->typeInfo = overload->typeInfo;
            SWAG_CHECK(setSymbolMatch(context, identifierRef, node, symbol, symbol->overloads[0], nullptr, dependentVar));
            return true;
        }
    }

    SWAG_CHECK(matchIdentifierParameters(context, genericParameters, callParameters, node));
    if (context->result == ContextResult::Pending)
        return true;

    auto& match    = job->cacheMatches[0];
    node->typeInfo = match.symbolOverload->typeInfo;
    SWAG_CHECK(setSymbolMatch(context, identifierRef, node, match.symbolName, match.symbolOverload, &match, dependentVar));
    return true;
}

void SemanticJob::collectAlternativeScopeHierarchy(SemanticContext* context, set<Scope*>& scopes, vector<AlternativeScope>& scopesVars, AstNode* startNode)
{
    if (!startNode->alternativeScopes.empty())
    {
        auto  job  = context->job;
        auto& here = job->scopesHere;

        {
            SWAG_RACE_CONDITION_READ(startNode->raceConditionAlternativeScopes);
            for (auto p : startNode->alternativeScopes)
            {
                if (scopes.find(p) == scopes.end())
                {
                    scopes.insert(p);
                    here.push_back(p);
                }
            }

            scopesVars.insert(scopesVars.end(), startNode->alternativeScopesVars.begin(), startNode->alternativeScopesVars.end());
        }
    }

    // If we are in an inline block, jump right to the function parent
    if (startNode->kind == AstNodeKind::Inline)
    {
        while (startNode->kind != AstNodeKind::FuncDecl)
            startNode = startNode->parent;
    }

    if (startNode->parent)
    {
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode->parent);
    }
}

bool SemanticJob::collectScopeHierarchy(SemanticContext* context, set<Scope*>& scopes, vector<AlternativeScope>& scopesVars, AstNode* startNode, uint32_t flags)
{
    auto  job        = context->job;
    auto& here       = job->scopesHere;
    auto  sourceFile = context->sourceFile;

    here.clear();
    scopes.clear();

    // Get alternative scopes from the node hierarchy
    collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode);

    auto startScope = startNode->ownerScope;
    if (startScope)
    {
        scopes.insert(startScope);
        here.push_back(startScope);
    }

    // Can be null because of g_CommandLine.addRuntimeModule to false
    if (g_Workspace.bootstrapModule)
    {
        auto runTime = g_Workspace.bootstrapModule->scopeRoot;
        scopes.insert(runTime);
        here.push_back(runTime);
    }

    for (int i = 0; i < here.size(); i++)
    {
        auto scope = here[i];

        // For an embedded function, jump right to its parent
        // Collect them because we can search for types in those scopes
        if (scope->kind == ScopeKind::Function)
        {
            while (scope->parentScope->kind == ScopeKind::Function || scope->parentScope->owner->ownerFct)
            {
                scope = scope->parentScope;
            }
        }

        // For an inline scope, jump right to the function
        if (scope->kind == ScopeKind::Inline || scope->kind == ScopeKind::Macro)
        {
            if (!(flags & COLLECT_PASS_INLINE))
            {
                bool wasMacro = scope->kind == ScopeKind::Macro;
                while (scope && scope->kind != ScopeKind::Function && scope->parentScope->kind != ScopeKind::Inline)
                    scope = scope->parentScope;
                if (wasMacro && scope->parentScope->kind == ScopeKind::Inline)
                    scope = scope->parentScope;
            }

            flags &= ~COLLECT_PASS_INLINE;
        }

        // Add parent scope
        if (scope->parentScope)
        {
            if (scopes.find(scope->parentScope) == scopes.end())
            {
                scopes.insert(scope->parentScope);
                here.push_back(scope->parentScope);
            }
        }

        // If we are on a module, add all files, except if this is a test module, because for
        // a test module, all file scopes are private
        if (scope->kind == ScopeKind::Module)
        {
            if (!(scope->flags & SCOPE_FLAG_MODULE_FROM_TEST))
            {
                for (auto child : scope->childScopes)
                {
                    if (child->name.empty() || child == sourceFile->scopePrivate)
                    {
                        scopes.insert(child);
                        here.push_back(child);
                    }
                }
            }
            else
            {
                scopes.insert(sourceFile->scopePrivate);
                here.push_back(sourceFile->scopePrivate);
                scopes.insert(sourceFile->scopeRoot);
                here.push_back(sourceFile->scopeRoot);
            }
        }
    }

    SWAG_VERIFY(!(flags & COLLECT_PASS_INLINE), context->report({startNode, startNode->token, "back ticked identifier can only be used inside a 'swag.macro' function or an '#inline' block"}));
    return true;
}

bool SemanticJob::checkSymbolGhosting(SemanticContext* context, AstNode* node, SymbolKind kind)
{
    auto job = context->job;

    // No ghosting from struct
    auto startScope = node->ownerScope;
    if (startScope->kind == ScopeKind::Struct)
        return true;

    uint32_t collectFlags = COLLECT_ALL;
    collectScopeHierarchy(context, job->cacheScopeHierarchy, job->cacheScopeHierarchyVars, node, collectFlags);

    for (auto scope : job->cacheScopeHierarchy)
    {
        // No ghosting with struct
        if (scope->kind == ScopeKind::Struct)
            continue;

        // Do not check if this is the same scope
        if (scope == startScope)
            continue;

        // Be sure that symbol is fully resolved, otherwise we cannot check for a ghosting
        {
            auto symbol = scope->symTable.find(node->name);
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
        }

        SWAG_CHECK(scope->symTable.checkHiddenSymbol(context, node, node->typeInfo, kind));
    }

    return true;
}

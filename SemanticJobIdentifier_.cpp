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
    node->byteCodeFct            = &ByteCodeGenJob::emitIdentifierRef;

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
    auto overload = node->resolvedSymbolOverload;
    if (overload && overload->flags & OVERLOAD_CONST)
        node->flags |= AST_IS_CONST_ASSIGN;

    if (node->parent->kind != AstNodeKind::IdentifierRef)
        return true;

    auto identifierRef = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);

    // Be sure we do not reference a structure field, without a corresponding concrete variable
    if (!identifierRef->typeInfo && overload && (overload->flags & OVERLOAD_VAR_STRUCT) && !(overload->flags & OVERLOAD_COMPUTED_VALUE))
    {
        Diagnostic diag{node, format("cannot reference structure identifier '%s'", node->name.c_str())};
        context->errorContext.report(diag);
        return false;
    }

    identifierRef->typeInfo             = typeInfo;
    identifierRef->previousResolvedNode = node;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        if ((typePointer->ptrCount == 1) && (typePointer->finalType->kind == TypeInfoKind::Struct))
            identifierRef->startScope = static_cast<TypeInfoStruct*>(typePointer->finalType)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::TypeList:
        identifierRef->startScope = static_cast<TypeInfoList*>(typeInfo)->scope;
        node->typeInfo            = typeInfo;
        break;

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

bool SemanticJob::makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstIdentifier* parent)
{
    CloneContext cloneContext;

    auto inlineNode = Ast::newInline(context->sourceFile, parent);
    auto newScope   = Ast::newScope(nullptr, "", ScopeKind::Inline, parent->ownerScope);

    inlineNode->scope = newScope;
    newScope->allocateSymTable();
    cloneContext.parent           = inlineNode;
    cloneContext.ownerInline      = inlineNode;
    cloneContext.ownerFct         = parent->ownerFct;
    cloneContext.ownerBreakable   = parent->ownerBreakable;
    cloneContext.parentScope      = newScope;
    auto newContent               = funcDecl->content->clone(cloneContext);
    newContent->byteCodeBeforeFct = nullptr;

    context->result       = SemanticResult::NewChilds;
    parent->semanticState = AstNodeResolveState::Enter;
    return true;
}

bool SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, SymbolName* symbol, SymbolOverload* overload, OneMatch* oneMatch, AstNode* dependentVar)
{
    // Direct reference to a constexpr typeinfo
    if (parent->previousResolvedNode &&
        parent->previousResolvedNode->flags & AST_VALUE_IS_TYPEINFO &&
        symbol->kind == SymbolKind::Variable)
    {
        SWAG_CHECK(derefTypeInfo(context, parent, overload));
        return true;
    }

    auto sourceFile                    = context->sourceFile;
    parent->resolvedSymbolName         = symbol;
    parent->resolvedSymbolOverload     = overload;
    parent->previousResolvedNode       = identifier;
    identifier->resolvedSymbolName     = symbol;
    identifier->resolvedSymbolOverload = overload;

    if (identifier->typeInfo->flags & TYPEINFO_GENERIC)
        identifier->flags |= AST_IS_GENERIC;

    // Symbol is linked to a using var : insert the variable name before the symbol
    if (dependentVar && !parent->typeInfo && parent->kind == AstNodeKind::IdentifierRef && symbol->kind != SymbolKind::Function)
    {
        auto idRef  = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
        auto idNode = Ast::newIdentifier(sourceFile, dependentVar->name, idRef, nullptr);
        idNode->inheritTokenLocation(identifier->token);
        Ast::addChildFront(idRef, idNode);
        context->node->semanticState = AstNodeResolveState::Enter;
        context->job->nodes.push_back(idNode);
        context->result = SemanticResult::NewChilds;
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
            sourceFile->module->deferReleaseChilds(identifierRef);
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
        identifier->flags |= AST_L_VALUE | AST_R_VALUE;

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
            identifier->byteCodeFct = &ByteCodeGenJob::emitLambdaCall;

            // Need to make all types compatible, in case a cast is necessary
            if (identifier->callParameters && oneMatch)
            {
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
        identifier->flags |= AST_L_VALUE | AST_R_VALUE;

        // Need to make all types compatible, in case a cast is necessary
        if (!identifier->ownerFct || !(identifier->ownerFct->flags & AST_IS_GENERIC))
        {
            if (identifier->callParameters && oneMatch)
            {
                auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr);
                auto maxParams    = identifier->callParameters->childs.size();
                for (int i = 0; i < maxParams; i++)
                {
                    auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
                    if (i < oneMatch->solvedParameters.size() && oneMatch->solvedParameters[i])
                        SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch->solvedParameters[i]->typeInfo, nullptr, nodeCall));
                    else if (oneMatch->solvedParameters.back() && oneMatch->solvedParameters.back()->typeInfo->kind == TypeInfoKind::TypedVariadic)
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

        // This is for a lambda
        if (identifier->flags & AST_TAKE_ADDRESS)
        {
            // The makePointer will deal with the real make lambda thing
            identifier->flags |= AST_NO_BYTECODE;
            break;
        }

        if (overload->node->attributeFlags & ATTRIBUTE_INLINE)
        {
            if (!(identifier->doneFlags & AST_DONE_INLINED))
            {
                identifier->doneFlags |= AST_DONE_INLINED;
                SWAG_CHECK(makeInline(context, static_cast<AstFuncDecl*>(overload->node), identifier));
            }

            identifier->byteCodeFct = nullptr;
            return true;
        }

        identifier->kind = AstNodeKind::FuncCall;
        identifier->inheritOrFlag(identifier->resolvedSymbolOverload->node, AST_CONST_EXPR);

        if (identifier->token.id == TokenId::Intrinsic)
        {
            identifier->byteCodeFct = &ByteCodeGenJob::emitIntrinsic;
        }
        else if (overload->node->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            identifier->byteCodeFct = &ByteCodeGenJob::emitForeignCall;
        }
        else
        {
            identifier->byteCodeFct = &ByteCodeGenJob::emitCall;
            auto ownerFct           = identifier->ownerFct;
            if (ownerFct)
            {
                auto myAttributes = ownerFct->attributeFlags;
                if (!(myAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER))
                    return context->errorContext.report({identifier, identifier->token, format("cannot call compiler function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
                if (!(myAttributes & ATTRIBUTE_TEST_FUNC) && (overload->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
                    return context->errorContext.report({identifier, identifier->token, format("cannot call test function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
            }
        }

        // Setup parent if necessary
        auto returnType = TypeManager::concreteType(identifier->typeInfo);
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
    auto  job                 = context->job;
    auto& matches             = job->cacheMatches;
    auto& genericMatches      = job->cacheGenericMatches;
    auto& badSignature        = job->cacheBadSignature;
    auto& badGenericSignature = job->cacheBadGenericSignature;
    auto& dependentSymbols    = job->cacheDependentSymbols;

anotherTry:
    matches.clear();
    genericMatches.clear();
    badSignature.clear();
    badGenericSignature.clear();

    bool forStruct               = false;
    bool hasGenericErrors        = false;
    int  numOverloads            = 0;
    int  numOverloadsWhenChecked = 0;
    for (auto symbol : dependentSymbols)
    {
        scoped_lock lock(symbol->mutex);
        if (symbol->cptOverloads)
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
                rawTypeInfo = TypeManager::concreteType(overload->typeInfo, MakeConcrete::FlagAlias);
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

            switch (job->symMatch.result)
            {
            case MatchResult::Ok:
                if (overload->flags & OVERLOAD_GENERIC)
                {
                    OneGenericMatch match;
                    match.flags                       = job->symMatch.flags;
                    match.symbolOverload              = overload;
                    match.genericParametersCallTypes  = move(job->symMatch.genericParametersCallTypes);
                    match.genericParametersCallValues = move(job->symMatch.genericParametersCallValues);
                    match.genericParametersGenTypes   = move(job->symMatch.genericParametersGenTypes);
                    genericMatches.emplace_back(match);
                }
                else
                {
                    OneMatch match;
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
        auto symbol = dependentSymbols[0];
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

    auto symbol = dependentSymbols[0];
    if (matches.size() == 0)
    {
        if (numOverloads == 1)
        {
            symbol->mutex.lock();
            auto overload = symbol->overloads[0];
            symbol->mutex.unlock();

            // Be sure this is not because of an invalid special function signature
            if (overload->node->kind == AstNodeKind::FuncDecl)
                SWAG_CHECK(checkFuncPrototype(context, CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl)));

            switch (job->symMatch.result)
            {
            case MatchResult::MissingNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{param, format("parameter '%d' must be named", job->symMatch.badSignatureParameterIdx + 1)};
                return context->errorContext.report(diag);
            }
            case MatchResult::InvalidNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{param->namedParamNode ? param->namedParamNode : param, format("unknown named parameter '%s'", param->namedParam.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::DuplicatedNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{param->namedParamNode, format("named parameter '%s' already used", param->namedParam.c_str())};
                return context->errorContext.report(diag);
            }
            case MatchResult::NotEnoughParameters:
            {
                Diagnostic diag{callParameters ? callParameters : node, format("not enough parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::MissingParameters:
            {
                Diagnostic diag{callParameters ? callParameters : node, format("missing function call '()' to %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::TooManyParameters:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{param, format("too many parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::NotEnoughGenericParameters:
            {
                Diagnostic diag{genericParameters ? genericParameters : node ? node : context->node, format("not enough generic parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::TooManyGenericParameters:
            {
                Diagnostic diag{genericParameters ? genericParameters : node ? node : context->node, format("too many generic parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::BadSignature:
            {
                SWAG_ASSERT(callParameters);
                string parameter;
                switch (job->symMatch.badSignatureParameterIdx)
                {
                case 0:
                    parameter = "first parameter";
                    break;
                case 1:
                    parameter = "second parameter";
                    break;
                case 2:
                    parameter = "third parameter";
                    break;
                default:
                    parameter = format("parameter '%d'", job->symMatch.badSignatureParameterIdx + 1);
                    break;
                }
                Diagnostic diag{callParameters->childs[job->symMatch.badSignatureParameterIdx],
                                format("bad type of %s for %s '%s' ('%s' expected, '%s' provided)",
                                       parameter.c_str(),
                                       SymTable::getNakedKindName(symbol->kind),
                                       symbol->name.c_str(),
                                       job->symMatch.badSignatureRequestedType->name.c_str(),
                                       job->symMatch.badSignatureGivenType->name.c_str())};
                Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::BadGenericSignature:
            {
                SWAG_ASSERT(genericParameters);
                if (job->symMatch.flags & SymbolMatchContext::MATCH_ERROR_VALUE_TYPE)
                {
                    Diagnostic diag{genericParameters->childs[job->symMatch.badSignatureParameterIdx],
                                    format("bad generic parameter '%d' for %s '%s' (type expected, value provided)",
                                           job->symMatch.badSignatureParameterIdx + 1,
                                           SymTable::getNakedKindName(symbol->kind),
                                           symbol->name.c_str())};
                    Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                    return context->errorContext.report(diag, &note);
                }
                else if (job->symMatch.flags & SymbolMatchContext::MATCH_ERROR_TYPE_VALUE)
                {
                    Diagnostic diag{genericParameters->childs[job->symMatch.badSignatureParameterIdx],
                                    format("bad generic parameter '%d' for %s '%s' (value expected, type provided)",
                                           job->symMatch.badSignatureParameterIdx + 1,
                                           SymTable::getNakedKindName(symbol->kind),
                                           symbol->name.c_str())};
                    Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                    return context->errorContext.report(diag, &note);
                }
                else
                {
                    Diagnostic diag{genericParameters->childs[job->symMatch.badSignatureParameterIdx],
                                    format("bad type of generic parameter '%d' for %s '%s' ('%s' expected, '%s' provided)",
                                           job->symMatch.badSignatureParameterIdx + 1,
                                           SymTable::getNakedKindName(symbol->kind),
                                           symbol->name.c_str(),
                                           job->symMatch.badSignatureRequestedType->name.c_str(),
                                           job->symMatch.badSignatureGivenType->name.c_str())};
                    Diagnostic note{overload->node, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                    return context->errorContext.report(diag, &note);
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
                for (auto overload : badSignature)
                {
                    auto note       = new Diagnostic{overload->node, overload->node->token, "cast has failed for", DiagnosticLevel::Note};
                    note->showRange = false;
                    notes.push_back(note);
                }

                return context->errorContext.report(diag, notes);
            }
            else if (badGenericSignature.size())
            {
                Diagnostic                diag{genericParameters ? genericParameters : node, format("none of the %d overloads could convert all the generic parameters types", numOverloads)};
                vector<const Diagnostic*> notes;
                for (auto overload : badGenericSignature)
                {
                    auto note       = new Diagnostic{overload->node, overload->node->token, "cast has failed for", DiagnosticLevel::Note};
                    note->showRange = false;
                    notes.push_back(note);
                }

                return context->errorContext.report(diag, notes);
            }
            else if (hasGenericErrors)
            {
                int         numParams = genericParameters ? (int) genericParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "generic parameter" : "generic parameters";
                Diagnostic  diag{genericParameters ? genericParameters : node, format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return context->errorContext.report(diag);
            }
            else
            {
                int         numParams = callParameters ? (int) callParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "parameter" : "parameters";
                Diagnostic  diag{callParameters ? callParameters : node, format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return context->errorContext.report(diag);
            }
        }
    }

    if (matches.size() > 1)
    {
        if (hasGenericErrors)
        {
            Diagnostic                diag{node, node->token, format("ambiguous generic resolution to %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
            vector<const Diagnostic*> notes;
            for (auto match : matches)
            {
                auto overload   = match.symbolOverload;
                auto note       = new Diagnostic{overload->node, overload->node->token, "could be", DiagnosticLevel::Note};
                note->showRange = false;
                notes.push_back(note);
            }

            context->errorContext.report(diag, notes);
        }
        else
        {
            Diagnostic                diag{node, node->token, format("ambiguous resolution of %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
            vector<const Diagnostic*> notes;
            for (auto match : matches)
            {
                auto overload   = match.symbolOverload;
                auto note       = new Diagnostic{overload->node, overload->node->token, "could be", DiagnosticLevel::Note};
                note->showRange = false;
                notes.push_back(note);
            }

            context->errorContext.report(diag, notes);
        }

        return false;
    }

    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto node         = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    node->byteCodeFct = &ByteCodeGenJob::emitIdentifier;

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
        SWAG_VERIFY(node->ownerStructScope, context->errorContext.report({node, node->token, "type 'Self' cannot be used outside an 'impl' block"}));
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
        auto startScope = identifierRef->startScope;
        if (!startScope)
        {
            startScope = node->ownerScope;
            collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, startScope);
        }
        else
        {
            scopeHierarchy.push_back(startScope);
            scopeHierarchy.insert(scopeHierarchy.end(), startScope->alternativeScopes.begin(), startScope->alternativeScopes.end());
            scopeHierarchyVars.insert(scopeHierarchyVars.end(), startScope->alternativeScopesVars.begin(), startScope->alternativeScopesVars.end());
        }

        // Search symbol in all the scopes of the hierarchy
        for (auto scope : scopeHierarchy)
        {
            if (scope->symTable)
            {
                auto symbol = scope->symTable->find(node->name);
                if (symbol)
                {
                    dependentSymbols.push_back(symbol);
                    if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Attribute)
                        break;
                }
            }
        }

        if (dependentSymbols.empty())
        {
            if (identifierRef->startScope)
            {
                auto displayName = identifierRef->startScope->fullname;
                if (identifierRef->startScope->name.empty() && identifierRef->typeInfo)
                    displayName = identifierRef->typeInfo->name;
                if (!displayName.empty())
                    return context->errorContext.report({node, node->token, format("identifier '%s' cannot be found in %s '%s'", node->name.c_str(), Scope::getNakedName(identifierRef->startScope->kind), displayName.c_str())});
            }

            return context->errorContext.report({node, node->token, format("unknown identifier '%s'", node->name.c_str())});
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto symbol : dependentSymbols)
    {
        scoped_lock lkn(symbol->mutex);
        if (symbol->cptOverloads)
        {
            // If a structure is referencing itself, we will match the incomplete symbol for now
            if (symbol->kind == SymbolKind::Struct && node->ownerStruct && node->ownerStruct->name == symbol->name)
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

    // Fill specified parameters
    job->symMatch.reset();
    if (node->flags & AST_TAKE_ADDRESS)
        job->symMatch.flags |= SymbolMatchContext::MATCH_FOR_LAMBDA;

    // If a variable is defined just before a function call, then this can be an UFCS (unified function call system)
    AstNode* ufcsParam = nullptr;
    if (!(node->doneFlags & AST_DONE_UFCS))
    {
        auto symbol = dependentSymbols[0];
        if (symbol->kind == SymbolKind::Function)
        {
            if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind == SymbolKind::Variable)
            {
                if (node->ownerFct && (node->ownerFct->flags & AST_IS_GENERIC))
                {
                    SWAG_ASSERT(identifierRef->previousResolvedNode);
                    ufcsParam = identifierRef->previousResolvedNode;
                }
                else
                {
                    if (!node->callParameters)
                    {
                        node->callParameters        = Ast::newNode(nullptr, &g_Pool_astNode, AstNodeKind::FuncCallParams, node->sourceFile, node);
                        node->callParameters->token = identifierRef->previousResolvedNode->token;
                    }

                    node->doneFlags |= AST_DONE_UFCS;
                    auto fctCallParam = Ast::newNode(nullptr, &g_Pool_astFuncCallParam, AstNodeKind::FuncCallParam, node->sourceFile, nullptr);
                    node->callParameters->childs.insert(node->callParameters->childs.begin(), fctCallParam);
                    fctCallParam->parent      = node->callParameters;
                    fctCallParam->typeInfo    = identifierRef->previousResolvedNode->typeInfo;
                    fctCallParam->token       = identifierRef->previousResolvedNode->token;
                    fctCallParam->byteCodeFct = &ByteCodeGenJob::emitFuncCallParam;
                    Ast::removeFromParent(identifierRef->previousResolvedNode);
                    Ast::addChildBack(fctCallParam, identifierRef->previousResolvedNode);
                }
            }
        }
    }

    auto  genericParameters = node->genericParameters;
    auto  callParameters    = node->callParameters;
    auto  symbol            = dependentSymbols[0];
    auto& symMatch          = job->symMatch;

    if (callParameters || ufcsParam)
    {
        if (symbol->kind != SymbolKind::Attribute &&
            symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            symbol->kind != SymbolKind::TypeAlias &&
            TypeManager::concreteType(symbol->overloads[0]->typeInfo, MakeConcrete::FlagAlias)->kind != TypeInfoKind::Lambda)
        {
            if (symbol->kind == SymbolKind::Variable)
            {
                Diagnostic diag{node, node->token, format("identifier '%s' has call parameters, but is a variable of type '%s' and not a function", node->name.c_str(), symbol->overloads[0]->typeInfo->name.c_str())};
                Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            else
            {
                Diagnostic diag{node, node->token, format("identifier '%s' has call parameters, but is %s and not a function", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
                Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
        }

        if (ufcsParam)
            symMatch.parameters.push_back(ufcsParam);

        if (callParameters)
        {
            auto childCount = callParameters->childs.size();
            for (int i = 0; i < childCount; i++)
            {
                auto oneParam = CastAst<AstFuncCallParam>(callParameters->childs[i], AstNodeKind::FuncCallParam);
                symMatch.parameters.push_back(oneParam);

                // Variadic parameter must be the last one
                if (i != childCount - 1)
                {
                    if (oneParam->typeInfo->kind == TypeInfoKind::Variadic || oneParam->typeInfo->kind == TypeInfoKind::TypedVariadic)
                    {
                        return context->errorContext.report({oneParam, "variadic argument must be the last one"});
                    }
                }
            }
        }
    }

    if (genericParameters)
    {
        node->inheritOrFlag(genericParameters, AST_IS_GENERIC);
        if (symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            symbol->kind != SymbolKind::TypeAlias)
        {
            Diagnostic diag{callParameters, callParameters->token, format("invalid generic parameters, identifier '%s' is %s and not a function or a structure", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
            Diagnostic note{symbol->defaultOverload.node->sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
            return context->errorContext.report(diag, &note);
        }

        auto childCount = genericParameters->childs.size();
        for (int i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(genericParameters->childs[i], AstNodeKind::FuncCallParam, AstNodeKind::IdentifierRef);
            symMatch.genericParameters.push_back(oneParam);
            symMatch.genericParametersCallValues.push_back(oneParam->computedValue);
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
                return context->errorContext.report(diag, &note);
            }

            dependentVar = dep.node;
        }
    }

    auto overload = symbol->overloads[0];
    if (symMatch.parameters.empty() && symMatch.genericParameters.empty())
    {
        // For everything except functions/attributes/structs (which have overloads), this is a match
        if (symbol->kind != SymbolKind::Attribute &&
            symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            overload->typeInfo->kind != TypeInfoKind::Lambda)
        {
            SWAG_ASSERT(dependentSymbols.size() == 1);
            SWAG_ASSERT(symbol->overloads.size() == 1);
            node->typeInfo = overload->typeInfo;
            SWAG_CHECK(setSymbolMatch(context, identifierRef, node, symbol, symbol->overloads[0], nullptr, dependentVar));
            return true;
        }
    }

    SWAG_CHECK(matchIdentifierParameters(context, genericParameters, callParameters, node));
    if (context->result == SemanticResult::Pending)
        return true;

    auto& match    = job->cacheMatches[0];
    node->typeInfo = match.symbolOverload->typeInfo;
    SWAG_CHECK(setSymbolMatch(context, identifierRef, node, symbol, match.symbolOverload, &match, dependentVar));
    return true;
}

void SemanticJob::collectScopeHierarchy(SemanticContext* context, vector<Scope*>& scopes, vector<AlternativeScope>& scopesVars, Scope* startScope, uint32_t flags)
{
    auto  job       = context->job;
    auto& here      = job->scopesHere;
    auto& hereNoAlt = job->scopesHereNoAlt;

    here.clear();
    hereNoAlt.clear();
    scopes.clear();
    if (!startScope)
        return;

    scopes.push_back(startScope);
    here.insert(startScope);

    // Can be null because of g_CommandLine.addRuntimeModule to false
    if (g_Workspace.runtimeModule)
    {
        auto runTime = g_Workspace.runtimeModule->scopeRoot;
        scopes.push_back(runTime);
        here.insert(runTime);
        hereNoAlt.insert(runTime);
    }

    for (int i = 0; i < scopes.size(); i++)
    {
        auto scope = scopes[i];

        // Add parent scope
        if (scope->kind != ScopeKind::Inline || !(flags & COLLECT_STOP_INLINE))
        {
            if (scope->parentScope)
            {
                if (here.find(scope->parentScope) == here.end())
                {
                    scopes.push_back(scope->parentScope);
                    here.insert(scope->parentScope);
                }
            }
        }

        // Add current alternative scopes, made by 'using'.
        if (!scope->alternativeScopes.empty())
        {
            // Can we add alternative scopes for that specific scope ?
            if (hereNoAlt.find(scope) == hereNoAlt.end())
            {
                for (int j = 0; j < scope->alternativeScopes.size(); j++)
                {
                    auto altScope = scope->alternativeScopes[j];
                    if (here.find(altScope) == here.end())
                    {
                        scopes.push_back(altScope);
                        here.insert(altScope);
                    }
                }
            }

            scopesVars.insert(scopesVars.end(), scope->alternativeScopesVars.begin(), scope->alternativeScopesVars.end());
        }

        // If we are on a module, add all files
        if (scope->kind == ScopeKind::Module)
        {
            for (auto child : scope->childScopes)
            {
                if (child->name.empty())
                {
                    if (here.find(child) == here.end())
                    {
                        scopes.push_back(child);
                        here.insert(child);
                        hereNoAlt.insert(child);
                    }
                }
            }
        }
    }
}

bool SemanticJob::checkSymbolGhosting(SemanticContext* context, Scope* startScope, AstNode* node, SymbolKind kind)
{
    auto job = context->job;

    // No ghosting from struct
    if (startScope->kind == ScopeKind::Struct)
        return true;

    SemanticJob::collectScopeHierarchy(context, job->cacheScopeHierarchy, job->cacheScopeHierarchyVars, startScope, COLLECT_STOP_INLINE);
    for (auto scope : job->cacheScopeHierarchy)
    {
        // No ghosting with struct
        if (scope->kind == ScopeKind::Struct)
            continue;

        // Do not check if this is the same scope
        if (!scope->symTable || scope == startScope)
            continue;

        // Be sure that symbol is fully resolved, otherwise we cannot check for a ghosting
        {
            auto symbol = scope->symTable->find(node->name);
            if (!symbol)
                continue;
            scoped_lock lock(symbol->mutex);
            if (symbol->cptOverloads)
            {
                job->waitForSymbolNoLock(symbol);
                return true;
            }
        }

        SWAG_CHECK(scope->symTable->checkHiddenSymbol(node, node->typeInfo, kind));
    }

    return true;
}

#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Attribute.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "Generic.h"

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto node                    = static_cast<AstIdentifierRef*>(context->node);
    auto childBack               = node->childs.back();
    node->resolvedSymbolName     = childBack->resolvedSymbolName;
    node->resolvedSymbolOverload = childBack->resolvedSymbolOverload;
    node->typeInfo               = childBack->typeInfo;
    node->name                   = childBack->name;

    // Flag inheritance
    bool isConstExpr = true;
    for (auto child : node->childs)
    {
        if (!(child->flags & AST_CONST_EXPR))
            isConstExpr = false;
    }

    if (isConstExpr)
        node->flags |= AST_CONST_EXPR;

    if (node->resolvedSymbolOverload)
    {
        // Symbol is in fact a constant value : no need for bytecode
        if (node->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE)
        {
            if (childBack->kind != AstNodeKind::ArrayPointerIndex)
            {
                node->computedValue = node->resolvedSymbolOverload->computedValue;
                node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR | AST_NO_BYTECODE_CHILDS;
            }
        }
        else if (node->resolvedSymbolName->kind == SymbolKind::Variable ||
                 node->resolvedSymbolName->kind == SymbolKind::Function)
        {
            node->flags |= AST_L_VALUE;
        }
    }

    return true;
}

void SemanticJob::collectScopeHiearchy(SemanticContext* context, vector<Scope*>& scopes, Scope* startScope)
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
    if (context->sourceFile->module->workspace->runtimeModule)
    {
        auto runTime = context->sourceFile->module->workspace->runtimeModule->scopeRoot;
        scopes.push_back(runTime);
        here.insert(runTime);
        hereNoAlt.insert(runTime);
    }

    for (int i = 0; i < scopes.size(); i++)
    {
        auto scope = scopes[i];

        // Add parent scope
        if (scope->parentScope)
        {
            if (here.find(scope->parentScope) == here.end())
            {
                scopes.push_back(scope->parentScope);
                here.insert(scope->parentScope);
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
        }

        // If we are on a module, add all files
        if (scope->kind == ScopeKind::Module)
        {
            for (auto child : scope->childScopes)
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

bool SemanticJob::checkSymbolGhosting(SemanticContext* context, Scope* startScope, AstNode* node, SymbolKind kind)
{
    auto sourceFile = context->sourceFile;
    auto job        = context->job;

    // No ghosting from struct
    if (startScope->kind == ScopeKind::Struct)
        return true;

    SemanticJob::collectScopeHiearchy(context, job->cacheScopeHierarchy, startScope);
    for (auto scope : job->cacheScopeHierarchy)
    {
        // No ghosting with struct
        if (scope->kind == ScopeKind::Struct)
            continue;

        // Do not check if this is the same scope
        if (!scope->symTable || scope == startScope)
            continue;

        SWAG_CHECK(scope->symTable->checkHiddenSymbol(sourceFile, node->token, node->name, node->typeInfo, kind));
    }

    return true;
}

bool SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstNode* node, SymbolName* symbol, SymbolOverload* overload)
{
    auto sourceFile                = context->sourceFile;
    parent->resolvedSymbolName     = symbol;
    parent->resolvedSymbolOverload = overload;
    parent->previousResolvedNode   = node;
    node->resolvedSymbolName       = symbol;
    node->resolvedSymbolOverload   = overload;
    node->typeInfo                 = node->resolvedSymbolOverload->typeInfo;

    switch (symbol->kind)
    {
    case SymbolKind::Namespace:
        parent->startScope = static_cast<TypeInfoNamespace*>(node->typeInfo)->scope;
        node->flags |= AST_CONST_EXPR;
        break;
    case SymbolKind::Enum:
        parent->startScope = static_cast<TypeInfoEnum*>(node->typeInfo)->scope;
        node->flags |= AST_CONST_EXPR;
        break;
    case SymbolKind::EnumValue:
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        node->computedValue = node->resolvedSymbolOverload->computedValue;
        break;
    case SymbolKind::Struct:
        parent->startScope = static_cast<TypeInfoStruct*>(node->typeInfo)->scope;
        node->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::Variable:
    {
        // Lambda call
        AstIdentifier* identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        auto           typeInfo   = g_TypeMgr.concreteType(identifier->typeInfo);

        if (typeInfo->kind == TypeInfoKind::Lambda && identifier->callParameters)
        {
            // From now this is considered as a function, not a lambda
            auto funcType     = typeInfo->clone();
            funcType->kind    = TypeInfoKind::FuncAttr;
            node->typeInfo    = g_TypeMgr.registerType(funcType);
            node->byteCodeFct = &ByteCodeGenJob::emitLambdaCall;
        }

        // Tuple
        else if (typeInfo->kind == TypeInfoKind::TypeList)
        {
            parent->startScope = static_cast<TypeInfoList*>(typeInfo)->scope;
            node->typeInfo     = typeInfo;
            parent->typeInfo   = typeInfo;
        }

        // Struct
        else if (typeInfo->kind == TypeInfoKind::Struct)
        {
            parent->startScope = static_cast<TypeInfoStruct*>(typeInfo)->scope;
            node->typeInfo     = typeInfo;
            parent->typeInfo   = typeInfo;
        }

        // Pointer
        else if (typeInfo->kind == TypeInfoKind::Pointer)
        {
            auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            if (typePointer->pointedType->kind == TypeInfoKind::Struct)
                parent->startScope = static_cast<TypeInfoStruct*>(typePointer->pointedType)->scope;
            node->typeInfo   = typeInfo;
            parent->typeInfo = typeInfo;
        }

        // Array
        else if (typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            if (typeArray->pointedType->kind == TypeInfoKind::Struct)
                parent->startScope = static_cast<TypeInfoStruct*>(typeArray->pointedType)->scope;
            node->typeInfo   = typeInfo;
            parent->typeInfo = typeInfo;
        }

        break;
    }

    case SymbolKind::Function:
    {

        // This is for a lambda
        if (node->flags & AST_TAKE_ADDRESS)
        {
            // The makePointer will deal with the real make lambda thing
            node->flags |= AST_NO_BYTECODE;
            break;
        }

        node->kind = AstNodeKind::FuncCall;
        node->inheritAndFlag(node->resolvedSymbolOverload->node, AST_CONST_EXPR);

        if (node->token.id == TokenId::Intrinsic)
        {
            node->byteCodeFct = &ByteCodeGenJob::emitIntrinsic;
        }
        else if (overload->node->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            node->byteCodeFct = &ByteCodeGenJob::emitForeignCall;
        }
        else
        {
            node->byteCodeFct = &ByteCodeGenJob::emitLocalCall;
            auto ownerFct     = node->ownerFct;
            if (ownerFct)
            {
                auto myAttributes = ownerFct->attributeFlags;
                if (!(myAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER))
                    return sourceFile->report({sourceFile, node->token, format("can't call compiler function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
                if (!(myAttributes & ATTRIBUTE_TEST) && (overload->node->attributeFlags & ATTRIBUTE_TEST))
                    return sourceFile->report({sourceFile, node->token, format("can't call test function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
            }
        }

        // For a tuple, need to reserve room on the stack for the return result
        auto returnType = g_TypeMgr.concreteType(node->typeInfo);
        if (returnType->kind == TypeInfoKind::TypeList)
        {
            auto fctCall                  = CastAst<AstIdentifier>(node, AstNodeKind::FuncCall);
            fctCall->fctCallStorageOffset = node->ownerScope->startStackSize;
            node->ownerScope->startStackSize += returnType->sizeOf;
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        }

        break;
    }
    }

    return true;
}

bool SemanticJob::checkFuncCall(SemanticContext* context, AstNode* genericParameters, AstNode* callParameters, AstIdentifier* node)
{
    auto  job              = context->job;
    auto  sourceFile       = context->sourceFile;
    auto& matches          = job->cacheMatches;
    auto& genericMatches   = job->cacheGenericMatches;
    auto& badSignature     = job->cacheBadSignature;
    auto& dependentSymbols = job->cacheDependentSymbols;

    matches.clear();
    genericMatches.clear();
    badSignature.clear();

    int numOverloads = 0;
    for (auto oneSymbol : dependentSymbols)
    {
        for (auto overload : oneSymbol->overloads)
        {
            numOverloads++;

            auto typeInfo = static_cast<TypeInfoFuncAttr*>(overload->typeInfo);
            assert(typeInfo->kind == TypeInfoKind::FuncAttr || typeInfo->kind == TypeInfoKind::Lambda);

            typeInfo->match(job->symMatch);

            if (job->symMatch.result == MatchResult::Ok)
            {
                if (overload->flags & OVERLOAD_GENERIC)
                {
                    OneGenericMatch match;
                    match.symbolOverload                  = overload;
                    match.genericMatchesParamsValues = job->symMatch.genericParametersValues;
                    match.genericMatchesParamsTypes  = job->symMatch.genericParametersTypes;
                    genericMatches.emplace_back(match);
                }
                else
                    matches.push_back(overload);
            }
            else if (job->symMatch.result == MatchResult::BadSignature)
            {
                badSignature.push_back(overload);
            }
        }
    }

    // This is a generic
    if (genericMatches.size() == 1 && matches.size() == 0)
    {
		SWAG_CHECK(Generic::InstanciateFunction(context, genericParameters, genericMatches[0]));
        return true;
    }

    auto symbol = dependentSymbols[0];
    if (matches.size() == 0)
    {
        if (numOverloads == 1)
        {
            auto overload = symbol->overloads[0];
            switch (job->symMatch.result)
            {
            case MatchResult::InvalidNamedParameter:
            {
                assert(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{sourceFile, param->namedParamNode, format("unknown named parameter '%s'", param->namedParam.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::DuplicatedNamedParameter:
            {
                assert(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{sourceFile, param->namedParamNode, format("named parameter '%s' already used", param->namedParam.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::NotEnoughParameters:
            {
                Diagnostic diag{sourceFile, callParameters ? callParameters : node, format("not enough parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::TooManyParameters:
            {
                Diagnostic diag{sourceFile, callParameters ? callParameters : node, format("too many parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::BadSignature:
            {
                assert(callParameters);
                Diagnostic diag{sourceFile,
                                callParameters->childs[job->symMatch.badSignatureParameterIdx],
                                format("bad type of parameter '%d' for %s '%s' ('%s' expected, '%s' provided)",
                                       job->symMatch.badSignatureParameterIdx + 1,
                                       SymTable::getNakedKindName(symbol->kind),
                                       symbol->name.c_str(),
                                       job->symMatch.badSignatureRequestedType->name.c_str(),
                                       job->symMatch.badSignatureGivenType->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            }
        }
        else
        {
            if (badSignature.size())
            {
                Diagnostic diag{sourceFile,
                                node->callParameters ? node->callParameters : node,
                                format("none of the %d overloads could convert all the parameters types", numOverloads)};

                vector<const Diagnostic*> notes;
                for (auto overload : badSignature)
                {
                    auto note       = new Diagnostic{overload->sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
                    note->showRange = false;
                    notes.push_back(note);
                }

                return sourceFile->report(diag, notes);
            }
            else
            {
                int         numParams = callParameters ? (int) node->callParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "parameter" : "parameters";
                Diagnostic  diag{sourceFile,
                                callParameters ? callParameters : node,
                                format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return sourceFile->report(diag);
            }
        }
    }

    if (matches.size() > 1)
    {
        Diagnostic diag{sourceFile,
                        callParameters ? callParameters : node,
                        format("ambiguous call to %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};

        vector<const Diagnostic*> notes;
        for (auto overload : matches)
        {
            auto note       = new Diagnostic{overload->sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
            note->showRange = false;
            notes.push_back(note);
        }

        sourceFile->report(diag, notes);
        return false;
    }

    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto  job              = context->job;
    auto& scopeHierarchy   = job->cacheScopeHierarchy;
    auto& dependentSymbols = job->cacheDependentSymbols;
    auto  node             = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto  identifierRef    = node->identifierRef;
    auto  sourceFile       = context->sourceFile;

    // Direct access to a tuple inside value
    if (node->flags & AST_IDENTIFIER_IS_INTEGER)
    {
        SWAG_VERIFY(identifierRef->startScope && identifierRef->typeInfo, sourceFile->report({sourceFile, node->token, "invalid access by literal"}));
        SWAG_VERIFY(identifierRef->typeInfo->kind == TypeInfoKind::TypeList, sourceFile->report({sourceFile, node->token, format("access by literal invalid on type '%s'", identifierRef->typeInfo->name.c_str())}));
        auto index    = stoi(node->name);
        auto typeList = CastTypeInfo<TypeInfoList>(identifierRef->typeInfo, TypeInfoKind::TypeList);
        SWAG_VERIFY(index >= 0 && index < typeList->childs.size(), sourceFile->report({sourceFile, node->token, format("access by literal is out of range (maximum index is '%d')", typeList->childs.size() - 1)}));

        // Compute offset from start of tuple
        int offset = 0;
        for (int i = 0; i < index; i++)
        {
            auto typeInfo = typeList->childs[i];
            offset += typeInfo->sizeOf;
        }

        node->computedValue.reg.u32 = (uint32_t) offset;
        node->typeInfo              = typeList->childs[index];
        identifierRef->typeInfo     = typeList->childs[index];
        return true;
    }

    if (node->semanticState == AstNodeResolveState::ProcessingChilds)
    {
        scopeHierarchy.clear();
        dependentSymbols.clear();
    }

    // Compute dependencies if not already done
    if (dependentSymbols.empty())
    {
        auto startScope = identifierRef->startScope;
        if (!startScope)
        {
            startScope = node->ownerScope;
            collectScopeHiearchy(context, scopeHierarchy, startScope);
        }
        else
            scopeHierarchy.push_back(startScope);

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

        if (job->cacheDependentSymbols.empty())
        {
            if (identifierRef->startScope)
            {
                auto displayName = identifierRef->startScope->fullname;
                if (displayName.empty() && identifierRef->typeInfo)
                    displayName = identifierRef->typeInfo->name;
                if (!displayName.empty())
                    return sourceFile->report({sourceFile, node->token, format("identifier '%s' cannot be found in %s '%s'", node->name.c_str(), Scope::getNakedName(identifierRef->startScope->kind), displayName.c_str())});
            }

            return sourceFile->report({sourceFile, node->token, format("unknown identifier '%s'", node->name.c_str())});
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto symbol : dependentSymbols)
    {
        scoped_lock lkn(symbol->mutex);
        if (symbol->cptOverloads)
        {
            symbol->dependentJobs.push_back(context->job);
            g_ThreadMgr.addPendingJob(context->job);
            context->result = SemanticResult::Pending;
            return true;
        }
    }

    // Fill specified parameters
    job->symMatch.reset();
    job->symMatch.forLambda = (node->flags & AST_TAKE_ADDRESS);

    if (node->callParameters)
    {
        auto symbol = dependentSymbols[0];
        if (symbol->kind != SymbolKind::Attribute &&
            symbol->kind != SymbolKind::Function &&
            (symbol->kind != SymbolKind::Variable || symbol->overloads[0]->typeInfo->kind != TypeInfoKind::Lambda))
        {
            Diagnostic diag{sourceFile, node->callParameters->token, format("identifier '%s' is %s and not a function", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
            Diagnostic note{sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
            return sourceFile->report(diag, &note);
        }

        // If a variable is defined just before the call, then this can be an UFCS (unified function call system)
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind == SymbolKind::Variable)
        {
            auto fctCallParam = Ast::newNode(&g_Pool_astFuncCallParam, AstNodeKind::FuncCallParam, node->sourceFileIdx, nullptr);
            node->callParameters->childs.insert(node->callParameters->childs.begin(), fctCallParam);
            fctCallParam->parent      = node->callParameters;
            fctCallParam->typeInfo    = identifierRef->previousResolvedNode->typeInfo;
            fctCallParam->token       = identifierRef->previousResolvedNode->token;
            fctCallParam->byteCodeFct = &ByteCodeGenJob::emitFuncCallParam;
            Ast::removeFromParent(identifierRef->previousResolvedNode);
            Ast::addChild(fctCallParam, identifierRef->previousResolvedNode);
        }

        if (node->genericParameters)
        {
            int idx = 0;
            for (auto param : node->genericParameters->childs)
            {
                auto oneParam = CastAst<AstFuncCallParam>(param, AstNodeKind::FuncCallParam);
                SWAG_VERIFY(oneParam->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, oneParam, format("generic parameter '%d' cannot be evaluated at compile time", idx + 1)}));
                job->symMatch.genericParameters.push_back(oneParam);
				job->symMatch.genericParametersValues.push_back(oneParam->computedValue);
				job->symMatch.genericParametersTypes.push_back(oneParam->typeInfo);
                idx++;
            }
        }

        for (auto param : node->callParameters->childs)
        {
            auto oneParam = CastAst<AstFuncCallParam>(param, AstNodeKind::FuncCallParam);
            job->symMatch.parameters.push_back(oneParam);
        }
    }
    else
    {
        // For everything except functions and attributes (which have overloads), this is
        // a match
        auto symbol = dependentSymbols[0];
        if (symbol->kind != SymbolKind::Attribute && symbol->kind != SymbolKind::Function)
        {
            SWAG_ASSERT(dependentSymbols.size() == 1);
            SWAG_ASSERT(symbol->overloads.size() == 1);
            SWAG_CHECK(setSymbolMatch(context, identifierRef, node, dependentSymbols[0], dependentSymbols[0]->overloads[0]));
            return true;
        }
    }

    SWAG_CHECK(checkFuncCall(context, node->genericParameters, node->callParameters, node));
	if (context->result == SemanticResult::Pending)
		return true;

    SWAG_CHECK(setSymbolMatch(context, identifierRef, node, job->cacheDependentSymbols[0], job->cacheMatches[0]));
    return true;
}

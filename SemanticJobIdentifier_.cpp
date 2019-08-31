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
        if (child->flags & AST_IS_GENERIC)
            node->flags |= AST_IS_GENERIC;
        if (child->flags & AST_GENERIC_MATCH_WAS_PARTIAL)
            node->flags |= AST_GENERIC_MATCH_WAS_PARTIAL;
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

        // Be sure that symbol is fully resolved, otherwise we cannot check for a ghosting
        {
            auto symbol = scope->symTable->find(node->name);
            if (!symbol)
                continue;
            scoped_lock lock(symbol->mutex);
            if (symbol->cptOverloads)
            {
                job->waitForSymbol(symbol);
                return true;
            }
        }

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

    if (node->typeInfo->flags & TYPEINFO_GENERIC)
        node->flags |= AST_IS_GENERIC;

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

    case SymbolKind::GenericType:
        break;

    case SymbolKind::Variable:
    {
        // Lambda call
        AstIdentifier* identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        auto           typeInfo   = TypeManager::concreteType(identifier->typeInfo);

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
            if (typeArray->rawType->kind == TypeInfoKind::Struct)
                parent->startScope = static_cast<TypeInfoStruct*>(typeArray->rawType)->scope;
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
        node->inheritOrFlag(node->resolvedSymbolOverload->node, AST_CONST_EXPR);

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
                    return context->errorContext.report({sourceFile, node->token, format("can't call compiler function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
                if (!(myAttributes & ATTRIBUTE_TEST) && (overload->node->attributeFlags & ATTRIBUTE_TEST))
                    return context->errorContext.report({sourceFile, node->token, format("can't call test function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
            }
        }

        // For a tuple, need to reserve room on the stack for the return result
        auto returnType = TypeManager::concreteType(node->typeInfo);
        if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            node->fctCallStorageOffset = node->ownerScope->startStackSize;
            node->ownerScope->startStackSize += returnType->sizeOf;
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        }

        break;
    }
    }

    return true;
}

bool SemanticJob::matchIdentifierParameters(SemanticContext* context, AstNode* genericParameters, AstNode* callParameters, AstIdentifier* node)
{
    auto  job                 = context->job;
    auto  sourceFile          = context->sourceFile;
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
            job->waitForSymbol(symbol);
            return true;
        }

        numOverloadsWhenChecked = (int) symbol->overloads.size();
        for (auto overload : symbol->overloads)
        {
            numOverloads++;

            auto rawTypeInfo = overload->typeInfo;

            // If to a type alias that already a generic instance, accept to not have generic
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
                auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
                typeInfo->match(job->symMatch);
            }

            switch (job->symMatch.result)
            {
            case MatchResult::Ok:
                if (overload->flags & OVERLOAD_GENERIC)
                {
                    OneGenericMatch match;
                    match.flags                       = job->symMatch.flags;
                    match.symbolOverload              = overload;
                    match.genericParametersCallTypes  = job->symMatch.genericParametersCallTypes;
                    match.genericParametersCallValues = job->symMatch.genericParametersCallValues;
                    match.genericParametersGenTypes   = job->symMatch.genericParametersGenTypes;
                    genericMatches.emplace_back(match);
                }
                else
                    matches.push_back(overload);
                break;

            case MatchResult::BadGenericSignature:
                badGenericSignature.push_back(overload);
                hasGenericErrors = true;
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
        auto symbol = dependentSymbols[0];
        symbol->mutex.lock();

        // Be sure we don't have more overloads waiting to be solved
        if (symbol->cptOverloads)
        {
            job->waitForSymbol(symbol);
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
            if (genericParameters && !(node->flags & AST_IS_GENERIC))
            {
                SWAG_CHECK(Generic::InstanciateStruct(context, genericParameters, firstMatch));
            }
            else
            {
                matches.push_back(firstMatch.symbolOverload);
                node->flags |= AST_IS_GENERIC;
                if (firstMatch.flags & SymbolMatchContext::MATCH_WAS_PARTIAL)
                    node->flags |= AST_GENERIC_MATCH_WAS_PARTIAL;
            }
        }
        else
        {
            SWAG_CHECK(Generic::InstanciateFunction(context, genericParameters, firstMatch));
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
                Diagnostic diag{sourceFile, param, format("parameter '%d' must be named", job->symMatch.badSignatureParameterIdx + 1)};
                return context->errorContext.report(diag);
            }
            case MatchResult::InvalidNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{sourceFile, param->namedParamNode ? param->namedParamNode : param, format("unknown named parameter '%s'", param->namedParam.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::DuplicatedNamedParameter:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{sourceFile, param->namedParamNode, format("named parameter '%s' already used", param->namedParam.c_str())};
                return context->errorContext.report(diag);
            }
            case MatchResult::NotEnoughParameters:
            {
                Diagnostic diag{sourceFile, callParameters ? callParameters : node, format("not enough parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::TooManyParameters:
            {
                SWAG_ASSERT(callParameters);
                auto       param = static_cast<AstFuncCallParam*>(callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{sourceFile, param, format("too many parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::NotEnoughGenericParameters:
            {
                Diagnostic diag{sourceFile, genericParameters ? genericParameters : node, format("not enough generic parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::TooManyGenericParameters:
            {
                Diagnostic diag{sourceFile, genericParameters ? genericParameters : node, format("too many generic parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
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
                Diagnostic diag{sourceFile,
                                callParameters->childs[job->symMatch.badSignatureParameterIdx],
                                format("bad type of %s for %s '%s' ('%s' expected, '%s' provided)",
                                       parameter.c_str(),
                                       SymTable::getNakedKindName(symbol->kind),
                                       symbol->name.c_str(),
                                       job->symMatch.badSignatureRequestedType->name.c_str(),
                                       job->symMatch.badSignatureGivenType->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            case MatchResult::BadGenericSignature:
            {
                SWAG_ASSERT(genericParameters);
                Diagnostic diag{sourceFile,
                                genericParameters->childs[job->symMatch.badSignatureParameterIdx],
                                format("bad type of generic parameter '%d' for %s '%s' ('%s' expected, '%s' provided)",
                                       job->symMatch.badSignatureParameterIdx + 1,
                                       SymTable::getNakedKindName(symbol->kind),
                                       symbol->name.c_str(),
                                       job->symMatch.badSignatureRequestedType->name.c_str(),
                                       job->symMatch.badSignatureGivenType->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", symbol->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
            }
        }
        else
        {
            if (badSignature.size())
            {
                Diagnostic                diag{sourceFile, callParameters ? callParameters : node, format("none of the %d overloads could convert all the parameters types", numOverloads)};
                vector<const Diagnostic*> notes;
                for (auto overload : badSignature)
                {
                    auto note       = new Diagnostic{overload->sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
                    note->showRange = false;
                    notes.push_back(note);
                }

                return context->errorContext.report(diag, notes);
            }
            else if (badGenericSignature.size())
            {
                Diagnostic                diag{sourceFile, genericParameters ? genericParameters : node, format("none of the %d overloads could convert all the generic parameters types", numOverloads)};
                vector<const Diagnostic*> notes;
                for (auto overload : badGenericSignature)
                {
                    auto note       = new Diagnostic{overload->sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
                    note->showRange = false;
                    notes.push_back(note);
                }

                return context->errorContext.report(diag, notes);
            }
            else if (hasGenericErrors)
            {
                int         numParams = genericParameters ? (int) genericParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "generic parameter" : "generic parameters";
                Diagnostic  diag{sourceFile, genericParameters ? genericParameters : node, format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return context->errorContext.report(diag);
            }
            else
            {
                int         numParams = callParameters ? (int) callParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "parameter" : "parameters";
                Diagnostic  diag{sourceFile, callParameters ? callParameters : node, format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return context->errorContext.report(diag);
            }
        }
    }

    if (matches.size() > 1)
    {
        if (hasGenericErrors)
        {
            Diagnostic                diag{sourceFile, genericParameters ? genericParameters : node, format("ambiguous generic call to %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
            vector<const Diagnostic*> notes;
            for (auto overload : matches)
            {
                auto note       = new Diagnostic{overload->sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
                note->showRange = false;
                notes.push_back(note);
            }

            context->errorContext.report(diag, notes);
        }
        else
        {
            Diagnostic                diag{sourceFile, callParameters ? callParameters : node, format("ambiguous call to %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
            vector<const Diagnostic*> notes;
            for (auto overload : matches)
            {
                auto note       = new Diagnostic{overload->sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
                note->showRange = false;
                notes.push_back(note);
            }

            context->errorContext.report(diag, notes);
        }

        return false;
    }

    return true;
}

bool SemanticJob::resolveTupleAccess(SemanticContext* context, bool& eaten)
{
    auto node          = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    auto identifierRef = node->identifierRef;
    auto sourceFile    = context->sourceFile;

    // Direct access to a tuple inside value
    if (node->flags & AST_IDENTIFIER_IS_INTEGER)
    {
        SWAG_VERIFY(identifierRef->startScope && identifierRef->typeInfo, context->errorContext.report({sourceFile, node->token, "invalid access by literal"}));
        SWAG_VERIFY(identifierRef->typeInfo->kind == TypeInfoKind::TypeList, context->errorContext.report({sourceFile, node->token, format("access by literal invalid on type '%s'", identifierRef->typeInfo->name.c_str())}));
        auto index    = stoi(node->name);
        auto typeList = CastTypeInfo<TypeInfoList>(identifierRef->typeInfo, TypeInfoKind::TypeList);
        SWAG_VERIFY(index >= 0 && index < typeList->childs.size(), context->errorContext.report({sourceFile, node->token, format("access by literal is out of range (maximum index is '%d')", typeList->childs.size() - 1)}));

        // Compute offset from start of tuple
        int offset = 0;
        for (int i = 0; i < index; i++)
        {
            auto typeInfo = typeList->childs[i];
            offset += typeInfo->sizeOf;
        }

        node->computedValue.reg.u32  = (uint32_t) offset;
        node->typeInfo               = typeList->childs[index];
        node->resolvedSymbolName     = identifierRef->previousResolvedNode->resolvedSymbolName;
        node->resolvedSymbolOverload = identifierRef->previousResolvedNode->resolvedSymbolOverload;
        identifierRef->typeInfo      = typeList->childs[index];
        eaten                        = true;
        return true;
    }

    // Access to tuple by name
    if (identifierRef && identifierRef->typeInfo && identifierRef->typeInfo->kind == TypeInfoKind::TypeList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(identifierRef->typeInfo, TypeInfoKind::TypeList);
        int  offset   = 0;
        int  index    = 0;
        for (auto& name : typeList->names)
        {
            if (name == node->name)
            {
                node->computedValue.reg.u32  = (uint32_t) offset;
                node->typeInfo               = typeList->childs[index];
                node->resolvedSymbolName     = identifierRef->previousResolvedNode->resolvedSymbolName;
                node->resolvedSymbolOverload = identifierRef->previousResolvedNode->resolvedSymbolOverload;
                identifierRef->typeInfo      = typeList->childs[index];
                eaten                        = true;
                node->flags |= AST_IDENTIFIER_IS_INTEGER;
                return true;
            }

            auto typeInfo = typeList->childs[index];
            offset += typeInfo->sizeOf;
            index++;
        }
    }

    eaten = false;
    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    // Direct access to a tuple inside value
    bool eatenByTyple = false;
    SWAG_CHECK(resolveTupleAccess(context, eatenByTyple));
    if (eatenByTyple)
        return true;

    auto  job              = context->job;
    auto& scopeHierarchy   = job->cacheScopeHierarchy;
    auto& dependentSymbols = job->cacheDependentSymbols;
    auto  node             = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    auto  identifierRef    = node->identifierRef;
    auto  sourceFile       = context->sourceFile;

    // Already solved
    if ((node->flags & AST_FROM_GENERIC) && node->typeInfo)
    {
        SWAG_CHECK(setSymbolMatch(context, identifierRef, node, node->resolvedSymbolName, node->resolvedSymbolOverload));
        return true;
    }

    if (node->name == "Self")
    {
        SWAG_VERIFY(node->ownerStructScope, context->errorContext.report({sourceFile, node->token, "type 'Self' cannot be used outside an 'impl' block"}));
        node->name = node->ownerStructScope->name;
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
        {
            scopeHierarchy.push_back(startScope);
            scopeHierarchy.insert(scopeHierarchy.end(), startScope->alternativeScopes.begin(), startScope->alternativeScopes.end());
        }

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
                if (identifierRef->startScope->name.empty() && identifierRef->typeInfo)
                    displayName = identifierRef->typeInfo->name;
                if (!displayName.empty())
                    return context->errorContext.report({sourceFile, node->token, format("identifier '%s' cannot be found in %s '%s'", node->name.c_str(), Scope::getNakedName(identifierRef->startScope->kind), displayName.c_str())});
            }

            return context->errorContext.report({sourceFile, node->token, format("unknown identifier '%s'", node->name.c_str())});
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto symbol : dependentSymbols)
    {
        scoped_lock lkn(symbol->mutex);
        if (symbol->cptOverloads)
        {
            // If a structrure is referencing itself, we will match the incomplete symbol for now
            if (symbol->kind == SymbolKind::Struct && node->ownerStruct && node->ownerStruct->name == symbol->name)
            {
                int a;
                a = 0;
            }
            else
            {
                job->waitForSymbol(symbol);
                return true;
            }
        }
    }

    // Fill specified parameters
    job->symMatch.reset();
    if (node->flags & AST_TAKE_ADDRESS)
        job->symMatch.flags |= SymbolMatchContext::MATCH_FOR_LAMBDA;

    // If a variable is defined just before the call, then this can be an UFCS (unified function call system)
    if (!(node->flags & AST_UFCS_DONE))
    {
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind == SymbolKind::Variable)
        {
            auto symbol = dependentSymbols[0];
            if (symbol->kind == SymbolKind::Function)
            {
                if (!node->callParameters)
                {
                    node->callParameters        = Ast::newNode(nullptr, &g_Pool_astNode, AstNodeKind::FuncCallParameters, node->sourceFileIdx, node);
                    node->callParameters->token = identifierRef->previousResolvedNode->token;
                }

                node->flags |= AST_UFCS_DONE;
                auto fctCallParam = Ast::newNode(nullptr, &g_Pool_astFuncCallParam, AstNodeKind::FuncCallParam, node->sourceFileIdx, nullptr);
                node->callParameters->childs.insert(node->callParameters->childs.begin(), fctCallParam);
                fctCallParam->parent      = node->callParameters;
                fctCallParam->typeInfo    = identifierRef->previousResolvedNode->typeInfo;
                fctCallParam->token       = identifierRef->previousResolvedNode->token;
                fctCallParam->byteCodeFct = &ByteCodeGenJob::emitFuncCallParam;
                Ast::removeFromParent(identifierRef->previousResolvedNode);
                Ast::addChild(fctCallParam, identifierRef->previousResolvedNode);
            }
        }
    }

    auto  genericParameters = node->genericParameters;
    auto  callParameters    = node->callParameters;
    auto  symbol            = dependentSymbols[0];
    auto& symMatch          = job->symMatch;

    if (callParameters)
    {
        if (symbol->kind != SymbolKind::Attribute &&
            symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            symbol->kind != SymbolKind::TypeAlias &&
            (symbol->kind != SymbolKind::Variable || symbol->overloads[0]->typeInfo->kind != TypeInfoKind::Lambda))
        {
            Diagnostic diag{sourceFile, callParameters->token, format("identifier '%s' is %s and not a function", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
            Diagnostic note{sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
            return context->errorContext.report(diag, &note);
        }

        for (auto param : callParameters->childs)
        {
            auto oneParam = CastAst<AstFuncCallParam>(param, AstNodeKind::FuncCallParam);
            symMatch.parameters.push_back(oneParam);
        }
    }

    if (genericParameters)
    {
        node->inheritOrFlag(genericParameters, AST_IS_GENERIC);
        if (symbol->kind != SymbolKind::Function &&
            symbol->kind != SymbolKind::Struct &&
            symbol->kind != SymbolKind::TypeAlias)
        {
            Diagnostic diag{sourceFile, callParameters->token, format("invalid generic parameters, identifier '%s' is %s and not a function or a structure", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
            Diagnostic note{sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
            return context->errorContext.report(diag, &note);
        }

        int idx = 0;
        for (auto param : genericParameters->childs)
        {
            auto oneParam = CastAst<AstFuncCallParam>(param, AstNodeKind::FuncCallParam, AstNodeKind::IdentifierRef);
            symMatch.genericParameters.push_back(oneParam);
            symMatch.genericParametersCallValues.push_back(oneParam->computedValue);
            symMatch.genericParametersCallTypes.push_back(oneParam->typeInfo);
            idx++;
        }
    }

    if (symMatch.parameters.empty() && symMatch.genericParameters.empty())
    {
        // For everything except functions/attributes/structs (which have overloads), this is a match
        if (symbol->kind != SymbolKind::Attribute && symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Struct)
        {
            SWAG_ASSERT(dependentSymbols.size() == 1);
            SWAG_ASSERT(symbol->overloads.size() == 1);
            auto overload  = dependentSymbols[0]->overloads[0];
            node->typeInfo = overload->typeInfo;
            SWAG_CHECK(setSymbolMatch(context, identifierRef, node, dependentSymbols[0], dependentSymbols[0]->overloads[0]));
            return true;
        }
    }

    SWAG_CHECK(matchIdentifierParameters(context, genericParameters, callParameters, node));
    if (context->result == SemanticResult::Pending)
        return true;

    auto overload  = job->cacheMatches[0];
    node->typeInfo = overload->typeInfo;
    SWAG_CHECK(setSymbolMatch(context, identifierRef, node, job->cacheDependentSymbols[0], job->cacheMatches[0]));
    return true;
}

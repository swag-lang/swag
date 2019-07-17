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
#include "Workspace.h"

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto           node       = static_cast<AstIdentifierRef*>(context->node);
    AstIdentifier* identifier = static_cast<AstIdentifier*>(node->childs.back());

    node->resolvedSymbolName     = identifier->resolvedSymbolName;
    node->resolvedSymbolOverload = identifier->resolvedSymbolOverload;
    node->typeInfo               = identifier->typeInfo;
    node->name                   = move(identifier->name);

    // Flag inheritance
    bool isConstExpr = true;
    for (auto child : node->childs)
    {
        if (!(child->flags & AST_CONST_EXPR))
            isConstExpr = false;
    }

    if (isConstExpr)
        node->flags |= AST_CONST_EXPR;

    // Symbol is in fact a constant value : no need for bytecode
    if (node->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE)
    {
        node->computedValue = node->resolvedSymbolOverload->computedValue;
        node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR | AST_NO_BYTECODE_CHILDS;
    }
    else if (node->resolvedSymbolName->kind == SymbolKind::GlobalVar || node->resolvedSymbolName->kind == SymbolKind::Variable)
    {
        node->flags |= AST_L_VALUE;
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

    SemanticJob::collectScopeHiearchy(context, job->cacheScopeHierarchy, startScope);
    for (auto scope : job->cacheScopeHierarchy)
    {
        if (scope->symTable && scope != startScope)
            SWAG_CHECK(scope->symTable->checkHiddenSymbol(sourceFile, node->token, node->name, node->typeInfo, kind));
    }

    return true;
}

bool SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstNode* node, SymbolName* symbol, SymbolOverload* overload)
{
    auto sourceFile              = context->sourceFile;
    node->resolvedSymbolName     = symbol;
    node->resolvedSymbolOverload = overload;
    node->typeInfo               = node->resolvedSymbolOverload->typeInfo;

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
    case SymbolKind::Function:
    {
        node->kind = AstNodeKind::FuncCall;
        node->inheritAndFlag(node->resolvedSymbolOverload->node, AST_CONST_EXPR);

        if (node->token.id == TokenId::IntrisicPrint || node->token.id == TokenId::IntrisicAssert)
            node->byteCodeFct = &ByteCodeGenJob::emitIntrinsic;
        else if (overload->node->attributeFlags & ATTRIBUTE_FOREIGN)
            node->byteCodeFct = &ByteCodeGenJob::emitForeignCall;
        else
        {
            node->byteCodeFct = &ByteCodeGenJob::emitLocalCall;

            auto ownerFct = node->ownerFct;
            if (ownerFct)
            {
                auto myAttributes = ownerFct->attributeFlags;
                if (!(myAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER))
                    return sourceFile->report({sourceFile, node->token, format("can't call compiler function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
                if (!(myAttributes & ATTRIBUTE_TEST) && (overload->node->attributeFlags & ATTRIBUTE_TEST))
                    return sourceFile->report({sourceFile, node->token, format("can't call test function '%s' from '%s'", overload->node->name.c_str(), ownerFct->name.c_str())});
            }
        }
        break;
    }
    }

    // Clear cache for the next symbol resolution
    context->job->cacheScopeHierarchy.clear();
    context->job->cacheDependentSymbols.clear();
    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto  job              = context->job;
    auto& scopeHierarchy   = job->cacheScopeHierarchy;
    auto& dependentSymbols = job->cacheDependentSymbols;
    auto  node             = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto  parent           = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);
    auto  sourceFile       = context->sourceFile;

    // Compute dependencies if not already done
    if (job->cacheDependentSymbols.empty())
    {
        dependentSymbols.clear();
        auto startScope = parent->startScope;
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
            if (parent->startScope)
                return sourceFile->report({sourceFile, node->token, format("identifier '%s' can't be found in %s '%s'", node->name.c_str(), Scope::getNakedName(parent->startScope->kind), parent->startScope->fullname.c_str())});
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
    if (node->callParameters)
    {
        auto symbol = dependentSymbols[0];
        if (symbol->kind != SymbolKind::Attribute && symbol->kind != SymbolKind::Function)
        {
            Diagnostic diag{sourceFile, node->callParameters->token, format("identifier '%s' is %s and not a function", node->name.c_str(), SymTable::getArticleKindName(symbol->kind))};
            Diagnostic note{sourceFile, symbol->defaultOverload.node->token.startLocation, symbol->defaultOverload.node->token.endLocation, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
            return sourceFile->report(diag, &note);
        }

        for (auto param : node->callParameters->childs)
        {
            auto oneParam = CastAst<AstFuncCallParam>(param, AstNodeKind::FuncCallOneParam);
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
            SWAG_CHECK(setSymbolMatch(context, parent, node, dependentSymbols[0], dependentSymbols[0]->overloads[0]));
            return true;
        }
    }

    job->cacheMatches.clear();
    job->cacheBadSignature.clear();
    auto& matches      = job->cacheMatches;
    auto& badSignature = job->cacheBadSignature;
    int   numOverloads = 0;
    for (auto symbol : dependentSymbols)
    {
        for (auto overload : symbol->overloads)
        {
            numOverloads++;

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
            typeInfo->match(job->symMatch);
            if (job->symMatch.result == MatchResult::Ok)
                matches.push_back(overload);
            else if (job->symMatch.result == MatchResult::BadSignature)
                badSignature.push_back(overload);
        }
    }

    if (matches.size() == 0)
    {
        auto symbol = dependentSymbols[0];
        if (numOverloads == 1)
        {
            auto overload = symbol->overloads[0];
            switch (job->symMatch.result)
            {
            case MatchResult::InvalidNamedParameter:
            {
                auto       param = static_cast<AstFuncCallParam*>(node->callParameters->childs[job->symMatch.badSignatureParameterIdx]);
                Diagnostic diag{sourceFile, param, format("bad named parameter '%s'", param->namedParam.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::NotEnoughParameters:
            {
                Diagnostic diag{sourceFile, node->callParameters ? node->callParameters : node, format("not enough parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::TooManyParameters:
            {
                Diagnostic diag{sourceFile, node->callParameters, format("too many parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::BadSignature:
            {
                Diagnostic diag{sourceFile,
                                node->callParameters->childs[job->symMatch.badSignatureParameterIdx],
                                format("bad type of parameter '%d' for %s '%s' ('%s' expected, '%s' provided)",
									   job->symMatch.badSignatureParameterIdx,
                                       SymTable::getNakedKindName(symbol->kind),
                                       symbol->name.c_str(),
									   job->symMatch.badSignatureRequestedType->name.c_str(),
									   job->symMatch.badSignatureGivenType->name.c_str())};
                Diagnostic note{overload->sourceFile, overload->node->token, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
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
                int         numParams = node->callParameters ? (int) node->callParameters->childs.size() : 0;
                const char* args      = numParams <= 1 ? "parameter" : "parameters";
                Diagnostic  diag{sourceFile,
                                node->callParameters ? node->callParameters : node,
                                format("no overloaded %s '%s' takes %d %s", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str(), numParams, args)};
                return sourceFile->report(diag);
            }
        }
    }

    if (matches.size() > 1)
    {
        auto       symbol = dependentSymbols[0];
        Diagnostic diag{sourceFile,
                        node->callParameters ? node->callParameters : node,
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

    SWAG_CHECK(setSymbolMatch(context, parent, node, dependentSymbols[0], matches[0]));
    return true;
}

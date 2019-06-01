#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "PoolFactory.h"
#include "SourceFile.h"

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto           node          = static_cast<AstIdentifierRef*>(context->node);
    AstIdentifier* identifier    = static_cast<AstIdentifier*>(node->childs.back());
    node->resolvedSymbolName     = identifier->resolvedSymbolName;
    node->resolvedSymbolOverload = identifier->resolvedSymbolOverload;
    node->typeInfo               = identifier->typeInfo;
    node->name                   = move(identifier->name);

    if (node->resolvedSymbolName->kind == SymbolKind::EnumValue)
    {
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        node->computedValue = node->resolvedSymbolOverload->computedValue;
    }

    return true;
}

void SemanticJob::collectScopeHiearchy(vector<Scope*>& scopes, Scope* startScope)
{
    while (startScope)
    {
        scopes.push_back(startScope);
        startScope = startScope->parentScope;
    }
}

bool SemanticJob::checkSymbolGhosting(SourceFile* sourceFile, Scope* startScope, AstNode* node, SymbolKind kind)
{
    // We need to check that the namespace name is not defined in the scope hierarchy, to avoid
    // symbol ghosting
    vector<Scope*> scopes;
    SemanticJob::collectScopeHiearchy(scopes, startScope);
    for (auto scope : scopes)
    {
        SWAG_CHECK(scope->symTable->checkHiddenSymbol(sourceFile, node->token, node->name, node->typeInfo, kind));
        scope = scope->parentScope;
    }

    return true;
}

void SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstNode* node, SymbolName* symbol, SymbolOverload* overload)
{
    node->resolvedSymbolName     = symbol;
    node->resolvedSymbolOverload = symbol->overloads[0];
    node->typeInfo               = node->resolvedSymbolOverload->typeInfo;
    switch (node->typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        parent->startScope = static_cast<TypeInfoNamespace*>(node->typeInfo)->scope;
        break;
    case TypeInfoKind::Enum:
        parent->startScope = static_cast<TypeInfoEnum*>(node->typeInfo)->scope;
        break;
    }

    // Clear cache for the next symbol resolution
    context->job->scopeHierarchy.clear();
    context->job->dependentSymbols.clear();
    context->result = SemanticResult::Done;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto  job              = context->job;
    auto& scopeHierarchy   = job->scopeHierarchy;
    auto& dependentSymbols = job->dependentSymbols;
    auto  node             = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto  parent           = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);
    auto  sourceFile       = context->sourceFile;

    // Compute dependencies if not already done
    if (job->dependentSymbols.empty())
    {
        auto startScope = parent->startScope;
        if (!startScope)
            startScope = node->scope;
        scopeHierarchy.clear();
        dependentSymbols.clear();
        collectScopeHiearchy(scopeHierarchy, startScope);
        for (auto scope : scopeHierarchy)
        {
            if (scope->symTable)
            {
                scoped_lock lk(scope->symTable->mutex);
                auto        symbol = scope->symTable->findNoLock(node->name);
                if (symbol)
                {
                    dependentSymbols.push_back(symbol);
                    if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Attribute)
                        break;
                }
            }
        }

        if (job->dependentSymbols.empty())
            return sourceFile->report({sourceFile, node->token, format("unknown identifier '%s'", node->name.c_str())});
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

    context->result = SemanticResult::Done;

    // Fill specified parameters
    SymbolMatchContext symMatch;
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
            auto matchParam      = sourceFile->poolFactory->symbolMatchParameter.alloc();
            matchParam->name     = param->name;
            matchParam->typeInfo = param->typeInfo;
            symMatch.parameters.push_back(matchParam);
        }
    }
    else
    {
        // For everything except functions and attributes (which have overloads), this is
        // a match
        auto symbol = dependentSymbols[0];
        if (symbol->kind != SymbolKind::Attribute && symbol->kind != SymbolKind::Function)
        {
            assert(dependentSymbols.size() == 1);
            assert(symbol->overloads.size() == 1);
            setSymbolMatch(context, parent, node, dependentSymbols[0], dependentSymbols[0]->overloads[0]);
            return true;
        }
    }

    vector<SymbolOverload*> matches;
    vector<SymbolOverload*> badSignature;
    int                     numOverloads = 0;
    for (auto symbol : dependentSymbols)
    {
        for (auto overload : symbol->overloads)
        {
            numOverloads++;

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FunctionAttribute);
            typeInfo->match(symMatch);
            if (symMatch.result == MatchResult::Ok)
                matches.push_back(overload);
            else if (symMatch.result == MatchResult::BadSignature)
                badSignature.push_back(overload);
        }
    }

    if (matches.size() == 0)
    {
        auto symbol = dependentSymbols[0];
        if (numOverloads == 1)
        {
            auto overload = symbol->overloads[0];
            switch (symMatch.result)
            {
            case MatchResult::NotEnoughParameters:
            {
                Diagnostic diag{sourceFile, node->callParameters ? node->callParameters : node, format("not enough parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{sourceFile, overload->node->token, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::TooManyParameters:
            {
                Diagnostic diag{sourceFile, node->callParameters, format("too many parameters for %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
                Diagnostic note{sourceFile, overload->node->token, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            case MatchResult::BadSignature:
            {
                Diagnostic diag{sourceFile,
                                node->callParameters,
                                format("bad type of parameter '%d' for %s '%s' ('%s' expected, '%s' provided)",
                                       symMatch.badSignatureParameterIdx,
                                       SymTable::getNakedKindName(symbol->kind),
                                       symbol->name.c_str(),
                                       symMatch.basSignatureRequestedType->name.c_str(),
                                       symMatch.basSignatureGivenType->name.c_str())};
                Diagnostic note{sourceFile, overload->node->token, format("this is the definition of '%s'", node->name.c_str()), DiagnosticLevel::Note};
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
                    auto note = new Diagnostic{sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
                    notes.push_back(note);
                }

                return sourceFile->report(diag, notes);
            }
            else
            {
                int         numParams = node->callParameters ? (int) node->callParameters->childs.size() : 0;
                const char* args      = numParams == 1 ? "parameter" : "parameters";
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
                        format("ambiguous call to overloaded %s '%s'", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};

        vector<const Diagnostic*> notes;
        for (auto overload : matches)
        {
            auto note = new Diagnostic{sourceFile, overload->node->token, "could be", DiagnosticLevel::Note};
            notes.push_back(note);
        }

        sourceFile->report(diag, notes);
        return false;
    }

    setSymbolMatch(context, parent, node, dependentSymbols[0], matches[0]);
    return true;
}

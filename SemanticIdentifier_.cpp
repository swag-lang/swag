#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Scope.h"

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
            scoped_lock lk(scope->symTable->mutex);
            auto        symbol = scope->symTable->findNoLock(node->name);
            if (symbol)
            {
                dependentSymbols.push_back(symbol);
                if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Attribute)
                    break;
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

    // Fill the search type
    TypeInfoFuncAttr searchType;
    if (node->callParameters)
    {
        for (auto param : node->callParameters->childs)
            searchType.parameters.push_back(param->typeInfo);
    }

    for (auto symbol : dependentSymbols)
    {
        for (auto overload : symbol->overloads)
        {
        }
    }

	setSymbolMatch(context, parent, node, dependentSymbols[0], dependentSymbols[0]->overloads[0]);
	return true;
}

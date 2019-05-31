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

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto node       = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto parent     = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);
    auto sourceFile = context->sourceFile;

    // If node->matchScope is defined, no need to rescan the scope hiearchy, as it is already
    // the scope where we have found the symbol in the first place (if it is defined, it means
    // that we come from a symbol wakeup)
    auto scope = node->matchScope;
    if (!scope)
        scope = parent->startScope;
    if (!scope)
        scope = node->scope;

    // Fill the search type
    TypeInfoFuncAttr searchType;
    if (node->callParameters)
    {
        for (auto param : node->callParameters->childs)
            searchType.parameters.push_back(param->typeInfo);
    }

    while (scope)
    {
        auto symTable = scope->symTable;
        {
            scoped_lock lk(symTable->mutex);
            auto        symbol = symTable->findNoLock(node->name);
            if (symbol)
            {
                scoped_lock lkn(symbol->mutex);

                node->matchScope = scope;
                if (!symbol->overloads.empty())
                {
                    auto overload = symbol->findOverload(&searchType);

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

                    context->result = SemanticResult::Done;
                    return true;
                }

                // Need to wait for the symbol to be resolved
                symbol->dependentJobs.push_back(context->job);
                g_ThreadMgr.addPendingJob(context->job);
                context->result = SemanticResult::Pending;
                return true;
            }
        }

        scope = scope->parentScope;
    }

    return sourceFile->report({sourceFile, node->token, format("unknown identifier '%s'", node->name.c_str())});
}

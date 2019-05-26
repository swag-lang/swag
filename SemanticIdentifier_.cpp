#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "Scope.h"

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto           node          = static_cast<AstIdentifierRef*>(context->node);
    AstIdentifier* identifier    = static_cast<AstIdentifier*>(node->childs.back());
    node->resolvedSymbolName     = identifier->resolvedSymbolName;
    node->resolvedSymbolOverload = identifier->resolvedSymbolOverload;
    node->typeInfo               = identifier->typeInfo;
    node->token.startLocation    = node->childs.front()->token.startLocation;
    node->token.endLocation      = node->childs.back()->token.startLocation;
    node->name                   = move(identifier->name);
    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto node       = static_cast<AstIdentifier*>(context->node);
    auto parent     = static_cast<AstIdentifierRef*>(node->parent);
    auto sourceFile = context->sourceFile;

    // If node->matchScope is defined, no need to rescan the scope hiearchy, as it is already
    // the scope where we have found the symbol in the first place (if it is defined, it means
    // that we come from a symbol wakeup)
    auto scope = node->matchScope;
    if (!scope)
        scope = parent->startScope;
    if (!scope)
        scope = node->scope;

    while (scope)
    {
        auto symTable = scope->symTable;
        {
            scoped_lock lk(symTable->mutex);
            auto        name = symTable->findNoLock(node->name);
            if (name)
            {
                scoped_lock lkn(name->mutex);
                node->matchScope = scope;
                if (!name->overloads.empty())
                {
                    node->resolvedSymbolName     = name;
                    node->resolvedSymbolOverload = name->overloads[0];
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
                name->dependentJobs.push_back(context->job);
                context->result = SemanticResult::Pending;
                return true;
            }
        }

        scope = scope->parentScope;
    }

    return sourceFile->report({sourceFile, node->token, format("unknown identifier '%s'", node->name.c_str())});
}

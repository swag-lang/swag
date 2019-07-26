#include "pch.h"
#include "SemanticJob.h"
#include "SymTable.h"
#include "Global.h"
#include "Scope.h"
#include "Ast.h"
#include "TypeInfo.h"

bool SemanticJob::resolveNamespace(SemanticContext* context)
{
    auto node = context->node;
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Namespace));
    return true;
}

bool SemanticJob::resolveUsing(SemanticContext* context)
{
    auto job   = context->job;
    auto node  = context->node;
    auto idref = CastAst<AstIdentifierRef>(node->childs[0], AstNodeKind::IdentifierRef);

	node->flags |= AST_NO_BYTECODE;

    Scope* scope = nullptr;
    switch (idref->resolvedSymbolOverload->typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
    {
        auto typeInfo = static_cast<TypeInfoNamespace*>(idref->resolvedSymbolOverload->typeInfo);
        scope         = typeInfo->scope;
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto typeInfo = static_cast<TypeInfoEnum*>(idref->resolvedSymbolOverload->typeInfo);
        scope         = typeInfo->scope;
        break;
    }
    default:
        return job->error(context, "invalid 'using' reference");
    }

    node->ownerScope->alternativeScopes.push_back(scope);
    return true;
}

bool SemanticJob::resolveScopedStmtBefore(SemanticContext* context)
{
    auto node                        = context->node;
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

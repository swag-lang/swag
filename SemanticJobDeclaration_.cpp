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
    SWAG_VERIFY(idref->resolvedSymbolName->kind == SymbolKind::Namespace, job->error(context, "using must reference a namespace"));
    auto typeInfo = CastTypeInfo<TypeInfoNamespace>(idref->resolvedSymbolOverload->typeInfo, TypeInfoKind::Namespace);
    node->ownerScope->alternativeScopes.push_back(typeInfo->scope);
    return true;
}

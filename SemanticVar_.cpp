#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Scope.h"
#include "TypeManager.h"
#include "ModuleSemanticJob.h"
#include "SymTable.h"

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto node = static_cast<AstVarDecl*>(context->node);

    // Find type
    if (node->astType && node->astAssignment)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->astType->typeInfo, node->astAssignment));
        node->typeInfo = node->astType->typeInfo;
    }
    else if (node->astAssignment)
    {
        node->typeInfo = node->astAssignment->typeInfo;
    }
    else
    {
        node->typeInfo = node->astType->typeInfo;
    }

    // Register symbol with its type
    auto overload = node->scope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Variable);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context->sourceFile, node->scope->parentScope, node, SymbolKind::Variable));

    // Attributes
    if (context->node->attributes)
    {
        collectAttributes(context, overload->attributes, context->node->attributes, context->node, AstNodeKind::VarDecl);
    }

    context->result = SemanticResult::Done;
    return true;
}

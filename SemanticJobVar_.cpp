#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Scope.h"
#include "TypeManager.h"
#include "ModuleSemanticJob.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "SourceFile.h"

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = static_cast<AstVarDecl*>(context->node);

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
    else if (node->astType)
    {
        node->typeInfo = node->astType->typeInfo;
    }

    SWAG_VERIFY(node->typeInfo, sourceFile->report({sourceFile, node->token, format("unable to deduce type of variable '%s'", node->name.c_str())}));

    // Register symbol with its type
    auto overload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Variable);
    SWAG_CHECK(overload);
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Variable));

    // Attributes
    if (context->node->attributes)
    {
        collectAttributes(context, overload->attributes, node->attributes, context->node, AstNodeKind::VarDecl, node->attributeFlags);
    }

    context->result = SemanticResult::Done;
    return true;
}

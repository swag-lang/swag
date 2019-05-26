#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Scope.h"
#include "TypeManager.h"
#include "ModuleSemanticJob.h"

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto node = static_cast<AstVarDecl*>(context->node);
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
    SWAG_CHECK(node->scope->symTable->addSymbolTypeInfo(context->sourceFile, node->token, node->name, node->typeInfo, SymbolKind::Variable));

    // We need to check the scope hierarchy for symbol ghosting
    auto scope = node->scope->parentScope;
    while (scope)
    {
        SWAG_CHECK(scope->symTable->checkHiddenSymbol(context->sourceFile, node->token, node->name, node->typeInfo, SymbolKind::Variable));
        scope = scope->parentScope;
    }

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveEnumType(SemanticContext* context)
{
    auto enumNode = context->node->parent;

    // Enum type
    auto rawTypeInfo = &g_TypeInfoS32;

    // Register symbol with its type
    auto typeInfo     = static_cast<TypeInfoEnum*>(enumNode->typeInfo);
    typeInfo->rawType = rawTypeInfo;
    SWAG_CHECK(enumNode->scope->symTable->addSymbolTypeInfo(context->sourceFile, enumNode->token, enumNode->name, enumNode->typeInfo, SymbolKind::Enum));

	context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveEnumValue(SemanticContext* context)
{
    auto valNode  = context->node;
    auto enumNode = valNode->parent;
    auto typeEnum = static_cast<TypeInfoEnum*>(enumNode->typeInfo);

    SWAG_CHECK(enumNode->scope->symTable->addSymbolTypeInfo(context->sourceFile, valNode->token, valNode->name, typeEnum, SymbolKind::EnumValue));

	context->result = SemanticResult::Done;
    return true;
}
#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Module.h"

bool SemanticJob::resolveNamespace(SemanticContext* context)
{
    auto node = context->node;
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node, SymbolKind::Namespace));
    return true;
}

bool SemanticJob::resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar)
{
    auto node    = context->node;
    auto regNode = node->ownerScope ? node->ownerScope->owner : node;

    SWAG_ASSERT(regNode);
    SWAG_VERIFY(node->ownerFct || node->ownerScope->kind == ScopeKind::Struct, context->report({node, format("'using' on a variable cannot be used in '%s' scope", Scope::getNakedKindName(node->ownerScope->kind))}));

    typeInfoVar = TypeManager::concreteReference(typeInfoVar);
    if (typeInfoVar->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfoVar, TypeInfoKind::Struct);
        {
            SWAG_RACE_CONDITION_WRITE(regNode->raceConditionAlternativeScopes);
            regNode->alternativeScopes.push_back(typeStruct->scope);
            regNode->alternativeScopesVars.push_back({varNode, typeStruct->scope});
        }
    }
    else if (typeInfoVar->kind == TypeInfoKind::Pointer)
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfoVar, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePointer->finalType->kind != TypeInfoKind::TypeSet, context->report({node, node->token, "'using' cannot be used on a typeset variable"}));
        SWAG_VERIFY(typePointer->finalType->kind != TypeInfoKind::Enum, context->report({node, node->token, "'using' cannot be used on an enum variable"}));

        SWAG_VERIFY(typePointer->ptrCount == 1, context->report({node, node->token, format("'using' cannot be used on a variable of type '%s'", typePointer->name.c_str())}));
        SWAG_VERIFY(typePointer->finalType->kind == TypeInfoKind::Struct, context->report({node, node->token, format("'using' cannot be used on a variable of type '%s'", typeInfoVar->name.c_str())}));
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typePointer->finalType, TypeInfoKind::Struct, TypeInfoKind::TypeSet);
        {
            SWAG_RACE_CONDITION_WRITE(regNode->raceConditionAlternativeScopes);
            regNode->alternativeScopes.push_back(typeStruct->scope);
            regNode->alternativeScopesVars.push_back({varNode, typeStruct->scope});
        }
    }
    else
    {
        return context->report({node, format("'using' cannot be used on a variable of type '%s'", node->typeInfo->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveUsing(SemanticContext* context)
{
    auto job   = context->job;
    auto node  = context->node;
    auto idref = CastAst<AstIdentifierRef>(node->childs[0], AstNodeKind::IdentifierRef);

    node->flags |= AST_NO_BYTECODE;

    if (idref->resolvedSymbolName->kind == SymbolKind::Variable)
    {
        SWAG_CHECK(resolveUsingVar(context, node->childs.front(), idref->resolvedSymbolOverload->typeInfo));
        return true;
    }

    Scope* scope        = nullptr;
    auto   typeResolved = idref->resolvedSymbolOverload->typeInfo;
    switch (typeResolved->kind)
    {
    case TypeInfoKind::Namespace:
    {
        auto typeInfo = CastTypeInfo<TypeInfoNamespace>(typeResolved, typeResolved->kind);
        scope         = typeInfo->scope;
        SWAG_RACE_CONDITION_WRITE(node->parent->raceConditionAlternativeScopes);
        node->parent->alternativeScopes.push_back(scope);
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto typeInfo = CastTypeInfo<TypeInfoEnum>(typeResolved, typeResolved->kind);
        scope         = typeInfo->scope;
        SWAG_RACE_CONDITION_WRITE(node->parent->raceConditionAlternativeScopes);
        node->parent->alternativeScopes.push_back(scope);
        break;
    }
    case TypeInfoKind::Struct:
    {
        auto typeInfo = CastTypeInfo<TypeInfoStruct>(typeResolved, typeResolved->kind);
        scope         = typeInfo->scope;
        SWAG_RACE_CONDITION_WRITE(node->parent->raceConditionAlternativeScopes);
        node->parent->alternativeScopes.push_back(scope);
        break;
    }
    default:
        return job->error(context, format("'using' cannot be used on type %s", TypeInfo::getNakedKindName(typeResolved)));
    }

    return true;
}

bool SemanticJob::resolveScopedStmtBefore(SemanticContext* context)
{
    auto node                        = context->node;
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    node->byteCodeAfterFct           = ByteCodeGenJob::emitLeaveScope;
    return true;
}

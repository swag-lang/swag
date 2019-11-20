#include "pch.h"
#include "SemanticJob.h"
#include "SymTable.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"

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
    SWAG_VERIFY(node->ownerFct, context->report({node, format("'using' on a variable cannot be used in '%s' scope", Scope::getNakedKindName(node->ownerScope->kind))}));
    if (typeInfoVar->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfoVar, TypeInfoKind::Struct);
        regNode->alternativeScopes.push_back(typeStruct->scope);
        regNode->alternativeScopesVars.push_back({varNode, typeStruct->scope});
    }
    else if (typeInfoVar->kind == TypeInfoKind::Pointer)
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfoVar, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePointer->ptrCount == 1, context->report({node, format("'using' cannot be used on a variable of type '%s'", typePointer->name.c_str())}));
        SWAG_VERIFY(typePointer->finalType->kind == TypeInfoKind::Struct, context->report({node, format("'using' cannot be used on a variable of type '%s'", typeInfoVar->name.c_str())}));
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typePointer->finalType, TypeInfoKind::Struct);
        regNode->alternativeScopes.push_back(typeStruct->scope);
        regNode->alternativeScopesVars.push_back({varNode, typeStruct->scope});
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
        SWAG_CHECK(resolveUsingVar(context, idref->resolvedSymbolOverload->node, idref->resolvedSymbolOverload->typeInfo));
        return true;
    }

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
    case TypeInfoKind::Struct:
    {
        auto typeInfo = static_cast<TypeInfoStruct*>(idref->resolvedSymbolOverload->typeInfo);
        scope         = typeInfo->scope;
        break;
    }
    default:
        return job->error(context, "invalid 'using' reference");
    }

    node->parent->alternativeScopes.push_back(scope);
    return true;
}

bool SemanticJob::resolveScopedStmtBefore(SemanticContext* context)
{
    auto node                        = context->node;
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    node->byteCodeAfterFct           = ByteCodeGenJob::emitLeaveScope;
    return true;
}

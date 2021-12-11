#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Module.h"
#include "ErrorIds.h"

bool SemanticJob::resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar)
{
    auto node    = context->node;
    auto regNode = node->ownerScope ? node->ownerScope->owner : node;

    SWAG_ASSERT(regNode);
    SWAG_VERIFY(node->ownerFct || node->ownerScope->kind == ScopeKind::Struct, context->report({node, Utf8::format(g_E[Err0689], Scope::getNakedKindName(node->ownerScope->kind))}));

    typeInfoVar = TypeManager::concreteReference(typeInfoVar);
    if (typeInfoVar->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfoVar, TypeInfoKind::Struct);
        regNode->allocateExtension();
        regNode->addAlternativeScope(typeStruct->scope);
        regNode->addAlternativeScopeVar({varNode, typeStruct->scope});
    }
    else if (typeInfoVar->kind == TypeInfoKind::Pointer)
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfoVar, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePointer->pointedType->kind != TypeInfoKind::Enum, context->report({node, g_E[Err0691]}));
        SWAG_VERIFY(typePointer->pointedType->kind == TypeInfoKind::Struct, context->report({node, Utf8::format(g_E[Err0822], typeInfoVar->getDisplayName().c_str())}));
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
        regNode->addAlternativeScope(typeStruct->scope);
        regNode->addAlternativeScopeVar({varNode, typeStruct->scope});
    }
    else
    {
        return context->report({node, Utf8::format(g_E[Err0692], typeInfoVar->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveWith(SemanticContext* context)
{
    auto node  = context->node;
    auto idref = CastAst<AstIdentifierRef>(node->childs[0], AstNodeKind::IdentifierRef);
    node->flags |= AST_NO_BYTECODE;

    SWAG_ASSERT(idref->resolvedSymbolName);
    if (idref->resolvedSymbolName->kind == SymbolKind::Variable)
    {
        SWAG_VERIFY(idref->resolvedSymbolOverload, context->report({node, g_E[Err0694]}));
        SWAG_CHECK(resolveUsingVar(context, idref, idref->resolvedSymbolOverload->typeInfo));
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
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto typeInfo = CastTypeInfo<TypeInfoEnum>(typeResolved, typeResolved->kind);
        scope         = typeInfo->scope;
        break;
    }
    default:
        return context->report({node, Utf8::format(g_E[Err0703], typeResolved->getDisplayName().c_str())});
    }

    context->node->addAlternativeScope(scope);
    return true;
}

bool SemanticJob::resolveUsing(SemanticContext* context)
{
    auto node  = context->node;
    auto idref = CastAst<AstIdentifierRef>(node->childs[0], AstNodeKind::IdentifierRef);
    node->flags |= AST_NO_BYTECODE;

    SWAG_ASSERT(idref->resolvedSymbolName);
    if (idref->resolvedSymbolName->kind == SymbolKind::Variable)
    {
        SWAG_VERIFY(idref->resolvedSymbolOverload, context->report({node, g_E[Err0694]}));
        SWAG_CHECK(resolveUsingVar(context, idref, idref->resolvedSymbolOverload->typeInfo));
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
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto typeInfo = CastTypeInfo<TypeInfoEnum>(typeResolved, typeResolved->kind);
        scope         = typeInfo->scope;
        break;
    }
    case TypeInfoKind::Struct:
    {
        auto typeInfo = CastTypeInfo<TypeInfoStruct>(typeResolved, typeResolved->kind);
        scope         = typeInfo->scope;
        break;
    }
    default:
        return context->report({node, Utf8::format(g_E[Err0695], typeResolved->getDisplayName().c_str())});
    }

    node->parent->addAlternativeScope(scope);

    if (!idref->ownerFct)
        node->parent->sourceFile->addGlobalUsing(scope);

    return true;
}

bool SemanticJob::resolveScopedStmtBefore(SemanticContext* context)
{
    auto node                        = context->node;
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    node->allocateExtension();

    // Can already been set in some cases... So be sure to not overwrite it.
    auto afterFct = node->extension->byteCodeAfterFct;
    if (afterFct != ByteCodeGenJob::emitIfAfterIf &&
        afterFct != ByteCodeGenJob::emitSwitchCaseAfterBlock &&
        afterFct != ByteCodeGenJob::emitLoopAfterBlock &&
        afterFct != ByteCodeGenJob::emitLeaveScope)
    {
        SWAG_ASSERT(!afterFct);
        node->extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;
    }

    return true;
}

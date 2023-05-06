#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Naming.h"

bool SemanticJob::resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar)
{
    auto node    = context->node;
    auto regNode = node->ownerScope ? node->ownerScope->owner : node;

    SWAG_ASSERT(regNode);
    SWAG_VERIFY(node->ownerFct || node->ownerScope->kind == ScopeKind::Struct, context->report({node, Fmt(Err(Err0689), Naming::kindName(node->ownerScope->kind).c_str())}));

    uint32_t altFlags = node->flags & AST_STRUCT_MEMBER ? ALTSCOPE_USING : 0;

    typeInfoVar = TypeManager::concretePtrRef(typeInfoVar);
    if (typeInfoVar->isStruct())
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfoVar, TypeInfoKind::Struct);
        regNode->allocateExtension(ExtensionKind::Misc);
        regNode->addAlternativeScope(typeStruct->scope, altFlags);
        regNode->addAlternativeScopeVar(typeStruct->scope, varNode, altFlags);
    }
    else if (typeInfoVar->isPointer())
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfoVar, TypeInfoKind::Pointer);
        SWAG_VERIFY(typePointer->pointedType->isStruct(), context->report({node, Fmt(Err(Err0822), typePointer->pointedType->getDisplayNameC())}));
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
        regNode->addAlternativeScope(typeStruct->scope, altFlags);
        regNode->addAlternativeScopeVar(typeStruct->scope, varNode, altFlags);
    }
    else
    {
        return context->report({node, Fmt(Err(Err0692), typeInfoVar->getDisplayNameC())});
    }

    return true;
}

bool SemanticJob::resolveWithVarDeclAfter(SemanticContext* context)
{
    SWAG_CHECK(resolveVarDeclAfter(context));
    SWAG_CHECK(resolveWith(context));
    return true;
}

bool SemanticJob::resolveWithAfterKnownType(SemanticContext* context)
{
    SWAG_CHECK(resolveAfterKnownType(context));
    SWAG_CHECK(resolveWith(context));
    return true;
}

bool SemanticJob::resolveWith(SemanticContext* context)
{
    auto n = context->node->findParent(AstNodeKind::With);
    SWAG_ASSERT(n);
    auto node = CastAst<AstWith>(n, AstNodeKind::With);

    // If this is a simple identifier, no bytecode generation
    TypeInfo* typeResolved = nullptr;
    auto      front        = node->childs.front();
    bool      fromVar      = false;
    if (front->kind == AstNodeKind::IdentifierRef)
    {
        front->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
        SWAG_ASSERT(front->resolvedSymbolName);
        SWAG_VERIFY(front->resolvedSymbolOverload, context->report({node, Err(Err0694)}));
        typeResolved = front->resolvedSymbolOverload->typeInfo;
        fromVar      = front->resolvedSymbolName->kind == SymbolKind::Variable;
    }
    else if (front->kind == AstNodeKind::VarDecl)
    {
        SWAG_VERIFY(front->resolvedSymbolOverload, context->report({node, Err(Err0694)}));
        typeResolved = front->resolvedSymbolOverload->typeInfo;
        fromVar      = true;
    }
    else if (front->kind == AstNodeKind::AffectOp)
    {
        SWAG_VERIFY(front->childs.front()->resolvedSymbolOverload, context->report({node, Err(Err0694)}));
        typeResolved = front->childs.front()->resolvedSymbolOverload->typeInfo;
        fromVar      = true;
    }

    SWAG_ASSERT(typeResolved);
    switch (typeResolved->kind)
    {
    case TypeInfoKind::Pointer:
        if (!typeResolved->isPointerTo(TypeInfoKind::Struct))
            return context->report({node, Fmt(Err(Err0703), typeResolved->getDisplayNameC())});
        break;

    case TypeInfoKind::Namespace:
    case TypeInfoKind::Struct:
        break;

    case TypeInfoKind::Enum:
        if (fromVar)
            return context->report({node, Fmt(Err(Err0073), typeResolved->getDisplayNameC())});
        break;

    default:
        return context->report({node, Fmt(Err(Err0703), typeResolved->getDisplayNameC())});
    }

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
        SWAG_VERIFY(idref->resolvedSymbolOverload, context->report({node, Err(Err0694)}));
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
        return context->report({node, Fmt(Err(Err0695), typeResolved->getDisplayNameC())});
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
    node->allocateExtension(ExtensionKind::ByteCode);

    // Can already been set in some cases... So be sure to not overwrite it.
    auto afterFct = node->extByteCode()->byteCodeAfterFct;
    if (afterFct != ByteCodeGenJob::emitIfAfterIf &&
        afterFct != ByteCodeGenJob::emitSwitchCaseAfterBlock &&
        afterFct != ByteCodeGenJob::emitLoopAfterBlock &&
        afterFct != ByteCodeGenJob::emitLeaveScope)
    {
        SWAG_ASSERT(!afterFct);
        node->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;
    }

    return true;
}

bool SemanticJob::resolveScopedStmtAfter(SemanticContext* context)
{
    SWAG_CHECK(warnUnusedVariables(context, context->node->ownerScope));
    return true;
}

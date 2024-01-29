#include "pch.h"
#include "Ast.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"
#include "Ast_Flags.h"

bool Semantic::resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar)
{
    auto node    = context->node;
    auto regNode = node->ownerScope ? node->ownerScope->owner : node;

    SWAG_ASSERT(regNode);
    SWAG_VERIFY(node->ownerFct || node->ownerScope->kind == ScopeKind::Struct, context->report({node, Fmt(Err(Err0477), Naming::kindName(node->ownerScope->kind).c_str())}));

    uint32_t altFlags = node->flags & AST_STRUCT_MEMBER ? ALTSCOPE_STRUCT_USING : 0;

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
        SWAG_VERIFY(typePointer->pointedType->isStruct(), context->report({node, Fmt(Err(Err0476), typePointer->pointedType->getDisplayNameC())}));
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
        regNode->addAlternativeScope(typeStruct->scope, altFlags);
        regNode->addAlternativeScopeVar(typeStruct->scope, varNode, altFlags);
    }
    else
    {
        return context->report({node, Fmt(Err(Err0474), typeInfoVar->getDisplayNameC())});
    }

    return true;
}

bool Semantic::resolveWithVarDeclAfter(SemanticContext* context)
{
    SWAG_CHECK(resolveVarDeclAfter(context));
    SWAG_CHECK(resolveWith(context));
    return true;
}

bool Semantic::resolveWithAfterKnownType(SemanticContext* context)
{
    SWAG_CHECK(resolveAfterKnownType(context));
    SWAG_CHECK(resolveWith(context));
    return true;
}

bool Semantic::resolveWith(SemanticContext* context)
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
        SWAG_ASSERT(front->resolvedSymbolName && front->resolvedSymbolOverload);
        typeResolved = front->resolvedSymbolOverload->typeInfo;
        fromVar      = front->resolvedSymbolName->kind == SymbolKind::Variable;
    }
    else if (front->kind == AstNodeKind::VarDecl)
    {
        SWAG_ASSERT(front->resolvedSymbolOverload);
        typeResolved = front->resolvedSymbolOverload->typeInfo;
        fromVar      = true;
    }
    else if (front->kind == AstNodeKind::AffectOp)
    {
        SWAG_ASSERT(front->childs.front()->resolvedSymbolOverload);
        typeResolved = front->childs.front()->resolvedSymbolOverload->typeInfo;
        fromVar      = true;
    }

    SWAG_ASSERT(typeResolved);
    switch (typeResolved->kind)
    {
    case TypeInfoKind::Pointer:
        if (!typeResolved->isPointerTo(TypeInfoKind::Struct))
            return context->report({node, Fmt(Err(Err0173), typeResolved->getDisplayNameC())});
        break;

    case TypeInfoKind::Namespace:
    case TypeInfoKind::Struct:
        break;

    case TypeInfoKind::Enum:
        if (fromVar)
            return context->report({node, Fmt(Err(Err0172), typeResolved->getDisplayNameC())});
        break;

    default:
        return context->report({node, Fmt(Err(Err0173), typeResolved->getDisplayNameC())});
    }

    return true;
}

bool Semantic::resolveUsing(SemanticContext* context)
{
    auto node  = context->node;
    auto idref = CastAst<AstIdentifierRef>(node->childs[0], AstNodeKind::IdentifierRef);
    node->flags |= AST_NO_BYTECODE;

    SWAG_ASSERT(idref->resolvedSymbolName);
    if (idref->resolvedSymbolName->kind == SymbolKind::Variable)
    {
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
        return context->report({node, Fmt(Err(Err0472), typeResolved->getDisplayNameC())});
    }

    node->parent->addAlternativeScope(scope, ALTSCOPE_USING);

    if (!idref->ownerFct)
        node->parent->sourceFile->addGlobalUsing(scope);

    return true;
}

bool Semantic::resolveScopedStmtBefore(SemanticContext* context)
{
    auto node                        = context->node;
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    node->allocateExtension(ExtensionKind::ByteCode);

    // Can already been set in some cases... So be sure to not overwrite it.
    auto afterFct = node->extByteCode()->byteCodeAfterFct;
    if (afterFct != ByteCodeGen::emitIfAfterIf &&
        afterFct != ByteCodeGen::emitSwitchCaseAfterBlock &&
        afterFct != ByteCodeGen::emitLoopAfterBlock &&
        afterFct != ByteCodeGen::emitLeaveScope)
    {
        SWAG_ASSERT(!afterFct);
        node->extByteCode()->byteCodeAfterFct = ByteCodeGen::emitLeaveScope;
    }

    return true;
}

bool Semantic::resolveScopedStmtAfter(SemanticContext* context)
{
    SWAG_CHECK(SemanticError::warnUnusedVariables(context, context->node->ownerScope));
    return true;
}

bool Semantic::resolveSubDeclRef(SemanticContext* context)
{
    auto node = CastAst<AstRefSubDecl>(context->node, AstNodeKind::RefSubDecl);

    ScopedLock lk(node->refSubDecl->mutex);
    if (node->refSubDecl->flags & AST_SPEC_SEMANTIC3)
    {
        node->refSubDecl->flags &= ~AST_SPEC_SEMANTIC3;
        launchResolveSubDecl(context, node->refSubDecl);
    }

    return true;
}

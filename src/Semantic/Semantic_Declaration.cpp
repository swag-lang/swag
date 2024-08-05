#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Wmf/SourceFile.h"

bool Semantic::resolveUsingVar(SemanticContext* context, AstNode* varNode, TypeInfo* typeInfoVar)
{
    auto       node    = context->node;
    const auto regNode = node->ownerScope ? node->ownerScope->owner : node;

    SWAG_ASSERT(regNode);
    SWAG_VERIFY(node->ownerFct || node->ownerScope->is(ScopeKind::Struct), context->report({node, formErr(Err0469, Naming::kindName(node->ownerScope->kind).c_str())}));

    const CollectedScopeFlags altFlags = node->hasAstFlag(AST_STRUCT_MEMBER) ? COLLECTED_SCOPE_STRUCT_USING : 0;

    typeInfoVar = TypeManager::concretePtrRef(typeInfoVar);
    if (typeInfoVar->isStruct())
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfoVar, TypeInfoKind::Struct);
        regNode->allocateExtension(ExtensionKind::Misc);
        regNode->addAlternativeScope(typeStruct->scope, altFlags);
        regNode->addAlternativeScopeVar(typeStruct->scope, varNode, altFlags);
    }
    else if (typeInfoVar->isPointer())
    {
        const auto typePointer = castTypeInfo<TypeInfoPointer>(typeInfoVar, TypeInfoKind::Pointer);
        if (!typePointer->pointedType->isInterface())
        {
            SWAG_VERIFY(typePointer->pointedType->isStruct(), context->report({node, formErr(Err0468, typePointer->pointedType->getDisplayNameC())}));
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            regNode->addAlternativeScope(typeStruct->scope, altFlags);
            regNode->addAlternativeScopeVar(typeStruct->scope, varNode, altFlags);
        }
    }
    else
    {
        return context->report({node, formErr(Err0467, typeInfoVar->getDisplayNameC())});
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
    const auto n = context->node->findParent(AstNodeKind::With);
    SWAG_ASSERT(n);
    const auto node = castAst<AstWith>(n, AstNodeKind::With);

    // If this is a simple identifier, no bytecode generation
    TypeInfo*  typeResolved = nullptr;
    const auto front        = node->firstChild();
    bool       fromVar      = false;
    if (front->is(AstNodeKind::IdentifierRef))
    {
        front->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
        SWAG_ASSERT(front->resolvedSymbolName() && front->resolvedSymbolOverload());
        typeResolved = front->resolvedSymbolOverload()->typeInfo;
        fromVar      = front->resolvedSymbolName()->is(SymbolKind::Variable);
    }
    else if (front->is(AstNodeKind::VarDecl))
    {
        SWAG_ASSERT(front->resolvedSymbolOverload());
        typeResolved = front->resolvedSymbolOverload()->typeInfo;
        fromVar      = true;
    }
    else if (front->is(AstNodeKind::AffectOp))
    {
        SWAG_ASSERT(front->firstChild()->resolvedSymbolOverload());
        typeResolved = front->firstChild()->resolvedSymbolOverload()->typeInfo;
        fromVar      = true;
    }

    SWAG_ASSERT(typeResolved);
    switch (typeResolved->kind)
    {
        case TypeInfoKind::Pointer:
            if (!typeResolved->isPointerTo(TypeInfoKind::Struct))
                return context->report({context->node, formErr(Err0168, typeResolved->getDisplayNameC())});
            break;

        case TypeInfoKind::Namespace:
        case TypeInfoKind::Struct:
            break;

        case TypeInfoKind::Enum:
            if (fromVar)
                return context->report({context->node, formErr(Err0167, typeResolved->getDisplayNameC())});
            break;

        default:
            return context->report({context->node, formErr(Err0168, typeResolved->getDisplayNameC())});
    }

    return true;
}

bool Semantic::resolveUsing(SemanticContext* context)
{
    auto node = castAst<AstUsing>(context->node, AstNodeKind::Using);
    for (const auto child : node->children)
    {
        const auto idRef = castAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
        node->addAstFlag(AST_NO_BYTECODE);

        SWAG_ASSERT(idRef->resolvedSymbolName());
        if (idRef->resolvedSymbolName()->is(SymbolKind::Variable))
        {
            SWAG_CHECK(resolveUsingVar(context, idRef, idRef->resolvedSymbolOverload()->typeInfo));
            continue;
        }

        Scope*     scope;
        const auto typeResolved = idRef->resolvedSymbolOverload()->typeInfo;
        switch (typeResolved->kind)
        {
            case TypeInfoKind::Namespace:
            {
                const auto typeInfo = castTypeInfo<TypeInfoNamespace>(typeResolved, typeResolved->kind);
                scope               = typeInfo->scope;
                break;
            }
            case TypeInfoKind::Enum:
            {
                const auto typeInfo = castTypeInfo<TypeInfoEnum>(typeResolved, typeResolved->kind);
                scope               = typeInfo->scope;
                break;
            }
            case TypeInfoKind::Struct:
            {
                const auto typeInfo = castTypeInfo<TypeInfoStruct>(typeResolved, typeResolved->kind);
                scope               = typeInfo->scope;
                break;
            }
            default:
                return context->report({node, formErr(Err0466, typeResolved->getDisplayNameC())});
        }

        node->parent->addAlternativeScope(scope, COLLECTED_SCOPE_USING);

        if (!idRef->ownerFct)
            node->parent->token.sourceFile->addGlobalUsing(scope);
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveScopedStmtBefore(SemanticContext* context)
{
    const auto node                  = context->node;
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    node->allocateExtension(ExtensionKind::ByteCode);

    // Can already been set in some cases... So be sure to not overwrite it.
    const auto afterFct = node->extByteCode()->byteCodeAfterFct;
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

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveSubDeclRef(SemanticContext* context)
{
    const auto node = castAst<AstRefSubDecl>(context->node, AstNodeKind::RefSubDecl);

    ScopedLock lk(node->refSubDecl->mutex);
    if (node->refSubDecl->hasAstFlag(AST_SPEC_SEMANTIC3))
    {
        node->refSubDecl->removeAstFlag(AST_SPEC_SEMANTIC3);
        launchResolveSubDecl(context, node->refSubDecl);
    }

    return true;
}

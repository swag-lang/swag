#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "SemanticJob.h"
#include "TypeManager.h"

bool Semantic::checkCanThrow(SemanticContext* context)
{
    auto node = context->node;

    // For a try/throw inside an inline block, take the original function, except if it is flagged with 'Swag.CalleeReturn'
    if (node->ownerInline)
    {
        if (!node->ownerInline->func->hasAttribute(ATTRIBUTE_CALLEE_RETURN) && !(node->flags & AST_IN_MIXIN))
            node->semFlags |= SEMFLAG_EMBEDDED_RETURN;
    }

    const auto parentFct = (node->semFlags & SEMFLAG_EMBEDDED_RETURN) ? node->ownerInline->func : node->ownerFct;

    if (parentFct->isSpecialFunctionName())
        return context->report({node, node->token, FMT(Err(Err0451), node->token.ctext(), node->token.ctext(), parentFct->token.ctext())});

    if (!(parentFct->typeInfo->flags & TYPEINFO_CAN_THROW) && !parentFct->hasAttribute(ATTRIBUTE_SHARP_FUNC))
        return context->report({node, node->token, FMT(Err(Err0450), node->token.ctext(), node->token.ctext(), parentFct->token.ctext())});

    return true;
}

bool Semantic::checkCanCatch(SemanticContext* context)
{
    auto       node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Try, AstNodeKind::Catch, AstNodeKind::TryCatch, AstNodeKind::Assume);
    const auto identifierRef = castAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);

    for (const auto c : identifierRef->childs)
    {
        if (!c->resolvedSymbolOverload)
            continue;
        if (c->resolvedSymbolOverload->symbol->kind == SymbolKind::Function || c->resolvedSymbolOverload->typeInfo->isLambdaClosure())
            return true;
    }

    const auto lastChild = identifierRef->childs.back();
    return context->report({node, node->token, FMT(Err(Err0499), node->token.ctext(), lastChild->token.ctext(), Naming::aKindName(lastChild->resolvedSymbolName->kind).c_str())});
}

bool Semantic::resolveTryBlock(SemanticContext* context)
{
    SWAG_CHECK(checkCanThrow(context));
    return true;
}

bool Semantic::resolveTry(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Try);
    const auto identifierRef = castAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanThrow(context));
    SWAG_CHECK(checkCanCatch(context));

    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->inheritComputedValue(identifierRef);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    return true;
}

bool Semantic::resolveTryCatch(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::TryCatch);
    const auto identifierRef = castAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->addSpecFlags(AstFuncDecl::SPECFLAG_REG_GET_CONTEXT);

    node->setBcNotifyBefore(ByteCodeGen::emitInitStackTrace);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->inheritComputedValue(identifierRef);

    return true;
}

bool Semantic::resolveCatch(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Catch);
    const auto identifierRef = castAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->addSpecFlags(AstFuncDecl::SPECFLAG_REG_GET_CONTEXT);

    node->allocateExtension(ExtensionKind::ByteCode);
    node->setBcNotifyBefore(ByteCodeGen::emitInitStackTrace);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->flags &= ~AST_DISCARD;
    node->inheritComputedValue(identifierRef);

    return true;
}

bool Semantic::resolveAssumeBlock(SemanticContext* context)
{
    return true;
}

bool Semantic::resolveAssume(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Assume);
    const auto identifierRef = castAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->addSpecFlags(AstFuncDecl::SPECFLAG_REG_GET_CONTEXT);

    node->allocateExtension(ExtensionKind::ByteCode);
    node->setBcNotifyBefore(ByteCodeGen::emitInitStackTrace);
    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->inheritComputedValue(identifierRef);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    return true;
}

bool Semantic::resolveThrow(SemanticContext* context)
{
    const auto node = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Throw);
    auto       expr = node->childs.front();
    node->typeInfo  = expr->typeInfo;

    SWAG_CHECK(checkCanThrow(context));

    const auto type = TypeManager::concretePtrRefType(expr->typeInfo);

    SWAG_VERIFY(!type->isVoid(), context->report({expr, Err(Err0379)}));
    if (!type->isAny() || !(node->specFlags & AstTryCatchAssume::SPECFLAG_THROW_GET_ERR))
        SWAG_VERIFY(type->isStruct(), context->report({expr, FMT(Err(Err0380), type->getDisplayNameC())}));

    if (type->isString())
        context->node->printLoc();

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoAny, node, expr, CASTFLAG_AUTO_OP_CAST | CASTFLAG_CONCRETE_ENUM));
    node->byteCodeFct = ByteCodeGen::emitThrow;
    return true;
}

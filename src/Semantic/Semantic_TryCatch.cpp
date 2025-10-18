#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"

bool Semantic::checkCanThrow(SemanticContext* context)
{
    auto node = context->node;

    // For a try/throw inside an inline block, take the original function, except if it is flagged with 'Swag.CalleeReturn'
    if (node->hasOwnerInline())
    {
        if (!node->ownerInline()->func->hasAttribute(ATTRIBUTE_CALLEE_RETURN) && !node->hasAstFlag(AST_IN_MIXIN))
            node->addSemFlag(SEMFLAG_EMBEDDED_RETURN);
    }

    const auto parentFct = node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN) ? node->ownerInline()->func : node->ownerFct;

    if (parentFct->isSpecialFunctionName())
        return context->report({node, node->token, formErr(Err0228, node->token.cstr(), parentFct->token.cstr())});

    if (!parentFct->typeInfo->hasFlag(TYPEINFO_CAN_THROW) && !parentFct->hasAttribute(ATTRIBUTE_SHARP_FUNC))
        return context->report({node, node->token, formErr(Err0227, node->token.cstr(), parentFct->token.cstr())});

    return true;
}

bool Semantic::checkCanCatch(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Try, AstNodeKind::Catch, AstNodeKind::TryCatch, AstNodeKind::Assume);
    const auto identifierRef = castAst<AstIdentifierRef>(node->firstChild(), AstNodeKind::IdentifierRef);

    for (const auto c : identifierRef->children)
    {
        if (!c->resolvedSymbolOverload())
            continue;
        if (c->resolvedSymbolOverload()->symbol->is(SymbolKind::Function) || c->resolvedSymbolOverload()->typeInfo->isLambdaClosure())
            return true;
    }

    const auto lastChild = identifierRef->lastChild();
    Diagnostic err{node, node->token, formErr(Err0229, node->token.cstr(), Naming::kindName(lastChild->resolvedSymbolName()->kind).cstr(), lastChild->token.cstr())};
    err.addNote(lastChild, form("a %s cannot raise errors", Naming::kindName(lastChild->resolvedSymbolName()->kind).cstr()));
    return context->report(err);
}

bool Semantic::resolveTryBlock(SemanticContext* context)
{
    SWAG_CHECK(checkCanThrow(context));
    return true;
}

bool Semantic::resolveTry(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Try);
    const auto identifierRef = castAst<AstIdentifierRef>(node->firstChild(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->lastChild();

    SWAG_CHECK(checkCanThrow(context));
    SWAG_CHECK(checkCanCatch(context));

    node->typeInfo = lastChild->typeInfo;
    node->addAstFlag(identifierRef->flags);
    node->inheritComputedValue(identifierRef);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    return true;
}

bool Semantic::resolveTryCatch(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::TryCatch);
    const auto identifierRef = castAst<AstIdentifierRef>(node->firstChild(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->lastChild();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_REG_GET_CONTEXT);

    node->setBcNotifyBefore(ByteCodeGen::emitInitStackTrace);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    node->typeInfo = lastChild->typeInfo;
    node->addAstFlag(identifierRef->flags);
    node->inheritComputedValue(identifierRef);

    return true;
}

bool Semantic::resolveCatch(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Catch);
    const auto identifierRef = castAst<AstIdentifierRef>(node->firstChild(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->lastChild();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_REG_GET_CONTEXT);

    node->allocateExtension(ExtensionKind::ByteCode);
    node->setBcNotifyBefore(ByteCodeGen::emitInitStackTrace);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    node->typeInfo = lastChild->typeInfo;
    node->addAstFlag(identifierRef->flags);
    if (node->hasAstFlag(AST_DISCARD))
    {
        node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_DISCARD);
        node->removeAstFlag(AST_DISCARD);
    }
    node->inheritComputedValue(identifierRef);

    return true;
}

bool Semantic::resolveAssumeBlock(SemanticContext*)
{
    return true;
}

bool Semantic::resolveAssume(SemanticContext* context)
{
    const auto node          = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Assume);
    const auto identifierRef = castAst<AstIdentifierRef>(node->firstChild(), AstNodeKind::IdentifierRef);
    const auto lastChild     = identifierRef->lastChild();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_REG_GET_CONTEXT);

    node->allocateExtension(ExtensionKind::ByteCode);
    node->setBcNotifyBefore(ByteCodeGen::emitInitStackTrace);
    node->addAstFlag(identifierRef->flags);
    node->inheritComputedValue(identifierRef);
    node->byteCodeFct = ByteCodeGen::emitPassThrough;

    node->typeInfo = lastChild->typeInfo;
    return true;
}

bool Semantic::resolveThrow(SemanticContext* context)
{
    const auto node = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Throw);
    auto       expr = node->firstChild();
    node->typeInfo  = expr->typeInfo;

    SWAG_CHECK(checkCanThrow(context));

    const auto type = TypeManager::concretePtrRefType(expr->typeInfo);

    SWAG_VERIFY(!type->isVoid(), context->report({expr, toErr(Err0474)}));
    if (!type->isAny() || !node->hasSpecFlag(AstTryCatchAssume::SPEC_FLAG_THROW_GET_ERR))
        SWAG_VERIFY(type->isStruct(), context->report({expr, formErr(Err0632, type->getDisplayNameC())}));

    if (type->isString())
        context->node->printLoc();

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoAny, node, expr, CAST_FLAG_AUTO_OP_CAST | CAST_FLAG_CONCRETE_ENUM));
    node->byteCodeFct = ByteCodeGen::emitThrow;
    return true;
}

void Semantic::setEmitTryCatchAssume(AstNode* node, const TypeInfo* typeInfo)
{
    if (!node->hasOwnerTryCatchAssume())
        return;

    if (typeInfo->hasFlag(TYPEINFO_CAN_THROW))
    {
        switch (node->ownerTryCatchAssume()->kind)
        {
            case AstNodeKind::Try:
                node->setBcNotifyAfter(ByteCodeGen::emitTry);
                break;
            case AstNodeKind::TryCatch:
                node->setBcNotifyAfter(ByteCodeGen::emitTryCatch);
                break;
            case AstNodeKind::Catch:
                node->setBcNotifyAfter(ByteCodeGen::emitCatch);
                break;
            case AstNodeKind::Assume:
                node->setBcNotifyAfter(ByteCodeGen::emitAssume);
                break;
        }
    }
    else if (typeInfo->isNullable() && node->ownerTryCatchAssume()->kind == AstNodeKind::Assume)
    {
        node->setBcNotifyAfter(ByteCodeGen::emitAssumeNotNull);
    }
}

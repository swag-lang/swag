#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "ErrorIds.h"

bool SyntaxJob::checkIsValidUserName(AstNode* node)
{
    if (node->parent && (node->parent->flags & AST_GENERATED))
        return true;
    if (node->flags & AST_GENERATED)
        return true;

    // An identifier that starts with '__' is reserved for internal usage !
    if (!sourceFile->isGenerated && !sourceFile->isBootstrapFile && !sourceFile->isRuntimeFile)
    {
        if (node->token.text.length() > 1 && node->token.text[0] == '_' && node->token.text[1] == '_')
            return error(node->token, Utf8::format(Msg0272, node->token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::checkIsSingleIdentifier(AstNode* node, const char* msg)
{
    if (node->kind != AstNodeKind::IdentifierRef ||
        node->childs.size() > 1 ||
        node->childs.back()->kind != AstNodeKind::Identifier)
    {
        Utf8 err = "expected a single identifier ";
        err += msg;
        return error(node, err);
    }

    return true;
}

bool SyntaxJob::doIdentifier(AstNode* parent, uint32_t identifierFlags)
{
    bool backTick = false;

    if (token.id == TokenId::SymBackTick)
    {
        SWAG_CHECK(eatToken());
        backTick = true;
        if (token.id == TokenId::SymQuestion)
            return error(token, Utf8::format(Msg0835, token.text.c_str()));
    }

    if (token.id == TokenId::SymQuestion && !(identifierFlags & IDENTIFIER_ACCEPT_QUESTION))
        return error(token, Utf8::format(Msg0835, token.text.c_str()));
    else if (token.id != TokenId::SymQuestion && Tokenizer::isSymbol(token.id))
        return error(token, Utf8::format(Msg0835, token.text.c_str()));
    else if (Tokenizer::isLiteral(token.id))
        return error(token, Utf8::format(Msg0285, token.text.c_str()));

    auto identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, parent);
    identifier->inheritTokenLocation(token);
    if (token.id == TokenId::CompilerScopeFct)
        identifier->semanticFct = SemanticJob::resolveCompilerScopeFct;
    else
        identifier->semanticFct = SemanticJob::resolveIdentifier;
    identifier->identifierRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
    if (backTick)
        identifier->specFlags |= AST_SPEC_IDENTIFIER_BACKTICK;
    if (contextualNoInline)
        identifier->specFlags |= AST_SPEC_IDENTIFIER_NO_INLINE;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsValidUserName(identifier));

    // Replace "Self" with the corresponding struct name
    if (identifier->token.text == g_LangSpec.name_Self)
    {
        SWAG_VERIFY(parent->ownerStructScope, sourceFile->report({identifier, identifier->token, Msg0838}));
        if (currentSelfStructScope)
            identifier->token.text = currentSelfStructScope->name;
        else
            identifier->token.text = parent->ownerStructScope->name;
    }

    if (!tokenizer.lastTokenIsEOL && !(identifierFlags & IDENTIFIER_NO_GEN_PARAMS))
    {
        // Generic arguments
        if (token.id == TokenId::SymQuote)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doGenericFuncCallParameters(identifier, &identifier->genericParameters));
            identifier->genericParameters->flags |= AST_NO_BYTECODE;
        }
    }

    // Function call parameters
    if (!tokenizer.lastTokenIsEOL && !(identifierFlags & IDENTIFIER_NO_FCT_PARAMS))
    {
        if (token.id == TokenId::SymLeftParen)
        {
            if (identifierFlags & IDENTIFIER_TYPE_DECL)
                return sourceFile->report({identifier, token, Msg0839});

            SWAG_CHECK(eatToken(TokenId::SymLeftParen));
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
        else if (!tokenizer.lastTokenIsBlank && token.id == TokenId::SymLeftCurly)
        {
            SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            identifier->callParameters->flags |= AST_CALL_FOR_STRUCT;
        }
    }

    // Array index
    if (token.id == TokenId::SymLeftSquare)
    {
        if (!(identifierFlags & IDENTIFIER_NO_PARAMS))
        {
            if (identifierFlags & IDENTIFIER_TYPE_DECL)
                return sourceFile->report({identifier, token, Msg0840});
            Ast::removeFromParent(identifier);
            SWAG_CHECK(doArrayPointerIndex((AstNode**) &identifier));
            Ast::addChildBack(parent, identifier);
        }
    }

    return true;
}

bool SyntaxJob::doIdentifierRef(AstNode* parent, AstNode** result, uint32_t identifierFlags)
{
    auto identifierRef         = Ast::newNode<AstIdentifierRef>(this, AstNodeKind::IdentifierRef, sourceFile, parent);
    identifierRef->semanticFct = SemanticJob::resolveIdentifierRef;
    if (result)
        *result = identifierRef;

    switch (token.id)
    {
    case TokenId::CompilerLocation:
        SWAG_CHECK(doCompilerLocation(identifierRef));
        break;

    case TokenId::IntrinsicSpread:
    case TokenId::IntrinsicSizeOf:
    case TokenId::IntrinsicAlignOf:
    case TokenId::IntrinsicOffsetOf:
    case TokenId::IntrinsicTypeOf:
    case TokenId::IntrinsicKindOf:
    case TokenId::IntrinsicCountOf:
    case TokenId::IntrinsicDataOf:
    case TokenId::IntrinsicStringOf:
    case TokenId::IntrinsicMakeAny:
    case TokenId::IntrinsicMakeSlice:
    case TokenId::IntrinsicMakeString:
    case TokenId::IntrinsicMakeInterface:
    case TokenId::IntrinsicMakeCallback:
    case TokenId::IntrinsicMakeForeign:
    case TokenId::IntrinsicIsConstExpr:
        SWAG_CHECK(doIntrinsicProp(identifierRef));
        break;

    default:
        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
        break;
    }

    while (token.id == TokenId::SymDot)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
    }

    return true;
}

bool SyntaxJob::doDiscard(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());
    ScopedFlags sf(this, AST_DISCARD);

    switch (token.id)
    {
    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent));
        break;
    case TokenId::KwdTry:
    case TokenId::KwdAssume:
        SWAG_CHECK(doTryAssume(parent, result));
        break;
    case TokenId::KwdCatch:
        SWAG_CHECK(doCatch(parent, result));
        break;
    default:
        return error(token, Utf8::format(Msg0841, token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::doTryAssume(AstNode* parent, AstNode** result)
{
    AstNode* node = nullptr;
    if (token.id == TokenId::KwdTry)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveTry;
    }
    else if (token.id == TokenId::KwdAssume)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Assume, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveAssume;
    }

    if (result)
        *result = node;
    SWAG_VERIFY(node->ownerFct, error(node, Utf8::format(Msg0842, node->token.text.c_str())));
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);

    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doCurlyStatement(node));
        if (node->semanticFct == SemanticJob::resolveTry)
            node->semanticFct = SemanticJob::resolveTryBlock;
        else
            node->semanticFct = nullptr;
    }
    else
    {
        SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Utf8::format(Msg0843, node->token.text.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Utf8::format(Msg0844, node->token.text.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Utf8::format(Msg0845, node->token.text.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Utf8::format(Msg0846, node->token.text.c_str())));
        SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Utf8::format(Msg0853, node->token.text.c_str())));
        SWAG_CHECK(doIdentifierRef(node));
    }

    return true;
}

bool SyntaxJob::doCatch(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Catch, sourceFile, parent);
    SWAG_VERIFY(node->ownerFct, error(node, Msg0848));
    node->semanticFct = SemanticJob::resolveCatch;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Utf8::format(Msg0843, node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Utf8::format(Msg0844, node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Utf8::format(Msg0845, node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Utf8::format(Msg0846, node->token.text.c_str())));
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Utf8::format(Msg0853, node->token.text.c_str())));
    SWAG_CHECK(doIdentifierRef(node));
    return true;
}

bool SyntaxJob::doThrow(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Throw, sourceFile, parent);
    SWAG_VERIFY(node->ownerFct, error(node, Msg0854));
    node->semanticFct = SemanticJob::resolveThrow;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Utf8::format(Msg0843, node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Utf8::format(Msg0844, node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Utf8::format(Msg0845, node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Utf8::format(Msg0846, node->token.text.c_str())));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    return true;
}
#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "Scoped.h"

bool SyntaxJob::checkIsSingleIdentifier(AstNode* node, const char* msg)
{
    if (node->kind != AstNodeKind::IdentifierRef ||
        node->childs.size() > 1 ||
        node->childs.back()->kind != AstNodeKind::Identifier)
    {
        Utf8 err = "expected a single identifier ";
        err += msg;
        return syntaxError(node, err);
    }

    return true;
}

bool SyntaxJob::doIdentifier(AstNode* parent, uint32_t identifierFlags)
{
    bool backTick = false;

    if (token.id == TokenId::SymBackTick)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        backTick = true;
    }

    auto identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, nullptr);
    identifier->inheritTokenLocation(token);
    if (token.id == TokenId::CompilerScopeFct)
        identifier->semanticFct = SemanticJob::resolveCompilerScopeFct;
    else
        identifier->semanticFct = SemanticJob::resolveIdentifier;
    identifier->identifierRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
    if (backTick)
        identifier->flags |= AST_IDENTIFIER_BACKTICK;
    SWAG_CHECK(tokenizer.getToken(token));

    SWAG_CHECK(checkIsValidUserName(identifier));

    // Replace "Self" with the corresponding struct name
    if (identifier->token.text == "Self")
    {
        SWAG_VERIFY(parent->ownerStructScope, sourceFile->report({identifier, identifier->token, "type 'Self' cannot be used outside an 'impl', 'struct' or 'interface' block"}));
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
                return sourceFile->report({identifier, token, "a struct initialization must be done with '{}' and not parenthesis (this is reserved for function calls)"});

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

    AstNode* expr = identifier;

    // Array index
    if (token.id == TokenId::SymLeftSquare)
    {
        if (identifierFlags & IDENTIFIER_TYPE_DECL)
            return sourceFile->report({identifier, token, "array size must be defined before the type name"});

        if (!(identifierFlags & IDENTIFIER_NO_PARAMS))
            SWAG_CHECK(doArrayPointerIndex(&expr));
    }

    Ast::addChildBack(parent, expr);

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
        return syntaxError(token, format("invalid token '%s' after 'discard'", token.text.c_str()));
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
    SWAG_VERIFY(node->ownerFct, error(node, format("'%s' can only be used inside a function", node->token.text.c_str())));
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
        SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, format("invalid 'try' inside '%s' expression", node->token.text.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, format("invalid 'catch' inside '%s' expression", node->token.text.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, format("invalid 'assume' inside '%s' expression", node->token.text.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, format("invalid 'throw' inside '%s' expression", node->token.text.c_str())));
        SWAG_VERIFY(token.id == TokenId::Identifier, error(token, format("'%s' must be immediatly followed by an identifier", node->token.text.c_str())));
        SWAG_CHECK(doIdentifierRef(node));
    }

    return true;
}

bool SyntaxJob::doCatch(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Catch, sourceFile, parent);
    SWAG_VERIFY(node->ownerFct, error(node, "'catch' can only be used inside a function"));
    node->semanticFct = SemanticJob::resolveCatch;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);
    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, format("invalid 'try' inside '%s' expression", node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, format("invalid 'catch' inside '%s' expression", node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, format("invalid 'assume' inside '%s' expression", node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, format("invalid 'throw' inside '%s' expression", node->token.text.c_str())));
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, format("'%s' must be immediatly followed by an identifier", node->token.text.c_str())));
    SWAG_CHECK(doIdentifierRef(node));
    return true;
}

bool SyntaxJob::doThrow(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Throw, sourceFile, parent);
    SWAG_VERIFY(node->ownerFct, error(node, "'throw' can only be used inside a function"));
    node->semanticFct = SemanticJob::resolveThrow;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, format("invalid 'try' inside '%s' expression", node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, format("invalid 'catch' inside '%s' expression", node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, format("invalid 'assume' inside '%s' expression", node->token.text.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, format("invalid 'throw' inside '%s' expression", node->token.text.c_str())));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    return true;
}
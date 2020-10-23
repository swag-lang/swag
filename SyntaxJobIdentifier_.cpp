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

bool SyntaxJob::doIdentifier(AstNode* parent, bool acceptParameters)
{
    bool backTick = false;
    if (token.id == TokenId::SymBackTick)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        backTick = true;
    }

    auto identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, nullptr);
    identifier->inheritTokenLocation(token);
    identifier->inheritTokenName(token);
    identifier->semanticFct   = SemanticJob::resolveIdentifier;
    identifier->identifierRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
    if (backTick)
        identifier->flags |= AST_IDENTIFIER_BACKTICK;
    SWAG_CHECK(tokenizer.getToken(token));

    if (acceptParameters && !tokenizer.lastTokenIsEOL)
    {
        // Generic arguments
        if (token.id == TokenId::SymQuote)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doGenericFuncCallParameters(identifier, &identifier->genericParameters));
            identifier->genericParameters->flags |= AST_NO_BYTECODE;
        }

        // Function call parameters
        if (!tokenizer.lastTokenIsEOL)
        {
            if (token.id == TokenId::SymLeftParen)
            {
                SWAG_CHECK(eatToken(TokenId::SymLeftParen));
                SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
            }
            else if (!tokenizer.lastTokenIsBlank && token.id == TokenId::SymLeftCurly)
            {
                ScopedFlags sk(this, AST_CALL_FOR_STRUCT);
                SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
                SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            }
        }
    }

    // Array index
    AstNode* expr = identifier;
    if (token.id == TokenId::SymLeftSquare)
        SWAG_CHECK(doArrayPointerIndex(&expr));
    Ast::addChildBack(parent, expr);

    return true;
}

bool SyntaxJob::doIdentifierRef(AstNode* parent, AstNode** result, bool acceptParameters)
{
    auto identifierRef         = Ast::newNode<AstIdentifierRef>(this, AstNodeKind::IdentifierRef, sourceFile, parent);
    identifierRef->semanticFct = SemanticJob::resolveIdentifierRef;
    if (result)
        *result = identifierRef;

    switch (token.id)
    {
    case TokenId::IntrinsicSizeOf:
    case TokenId::IntrinsicTypeOf:
    case TokenId::IntrinsicKindOf:
    case TokenId::IntrinsicCountOf:
    case TokenId::IntrinsicDataOf:
    case TokenId::IntrinsicMakeAny:
    case TokenId::IntrinsicMakeSlice:
    case TokenId::IntrinsicMakeInterface:
    case TokenId::IntrinsicSpread:
        SWAG_CHECK(doIntrinsicProp(identifierRef));
        break;
    default:
        SWAG_CHECK(doIdentifier(identifierRef, acceptParameters));
        break;
    }

    while (token.id == TokenId::SymDot)
    {
        SWAG_CHECK(eatToken(TokenId::SymDot));
        SWAG_CHECK(doIdentifier(identifierRef, acceptParameters));
    }

    return true;
}

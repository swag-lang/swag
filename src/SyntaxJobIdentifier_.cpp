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

    if (!(identifierFlags & IDENTIFIER_NO_PARAMS) && !tokenizer.lastTokenIsEOL)
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

bool SyntaxJob::doTryCatch(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::KwdTry)
    {
        auto tryNode = Ast::newNode<AstTryCatch>(this, AstNodeKind::Try, sourceFile, parent);
        SWAG_VERIFY(tryNode->ownerFct, error(tryNode, "'try' can only be used inside a function"));
        tryNode->semanticFct = SemanticJob::resolveTry;
        if (result)
            *result = tryNode;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(tryNode));
    }
    else if (token.id == TokenId::KwdCatch)
    {
        auto catchNode = Ast::newNode<AstTryCatch>(this, AstNodeKind::Catch, sourceFile, parent);
        SWAG_VERIFY(catchNode->ownerFct, error(catchNode, "'catch' can only be used inside a function"));
        catchNode->semanticFct = SemanticJob::resolveCatch;
        if (result)
            *result = catchNode;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(catchNode));
    }
    else if (token.id == TokenId::KwdAssume)
    {
        auto assumeNode = Ast::newNode<AstTryCatch>(this, AstNodeKind::Assume, sourceFile, parent);
        SWAG_VERIFY(assumeNode->ownerFct, error(assumeNode, "'assume' can only be used inside a function"));
        assumeNode->semanticFct = SemanticJob::resolveAssume;
        if (result)
            *result = assumeNode;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(assumeNode));
    }

    return true;
}

bool SyntaxJob::doThrow(AstNode* parent, AstNode** result)
{
    auto throwNode = Ast::newNode<AstTryCatch>(this, AstNodeKind::Throw, sourceFile, parent);
    SWAG_VERIFY(throwNode->ownerFct, error(throwNode, "'throw' can only be used inside a function"));
    throwNode->semanticFct = SemanticJob::resolveThrow;
    if (result)
        *result = throwNode;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doExpression(throwNode));
    return true;
}
#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Literal, parent, false);
    node->semanticFct = &SemanticJob::resolveLiteral;
    node->token       = move(token);

    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::SymLeftParen:
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(parent, result));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        return true;

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
        return doLiteral(parent, result);

    default:
        return notSupportedError(token);
    }
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymMinus || token.id == TokenId::SymExclam)
    {
        auto node         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::SingleOp, parent, false);
        node->semanticFct = &SemanticJob::resolveSingleOp;
        node->token       = move(token);

        if (result)
            *result = node;
        SWAG_CHECK(tokenizer.getToken(token));
        return doSinglePrimaryExpression(node);
    }

    return doSinglePrimaryExpression(parent, result);
}

bool SyntaxJob::doFactorExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doUnaryExpression(nullptr, &leftNode));

    bool isBinary = false;
    while ((token.id == TokenId::SymPlus) ||
           (token.id == TokenId::SymMinus) ||
           (token.id == TokenId::SymAsterisk) ||
           (token.id == TokenId::SymSlash))
    {
        auto binaryNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::BinaryOp, parent, false);
        binaryNode->semanticFct = &SemanticJob::resolveFactorExpression;
        binaryNode->token       = move(token);

        Ast::addChild(binaryNode, leftNode, false);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doUnaryExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChild(parent, leftNode, false);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doCompareExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doFactorExpression(nullptr, &leftNode));

    bool isBinary = false;
    while ((token.id == TokenId::SymEqualEqual) ||
           (token.id == TokenId::SymExclamEqual) ||
           (token.id == TokenId::SymLowerEqual) ||
           (token.id == TokenId::SymGreaterEqual) ||
           (token.id == TokenId::SymLower) ||
           (token.id == TokenId::SymGreater))
    {
        auto binaryNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::BinaryOp, parent, false);
        binaryNode->semanticFct = &SemanticJob::resolveCompareExpression;
        binaryNode->token       = move(token);

        Ast::addChild(binaryNode, leftNode, false);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChild(parent, leftNode, false);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doBoolExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doCompareExpression(nullptr, &leftNode));

    bool isBinary = false;
    while ((token.id == TokenId::SymVerticalVertical) || (token.id == TokenId::SymAmpersandAmpersand))
    {
        auto binaryNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::BinaryOp, parent, false);
        binaryNode->semanticFct = &SemanticJob::resolveBoolExpression;
        binaryNode->token       = move(token);

        Ast::addChild(binaryNode, leftNode, false);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doCompareExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChild(parent, leftNode, false);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doExpression(AstNode* parent, AstNode** result)
{
    return doBoolExpression(parent, result);
}

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doBoolExpression(parent, result);
}

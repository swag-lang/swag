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

bool SyntaxJob::doPrimaryExpression(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(token.id == TokenId::LiteralNumber || token.id == TokenId::LiteralCharacter, notSupportedError(token));
    return doLiteral(parent, result);
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymMinus)
    {
        auto node         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::SingleOp, parent, false);
        node->semanticFct = &SemanticJob::resolveSingleOp;
        node->token       = move(token);

        if (result)
            *result = node;
        SWAG_CHECK(tokenizer.getToken(token));
        return doPrimaryExpression(node);
    }

    return doPrimaryExpression(parent, result);
}

bool SyntaxJob::doFactorExpression(AstNode* parent, AstNode** result)
{
    return doUnaryExpression(parent, result);
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

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doBoolExpression(parent, result);
}

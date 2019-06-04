#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_PoolFactory.astNode, AstNodeKind::Literal, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveLiteral;
    node->byteCodeFct = &ByteCodeGenJob::emitLiteral;
    node->token       = move(token);

    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, AstNode** result)
{
    AstNode* expr = nullptr;

    switch (token.id)
    {
    case TokenId::SymLeftParen:
    {
        auto saveLocation = token.startLocation;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(nullptr, &expr));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        expr->token.startLocation = saveLocation;
        break;
    }

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(nullptr, &expr));
        break;

    case TokenId::Identifier:
    case TokenId::Intrisic:
        SWAG_CHECK(doIdentifierRef(nullptr, &expr));
        break;

    default:
        return syntaxError(token, format("invalid token '%s'", token.text.c_str()));
    }

    // Cast
    if (token.id == TokenId::KwdAs)
    {
        AstNode* castNode;
        SWAG_CHECK(doCast(parent, &castNode));
        Ast::addChild(castNode, expr);
        expr = castNode;
        // This is in reverse order ! "expression as type", but 'type' is the first child, and expression the 'second'
        castNode->token.startLocation = castNode->childs.back()->token.startLocation;
        castNode->token.endLocation   = castNode->childs.front()->token.endLocation;
    }

    if (parent)
        Ast::addChild(parent, expr);
    if (result)
        *result = expr;

    return true;
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymMinus || token.id == TokenId::SymExclam || token.id == TokenId::SymTilde)
    {
        auto node = Ast::newNode(&g_PoolFactory.astNode, AstNodeKind::SingleOp, sourceFile->indexInModule, parent, false);
        node->inheritOwners(this);
        node->semanticFct = &SemanticJob::resolveUnaryOp;
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
           (token.id == TokenId::SymSlash) ||
           (token.id == TokenId::SymVertical) ||
           (token.id == TokenId::SymAmpersand) ||
           (token.id == TokenId::SymGreaterGreater) ||
           (token.id == TokenId::SymLowerLower) ||
           (token.id == TokenId::SymCircumflex))
    {
        auto binaryNode = Ast::newNode(&g_PoolFactory.astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent, false);
        binaryNode->inheritOwners(this);
        if (token.id == TokenId::SymGreaterGreater || token.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = &SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = &SemanticJob::resolveFactorExpression;
        binaryNode->token = move(token);

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
        auto binaryNode = Ast::newNode(&g_PoolFactory.astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent, false);
        binaryNode->inheritOwners(this);
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
        auto binaryNode = Ast::newNode(&g_PoolFactory.astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent, false);
        binaryNode->inheritOwners(this);
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

bool SyntaxJob::doAffectExpression(AstNode* parent)
{
    return doBoolExpression(parent);
}

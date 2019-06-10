#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "SemanticJob.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::Literal, sourceFile->indexInModule, parent);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveLiteral;
    node->byteCodeFct = &ByteCodeGenJob::emitLiteral;
    node->token       = move(token);

    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doLeftExpression(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::Identifier:
    case TokenId::Intrisic:
        SWAG_CHECK(doIdentifierRef(parent, result, AST_LEFT_EXPRESSION));
        break;

    default:
        return syntaxError(token, format("invalid token '%s'", token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::SymLeftParen:
    {
        auto saveLocation = token.startLocation;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(parent, result));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        break;
    }

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(parent, result));
        break;

    case TokenId::Identifier:
    case TokenId::Intrisic:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    default:
        return syntaxError(token, format("invalid token '%s'", token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, AstNode** result)
{
    // Cast
    if (token.id == TokenId::KwdCast)
    {
        AstNode* castNode;
        SWAG_CHECK(doCast(parent, &castNode));
        if (result)
            *result = castNode;
        return doUnaryExpression(castNode);
    }

    if (token.id == TokenId::SymMinus || token.id == TokenId::SymExclam || token.id == TokenId::SymTilde)
    {
        auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::SingleOp, sourceFile->indexInModule, parent);
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
        auto binaryNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent);
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
        auto binaryNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent);
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
        auto binaryNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent);
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
    if (token.id == TokenId::CompilerRun)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        AstNode* expr;
        SWAG_CHECK(doBoolExpression(parent, &expr));
        if (result)
            *result = expr;
        expr->semanticAfterFct = &SemanticJob::forceExecuteNode;
        return true;
    }

    return doBoolExpression(parent, result);
}

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doExpression(parent, result);
}

bool SyntaxJob::doAffectExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpression(nullptr, &leftNode));

    if (token.id == TokenId::SymEqual ||
        token.id == TokenId::SymPlusEqual ||
        token.id == TokenId::SymMinusEqual ||
        token.id == TokenId::SymAsteriskEqual ||
        token.id == TokenId::SymSlashEqual ||
        token.id == TokenId::SymAmpersandEqual ||
        token.id == TokenId::SymVerticalEqual ||
        token.id == TokenId::SymCircumflexEqual ||
        token.id == TokenId::SymLowerLowerEqual ||
        token.id == TokenId::SymGreaterGreaterEqual)
    {
        auto affectNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::AffectOp, sourceFile->indexInModule, parent);
        affectNode->inheritOwners(this);
        affectNode->semanticFct = &SemanticJob::resolveAffect;
        affectNode->token       = move(token);

        Ast::addChild(affectNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(affectNode, &leftNode));

        if (result)
            *result = affectNode;
    }
    else
    {
        Ast::addChild(parent, leftNode);
        if (result)
            *result = leftNode;
    }

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    return true;
}

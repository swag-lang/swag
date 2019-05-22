#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Literal, parent, false);
    node->semanticFct = &SemanticJob::resolveLiteral;
    if (result)
        *result = node;

    node->token = move(token);
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doPrimaryExpression(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(token.id == TokenId::LiteralNumber, notSupportedError(token));
    return doLiteral(parent, result);
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymMinus)
    {
        auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::SingleOp, parent, false);
		node->semanticFct = &SemanticJob::resolveSingleOp;
        if (result)
            *result = node;
        node->token = move(token);
        SWAG_CHECK(tokenizer.getToken(token));
        return doPrimaryExpression(node);
    }

    return doPrimaryExpression(parent, result);
}

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doUnaryExpression(parent, result);
}

#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"

bool SyntaxJob::doIf(AstNode* parent)
{
    auto node         = Ast::newNode(&g_Pool_astIf, AstNodeKind::If, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveIf;
    node->inheritOwners(this);
    node->inheritToken(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
    SWAG_CHECK(doEmbeddedStatement(node, &node->ifBlock));

    if (token.id == TokenId::KwdElse)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doEmbeddedStatement(node, &node->elseBlock));
    }

    return true;
}

bool SyntaxJob::doWhile(AstNode* parent)
{
    auto node         = Ast::newNode(&g_Pool_astWhile, AstNodeKind::While, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveWhile;
    node->inheritOwners(this);
    node->inheritToken(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
    SWAG_CHECK(doEmbeddedStatement(node, &node->block));

    return true;
}

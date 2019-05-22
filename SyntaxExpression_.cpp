#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Literal, parent, false);
    node->semanticFct = &SemanticJob::resolveLiteral;
    if (result)
        *result = node;

    node->token = move(token);
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(token.id == TokenId::LiteralNumber, notSupportedError(token));
    return doLiteral(parent, result);
}

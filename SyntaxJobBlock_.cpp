#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "Scope.h"
#include "Scoped.h"

bool SyntaxJob::doIf(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(&g_Pool_astIf, AstNodeKind::If, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveIf;
    node->inheritOwnersAndFlags(this);
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
    SWAG_CHECK(doEmbeddedStatement(node, &node->ifBlock));

    if (token.id == TokenId::KwdElse)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doEmbeddedStatement(node, &node->elseBlock));
		node->elseBlock->kind = AstNodeKind::Else;
    }

    return true;
}

bool SyntaxJob::doWhile(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(&g_Pool_astWhile, AstNodeKind::While, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveWhile;
    node->inheritOwnersAndFlags(this);
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    {
        ScopedBreakable scoped(this, node);
        SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doBreak(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentBreakable, sourceFile->report({sourceFile, token, "'break' can only be used inside a breakable scope"}));

    auto node         = Ast::newNode(&g_Pool_astBreakContinue, AstNodeKind::Break, sourceFile->indexInModule, parent);
    node->byteCodeFct = &ByteCodeGenJob::emitBreak;
    node->inheritOwnersAndFlags(this);
    node->inheritToken(token);
    if (result)
        *result = node;

    auto breakable = static_cast<AstBreakable*>(currentBreakable);
    breakable->breakList.push_back(node);

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doContinue(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentBreakable, sourceFile->report({sourceFile, token, "'continue' can only be used inside a breakable scope"}));

    auto node         = Ast::newNode(&g_Pool_astBreakContinue, AstNodeKind::Continue, sourceFile->indexInModule, parent);
    node->byteCodeFct = &ByteCodeGenJob::emitContinue;
    node->inheritOwnersAndFlags(this);
    node->inheritToken(token);
    if (result)
        *result = node;

    auto breakable = static_cast<AstBreakable*>(currentBreakable);
    breakable->continueList.push_back(node);

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

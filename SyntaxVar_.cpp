#include "pch.h"
#include "Global.h"
#include "PoolFactory.h"
#include "SourceFile.h"

bool SyntaxJob::doVarDecl(AstNode* parent, AstVarDecl** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::VarDecl, currentScope, sourceFile->indexInModule, parent, false);
    node->semanticFct = &SemanticJob::resolveVarDecl;
    if (result)
        *result = node;

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    node->name = token.text;
    node->name.computeCrc();
    node->token = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(node, &node->astType));
        SWAG_CHECK(node->astType);
    }

    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doAssignmentExpression(node, &node->astAssignment));
    }

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    currentScope->allocateSymTable();
    currentScope->symTable->registerSymbolNameNoLock(sourceFile, node, SymbolKind::Variable);

    return true;
}

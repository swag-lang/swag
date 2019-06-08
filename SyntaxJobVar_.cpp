#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "SymTable.h"

bool SyntaxJob::doVarDecl(AstNode* parent)
{
    // First variable
    AstVarDecl* varNode = Ast::newNode(&g_Pool_astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent);
    varNode->inheritOwners(this);
    varNode->semanticFct = &SemanticJob::resolveVarDecl;

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    varNode->inheritToken(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(varNode, &varNode->astType));
    }

    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doAssignmentExpression(varNode, &varNode->astAssignment));
    }

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    // Be sure we will be able to have a type
    if (!varNode->astType && !varNode->astAssignment)
    {
        return error(varNode->token, "variable must be initialized because no type is specified");
    }

    currentScope->allocateSymTable();
    SWAG_CHECK(currentScope->symTable->registerSymbolNameNoLock(sourceFile, varNode, SymbolKind::Variable));

    return true;
}

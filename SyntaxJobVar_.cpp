#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "SymTable.h"

bool SyntaxJob::doVarDecl(AstNode* parent, AstNode** result)
{
    // First variable
    AstNodeKind kind = AstNodeKind::VarDecl;
    switch (token.id)
    {
    case TokenId::KwdConst:
        kind = AstNodeKind::ConstDecl;
        break;
    case TokenId::KwdLet:
        kind = AstNodeKind::LetDecl;
        break;
    }

	SWAG_CHECK(tokenizer.getToken(token));

    AstVarDecl* varNode = Ast::newNode(&g_Pool_astVarDecl, kind, sourceFile->indexInModule, parent);
    varNode->inheritOwnersAndFlags(this);
    varNode->semanticFct = SemanticJob::resolveVarDecl;
    if (result)
        *result = varNode;

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    varNode->inheritToken(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(varNode, &varNode->astType));
    }

    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doInitializationExpression(varNode, &varNode->astAssignment));
    }

    SWAG_CHECK(eatSemiCol("at the end of a variable declation"));

    // Be sure we will be able to have a type
    if (!varNode->astType && !varNode->astAssignment)
    {
        return error(varNode->token, "variable must be initialized because no type is specified");
    }

    if (!isContextDisabled())
    {
        currentScope->allocateSymTable();
        SWAG_CHECK(currentScope->symTable->registerSymbolNameNoLock(sourceFile, varNode, SymbolKind::Variable));
    }

    return true;
}

#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "SymTable.h"

bool SyntaxJob::doVarDecl(AstNode* parent)
{
    vector<AstVarDecl*> allVars;

    // First variable
    AstVarDecl* firstVar = Ast::newNode(&g_Pool_astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent);
    firstVar->inheritOwners(this);
    firstVar->semanticFct = &SemanticJob::resolveVarDecl;
    allVars.push_back(firstVar);

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    firstVar->inheritToken(token);

    SWAG_CHECK(tokenizer.getToken(token));

    // All other variables with same type and initialization
    // Evaluation of type and assignment is for the first variable only
    // Other variables will be affected with the first one
    // a, b: bool = x is compiled as a: bool = x; b = a;
    while (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));

        auto node         = Ast::newNode(&g_Pool_astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent);
        node->semanticFct = &SemanticJob::resolveVarDecl;
        node->inheritOwners(this);
        node->inheritToken(token);
        allVars.push_back(node);

        auto idRef = Ast::createIdentifierRef(this, firstVar->name, node->token, node);
		node->astAssignment = idRef;

        SWAG_CHECK(tokenizer.getToken(token));
    }

    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(firstVar, &firstVar->astType));
    }

    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doAssignmentExpression(firstVar, &firstVar->astAssignment));
    }

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    // Be sure we will be able to have a type
    if (!firstVar->astType && !firstVar->astAssignment)
    {
        if (allVars.size() == 1)
            return error(allVars.front()->token, "variable must be initialized because no type is specified");
        return error(allVars.front()->token.startLocation, allVars.back()->token.endLocation, "variables must be initialized because no type is specified");
    }

    currentScope->allocateSymTable();

    // Register all symbols
    for (int i = 0; i < allVars.size(); i++)
    {
        auto var = allVars[i];
        SWAG_CHECK(currentScope->symTable->registerSymbolNameNoLock(sourceFile, var, SymbolKind::Variable));
    }

    return true;
}

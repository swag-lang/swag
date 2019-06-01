#include "pch.h"
#include "Global.h"
#include "PoolFactory.h"
#include "SourceFile.h"

bool SyntaxJob::doVarDecl(AstNode* parent)
{
    vector<AstVarDecl*> allVars;
    while (true)
    {
        auto node = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent, false);
        node->inheritOwners(this);
        node->semanticFct = &SemanticJob::resolveVarDecl;
        allVars.push_back(node);

        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
        node->name = token.text;
        node->name.computeCrc();
        node->token = move(token);

        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken(TokenId::SymComma));
            continue;
        }

        break;
    }

    AstNode* type = nullptr;
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(nullptr, &type));
    }

    AstNode* assignment = nullptr;
    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doAssignmentExpression(nullptr, &assignment));
    }

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

	// Be sure we will be able to have a type
    if (!type && !assignment)
    {
        if (allVars.size() == 1)
            return error(allVars.front()->token, "variable must be initialized because no type is specified");
        return error(allVars.front()->token.startLocation, allVars.back()->token.endLocation, "variables must be initialized because no type is specified");
    }

    currentScope->allocateSymTable();

    auto front = allVars.front();
    if (type)
        Ast::addChild(front, type);
    if (assignment)
        Ast::addChild(front, assignment);

    for (auto var : allVars)
    {
        var->astType       = type;
        var->astAssignment = assignment;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, var, SymbolKind::Variable);
    }

    return true;
}

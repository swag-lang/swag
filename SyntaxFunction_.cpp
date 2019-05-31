#include "pch.h"
#include "Global.h"
#include "Module.h"
#include "Diagnostic.h"

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto allParamsNode         = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::FuncDeclParams, currentScope, sourceFile->indexInModule, parent, false);
        allParamsNode->semanticFct = &SemanticJob::resolveFuncDeclParams;
        if (result)
            *result = allParamsNode;

        while (token.id != TokenId::SymRightParen)
        {
            auto paramNode         = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::VarDecl, currentScope, sourceFile->indexInModule, allParamsNode, false);
            paramNode->semanticFct = &SemanticJob::resolveVarDecl;

            SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
            paramNode->inheritToken(token);

            SWAG_CHECK(tokenizer.getToken(token));
            if (token.id == TokenId::SymColon)
            {
                SWAG_CHECK(eatToken(TokenId::SymColon));
                SWAG_CHECK(doTypeExpression(paramNode, &paramNode->astType));
                SWAG_CHECK(paramNode->astType);
            }

            if (token.id == TokenId::SymEqual)
            {
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->astAssignment));
            }

            if (token.id == TokenId::SymComma)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
            }
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doFuncDecl(AstNode* parent, AstNode** result)
{
    auto funcNode         = Ast::newNode(&sourceFile->poolFactory->astFuncDecl, AstNodeKind::FuncDecl, currentScope, sourceFile->indexInModule, parent, false);
    funcNode->semanticFct = &SemanticJob::resolveFuncDecl;
    if (result)
        *result = funcNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid function name '%s'", token.text.c_str())));
    Ast::assignToken(funcNode, token);

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = sourceFile->poolFactory->typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(sourceFile, funcNode->name, ScopeKind::Function, currentScope);
        newScope->allocateSymTable();
        typeInfo->name     = funcNode->name;
        funcNode->typeInfo = typeInfo;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode->token, newScope->name, SymbolKind::Function);
    }

	// Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters));
    }

    // Return type
    auto typeNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncDeclType, currentScope, sourceFile->indexInModule, funcNode, false);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = &SemanticJob::resolveFuncDeclType;
    if (token.id == TokenId::SymMinusGreat)
    {
        SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
        SWAG_CHECK(doTypeExpression(typeNode));
    }

    // Content of function
    auto curly = move(token);
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));

    return true;
}

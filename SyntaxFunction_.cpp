#include "pch.h"
#include "Global.h"
#include "Module.h"
#include "Diagnostic.h"

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result)
{
    auto paramsNode = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::FuncDeclParams, currentScope, sourceFile->indexInModule, parent, false);
    if (result)
        *result = paramsNode;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    while (token.id != TokenId::SymRightParen)
    {
        auto node = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::VarDecl, currentScope, sourceFile->indexInModule, paramsNode, false);
        Ast::assignToken(node, token);
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(node));

        if (token.id == TokenId::SymComma)
        {
			SWAG_CHECK(eatToken(TokenId::SymComma));
			SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
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

    // Parameters
    SWAG_CHECK(tokenizer.getToken(token));
    auto paramsNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncDeclParams, currentScope, sourceFile->indexInModule, funcNode, false);
    funcNode->parameters    = paramsNode;
    paramsNode->semanticFct = &SemanticJob::resolveFuncDeclParams;
    SWAG_CHECK(eatToken(TokenId::SymColon));
    SWAG_CHECK(doFuncDeclParameters(paramsNode));

    // Return type
    auto typeNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncDeclType, currentScope, sourceFile->indexInModule, funcNode, false);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = &SemanticJob::resolveFuncDeclType;
    if (token.id == TokenId::SymMinusGreat)
    {
        SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
        SWAG_CHECK(doTypeExpression(typeNode));
    }

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = sourceFile->poolFactory->typeInfoFunc.alloc();
        newScope             = Ast::newScope(sourceFile, funcNode->name, ScopeKind::Function, currentScope);
        newScope->allocateSymTable();
        typeInfo->name     = funcNode->name;
        funcNode->typeInfo = typeInfo;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode->token, newScope->name, SymbolKind::Function);
    }

    // Content of function
    auto curly = move(token);
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    auto savedScope = currentScope;
    currentScope    = newScope;

    currentScope = savedScope;

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

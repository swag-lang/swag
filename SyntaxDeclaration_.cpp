#include "pch.h"
#include "SyntaxJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Workspace.h"
#include "Stats.h"
#include "SourceFile.h"
#include "Module.h"
#include "Pool.h"
#include "PoolFactory.h"
#include "SymTable.h"

bool SyntaxJob::doNamespace(AstNode* parent, AstScope** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astScope, AstNodeType::Namespace, parent, false);
    node->parentScope = currentScope;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid namespace name '%s'", token.text.c_str())));
    node->name = token.text;
    node->name.computeCrc();
    node->token = move(token);

    auto newScope = sourceFile->module->newNamespace(currentScope, node->name);

    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    auto curly = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        currentScope = newScope;
        auto ok      = doTopLevel(node);
        currentScope = node->parentScope;
        SWAG_CHECK(ok && tokenizer.getToken(token));
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    return true;
}

bool SyntaxJob::doType(AstNode* parent, AstType** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astType, AstNodeType::Type, parent, false);
    node->semanticFct = &SemanticJob::resolveType;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::NativeType, syntaxError(token, format("invalid type name '%s'", token.text.c_str())));
    node->token = move(token);

    return true;
}

bool SyntaxJob::doVarDecl(AstNode* parent, AstVarDecl** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeType::VarDecl, parent, false);
    node->semanticFct = &SemanticJob::resolveVarDecl;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    node->name = token.text;
    node->name.computeCrc();
    node->token = move(token);

    SWAG_CHECK(eatToken(TokenId::SymColon));
    SWAG_CHECK(doType(node, &node->astType));
    SWAG_CHECK(node->astType);

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    node->scope = currentScope;
    currentScope->allocateSymTable();
    currentScope->symTable->registerSymbolNameNoLock(sourceFile->poolFactory, node->name, SymbolType::Variable);

    return true;
}

bool SyntaxJob::doTopLevel(AstNode* parent)
{
    switch (token.id)
    {
    case TokenId::SymSemiColon:
        break;
    case TokenId::KwdVar:
        SWAG_CHECK(doVarDecl(parent));
        break;
    case TokenId::KwdNamespace:
        SWAG_CHECK(doNamespace(parent));
        break;
    case TokenId::CompilerUnitTest:
        SWAG_CHECK(doCompilerUnitTest());
        break;
    default:
        syntaxError(token, format("invalid token '%s'", token.text.c_str()));
        return false;
    }

    return true;
}

#include "pch.h"
#include "Global.h"
#include "Module.h"

bool SyntaxJob::doNamespace(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Namespace, currentScope, parent, false);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid namespace name '%s'", token.text.c_str())));
    node->token = move(token);

    // Add/Get namespace
    Utf8Crc name = node->token.text;
    name.computeCrc();
    auto newScope = sourceFile->module->newNamespace(currentScope, name);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    auto curly = move(token);

    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        auto savedScope = currentScope;
        currentScope    = newScope;
        auto ok         = doTopLevel(node);
        currentScope    = savedScope;
        SWAG_CHECK(ok);
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doVarDecl(AstNode* parent, AstVarDecl** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeType::VarDecl, currentScope, parent, false);
    node->semanticFct = &SemanticJob::resolveVarDecl;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
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

    node->scope = currentScope;
    currentScope->allocateSymTable();
    currentScope->symTable->registerSymbolNameNoLock(sourceFile->poolFactory, node->name, SymbolKind::Variable);

    return true;
}

bool SyntaxJob::doTopLevel(AstNode* parent)
{
    switch (token.id)
    {
    case TokenId::SymSemiColon:
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdVar:
        SWAG_CHECK(doVarDecl(parent));
        break;
    case TokenId::KwdType:
        SWAG_CHECK(doTypeDecl(parent));
        break;
    case TokenId::KwdNamespace:
        SWAG_CHECK(doNamespace(parent));
        break;
    case TokenId::CompilerUnitTest:
        SWAG_CHECK(doCompilerUnitTest());
        break;
    case TokenId::CompilerAssert:
        SWAG_CHECK(doCompilerAssert(parent));
        break;
    case TokenId::CompilerPrint:
        SWAG_CHECK(doCompilerPrint(parent));
        break;
    case TokenId::CompilerRun:
        SWAG_CHECK(doCompilerRun(parent));
        break;
    default:
        syntaxError(token, format("invalid token '%s'", token.text.c_str()));
        return false;
    }

    return true;
}

#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "SymTable.h"

bool SyntaxJob::doTypeDecl(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::TypeDecl, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveTypeDecl;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid type name '%s'", token.text.c_str())));
    node->name = token.text;
    node->name.computeCrc();
    node->token = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymColon));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    currentScope->allocateSymTable();
    if (!isContextDisabled())
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, node, SymbolKind::Type);

    return true;
}

bool SyntaxJob::doTypeExpression(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astType, AstNodeKind::Type, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveTypeExpression;
    if (result)
        *result = node;

    // Array
    node->arrayDim = 0;
    if (token.id == TokenId::SymLeftSquare)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        while (true)
        {
            node->arrayDim++;
            SWAG_CHECK(doExpression(node));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(tokenizer.getToken(token));
        }

        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    }

    // Pointers
    node->ptrCount = 0;
    while (token.id == TokenId::SymAsterisk)
    {
        node->ptrCount++;
        SWAG_CHECK(tokenizer.getToken(token));
    }

    if (token.id == TokenId::NativeType)
    {
        node->token = move(token);
        SWAG_CHECK(tokenizer.getToken(token));
        return true;
    }

    if (token.id == TokenId::Identifier)
    {
        SWAG_CHECK(doIdentifierRef(node, &node->typeExpression));
        return true;
    }

    SWAG_CHECK(syntaxError(token, format("invalid type declaration '%s'", token.text.c_str())));
    return true;
}

bool SyntaxJob::doCast(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::Cast, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveCast;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

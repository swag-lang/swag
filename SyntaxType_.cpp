#include "pch.h"
#include "Global.h"
#include "Module.h"

bool SyntaxJob::doTypeDecl(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::TypeDecl, currentScope, parent, false);
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
    currentScope->symTable->registerSymbolNameNoLock(sourceFile->poolFactory, node->name, SymbolKind::TypeDecl);

    return true;
}

bool SyntaxJob::doIdentifier(AstNode* parent, AstIdentifier** result)
{
    auto identifier         = Ast::newNode(&sourceFile->poolFactory->astIdentifier, AstNodeType::Identifier, currentScope, parent, false);
    identifier->semanticFct = &SemanticJob::resolveIdentifier;
    identifier->token       = move(token);
    identifier->name        = identifier->token.text;
    identifier->name.computeCrc();
	if (result)
		*result = identifier;
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doTypeExpression(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Type, currentScope, parent, false);
    node->semanticFct = &SemanticJob::resolveTypeExpression;
    if (result)
        *result = node;

    if (token.id == TokenId::NativeType)
    {
        node->token = move(token);
        SWAG_CHECK(tokenizer.getToken(token));
        return true;
    }

    if (token.id == TokenId::Identifier)
    {
        AstIdentifier* identifier;
        SWAG_CHECK(doIdentifier(node, &identifier));
        identifier->symbolKind = SymbolKind::TypeDecl;
        return true;
    }

    SWAG_CHECK(notSupportedError(token));
    return true;
}

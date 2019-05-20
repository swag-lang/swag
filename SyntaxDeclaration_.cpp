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

bool SyntaxJob::doType(AstNode* parent, AstType** result)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astType, AstNodeType::Type, parent, false);
	node->semanticFct = &SemanticJob::resolveType;
	if (result)
		*result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::NativeType, syntaxError(format("invalid type name '%s'", token.text.c_str())));
    return true;
}

bool SyntaxJob::doVarDecl(AstNode* parent, AstVarDecl** result)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeType::VarDecl, parent, false);
    node->semanticFct = &SemanticJob::resolveVarDecl;
	if (result)
		*result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(format("invalid variable name '%s'", token.text.c_str())));
    node->name = token.text;

    SWAG_CHECK(eatToken(TokenId::SymColon));
	SWAG_CHECK(doType(node, &node->astType));
    SWAG_CHECK(node->astType);

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

	// A top level symbol must be registered in order to be found by the semantic solving of identifiers
	if (parent->flags & AST_IS_TOPLEVEL)
	{
		currentScope->allocateSymTable();
		currentScope->symTable->registerSyntaxSymbol(sourceFile->poolFactory, node->name, SymbolType::Variable);
	}

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
    default:
        syntaxError(format("invalid token '%s'", token.text.c_str()));
        return false;
    }

    return true;
}

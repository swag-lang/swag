#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"

bool SyntaxJob::doIdentifier(AstNode* parent, AstNode** result)
{
    auto identifier         = Ast::newNode(&sourceFile->poolFactory->astIdentifier, AstNodeKind::Identifier, currentScope, sourceFile->indexInModule, parent, false);
    identifier->semanticFct = &SemanticJob::resolveIdentifier;
    identifier->token       = move(token);
    identifier->name        = identifier->token.text;
    identifier->name.computeCrc();
    if (result)
        *result = identifier;
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymLeftParen)
    {
        auto callParams            = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncCallParams, currentScope, sourceFile->indexInModule, identifier, false);
        identifier->callParameters = callParams;
        callParams->semanticFct    = &SemanticJob::resolveFuncCallParams;
        callParams->token          = move(token);
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        while (token.id != TokenId::SymRightParen)
        {
			while(true)
			{
				SWAG_CHECK(doExpression(callParams));
				if (token.id != TokenId::SymComma)
					break;
                SWAG_CHECK(eatToken(TokenId::SymComma));
            }
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    return true;
}

bool SyntaxJob::doIdentifierRef(AstNode* parent, AstNode** result)
{
    auto identifierRef         = Ast::newNode(&sourceFile->poolFactory->astIdentifierRef, AstNodeKind::IdentifierRef, currentScope, sourceFile->indexInModule, parent, false);
    identifierRef->semanticFct = &SemanticJob::resolveIdentifierRef;
    if (result)
        *result = identifierRef;

    SWAG_CHECK(doIdentifier(identifierRef));
    while (token.id == TokenId::SymDot)
    {
        SWAG_CHECK(eatToken(TokenId::SymDot));
        SWAG_CHECK(doIdentifier(identifierRef));
    }

    identifierRef->inheritLocation();
    return true;
}

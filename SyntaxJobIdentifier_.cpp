#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Pool.h"
#include "PoolFactory.h"

bool SyntaxJob::doIdentifier(AstNode* parent)
{
    auto identifier = Ast::newNode(&sourceFile->poolFactory->astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, parent, false);
    identifier->inheritOwners(this);
    identifier->semanticFct = &SemanticJob::resolveIdentifier;
    identifier->byteCodeFct = &ByteCodeGenJob::emitIdentifier;
    identifier->inheritToken(token);
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymLeftParen)
    {
        auto callParams = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncCallParams, sourceFile->indexInModule, identifier, false);
        callParams->inheritOwners(this);
        identifier->callParameters = callParams;
        callParams->semanticFct    = &SemanticJob::resolveFuncCallParams;
        callParams->token          = move(token);

        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        while (token.id != TokenId::SymRightParen)
        {
            while (true)
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
    auto identifierRef = Ast::newNode(&sourceFile->poolFactory->astIdentifierRef, AstNodeKind::IdentifierRef, sourceFile->indexInModule, parent, false);
    identifierRef->inheritOwners(this);
    identifierRef->semanticFct = &SemanticJob::resolveIdentifierRef;
    identifierRef->byteCodeFct = &ByteCodeGenJob::emitIdentifierRef;
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

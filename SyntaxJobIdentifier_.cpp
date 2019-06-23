#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "Diagnostic.h"

bool SyntaxJob::doIdentifier(AstNode* parent, uint64_t flags)
{
    if (token.id != TokenId::Identifier &&
        token.id != TokenId::IntrisicPrint &&
        token.id != TokenId::IntrisicAssert)
        return sourceFile->report({sourceFile, token, format("invalid identifier '%s'", token.text.c_str())});

    // An identifier that starts with '__' is reserved for internal usage !
    if (token.text.length() > 1 && token.text[0] == '_' && token.text[1] == '_')
        return error(token, format("identifier '%s' starts with '__', and this is reserved by the language", token.text.c_str()));

    auto identifier = Ast::newNode(&g_Pool_astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, parent);
    identifier->inheritOwnersAndFlags(this);
    identifier->flags |= flags;
    identifier->semanticFct = &SemanticJob::resolveIdentifier;
    identifier->byteCodeFct = &ByteCodeGenJob::emitIdentifier;
    identifier->inheritToken(token);
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymLeftParen)
    {
        auto callParams = Ast::newNode(&g_Pool_astNode, AstNodeKind::FuncCallParams, sourceFile->indexInModule, identifier);
        callParams->inheritOwnersAndFlags(this);
        identifier->callParameters = callParams;
        callParams->semanticFct    = &SemanticJob::resolveFuncCallParams;
        callParams->token          = move(token);

        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        while (token.id != TokenId::SymRightParen)
        {
            while (true)
            {
                auto param = Ast::newNode(&g_Pool_astFuncCallParam, AstNodeKind::FuncCallParam, sourceFile->indexInModule, callParams);
                param->inheritOwnersAndFlags(this);
                param->semanticFct = &SemanticJob::resolveFuncCallParam;
                param->token       = token;
                SWAG_CHECK(doExpression(param));
                if (token.id != TokenId::SymComma)
                    break;
                SWAG_CHECK(eatToken(TokenId::SymComma));
            }
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    return true;
}

bool SyntaxJob::doIdentifierRef(AstNode* parent, AstNode** result, uint64_t flags)
{
    auto identifierRef = Ast::newNode(&g_Pool_astIdentifierRef, AstNodeKind::IdentifierRef, sourceFile->indexInModule, parent);
    identifierRef->inheritOwnersAndFlags(this);
    identifierRef->flags |= flags;
    identifierRef->semanticFct = &SemanticJob::resolveIdentifierRef;
    identifierRef->byteCodeFct = &ByteCodeGenJob::emitIdentifierRef;
    if (result)
        *result = identifierRef;

    SWAG_CHECK(doIdentifier(identifierRef, flags));
    while (token.id == TokenId::SymDot)
    {
        SWAG_CHECK(eatToken(TokenId::SymDot));
        SWAG_CHECK(doIdentifier(identifierRef, flags));
    }

    identifierRef->inheritLocation();
    return true;
}

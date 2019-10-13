#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "SemanticJob.h"
#include "Diagnostic.h"

bool SyntaxJob::doIdentifier(AstNode* parent, bool acceptParameters)
{
    // An identifier that starts with '__' is reserved for internal usage !
    if (token.text.length() > 1 && token.text[0] == '_' && token.text[1] == '_')
        return error(token, format("identifier '%s' starts with '__', and this is reserved by the language", token.text.c_str()));

    auto identifier = Ast::newNode(this, &g_Pool_astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, nullptr);
    identifier->inheritTokenName(token);
    identifier->semanticFct   = &SemanticJob::resolveIdentifier;
    identifier->identifierRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
    SWAG_CHECK(tokenizer.getToken(token));

    // Template arguments
    if (acceptParameters)
    {
        if (token.id == TokenId::SymExclam)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->genericParameters));
            identifier->genericParameters->flags |= AST_NO_BYTECODE;
        }

        // Function call parameters
        if (token.id == TokenId::SymLeftParen)
        {
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters));
        }
    }

	// Array index
    AstNode* expr = identifier;
    if (token.id == TokenId::SymLeftSquare)
        SWAG_CHECK(doArrayPointerIndex(&expr));
    Ast::addChildBack(parent, expr);

    return true;
}

bool SyntaxJob::doIdentifierRef(AstNode* parent, AstNode** result)
{
    auto identifierRef         = Ast::newNode(this, &g_Pool_astIdentifierRef, AstNodeKind::IdentifierRef, sourceFile->indexInModule, parent);
    identifierRef->semanticFct = &SemanticJob::resolveIdentifierRef;
    if (result)
        *result = identifierRef;

    SWAG_CHECK(doIdentifier(identifierRef));
    while (token.id == TokenId::SymDot)
    {
        SWAG_CHECK(eatToken(TokenId::SymDot));
        SWAG_CHECK(doIdentifier(identifierRef));
    }

    return true;
}

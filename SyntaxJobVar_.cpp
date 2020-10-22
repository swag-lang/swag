#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "Scoped.h"
#include "SymTable.h"

bool SyntaxJob::doVarDecl(AstNode* parent, AstNode** result)
{
    // First variable
    AstNodeKind kind;
    if (token.id == TokenId::KwdConst)
        kind = AstNodeKind::ConstDecl;
    else
        kind = AstNodeKind::VarDecl;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doVarDecl(parent, result, kind));
    return true;
}

bool SyntaxJob::doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    AstNode* leftNode;

    auto           saveToken = token;
    ScopedLocation sl(this, &saveToken);

    while (true)
    {
        SWAG_CHECK(doLeftExpressionVar(&leftNode, false));

        SWAG_VERIFY(token.id != TokenId::SymEqualEqual, syntaxError(token, "invalid compare operator '==', do you mean '=' ?"));
        SWAG_VERIFY(token.id == TokenId::SymColon || token.id == TokenId::SymEqual || token.id == TokenId::SymColonEqual, syntaxError(token, format("invalid token '%s' in variable declaration, ':', '=' or ':=' are expected here", token.text.c_str())));

        AstNode* type = nullptr;
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(nullptr, &type, true));
        }

        AstNode* assign = nullptr;
        SWAG_VERIFY(token.id != TokenId::SymEqualEqual, syntaxError(token, "invalid compare operator '==', do you mean '=' ?"));
        if (token.id == TokenId::SymEqual || token.id == TokenId::SymColonEqual)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doInitializationExpression(nullptr, &assign));
        }

        // Be sure we will be able to have a type
        if (!type && !assign)
            return error(leftNode->token, "variable must have a type or must be initialized");

        SWAG_CHECK(doVarDeclExpression(parent, leftNode, type, assign, kind, result));

        // If we have a type, and that type has parameters (struct construction), then we need to evaluate and push the parameters
        if (type && type->kind == AstNodeKind::TypeExpression)
        {
            auto typeExpression = CastAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
            if (typeExpression->identifier && typeExpression->identifier->kind == AstNodeKind::IdentifierRef)
            {
                auto identifier = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
                if (identifier->callParameters)
                {
                    typeExpression->flags &= ~AST_NO_BYTECODE_CHILDS;
                    typeExpression->flags |= AST_HAS_STRUCT_PARAMETERS;
                }
            }
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatSemiCol("at the end of a variable declation"));
    return true;
}

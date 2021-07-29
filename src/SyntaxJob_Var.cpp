#include "pch.h"
#include "Ast.h"
#include "ErrorIds.h"

bool SyntaxJob::doVarDecl(AstNode* parent, AstNode** result)
{
    // First variable
    AstNodeKind kind;
    if (token.id == TokenId::KwdConst)
        kind = AstNodeKind::ConstDecl;
    else
        kind = AstNodeKind::VarDecl;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(doVarDecl(parent, result, kind));
    return true;
}

bool SyntaxJob::doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    AstNode* leftNode;
    while (true)
    {
        SWAG_CHECK(doLeftExpressionVar(parent, &leftNode, IDENTIFIER_NO_PARAMS));
        Ast::removeFromParent(leftNode);

        SWAG_VERIFY(token.id != TokenId::SymEqualEqual, error(token, Msg0454));
        SWAG_VERIFY(token.id == TokenId::SymColon || token.id == TokenId::SymEqual, error(token, Utf8::format(Msg0455, token.text.c_str())));

        AstNode* type = nullptr;
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(parent, &type, true));
            Ast::removeFromParent(type);
        }

        AstNode* assign = nullptr;
        SWAG_VERIFY(token.id != TokenId::SymEqualEqual, error(token, Msg0454));
        if (token.id == TokenId::SymEqual)
        {
            auto saveToken = move(token);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doInitializationExpression(saveToken, parent, &assign));
            Ast::removeFromParent(assign);
        }

        // Be sure we will be able to have a type
        if (!type && !assign)
            return error(leftNode->token, Msg0457);

        SWAG_CHECK(doVarDeclExpression(parent, leftNode, type, assign, kind, result));

        // If we have a type, and that type has parameters (struct construction), then we need to evaluate and push the parameters
        if (type && type->kind == AstNodeKind::TypeExpression)
        {
            auto typeExpression = CastAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
            if (typeExpression->identifier && typeExpression->identifier->kind == AstNodeKind::IdentifierRef)
            {
                auto back = typeExpression->identifier->childs.back();
                if (back->kind == AstNodeKind::Identifier)
                {
                    auto identifier = CastAst<AstIdentifier>(back, AstNodeKind::Identifier);
                    if (identifier->callParameters)
                    {
                        typeExpression->flags &= ~AST_NO_BYTECODE_CHILDS;
                        typeExpression->flags |= AST_HAS_STRUCT_PARAMETERS;
                    }
                }
            }
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatSemiCol("end of a variable declation"));
    return true;
}

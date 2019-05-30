#include "pch.h"
#include "Global.h"
#include "Module.h"
#include "Diagnostic.h"

bool SyntaxJob::doAttributeDecl(AstNode* parent, AstNode** result)
{
    auto attrNode = Ast::newNode(&sourceFile->poolFactory->astAttribute, AstNodeKind::AttributeDecl, currentScope, sourceFile->indexInModule, parent, false);
    if (result)
        *result = attrNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid attribute name '%s'", token.text.c_str())));
    Ast::assignToken(attrNode, token);

    attrNode->attribute        = sourceFile->poolFactory->attribute.alloc();
    attrNode->attribute->name  = attrNode->name;
    attrNode->attribute->flags = 0;

    // Parameters
    SWAG_CHECK(tokenizer.getToken(token));
    auto paramsNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncDeclParams, currentScope, sourceFile->indexInModule, attrNode, false);
    attrNode->parameters    = paramsNode;
    paramsNode->semanticFct = &SemanticJob::resolveFuncDeclParams;
    SWAG_CHECK(eatToken(TokenId::SymColon));
    SWAG_CHECK(doFunctionDeclParameters(paramsNode));

    // Return type
    SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
    while (token.id != TokenId::SymSemiColon)
    {
        switch (token.id)
        {
        case TokenId::KwdFunc:
			SWAG_VERIFY((attrNode->attribute->flags & ATTRIBUTE_FUNC) == 0, syntaxError(token, "attribute type 'func' already defined"));
            attrNode->attribute->flags |= ATTRIBUTE_FUNC;
            break;
        case TokenId::KwdVar:
            SWAG_VERIFY((attrNode->attribute->flags & ATTRIBUTE_VAR) == 0, syntaxError(token, "attribute type 'var' already defined"));
            attrNode->attribute->flags |= ATTRIBUTE_VAR;
            break;
        default:
            return error(token, format("invalid attribute type '%s'", token.text.c_str()));
        }

        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id != TokenId::SymSemiColon)
        {
			SWAG_CHECK(eatToken(TokenId::SymComma));
			SWAG_VERIFY(token.id != TokenId::SymSemiColon, syntaxError(token, "missing attribute type"));
        }
    }

    SWAG_VERIFY(attrNode->attribute->flags, syntaxError(token, "missing attribute type"));
    return true;
}

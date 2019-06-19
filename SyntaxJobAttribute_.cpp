#include "pch.h"
#include "Global.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Scope.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "Ast.h"

bool SyntaxJob::doAttrDecl(AstNode* parent, AstNode** result)
{
    auto attrNode = Ast::newNode(&g_Pool_astAttrDecl, AstNodeKind::AttrDecl, sourceFile->indexInModule, parent);
    attrNode->inheritOwnersAndFlags(this);
    attrNode->semanticFct = &SemanticJob::resolveAttrDecl;
    if (result)
        *result = attrNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid attribute name '%s'", token.text.c_str())));
    Ast::assignToken(attrNode, token);

    attrNode->typeInfo       = g_Pool_typeInfoFuncAttr.alloc();
    attrNode->typeInfo->name = attrNode->name;

    // Parameters
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymColon));
    SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));

    // Return type
    SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
    while (token.id != TokenId::SymSemiColon)
    {
        switch (token.id)
        {
        case TokenId::KwdFunc:
            SWAG_VERIFY((attrNode->typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC) == 0, syntaxError(token, "attribute type 'func' already defined"));
            attrNode->typeInfo->flags |= TYPEINFO_ATTRIBUTE_FUNC;
            break;
        case TokenId::KwdVar:
            SWAG_VERIFY((attrNode->typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR) == 0, syntaxError(token, "attribute type 'var' already defined"));
            attrNode->typeInfo->flags |= TYPEINFO_ATTRIBUTE_VAR;
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

    SWAG_VERIFY(attrNode->typeInfo->flags, syntaxError(token, "missing attribute type"));

    // Register attribute
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, attrNode, SymbolKind::Attribute);
    }

    return true;
}

bool SyntaxJob::doAttrUse(AstNode* parent, AstNode** result)
{
    auto attrBlockNode = Ast::newNode(&g_Pool_astAttrUse, AstNodeKind::AttrUse, sourceFile->indexInModule, parent);
    attrBlockNode->inheritOwnersAndFlags(this);
    attrBlockNode->semanticFct = &SemanticJob::resolveAttrUse;
    if (result)
        *result = attrBlockNode;

    while (token.id == TokenId::SymAttrStart)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        while (token.id == TokenId::Identifier)
        {
            SWAG_CHECK(doIdentifierRef(attrBlockNode));
            if (token.id != TokenId::SymRightSquare)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid attribute name '%s'", token.text.c_str())));
            }
        }

        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    }

    return true;
}
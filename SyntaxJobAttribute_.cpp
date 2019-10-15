#include "pch.h"
#include "Global.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Scope.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"

bool SyntaxJob::doAttrDecl(AstNode* parent, AstNode** result)
{
    auto attrNode         = Ast::newNode(this, &g_Pool_astAttrDecl, AstNodeKind::AttrDecl, sourceFile->indexInModule, parent);
    attrNode->semanticFct = &SemanticJob::resolveAttrDecl;
    if (result)
        *result = attrNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid attribute name '%s'", token.text.c_str())));

    attrNode->inheritTokenName(token);

    // Register attribute
	currentScope->allocateSymTable();
    scoped_lock lk(currentScope->symTable->mutex);
    auto        typeInfo = g_Pool_typeInfoFuncAttr.alloc();
    auto        newScope = Ast::newScope(attrNode, attrNode->name, ScopeKind::Attribute, currentScope);
    newScope->allocateSymTable();
    attrNode->typeInfo = typeInfo;
    currentScope->symTable->registerSymbolNameNoLock(sourceFile, attrNode, SymbolKind::Attribute);

    // Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));
    }

    // Return type
    SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
    while (true)
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
        case TokenId::KwdStruct:
		case TokenId::KwdUnion:
            SWAG_VERIFY((attrNode->typeInfo->flags & TYPEINFO_ATTRIBUTE_STRUCT) == 0, syntaxError(token, "attribute type 'struct' already defined"));
            attrNode->typeInfo->flags |= TYPEINFO_ATTRIBUTE_STRUCT;
            break;
        case TokenId::KwdEnum:
            SWAG_VERIFY((attrNode->typeInfo->flags & TYPEINFO_ATTRIBUTE_ENUM) == 0, syntaxError(token, "attribute type 'enum' already defined"));
            attrNode->typeInfo->flags |= TYPEINFO_ATTRIBUTE_ENUM;
            break;
        case TokenId::KwdEnumValue:
            SWAG_VERIFY((attrNode->typeInfo->flags & TYPEINFO_ATTRIBUTE_ENUMVALUE) == 0, syntaxError(token, "attribute type 'enumvalue' already defined"));
            attrNode->typeInfo->flags |= TYPEINFO_ATTRIBUTE_ENUMVALUE;
            break;
        default:
            return error(token, format("invalid attribute type '%s'", token.text.c_str()));
        }

        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatSemiCol("after attribute definition"));
    SWAG_VERIFY(attrNode->typeInfo->flags, syntaxError(token, "missing attribute type"));

    return true;
}

bool SyntaxJob::doAttrUse(AstNode* parent, AstNode** result)
{
    auto attrBlockNode         = Ast::newNode(this, &g_Pool_astAttrUse, AstNodeKind::AttrUse, sourceFile->indexInModule, parent);
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
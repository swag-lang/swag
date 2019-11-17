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
    auto attrNode         = Ast::newNode(this, &g_Pool_astAttrDecl, AstNodeKind::AttrDecl, sourceFile, parent);
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
    currentScope->symTable->registerSymbolNameNoLock(&context, attrNode, SymbolKind::Attribute);

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
        if (token.id != TokenId::LiteralString)
            return error(token, format("string requested instead of '%s'", token.text.c_str()));

        if (token.text == "func")
        {
            SWAG_VERIFY((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_FUNC) == 0, syntaxError(token, "attribute constraint 'func' already defined"));
            typeInfo->attributeFlags |= TYPEINFO_ATTRIBUTE_FUNC;
        }
        else if (token.text == "macro")
        {
            SWAG_VERIFY((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_MACRO) == 0, syntaxError(token, "attribute constraint 'macro' already defined"));
            typeInfo->attributeFlags |= TYPEINFO_ATTRIBUTE_MACRO;
        }
        else if (token.text == "var")
        {
            SWAG_VERIFY((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_VAR) == 0, syntaxError(token, "attribute constraint 'var' already defined"));
            typeInfo->attributeFlags |= TYPEINFO_ATTRIBUTE_VAR;
        }
        else if (token.text == "structvar")
        {
            SWAG_VERIFY((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_STRUCTVAR) == 0, syntaxError(token, "attribute constraint 'structvar' already defined"));
            typeInfo->attributeFlags |= TYPEINFO_ATTRIBUTE_STRUCTVAR;
        }
        else if (token.text == "struct" || token.text == "union")
        {
            SWAG_VERIFY((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_STRUCT) == 0, syntaxError(token, "attribute constraint 'struct' already defined"));
            typeInfo->attributeFlags |= TYPEINFO_ATTRIBUTE_STRUCT;
        }
        else if (token.text == "enum")
        {
            SWAG_VERIFY((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_ENUM) == 0, syntaxError(token, "attribute constraint 'enum' already defined"));
            typeInfo->attributeFlags |= TYPEINFO_ATTRIBUTE_ENUM;
        }
        else if (token.text == "enumvalue")
        {
            SWAG_VERIFY((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_ENUMVALUE) == 0, syntaxError(token, "attribute constraint 'enumvalue' already defined"));
            typeInfo->attributeFlags |= TYPEINFO_ATTRIBUTE_ENUMVALUE;
        }
        else
        {
            return error(token, format("invalid attribute constraint '%s'", token.text.c_str()));
        }

        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatSemiCol("after attribute definition"));
    SWAG_VERIFY(typeInfo->attributeFlags, syntaxError(token, "missing attribute constraint"));

    return true;
}

bool SyntaxJob::doAttributeExpose(AstNode* parent, AstNode** result)
{
    uint32_t attr = 0;

    Scope* newScope = currentScope;

    switch (token.id)
    {
    case TokenId::KwdPrivate:
        attr = ATTRIBUTE_PRIVATE;
        SWAG_VERIFY(currentScope->isGlobal(), error(token, "a private definition must appear at file or namespace scope"));
        newScope = sourceFile->scopePrivate;
        break;
    case TokenId::KwdPublic:
        attr = ATTRIBUTE_PUBLIC;
        SWAG_VERIFY(currentScope->isGlobal() || currentScope->kind == ScopeKind::Struct, error(token, "a public definition must appear at file or namespace scope"));
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    SWAG_CHECK(tokenizer.getToken(token));

    switch (token.id)
    {
    case TokenId::SymLeftCurly:
    case TokenId::KwdFunc:
    case TokenId::KwdVar:
    case TokenId::KwdLet:
    case TokenId::KwdConst:
    case TokenId::KwdEnum:
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::KwdTypeAlias:
        break;

    default:
		if(attr == ATTRIBUTE_PRIVATE)
			return syntaxError(token, format("unexpected token '%s' after 'private' attribute", token.text.c_str()));
		return syntaxError(token, format("unexpected token '%s' after 'public' attribute", token.text.c_str()));
    }

    Scoped                scoped(this, newScope);
    ScopedAttributesFlags scopedAttributes(this, attr);
    SWAG_CHECK(doTopLevelInstruction(parent));
    return true;
}

bool SyntaxJob::doAttrUse(AstNode* parent, AstNode** result)
{
    auto attrBlockNode         = Ast::newNode(this, &g_Pool_astAttrUse, AstNodeKind::AttrUse, sourceFile, parent);
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
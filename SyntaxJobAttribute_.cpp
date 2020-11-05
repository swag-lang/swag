#include "pch.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"

bool SyntaxJob::doAttrDecl(AstNode* parent, AstNode** result)
{
    auto attrNode               = Ast::newNode<AstAttrDecl>(this, AstNodeKind::AttrDecl, sourceFile, parent);
    attrNode->semanticBeforeFct = SemanticJob::preResolveAttrDecl;
    attrNode->semanticFct       = SemanticJob::resolveAttrDecl;
    if (result)
        *result = attrNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid attribute name '%s'", token.text.c_str())));

    attrNode->inheritTokenName(token);

    // Register attribute
    scoped_lock lk(currentScope->symTable.mutex);
    auto        typeInfo = g_Allocator.alloc<TypeInfoFuncAttr>();
    typeInfo->declNode   = attrNode;

    auto newScope      = Ast::newScope(attrNode, attrNode->name, ScopeKind::Attribute, currentScope);
    attrNode->typeInfo = typeInfo;
    currentScope->symTable.registerSymbolNameNoLock(&context, attrNode, SymbolKind::Attribute);

    // Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));
    }

    SWAG_CHECK(eatSemiCol("after attribute definition"));

    return true;
}

bool SyntaxJob::doGlobalAttributeExpose(AstNode* parent, AstNode** result)
{
    uint32_t attr = 0;

    Scope* newScope = currentScope;

    switch (token.id)
    {
    case TokenId::KwdPrivate:
        attr = ATTRIBUTE_PRIVATE;
        SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, "a private definition must appear at file or namespace scope"));
        newScope = sourceFile->scopePrivate;
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdPublic:
        attr = ATTRIBUTE_PUBLIC;
        SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, "a public definition must appear at file or namespace scope"));
        SWAG_VERIFY(!sourceFile->forcedPublic, error(token, "'public' attribute cannot be used in a file marked with '#public', because the whole file is implicitly public"));
        if (sourceFile->fromTests && currentScope->kind == ScopeKind::File)
            newScope = currentScope->parentScope;
        else
            SWAG_VERIFY(!(currentScope->flags & SCOPE_PRIVATE), error(token, "cannot declare a public symbol in a private scope"));
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    default:
        break;
    }

    SWAG_ASSERT(newScope);
    switch (token.id)
    {
    case TokenId::SymLeftCurly:
    case TokenId::KwdFunc:
    case TokenId::KwdAttr:
    case TokenId::KwdVar:
    case TokenId::KwdConst:
    case TokenId::KwdEnum:
    case TokenId::KwdStruct:
    case TokenId::KwdNamespace:
    case TokenId::KwdImpl:
    case TokenId::KwdInterface:
    case TokenId::KwdTypeSet:
    case TokenId::KwdAlias:
    case TokenId::KwdUsing:
    case TokenId::CompilerBake:
        break;

    default:
        if (attr == ATTRIBUTE_PRIVATE)
            return syntaxError(token, format("unexpected token '%s' after 'private' attribute", token.text.c_str()));
        return syntaxError(token, format("unexpected token '%s' after 'public' attribute", token.text.c_str()));
    }

    AstNode* topStmt = nullptr;

    {
        Scoped               scoped(this, newScope);
        ScopedAttributeFlags scopedAttributes(this, attr);
        SWAG_CHECK(doTopLevelInstruction(parent, &topStmt));
        if (result)
            *result = topStmt;
    }

    // Add original scope
    if (topStmt)
    {
        SWAG_RACE_CONDITION_WRITE(topStmt->raceConditionAlternativeScopes);
        topStmt->alternativeScopes.push_back(currentScope);
    }

    return true;
}

bool SyntaxJob::doAttrUse(AstNode* parent, AstNode** result)
{
    auto attrBlockNode         = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    attrBlockNode->semanticFct = SemanticJob::resolveAttrUse;
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
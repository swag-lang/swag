#include "pch.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "ErrorIds.h"

bool SyntaxJob::doAttrDecl(AstNode* parent, AstNode** result)
{
    auto attrNode = Ast::newNode<AstAttrDecl>(this, AstNodeKind::AttrDecl, sourceFile, parent);
    attrNode->allocateExtension();
    attrNode->extension->semanticBeforeFct = SemanticJob::preResolveAttrDecl;
    attrNode->semanticFct                  = SemanticJob::resolveAttrDecl;
    if (result)
        *result = attrNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, format(Msg0355, token.text.c_str())));

    attrNode->inheritTokenName(token);

    // Register attribute
    scoped_lock lk(currentScope->symTable.mutex);
    auto        typeInfo = allocType<TypeInfoFuncAttr>();
    typeInfo->declNode   = attrNode;

    auto newScope      = Ast::newScope(attrNode, attrNode->token.text, ScopeKind::Attribute, currentScope);
    attrNode->typeInfo = typeInfo;
    currentScope->symTable.registerSymbolNameNoLock(&context, attrNode, SymbolKind::Attribute);

    // Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));
    }

    SWAG_CHECK(eatSemiCol("attribute definition"));

    return true;
}

bool SyntaxJob::doGlobalAttributeExpose(AstNode* parent, AstNode** result, bool forGlobal)
{
    uint32_t attr     = 0;
    Scope*   newScope = currentScope;

    switch (token.id)
    {
    case TokenId::KwdPrivate:
        attr = ATTRIBUTE_PRIVATE;
        SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, Msg0348));
        newScope = Ast::newPrivateScope(parent, parent->sourceFile, currentScope);
        SWAG_CHECK(tokenizer.getToken(token));
        break;

    case TokenId::KwdPublic:
    case TokenId::KwdProtected:
        if (token.id == TokenId::KwdPublic)
            attr = ATTRIBUTE_PUBLIC;
        else
            attr = ATTRIBUTE_PROTECTED;

        SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, format(Msg0349, token.text.c_str())));
        SWAG_VERIFY(!sourceFile->forceExport, error(token, format(Msg0350, token.text.c_str())));
        if (newScope->flags & SCOPE_PRIVATE)
        {
            SWAG_VERIFY(newScope->isTopLevel(), error(token, format(Msg0351, token.text.c_str(), Scope::getNakedKindName(newScope->kind), newScope->name.c_str())));
            while (newScope->flags & SCOPE_PRIVATE)
                newScope = newScope->parentScope;
        }
        SWAG_CHECK(tokenizer.getToken(token));
        break;

    default:
        break;
    }

    SWAG_ASSERT(newScope);
    Scoped scoped(this, newScope);
    auto   attrUse = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    if (result)
        *result = attrUse;
    attrUse->attributeFlags = attr;

    AstNode* topStmt = nullptr;

    if (!forGlobal)
    {
        // Check following instruction
        switch (token.id)
        {
        case TokenId::SymLeftCurly:
        case TokenId::KwdFunc:
        case TokenId::KwdAttr:
        case TokenId::KwdVar:
        case TokenId::KwdConst:
        case TokenId::KwdEnum:
        case TokenId::KwdStruct:
        case TokenId::KwdUnion:
        case TokenId::KwdImpl:
        case TokenId::KwdInterface:
        case TokenId::KwdTypeSet:
        case TokenId::KwdAlias:
        case TokenId::KwdNamespace:
            break;

        default:
            if (attr == ATTRIBUTE_PRIVATE)
                return error(token, format(Msg0352, token.text.c_str()));
            if (attr == ATTRIBUTE_PROTECTED)
                return error(token, format(Msg0353, token.text.c_str()));
            return error(token, format(Msg0354, token.text.c_str()));
        }

        SWAG_CHECK(doTopLevelInstruction(attrUse, &topStmt));
    }
    else
    {
        attrUse->flags |= AST_GLOBAL_NODE;
        topStmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, attrUse);
        topStmt->flags |= AST_GLOBAL_NODE;
        while (token.id != TokenId::EndOfFile)
            SWAG_CHECK(doTopLevelInstruction(topStmt));
    }

    attrUse->content = topStmt;
    attrUse->content->setOwnerAttrUse(attrUse);

    // Add original scope
    if (topStmt)
    {
        topStmt->allocateExtension();
        topStmt->extension->alternativeScopes.push_back(currentScope);
    }

    return true;
}

bool SyntaxJob::doAttrUse(AstNode* parent, AstNode** result, bool single)
{
    auto attrBlockNode = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    if (result)
        *result = attrBlockNode;

    while (token.id == TokenId::SymAttrStart)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        while (token.id == TokenId::Identifier)
        {
            AstNode* params;
            SWAG_CHECK(doIdentifierRef(attrBlockNode, &params));
            params->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

            if (token.id != TokenId::SymRightSquare)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier, error(token, format(Msg0355, token.text.c_str())));
            }
        }

        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
        if (single)
            break;
    }

    SWAG_VERIFY(!attrBlockNode->childs.empty(), error(attrBlockNode, Msg0356));
    auto back = attrBlockNode->childs.back();
    back->allocateExtension();
    SWAG_ASSERT(!back->extension->semanticAfterFct);
    back->extension->semanticAfterFct = SemanticJob::resolveAttrUse;
    return true;
}
#include "pch.h"
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

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Utf8::format(g_E[Err0355], token.text.c_str())));

    attrNode->inheritTokenName(token);

    auto typeInfo      = allocType<TypeInfoFuncAttr>();
    typeInfo->declNode = attrNode;
    attrNode->typeInfo = typeInfo;

    // Register attribute
    Scope* newScope = nullptr;
    {
        ScopedLock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(attrNode, attrNode->token.text, ScopeKind::Attribute, currentScope);
        currentScope->symTable.registerSymbolNameNoLock(&context, attrNode, SymbolKind::Attribute);
    }

    // Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));
    }

    SWAG_VERIFY(token.id != TokenId::SymMinusGreat, error(token, g_E[Err0726], g_E[Hlp0000]));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, g_E[Err0190], g_E[Hlp0001]));
    SWAG_CHECK(eatSemiCol("attribute definition"));

    return true;
}

bool SyntaxJob::doGlobalAttributeExpose(AstNode* parent, AstNode** result, bool forGlobal)
{
    uint32_t attr      = 0;
    Scope*   newScope  = currentScope;
    auto     tokenAttr = token;

    switch (token.id)
    {
    case TokenId::KwdPublic:
    case TokenId::KwdPrivate:
        if (token.id == TokenId::KwdPublic)
            attr = ATTRIBUTE_PUBLIC;
        else
            attr = ATTRIBUTE_PRIVATE;

        SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, Utf8::format(g_E[Err0349], token.text.c_str())));
        SWAG_VERIFY(!sourceFile->forceExport, error(token, Utf8::format(g_E[Err0350], token.text.c_str())));
        if (newScope->flags & SCOPE_FILE)
            newScope = newScope->parentScope;
        SWAG_CHECK(eatToken());
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
        case TokenId::KwdMethod:
        case TokenId::KwdConstMethod:
        case TokenId::KwdAttr:
        case TokenId::KwdVar:
        case TokenId::KwdConst:
        case TokenId::KwdEnum:
        case TokenId::KwdStruct:
        case TokenId::KwdUnion:
        case TokenId::KwdImpl:
        case TokenId::KwdInterface:
        case TokenId::KwdAlias:
        case TokenId::KwdNamespace:
            break;

        case TokenId::SymAttrStart:
        {
            PushErrHint eh(Utf8::format(g_E[Hnt0043], tokenAttr.text.c_str()));
            return error(token, Utf8::format(g_E[Err0354], token.text.c_str(), tokenAttr.text.c_str()));
        }

        default:
            return error(token, Utf8::format(g_E[Err0353], token.text.c_str(), tokenAttr.text.c_str()));
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
        topStmt->addAlternativeScope(currentScope);

    return true;
}

bool SyntaxJob::doAttrUse(AstNode* parent, AstNode** result, bool single)
{
    auto attrBlockNode = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    if (result)
        *result = attrBlockNode;

    while (token.id == TokenId::SymAttrStart)
    {
        SWAG_CHECK(eatToken());
        while (token.id == TokenId::Identifier)
        {
            AstNode* params;
            SWAG_CHECK(doIdentifierRef(attrBlockNode, &params));
            params->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

            SWAG_VERIFY(token.id == TokenId::SymRightSquare || token.id == TokenId::SymComma, error(token, Utf8::format(g_E[Err0825], token.text.c_str())));

            if (token.id != TokenId::SymRightSquare)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Utf8::format(g_E[Err0355], token.text.c_str())));
            }
        }

        attrBlockNode->token.endLocation = token.endLocation;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
        if (single)
            break;
    }

    // :AttrUseLastChild
    SWAG_VERIFY(!attrBlockNode->childs.empty(), error(attrBlockNode, g_E[Err0356]));
    auto back = attrBlockNode->childs.back();
    back->allocateExtension();
    SWAG_ASSERT(!back->extension->semanticAfterFct);
    back->extension->semanticAfterFct = SemanticJob::resolveAttrUse;

    return true;
}
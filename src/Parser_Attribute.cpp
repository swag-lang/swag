#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

bool Parser::doAttrDecl(AstNode* parent, AstNode** result)
{
    auto attrNode = Ast::newNode<AstAttrDecl>(this, AstNodeKind::AttrDecl, sourceFile, parent);
    *result       = attrNode;
    attrNode->allocateExtension(ExtensionKind::Semantic);
    attrNode->extSemantic()->semanticBeforeFct = SemanticJob::preResolveAttrDecl;
    attrNode->semanticFct                      = SemanticJob::resolveAttrDecl;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0072), token.ctext())));

    attrNode->inheritTokenName(token);

    auto typeInfo      = makeType<TypeInfoFuncAttr>();
    typeInfo->declNode = attrNode;
    attrNode->typeInfo = typeInfo;
    typeInfo->flags |= TYPEINFO_FUNC_IS_ATTR;

    // Register attribute
    Scope* newScope = nullptr;
    {
        ScopedLock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(attrNode, attrNode->token.text, ScopeKind::Attribute, currentScope);
        currentScope->symTable.registerSymbolNameNoLock(context, attrNode, SymbolKind::Attribute);
    }

    // Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));
    }

    SWAG_VERIFY(token.id != TokenId::SymMinusGreat, error(token, Err(Syn0138), Hlp(Hlp0000)));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Err(Err0849), Hlp(Hlp0001)));
    SWAG_CHECK(eatSemiCol("attribute definition"));

    //////
    if (sourceFile->isBootstrapFile || sourceFile->isRuntimeFile)
    {
        if (attrNode->token.text == g_LangSpec->name_Align)
            typeInfo->attributeUsage = 0;
        else if (attrNode->token.text == g_LangSpec->name_Strict)
            typeInfo->attributeUsage = 0;
        else if (attrNode->token.text == g_LangSpec->name_Global)
            typeInfo->attributeUsage = 0;
    }

    return true;
}

bool Parser::doGlobalAttributeExpose(AstNode* parent, AstNode** result, bool forGlobal)
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

        sourceFile->globalAttr |= attr;
        SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, Fmt(Err(Syn0032), token.ctext())));
        SWAG_VERIFY(!sourceFile->forceExport, error(token, Fmt(Err(Syn0018), token.ctext())));
        if (newScope->flags & SCOPE_FILE)
            newScope = newScope->parentScope;
        SWAG_CHECK(eatToken());
        break;

    default:
        break;
    }

    SWAG_ASSERT(newScope);
    Scoped scoped(this, newScope);
    auto   attrUse          = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    *result                 = attrUse;
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
            return error(token, Fmt(Err(Syn0150), token.ctext(), tokenAttr.ctext()), nullptr, Fmt(Hnt(Hnt0043), tokenAttr.ctext()));

        default:
            return error(token, Fmt(Err(Syn0174), token.ctext(), tokenAttr.ctext()));
        }

        SWAG_CHECK(doTopLevelInstruction(attrUse, &topStmt));
    }
    else
    {
        attrUse->flags |= AST_GLOBAL_NODE;
        topStmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, attrUse);
        topStmt->flags |= AST_GLOBAL_NODE;
        while (token.id != TokenId::EndOfFile)
            SWAG_CHECK(doTopLevelInstruction(topStmt, &dummyResult));
    }

    attrUse->content = topStmt;
    attrUse->content->setOwnerAttrUse(attrUse);

    // Add original scope
    if (topStmt)
        topStmt->addAlternativeScope(currentScope);

    return true;
}

bool Parser::doAttrUse(AstNode* parent, AstNode** result, bool single)
{
    auto attrBlockNode = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    *result            = attrBlockNode;

    if (tokenizer.comment.length())
    {
        attrBlockNode->allocateExtension(ExtensionKind::Misc);
        attrBlockNode->extMisc()->docComment = std::move(tokenizer.comment);
    }

    while (token.id == TokenId::SymAttrStart)
    {
        SWAG_CHECK(eatToken());
        while (token.id == TokenId::Identifier)
        {
            AstNode* params;
            SWAG_CHECK(doIdentifierRef(attrBlockNode, &params));
            params->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

            SWAG_VERIFY(token.id == TokenId::SymRightSquare || token.id == TokenId::SymComma, error(token, Fmt(Err(Syn0050), token.ctext())));

            if (token.id != TokenId::SymRightSquare)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0072), token.ctext())));
            }
        }

        attrBlockNode->token.endLocation = token.endLocation;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
        if (single)
            break;
    }

    // :AttrUseLastChild
    SWAG_VERIFY(!attrBlockNode->childs.empty(), error(attrBlockNode, Err(Syn0159)));
    auto back = attrBlockNode->childs.back();
    back->allocateExtension(ExtensionKind::Semantic);
    SWAG_ASSERT(!back->extSemantic()->semanticAfterFct);
    back->extSemantic()->semanticAfterFct = SemanticJob::resolveAttrUse;

    return true;
}
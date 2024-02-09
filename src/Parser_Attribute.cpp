#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Semantic.h"
#include "SourceFile.h"
#include "TypeManager.h"

bool Parser::doAttrDecl(AstNode* parent, AstNode** result)
{
    const auto attrNode = Ast::newNode<AstAttrDecl>(this, AstNodeKind::AttrDecl, sourceFile, parent);
    *result             = attrNode;
    attrNode->allocateExtension(ExtensionKind::Semantic);
    attrNode->extSemantic()->semanticBeforeFct = Semantic::preResolveAttrDecl;
    attrNode->semanticFct                      = Semantic::resolveAttrDecl;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0219), token.ctext())));

    attrNode->inheritTokenName(token);
    attrNode->tokenName = static_cast<Token>(token);

    const auto typeInfo = makeType<TypeInfoFuncAttr>();
    typeInfo->declNode  = attrNode;
    attrNode->typeInfo  = typeInfo;
    typeInfo->addFlag(TYPEINFO_FUNC_IS_ATTR);

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

    SWAG_VERIFY(token.id != TokenId::SymMinusGreat, error(token, Err(Err0676), Nte(Nte0020)));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Err(Err0671), Nte(Nte0020)));
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

bool Parser::doAttrUse(AstNode* parent, AstNode** result, bool single)
{
    const auto attrBlockNode = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    *result                  = attrBlockNode;

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
            params->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS);

            SWAG_VERIFY(token.id == TokenId::SymRightSquare || token.id == TokenId::SymComma, error(token, FMT(Err(Err0220), token.ctext())));

            if (token.id != TokenId::SymRightSquare)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma, "to use another attribute, or ']' to end"));
                SWAG_VERIFY(token.id != TokenId::SymLeftParen, error(token, Err(Err0543)));
                SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0219), token.ctext())));
            }
        }

        attrBlockNode->token.endLocation = token.endLocation;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare, "to end the attribute list"));
        if (single)
            break;
    }

    // :AttrUseLastChild
    SWAG_VERIFY(!attrBlockNode->childs.empty(), error(attrBlockNode, Err(Err0263)));
    const auto back = attrBlockNode->childs.back();
    back->allocateExtension(ExtensionKind::Semantic);
    SWAG_ASSERT(!back->extSemantic()->semanticAfterFct);
    back->extSemantic()->semanticAfterFct = Semantic::resolveAttrUse;

    return true;
}

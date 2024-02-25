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
    const auto attrNode = Ast::newNode<AstAttrDecl>(AstNodeKind::AttrDecl, this, parent);
    *result             = attrNode;
    attrNode->allocateExtension(ExtensionKind::Semantic);
    attrNode->extSemantic()->semanticBeforeFct = Semantic::preResolveAttrDecl;
    attrNode->semanticFct                      = Semantic::resolveAttrDecl;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0219, tokenParse.token.c_str())));

    attrNode->inheritTokenName(tokenParse.token);
    attrNode->tokenName = tokenParse.token;

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

    SWAG_VERIFY(tokenParse.token.id != TokenId::SymMinusGreat, error(tokenParse.token, toErr(Err0676), toNte(Nte0020)));
    SWAG_VERIFY(tokenParse.token.id != TokenId::KwdThrow, error(tokenParse.token, toErr(Err0671), toNte(Nte0020)));
    SWAG_CHECK(eatSemiCol("attribute definition"));

    //////
    if (sourceFile->hasFlag(FILE_IS_BOOTSTRAP_FILE) || sourceFile->hasFlag(FILE_IS_RUNTIME_FILE))
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
    const auto attrBlockNode = Ast::newNode<AstAttrUse>(AstNodeKind::AttrUse, this, parent);
    *result                  = attrBlockNode;

    if (tokenizer.comment.length())
    {
        attrBlockNode->allocateExtension(ExtensionKind::Misc);
        attrBlockNode->extMisc()->docComment = std::move(tokenizer.comment);
    }

    while (tokenParse.token.id == TokenId::SymAttrStart)
    {
        SWAG_CHECK(eatToken());
        while (tokenParse.token.id == TokenId::Identifier)
        {
            AstNode* params;
            SWAG_CHECK(doIdentifierRef(attrBlockNode, &params));
            params->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

            SWAG_VERIFY(tokenParse.token.id == TokenId::SymRightSquare || tokenParse.token.id == TokenId::SymComma, error(tokenParse.token, formErr(Err0220, tokenParse.token.c_str())));

            if (tokenParse.token.id != TokenId::SymRightSquare)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma, "to use another attribute, or ']' to end"));
                SWAG_VERIFY(tokenParse.token.id != TokenId::SymLeftParen, error(tokenParse.token, toErr(Err0543)));
                SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0219, tokenParse.token.c_str())));
            }
        }

        attrBlockNode->token.endLocation = tokenParse.token.endLocation;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare, "to end the attribute list"));
        if (single)
            break;
    }

    // :AttrUseLastChild
    SWAG_VERIFY(!attrBlockNode->children.empty(), error(attrBlockNode, toErr(Err0263)));
    const auto back = attrBlockNode->lastChild();
    back->allocateExtension(ExtensionKind::Semantic);
    SWAG_ASSERT(!back->extSemantic()->semanticAfterFct);
    back->extSemantic()->semanticAfterFct = Semantic::resolveAttrUse;

    return true;
}

#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"

bool Parser::doAttrStart(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(doAttrUse(parent, result));
    const auto attrUse = castAst<AstAttrUse>(*result, AstNodeKind::AttrUse);
    // We do not want a #[] to create a new scope when inside a function
    if (tokenParse.is(TokenId::SymLeftCurly))
        SWAG_CHECK(doCurlyStatement(attrUse, &attrUse->content));
    else
        SWAG_CHECK(doEmbeddedInstruction(attrUse, &attrUse->content));
    if (attrUse->content)
        attrUse->content->setOwnerAttrUse(attrUse);
    return true;
}

bool Parser::doAttrDecl(AstNode* parent, AstNode** result)
{
    const auto attrNode = Ast::newNode<AstAttrDecl>(AstNodeKind::AttrDecl, this, parent);
    *result             = attrNode;
    attrNode->allocateExtension(ExtensionKind::Semantic);
    attrNode->extSemantic()->semanticBeforeFct = Semantic::preResolveAttrDecl;
    attrNode->semanticFct                      = Semantic::resolveAttrDecl;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0623)));

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
        ParserPushScope scoped(this, newScope);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));
    }

    SWAG_VERIFY(tokenParse.isNot(TokenId::SymMinusGreat), error(tokenParse, toErr(Err0624), "an attribute is not a function; consider removing it"));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdThrow), error(tokenParse, toErr(Err0346), "an attribute is not a function; consider removing it"));
    SWAG_CHECK(eatSemiCol("attribute definition"));

    //////
    if (sourceFile->hasFlag(FILE_BOOTSTRAP) || sourceFile->hasFlag(FILE_RUNTIME))
    {
        if (attrNode->token.is(g_LangSpec->name_Align))
            typeInfo->attributeUsage = 0;
        else if (attrNode->token.is(g_LangSpec->name_Strict))
            typeInfo->attributeUsage = 0;
        else if (attrNode->token.is(g_LangSpec->name_Global))
            typeInfo->attributeUsage = 0;
    }

    return true;
}

bool Parser::doAttrUse(AstNode* parent, AstNode** result, bool single)
{
    const auto attrBlockNode = Ast::newNode<AstAttrUse>(AstNodeKind::AttrUse, this, parent);
    *result                  = attrBlockNode;

    while (tokenParse.is(TokenId::SymAttrStart))
    {
        SWAG_CHECK(eatToken());
        while (tokenParse.is(TokenId::Identifier))
        {
            AstNode* params;
            SWAG_CHECK(doIdentifierRef(attrBlockNode, &params));
            params->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

            if (tokenParse.is(TokenId::SymRightSquare))
                break;
            SWAG_CHECK(eatTokenError(TokenId::SymComma, toErr(Err0104)));
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightSquare), error(tokenParse, toErr(Err0105)));
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftParen), error(tokenParse, toErr(Err0441)));
            SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0623)));
        }

        FormatAst::inheritFormatAfter(this, attrBlockNode, &tokenParse);
        attrBlockNode->token.endLocation = tokenParse.token.endLocation;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare, "to end the attribute list"));
        if (single)
            break;
    }

    // @AttrUseLastChild
    SWAG_VERIFY(!attrBlockNode->children.empty(), error(attrBlockNode, toErr(Err0065)));
    const auto back = attrBlockNode->lastChild();
    back->allocateExtension(ExtensionKind::Semantic);
    SWAG_ASSERT(!back->extSemantic()->semanticAfterFct);
    back->extSemantic()->semanticAfterFct = Semantic::resolveAttrUse;

    return true;
}

#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser_Push.h"

bool Parser::doEnum(AstNode* parent, AstNode** result)
{
    const auto enumNode   = Ast::newNode<AstEnum>(AstNodeKind::EnumDecl, this, parent);
    enumNode->semanticFct = Semantic::resolveEnum;
    enumNode->allocateExtension(ExtensionKind::Semantic);
    enumNode->extSemantic()->semanticAfterFct = Semantic::sendCompilerMsgTypeDecl;
    *result                                   = enumNode;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymColon), error(tokenParse.token, toErr(Err0553)));
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly), error(tokenParse.token, toErr(Err0552)));
    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0266, tokenParse.token.c_str())));
    enumNode->inheritTokenName(tokenParse.token);
    enumNode->tokenName = tokenParse.token;
    SWAG_CHECK(checkIsValidUserName(enumNode));

    // Add enum type and scope
    Scope* newScope = nullptr;
    {
        ScopedLock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(enumNode, enumNode->token.text, ScopeKind::Enum, currentScope, true);
        if (newScope->isNot(ScopeKind::Enum))
        {
            if (newScope->owner->is(AstNodeKind::Impl))
            {
                const auto implNode = castAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                Diagnostic err{implNode->identifier, formErr(Err0008, Naming::kindName(newScope->kind).c_str(), implNode->token.c_str(), Naming::kindName(ScopeKind::Enum).c_str())};
                err.addNote(Diagnostic::hereIs(enumNode));
                err.addNote(formNte(Nte0043, implNode->token.c_str()));
                return context->report(err);
            }

            const Utf8 asA = form("as %s", Naming::aKindName(newScope->kind).c_str());
            Diagnostic err{enumNode->token.sourceFile, tokenParse.token, formErr(Err0626, "enum", enumNode->token.c_str(), asA.c_str())};
            err.addNote(newScope->owner, newScope->owner->getTokenName(), toNte(Nte0071));
            return context->report(err);
        }

        enumNode->scope = newScope;

        // If an 'impl' came first, then typeinfo has already been defined
        ScopedLock lk1(newScope->owner->mutex);
        auto       typeInfo = castTypeInfo<TypeInfoEnum>(newScope->owner->typeInfo);
        if (!typeInfo)
        {
            typeInfo                  = makeType<TypeInfoEnum>();
            newScope->owner->typeInfo = typeInfo;
        }

        SWAG_ASSERT(typeInfo->isEnum());
        typeInfo->declNode = enumNode;
        typeInfo->name     = enumNode->token.text;
        typeInfo->scope    = newScope;
        enumNode->typeInfo = typeInfo;
        typeInfo->computeName();
        enumNode->setResolvedSymbolName(currentScope->symTable.registerSymbolNameNoLock(context, enumNode, SymbolKind::Enum));
    }

    // Raw type
    SWAG_CHECK(eatToken());
    const auto typeNode   = Ast::newNode<AstNode>(AstNodeKind::EnumType, this, enumNode);
    typeNode->semanticFct = Semantic::resolveEnumType;
    if (tokenParse.is(TokenId::SymColon))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &enumNode->type));
    }

    // Content of enum
    ParserPushScope scoped(this, newScope);
    const auto      startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[enum]] body"));
    while (tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::EndOfFile))
        SWAG_CHECK(doEnumContent(enumNode, &dummyResult));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[enum]] body"));
    return true;
}

bool Parser::doEnumContent(AstNode* parent, AstNode** result)
{
    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        const auto startLoc = tokenParse.token.startLocation;
        const auto stmt     = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
        *result             = stmt;
        SWAG_CHECK(eatToken());
        while (tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::EndOfFile))
            SWAG_CHECK(doEnumContent(stmt, &dummyResult));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
        return true;
    }

    switch (tokenParse.token.id)
    {
        case TokenId::CompilerAst:
            SWAG_CHECK(doCompilerAst(parent, result));
            break;
        case TokenId::CompilerAssert:
            SWAG_CHECK(doCompilerAssert(parent, result));
            break;
        case TokenId::CompilerIf:
            SWAG_CHECK(doCompilerIfFor(parent, result, AstNodeKind::EnumDecl));
            break;

        case TokenId::SymAttrStart:
        {
            AstAttrUse* attrUse;
            SWAG_CHECK(doAttrUse(parent, reinterpret_cast<AstNode**>(&attrUse)));
            SWAG_CHECK(doEnumContent(attrUse, &attrUse->content));
            if (attrUse->content)
                attrUse->content->setOwnerAttrUse(attrUse);
            break;
        }

        case TokenId::SymLeftCurly:
            SWAG_CHECK(doEnumContent(parent, &dummyResult));
            break;

        case TokenId::KwdUsing:
            SWAG_CHECK(doSubEnumValue(parent, result));
            break;

        default:
            SWAG_CHECK(doEnumValue(parent, result));
            break;
    }

    return true;
}

bool Parser::doSubEnumValue(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());

    const auto enumValue = Ast::newNode<AstEnumValue>(AstNodeKind::EnumValue, this, parent);
    enumValue->addSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING);
    enumValue->semanticFct = Semantic::resolveSubEnumValue;
    *result                = enumValue;

    SWAG_CHECK(doIdentifierRef(enumValue, &dummyResult, IDENTIFIER_NO_PARAMS));

    if (tokenParse.is(TokenId::SymComma))
        SWAG_CHECK(eatToken());
    else if (tokenParse.isNot(TokenId::SymRightCurly))
        SWAG_CHECK(eatSemiCol("enum value"));

    return true;
}

bool Parser::doEnumValue(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0265, tokenParse.token.c_str())));

    const auto enumValue = Ast::newNode<AstEnumValue>(AstNodeKind::EnumValue, this, parent);
    *result              = enumValue;
    enumValue->inheritTokenName(tokenParse.token);
    enumValue->semanticFct = Semantic::resolveEnumValue;
    currentScope->symTable.registerSymbolName(context, enumValue, SymbolKind::EnumValue);

    SWAG_CHECK(eatToken());

    AstNode* expression = nullptr;
    if (tokenParse.is(TokenId::SymEqual))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(enumValue, EXPR_FLAG_NONE, &expression));
    }

    if (tokenParse.is(TokenId::SymComma))
    {
        enumValue->inheritFormatFromAfter(this, &tokenParse);
        SWAG_CHECK(eatToken());
    }
    else
    {
        if (tokenParse.isNot(TokenId::SymRightCurly))
        {
            SWAG_CHECK(eatSemiCol("enum value"));
        }
    }

    return true;
}

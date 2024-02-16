#include "pch.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Scoped.h"
#include "Semantic.h"
#include "TypeManager.h"

bool Parser::doEnum(AstNode* parent, AstNode** result)
{
    const auto enumNode   = Ast::newNode<AstEnum>(this, AstNodeKind::EnumDecl, sourceFile, parent);
    enumNode->semanticFct = Semantic::resolveEnum;
    enumNode->allocateExtension(ExtensionKind::Semantic);
    enumNode->extSemantic()->semanticAfterFct = Semantic::sendCompilerMsgTypeDecl;
    *result                                   = enumNode;

    if (!tokenizer.comment.empty())
    {
        enumNode->allocateExtension(ExtensionKind::Misc);
        enumNode->extMisc()->docComment = std::move(tokenizer.comment);
    }

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id != TokenId::SymColon, error(token, Err(Err0553)));
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err0552)));
    SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0266), token.c_str())));
    enumNode->inheritTokenName(token);
    enumNode->tokenName = static_cast<Token>(token);
    SWAG_CHECK(checkIsValidUserName(enumNode));

    // Add enum type and scope
    Scope* newScope = nullptr;
    {
        ScopedLock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(enumNode, enumNode->token.text, ScopeKind::Enum, currentScope, true);
        if (newScope->kind != ScopeKind::Enum)
        {
            if (newScope->owner->kind == AstNodeKind::Impl)
            {
                const auto       implNode = castAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                const Diagnostic diag{implNode->identifier, FMT(Err(Err0008), Naming::kindName(newScope->kind).c_str(), implNode->token.c_str(), Naming::kindName(ScopeKind::Enum).c_str())};
                const auto       note  = Diagnostic::hereIs(enumNode);
                const auto       note1 = Diagnostic::note(FMT(Nte(Nte0043), implNode->token.c_str()));
                return context->report(diag, note, note1);
            }

            const Utf8       asA = FMT("as %s", Naming::aKindName(newScope->kind).c_str());
            const Diagnostic diag{enumNode->sourceFile, token, FMT(Err(Err0626), "enum", enumNode->token.c_str(), asA.c_str())};
            const auto       note = Diagnostic::note(newScope->owner, newScope->owner->getTokenName(), Nte(Nte0071));
            return context->report(diag, note);
        }

        enumNode->scope = newScope;

        // If an 'impl' came first, then typeinfo has already been defined
        ScopedLock lk1(newScope->owner->mutex);
        auto       typeInfo = static_cast<TypeInfoEnum*>(newScope->owner->typeInfo);
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
        enumNode->resolvedSymbolName = currentScope->symTable.registerSymbolNameNoLock(context, enumNode, SymbolKind::Enum);
    }

    // Raw type
    SWAG_CHECK(eatToken());
    const auto typeNode   = Ast::newNode<AstNode>(this, AstNodeKind::EnumType, sourceFile, enumNode);
    typeNode->semanticFct = Semantic::resolveEnumType;
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &enumNode->type));
    }

    // Content of enum
    Scoped     scoped(this, newScope);
    const auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[enum]] body"));
    while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
        SWAG_CHECK(doEnumContent(enumNode, &dummyResult));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[enum]] body"));
    return true;
}

bool Parser::doEnumContent(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
    {
        const auto startLoc = token.startLocation;
        const auto stmt     = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        *result             = stmt;
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
            SWAG_CHECK(doEnumContent(stmt, &dummyResult));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
        return true;
    }

    switch (token.id)
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

    const auto enumValue = Ast::newNode<AstEnumValue>(this, AstNodeKind::EnumValue, sourceFile, parent);
    enumValue->addSpecFlag(AstEnumValue::SPECFLAG_HAS_USING);
    enumValue->semanticFct = Semantic::resolveSubEnumValue;
    *result                = enumValue;

    SWAG_CHECK(doIdentifierRef(enumValue, &dummyResult, IDENTIFIER_NO_PARAMS));

    if (tokenizer.comment.length())
    {
        enumValue->allocateExtension(ExtensionKind::Misc);
        enumValue->extMisc()->docComment = std::move(tokenizer.comment);
    }

    if (token.id == TokenId::SymComma)
        SWAG_CHECK(eatToken());
    else if (token.id != TokenId::SymRightCurly)
        SWAG_CHECK(eatSemiCol("enum value"));

    return true;
}

bool Parser::doEnumValue(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0265), token.c_str())));

    const auto enumValue = Ast::newNode<AstEnumValue>(this, AstNodeKind::EnumValue, sourceFile, parent);
    *result              = enumValue;
    enumValue->inheritTokenName(token);
    enumValue->semanticFct = Semantic::resolveEnumValue;
    currentScope->symTable.registerSymbolName(context, enumValue, SymbolKind::EnumValue);

    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(enumValue, EXPR_FLAG_NONE, &dummyResult));
    }

    if (tokenizer.comment.length())
    {
        enumValue->allocateExtension(ExtensionKind::Misc);
        enumValue->extMisc()->docComment = std::move(tokenizer.comment);
    }

    if (token.id == TokenId::SymComma)
        SWAG_CHECK(eatToken());
    else if (token.id != TokenId::SymRightCurly)
        SWAG_CHECK(eatSemiCol("enum value"));

    return true;
}

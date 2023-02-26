#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "TypeManager.h"

bool Parser::doEnum(AstNode* parent, AstNode** result)
{
    auto enumNode         = Ast::newNode<AstEnum>(this, AstNodeKind::EnumDecl, sourceFile, parent);
    enumNode->semanticFct = SemanticJob::resolveEnum;
    enumNode->allocateExtension(ExtensionKind::Semantic);
    enumNode->extSemantic()->semanticAfterFct = SemanticJob::sendCompilerMsgTypeDecl;
    if (result)
        *result = enumNode;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0074), token.ctext())));
    enumNode->inheritTokenName(token);
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
                auto       implNode = CastAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                Diagnostic diag{implNode->identifier, Fmt(Err(Syn0123), Naming::kindName(newScope->kind).c_str(), implNode->token.ctext(), Naming::kindName(ScopeKind::Enum).c_str())};
                diag.hint = Fmt(Hnt(Hnt0019), implNode->token.ctext());
                auto note = Diagnostic::note(enumNode, enumNode->token, Fmt(Nte(Nte0027), implNode->token.ctext()));
                return context->report(diag, note);
            }
            else
            {
                Diagnostic diag{enumNode->sourceFile, token, Fmt(Err(Err0394), enumNode->token.ctext(), Naming::aKindName(newScope->kind).c_str())};
                auto       note = Diagnostic::note(newScope->owner, newScope->owner->token, Nte(Nte0036));
                return context->report(diag, note);
            }
        }

        enumNode->scope = newScope;

        // If an 'impl' came first, then typeinfo has already been defined
        ScopedLock    lk1(newScope->owner->mutex);
        TypeInfoEnum* typeInfo = (TypeInfoEnum*) newScope->owner->typeInfo;
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
    auto typeNode         = Ast::newNode<AstNode>(this, AstNodeKind::EnumType, sourceFile, enumNode);
    typeNode->semanticFct = SemanticJob::resolveEnumType;
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(typeNode));
    }

    SWAG_VERIFY(token.id == TokenId::SymLeftCurly, error(token, Fmt(Err(Syn0031), token.ctext())));

    // Content of enum
    Scoped scoped(this, newScope);
    auto   startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the enum body"));
    while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
        SWAG_CHECK(doEnumContent(enumNode));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the enum body"));
    return true;
}

bool Parser::doEnumContent(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
    {
        auto startLoc = token.startLocation;
        auto stmt     = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        if (result)
            *result = stmt;
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
            SWAG_CHECK(doEnumContent(stmt));
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
        SWAG_CHECK(doAttrUse(parent, (AstNode**) &attrUse));
        SWAG_CHECK(doEnumContent(attrUse, &attrUse->content));
        if (attrUse->content)
            attrUse->content->setOwnerAttrUse(attrUse);
        break;
    }

    case TokenId::SymLeftCurly:
        SWAG_CHECK(doEnumContent(parent));
        break;

    default:
        // If this a function call ?
        // (a mixin)
        bool isFunc = false;
        if (token.id == TokenId::Identifier)
        {
            tokenizer.saveState(token);
            eatToken();
            if (token.id == TokenId::SymDot || token.id == TokenId::SymLeftParen)
                isFunc = true;
            tokenizer.restoreState(token);
        }

        if (isFunc)
        {
            SWAG_CHECK(doIdentifierRef(parent, result, IDENTIFIER_GLOBAL));
        }
        else
        {
            SWAG_CHECK(doEnumValue(parent, result));
        }

        break;
    }

    return true;
}

bool Parser::doEnumValue(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0075), token.ctext())));
    auto enumValue = Ast::newNode<AstEnumValue>(this, AstNodeKind::EnumValue, sourceFile, parent);
    if (result)
        *result = enumValue;
    enumValue->inheritTokenName(token);
    enumValue->semanticFct = SemanticJob::resolveEnumValue;
    currentScope->symTable.registerSymbolName(context, enumValue, SymbolKind::EnumValue);

    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doExpression(enumValue, EXPR_FLAG_NONE));
    }

    SWAG_CHECK(eatSemiCol("enum value"));
    return true;
}
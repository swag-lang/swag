#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "ScopedLock.h"

bool SyntaxJob::doEnum(AstNode* parent, AstNode** result)
{
    auto enumNode = Ast::newNode<AstEnum>(this, AstNodeKind::EnumDecl, sourceFile, parent);
    if (result)
        *result = enumNode;
    enumNode->semanticFct = SemanticJob::resolveEnum;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Utf8::format(Msg0396, token.text.c_str())));
    enumNode->inheritTokenName(token);
    SWAG_CHECK(checkIsValidUserName(enumNode));

    // Add enum type and scope
    Scope* newScope = nullptr;
    {
        ScopedLock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(enumNode, enumNode->token.text, ScopeKind::Enum, currentScope, true);
        if (newScope->kind != ScopeKind::Enum)
        {
            auto        implNode = CastAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
            PushErrHint errh(Utf8::format(Hnt0019, implNode->token.text.c_str()));
            Diagnostic  diag{implNode->identifier, implNode->identifier->token, Utf8::format(Msg0441, Scope::getNakedKindName(newScope->kind), implNode->token.text.c_str(), Scope::getNakedKindName(ScopeKind::Enum))};
            Diagnostic  note{enumNode, enumNode->token, Utf8::format(Msg0398, implNode->token.text.c_str()), DiagnosticLevel::Note};
            return sourceFile->report(diag, &note);
        }

        enumNode->scope = newScope;

        // If an 'impl' came first, then typeinfo has already been defined
        ScopedLock   lk1(newScope->owner->mutex);
        TypeInfoEnum* typeInfo = (TypeInfoEnum*) newScope->owner->typeInfo;
        if (!typeInfo)
        {
            typeInfo                  = allocType<TypeInfoEnum>();
            newScope->owner->typeInfo = typeInfo;
        }

        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Enum);
        typeInfo->declNode = enumNode;
        typeInfo->name     = enumNode->token.text;
        typeInfo->scope    = newScope;
        enumNode->typeInfo = typeInfo;
        typeInfo->computeName();
        enumNode->resolvedSymbolName = currentScope->symTable.registerSymbolNameNoLock(&context, enumNode, SymbolKind::Enum);
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

    SWAG_VERIFY(token.id == TokenId::SymLeftCurly, error(token, Utf8::format(Msg0399, token.text.c_str())));

    // Content of enum
    Scoped scoped(this, newScope);
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
        SWAG_CHECK(doEnumContent(enumNode));
    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doEnumContent(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
    {
        auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        if (result)
            *result = stmt;
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
            SWAG_CHECK(doEnumContent(stmt));
        SWAG_CHECK(eatToken(TokenId::SymRightCurly));
        return true;
    }

    switch (token.id)
    {
    case TokenId::CompilerAst:
        SWAG_CHECK(doCompilerAst(parent, result, CompilerAstKind::EnumValue));
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
        SWAG_CHECK(doEnumValue(parent, result));
        break;
    }

    return true;
}

bool SyntaxJob::doEnumValue(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Msg0400));
    auto enumValue = Ast::newNode<AstEnumValue>(this, AstNodeKind::EnumValue, sourceFile, parent);
    if (result)
        *result = enumValue;
    enumValue->inheritTokenName(token);
    enumValue->semanticFct = SemanticJob::resolveEnumValue;
    currentScope->symTable.registerSymbolName(&context, enumValue, SymbolKind::EnumValue);

    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doExpression(enumValue, EXPR_FLAG_NONE));
    }

    SWAG_CHECK(eatSemiCol("enum value"));
    return true;
}
#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Ast.h"
#include "Scope.h"
#include "Scoped.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"

bool SyntaxJob::doEnum(AstNode* parent, AstNode** result)
{
    auto enumNode = Ast::newNode<AstEnum>(this, AstNodeKind::EnumDecl, sourceFile, parent);
    if (result)
        *result = enumNode;
    enumNode->semanticFct = SemanticJob::resolveEnum;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid enum name '%s'", token.text.c_str())));
    enumNode->inheritTokenName(token);
    SWAG_CHECK(checkIsValidUserName(enumNode));

    // Add enum type and scope
    Scope* newScope = nullptr;
    {
        scoped_lock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(enumNode, enumNode->token.text, ScopeKind::Enum, currentScope, true);
        if (newScope->kind != ScopeKind::Enum)
        {
            auto       implNode = CastAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
            Diagnostic diag{implNode->identifier, implNode->identifier->token, format("the implementation block kind (%s) does not match the type of '%s' (%s)", Scope::getNakedKindName(newScope->kind), implNode->token.text.c_str(), Scope::getNakedKindName(ScopeKind::Enum))};
            Diagnostic note{enumNode, enumNode->token, format("this is the declaration of '%s'", implNode->token.text.c_str()), DiagnosticLevel::Note};
            return sourceFile->report(diag, &note);
        }

        enumNode->scope = newScope;

        // If an 'impl' came first, then typeinfo has already been defined
        scoped_lock   lk1(newScope->owner->mutex);
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
    SWAG_CHECK(tokenizer.getToken(token));
    auto typeNode         = Ast::newNode<AstNode>(this, AstNodeKind::EnumType, sourceFile, enumNode);
    typeNode->semanticFct = SemanticJob::resolveEnumType;
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(typeNode));
    }

    SWAG_VERIFY(token.id == TokenId::SymLeftCurly, syntaxError(token, format("'{' is expected instead of '%s'", token.text.c_str())));

    // Content of enum
    Scoped         scoped(this, newScope);
    ScopedMainNode scopedMainNode(this, enumNode);
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
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "enum value identifier expected"));
    auto enumValue = Ast::newNode<AstEnumValue>(this, AstNodeKind::EnumValue, sourceFile, parent);
    if (result)
        *result = enumValue;
    enumValue->inheritTokenName(token);
    enumValue->semanticFct = SemanticJob::resolveEnumValue;
    currentScope->symTable.registerSymbolName(&context, enumValue, SymbolKind::EnumValue);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doExpression(enumValue));
    }

    SWAG_CHECK(eatSemiCol("after enum value"));
    return true;
}
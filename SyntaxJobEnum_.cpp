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
    SWAG_CHECK(isValidUserName(enumNode));

    // Add enum type and scope
    Scope* newScope = nullptr;
    {
        scoped_lock lk(currentScope->symTable.mutex);
        auto        symbol = currentScope->symTable.findNoLock(enumNode->name);
        if (!symbol)
        {
            newScope = Ast::newScope(enumNode, enumNode->name, ScopeKind::Enum, currentScope, true);
            if (newScope->kind != ScopeKind::Enum)
            {
                auto       implNode = CastAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                Diagnostic diag{implNode->identifier, implNode->identifier->token, format("the implementation block kind (%s) does not match the type of '%s' (%s)", Scope::getNakedKindName(newScope->kind), implNode->name.c_str(), Scope::getNakedKindName(ScopeKind::Enum))};
                Diagnostic note{enumNode, enumNode->token, format("this is the declaration of '%s'", implNode->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }

            enumNode->scope = newScope;

            // If an 'impl' came first, then typeinfo has already been defined
            scoped_lock   lk1(newScope->owner->mutex);
            TypeInfoEnum* typeInfo = (TypeInfoEnum*) newScope->owner->typeInfo;
            if (!typeInfo)
            {
                typeInfo                  = g_Allocator.alloc<TypeInfoEnum>();
                newScope->owner->typeInfo = typeInfo;
            }

            SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Enum);
            typeInfo->declNode  = enumNode;
            typeInfo->name      = enumNode->name;
            typeInfo->nakedName = enumNode->name;
            typeInfo->scope     = newScope;
            enumNode->typeInfo  = typeInfo;
            typeInfo->computeName();
            currentScope->symTable.registerSymbolNameNoLock(&context, enumNode, SymbolKind::Enum);
        }
        else
        {
            auto       firstOverload = &symbol->defaultOverload;
            Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
            return sourceFile->report(diag, &diagNote);
        }
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
    SWAG_CHECK(doEnumContent(enumNode));
    return true;
}

bool SyntaxJob::doEnumContent(AstNode* parent)
{
    bool waitCurly = false;
    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
        waitCurly = true;
    }

    while (true)
    {
        if (token.id == TokenId::EndOfFile)
        {
            SWAG_CHECK(syntaxError(token, "no matching '}' found in enum declaration"));
            break;
        }

        if (token.id == TokenId::SymRightCurly)
        {
            SWAG_CHECK(eatToken());
            break;
        }

        switch (token.id)
        {
        case TokenId::CompilerAst:
            SWAG_CHECK(doCompilerAst(parent, nullptr, CompilerAstKind::EnumValue));
            break;
        case TokenId::CompilerAssert:
            SWAG_CHECK(doCompilerAssert(parent, nullptr));
            break;
        case TokenId::CompilerIf:
            SWAG_CHECK(doCompilerIfFor(parent, nullptr, AstNodeKind::EnumDecl));
            break;

        case TokenId::SymAttrStart:
            SWAG_CHECK(doAttrUse(parent));
            break;
        case TokenId::SymLeftCurly:
        {
            auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            SWAG_CHECK(doEnumContent(stmt));
            break;
        }

        default:
            SWAG_CHECK(doEnumValue(parent));
            break;
        }

        if (!waitCurly)
            break;
    }

    return true;
}

bool SyntaxJob::doEnumValue(AstNode* parent)
{
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "enum value identifier expected"));
    auto enumValue = Ast::newNode<AstNode>(this, AstNodeKind::EnumValue, sourceFile, parent);
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
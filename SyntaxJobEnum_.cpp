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
    auto enumNode = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::EnumDecl, sourceFile, parent);
    if (result)
        *result = enumNode;
    enumNode->semanticFct = &SemanticJob::resolveEnum;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid enum name '%s'", token.text.c_str())));
    enumNode->inheritTokenName(token);

    // Add enum type and scope
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(enumNode->name);
        if (!symbol)
        {
            auto typeInfo = g_Pool_typeInfoEnum.alloc();
            newScope      = Ast::newScope(enumNode, enumNode->name, ScopeKind::Enum, currentScope);
            newScope->allocateSymTable();
            typeInfo->name     = enumNode->name;
            typeInfo->scope    = newScope;
            enumNode->typeInfo = typeInfo;
            currentScope->symTable->registerSymbolNameNoLock(sourceFile, enumNode, SymbolKind::Enum);
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
    auto typeNode         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::EnumType, sourceFile, enumNode);
    typeNode->semanticFct = &SemanticJob::resolveEnumType;
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

        if (token.id == TokenId::SymAttrStart)
        {
            SWAG_CHECK(doAttrUse(parent));
        }
        else if (token.id == TokenId::SymLeftCurly)
        {
            auto stmt = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Statement, sourceFile, parent);
            SWAG_CHECK(doEnumContent(stmt));
        }
        else if (token.id == TokenId::CompilerIf)
        {
            SWAG_CHECK(doCompilerIfFor(parent, nullptr, AstNodeKind::EnumDecl));
        }
        else
        {
            SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "enum value identifier expected"));
            auto enumValue = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::EnumValue, sourceFile, parent);
            enumValue->inheritTokenName(token);
            enumValue->semanticFct = &SemanticJob::resolveEnumValue;
            currentScope->symTable->registerSymbolNameNoLock(sourceFile, enumValue, SymbolKind::EnumValue);

            SWAG_CHECK(tokenizer.getToken(token));
            if (token.id == TokenId::SymEqual)
            {
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_CHECK(doExpression(enumValue));
            }

            SWAG_CHECK(eatSemiCol("after enum value"));
        }

        if (!waitCurly)
            break;
    }

    return true;
}
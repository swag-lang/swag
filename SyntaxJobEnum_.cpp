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

    // Content of enum
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    Scoped scoped(this, newScope);
    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        if (token.id == TokenId::SymAttrStart)
        {
            SWAG_CHECK(doAttrUse(enumNode));
        }
        else
        {
            SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "enum value identifier expected"));
            auto enumValue = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::EnumValue, sourceFile, enumNode);
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
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

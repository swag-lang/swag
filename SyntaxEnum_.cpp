#include "pch.h"
#include "Global.h"
#include "Module.h"
#include "Diagnostic.h"

bool SyntaxJob::doEnum(AstNode* parent, AstNode** result)
{
    auto enumNode = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::EnumDecl, currentScope, parent, false);
    if (result)
        *result = enumNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid enum name '%s'", token.text.c_str())));
    Ast::assignToken(enumNode, token);

    // Add enum type and scope
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(enumNode->name);
        if (!symbol)
        {
            auto typeInfo = sourceFile->poolFactory->typeInfoEnum.alloc();
            newScope      = Ast::newScope(sourceFile, enumNode->name, ScopeKind::Enum, currentScope);
            newScope->allocateSymTable();
            typeInfo->name     = "enum " + enumNode->name;
            typeInfo->scope    = newScope;
            enumNode->typeInfo = typeInfo;
            currentScope->symTable->registerSymbolNameNoLock(sourceFile, enumNode->token, newScope->name, SymbolKind::Enum);
        }
        else
        {
            auto       firstOverload = &symbol->defaultOverload;
            Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
            return sourceFile->report(diag, &diagNote);
        }
    }

    SWAG_CHECK(tokenizer.getToken(token));

    // Raw type
    auto typeNode         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::EnumType, nullptr, enumNode, false);
    typeNode->semanticFct = &SemanticJob::resolveEnumType;
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(typeNode));
    }

    // Content of enum
    auto curly = move(token);
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    auto savedScope = currentScope;
    currentScope    = newScope;
    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "enum value identifier expected"));
        auto enumValue         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::EnumDecl, currentScope, enumNode, false);
        enumValue->semanticFct = &SemanticJob::resolveEnumValue;
        Ast::assignToken(enumValue, token);
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, enumValue->token, enumValue->name, SymbolKind::EnumValue);

        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id == TokenId::SymEqual)
        {
			SWAG_CHECK(eatToken(TokenId::SymEqual));
			SWAG_CHECK(doExpression(enumValue));
        }

        SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    }

    currentScope = savedScope;

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Ast.h"
#include "Scope.h"
#include "Scoped.h"
#include "SymTable.h"
#include "SemanticJob.h"

bool SyntaxJob::doStruct(AstNode* parent, AstNode** result)
{
    auto structNode = Ast::newNode(&g_Pool_astStruct, AstNodeKind::StructDecl, sourceFile->indexInModule, parent);
    structNode->inheritOwnersAndFlags(this);
    structNode->semanticFct = &SemanticJob::resolveStruct;
    if (result)
        *result = structNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid struct name '%s'", token.text.c_str())));
    Ast::assignToken(structNode, token);

    // Add struct type and scope
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(structNode->name);
        if (!symbol)
        {
            auto typeInfo = g_Pool_typeInfoStruct.alloc();
            newScope      = Ast::newScope(structNode->name, ScopeKind::Struct, currentScope);
            newScope->allocateSymTable();
            typeInfo->name       = structNode->name;
            typeInfo->scope      = newScope;
            structNode->typeInfo = typeInfo;
            if (!isContextDisabled())
                currentScope->symTable->registerSymbolNameNoLock(sourceFile, structNode, SymbolKind::Struct);
        }
        else
        {
            auto       firstOverload = &symbol->defaultOverload;
            Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
            return sourceFile->report(diag, &diagNote);
        }
    }

    // Raw type
    SWAG_CHECK(tokenizer.getToken(token));

    // Content of struct
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    {
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doVarDecl(structNode));
        }
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

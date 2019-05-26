#include "pch.h"
#include "Global.h"
#include "Module.h"
#include "Diagnostic.h"

bool SyntaxJob::doNamespace(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentScope->isGlobal(), error(token, "a namespace definition must appear either at file scope or immediately within another namespace definition"));

    auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Namespace, currentScope, parent, false);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid namespace name '%s'", token.text.c_str())));
    node->token = move(token);

    Utf8Crc name = node->token.text;
    name.computeCrc();
    node->name = name;

    // Add/Get namespace
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(name);
        if (!symbol)
        {
            auto typeInfo         = sourceFile->poolFactory->typeInfoNamespace.alloc();
            auto fullname         = currentScope->fullname + "." + name;
            newScope              = sourceFile->poolFactory->scope.alloc();
            newScope->kind        = ScopeKind::Namespace;
            newScope->parentScope = currentScope;
            newScope->name        = move(name);
            newScope->fullname    = move(fullname);
            typeInfo->scope       = newScope;
            node->typeInfo        = typeInfo;
            currentScope->symTable->addSymbolTypeInfoNoLock(sourceFile, node->token, newScope->name, typeInfo, SymbolKind::Namespace);
        }
        else if (symbol->kind != SymbolKind::Namespace)
        {
            auto       firstOverload = &symbol->defaultOverload;
            Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
            return sourceFile->report(diag, &diagNote);
        }
        else
            newScope = static_cast<TypeInfoNamespace*>(symbol->overloads[0]->typeInfo)->scope;
    }

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    auto curly = move(token);

    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        auto savedScope = currentScope;
        currentScope    = newScope;
        auto ok         = doTopLevel(node);
        currentScope    = savedScope;
        SWAG_CHECK(ok);
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doEnum(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Enum, currentScope, parent, false);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid enum name '%s'", token.text.c_str())));
    node->token = move(token);

    Utf8Crc name = node->token.text;
    name.computeCrc();
    node->name = name;

    // Add/Get namespace
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(name);
        if (!symbol)
        {
            auto typeInfo         = sourceFile->poolFactory->typeInfoNamespace.alloc();
            auto fullname         = currentScope->fullname + "." + name;
            newScope              = sourceFile->poolFactory->scope.alloc();
            newScope->kind        = ScopeKind::Enum;
            newScope->parentScope = currentScope;
            newScope->name        = move(name);
            newScope->fullname    = move(fullname);
            typeInfo->scope       = newScope;
            node->typeInfo        = typeInfo;
            currentScope->symTable->addSymbolTypeInfoNoLock(sourceFile, node->token, newScope->name, typeInfo, SymbolKind::Enum);
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
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    auto curly = move(token);

    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        auto savedScope = currentScope;
        currentScope    = newScope;
        auto ok         = doTopLevel(node);
        currentScope    = savedScope;
        SWAG_CHECK(ok);
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doVarDecl(AstNode* parent, AstVarDecl** result)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeType::VarDecl, currentScope, parent, false);
    node->semanticFct = &SemanticJob::resolveVarDecl;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    node->name = token.text;
    node->name.computeCrc();
    node->token = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(node, &node->astType));
        SWAG_CHECK(node->astType);
    }

    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doAssignmentExpression(node, &node->astAssignment));
    }

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    currentScope->allocateSymTable();
    currentScope->symTable->registerSymbolNameNoLock(sourceFile, node->token, node->name, SymbolKind::Variable);

    return true;
}

bool SyntaxJob::doTopLevel(AstNode* parent)
{
    switch (token.id)
    {
    case TokenId::SymSemiColon:
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdVar:
        SWAG_CHECK(doVarDecl(parent));
        break;
    case TokenId::KwdType:
        SWAG_CHECK(doTypeDecl(parent));
        break;
    case TokenId::KwdNamespace:
        SWAG_CHECK(doNamespace(parent));
        break;
    case TokenId::KwdEnum:
        SWAG_CHECK(doEnum(parent));
        break;
    case TokenId::CompilerUnitTest:
        SWAG_CHECK(doCompilerUnitTest());
        break;
    case TokenId::CompilerAssert:
        SWAG_CHECK(doCompilerAssert(parent));
        break;
    case TokenId::CompilerPrint:
        SWAG_CHECK(doCompilerPrint(parent));
        break;
    case TokenId::CompilerRun:
        SWAG_CHECK(doCompilerRun(parent));
        break;
    default:
        syntaxError(token, format("invalid token '%s'", token.text.c_str()));
        return false;
    }

    return true;
}

#include "pch.h"
#include "Global.h"
#include "Module.h"
#include "Diagnostic.h"

bool SyntaxJob::doNamespace(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentScope->isGlobal(), error(token, "a namespace definition must appear either at file scope or immediately within another namespace definition"));

    auto namespaceNode = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::Namespace, currentScope, parent, false);
    if (result)
        *result = namespaceNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid namespace name '%s'", token.text.c_str())));
    Ast::assignToken(namespaceNode, token);

    // Add/Get namespace
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(namespaceNode->name);
        if (!symbol)
        {
            auto typeInfo           = sourceFile->poolFactory->typeInfoNamespace.alloc();
            typeInfo->name          = "namespace " + namespaceNode->name;
            newScope                = Ast::newScope(sourceFile, namespaceNode->name, ScopeKind::Namespace, currentScope);
            typeInfo->scope         = newScope;
            namespaceNode->typeInfo = typeInfo;
            currentScope->symTable->addSymbolTypeInfoNoLock(sourceFile, namespaceNode->token, newScope->name, typeInfo, SymbolKind::Namespace);
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
    auto curly = move(token);
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    // Content of namespace is toplevel
    auto savedScope = currentScope;
    currentScope    = newScope;
    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        SWAG_CHECK(doTopLevel(namespaceNode));
    }

    currentScope = savedScope;

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doEnum(AstNode* parent, AstNode** result)
{
    auto enumNode = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::EnumDecl, currentScope, parent, false);
    if (result)
        *result = enumNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid enum name '%s'", token.text.c_str())));
    Ast::assignToken(enumNode, token);

    // Add/Get namespace
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
        SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    }

    currentScope = savedScope;

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

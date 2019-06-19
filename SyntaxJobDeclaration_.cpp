#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "Scoped.h"
#include "SourceFile.h"
#include "Scope.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "Ast.h"
#include "SemanticJob.h"

bool SyntaxJob::doUsing(AstNode* parent)
{
    auto node         = Ast::newNode(&g_Pool_astNode, AstNodeKind::Namespace, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveUsing;
    node->inheritOwnersAndFlags(this);
    node->inheritToken(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doIdentifierRef(node));

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    return true;
}

bool SyntaxJob::doNamespace(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isGlobal(), error(token, "a namespace definition must appear either at file scope or immediately within another namespace definition"));
    SWAG_CHECK(tokenizer.getToken(token));

    AstNode* namespaceNode;
    Scope*   oldScope = currentScope;
    Scope*   newScope = nullptr;
    while (true)
    {
        namespaceNode              = Ast::newNode(&g_Pool_astNode, AstNodeKind::Namespace, sourceFile->indexInModule, parent);
        namespaceNode->semanticFct = &SemanticJob::resolveNamespace;
        namespaceNode->inheritOwnersAndFlags(this);

        switch (token.id)
        {
        case TokenId::Identifier:
            break;
        case TokenId::SymLeftCurly:
            return syntaxError(token, "missing namespace name before '{'");
        default:
            return syntaxError(token, format("invalid namespace name '%s'", token.text.c_str()));
        }

        // Be sure this is not the swag namespace, except for a runtime file
        if (!sourceFile->externalBuffer)
            SWAG_VERIFY(token.text != "swag", syntaxError(token, "the 'swag' namespace is reserved by the compiler"));
        Ast::assignToken(namespaceNode, token);

        // Add/Get namespace
        currentScope->allocateSymTable();
        {
            scoped_lock lk(currentScope->symTable->mutex);
            auto        symbol = currentScope->symTable->findNoLock(namespaceNode->name);
            if (!symbol)
            {
                auto typeInfo           = g_Pool_typeInfoNamespace.alloc();
                typeInfo->name          = namespaceNode->name;
                newScope                = Ast::newScope(sourceFile, namespaceNode->name, ScopeKind::Namespace, currentScope);
                typeInfo->scope         = newScope;
                namespaceNode->typeInfo = typeInfo;
                currentScope->symTable->addSymbolTypeInfoNoLock(sourceFile, namespaceNode, typeInfo, SymbolKind::Namespace);
            }
            else if (symbol->kind != SymbolKind::Namespace)
            {
                auto       firstOverload = &symbol->defaultOverload;
                Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
                Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
                Utf8       note = "this is the other definition";
                Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
                return sourceFile->report(diag, &diagNote);
            }
            else
                newScope = static_cast<TypeInfoNamespace*>(symbol->overloads[0]->typeInfo)->scope;
        }

        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id != TokenId::SymDot)
            break;
        SWAG_CHECK(eatToken(TokenId::SymDot));
        parent       = namespaceNode;
        currentScope = newScope;
    }

    currentScope = oldScope;
    auto curly   = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    // Content of namespace is toplevel
    {
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTopLevelInstruction(namespaceNode));
        }
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::Statement, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    if (result)
        *result = node;

    bool isGlobal = currentScope->isGlobal();
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        if (isGlobal)
        {
            SWAG_CHECK(doTopLevelInstruction(node));
        }
        else
        {
            SWAG_CHECK(doEmbeddedInstruction(node));
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doScopedCurlyStatement(AstNode* parent, AstNode** result)
{
    auto     newScope = Ast::newScope(sourceFile, "", ScopeKind::Statement, currentScope);
    AstNode* statement;

    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(doCurlyStatement(parent, &statement));
        statement->semanticBeforeFct = &SemanticJob::resolveScopedStmtBefore;
    }

    if (result)
        *result = statement;
    return true;
}

bool SyntaxJob::doEmbeddedStatement(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
        return doScopedCurlyStatement(parent, result);
    return doEmbeddedInstruction(parent, result);
}

bool SyntaxJob::doStatement(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
        return doCurlyStatement(parent, result);

    bool isGlobal = currentScope->isGlobal();
    if (isGlobal)
    {
        auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::Statement, sourceFile->indexInModule, parent);
        node->inheritOwnersAndFlags(this);
        if (result)
            *result = node;
        return doTopLevelInstruction(node);
    }

    return doEmbeddedInstruction(parent, result);
}

bool SyntaxJob::doEmbeddedInstruction(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::SymLeftCurly:
        SWAG_CHECK(doScopedCurlyStatement(parent, result));
        break;
    case TokenId::SymSemiColon:
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdReturn:
        SWAG_CHECK(doReturn(parent, result));
        SWAG_CHECK(eatToken(TokenId::SymSemiColon));
        break;
    case TokenId::KwdIf:
        SWAG_CHECK(doIf(parent, result));
        break;
    case TokenId::KwdWhile:
        SWAG_CHECK(doWhile(parent, result));
        break;
    case TokenId::KwdVar:
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doVarDecl(parent, result));
        break;
    case TokenId::Identifier:
    case TokenId::Intrisic:
        SWAG_CHECK(doAffectExpression(parent, result));
        break;
    case TokenId::KwdBreak:
        SWAG_CHECK(doBreak(parent, result));
        break;
    case TokenId::KwdContinue:
        SWAG_CHECK(doContinue(parent, result));
        break;
    default:
        return syntaxError(token, format("invalid token '%s'", token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::doTopLevelInstruction(AstNode* parent)
{
    switch (token.id)
    {
    case TokenId::SymLeftCurly:
        SWAG_CHECK(doCurlyStatement(parent));
        break;
    case TokenId::SymSemiColon:
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdVar:
        SWAG_CHECK(tokenizer.getToken(token));
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
    case TokenId::KwdAttr:
        SWAG_CHECK(doAttrDecl(parent));
        break;
    case TokenId::KwdUsing:
        SWAG_CHECK(doUsing(parent));
        break;
    case TokenId::SymAttrStart:
        SWAG_CHECK(doAttrUse(parent));
        break;
    case TokenId::KwdFunc:
        SWAG_CHECK(doFuncDecl(parent));
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
    case TokenId::CompilerVersion:
        SWAG_CHECK(doCompilerVersion(parent));
        break;
    case TokenId::CompilerRun:
        SWAG_CHECK(doCompilerRunDecl(parent));
        break;
    case TokenId::CompilerImport:
        SWAG_CHECK(doCompilerImport(parent));
        break;
    default:
        syntaxError(token, format("invalid token '%s'", token.text.c_str()));
        return false;
    }

    return true;
}

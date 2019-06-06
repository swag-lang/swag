#include "pch.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Workspace.h"
#include "Module.h"
#include "SourceFile.h"
#include "Scope.h"
#include "SemanticJob.h"
#include "Ast.h"

bool SyntaxJob::doCompilerAssert(AstNode* parent)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::CompilerAssert, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerAssert;
    node->token       = move(token);

    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#assert can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    return true;
}

bool SyntaxJob::doCompilerPrint(AstNode* parent)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::CompilerPrint, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerPrint;
    node->token       = move(token);

    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#print can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    return true;
}

bool SyntaxJob::doCompilerRun(AstNode* parent)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::CompilerRun, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerRun;
    node->token       = move(token);

    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#run can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    return true;
}

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#unittest can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.text == "error")
    {
        if (g_CommandLine.test)
            sourceFile->unittestError++;
    }
    else if (token.text == "pass")
    {
        SWAG_CHECK(tokenizer.getToken(token));
        if (token.text == "lexer")
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Lexer;
        }
        else if (token.text == "syntax")
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Syntax;
        }
        else if (token.text == "semantic")
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Semantic;
        }
        else
        {
            sourceFile->report({sourceFile, token, format("invalid pass name '%s'", token.text.c_str())});
            return false;
        }
    }
    else if (token.text == "module")
    {
        SWAG_VERIFY(!moduleSpecified, sourceFile->report({sourceFile, token, "#unittest module can only be specified once"}));
        SWAG_VERIFY(canChangeModule, sourceFile->report({sourceFile, token, "#unittest module instruction must be done before any code"}));
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_VERIFY(token.id == TokenId::Identifier, sourceFile->report({sourceFile, token, format("invalid module name '%s'", token.text.c_str())}));
        moduleSpecified = true;
        if (g_CommandLine.test)
        {
            auto newModule = g_Workspace.createOrUseModule(token.text);
            sourceFile->module->removeFile(sourceFile);
            newModule->addFile(sourceFile);
            currentScope = newModule->scopeRoot;
        }
    }
    else
    {
        sourceFile->report({sourceFile, token, format("unknown #unittest parameter '%s'", token.text.c_str())});
        return false;
    }

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    return true;
}

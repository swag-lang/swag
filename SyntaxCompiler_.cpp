#include "pch.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Workspace.h"
#include "Module.h"
#include "PoolFactory.h"
#include "SourceFile.h"

bool SyntaxJob::doCompilerAssert(AstNode* parent)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::CompilerAssert, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerAssert;
    node->token       = move(token);

    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#assert can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getTokenOrEOL(token));
    SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing #assert expression"}));
    SWAG_CHECK(doExpression(node));

    return true;
}

bool SyntaxJob::doCompilerPrint(AstNode* parent)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::CompilerPrint, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerPrint;
    node->token       = move(token);

    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#print can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getTokenOrEOL(token));
    SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing #print expression"}));
    SWAG_CHECK(doExpression(node));

    return true;
}

bool SyntaxJob::doCompilerRun(AstNode* parent)
{
    auto node         = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::CompilerRun, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerRun;
    node->token       = move(token);

    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#run can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getTokenOrEOL(token));
    SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing #run expression"}));
    SWAG_CHECK(doExpression(node));

    return true;
}

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#unittest can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getTokenOrEOL(token));
    SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing #unittest parameter"}));

    if (token.text == "error")
    {
        if (g_CommandLine.test)
            sourceFile->unittestError++;
    }
    else if (token.text == "pass")
    {
        SWAG_CHECK(tokenizer.getTokenOrEOL(token));
        SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing pass name"}));
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
        SWAG_CHECK(tokenizer.getTokenOrEOL(token));
        SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing module name"}));
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
    return true;
}

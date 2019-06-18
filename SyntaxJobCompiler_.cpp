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
#include "Scoped.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "Attribute.h"
#include "TypeManager.h"

bool SyntaxJob::doCompilerAssert(AstNode* parent)
{
    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#assert can only be declared in the top level scope"}));

    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::CompilerAssert, sourceFile->indexInModule, parent);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerAssert;
    node->token       = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    return true;
}

bool SyntaxJob::doCompilerPrint(AstNode* parent)
{
    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#print can only be declared in the top level scope"}));

    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::CompilerPrint, sourceFile->indexInModule, parent);
    node->inheritOwners(this);
    node->semanticFct = &SemanticJob::resolveCompilerPrint;
    node->token       = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    return true;
}

bool SyntaxJob::doCompilerRunDecl(AstNode* parent)
{
    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#run can only be declared in the top level scope"}));

    auto runNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::CompilerRun, sourceFile->indexInModule, parent);
    runNode->inheritOwners(this);
    runNode->semanticFct = &SemanticJob::resolveCompilerRun;
    runNode->token       = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id != TokenId::SymLeftCurly)
        return doExpression(runNode);

    // Generated function
    auto funcNode = Ast::newNode(&g_Pool_astFuncDecl, AstNodeKind::FuncDecl, sourceFile->indexInModule, parent);
    funcNode->inheritOwners(this);
    funcNode->semanticFct = &SemanticJob::resolveFuncDecl;
    funcNode->attributeFlags |= ATTRIBUTE_COMPILER;

    auto typeNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::FuncDeclType, sourceFile->indexInModule, funcNode);
    typeNode->inheritOwners(this);
    typeNode->semanticFct = &SemanticJob::resolveFuncDeclType;

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = g_Pool_typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(sourceFile, funcNode->name, ScopeKind::Function, currentScope);
        int id               = g_Global.uniqueID.fetch_add(1);
        funcNode->name       = "__" + to_string(id);
        typeInfo->name       = funcNode->name;
        funcNode->typeInfo   = typeInfo;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode, SymbolKind::Function);
    }

    // #run content is the function body
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
    }

    // Generate a call
    Ast::createIdentifierRef(this, funcNode->name, runNode->token, runNode);
    return true;
}

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#unittest can only be declared in the top level scope"}));
    SWAG_CHECK(tokenizer.getToken(token));

	// ERROR
    if (token.text == "error")
    {
        if (g_CommandLine.unittest)
            sourceFile->unittestError++;
    }

	// BACKEND
    else if (token.text == "backend")
    {
		SWAG_CHECK(tokenizer.getToken(token));
        if (token.text == "lib")
        {
            if (g_CommandLine.unittest)
                sourceFile->module->backendParameters.type = BackendType::Lib;
        }
        else if (token.text == "dll")
        {
            if (g_CommandLine.unittest)
                sourceFile->module->backendParameters.type = BackendType::Dll;
        }
        else if (token.text == "exe")
        {
            if (g_CommandLine.unittest)
                sourceFile->module->backendParameters.type = BackendType::Exe;
        }
        else
        {
            sourceFile->report({sourceFile, token, format("invalid backend parameter '%s'", token.text.c_str())});
            return false;
        }
    }

	// PASS
    else if (token.text == "pass")
    {
        SWAG_CHECK(tokenizer.getToken(token));
        if (token.text == "lexer")
        {
            if (g_CommandLine.unittest)
                sourceFile->buildPass = BuildPass::Lexer;
        }
        else if (token.text == "syntax")
        {
            if (g_CommandLine.unittest)
                sourceFile->buildPass = BuildPass::Syntax;
        }
        else if (token.text == "semantic")
        {
            if (g_CommandLine.unittest)
                sourceFile->buildPass = BuildPass::Semantic;
        }
        else if (token.text == "backend")
        {
            if (g_CommandLine.unittest)
                sourceFile->buildPass = BuildPass::Backend;
        }
        else
        {
            sourceFile->report({sourceFile, token, format("invalid pass name '%s'", token.text.c_str())});
            return false;
        }

        sourceFile->module->setBuildPass(sourceFile->buildPass);
    }

	// MODULE
    else if (token.text == "module")
    {
        SWAG_VERIFY(!moduleSpecified, sourceFile->report({sourceFile, token, "#unittest module can only be specified once"}));
        SWAG_VERIFY(canChangeModule, sourceFile->report({sourceFile, token, "#unittest module instruction must be done before any code"}));
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_VERIFY(token.id == TokenId::Identifier, sourceFile->report({sourceFile, token, format("invalid module name '%s'", token.text.c_str())}));
        moduleSpecified = true;
        if (g_CommandLine.unittest)
        {
            auto newModule = g_Workspace.createOrUseModule(token.text);
            sourceFile->module->removeFile(sourceFile);
            newModule->addFile(sourceFile);
            currentScope = newModule->scopeRoot;
        }
    }

	// ???
    else
    {
        sourceFile->report({sourceFile, token, format("unknown #unittest parameter '%s'", token.text.c_str())});
        return false;
    }

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    return true;
}

bool SyntaxJob::doCompilerImport(AstNode* parent)
{
    SWAG_VERIFY(currentScope->kind == ScopeKind::Module, sourceFile->report({sourceFile, token, "#assert can only be declared in the top level scope"}));

    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::CompilerImport, sourceFile->indexInModule, parent);
    node->inheritOwners(this);
    node->inheritToken(token);

    AstNode* moduleName;
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doLiteral(nullptr, &moduleName));
    SWAG_VERIFY(moduleName->token.literalType == g_TypeMgr.typeInfoString, sourceFile->report({sourceFile, moduleName, "#import must be followed by a module name"}));
    node->name = moduleName->token.text;
	node->token.endLocation = token.endLocation;
    SWAG_CHECK(eatToken(TokenId::SymSemiColon));

    sourceFile->module->addDependency(node);

    return true;
}
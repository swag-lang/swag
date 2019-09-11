#include "pch.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Workspace.h"
#include "Module.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "SymTable.h"
#include "Attribute.h"
#include "TypeManager.h"

bool SyntaxJob::doCompilerIf(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(this, &g_Pool_astIf, AstNodeKind::If, sourceFile->indexInModule, parent);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
    SWAG_CHECK(doEmbeddedStatement(node, &node->ifBlock));
    node->boolExpression->semanticAfterFct = &SemanticJob::resolveCompilerIf;

    if (token.id == TokenId::CompilerElse)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doEmbeddedStatement(node, &node->elseBlock));
    }

    return true;
}

bool SyntaxJob::doCompilerAssert(AstNode* parent)
{
    auto node = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::CompilerAssert, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveCompilerAssert;
    node->token       = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymLeftParen)
    {
		SWAG_CHECK(eatToken());
		SWAG_CHECK(doExpression(node));
        if (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::LiteralString, sourceFile->report({sourceFile, token, "invalid #assert message"}));
            node->name = token.text;
			SWAG_CHECK(eatToken());
        }

		SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }
    else
        SWAG_CHECK(doExpression(node));

    SWAG_CHECK(eatSemiCol("after '#compiler' expression"));

    return true;
}

bool SyntaxJob::doCompilerPrint(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, "#print can only be declared in the top level scope"}));

    auto node = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::CompilerPrint, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveCompilerPrint;
    node->token       = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatSemiCol("after '#print' expression"));

    return true;
}

bool SyntaxJob::doCompilerVersion(AstNode* parent)
{
    auto node = Ast::newNode(this, &g_Pool_astIf, AstNodeKind::CompilerVersion, sourceFile->indexInModule, parent);
    node->token = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doIdentifierRef(nullptr, &node->boolExpression));
    SWAG_CHECK(eatToken(TokenId::SymRightParen));

    auto version          = node->boolExpression->childs.back()->name;
    bool versionValidated = sourceFile->module->compileVersion.find(version) != sourceFile->module->compileVersion.end();

    uint64_t ifFlags = versionValidated ? 0 : AST_DISABLED;
    {
        ScopedFlags scopedFlags(this, ifFlags);
        SWAG_CHECK(doStatement(nullptr, &node->ifBlock));
    }

    if (token.id == TokenId::CompilerElse)
    {
        uint64_t elseFlags = versionValidated ? AST_DISABLED : 0;
        SWAG_CHECK(tokenizer.getToken(token));
        {
            ScopedFlags scopedFlags(this, elseFlags);
            SWAG_CHECK(doStatement(nullptr, &node->elseBlock));
        }
    }

    if (versionValidated)
        Ast::addChildBack(node, node->ifBlock);
    else if (node->elseBlock)
        Ast::addChildBack(node, node->elseBlock);

    return true;
}

bool SyntaxJob::doCompilerRunDecl(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, "#run can only be declared in the top level scope"}));

    auto runNode = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::CompilerRun, sourceFile->indexInModule, parent);
    runNode->semanticFct = &SemanticJob::resolveCompilerRun;
    runNode->token       = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id != TokenId::SymLeftCurly)
        return doExpression(runNode);

    // Generated function
    auto funcNode = Ast::newNode(this, &g_Pool_astFuncDecl, AstNodeKind::FuncDecl, sourceFile->indexInModule, parent);
    funcNode->semanticFct = &SemanticJob::resolveFuncDecl;
    funcNode->attributeFlags |= ATTRIBUTE_COMPILER;

    auto typeNode = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclType, sourceFile->indexInModule, funcNode);
    typeNode->semanticFct = &SemanticJob::resolveFuncDeclType;

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = g_Pool_typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(funcNode, funcNode->name, ScopeKind::Function, currentScope);
        int id               = g_Global.uniqueID.fetch_add(1);
        funcNode->name       = "__" + to_string(id);
        funcNode->typeInfo   = typeInfo;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode, SymbolKind::Function);
    }

    // run content is the function body
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
    }

    // Generate a call
    Ast::createIdentifierRef(this, funcNode->name, runNode->token, runNode);
    return true;
}

bool SyntaxJob::doCompilerModule()
{
    if (!isContextDisabled())
    {
        SWAG_VERIFY(!moduleSpecified, sourceFile->report({sourceFile, token, "#module can only be specified once"}));
        SWAG_VERIFY(canChangeModule, sourceFile->report({sourceFile, token, "#module instruction must be done before any code"}));
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_VERIFY(token.id == TokenId::Identifier, sourceFile->report({sourceFile, token, format("invalid module name '%s'", token.text.c_str())}));
        moduleSpecified = true;

        auto newModule = g_Workspace.createOrUseModule(token.text);
        newModule->compileVersion.insert(sourceFile->module->compileVersion.begin(), sourceFile->module->compileVersion.end());
        sourceFile->module->removeFile(sourceFile);
        newModule->addFile(sourceFile);
        currentScope = sourceFile->scopeRoot;
    }

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatSemiCol("after module name"));
    return true;
}

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, "#unittest can only be declared in the top level scope"}));
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
            if (g_CommandLine.unittest && !isContextDisabled())
                sourceFile->buildPass = BuildPass::Lexer;
        }
        else if (token.text == "syntax")
        {
            if (g_CommandLine.unittest && !isContextDisabled())
                sourceFile->buildPass = BuildPass::Syntax;
        }
        else if (token.text == "semantic")
        {
            if (g_CommandLine.unittest && !isContextDisabled())
                sourceFile->buildPass = BuildPass::Semantic;
        }
        else if (token.text == "backend")
        {
            if (g_CommandLine.unittest && !isContextDisabled())
                sourceFile->buildPass = BuildPass::Backend;
        }
        else
        {
            sourceFile->report({sourceFile, token, format("invalid pass name '%s'", token.text.c_str())});
            return false;
        }

        if (!isContextDisabled())
            sourceFile->module->setBuildPass(sourceFile->buildPass);
    }

    // ???
    else
    {
        sourceFile->report({sourceFile, token, format("unknown #unittest parameter '%s'", token.text.c_str())});
        return false;
    }

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatSemiCol("after unittest expression"));
    return true;
}

bool SyntaxJob::doCompilerImport(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, "#assert can only be declared in the top level scope"}));

    auto node = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::CompilerImport, sourceFile->indexInModule, parent);

    SWAG_CHECK(tokenizer.getToken(token));
    AstNode* identifier;
    SWAG_CHECK(doIdentifierRef(nullptr, &identifier));
    auto moduleName         = identifier->childs.back()->name;
    node->name              = moduleName;
    node->token.endLocation = identifier->childs.back()->token.endLocation;
    SWAG_CHECK(eatSemiCol("after import expression"));

    if (!isContextDisabled())
        sourceFile->module->addDependency(node);

    return true;
}
#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "Module.h"

bool SyntaxJob::doCompilerForeignLib(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstIf>(this, AstNodeKind::CompilerForeignLib, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerForeignLib;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::LiteralString, syntaxError(token, "#foreignlib invalid string"));

    AstNode* literal;
    SWAG_CHECK(doLiteral(node, &literal));
    SWAG_VERIFY(literal->token.literalType->isNative(NativeTypeKind::String), syntaxError(literal->token, "#foreignlib invalid string"));
    return true;
}

bool SyntaxJob::doCompilerIf(AstNode* parent, AstNode** result)
{
    return doCompilerIfFor(parent, result, AstNodeKind::Statement);
}

bool SyntaxJob::doCompilerIfFor(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    auto node = Ast::newNode<AstIf>(this, AstNodeKind::CompilerIf, sourceFile, parent);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly, "missing '#if' expression before '{'"));
    SWAG_CHECK(doExpression(node, &node->boolExpression));
    node->boolExpression->semanticAfterFct = SemanticJob::resolveCompilerIf;

    {
        auto block    = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->ifBlock = block;
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->blocks.push_back(block);

        ScopedCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(doStatementFor(block, nullptr, kind));
    }

    if (token.id == TokenId::CompilerElse || token.id == TokenId::CompilerElseIf)
    {
        auto block      = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->elseBlock = block;
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->blocks.push_back(block);

        ScopedCompilerIfBlock scopedIf(this, block);
        if (token.id == TokenId::CompilerElseIf)
            SWAG_CHECK(doCompilerIf(block));
        else
        {
            SWAG_CHECK(tokenizer.getToken(token));
            SWAG_CHECK(doStatementFor(block, nullptr, kind));
        }
    }

    return true;
}

bool SyntaxJob::doCompilerMixin(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerMixin>(this, AstNodeKind::CompilerMixin, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerMixin;
    node->token       = move(token);

    SWAG_CHECK(tokenizer.getToken(token));

    // Code identifier
    SWAG_CHECK(doIdentifierRef(node));

    // Replacement parameters
    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(eatToken());
        AstNode* stmt;
        while (token.id != TokenId::SymRightCurly)
        {
            auto tokenId = token.id;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatToken(TokenId::SymEqual));
            SWAG_CHECK(doEmbeddedInstruction(nullptr, &stmt));
            node->replaceTokens[tokenId] = stmt;
            SWAG_CHECK(eatSemiCol("after '#mixin' replacement statement"));
        }

        SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    }

    SWAG_CHECK(eatSemiCol("after '#mixin' expression"));
    return true;
}

bool SyntaxJob::doCompilerMacro(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerMacro>(this, AstNodeKind::CompilerMacro, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticBeforeFct = SemanticJob::resolveCompilerMacro;

    SWAG_CHECK(tokenizer.getToken(token));
    auto newScope = Ast::newScope(node, "", ScopeKind::Macro, node->ownerScope);

    Scoped         scoped(this, newScope);
    ScopedMainNode scopedMainNode(this, node);
    SWAG_CHECK(doCurlyStatement(node));
    return true;
}

bool SyntaxJob::doCompilerInline(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerInline>(this, AstNodeKind::CompilerInline, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticBeforeFct = SemanticJob::resolveCompilerInline;

    SWAG_CHECK(tokenizer.getToken(token));
    auto newScope = Ast::newScope(node, "", ScopeKind::Inline, node->ownerScope);

    Scoped         scoped(this, newScope);
    ScopedMainNode scopedMainNode(this, node);
    SWAG_CHECK(doCurlyStatement(node));
    return true;
}

bool SyntaxJob::doCompilerAssert(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerAssert, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerAssert;
    node->token       = move(token);

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node));
        if (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doExpression(node));
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }
    else
        SWAG_CHECK(doExpression(node));

    SWAG_CHECK(eatSemiCol("after '#compiler' expression"));

    return true;
}

bool SyntaxJob::doCompilerAst(AstNode* parent, AstNode** result, CompilerAstKind kind)
{
    auto node = Ast::newNode<AstCompilerAst>(this, AstNodeKind::CompilerAst, sourceFile, parent);
    if (result)
        *result = node;
    node->embeddedKind = kind;
    node->semanticFct  = SemanticJob::resolveCompilerAstExpression;
    SWAG_CHECK(eatToken());

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(sourceFile->astRoot, &funcNode, TokenId::CompilerAst));
        auto idRef                 = Ast::newIdentifierRef(sourceFile, funcNode->name, node, this);
        idRef->token.startLocation = node->token.startLocation;
        idRef->token.endLocation   = node->token.endLocation;
        auto identifier            = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
        identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier, this);
        identifier->token.startLocation = node->token.startLocation;
        identifier->token.endLocation   = node->token.endLocation;
    }
    else
    {
        SWAG_CHECK(doExpression(node));
        SWAG_CHECK(eatSemiCol("after '#ast' expression"));
    }

    return true;
}

bool SyntaxJob::doCompilerRunStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerRun, sourceFile, parent);
    if (result)
        *result = node;

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    SWAG_CHECK(doEmbeddedInstruction(node));
    SWAG_CHECK(eatSemiCol("after '#run' expression"));
    return true;
}

bool SyntaxJob::doCompilerRunExpression(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerRun, sourceFile, parent);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE;

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK);
    node->semanticFct = SemanticJob::resolveCompilerRun;
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatSemiCol("after '#run' expression"));
    return true;
}

bool SyntaxJob::doCompilerPrint(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPrint, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerPrint;
    node->token       = move(token);

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatSemiCol("after '#print' expression"));

    return true;
}

bool SyntaxJob::doCompilerModule()
{
    SWAG_VERIFY(!moduleSpecified, sourceFile->report({sourceFile, token, "#module can only be specified once"}));
    SWAG_VERIFY(canChangeModule, sourceFile->report({sourceFile, token, "#module instruction must be done before any code"}));
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, sourceFile->report({sourceFile, token, format("invalid module name '%s'", token.text.c_str())}));
    moduleSpecified = true;

    auto newModule       = g_Workspace.createOrUseModule(token.text);
    newModule->fromTestsFolder = sourceFile->module->fromTestsFolder;
    sourceFile->module->removeFile(sourceFile);
    newModule->addFile(sourceFile);
    currentScope = sourceFile->scopeRoot;

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
        if (g_CommandLine.test)
        {
            sourceFile->unittestError++;
            sourceFile->module->hasUnittestError = true;
        }
    }

    // PASS
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
        else if (token.text == "backend")
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Backend;
        }
        else
        {
            sourceFile->report({sourceFile, token, format("invalid pass name '%s'", token.text.c_str())});
            return false;
        }

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
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, "#import can only be declared in the top level scope"}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerImport, sourceFile, parent);

    SWAG_CHECK(tokenizer.getToken(token));
    AstNode* identifier;
    SWAG_CHECK(doIdentifierRef(nullptr, &identifier));
    auto moduleName         = identifier->childs.back()->name;
    node->name              = moduleName;
    node->token.endLocation = identifier->childs.back()->token.endLocation;
    SWAG_CHECK(eatSemiCol("after import expression"));
    sourceFile->module->addDependency(node);
    return true;
}
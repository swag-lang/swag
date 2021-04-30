#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "Module.h"
#include "ByteCodeGenJob.h"
#include "ErrorIds.h"

bool SyntaxJob::doCompilerTag(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerSpecialFunction, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerSpecialFunction;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));

    if (node->token.id == TokenId::CompilerTagVal)
    {
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));

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
    SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, Msg0878));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
    node->boolExpression->allocateExtension();
    node->boolExpression->extension->semanticAfterFct = SemanticJob::resolveCompilerIf;

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
        SWAG_VERIFY(node->ownerBreakable, error(token, Msg0364));
        SWAG_CHECK(eatToken());
        if (token.id != TokenId::KwdBreak && token.id != TokenId::KwdContinue)
            return error(token, format(Msg0365, token.text.c_str()));
        SWAG_VERIFY(token.id != TokenId::SymRightCurly, error(token, Msg0366));

        AstNode* stmt;
        while (token.id != TokenId::SymRightCurly)
        {
            auto tokenId = token.id;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatToken(TokenId::SymEqual));
            SWAG_CHECK(doEmbeddedInstruction(nullptr, &stmt));
            node->replaceTokens[tokenId] = stmt;
            SWAG_CHECK(eatSemiCol("'#mixin' replacement statement"));
        }

        SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    }

    SWAG_CHECK(eatSemiCol("'#mixin' expression"));
    return true;
}

bool SyntaxJob::doCompilerMacro(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerMacro>(this, AstNodeKind::CompilerMacro, sourceFile, parent);
    if (result)
        *result = node;
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::resolveCompilerMacro;

    SWAG_CHECK(tokenizer.getToken(token));
    auto newScope = Ast::newScope(node, "", ScopeKind::Macro, node->ownerScope);
    node->scope   = newScope;

    Scoped scoped(this, newScope);
    SWAG_CHECK(doCurlyStatement(node));
    return true;
}

bool SyntaxJob::doCompilerInline(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerInline>(this, AstNodeKind::CompilerInline, sourceFile, parent);
    if (result)
        *result = node;
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::resolveCompilerInline;

    SWAG_CHECK(tokenizer.getToken(token));
    auto newScope = Ast::newScope(node, "", ScopeKind::Inline, node->ownerScope);
    node->scope   = newScope;

    Scoped scoped(this, newScope);
    SWAG_CHECK(doCurlyStatement(node));
    return true;
}

bool SyntaxJob::doCompilerAssert(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerAssert, sourceFile, parent);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                  = SemanticJob::resolveCompilerAssert;
    node->token                        = move(token);

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
        if (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }
    else
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));

    SWAG_CHECK(eatSemiCol("'#compiler' expression"));

    return true;
}

bool SyntaxJob::doCompilerSelectIf(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerSelectIf, sourceFile, parent);
    if (result)
        *result = node;
    auto tokenId = token.id;
    if (tokenId == TokenId::CompilerCheckIf)
        node->kind = AstNodeKind::CompilerCheckIf;
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                  = SemanticJob::resolveCompilerSelectIfExpression;
    SWAG_CHECK(eatToken());
    parent->flags |= AST_HAS_SELECT_IF;
    node->flags |= AST_NO_BYTECODE_CHILDS;

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND | AST_IN_SELECTIF);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, tokenId));

        ScopedFlags scoped(this, AST_NO_CALLSTACK);
        auto        idRef               = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
        idRef->token.startLocation      = node->token.startLocation;
        idRef->token.endLocation        = node->token.endLocation;
        auto identifier                 = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
        identifier->callParameters      = Ast::newFuncCallParams(sourceFile, identifier, this);
        identifier->token.startLocation = node->token.startLocation;
        identifier->token.endLocation   = node->token.endLocation;
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
        SWAG_CHECK(eatSemiCol("'#selectif' expression"));
    }

    return true;
}

bool SyntaxJob::doCompilerAst(AstNode* parent, AstNode** result, CompilerAstKind kind)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerAst, sourceFile, parent);
    if (result)
        *result = node;
    node->embeddedKind = kind;
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                  = SemanticJob::resolveCompilerAstExpression;
    SWAG_CHECK(eatToken());

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerAst));

        ScopedFlags scoped(this, AST_NO_CALLSTACK);
        auto        idRef               = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
        idRef->token.startLocation      = node->token.startLocation;
        idRef->token.endLocation        = node->token.endLocation;
        auto identifier                 = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
        identifier->callParameters      = Ast::newFuncCallParams(sourceFile, identifier, this);
        identifier->token.startLocation = node->token.startLocation;
        identifier->token.endLocation   = node->token.endLocation;
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
        SWAG_CHECK(eatSemiCol("'#ast' expression"));
    }

    return true;
}

bool SyntaxJob::doCompilerRunTopLevel(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doFuncDecl(parent, result, TokenId::CompilerRun));
        return true;
    }

    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerRun, sourceFile, parent);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->semanticFct = SemanticJob::resolveCompilerRun;
    SWAG_CHECK(doEmbeddedInstruction(node));
    SWAG_CHECK(eatSemiCol("'#run' statement"));
    return true;
}

bool SyntaxJob::doCompilerRunEmbedded(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerRun, sourceFile, parent);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                  = SemanticJob::resolveCompilerRun;
    node->token                        = move(token);
    SWAG_CHECK(eatToken());

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerGeneratedRun));

        ScopedFlags scoped(this, AST_NO_CALLSTACK);
        auto        idRef               = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
        idRef->token.startLocation      = node->token.startLocation;
        idRef->token.endLocation        = node->token.endLocation;
        auto identifier                 = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
        identifier->callParameters      = Ast::newFuncCallParams(sourceFile, identifier, this);
        identifier->token.startLocation = node->token.startLocation;
        identifier->token.endLocation   = node->token.endLocation;
    }
    else
    {
        SWAG_CHECK(doEmbeddedInstruction(node));
        SWAG_CHECK(eatSemiCol("'#run' expression"));
    }

    return true;
}

bool SyntaxJob::doCompilerSemError(AstNode* parent, AstNode** result, bool embedded)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerSemError, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerTestError;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->token = move(token);
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(sourceFile->module->kind == ModuleKind::Test, sourceFile->report({sourceFile, token, Msg0367}));
    SWAG_ASSERT(g_CommandLine.test);
    SWAG_VERIFY(!currentCompilerIfBlock, sourceFile->report({sourceFile, token, Msg0368}));

    if (embedded)
        SWAG_CHECK(doEmbeddedInstruction(node));
    else
        SWAG_CHECK(doTopLevelInstruction(node));

    return true;
}

bool SyntaxJob::doCompilerPrint(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPrint, sourceFile, parent);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                  = SemanticJob::resolveCompilerPrint;
    node->token                        = move(token);
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    SWAG_CHECK(eatSemiCol("'#print' expression"));
    return true;
}

bool SyntaxJob::doCompilerGlobal(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(!afterGlobal, error(token, Msg0369));

    SWAG_CHECK(tokenizer.getToken(token));

    /////////////////////////////////
    if (token.text == "export")
    {
        if (!sourceFile->imported)
        {
            SWAG_VERIFY(!sourceFile->forceExport, error(token, Msg0370));
            sourceFile->forceExport = true;
            sourceFile->module->addExportSourceFile(sourceFile);
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global export'"));
    }

    /////////////////////////////////
    else if (token.text == "generated")
    {
        sourceFile->isGenerated = true;
        if (sourceFile->imported)
            sourceFile->imported->isSwag = true;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global generated'"));
    }

    /////////////////////////////////
    else if (token.text == "foreignlib")
    {
        auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerForeignLib, sourceFile, parent);
        if (result)
            *result = node;
        node->semanticFct = SemanticJob::resolveCompilerForeignLib;

        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, Msg0371));

        AstNode* literal;
        SWAG_CHECK(doLiteral(node, &literal));
        if (literal->token.literalType != LiteralType::TT_STRING &&
            literal->token.literalType != LiteralType::TT_RAW_STRING &&
            literal->token.literalType != LiteralType::TT_ESCAPE_STRING)
            return error(literal->token, Msg0371);
        SWAG_CHECK(eatSemiCol("'#global foreignlib'"));
    }

    /////////////////////////////////
    else if (token.text == "if")
    {
        auto node = Ast::newNode<AstIf>(this, AstNodeKind::CompilerIf, sourceFile, parent);
        if (result)
            *result = node;
        node->flags |= AST_GLOBAL_NODE;

        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, Msg0879));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        node->boolExpression->allocateExtension();
        node->boolExpression->extension->semanticAfterFct = SemanticJob::resolveCompilerIf;

        auto block    = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->ifBlock = block;
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->blocks.push_back(block);
        block->flags |= AST_GLOBAL_NODE;

        ScopedCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(eatSemiCol("'#global if'"));
        while (token.id != TokenId::EndOfFile)
        {
            SWAG_CHECK(doTopLevelInstruction(block));
        }
    }

    /////////////////////////////////
    else if (token.text == "namespace")
    {
        SWAG_CHECK(doNamespace(parent, result, true));
    }

    /////////////////////////////////
    else if (token.text == "public" || token.text == "protected" || token.text == "private")
    {
        SWAG_CHECK(doGlobalAttributeExpose(parent, result, true));
    }

    /////////////////////////////////
    else if (token.text == "skip")
    {
        sourceFile->buildPass = BuildPass::Lexer;
    }

    /////////////////////////////////
    else if (token.text == "testpass")
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
            sourceFile->report({sourceFile, token, format(Msg0373, token.text.c_str())});
            return false;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global testpass'"));
    }

    /////////////////////////////////
    else if (token.text == "testerror" || token.text == "testerrors")
    {
        // Put the file in its own module, because of errors
        if (!moduleSpecified)
        {
            moduleSpecified = true;
            auto newModule  = g_Workspace.createOrUseModule(sourceFile->name, sourceFile->module->path, sourceFile->module->kind);
            auto oldModule  = sourceFile->module;
            oldModule->removeFile(sourceFile);
            newModule->addFile(sourceFile);
            oldModule->addErrorModule(newModule);
        }

        SWAG_VERIFY(sourceFile->module->kind == ModuleKind::Test, sourceFile->report({sourceFile, token, Msg0374}));
        SWAG_ASSERT(g_CommandLine.test);

        if (token.text == "testerrors")
            sourceFile->multipleTestErrors = true;
        else
        {
            sourceFile->numTestErrors++;
            sourceFile->module->numTestErrors++;
            if (currentCompilerIfBlock)
                currentCompilerIfBlock->numTestErrors++;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global testerror'"));
    }

    /////////////////////////////////
    else if (token.text == "testwarning" || token.text == "testwarnings")
    {
        // Put the file in its own module, because of errors
        if (!moduleSpecified)
        {
            moduleSpecified = true;
            auto newModule  = g_Workspace.createOrUseModule(sourceFile->name, sourceFile->module->path, sourceFile->module->kind);
            auto oldModule  = sourceFile->module;
            oldModule->removeFile(sourceFile);
            newModule->addFile(sourceFile);
            oldModule->addErrorModule(newModule);
        }

        SWAG_VERIFY(sourceFile->module->kind == ModuleKind::Test, sourceFile->report({sourceFile, token, Msg0375}));
        SWAG_ASSERT(g_CommandLine.test);

        if (token.text == "testwarnings")
            sourceFile->multipleTestWarnings = true;
        else
        {
            sourceFile->numTestWarnings++;
            sourceFile->module->numTestWarnings++;
            if (currentCompilerIfBlock)
                currentCompilerIfBlock->numTestWarnings++;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global testwarning'"));
    }

    /////////////////////////////////
    else if (token.text == "#[")
    {
        AstNode* resultNode;

        SWAG_CHECK(doAttrUse(nullptr, &resultNode, true));

        // We need to be sure that our parent is in the sourceFile->astAttrUse hierarchy.
        // If #global #[???] has a #global protected before (for example), this is not the case.
        if (sourceFile->astAttrUse)
        {
            parent = sourceFile->astAttrUse;
            Ast::addChildBack(parent, resultNode);
        }
        else
        {
            parent = sourceFile->astRoot;

            // Add front in case astRoot already has some childs (if the #global comes after another one).
            // We need #global attributes to be first in the file, before other #globals (namespaces, public/protected etc...).
            // Otherwise we can have a race condition between multiple globals.
            Ast::addChildFront(parent, resultNode);
        }

        auto attrUse          = (AstAttrUse*) resultNode;
        attrUse->isGlobal     = true;
        attrUse->ownerAttrUse = sourceFile->astAttrUse;
        attrUse->flags |= AST_GLOBAL_NODE;
        sourceFile->astAttrUse = attrUse;
        SWAG_CHECK(eatSemiCol("'#global attribute'"));
    }

    /////////////////////////////////
    else
    {
        return sourceFile->report({sourceFile, token, format(Msg0376, token.text.c_str())});
    }

    return true;
}

bool SyntaxJob::doCompilerLocation(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::CompilerSpecialFunction, sourceFile, parent);
    if (result)
        *result = exprNode;
    SWAG_CHECK(eatToken());

    // Parameter
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(exprNode, nullptr, IDENTIFIER_NO_PARAMS));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    exprNode->semanticFct = SemanticJob::resolveCompilerSpecialFunction;
    return true;
}

bool SyntaxJob::doCompilerSpecialFunction(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::CompilerSpecialFunction, sourceFile, parent);
    if (result)
        *result = exprNode;
    SWAG_CHECK(eatToken());
    exprNode->semanticFct = SemanticJob::resolveCompilerSpecialFunction;
    return true;
}

bool SyntaxJob::doCompilerDefined(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::CompilerDefined, sourceFile, parent);
    if (result)
        *result = exprNode;
    exprNode->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));

    ScopedFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doIdentifierRef(exprNode, nullptr, IDENTIFIER_NO_PARAMS));

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    exprNode->semanticFct = SemanticJob::resolveCompilerDefined;
    return true;
}

bool SyntaxJob::doCompilerLoad(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::CompilerLoad, sourceFile, parent);
    if (result)
        *result = exprNode;
    exprNode->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));

    ScopedFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doExpression(exprNode, EXPR_FLAG_NONE, nullptr));

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    exprNode->semanticFct = SemanticJob::resolveCompilerLoad;
    return true;
}

bool SyntaxJob::doCompilerDependencies(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isCfgFile, sourceFile->report({sourceFile, token, Msg0377}));
    SWAG_VERIFY(parent->kind == AstNodeKind::File, sourceFile->report({sourceFile, token, Msg0268}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerDependencies, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doCurlyStatement(node));

    if (sourceFile->module->kind != ModuleKind::Config)
    {
        node->flags |= AST_NO_SEMANTIC;
        node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    }

    return true;
}

bool SyntaxJob::doCompilerImport(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isGenerated || sourceFile->isCfgFile, sourceFile->report({sourceFile, token, Msg0377}));
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, Msg0378}));

    // Be sure this is in a '#dependencies' block
    if (sourceFile->module->kind == ModuleKind::Config)
    {
        auto scan = parent;
        while (scan)
        {
            if (scan->kind == AstNodeKind::CompilerDependencies)
                break;
            scan = scan->parent;
        }
        SWAG_VERIFY(scan, sourceFile->report({sourceFile, token, Msg0235}));
    }

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerImport, sourceFile, parent);
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::LiteralString, sourceFile->report({sourceFile, token, Msg0379}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());

    // Specific dependency stuff
    Token tokenLocation, tokenVersion;
    if (sourceFile->isCfgFile)
    {
        while (true)
        {
            if (token.text == "location")
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_VERIFY(tokenLocation.text.empty(), error(token, Msg0380));
                SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, format(Msg0381, token.text.c_str())));
                tokenLocation = token;
                SWAG_CHECK(eatToken());
                continue;
            }

            if (token.text == "version")
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_VERIFY(tokenVersion.text.empty(), error(token, Msg0382));
                SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, format(Msg0383, token.text.c_str())));
                tokenVersion = token;
                SWAG_CHECK(eatToken());
                continue;
            }

            break;
        }
    }

    SWAG_CHECK(eatSemiCol("'#import' expression"));
    if (sourceFile->isGenerated || sourceFile->module->kind == ModuleKind::Config)
        SWAG_CHECK(sourceFile->module->addDependency(node, tokenLocation, tokenVersion));
    return true;
}

bool SyntaxJob::doCompilerPlaceHolder(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), sourceFile->report({sourceFile, token, Msg0384}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPlaceHolder, sourceFile, parent);
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, sourceFile->report({sourceFile, token, Msg0385}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatSemiCol("'#placeholder' expression"));

    currentScope->symTable.registerSymbolName(&context, node, SymbolKind::PlaceHolder);

    return true;
}

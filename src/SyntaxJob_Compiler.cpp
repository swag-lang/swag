#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"

bool SyntaxJob::doIntrinsicTag(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::IntrinsicProp, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveIntrinsicTag;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));

    if (node->token.id == TokenId::IntrinsicGetTag)
    {
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doTypeExpression(node));
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

    // Expression
    {
        ScopedFlags scopedFlags(this, AST_NO_BACKEND);
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(node->token, Err(Syn0083)));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        node->boolExpression->allocateExtension(ExtensionKind::Semantic);
        node->boolExpression->extension->semantic->semanticAfterFct = SemanticJob::resolveCompilerIf;
    }

    // Block
    {
        auto block    = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->ifBlock = block;
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->blocks.push_back(block);

        ScopedCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(doStatementFor(block, nullptr, kind));
    }

    // Else block
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
            SWAG_CHECK(eatToken());
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
    node->token       = token;

    SWAG_CHECK(eatToken());

    // Code identifier
    SWAG_CHECK(doIdentifierRef(node));

    // Replacement parameters
    if (token.id == TokenId::SymLeftCurly)
    {
        auto startLoc = token.startLocation;
        SWAG_VERIFY(node->ownerBreakable, error(token, Err(Syn0034)));
        SWAG_CHECK(eatToken());
        if (token.id != TokenId::KwdBreak && token.id != TokenId::KwdContinue)
            return error(token, Fmt(Err(Syn0016), token.ctext()));
        SWAG_VERIFY(token.id != TokenId::SymRightCurly, error(token, Err(Syn0015)));

        AstNode* stmt;
        while (token.id != TokenId::SymRightCurly)
        {
            auto tokenId = token.id;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatToken(TokenId::SymEqual));
            SWAG_CHECK(doEmbeddedInstruction(nullptr, &stmt));
            node->replaceTokens[tokenId] = stmt;
            if (token.id != TokenId::SymRightCurly)
                SWAG_CHECK(eatSemiCol("`#mixin` replacement statement"));
        }

        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the #mixin replacement statement"));
    }

    SWAG_CHECK(eatSemiCol("`#mixin` expression"));
    return true;
}

bool SyntaxJob::doCompilerMacro(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerMacro>(this, AstNodeKind::CompilerMacro, sourceFile, parent);
    if (result)
        *result = node;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::resolveCompilerMacro;

    SWAG_CHECK(eatToken());
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
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::resolveCompilerInline;

    SWAG_CHECK(eatToken());
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
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                            = SemanticJob::resolveCompilerAssert;
    node->token                                  = token;

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    SWAG_CHECK(eatToken());
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

    SWAG_CHECK(eatSemiCol("`#assert` expression"));
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
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                            = SemanticJob::resolveCompilerSelectIfExpression;
    SWAG_CHECK(eatToken());
    parent->flags |= AST_HAS_SELECT_IF;
    node->flags |= AST_NO_BYTECODE_CHILDS;

    // Not for the 3 special special functions
    if (parent->token.text == g_LangSpec->name_opDrop ||
        parent->token.text == g_LangSpec->name_opPostCopy ||
        parent->token.text == g_LangSpec->name_opPostMove)
    {
        return error(node, Fmt(Err(Syn0137), parent->token.ctext()), Hlp(Hlp0034));
    }

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND | AST_IN_SELECTIF);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, tokenId));

        auto idRef                      = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
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
        SWAG_CHECK(eatSemiCol("`#selectif/#checkif` expression"));
    }

    return true;
}

bool SyntaxJob::doCompilerAst(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerAst, sourceFile, parent);
    if (result)
        *result = node;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                            = SemanticJob::resolveCompilerAstExpression;
    SWAG_CHECK(eatToken());

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerAst));

        auto idRef                      = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
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
        SWAG_CHECK(eatSemiCol("`#ast` expression"));
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
    SWAG_CHECK(eatSemiCol("`#run` statement"));
    return true;
}

bool SyntaxJob::doCompilerRunEmbedded(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerRun, sourceFile, parent);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                            = SemanticJob::resolveCompilerRun;
    node->token                                  = token;
    SWAG_CHECK(eatToken());

    ScopedFlags scopedFlags(this, AST_RUN_BLOCK | AST_NO_BACKEND);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerGeneratedRun));

        auto idRef                      = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
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
        SWAG_CHECK(eatSemiCol("`#run` expression"));
    }

    return true;
}

bool SyntaxJob::doCompilerPrint(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPrint, sourceFile, parent);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;
    node->semanticFct                            = SemanticJob::resolveCompilerPrint;
    node->token                                  = token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    SWAG_CHECK(eatSemiCol("`#print` expression"));
    return true;
}

bool SyntaxJob::doCompilerForeignLib(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerForeignLib, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerForeignLib;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, Err(Syn0002)));

    AstNode* literal;
    SWAG_CHECK(doLiteral(node, &literal));
    if (literal->token.literalType != LiteralType::TT_STRING &&
        literal->token.literalType != LiteralType::TT_RAW_STRING &&
        literal->token.literalType != LiteralType::TT_ESCAPE_STRING)
        return error(literal->token, Err(Syn0002));
    SWAG_CHECK(eatSemiCol("`#foreignlib`"));
    return true;
}

bool SyntaxJob::doCompilerGlobal(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(!afterGlobal, error(token, Err(Syn0006)));
    SWAG_CHECK(eatToken());

    /////////////////////////////////
    if (token.text == g_LangSpec->name_export)
    {
        if (!sourceFile->imported)
        {
            SWAG_VERIFY(!sourceFile->forceExport, error(token, Err(Syn0001)));
            sourceFile->forceExport = true;
            sourceFile->module->addExportSourceFile(sourceFile);
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("`#global export`"));
    }

    /////////////////////////////////
    else if (token.text == g_LangSpec->name_generated)
    {
        sourceFile->isGenerated = true;
        if (sourceFile->imported)
            sourceFile->imported->isSwag = true;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("`#global generated`"));
    }

    /////////////////////////////////
    else if (token.id == TokenId::KwdIf)
    {
        auto node = Ast::newNode<AstIf>(this, AstNodeKind::CompilerIf, sourceFile, parent);
        if (result)
            *result = node;
        node->flags |= AST_GLOBAL_NODE;

        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(node->token, Err(Syn0082)));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        node->boolExpression->allocateExtension(ExtensionKind::Semantic);
        node->boolExpression->extension->semantic->semanticAfterFct = SemanticJob::resolveCompilerIf;

        auto block    = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->ifBlock = block;
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->blocks.push_back(block);
        block->flags |= AST_GLOBAL_NODE;

        ScopedCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(eatSemiCol("`#global if`"));
        while (token.id != TokenId::EndOfFile)
        {
            SWAG_CHECK(doTopLevelInstruction(block));
        }
    }

    /////////////////////////////////
    else if (token.id == TokenId::KwdNamespace)
    {
        SWAG_CHECK(doNamespace(parent, result, true, false));
    }

    /////////////////////////////////
    else if (token.id == TokenId::KwdPublic || token.id == TokenId::KwdPrivate)
    {
        SWAG_CHECK(doGlobalAttributeExpose(parent, result, true));
    }

    /////////////////////////////////
    else if (token.text == g_LangSpec->name_skip)
    {
        sourceFile->buildPass = BuildPass::Lexer;
    }

    /////////////////////////////////
    else if (token.text == g_LangSpec->name_testpass)
    {
        SWAG_CHECK(eatToken());
        if (token.text == g_LangSpec->name_lexer)
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Lexer;
        }
        else if (token.text == g_LangSpec->name_syntax)
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Syntax;
        }
        else if (token.text == g_LangSpec->name_semantic)
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Semantic;
        }
        else if (token.text == g_LangSpec->name_backend)
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Backend;
        }
        else
        {
            Report::report({sourceFile, token, Fmt(Err(Syn0167), token.ctext())});
            return false;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("`#global testpass`"));
    }

    /////////////////////////////////
    else if (token.text == g_LangSpec->name_testerror || token.text == g_LangSpec->name_testerrors)
    {
        // Put the file in its own module, because of errors
        if (!moduleSpecified)
        {
            moduleSpecified = true;
            auto newModule  = g_Workspace->createOrUseModule(sourceFile->name, sourceFile->module->path, sourceFile->module->kind, true);
            auto oldModule  = sourceFile->module;
            oldModule->removeFile(sourceFile);
            newModule->addFile(sourceFile);

            for (auto dep : oldModule->moduleDependencies)
            {
                ModuleDependency* newDep = g_Allocator.alloc<ModuleDependency>();
                *newDep                  = *dep;
                newDep->importDone       = false;
                newModule->moduleDependencies.push_back(newDep);
            }

            oldModule->addErrorModule(newModule);
            module = newModule;
        }

        SWAG_VERIFY(sourceFile->module->kind == ModuleKind::Test, Report::report({sourceFile, token, Err(Syn0003)}));
        SWAG_ASSERT(g_CommandLine.test);

        if (token.text == g_LangSpec->name_testerrors)
            sourceFile->multipleTestErrors = true;
        else
        {
            sourceFile->numTestErrors++;
            sourceFile->module->numTestErrors++;
            if (currentCompilerIfBlock)
                currentCompilerIfBlock->numTestErrors++;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("`#global testerror`"));
    }

    /////////////////////////////////
    else if (token.text == g_LangSpec->name_testwarning || token.text == g_LangSpec->name_testwarnings)
    {
        // Put the file in its own module, because of errors
        if (!moduleSpecified)
        {
            moduleSpecified = true;
            auto newModule  = g_Workspace->createOrUseModule(sourceFile->name, sourceFile->module->path, sourceFile->module->kind, true);
            auto oldModule  = sourceFile->module;
            oldModule->removeFile(sourceFile);
            newModule->addFile(sourceFile);
            oldModule->addErrorModule(newModule);
            module = newModule;
        }

        SWAG_VERIFY(sourceFile->module->kind == ModuleKind::Test, Report::report({sourceFile, token, Err(Syn0004)}));
        SWAG_ASSERT(g_CommandLine.test);

        if (token.text == g_LangSpec->name_testwarnings)
        {
            sourceFile->multipleTestWarnings = true;
            sourceFile->module->numTestWarnings++;
        }
        else
        {
            sourceFile->numTestWarnings++;
            sourceFile->module->numTestWarnings++;
            if (currentCompilerIfBlock)
                currentCompilerIfBlock->numTestWarnings++;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("`#global testwarning`"));
    }

    /////////////////////////////////
    else if (token.id == TokenId::SymAttrStart)
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
            // We need #global attributes to be first in the file, before other #globals (namespaces, public etc...).
            // Otherwise we can have a race condition between multiple globals.
            Ast::addChildFront(parent, resultNode);
        }

        auto attrUse = (AstAttrUse*) resultNode;
        attrUse->specFlags |= AST_SPEC_ATTRUSE_GLOBAL;
        attrUse->allocateExtension(ExtensionKind::Owner);
        attrUse->extension->owner->ownerAttrUse = sourceFile->astAttrUse;
        attrUse->flags |= AST_GLOBAL_NODE;
        sourceFile->astAttrUse = attrUse;
        SWAG_CHECK(eatSemiCol("`#global attribute`"));
    }

    /////////////////////////////////
    else if (token.id == TokenId::KwdUsing)
    {
        SWAG_VERIFY(sourceFile->isCfgFile, Report::report({sourceFile, token, Err(Syn0005)}));

        int prevCount = parent->childs.count;
        SWAG_CHECK(doUsing(parent));
        while (prevCount != parent->childs.count)
        {
            sourceFile->module->buildParameters.globalUsings.push_back(parent->childs[prevCount]);
            prevCount++;
        }
    }

    /////////////////////////////////
    else
    {
        return Report::report({sourceFile, token, Fmt(Err(Syn0136), token.ctext())});
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

bool SyntaxJob::doIntrinsicDefined(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::IntrinsicDefined, sourceFile, parent);
    if (result)
        *result = exprNode;
    exprNode->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));

    ScopedFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doIdentifierRef(exprNode, nullptr, IDENTIFIER_NO_PARAMS));

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    exprNode->semanticFct = SemanticJob::resolveIntrinsicDefined;
    return true;
}

bool SyntaxJob::doCompilerDependencies(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isCfgFile || sourceFile->isScriptFile, Report::report({sourceFile, token, Err(Syn0183)}));
    SWAG_VERIFY(parent->kind == AstNodeKind::File, Report::report({sourceFile, token, Err(Syn0182)}));

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

bool SyntaxJob::doCompilerInclude(AstNode* parent, AstNode** result)
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
    exprNode->semanticFct = SemanticJob::resolveCompilerInclude;
    return true;
}

bool SyntaxJob::doCompilerLoad(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isCfgFile || sourceFile->isScriptFile, Report::report({sourceFile, token, Err(Syn0013)}));
    SWAG_VERIFY(currentScope->isTopLevel(), Report::report({sourceFile, token, Err(Syn0012)}));

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
        SWAG_VERIFY(scan, Report::report({sourceFile, token, Err(Syn0014)}));
    }

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerInclude, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::LiteralString, Report::report({sourceFile, token, Err(Syn0011)}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatSemiCol("`#include` expression"));
    if (sourceFile->module->kind == ModuleKind::Config)
    {
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->includes.push_back(node);
        SWAG_CHECK(sourceFile->module->addInclude(node));
    }

    return true;
}

bool SyntaxJob::doCompilerImport(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isGenerated || sourceFile->isCfgFile || sourceFile->isScriptFile, Report::report({sourceFile, token, Err(Syn0009)}));
    SWAG_VERIFY(currentScope->isTopLevel(), Report::report({sourceFile, token, Err(Syn0008)}));

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
        SWAG_VERIFY(scan, Report::report({sourceFile, token, Err(Syn0010)}));
    }

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerImport, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::LiteralString, Report::report({sourceFile, token, Err(Syn0007)}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());

    // Specific dependency stuff
    Token tokenLocation, tokenVersion;
    if (sourceFile->isCfgFile || sourceFile->isScriptFile)
    {
        while (true)
        {
            if (token.text == g_LangSpec->name_location)
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_VERIFY(tokenLocation.text.empty(), error(token, Err(Syn0116)));
                SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, Fmt(Err(Syn0109), token.ctext())));
                tokenLocation = token;
                SWAG_CHECK(eatToken());
                continue;
            }

            if (token.text == g_LangSpec->name_version)
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_VERIFY(tokenVersion.text.empty(), error(token, Err(Syn0117)));
                SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, Fmt(Err(Syn0113), token.ctext())));
                tokenVersion = token;
                SWAG_CHECK(eatToken());
                continue;
            }

            break;
        }
    }

    SWAG_CHECK(eatSemiCol("`#import` expression"));
    if (sourceFile->isGenerated || sourceFile->module->kind == ModuleKind::Config)
    {
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->imports.push_back(node);
        SWAG_CHECK(sourceFile->module->addDependency(node, tokenLocation, tokenVersion));
    }

    return true;
}

bool SyntaxJob::doCompilerPlaceHolder(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), Report::report({sourceFile, token, Err(Syn0035)}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPlaceHolder, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::Identifier, Report::report({sourceFile, token, Err(Syn0017)}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatSemiCol("`#placeholder` expression"));

    currentScope->symTable.registerSymbolName(&context, node, SymbolKind::PlaceHolder);

    return true;
}

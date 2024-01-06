#include "pch.h"
#include "Workspace.h"
#include "Semantic.h"
#include "Ast.h"
#include "Scoped.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"

bool Parser::doIntrinsicTag(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::IntrinsicProp, sourceFile, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveIntrinsicTag;

    SWAG_CHECK(eatToken());

    auto startLoc = token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, Err(Err1175)));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));

    if (node->tokenId == TokenId::IntrinsicGetTag)
    {
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the second argument"));
        SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the third argument"));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doCompilerIf(AstNode* parent, AstNode** result)
{
    return doCompilerIfFor(parent, result, AstNodeKind::Statement);
}

bool Parser::doCompilerIfFor(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    auto node = Ast::newNode<AstIf>(this, AstNodeKind::CompilerIf, sourceFile, parent);
    *result   = node;

    // Expression
    {
        ScopedFlags scopedFlags(this, AST_NO_BACKEND);
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, Fmt(Err(Err1083), token.ctext())));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        node->allocateExtension(ExtensionKind::Semantic);
        node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
        node->boolExpression->allocateExtension(ExtensionKind::Semantic);
        node->boolExpression->extSemantic()->semanticAfterFct = Semantic::resolveCompilerIf;
    }

    // Block
    {
        auto block    = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->ifBlock = block;
        if (node->hasExtOwner() && node->extOwner()->ownerCompilerIfBlock)
            node->extOwner()->ownerCompilerIfBlock->blocks.push_back(block);

        ScopedCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(doStatementFor(block, &dummyResult, kind));
    }

    // Else block
    if (token.id == TokenId::CompilerElse || token.id == TokenId::CompilerElseIf)
    {
        auto block      = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->elseBlock = block;
        if (node->hasExtOwner() && node->extOwner()->ownerCompilerIfBlock)
            node->extOwner()->ownerCompilerIfBlock->blocks.push_back(block);

        ScopedCompilerIfBlock scopedIf(this, block);
        if (token.id == TokenId::CompilerElseIf)
            SWAG_CHECK(doCompilerIf(block, &dummyResult));
        else
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doStatementFor(block, &dummyResult, kind));
        }
    }

    return true;
}

bool Parser::doCompilerMixin(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstCompilerMixin>(this, AstNodeKind::CompilerMixin, sourceFile, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveCompilerMixin;
    node->token       = token;

    SWAG_CHECK(eatToken());

    // Code identifier
    SWAG_CHECK(doIdentifierRef(node, &dummyResult));

    // Replacement parameters
    if (token.id == TokenId::SymLeftCurly && !(token.flags & TOKENPARSE_LAST_EOL))
    {
        auto startLoc = token.startLocation;
        SWAG_VERIFY(node->ownerBreakable, error(token, Err(Err1034)));
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id != TokenId::SymRightCurly, error(token, Err(Err1015)));
        if (token.id != TokenId::KwdBreak && token.id != TokenId::KwdContinue)
            return error(token, Fmt(Err(Err1016), token.ctext()));

        node->allocateExtension(ExtensionKind::Owner);

        AstNode* stmt;
        while (token.id != TokenId::SymRightCurly)
        {
            auto tokenId = token.id;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify another '#mixin' block argument"));
            SWAG_CHECK(doEmbeddedInstruction(nullptr, &stmt));
            node->replaceTokens[tokenId] = stmt;
            node->extOwner()->nodesToFree.push_back(stmt);
            if (token.id != TokenId::SymRightCurly)
                SWAG_CHECK(eatSemiCol("'#mixin' replacement statement"));
        }

        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the '#mixin' replacement statement"));
    }

    SWAG_CHECK(eatSemiCol("'#mixin' expression"));
    return true;
}

bool Parser::doCompilerMacro(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerMacro>(this, AstNodeKind::CompilerMacro, sourceFile, parent);
    *result   = node;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveCompilerMacro;

    SWAG_CHECK(eatToken());
    auto newScope = Ast::newScope(node, "", ScopeKind::Macro, node->ownerScope);
    node->scope   = newScope;

    Scoped scoped(this, newScope);
    SWAG_CHECK(doCurlyStatement(node, &dummyResult));
    return true;
}

bool Parser::doCompilerAssert(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerAssert, sourceFile, parent);
    *result   = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerAssert;
    node->token                            = token;

    ScopedFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("'#assert' expression"));
    return true;
}

bool Parser::doCompilerError(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerError, sourceFile, parent);
    *result   = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerError;
    node->token                            = token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("'#error' expression"));
    return true;
}

bool Parser::doCompilerWarning(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerWarning, sourceFile, parent);
    *result   = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerWarning;
    node->token                            = token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("'#warning' expression"));
    return true;
}

bool Parser::doCompilerValidIf(AstNode* parent, AstNode** result)
{
    auto node    = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerValidIf, sourceFile, parent);
    *result      = node;
    auto tokenId = token.id;
    if (tokenId == TokenId::CompilerValidIfx)
        node->kind = AstNodeKind::CompilerValidIfx;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerValidIfExpression;
    SWAG_CHECK(eatToken());
    parent->flags |= AST_HAS_SELECT_IF;
    node->flags |= AST_NO_BYTECODE_CHILDS;

    // Not for the 3 special special functions
    if (parent->token.text == g_LangSpec->name_opDrop ||
        parent->token.text == g_LangSpec->name_opPostCopy ||
        parent->token.text == g_LangSpec->name_opPostMove)
    {
        return error(node, Fmt(Err(Err1137), parent->token.ctext()));
    }

    ScopedFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND | AST_IN_VALIDIF);
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
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatSemiCol("'#validifx' expression"));
    }

    return true;
}

bool Parser::doCompilerAst(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerAst, sourceFile, parent);
    *result   = node;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerAstExpression;
    SWAG_CHECK(eatToken());

    ScopedFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND);
    if (token.id == TokenId::SymLeftCurly)
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerAst));
        funcNode->inheritTokenLocation(node);

        auto idRef = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
        idRef->inheritTokenLocation(node);
        auto identifier            = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
        identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier, this);
        identifier->inheritTokenLocation(node);
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatSemiCol("'#ast' expression"));
    }

    return true;
}

bool Parser::doCompilerRunTopLevel(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doFuncDecl(parent, result, TokenId::CompilerRun));
        return true;
    }

    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerRun, sourceFile, parent);
    *result   = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->semanticFct = Semantic::resolveCompilerRun;
    SWAG_CHECK(doEmbeddedInstruction(node, &dummyResult));
    SWAG_CHECK(eatSemiCol("'#run' statement"));
    return true;
}

bool Parser::doCompilerRunEmbedded(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerRun, sourceFile, parent);
    *result   = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerRun;
    node->token                            = token;
    SWAG_CHECK(eatToken());

    ScopedFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND);
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
        SWAG_CHECK(doEmbeddedInstruction(node, &dummyResult));
        SWAG_CHECK(eatSemiCol("'#run' expression"));
    }

    return true;
}

bool Parser::doCompilerPrint(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPrint, sourceFile, parent);
    *result   = node;
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerPrint;
    node->token                            = token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("'#print' expression"));
    return true;
}

bool Parser::doCompilerForeignLib(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::CompilerForeignLib, sourceFile, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveCompilerForeignLib;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, Fmt(Err(Err1002), token.ctext())));

    AstNode* literal;
    SWAG_CHECK(doLiteral(node, &literal));
    SWAG_CHECK(eatSemiCol("'#foreignlib'"));
    return true;
}

bool Parser::doCompilerGlobal(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(!afterGlobal, error(token, Err(Err1006)));
    SWAG_CHECK(eatToken());

    /////////////////////////////////
    if (token.text == g_LangSpec->name_export)
    {
        if (!sourceFile->imported)
        {
            SWAG_VERIFY(!sourceFile->forceExport, error(token, Err(Err1001)));
            sourceFile->forceExport = true;
            sourceFile->module->addExportSourceFile(sourceFile);
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global export'"));
    }

    /////////////////////////////////
    else if (token.text == g_LangSpec->name_generated)
    {
        sourceFile->isGenerated = true;
        if (sourceFile->imported)
            sourceFile->imported->isSwag = true;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global generated'"));
    }

    /////////////////////////////////
    else if (token.id == TokenId::CompilerIf)
    {
        auto node = Ast::newNode<AstIf>(this, AstNodeKind::CompilerIf, sourceFile, parent);
        *result   = node;
        node->flags |= AST_GLOBAL_NODE;

        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        node->boolExpression->allocateExtension(ExtensionKind::Semantic);
        node->boolExpression->extSemantic()->semanticAfterFct = Semantic::resolveCompilerIf;

        auto block    = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->ifBlock = block;
        if (node->hasExtOwner() && node->extOwner()->ownerCompilerIfBlock)
            node->extOwner()->ownerCompilerIfBlock->blocks.push_back(block);
        block->flags |= AST_GLOBAL_NODE;

        ScopedCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(eatSemiCol("'#global if'"));
        while (token.id != TokenId::EndOfFile)
        {
            SWAG_CHECK(doTopLevelInstruction(block, &dummyResult));
        }
    }

    /////////////////////////////////
    else if (token.id == TokenId::KwdNamespace)
    {
        SWAG_CHECK(doNamespace(parent, result, true, false));
    }

    /////////////////////////////////
    else if (token.id == TokenId::KwdPublic || token.id == TokenId::KwdInternal)
    {
        SWAG_CHECK(doPublicInternal(parent, result, true));
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
            context->report({sourceFile, token, Fmt(Err(Err1167), token.ctext())});
            return false;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("'#global testpass'"));
    }

    /////////////////////////////////
    else if (token.text == g_LangSpec->name_testerror || token.text == g_LangSpec->name_testwarning)
    {
        // Put the file in its own module, because of errors/warnings
        if (!sourceFile->module->isErrorModule)
        {
            auto newModule = g_Workspace->createOrUseModule(sourceFile->name, sourceFile->module->path, sourceFile->module->kind, true);
            auto oldModule = sourceFile->module;
            oldModule->removeFile(sourceFile);
            newModule->addFile(sourceFile);

            // Kick the link to the module node
            sourceFile->astRoot->parent = nullptr;

            for (auto dep : oldModule->moduleDependencies)
            {
                ModuleDependency* newDep = Allocator::alloc<ModuleDependency>();
                *newDep                  = *dep;
                newDep->importDone       = false;
                newModule->moduleDependencies.push_back(newDep);
            }

            oldModule->addErrorModule(newModule);
            module = newModule;
        }

        if (token.text == g_LangSpec->name_testerror)
        {
            SWAG_VERIFY(sourceFile->module->kind == ModuleKind::Test, context->report({sourceFile, token, Err(Err1003)}));
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::LiteralString, context->report({sourceFile, token, Fmt(Err(Err1204), token.ctext())}));
            sourceFile->tokenHasError   = token;
            sourceFile->shouldHaveError = true;
            module->shouldHaveError     = true;
            sourceFile->shouldHaveErrorString.push_back(token.text);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatSemiCol("'#global testerror'"));
        }
        else
        {
            SWAG_VERIFY(sourceFile->module->kind == ModuleKind::Test, context->report({sourceFile, token, Err(Err1004)}));
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::LiteralString, context->report({sourceFile, token, Fmt(Err(Err1205), token.ctext())}));
            sourceFile->tokenHasWarning   = token;
            sourceFile->shouldHaveWarning = true;
            module->shouldHaveWarning     = true;
            sourceFile->shouldHaveWarningString.push_back(token.text);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatSemiCol("'#global testwarning'"));
        }
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
        attrUse->addSpecFlags(AstAttrUse::SPECFLAG_GLOBAL);
        attrUse->allocateExtension(ExtensionKind::Owner);
        attrUse->extOwner()->ownerAttrUse = sourceFile->astAttrUse;
        attrUse->flags |= AST_GLOBAL_NODE;
        sourceFile->astAttrUse = attrUse;
        SWAG_CHECK(eatSemiCol("'#global attribute'"));
    }

    /////////////////////////////////
    else if (token.id == TokenId::KwdUsing)
    {
        SWAG_VERIFY(sourceFile->isCfgFile, context->report({sourceFile, token, Err(Err1005)}));

        auto prevCount = parent->childs.count;
        SWAG_CHECK(doUsing(parent, &dummyResult));
        while (prevCount != parent->childs.count)
        {
            sourceFile->module->buildParameters.globalUsings.push_back(parent->childs[prevCount]);
            prevCount++;
        }
    }

    /////////////////////////////////
    else
    {
        return context->report({sourceFile, token, Fmt(Err(Err1136), token.ctext())});
    }

    return true;
}

bool Parser::doCompilerSpecialValue(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::CompilerSpecialValue, sourceFile, parent);
    *result       = exprNode;
    SWAG_CHECK(eatToken());
    exprNode->semanticFct = Semantic::resolveCompilerSpecialValue;
    return true;
}

bool Parser::doIntrinsicLocation(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::IntrinsicLocation, sourceFile, parent);
    *result       = exprNode;
    exprNode->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(eatToken());

    auto startLoc = token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, Err(Err1175)));

    ScopedFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doIdentifierRef(exprNode, &dummyResult, IDENTIFIER_NO_PARAMS));

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    exprNode->semanticFct = Semantic::resolveIntrinsicLocation;
    return true;
}

bool Parser::doIntrinsicDefined(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::IntrinsicDefined, sourceFile, parent);
    *result       = exprNode;
    exprNode->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(eatToken());

    auto startLoc = token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, Err(Err1175)));

    ScopedFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doIdentifierRef(exprNode, &dummyResult, IDENTIFIER_NO_PARAMS));

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));

    exprNode->semanticFct = Semantic::resolveIntrinsicDefined;
    return true;
}

bool Parser::doCompilerDependencies(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isCfgFile || sourceFile->isScriptFile, context->report({sourceFile, token, Err(Err1183)}));
    SWAG_VERIFY(parent->kind == AstNodeKind::File, context->report({sourceFile, token, Err(Err1182)}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerDependencies, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doCurlyStatement(node, &dummyResult));

    if (sourceFile->module->kind != ModuleKind::Config)
    {
        node->flags |= AST_NO_SEMANTIC;
        node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    }

    return true;
}

bool Parser::doCompilerInclude(AstNode* parent, AstNode** result)
{
    auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::CompilerInclude, sourceFile, parent);
    *result       = exprNode;
    exprNode->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(eatToken());

    ScopedFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doExpression(exprNode, EXPR_FLAG_NONE, &dummyResult));

    exprNode->semanticFct = Semantic::resolveCompilerInclude;
    return true;
}

bool Parser::doCompilerLoad(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isCfgFile || sourceFile->isScriptFile, context->report({sourceFile, token, Err(Err1013)}));

    // Be sure this is in a '#dependencies' block
    auto scan = parent;
    while (scan)
    {
        if (scan->kind == AstNodeKind::CompilerDependencies)
            break;
        scan = scan->parent;
    }
    SWAG_VERIFY(scan, context->report({sourceFile, token, Err(Err1014)}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerLoad, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::LiteralString, context->report({sourceFile, token, Fmt(Err(Err1011), token.ctext())}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatSemiCol("'#load' expression"));
    if (sourceFile->module->kind == ModuleKind::Config)
    {
        if (node->hasExtOwner() && node->extOwner()->ownerCompilerIfBlock)
            node->extOwner()->ownerCompilerIfBlock->includes.push_back(node);
        SWAG_CHECK(sourceFile->module->addFileToLoad(node));
    }

    return true;
}

bool Parser::doCompilerImport(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->isGenerated || sourceFile->isCfgFile || sourceFile->isScriptFile, context->report({sourceFile, token, Err(Err1009)}));

    // Be sure this is in a '#dependencies' block
    if (!sourceFile->isGenerated)
    {
        auto scan = parent;
        while (scan)
        {
            if (scan->kind == AstNodeKind::CompilerDependencies)
                break;
            scan = scan->parent;
        }
        SWAG_VERIFY(scan, context->report({sourceFile, token, Err(Err1010)}));
    }

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerImport, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::LiteralString, context->report({sourceFile, token, Fmt(Err(Err1007), token.ctext())}));
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
                SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the location"));
                SWAG_VERIFY(tokenLocation.text.empty(), error(token, Err(Err1116)));
                SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, Fmt(Err(Err1109), token.ctext())));
                tokenLocation = token;
                SWAG_CHECK(eatToken());
                continue;
            }

            if (token.text == g_LangSpec->name_version)
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the version"));
                SWAG_VERIFY(tokenVersion.text.empty(), error(token, Err(Err1117)));
                SWAG_VERIFY(token.id == TokenId::LiteralString, error(token, Fmt(Err(Err1113), token.ctext())));
                tokenVersion = token;
                SWAG_CHECK(eatToken());
                continue;
            }

            break;
        }
    }

    SWAG_CHECK(eatSemiCol("'#import' expression"));
    if (sourceFile->isGenerated || sourceFile->module->kind == ModuleKind::Config)
    {
        if (node->hasExtOwner() && node->extOwner()->ownerCompilerIfBlock)
            node->extOwner()->ownerCompilerIfBlock->imports.push_back(node);
        SWAG_CHECK(sourceFile->module->addDependency(node, tokenLocation, tokenVersion));
    }

    return true;
}

bool Parser::doCompilerPlaceHolder(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), context->report({sourceFile, token, Err(Err1035)}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPlaceHolder, sourceFile, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id == TokenId::Identifier, context->report({sourceFile, token, Fmt(Err(Err1017), token.ctext())}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatSemiCol("'#placeholder' expression"));

    currentScope->symTable.registerSymbolName(context, node, SymbolKind::PlaceHolder);

    return true;
}

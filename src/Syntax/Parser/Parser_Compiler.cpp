#include "pch.h"

#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

bool Parser::doIntrinsicTag(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstNode>(AstNodeKind::IntrinsicProp, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveIntrinsicTag;

    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, toErr(Err0528)));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));

    if (node->token.is(TokenId::IntrinsicGetTag))
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
    const auto node = Ast::newNode<AstIf>(AstNodeKind::CompilerIf, this, parent);
    *result         = node;

    // Expression
    {
        ParserPushAstNodeFlags scopedFlags(this, AST_NO_BACKEND);
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse.token, formErr(Err0520, tokenParse.token.c_str())));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        node->allocateExtension(ExtensionKind::Semantic);
        node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
        node->boolExpression->allocateExtension(ExtensionKind::Semantic);
        node->boolExpression->extSemantic()->semanticAfterFct = Semantic::resolveCompilerIf;
    }

    // Block
    {
        const auto block = Ast::newNode<AstCompilerIfBlock>(AstNodeKind::CompilerIfBlock, this, node);
        node->ifBlock    = block;
        if (node->hasOwnerCompilerIfBlock())
            node->ownerCompilerIfBlock()->blocks.push_back(block);

        ParserPushCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(doCompilerIfStatementFor(block, &dummyResult, kind, true));
    }

    // Else block
    if (tokenParse.is(TokenId::CompilerElse) || tokenParse.is(TokenId::CompilerElseIf))
    {
        const auto block = Ast::newNode<AstCompilerIfBlock>(AstNodeKind::CompilerIfBlock, this, node);
        node->elseBlock  = block;
        if (node->hasOwnerCompilerIfBlock())
            node->ownerCompilerIfBlock()->blocks.push_back(block);

        ParserPushCompilerIfBlock scopedIf(this, block);
        if (tokenParse.is(TokenId::CompilerElseIf))
            SWAG_CHECK(doCompilerIf(block, &dummyResult));
        else
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doCompilerIfStatementFor(block, &dummyResult, kind, false));
        }
    }

    return true;
}

bool Parser::doCompilerIfStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind, bool forIf)
{
    if (tokenParse.is(TokenId::CompilerDo))
    {
        SWAG_CHECK(eatToken());
        if (tokenParse.is(TokenId::SymLeftCurly))
        {
            const Diagnostic err{sourceFile, tokenParse.token, formErr(Err0434, tokenParse.token.c_str())};
            return context->report(err);
        }
    }
    else if (tokenParse.isNot(TokenId::SymLeftCurly))
    {
        Diagnostic err{sourceFile, tokenParse.token, formErr(Err0516, tokenParse.token.c_str())};
        if (forIf)
            parent = parent->parent;
        err.addNote(parent, parent->token, formNte(Nte0015, parent->token.c_str()));
        return context->report(err);
    }

    switch (kind)
    {
        case AstNodeKind::Statement:
            return doCompilerIfStatement(parent, result);
        case AstNodeKind::EnumDecl:
            return doEnumContent(parent, &dummyResult);
        case AstNodeKind::StructDecl:
            return doStructBody(parent, SyntaxStructType::Struct, &dummyResult);
        case AstNodeKind::InterfaceDecl:
            return doStructBody(parent, SyntaxStructType::Interface, &dummyResult);

        default:
            SWAG_ASSERT(false);
            break;
    }

    return true;
}

bool Parser::doCompilerIfStatement(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse.token, toErr(Err0264), toNte(Nte0054)));

    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        SWAG_CHECK(doCurlyStatement(parent, result));
    }
    else if (currentScope->isGlobalOrImpl())
    {
        *result = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
        SWAG_CHECK(doTopLevelInstruction(*result, &dummyResult));
    }
    else
    {
        SWAG_CHECK(doEmbeddedInstruction(parent, result));
    }

    return true;
}

bool Parser::doCompilerMixin(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstCompilerMixin>(AstNodeKind::CompilerMixin, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveCompilerMixin;
    node->token       = tokenParse.token;

    SWAG_CHECK(eatToken());

    // Code identifier
    SWAG_CHECK(doIdentifierRef(node, &dummyResult));

    // Replacement parameters
    if (tokenParse.is(TokenId::SymLeftCurly) && !tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        const auto startLoc = tokenParse.token.startLocation;
        SWAG_VERIFY(node->hasOwnerBreakable(), error(tokenParse.token, toErr(Err0445)));
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightCurly), error(tokenParse.token, toErr(Err0072)));
        if (tokenParse.isNot(TokenId::KwdBreak) && tokenParse.isNot(TokenId::KwdContinue))
            return error(tokenParse.token, formErr(Err0143, tokenParse.token.c_str()));

        node->allocateExtension(ExtensionKind::Owner);

        AstNode* stmt;
        while (tokenParse.isNot(TokenId::SymRightCurly))
        {
            auto tokenId = tokenParse.token.id;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify another [[#mixin]] block argument"));
            SWAG_CHECK(doEmbeddedInstruction(nullptr, &stmt));
            node->replaceTokens[tokenId] = stmt;
            node->extOwner()->nodesToFree.push_back(stmt);
            if (tokenParse.isNot(TokenId::SymRightCurly))
                SWAG_CHECK(eatSemiCol("[[#mixin]] replacement statement"));
        }

        SWAG_CHECK(eatFormat(node));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[#mixin]] replacement statement"));
    }

    SWAG_CHECK(eatSemiCol("[[#mixin]] expression"));
    return true;
}

bool Parser::doCompilerMacro(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstCompilerMacro>(AstNodeKind::CompilerMacro, this, parent);
    *result         = node;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveCompilerMacro;

    SWAG_CHECK(eatToken());
    const auto newScope = Ast::newScope(node, "", ScopeKind::Macro, node->ownerScope);
    node->scope         = newScope;

    ParserPushScope scoped(this, newScope);
    SWAG_CHECK(doCurlyStatement(node, &dummyResult));
    return true;
}

bool Parser::doCompilerAssert(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerAssert, this, parent);
    *result         = node;
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerAssert;
    node->token                            = tokenParse.token;

    ParserPushAstNodeFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("[[#assert]] expression"));
    return true;
}

bool Parser::doCompilerError(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerError, this, parent);
    *result         = node;
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerError;
    node->token                            = tokenParse.token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("[[#error]] expression"));
    return true;
}

bool Parser::doCompilerWarning(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerWarning, this, parent);
    *result         = node;
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerWarning;
    node->token                            = tokenParse.token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("[[#warning]] expression"));
    return true;
}

bool Parser::doCompilerWhere(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstCompilerSpecFunc>(AstNodeKind::CompilerWhere, this, parent);
    *result         = node;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerWhereExpression;
    parent->addAstFlag(AST_HAS_SELECT_IF);
    node->addAstFlag(AST_NO_BYTECODE_CHILDREN);

    SWAG_CHECK(eatToken());

    // Each mode
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->token.id, mdfFlags, node));
    if (mdfFlags.has(MODIFIER_EACH))
        node->kind = AstNodeKind::CompilerWhereEach;

    // Not for the 3 special functions
    if (parent->token.text == g_LangSpec->name_opDrop ||
        parent->token.text == g_LangSpec->name_opPostCopy ||
        parent->token.text == g_LangSpec->name_opPostMove)
    {
        return error(node, formErr(Err0658, parent->token.c_str()));
    }

    ParserPushAstNodeFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND | AST_IN_WHERE);
    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerWhere));

        const auto idRef           = Ast::newIdentifierRef(funcNode->token.text, this, node);
        const auto identifier      = castAst<AstIdentifier>(idRef->lastChild(), AstNodeKind::Identifier);
        identifier->callParameters = Ast::newFuncCallParams(this, identifier);
        idRef->inheritTokenLocation(node->token);
        identifier->inheritTokenLocation(node->token);
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatSemiCol("[[check]] expression"));
    }

    return true;
}

bool Parser::doCompilerAst(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstCompilerSpecFunc>(AstNodeKind::CompilerAst, this, parent);
    *result         = node;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerAstExpression;
    SWAG_CHECK(eatToken());

    ParserPushAstNodeFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND);
    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerAst));
        funcNode->inheritTokenLocation(node->token);

        ParserPushFreezeFormat ff{this};
        const auto             idRef      = Ast::newIdentifierRef(funcNode->token.text, this, node);
        const auto             identifier = castAst<AstIdentifier>(idRef->lastChild(), AstNodeKind::Identifier);
        identifier->callParameters        = Ast::newFuncCallParams(this, identifier);
        idRef->inheritTokenLocation(node->token);
        identifier->inheritTokenLocation(node->token);
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatSemiCol("[[#ast]] expression"));
    }

    return true;
}

bool Parser::doCompilerRunTopLevel(AstNode* parent, AstNode** result)
{
    auto savedToken = tokenParse;
    SWAG_CHECK(eatToken());

    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        AstNode* funcDecl;
        SWAG_CHECK(doFuncDecl(parent, &funcDecl, TokenId::CompilerRun));
        *result = funcDecl;
        FormatAst::inheritFormatBefore(this, funcDecl, &savedToken);
        return true;
    }

    const auto node = Ast::newNode<AstCompilerSpecFunc>(AstNodeKind::CompilerRun, this, parent);
    *result         = node;
    FormatAst::inheritFormatBefore(this, node, &savedToken);

    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    node->semanticFct = Semantic::resolveCompilerRun;
    SWAG_CHECK(doEmbeddedInstruction(node, &dummyResult));
    SWAG_CHECK(eatSemiCol("[[#run]] statement"));
    return true;
}

bool Parser::doCompilerRunEmbedded(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstCompilerSpecFunc>(AstNodeKind::CompilerRun, this, parent);
    *result         = node;
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerRun;
    node->token                            = tokenParse.token;
    SWAG_CHECK(eatToken());

    ParserPushAstNodeFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND);
    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerGeneratedRun));

        const auto idRef           = Ast::newIdentifierRef(funcNode->token.text, this, node);
        const auto identifier      = castAst<AstIdentifier>(idRef->lastChild(), AstNodeKind::Identifier);
        identifier->callParameters = Ast::newFuncCallParams(this, identifier);
        idRef->inheritTokenLocation(node->token);
        identifier->inheritTokenLocation(node->token);
    }
    else
    {
        SWAG_CHECK(doEmbeddedInstruction(node, &dummyResult));
        SWAG_CHECK(eatSemiCol("[[#run]] expression"));
    }

    return true;
}

bool Parser::doCompilerPrint(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerPrint, this, parent);
    *result         = node;
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveCompilerPrint;
    node->token                            = tokenParse.token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatSemiCol("[[#print]] expression"));
    return true;
}

bool Parser::doCompilerForeignLib(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstNode>(AstNodeKind::CompilerForeignLib, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveCompilerForeignLib;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.is(TokenId::LiteralString), error(tokenParse.token, formErr(Err0517, tokenParse.token.c_str())));

    AstNode* literal;
    SWAG_CHECK(doLiteral(node, &literal));
    SWAG_CHECK(eatSemiCol("[[#foreignlib]]"));
    return true;
}

bool Parser::doCompilerGlobal(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(!afterGlobal, error(tokenParse.token, toErr(Err0438)));
    auto savedToken = tokenParse;
    SWAG_CHECK(eatToken());

    /////////////////////////////////
    if (tokenParse.token.text == g_LangSpec->name_export)
    {
        const auto globalDecl = Ast::newNode<AstCompilerGlobal>(AstNodeKind::CompilerGlobal, this, parent);
        FormatAst::inheritFormatBefore(this, globalDecl, &savedToken);
        const auto idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
        idRef->addAstFlag(AST_NO_SEMANTIC);

        if (!sourceFile->imported)
        {
            SWAG_VERIFY(!sourceFile->hasFlag(FILE_FORCE_EXPORT), error(tokenParse.token, toErr(Err0004)));
            sourceFile->addFlag(FILE_FORCE_EXPORT);
            sourceFile->module->addExportSourceFile(sourceFile);
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("[[#global export]]"));
    }

    /////////////////////////////////
    else if (tokenParse.token.text == g_LangSpec->name_generated)
    {
        const auto globalDecl = Ast::newNode<AstCompilerGlobal>(AstNodeKind::CompilerGlobal, this, parent);
        FormatAst::inheritFormatBefore(this, globalDecl, &savedToken);
        const auto idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
        idRef->addAstFlag(AST_NO_SEMANTIC);

        sourceFile->addFlag(FILE_GENERATED);
        if (sourceFile->imported)
            sourceFile->imported->isSwag = true;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("[[#global generated]]"));
    }

    /////////////////////////////////
    else if (tokenParse.is(TokenId::CompilerIf))
    {
        const auto node = Ast::newNode<AstIf>(AstNodeKind::CompilerIf, this, parent);
        *result         = node;
        node->addAstFlag(AST_GLOBAL_NODE);
        FormatAst::inheritFormatBefore(this, node, &savedToken);

        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        node->boolExpression->allocateExtension(ExtensionKind::Semantic);
        node->boolExpression->extSemantic()->semanticAfterFct = Semantic::resolveCompilerIf;

        const auto block = Ast::newNode<AstCompilerIfBlock>(AstNodeKind::CompilerIfBlock, this, node);
        node->ifBlock    = block;
        if (node->hasOwnerCompilerIfBlock())
            node->ownerCompilerIfBlock()->blocks.push_back(block);
        block->addAstFlag(AST_GLOBAL_NODE);

        ParserPushCompilerIfBlock scopedIf(this, block);

        const auto stmt = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, block);
        SWAG_CHECK(eatSemiCol("[[#global if]]"));
        while (tokenParse.isNot(TokenId::EndOfFile))
            SWAG_CHECK(doTopLevelInstruction(stmt, &dummyResult));
    }

    /////////////////////////////////
    else if (tokenParse.is(TokenId::KwdNamespace))
    {
        SWAG_CHECK(doNamespace(parent, result, true, false));
        FormatAst::inheritFormatBefore(this, *result, &savedToken);
    }

    /////////////////////////////////
    else if (tokenParse.is(TokenId::KwdPublic) || tokenParse.is(TokenId::KwdInternal))
    {
        if (parserFlags.has(PARSER_TRACK_FORMAT))
        {
            const auto globalDecl = Ast::newNode<AstCompilerGlobal>(AstNodeKind::CompilerGlobal, this, parent);
            FormatAst::inheritFormatBefore(this, globalDecl, &savedToken);
            (void) Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatSemiCol("[[#global public/internal]]"));
        }
        else
        {
            SWAG_CHECK(doPublicInternal(parent, result, true));
        }
    }

    /////////////////////////////////
    else if (tokenParse.token.text == g_LangSpec->name_skip)
    {
        const auto globalDecl = Ast::newNode<AstCompilerGlobal>(AstNodeKind::CompilerGlobal, this, parent);
        FormatAst::inheritFormatBefore(this, globalDecl, &savedToken);
        const auto idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
        idRef->addAstFlag(AST_NO_SEMANTIC);
        sourceFile->addFlag(FILE_NO_FORMAT);
        sourceFile->buildPass = BuildPass::Lexer;
    }

    /////////////////////////////////
    else if (tokenParse.token.text == g_LangSpec->name_skipfmt)
    {
        sourceFile->addFlag(FILE_NO_FORMAT);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("[[#global noformat]]"));
    }

    /////////////////////////////////
    else if (tokenParse.token.text == g_LangSpec->name_testpass)
    {
        const auto globalDecl = Ast::newNode<AstCompilerGlobal>(AstNodeKind::CompilerGlobal, this, parent);
        FormatAst::inheritFormatBefore(this, globalDecl, &savedToken);
        auto idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
        idRef->addAstFlag(AST_NO_SEMANTIC);

        SWAG_CHECK(eatToken());
        if (tokenParse.token.text == g_LangSpec->name_lexer)
        {
            idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
            idRef->addAstFlag(AST_NO_SEMANTIC);
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Lexer;
        }
        else if (tokenParse.token.text == g_LangSpec->name_syntax)
        {
            idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
            idRef->addAstFlag(AST_NO_SEMANTIC);
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Syntax;
        }
        else if (tokenParse.token.text == g_LangSpec->name_semantic)
        {
            idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
            idRef->addAstFlag(AST_NO_SEMANTIC);
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Semantic;
        }
        else if (tokenParse.token.text == g_LangSpec->name_backend)
        {
            idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
            idRef->addAstFlag(AST_NO_SEMANTIC);
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Backend;
        }
        else
        {
            context->report({sourceFile, tokenParse.token, formErr(Err0357, tokenParse.token.c_str())});
            return false;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("[[#global testpass]]"));
    }

    /////////////////////////////////
    else if (tokenParse.token.text == g_LangSpec->name_testerror || tokenParse.token.text == g_LangSpec->name_testwarning)
    {
        // Put the file in its own module, because of errors/warnings
        if (!sourceFile->module->isErrorModule && !parserFlags.has(PARSER_TRACK_FORMAT))
        {
            const auto newModule = g_Workspace->createModule(sourceFile->name, sourceFile->module->path, sourceFile->module->kind, true);
            const auto oldModule = sourceFile->module;
            oldModule->removeFile(sourceFile);
            newModule->addFile(sourceFile);

            // Kick the link to the module node
            sourceFile->astRoot->parent = nullptr;

            for (const auto dep : oldModule->moduleDependencies)
            {
                ModuleDependency* newDep = Allocator::alloc<ModuleDependency>();
                *newDep                  = *dep;
                newDep->importDone       = false;
                newModule->moduleDependencies.push_back(newDep);
            }

            oldModule->addErrorModule(newModule);
            module = newModule;
        }

        const auto globalDecl = Ast::newNode<AstCompilerGlobal>(AstNodeKind::CompilerGlobal, this, parent);
        FormatAst::inheritFormatBefore(this, globalDecl, &savedToken);
        const auto idRef = Ast::newIdentifierRef(tokenParse.token.text, this, globalDecl);
        idRef->addAstFlag(AST_NO_SEMANTIC);

        if (tokenParse.token.text == g_LangSpec->name_testerror)
        {
            SWAG_VERIFY(sourceFile->module->is(ModuleKind::Test) || sourceFile->hasFlag(FILE_FOR_FORMAT), context->report({sourceFile, tokenParse.token, toErr(Err0435)}));
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.is(TokenId::LiteralString), context->report({sourceFile, tokenParse.token, formErr(Err0518, tokenParse.token.c_str())}));
            sourceFile->tokenHasError = tokenParse.token;
            sourceFile->addFlag(FILE_SHOULD_HAVE_ERROR);
            module->shouldHaveError = true;
            sourceFile->shouldHaveErrorString.push_back(tokenParse.token.text);
            const auto literal   = Ast::newNode<AstLiteral>(AstNodeKind::Literal, this, globalDecl);
            literal->literalType = LiteralType::TypeString;
            literal->addAstFlag(AST_NO_SEMANTIC);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatSemiCol("[[#global testerror]]"));
        }
        else
        {
            SWAG_VERIFY(sourceFile->module->is(ModuleKind::Test) || sourceFile->hasFlag(FILE_FOR_FORMAT), context->report({sourceFile, tokenParse.token, toErr(Err0436)}));
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.is(TokenId::LiteralString), context->report({sourceFile, tokenParse.token, formErr(Err0519, tokenParse.token.c_str())}));
            sourceFile->tokenHasWarning = tokenParse.token;
            sourceFile->addFlag(FILE_SHOULD_HAVE_WARNING);
            module->shouldHaveWarning = true;
            sourceFile->shouldHaveWarningString.push_back(tokenParse.token.text);
            const auto literal   = Ast::newNode<AstLiteral>(AstNodeKind::Literal, this, globalDecl);
            literal->literalType = LiteralType::TypeString;
            literal->addAstFlag(AST_NO_SEMANTIC);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatSemiCol("[[#global testwarning]]"));
        }
    }

    /////////////////////////////////
    else if (tokenParse.is(TokenId::SymAttrStart))
    {
        AstNode* resultNode;
        SWAG_CHECK(doAttrUse(parent, &resultNode, true));

        if (!parserFlags.has(PARSER_TRACK_FORMAT))
        {
            Ast::removeFromParent(resultNode);

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

                // Add front in case astRoot already has some children (if the #global comes after another one).
                // We need #global attributes to be first in the file, before other #globals (namespaces, public etc...).
                // Otherwise we can have a race condition between multiple globals.
                Ast::addChildFront(parent, resultNode);
            }
        }

        const auto attrUse = castAst<AstAttrUse>(resultNode);
        attrUse->addSpecFlag(AstAttrUse::SPEC_FLAG_GLOBAL);
        attrUse->allocateExtension(ExtensionKind::Owner);
        attrUse->extOwner()->ownerAttrUse = sourceFile->astAttrUse;
        attrUse->addAstFlag(AST_GLOBAL_NODE);
        sourceFile->astAttrUse = attrUse;
        SWAG_CHECK(eatSemiCol("[[#global attribute]]"));
    }

    /////////////////////////////////
    else if (tokenParse.is(TokenId::KwdUsing))
    {
        if (!sourceFile->hasFlag(FILE_CFG) &&
            !sourceFile->hasFlag(FILE_FOR_FORMAT))
        {
            const Diagnostic err{sourceFile, tokenParse.token, toErr(Err0437)};
            return context->report(err);
        }

        auto prevCount = parent->children.count;
        SWAG_CHECK(doUsing(parent, &dummyResult, true));
        while (prevCount != parent->children.count)
        {
            sourceFile->module->buildParameters.globalUsing.push_back(parent->children[prevCount]);
            prevCount++;
        }
    }

    /////////////////////////////////
    else
    {
        return context->report({sourceFile, tokenParse.token, formErr(Err0139, tokenParse.token.c_str())});
    }

    return true;
}

bool Parser::doCompilerSpecialValue(AstNode* parent, AstNode** result)
{
    const auto exprNode = Ast::newNode<AstNode>(AstNodeKind::CompilerSpecialValue, this, parent);
    *result             = exprNode;
    SWAG_CHECK(eatToken());
    exprNode->semanticFct = Semantic::resolveCompilerSpecialValue;
    return true;
}

bool Parser::doIntrinsicLocation(AstNode* parent, AstNode** result)
{
    const auto exprNode = Ast::newNode<AstNode>(AstNodeKind::IntrinsicLocation, this, parent);
    *result             = exprNode;
    exprNode->addAstFlag(AST_NO_BYTECODE);
    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, toErr(Err0528)));

    ParserPushAstNodeFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doIdentifierRef(exprNode, &dummyResult, IDENTIFIER_NO_PARAMS));

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    exprNode->semanticFct = Semantic::resolveIntrinsicLocation;
    return true;
}

bool Parser::doIntrinsicDefined(AstNode* parent, AstNode** result)
{
    const auto exprNode = Ast::newNode<AstNode>(AstNodeKind::IntrinsicDefined, this, parent);
    *result             = exprNode;
    exprNode->addAstFlag(AST_NO_BYTECODE);
    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, toErr(Err0528)));

    ParserPushAstNodeFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doIdentifierRef(exprNode, &dummyResult, IDENTIFIER_NO_PARAMS));

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));

    exprNode->semanticFct = Semantic::resolveIntrinsicDefined;
    return true;
}

bool Parser::doCompilerDependencies(AstNode* parent)
{
    if (!sourceFile->hasFlag(FILE_CFG) && !sourceFile->hasFlag(FILE_SCRIPT) && !sourceFile->hasFlag(FILE_FOR_FORMAT))
    {
        const Diagnostic err{sourceFile, tokenParse.token, toErr(Err0432)};
        return context->report(err);
    }

    SWAG_VERIFY(parent->is(AstNodeKind::File), context->report({sourceFile, tokenParse.token, toErr(Err0433)}));

    const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerDependencies, this, parent);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doCurlyStatement(node, &dummyResult));

    if (sourceFile->module->isNot(ModuleKind::Config))
    {
        node->addAstFlag(AST_NO_SEMANTIC);
        node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    }

    return true;
}

bool Parser::doCompilerInclude(AstNode* parent, AstNode** result)
{
    const auto exprNode = Ast::newNode<AstNode>(AstNodeKind::CompilerInclude, this, parent);
    *result             = exprNode;
    exprNode->addAstFlag(AST_NO_BYTECODE);
    SWAG_CHECK(eatToken());

    ParserPushAstNodeFlags sc(this, AST_SILENT_CHECK);
    SWAG_CHECK(doExpression(exprNode, EXPR_FLAG_NONE, &dummyResult));

    exprNode->semanticFct = Semantic::resolveCompilerInclude;
    return true;
}

bool Parser::doCompilerLoad(AstNode* parent)
{
    SWAG_VERIFY(sourceFile->hasFlag(FILE_CFG) || sourceFile->hasFlag(FILE_SCRIPT), context->report({sourceFile, tokenParse.token, toErr(Err0442)}));

    // Be sure this is in a '#dependencies' block
    auto scan = parent;
    while (scan)
    {
        if (scan->is(AstNodeKind::CompilerDependencies))
            break;
        scan = scan->parent;
    }
    SWAG_VERIFY(scan, context->report({sourceFile, tokenParse.token, toErr(Err0443)}));

    const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerLoad, this, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.is(TokenId::LiteralString), context->report({sourceFile, tokenParse.token, formErr(Err0522, tokenParse.token.c_str())}));
    node->inheritTokenName(tokenParse.token);
    node->inheritTokenLocation(tokenParse.token);
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatSemiCol("[[#load]] expression"));
    if (sourceFile->module->is(ModuleKind::Config))
    {
        if (node->hasOwnerCompilerIfBlock())
            node->ownerCompilerIfBlock()->includes.push_back(node);
        SWAG_CHECK(sourceFile->module->addFileToLoad(node));
    }

    return true;
}

bool Parser::doCompilerImport(AstNode* parent)
{
    if (!sourceFile->hasFlag(FILE_GENERATED) &&
        !sourceFile->hasFlag(FILE_CFG) &&
        !sourceFile->hasFlag(FILE_SCRIPT) &&
        !sourceFile->acceptsInternalStuff())
    {
        const Diagnostic err{sourceFile, tokenParse.token, toErr(Err0439)};
        return context->report(err);
    }

    // Be sure this is in a '#dependencies' block
    if (!sourceFile->hasFlag(FILE_GENERATED))
    {
        const auto scan = parent ? parent->findParentOrMe(AstNodeKind::CompilerDependencies) : nullptr;
        SWAG_VERIFY(scan, context->report({sourceFile, tokenParse.token, toErr(Err0440)}));
    }

    const auto node = Ast::newNode<AstCompilerImport>(AstNodeKind::CompilerImport, this, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.is(TokenId::LiteralString), context->report({sourceFile, tokenParse.token, formErr(Err0521, tokenParse.token.c_str())}));
    node->inheritTokenName(tokenParse.token);
    node->inheritTokenLocation(tokenParse.token);
    SWAG_CHECK(eatToken());

    // Specific dependency stuff
    if (sourceFile->hasFlag(FILE_CFG) ||
        sourceFile->hasFlag(FILE_SCRIPT) ||
        sourceFile->acceptsInternalStuff())
    {
        while (true)
        {
            if (tokenParse.token.text == g_LangSpec->name_location)
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the location"));
                SWAG_VERIFY(node->tokenLocation.text.empty(), error(tokenParse.token, toErr(Err0064)));
                SWAG_VERIFY(tokenParse.is(TokenId::LiteralString), error(tokenParse.token, formErr(Err0140, tokenParse.token.c_str())));
                node->tokenLocation = tokenParse.token;
                SWAG_CHECK(eatToken());
                continue;
            }

            if (tokenParse.token.text == g_LangSpec->name_version)
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the version"));
                SWAG_VERIFY(node->tokenVersion.text.empty(), error(tokenParse.token, toErr(Err0065)));
                SWAG_VERIFY(tokenParse.is(TokenId::LiteralString), error(tokenParse.token, formErr(Err0141, tokenParse.token.c_str())));
                node->tokenVersion = tokenParse.token;
                SWAG_CHECK(eatToken());
                continue;
            }

            break;
        }
    }

    SWAG_CHECK(eatSemiCol("[[#import]] expression"));
    if (sourceFile->hasFlag(FILE_GENERATED) || sourceFile->module->is(ModuleKind::Config))
    {
        if (node->hasOwnerCompilerIfBlock())
            node->ownerCompilerIfBlock()->imports.push_back(node);
        SWAG_CHECK(sourceFile->module->addDependency(node, node->tokenLocation, node->tokenVersion));
    }

    return true;
}

bool Parser::doCompilerPlaceHolder(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), context->report({sourceFile, tokenParse.token, toErr(Err0446)}));

    const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerPlaceHolder, this, parent);
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.is(TokenId::Identifier), context->report({sourceFile, tokenParse.token, formErr(Err0526, tokenParse.token.c_str())}));
    node->inheritTokenName(tokenParse.token);
    node->inheritTokenLocation(tokenParse.token);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatFormat(node));
    SWAG_CHECK(eatSemiCol("[[#placeholder]] expression"));

    currentScope->symTable.registerSymbolName(context, node, SymbolKind::PlaceHolder);

    return true;
}

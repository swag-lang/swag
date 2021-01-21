#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "Module.h"

bool SyntaxJob::doCompilerTag(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerSpecialFunction, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveCompilerSpecialFunction;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node));

    if (node->token.id == TokenId::CompilerTagVal)
    {
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(node));
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
    SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, "missing '#if' expression"));
    SWAG_CHECK(doExpression(node, &node->boolExpression));
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
        SWAG_VERIFY(node->ownerBreakable, error(token, "'#mixin' replacement block can only be used inside a breakable statement"));
        SWAG_CHECK(eatToken());
        if (token.id != TokenId::KwdBreak && token.id != TokenId::KwdContinue)
            return error(token, format("'#mixin' invalid replacement '%s'", token.text.c_str()));
        SWAG_VERIFY(token.id != TokenId::SymRightCurly, syntaxError(token, "'#mixin' empty replacement block"));

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
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::resolveCompilerMacro;

    SWAG_CHECK(tokenizer.getToken(token));
    auto newScope = Ast::newScope(node, "", ScopeKind::Macro, node->ownerScope);

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

    Scoped scoped(this, newScope);
    SWAG_CHECK(doCurlyStatement(node));
    return true;
}

bool SyntaxJob::isValidScopeForCompilerRun(AstNode* node)
{
    if (!node->ownerScope->isGlobalOrImpl() && node->ownerScope->kind != ScopeKind::FunctionBody)
    {
        auto scope = node->ownerScope;
        while (scope && (scope->kind == ScopeKind::EmptyStatement || scope->kind == ScopeKind::Inline || scope->kind == ScopeKind::Macro))
            scope = scope->parentScope;
        if (!scope->isGlobalOrImpl() && scope->kind != ScopeKind::FunctionBody)
            return node->sourceFile->report({node, node->token, format("compile time '%s' cannot be used in a local scope", node->token.text.c_str())});
    }

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
    SWAG_CHECK(isValidScopeForCompilerRun(node));

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

bool SyntaxJob::doCompilerSelectIf(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerSelectIf, sourceFile, parent);
    if (result)
        *result = node;
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
        SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerSelectIf));

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
        SWAG_CHECK(doExpression(node));
        SWAG_CHECK(eatSemiCol("after '#selectif' expression"));
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
        SWAG_CHECK(doExpression(node));
        SWAG_CHECK(eatSemiCol("after '#ast' expression"));
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
    SWAG_CHECK(eatSemiCol("after '#run' statement"));
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
    SWAG_CHECK(isValidScopeForCompilerRun(node));
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
        SWAG_CHECK(eatSemiCol("after '#run' expression"));
    }

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
    SWAG_CHECK(isValidScopeForCompilerRun(node));
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(node));
    SWAG_CHECK(eatSemiCol("after '#print' expression"));
    return true;
}

bool SyntaxJob::doCompilerGlobal(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, "'#global' can only be used in the top level scope"}));
    SWAG_VERIFY(!afterGlobal, error(token, "'#global' must be defined first, at the top of the file"));

    SWAG_CHECK(tokenizer.getToken(token));

    /////////////////////////////////
    if (token.text == "export")
    {
        if (!sourceFile->imported)
        {
            SWAG_VERIFY(!sourceFile->forceExport, error(token, "'#global export' already defined"));
            sourceFile->forceExport = true;
            sourceFile->module->addExportSourceFile(sourceFile);
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("after '#global export'"));
    }

    /////////////////////////////////
    else if (token.text == "generated")
    {
        sourceFile->generated = true;
        if (sourceFile->imported)
            sourceFile->imported->isSwag = true;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("after '#global generated'"));
    }

    /////////////////////////////////
    else if (token.text == "foreignlib")
    {
        auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerForeignLib, sourceFile, parent);
        if (result)
            *result = node;
        node->semanticFct = SemanticJob::resolveCompilerForeignLib;

        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_VERIFY(token.id == TokenId::LiteralString, syntaxError(token, "'#global foreignlib' must be followed by a string"));

        AstNode* literal;
        SWAG_CHECK(doLiteral(node, &literal));
        SWAG_VERIFY(literal->token.literalType == LiteralType::TT_STRING, syntaxError(literal->token, "'#global foreignlib' must be followed by a string"));
        SWAG_CHECK(eatSemiCol("after '#global foreignlib'"));
    }

    /////////////////////////////////
    else if (token.text == "if")
    {
        auto node = Ast::newNode<AstIf>(this, AstNodeKind::CompilerIf, sourceFile, parent);
        if (result)
            *result = node;

        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, "missing '#global if' expression"));
        SWAG_CHECK(doExpression(node, &node->boolExpression));
        node->boolExpression->allocateExtension();
        node->boolExpression->extension->semanticAfterFct = SemanticJob::resolveCompilerIf;

        auto block    = Ast::newNode<AstCompilerIfBlock>(this, AstNodeKind::CompilerIfBlock, sourceFile, node);
        node->ifBlock = block;
        if (node->ownerCompilerIfBlock)
            node->ownerCompilerIfBlock->blocks.push_back(block);

        ScopedCompilerIfBlock scopedIf(this, block);
        SWAG_CHECK(eatSemiCol("after '#global if'"));
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
            sourceFile->report({sourceFile, token, format("invalid pass name '%s'", token.text.c_str())});
            return false;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("after '#global testpass'"));
    }

    /////////////////////////////////
    else if (token.text == "testerror")
    {
        // Put the file in its own module, because of errors
        if (!moduleSpecified)
        {
            moduleSpecified = true;
            auto newModule  = g_Workspace.createOrUseModule(sourceFile->name, sourceFile->module->path, sourceFile->module->kind);
            sourceFile->module->removeFile(sourceFile);
            newModule->addFile(sourceFile);
        }

        if (g_CommandLine.test)
        {
            sourceFile->numTestErrors++;
            sourceFile->module->numTestErrors++;
            if (currentCompilerIfBlock)
                currentCompilerIfBlock->numTestErrors++;
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatSemiCol("after '#global testerror"));
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
        SWAG_CHECK(doIdentifierRef(exprNode, nullptr, false));
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
    SWAG_CHECK(doIdentifierRef(exprNode, nullptr, false));

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
    SWAG_CHECK(doExpression(exprNode, nullptr));

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    exprNode->semanticFct = SemanticJob::resolveCompilerLoad;
    return true;
}

bool SyntaxJob::doCompilerImport(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isTopLevel(), sourceFile->report({sourceFile, token, "'#import' can only be declared in the top level scope"}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerImport, sourceFile, parent);
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::LiteralString, sourceFile->report({sourceFile, token, "'#import' must be followed by a string"}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());

    // Specific dependency stuff
    Utf8 forceNameSpace;
    Utf8 location;
    Utf8 version;
    if (sourceFile->cfgFile)
    {
        while (true)
        {
            if (token.text == "name")
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_VERIFY(forceNameSpace.empty(), error(token, "'#import' namespace defined twice"));
                SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid namespace name '%s'", token.text.c_str())));
                forceNameSpace = token.text;
                SWAG_CHECK(eatToken());
                continue;
            }

            if (token.text == "location")
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_VERIFY(location.empty(), error(token, "'#import' location defined twice"));
                SWAG_VERIFY(token.id == TokenId::LiteralString, syntaxError(token, format("invalid location '%s'", token.text.c_str())));
                location = token.text;
                SWAG_CHECK(eatToken());
                continue;
            }

            if (token.text == "version")
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(eatToken(TokenId::SymEqual));
                SWAG_VERIFY(version.empty(), error(token, "'#import' version defined twice"));
                SWAG_VERIFY(token.id == TokenId::LiteralString, syntaxError(token, format("invalid version '%s'", token.text.c_str())));
                version = token.text;
                SWAG_CHECK(eatToken());
                continue;
            }

            break;
        }
    }

    SWAG_CHECK(eatSemiCol("after '#import' expression"));
    SWAG_CHECK(sourceFile->module->addDependency(node, forceNameSpace, location, version));
    return true;
}

bool SyntaxJob::doCompilerPlaceHolder(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), sourceFile->report({sourceFile, token, "'#placeholder' can only be declared in a top level scope"}));

    auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerPlaceHolder, sourceFile, parent);
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, sourceFile->report({sourceFile, token, "'#placeholder' must be followed by an identifier"}));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatSemiCol("after '#placeholder' expression"));

    currentScope->symTable.registerSymbolName(&context, node, SymbolKind::PlaceHolder);

    return true;
}

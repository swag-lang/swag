#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "Scope.h"
#include "Scoped.h"

bool SyntaxJob::doIf(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstIf>(this, AstNodeKind::If, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveIf;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    SWAG_CHECK(doExpression(node, &node->boolExpression));
    SWAG_CHECK(doEmbeddedStatement(node, &node->ifBlock));

    if (token.id == TokenId::KwdElse)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doEmbeddedStatement(node, &node->elseBlock));
    }

    return true;
}

bool SyntaxJob::doWhile(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstWhile>(this, AstNodeKind::While, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveWhile;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    {
        ScopedBreakable scoped(this, node);
        SWAG_CHECK(doExpression(node, &node->boolExpression));
        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doSwitch(AstNode* parent, AstNode** result)
{
    auto switchNode         = Ast::newNode<AstSwitch>(this, AstNodeKind::Switch, sourceFile, parent);
    switchNode->semanticFct = SemanticJob::resolveSwitch;
    if (result)
        *result = switchNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doExpression(switchNode, &switchNode->expression));
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    AstSwitchCase*      defaultCase      = nullptr;
    AstSwitchCaseBlock* defaultStatement = nullptr;
    bool                hasDefault       = false;
    while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
    {
        SWAG_VERIFY(token.id == TokenId::KwdCase || token.id == TokenId::KwdDefault, sourceFile->report({sourceFile, token, "'case' or 'default' expression expected"}));
        bool isDefault = token.id == TokenId::KwdDefault;
        SWAG_VERIFY(!isDefault || !hasDefault, sourceFile->report({sourceFile, token, "'default' expression already defined"}));
        if (isDefault)
            hasDefault = true;

        // One case
        auto caseNode         = Ast::newNode<AstSwitchCase>(this, AstNodeKind::SwitchCase, sourceFile, isDefault ? nullptr : switchNode);
        caseNode->isDefault   = isDefault;
        caseNode->ownerSwitch = switchNode;
        caseNode->semanticFct = SemanticJob::resolveCase;
        SWAG_CHECK(tokenizer.getToken(token));
        if (isDefault)
            defaultCase = caseNode;
        else
            switchNode->cases.push_back(caseNode);

        // Case expressions
        if (!isDefault)
        {
            while (token.id != TokenId::SymColon)
            {
                AstNode* expression;
                SWAG_CHECK(doExpression(caseNode, &expression));
                caseNode->expressions.push_back(expression);
                if (token.id != TokenId::SymComma)
                    break;
                SWAG_CHECK(eatToken());
            }
        }

        if (isDefault)
            SWAG_CHECK(eatToken(TokenId::SymColon, "after 'default' expression"));
        else
            SWAG_CHECK(eatToken(TokenId::SymColon, "after 'case' expression"));

        // Content
        {
            auto   newScope = Ast::newScope(switchNode, "", ScopeKind::Statement, currentScope);
            Scoped scoped(this, newScope);

            auto statement               = Ast::newNode<AstSwitchCaseBlock>(this, AstNodeKind::Statement, sourceFile, caseNode);
            statement->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
            statement->ownerCase         = caseNode;
            caseNode->block              = statement;
            if (isDefault)
                defaultStatement = statement;

            // Instructions
            ScopedBreakable scopedBreakable(this, switchNode);
            while (token.id != TokenId::KwdCase && token.id != TokenId::KwdDefault && token.id != TokenId::SymRightCurly)
            {
                SWAG_CHECK(doEmbeddedInstruction(statement));
            }
        }
    }

    // Add the default case as the last one
    if (defaultCase)
    {
        Ast::addChildBack(switchNode, defaultCase);
        switchNode->cases.push_back(defaultCase);
    }

    SWAG_CHECK(eatToken(TokenId::SymRightCurly));

    return true;
}

bool SyntaxJob::doFor(AstNode* parent, AstNode** result)
{
    auto   newScope = Ast::newScope(nullptr, "", ScopeKind::Statement, currentScope);
    Scoped scoped(this, newScope);

    auto node               = Ast::newNode<AstFor>(this, AstNodeKind::For, sourceFile, parent);
    node->semanticBeforeFct = SemanticJob::resolveForBefore;
    node->semanticFct       = SemanticJob::resolveFor;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    bool hasParen = token.id == TokenId::SymLeftParen;
    if (hasParen)
        SWAG_CHECK(eatToken());

    {
        ScopedBreakable scopedBreakable(this, node);

        // Pre statement. Do not call doScopedCurlyStatement in order to avoid
        // creating a new scope in the case of for { i:= 0; j := 0 } for example
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doCurlyStatement(node, &node->preExpression));
        else
            SWAG_CHECK(doEmbeddedInstruction(node, &node->preExpression));

        // Boolean expression
        SWAG_CHECK(doExpression(node, &node->boolExpression));
        SWAG_CHECK(eatSemiCol("after 'for' boolean expression"));

        // Post expression
        SWAG_CHECK(doEmbeddedStatement(node, &node->postExpression));

        if (hasParen)
            SWAG_CHECK(eatToken(TokenId::SymRightParen));

        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doVisit(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstVisit>(this, AstNodeKind::Visit, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveVisit;
    if (result)
        *result = node;

    // Eat visit keyword
    SWAG_CHECK(tokenizer.getToken(token));

    // Ear extra name on the special function
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "opVisit name expected"));
        node->extraName = token.text;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    // Variable to visit
    SWAG_CHECK(doIdentifierRef(nullptr, &node->expression));
    if (token.id == TokenId::SymColon || token.id == TokenId::SymComma)
    {
        if (node->expression->childs.size() != 1)
            return sourceFile->report({node->expression, "invalid visitor name"});
        node->aliasNames.push_back(node->expression->childs.back()->name);
        while (token.id != TokenId::SymColon)
        {
            SWAG_CHECK(eatToken(TokenId::SymComma));
            SWAG_CHECK(doIdentifierRef(nullptr, &node->expression));
            if (node->expression->childs.size() != 1)
                return sourceFile->report({node->expression, "invalid visitor name"});
            node->aliasNames.push_back(node->expression->childs.back()->name);
        }

        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doIdentifierRef(node, &node->expression));
    }
    else
    {
        Ast::addChildBack(node, node->expression);
    }

    // Visit statement code block
    SWAG_CHECK(doEmbeddedStatement(nullptr, &node->block));

    // We do not want semantic on the block part, as this has to be solved when the block
    // is inlined
    node->block->flags |= AST_NO_SEMANTIC;

    return true;
}

bool SyntaxJob::doLoop(AstNode* parent, AstNode** result)
{
    auto   newScope = Ast::newScope(nullptr, "", ScopeKind::Statement, currentScope);
    Scoped scoped(this, newScope);

    auto node               = Ast::newNode<AstLoop>(this, AstNodeKind::Loop, sourceFile, parent);
    node->semanticBeforeFct = SemanticJob::resolveLoopBefore;
    node->semanticFct       = SemanticJob::resolveLoop;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    bool hasParen = token.id == TokenId::SymLeftParen;
    if (hasParen)
        SWAG_CHECK(eatToken());

    {
        ScopedBreakable scopedBreakable(this, node);
        SWAG_CHECK(doExpression(nullptr, &node->expression));
        Token tokenName = node->expression->token;

        Utf8 name;
        if (token.id == TokenId::SymColon)
        {
            if (node->expression->kind != AstNodeKind::IdentifierRef || node->expression->childs.size() != 1)
                return sourceFile->report({node->expression, format("invalid named index '%s'", token.text.c_str())});
            name = node->expression->childs.front()->name;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doExpression(node, &node->expression));
        }
        else
        {
            Ast::addChildBack(node, node->expression);
        }

        // Creates a variable if we have a named index
        if (!name.empty())
        {
            auto var   = Ast::newVarDecl(sourceFile, name, node, this, AstNodeKind::LetDecl);
            var->token = tokenName;

            auto identifer         = Ast::newNode<AstNode>(this, AstNodeKind::Index, sourceFile, var);
            identifer->semanticFct = SemanticJob::resolveIndex;

            var->assignment = identifer;
        }

        if (hasParen)
            SWAG_CHECK(eatToken(TokenId::SymRightParen));

        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doIndex(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Index, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveIndex;
    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doBreak(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstBreakContinue>(this, AstNodeKind::Break, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveBreak;
    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
    if (tokenizer.lastTokenIsEOL)
        return true;
    if (token.id != TokenId::SymSemiColon)
    {
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "invalid label name, identifier expected"));
        node->label = move(token.text);
        SWAG_CHECK(eatToken());
    }

    return true;
}

bool SyntaxJob::doContinue(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstBreakContinue>(this, AstNodeKind::Continue, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveContinue;
    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
    if (tokenizer.lastTokenIsEOL)
        return true;
    if (token.id != TokenId::SymSemiColon)
    {
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "invalid label name, identifier expected"));
        node->label = move(token.text);
        SWAG_CHECK(eatToken());
    }

    return true;
}

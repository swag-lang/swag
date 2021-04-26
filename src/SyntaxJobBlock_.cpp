#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scoped.h"
#include "ErrorIds.h"

bool SyntaxJob::doIf(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstIf>(this, AstNodeKind::If, sourceFile, parent, 2);
    node->semanticFct = SemanticJob::resolveIf;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly, Msg0863));

    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
    SWAG_CHECK(doEmbeddedStatement(node, (AstNode**) &node->ifBlock));

    if (token.id == TokenId::KwdElse)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doEmbeddedStatement(node, (AstNode**) &node->elseBlock));
    }

    return true;
}

bool SyntaxJob::doWhile(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstWhile>(this, AstNodeKind::While, sourceFile, parent, 2);
    node->semanticFct = SemanticJob::resolveWhile;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    {
        ScopedBreakable scoped(this, node);
        SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly, Msg0864));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doSwitch(AstNode* parent, AstNode** result)
{
    auto switchNode         = Ast::newNode<AstSwitch>(this, AstNodeKind::Switch, sourceFile, parent, 4);
    switchNode->semanticFct = SemanticJob::resolveSwitch;
    if (result)
        *result = switchNode;

    // switch can have no expression
    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id != TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doExpression(switchNode, EXPR_FLAG_NONE, &switchNode->expression));
        switchNode->expression->allocateExtension();
        switchNode->expression->extension->semanticAfterFct = SemanticJob::resolveSwitchAfterExpr;
    }

    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    AstSwitchCase*      defaultCase      = nullptr;
    AstSwitchCaseBlock* defaultStatement = nullptr;
    bool                hasDefault       = false;
    while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
    {
        SWAG_CHECK(verifyError(token, token.id == TokenId::KwdCase || token.id == TokenId::KwdDefault, Msg0865));
        bool isDefault = token.id == TokenId::KwdDefault;
        SWAG_CHECK(verifyError(token, !isDefault || !hasDefault, Msg0866));
        if (isDefault)
            hasDefault = true;

        // One case
        auto caseNode         = Ast::newNode<AstSwitchCase>(this, AstNodeKind::SwitchCase, sourceFile, isDefault ? nullptr : switchNode);
        caseNode->isDefault   = isDefault;
        caseNode->ownerSwitch = switchNode;
        caseNode->semanticFct = SemanticJob::resolveCase;
        auto previousToken    = token;
        SWAG_CHECK(tokenizer.getToken(token));
        if (isDefault)
            defaultCase = caseNode;
        else
            switchNode->cases.push_back(caseNode);
        caseNode->caseIndex = (int) switchNode->cases.size() - 1;

        // Case expressions
        if (!isDefault)
        {
            SWAG_CHECK(verifyError(token, token.id != TokenId::SymColon, Msg0867));
            SWAG_CHECK(verifyError(token, token.id != TokenId::KwdBreak, Msg0868));
            while (token.id != TokenId::SymColon)
            {
                AstNode* expression;
                SWAG_CHECK(doExpression(caseNode, EXPR_FLAG_NONE, &expression));
                caseNode->expressions.push_back(expression);
                if (token.id != TokenId::SymComma)
                    break;
                SWAG_CHECK(eatToken());
            }
        }

        if (isDefault)
            SWAG_CHECK(eatToken(TokenId::SymColon, "after 'default' statement"));
        else
            SWAG_CHECK(eatToken(TokenId::SymColon, "after 'case' statement"));

        // Content
        {
            auto   newScope = Ast::newScope(switchNode, "", ScopeKind::Statement, currentScope);
            Scoped scoped(this, newScope);

            auto statement = Ast::newNode<AstSwitchCaseBlock>(this, AstNodeKind::Statement, sourceFile, caseNode);
            statement->allocateExtension();
            statement->extension->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
            statement->ownerCase                    = caseNode;
            caseNode->block                         = statement;
            if (isDefault)
                defaultStatement = statement;
            newScope->owner = statement;

            // Instructions
            ScopedBreakable scopedBreakable(this, switchNode);
            SWAG_CHECK(verifyError(previousToken, token.id != TokenId::KwdCase && token.id != TokenId::KwdDefault && token.id != TokenId::SymRightCurly, Msg0869));
            while (token.id != TokenId::KwdCase && token.id != TokenId::KwdDefault && token.id != TokenId::SymRightCurly)
                SWAG_CHECK(doEmbeddedInstruction(statement));
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
    auto   newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped scoped(this, newScope);

    auto node = Ast::newNode<AstFor>(this, AstNodeKind::For, sourceFile, parent, 4);
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::resolveForBefore;
    node->semanticFct                  = SemanticJob::resolveFor;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    ScopedBreakable scopedBreakable(this, node);

    // Pre statement. Do not call doScopedCurlyStatement in order to avoid
    // creating a new scope in the case of for { i:= 0; j := 0 } for example
    if (token.id == TokenId::SymLeftCurly)
        SWAG_CHECK(doCurlyStatement(node, &node->preExpression));
    else
        SWAG_CHECK(doEmbeddedInstruction(node, &node->preExpression));

    // Boolean expression
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
    SWAG_CHECK(eatSemiCol("'for' boolean expression"));

    // Post expression
    if (token.id == TokenId::SymLeftCurly)
        SWAG_CHECK(doCurlyStatement(node, &node->postExpression));
    else
        SWAG_CHECK(doEmbeddedInstruction(node, &node->postExpression));

    // For optim purposes in the bytecode generation, we must generate postExpression first,
    // then the bool expression. So here we put the bool expression after the post one.
    Ast::removeFromParent(node->boolExpression);
    Ast::addChildBack(node, node->boolExpression);

    SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    return true;
}

bool SyntaxJob::doVisit(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstVisit>(this, AstNodeKind::Visit, sourceFile, parent, 3);
    node->semanticFct = SemanticJob::resolveVisit;
    if (result)
        *result = node;

    // Eat visit keyword
    SWAG_CHECK(tokenizer.getToken(token));

    // Extra name on the special function
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(verifyError(token, token.id == TokenId::Identifier, Msg0870));
        node->extraNameToken = move(token);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    if (token.id == TokenId::SymAsterisk)
    {
        node->wantPointerToken = move(token);
        node->wantPointer      = true;
        SWAG_CHECK(tokenizer.getToken(token));
    }

    // Variable to visit
    SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly, Msg0871));
    {
        PushErrHint errh(Msg0357);
        SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));
    }

    if (token.id == TokenId::SymColon || token.id == TokenId::SymComma)
    {
        SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'visit' variable name"));
        SWAG_CHECK(checkIsValidVarName(node->expression->childs.back()));
        node->aliasNames.push_back(node->expression->childs.back()->token);
        while (token.id != TokenId::SymColon)
        {
            auto prevToken = token;
            SWAG_CHECK(eatToken(TokenId::SymComma));

            {
                PushErrHint errh(Msg0358);
                SWAG_CHECK(verifyError(prevToken, token.id != TokenId::SymColon, Msg0872));
            }

            {
                PushErrHint errh(Msg0359);
                SWAG_CHECK(doIdentifierRef(nullptr, &node->expression));
            }

            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'visit' variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->childs.back()));
            node->aliasNames.push_back(node->expression->childs.back()->token);
        }

        SWAG_CHECK(eatToken(TokenId::SymColon));

        {
            SWAG_CHECK(verifyError(token, token.id != TokenId::SymLeftCurly, Msg0873));
            PushErrHint errh(Msg0360);
            SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
        }
    }
    else
    {
        Ast::addChildBack(node, node->expression);
    }

    // Visit statement code block
    SWAG_CHECK(doEmbeddedStatement(node, &node->block));

    // We do not want semantic on the block part, as this has to be solved when the block
    // is inlined
    node->block->flags |= AST_NO_SEMANTIC;

    return true;
}

bool SyntaxJob::doLoop(AstNode* parent, AstNode** result)
{
    auto   newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped scoped(this, newScope);

    auto node = Ast::newNode<AstLoop>(this, AstNodeKind::Loop, sourceFile, parent, 2);
    node->allocateExtension();
    node->extension->semanticBeforeFct = SemanticJob::resolveLoopBefore;
    node->semanticFct                  = SemanticJob::resolveLoop;
    if (result)
        *result = node;
    newScope->owner = node;

    SWAG_CHECK(tokenizer.getToken(token));

    ScopedBreakable scopedBreakable(this, node);
    SWAG_CHECK(verifyError(node->token, token.id != TokenId::SymLeftCurly, Msg0874));

    {
        PushErrHint errh(Msg0361);
        SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));
    }

    Token tokenName = node->expression->token;

    Utf8 name;
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'loop' variable name"));
        SWAG_CHECK(checkIsValidVarName(node->expression->childs.back()));
        name = node->expression->childs.back()->token.text;
        SWAG_CHECK(eatToken());

        {
            SWAG_CHECK(verifyError(token, token.id != TokenId::SymLeftCurly, Msg0875));
            PushErrHint errh(Msg0362);
            SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
        }
    }
    else
    {
        Ast::addChildBack(node, node->expression);
    }

    // Range
    if (token.id == TokenId::SymDotDot)
    {
        SWAG_CHECK(eatToken());
        PushErrHint errh(Msg0363);
        SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression1));
    }

    // Creates a variable if we have a named index
    if (!name.empty())
    {
        auto var         = Ast::newVarDecl(sourceFile, name, node, this, AstNodeKind::VarDecl);
        var->token       = tokenName;
        var->constAssign = true;
        var->inheritTokenLocation(node->expression->token);
        node->specificName = var;

        auto identifer         = Ast::newNode<AstNode>(this, AstNodeKind::Index, sourceFile, var);
        identifer->semanticFct = SemanticJob::resolveIndex;
        identifer->inheritTokenLocation(var->token);

        var->assignment = identifer;
    }

    SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    return true;
}

bool SyntaxJob::doGetErr(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::GetErr, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveGetErr;
    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
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
        SWAG_CHECK(verifyError(token, token.id == TokenId::Identifier, Msg0876));
        node->label = move(token.text);
        SWAG_CHECK(eatToken());
    }

    return true;
}

bool SyntaxJob::doFallThrough(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstBreakContinue>(this, AstNodeKind::FallThrough, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveFallThrough;
    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));
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
        SWAG_CHECK(verifyError(token, token.id == TokenId::Identifier, Msg0877));
        node->label = move(token.text);
        SWAG_CHECK(eatToken());
    }

    return true;
}

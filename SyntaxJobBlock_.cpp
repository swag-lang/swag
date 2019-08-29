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
    auto node         = Ast::newNode(this, &g_Pool_astIf, AstNodeKind::If, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveIf;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
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
    auto node         = Ast::newNode(this, &g_Pool_astWhile, AstNodeKind::While, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveWhile;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));

    {
        ScopedBreakable scoped(this, node);
        SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doSwitch(AstNode* parent, AstNode** result)
{
    auto switchNode         = Ast::newNode(this, &g_Pool_astSwitch, AstNodeKind::Switch, sourceFile->indexInModule, parent);
    switchNode->semanticFct = &SemanticJob::resolveSwitch;
    switchNode->inheritToken(token);
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
        auto caseNode         = Ast::newNode(this, &g_Pool_astSwitchCase, AstNodeKind::SwitchCase, sourceFile->indexInModule, isDefault ? nullptr : switchNode);
        caseNode->isDefault   = isDefault;
        caseNode->ownerSwitch = switchNode;
        caseNode->semanticFct = &SemanticJob::resolveCase;
        caseNode->inheritToken(token);
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
        {
            SWAG_CHECK(eatToken(TokenId::SymColon, "after 'default' expression"));
        }
        else
        {
            SWAG_CHECK(eatToken(TokenId::SymColon, "after 'case' expression"));
        }

        // Content
        auto statement       = Ast::newNode(this, &g_Pool_astSwitchCaseBlock, AstNodeKind::Statement, sourceFile->indexInModule, isDefault ? nullptr : switchNode);
        statement->ownerCase = caseNode;
        caseNode->block      = statement;
        if (isDefault)
            defaultStatement = statement;

        // Instructions
        ScopedBreakable scoped(this, switchNode);
        while (token.id != TokenId::KwdCase && token.id != TokenId::KwdDefault && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doEmbeddedInstruction(statement));
        }
    }

    // Add the default case as the last one
    if (defaultCase)
    {
        Ast::addChild(switchNode, defaultCase);
        Ast::addChild(switchNode, defaultStatement);
        switchNode->cases.push_back(defaultCase);
    }

    SWAG_CHECK(eatToken(TokenId::SymRightCurly));

    return true;
}

bool SyntaxJob::doFor(AstNode* parent, AstNode** result)
{
    auto   newScope = Ast::newScope(nullptr, "", ScopeKind::Statement, currentScope);
    Scoped scoped(this, newScope);

    auto node               = Ast::newNode(this, &g_Pool_astFor, AstNodeKind::For, sourceFile->indexInModule, parent);
    node->semanticBeforeFct = &SemanticJob::resolveForBefore;
    node->semanticFct       = &SemanticJob::resolveFor;
    node->inheritToken(token);
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
        {
            SWAG_CHECK(doCurlyStatement(node, &node->preExpression));
        }
        else
        {
            SWAG_CHECK(doEmbeddedInstruction(node, &node->preExpression));
        }

        SWAG_CHECK(doBoolExpression(node, &node->boolExpression));
        SWAG_CHECK(eatSemiCol("after 'for' boolean expression"));
        SWAG_CHECK(doEmbeddedStatement(node, &node->postExpression));

        if (hasParen)
            SWAG_CHECK(eatToken(TokenId::SymRightParen));

        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doLoop(AstNode* parent, AstNode** result)
{
    auto newScope = Ast::newScope(nullptr, "", ScopeKind::Statement, currentScope);
    newScope->allocateSymTable();
    Scoped scoped(this, newScope);

    auto node               = Ast::newNode(this, &g_Pool_astLoop, AstNodeKind::Loop, sourceFile->indexInModule, parent);
    node->semanticBeforeFct = &SemanticJob::resolveLoopBefore;
    node->semanticFct       = &SemanticJob::resolveLoop;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    bool hasParen = token.id == TokenId::SymLeftParen;
    if (hasParen)
        SWAG_CHECK(eatToken());

    {
        ScopedBreakable scopedBreakable(this, node);
        SWAG_CHECK(doExpression(nullptr, &node->expression));

        Utf8  name;
        Token tokenName;
        if (token.id == TokenId::SymColon)
        {
            if (node->expression->kind != AstNodeKind::IdentifierRef || node->expression->childs.size() != 1)
                return sourceFile->report({sourceFile, node->expression, format("invalid named index '%s'", token.text.c_str())});
            name      = node->expression->childs.front()->name;
            tokenName = token;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doExpression(node, &node->expression));
        }
        else
        {
            Ast::addChild(node, node->expression);
        }

        // Creates a variable if we have a named index
        if (!name.empty())
        {
            auto var         = Ast::newNode(this, &g_Pool_astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, node);
            var->semanticFct = &SemanticJob::resolveVarDecl;
            var->token       = tokenName;
            var->name        = name;

            auto identifer         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Index, sourceFile->indexInModule, var);
            identifer->semanticFct = &SemanticJob::resolveIndex;
            identifer->inheritToken(token);

            var->assignment = identifer;
        }

        if (hasParen)
            SWAG_CHECK(eatToken(TokenId::SymRightParen));

        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool SyntaxJob::doBreak(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentBreakable, sourceFile->report({sourceFile, token, "'break' can only be used inside a breakable scope"}));

    auto node         = Ast::newNode(this, &g_Pool_astBreakContinue, AstNodeKind::Break, sourceFile->indexInModule, parent);
    node->byteCodeFct = &ByteCodeGenJob::emitBreak;
    node->inheritToken(token);
    if (result)
        *result = node;

    auto breakable = static_cast<AstBreakable*>(currentBreakable);
    breakable->breakList.push_back(node);

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doIndex(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentBreakable, sourceFile->report({sourceFile, token, "'index' can only be used inside a breakable loop"}));
    SWAG_VERIFY(currentBreakable->breakableFlags & BREAKABLE_CAN_HAVE_INDEX, sourceFile->report({sourceFile, token, "'index' can only be used inside a breakable loop"}));

    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Index, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveIndex;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doContinue(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentBreakable, sourceFile->report({sourceFile, token, "'continue' can only be used inside a breakable loop"}));
    SWAG_VERIFY(currentBreakable->breakableFlags & BREAKABLE_CAN_HAVE_CONTINUE, sourceFile->report({sourceFile, token, "'continue' can only be used inside a breakable loop"}));

    auto node         = Ast::newNode(this, &g_Pool_astBreakContinue, AstNodeKind::Continue, sourceFile->indexInModule, parent);
    node->byteCodeFct = &ByteCodeGenJob::emitContinue;
    node->inheritToken(token);
    if (result)
        *result = node;

    auto breakable = static_cast<AstBreakable*>(currentBreakable);
    breakable->continueList.push_back(node);

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

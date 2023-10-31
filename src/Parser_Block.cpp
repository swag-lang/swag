#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scoped.h"
#include "ErrorIds.h"
#include "ByteCodeGenJob.h"

bool Parser::doIf(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstIf>(this, AstNodeKind::If, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveIf;
    *result           = node;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, Fmt(Err(Err1084), token.ctext())));

    // If with an assignment
    if (token.id == TokenId::KwdVar || token.id == TokenId::KwdConst || token.id == TokenId::KwdLet)
    {
        node->specFlags |= AstIf::SPECFLAG_ASSIGN;

        auto   newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
        Scoped scoped(this, newScope);

        AstNode* varDecl;
        SWAG_CHECK(doVarDecl(node, &varDecl));

        SWAG_VERIFY(varDecl->childs.size() == 1, error(varDecl->childs.back()->token, Err(Err1065)));

        node->boolExpression = Ast::newIdentifierRef(sourceFile, varDecl->token.text, node, this);
        node->boolExpression->flags |= AST_GENERATED;
        node->boolExpression->inheritTokenLocation(varDecl);

        SWAG_CHECK(doEmbeddedStatement(node, (AstNode**) &node->ifBlock));

        if (token.id == TokenId::KwdElse)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doEmbeddedStatement(node, (AstNode**) &node->elseBlock));
        }
        else if (token.id == TokenId::KwdElif)
        {
            SWAG_CHECK(doIf(node, (AstNode**) &node->elseBlock));
        }
    }

    // If with a simple expression
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doEmbeddedStatement(node, (AstNode**) &node->ifBlock));

        if (token.id == TokenId::KwdElse)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doEmbeddedStatement(node, (AstNode**) &node->elseBlock));
        }
        else if (token.id == TokenId::KwdElif)
        {
            SWAG_CHECK(doIf(node, (AstNode**) &node->elseBlock));
        }
    }

    return true;
}

bool Parser::doWhile(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstWhile>(this, AstNodeKind::While, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveWhile;
    *result           = node;

    SWAG_CHECK(eatToken());

    {
        ScopedBreakable scoped(this, node);
        SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, Fmt(Err(Err1087), token.ctext())));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    }

    return true;
}

bool Parser::doSwitch(AstNode* parent, AstNode** result)
{
    auto switchNode         = Ast::newNode<AstSwitch>(this, AstNodeKind::Switch, sourceFile, parent);
    switchNode->semanticFct = SemanticJob::resolveSwitch;
    *result                 = switchNode;

    // switch can have no expression
    SWAG_CHECK(eatToken());
    if (token.id != TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doExpression(switchNode, EXPR_FLAG_NONE, &switchNode->expression));
        switchNode->expression->allocateExtension(ExtensionKind::Semantic);
        switchNode->expression->extSemantic()->semanticAfterFct = SemanticJob::resolveSwitchAfterExpr;
    }

    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the 'switch' body"));

    AstSwitchCase* defaultCase = nullptr;
    bool           hasDefault  = false;
    while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
    {
        SWAG_VERIFY(token.id == TokenId::KwdCase || token.id == TokenId::KwdDefault, error(token, Fmt(Err(Err1053), token.ctext())));
        bool isDefault = token.id == TokenId::KwdDefault;
        SWAG_VERIFY(!isDefault || !hasDefault, error(token, Err(Err1181)));
        if (isDefault)
            hasDefault = true;

        // One case
        auto caseNode         = Ast::newNode<AstSwitchCase>(this, AstNodeKind::SwitchCase, sourceFile, isDefault ? nullptr : switchNode);
        caseNode->specFlags   = isDefault ? AstSwitchCase::SPECFLAG_IS_DEFAULT : 0;
        caseNode->ownerSwitch = switchNode;
        caseNode->semanticFct = SemanticJob::resolveCase;
        auto previousToken    = token;
        SWAG_CHECK(eatToken());
        if (isDefault)
            defaultCase = caseNode;
        else
            switchNode->cases.push_back(caseNode);
        caseNode->caseIndex = (int) switchNode->cases.size() - 1;

        // Case expressions
        if (!isDefault)
        {
            SWAG_VERIFY(token.id != TokenId::SymColon, error(token, Fmt(Err(Err1055), token.ctext())));
            while (token.id != TokenId::SymColon)
            {
                AstNode* expression;
                SWAG_CHECK(doExpression(caseNode, EXPR_FLAG_NONE, &expression));
                if (token.id == TokenId::KwdTo || token.id == TokenId::KwdUntil)
                    SWAG_CHECK(doRange(caseNode, expression, &expression));
                caseNode->expressions.push_back(expression);
                if (token.id != TokenId::SymComma)
                    break;
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(token.id != TokenId::SymColon, error(token, Err(Err1056)));
            }
        }

        if (isDefault)
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'default' statement"));
        else
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'case' statement"));

        // Content
        {
            auto   newScope = Ast::newScope(switchNode, "", ScopeKind::Statement, currentScope);
            Scoped scoped(this, newScope);

            auto statement = Ast::newNode<AstSwitchCaseBlock>(this, AstNodeKind::SwitchCaseBlock, sourceFile, caseNode);
            statement->allocateExtension(ExtensionKind::Semantic);
            statement->extSemantic()->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
            statement->extSemantic()->semanticAfterFct  = SemanticJob::resolveScopedStmtAfter;
            statement->ownerCase                        = caseNode;
            caseNode->block                             = statement;
            newScope->owner                             = statement;

            // Instructions
            ScopedBreakable scopedBreakable(this, switchNode);
            if (token.id == TokenId::KwdCase || token.id == TokenId::KwdDefault)
                return error(previousToken, isDefault ? Err(Err1052) : Err(Err1158), Nte(Nte0137));
            if (token.id == TokenId::SymRightCurly)
                return error(previousToken, isDefault ? Err(Err1052) : Err(Err1158), Nte(Nte0138));
            while (token.id != TokenId::KwdCase && token.id != TokenId::KwdDefault && token.id != TokenId::SymRightCurly)
                SWAG_CHECK(doEmbeddedInstruction(statement, &dummyResult));
        }
    }

    // Add the default case as the last one
    if (defaultCase)
    {
        Ast::addChildBack(switchNode, defaultCase);
        switchNode->cases.push_back(defaultCase);
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the 'switch' body"));

    return true;
}

bool Parser::doFor(AstNode* parent, AstNode** result)
{
    auto   newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped scoped(this, newScope);

    auto node = Ast::newNode<AstFor>(this, AstNodeKind::For, sourceFile, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = SemanticJob::resolveForBefore;
    node->semanticFct                      = SemanticJob::resolveFor;
    *result                                = node;

    SWAG_CHECK(eatToken());

    ScopedBreakable scopedBreakable(this, node);

    // Pre statement. Do not call doScopedCurlyStatement in order to avoid
    // creating a new scope in the case of for { var i = 0; var j = 0 } for example
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

bool Parser::doVisit(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstVisit>(this, AstNodeKind::Visit, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveVisit;
    *result           = node;

    // Eat visit keyword
    SWAG_CHECK(eatToken());

    // Reverse loop
    uint32_t mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->tokenId, mdfFlags));
    if (mdfFlags & MODIFIER_BACK)
    {
        node->specFlags |= AstVisit::SPECFLAG_BACK;
    }

    // Extra name on the special function
    if (token.id == TokenId::SymLeftParen)
    {
        auto startLoc = token.startLocation;
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id != TokenId::SymRightParen, error(token, Err(Err1214)));
        SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1115), token.ctext())));
        node->extraNameToken = token;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, "to end the 'visit' argument"));
    }

    if (token.id == TokenId::SymAmpersand)
    {
        node->wantPointerToken = token;
        node->specFlags        = AstVisit::SPECFLAG_WANT_POINTER;
        SWAG_CHECK(eatToken());
    }

    // Variable to visit
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, Fmt(Err(Err1086), token.ctext())));
    SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));

    if (token.id == TokenId::SymColon || token.id == TokenId::SymComma)
    {
        SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'visit' variable name"));
        SWAG_CHECK(checkIsValidVarName(node->expression->childs.back()));
        node->aliasNames.push_back(node->expression->childs.back()->token);
        node->expression->release();
        while (token.id != TokenId::SymColon)
        {
            SWAG_CHECK(eatToken(TokenId::SymComma, "to define another alias name or ':' to specify the visit variable"));
            SWAG_VERIFY(token.id != TokenId::SymColon, error(prevToken, Err(Err1212)));
            SWAG_CHECK(doIdentifierRef(nullptr, &node->expression));
            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'visit' variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->childs.back()));
            node->aliasNames.push_back(node->expression->childs.back()->token);
            node->expression->release();
        }

        SWAG_CHECK(eatToken(TokenId::SymColon, "to define the 'visit' variable"));
        SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err1086)));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
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

bool Parser::doLoop(AstNode* parent, AstNode** result)
{
    auto   newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped scoped(this, newScope);

    auto node = Ast::newNode<AstLoop>(this, AstNodeKind::Loop, sourceFile, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = SemanticJob::resolveLoopBefore;
    node->semanticFct                      = SemanticJob::resolveLoop;
    *result                                = node;
    newScope->owner                        = node;

    SWAG_CHECK(eatToken());

    ScopedBreakable scopedBreakable(this, node);

    // loop can be empty for an infinit loop
    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doEmbeddedStatement(node, &node->block));
        return true;
    }

    // Reverse loop
    uint32_t mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->tokenId, mdfFlags));
    if (mdfFlags & MODIFIER_BACK)
    {
        node->specFlags |= AstLoop::SPECFLAG_BACK;
    }

    Utf8  name;
    Token tokenName;
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
    }
    else
    {
        SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));

        tokenName = node->expression->token;
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'loop' variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->childs.back()));
            name = node->expression->childs.back()->token.text;
            node->expression->release();
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, Fmt(Err(Err1085), token.ctext())));
            SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
        }
        else
        {
            Ast::addChildBack(node, node->expression);

            // Missing ':' ?
            if (node->expression->kind == AstNodeKind::IdentifierRef &&
                node->expression->childs.size() == 1 &&
                node->expression->childs.back()->kind == AstNodeKind::Identifier &&
                token.id == TokenId::LiteralNumber)
            {
                return error(token, Fmt(Err(Err1051), node->expression->childs.back()->token.ctext()));
            }
        }
    }

    // Range
    if (token.id == TokenId::KwdTo || token.id == TokenId::KwdUntil)
    {
        SWAG_CHECK(doRange(node, node->expression, &node->expression));
    }

    // Creates a variable if we have a named index
    if (!name.empty())
    {
        auto var   = Ast::newVarDecl(sourceFile, name, node, this, AstNodeKind::VarDecl);
        var->token = tokenName;
        var->specFlags |= AstVarDecl::SPECFLAG_CONST_ASSIGN;
        node->specificName = var;

        auto identifer         = Ast::newNode<AstNode>(this, AstNodeKind::Index, sourceFile, var);
        identifer->semanticFct = SemanticJob::resolveIndex;
        identifer->inheritTokenLocation(var);

        var->assignment = identifer;
    }

    SWAG_CHECK(doEmbeddedStatement(node, &node->block));
    return true;
}

bool Parser::doIndex(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Index, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveIndex;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doFallThrough(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstBreakContinue>(this, AstNodeKind::FallThrough, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveFallThrough;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doUnreachable(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Unreachable, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveUnreachable;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doBreak(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstBreakContinue>(this, AstNodeKind::Break, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveBreak;
    *result           = node;
    SWAG_CHECK(eatToken());
    if (token.flags & TOKENPARSE_LAST_EOL)
        return true;

    if (token.id != TokenId::SymSemiColon)
    {
        SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1108), token.ctext())));
        node->label = token;
        SWAG_CHECK(eatToken());
    }

    return true;
}

bool Parser::doContinue(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstBreakContinue>(this, AstNodeKind::Continue, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveContinue;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());
    return true;
}

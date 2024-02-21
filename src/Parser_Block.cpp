#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Semantic.h"

bool Parser::doIf(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstIf>(this, AstNodeKind::If, sourceFile, parent);
    node->semanticFct = Semantic::resolveIf;
    *result           = node;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, FMT(Err(Err0534), token.c_str())));

    // If with an assignment
    if (token.id == TokenId::KwdVar || token.id == TokenId::KwdConst || token.id == TokenId::KwdLet)
    {
        node->addSpecFlag(AstIf::SPEC_FLAG_ASSIGN);

        const auto newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
        Scoped     scoped(this, newScope);

        AstNode* varDecl;
        SWAG_CHECK(doVarDecl(node, &varDecl));

        SWAG_VERIFY(varDecl->children.size() == 1, error(varDecl->children.back()->token, Err(Err0406)));

        node->boolExpression = Ast::newIdentifierRef(sourceFile, varDecl->token.text, node, this);
        node->boolExpression->addAstFlag(AST_GENERATED);
        node->boolExpression->inheritTokenLocation(varDecl);

        SWAG_CHECK(doScopedStatement(node, node->token, reinterpret_cast<AstNode**>(&node->ifBlock)));

        if (token.id == TokenId::KwdElse)
        {
            const auto tokenElse = token;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doScopedStatement(node, tokenElse, reinterpret_cast<AstNode**>(&node->elseBlock)));
        }
        else if (token.id == TokenId::KwdElif)
        {
            SWAG_CHECK(doIf(node, reinterpret_cast<AstNode**>(&node->elseBlock)));
        }
    }

    // If with a simple expression
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doScopedStatement(node, node->token, reinterpret_cast<AstNode**>(&node->ifBlock)));

        if (token.id == TokenId::KwdElse)
        {
            const auto tokenElse = token;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doScopedStatement(node, tokenElse, reinterpret_cast<AstNode**>(&node->elseBlock)));
        }
        else if (token.id == TokenId::KwdElif)
        {
            SWAG_CHECK(doIf(node, reinterpret_cast<AstNode**>(&node->elseBlock)));
        }
    }

    return true;
}

bool Parser::doWhile(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstWhile>(this, AstNodeKind::While, sourceFile, parent);
    node->semanticFct = Semantic::resolveWhile;
    *result           = node;

    SWAG_CHECK(eatToken());

    {
        ScopedBreakable scoped(this, node);
        SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, FMT(Err(Err0539), token.c_str())));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doScopedStatement(node, node->token, &node->block));
    }

    return true;
}

bool Parser::doSwitch(AstNode* parent, AstNode** result)
{
    const auto switchNode   = Ast::newNode<AstSwitch>(this, AstNodeKind::Switch, sourceFile, parent);
    switchNode->semanticFct = Semantic::resolveSwitch;
    *result                 = switchNode;

    // switch can have no expression
    SWAG_CHECK(eatToken());
    if (token.id != TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doExpression(switchNode, EXPR_FLAG_NONE, &switchNode->expression));
        switchNode->expression->allocateExtension(ExtensionKind::Semantic);
        switchNode->expression->extSemantic()->semanticAfterFct = Semantic::resolveSwitchAfterExpr;
    }

    const auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[switch]] body"));

    AstSwitchCase* defaultCase = nullptr;
    bool           hasDefault  = false;
    while (token.id != TokenId::SymRightCurly && token.id != TokenId::EndOfFile)
    {
        SWAG_VERIFY(token.id == TokenId::KwdCase || token.id == TokenId::KwdDefault, error(token, FMT(Err(Err0168), token.c_str())));
        const bool isDefault = token.id == TokenId::KwdDefault;
        SWAG_VERIFY(!isDefault || !hasDefault, error(token, Err(Err0007)));
        if (isDefault)
            hasDefault = true;

        // One case
        auto caseNode            = Ast::newNode<AstSwitchCase>(this, AstNodeKind::SwitchCase, sourceFile, isDefault ? nullptr : switchNode);
        caseNode->specFlags      = isDefault ? AstSwitchCase::SPEC_FLAG_IS_DEFAULT : 0;
        caseNode->ownerSwitch    = switchNode;
        caseNode->semanticFct    = Semantic::resolveCase;
        const auto previousToken = token;
        SWAG_CHECK(eatToken());
        if (isDefault)
            defaultCase = caseNode;
        else
            switchNode->cases.push_back(caseNode);
        caseNode->caseIndex = static_cast<int>(switchNode->cases.size()) - 1;

        // Case expressions
        if (!isDefault)
        {
            SWAG_VERIFY(token.id != TokenId::SymColon, error(token, FMT(Err(Err0531), token.c_str())));
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
                SWAG_VERIFY(token.id != TokenId::SymColon, error(token, Err(Err0118)));
            }
        }

        if (isDefault)
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'default' statement"));
        else
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'case' statement"));

        // Content
        {
            const auto newScope = Ast::newScope(switchNode, "", ScopeKind::Statement, currentScope);
            Scoped     scoped(this, newScope);

            const auto statement = Ast::newNode<AstSwitchCaseBlock>(this, AstNodeKind::SwitchCaseBlock, sourceFile, caseNode);
            statement->allocateExtension(ExtensionKind::Semantic);
            statement->extSemantic()->semanticBeforeFct = Semantic::resolveScopedStmtBefore;
            statement->extSemantic()->semanticAfterFct  = Semantic::resolveScopedStmtAfter;
            statement->ownerCase                        = caseNode;
            caseNode->block                             = statement;
            newScope->owner                             = statement;

            // Instructions
            ScopedBreakable scopedBreakable(this, switchNode);
            if (token.id == TokenId::KwdCase || token.id == TokenId::KwdDefault)
                return error(previousToken, isDefault ? Err(Err0074) : Err(Err0073), Nte(Nte0029));
            if (token.id == TokenId::SymRightCurly)
                return error(previousToken, isDefault ? Err(Err0074) : Err(Err0073), Nte(Nte0028));
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

    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[switch]] body"));

    return true;
}

bool Parser::doFor(AstNode* parent, AstNode** result)
{
    const auto newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped     scoped(this, newScope);

    const auto node = Ast::newNode<AstFor>(this, AstNodeKind::For, sourceFile, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveForBefore;
    node->semanticFct                      = Semantic::resolveFor;
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
    SWAG_CHECK(eatSemiCol("[[for]] boolean expression"));

    // Post expression
    if (token.id == TokenId::SymLeftCurly)
        SWAG_CHECK(doCurlyStatement(node, &node->postExpression));
    else
        SWAG_CHECK(doEmbeddedInstruction(node, &node->postExpression));

    // For optimisation purposes in the bytecode generation, we must generate postExpression first,
    // then the bool expression. So here we put the bool expression after the post one.
    Ast::removeFromParent(node->boolExpression);
    Ast::addChildBack(node, node->boolExpression);

    SWAG_CHECK(doScopedStatement(node, node->token, &node->block));
    return true;
}

bool Parser::doVisit(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstVisit>(this, AstNodeKind::Visit, sourceFile, parent);
    node->semanticFct = Semantic::resolveVisit;
    *result           = node;

    // Eat visit keyword
    SWAG_CHECK(eatToken());

    // Specialized name
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0170), token.c_str())));
        node->extraNameToken = static_cast<Token>(token);
        SWAG_CHECK(eatToken());
    }

    // Reverse loop
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->tokenId, mdfFlags, node));
    if (mdfFlags.has(MODIFIER_BACK))
    {
        node->addSpecFlag(AstVisit::SPEC_FLAG_BACK);
    }

    if (token.id == TokenId::SymAmpersand)
    {
        node->wantPointerToken = static_cast<Token>(token);
        node->specFlags        = AstVisit::SPEC_FLAG_WANT_POINTER;
        SWAG_CHECK(eatToken());
    }

    // Variable to visit
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, FMT(Err(Err0537), token.c_str())));
    SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));

    if (token.id == TokenId::SymColon || token.id == TokenId::SymComma)
    {
        SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'visit' variable name"));
        SWAG_CHECK(checkIsValidVarName(node->expression->children.back()));
        node->aliasNames.push_back(node->expression->children.back()->token);
        node->expression->release();
        while (token.id != TokenId::SymColon)
        {
            SWAG_CHECK(eatToken(TokenId::SymComma, "to define another alias name or ':' to specify the visit variable"));
            SWAG_VERIFY(token.id != TokenId::SymColon, error(prevToken, Err(Err0538)));
            SWAG_CHECK(doIdentifierRef(nullptr, &node->expression));
            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a 'visit' variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->children.back()));
            node->aliasNames.push_back(node->expression->children.back()->token);
            node->expression->release();
        }

        SWAG_CHECK(eatToken(TokenId::SymColon, "to define the 'visit' variable"));
        SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err0537)));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
    }
    else
    {
        Ast::addChildBack(node, node->expression);
    }

    // Visit statement code block
    SWAG_CHECK(doScopedStatement(node, node->token, &node->block));

    // We do not want semantic on the block part, as this has to be solved when the block
    // is inlined
    node->block->addAstFlag(AST_NO_SEMANTIC);

    return true;
}

bool Parser::doLoop(AstNode* parent, AstNode** result)
{
    const auto newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped     scoped(this, newScope);

    const auto node = Ast::newNode<AstLoop>(this, AstNodeKind::Loop, sourceFile, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveLoopBefore;
    node->semanticFct                      = Semantic::resolveLoop;
    *result                                = node;
    newScope->owner                        = node;

    SWAG_CHECK(eatToken());

    ScopedBreakable scopedBreakable(this, node);

    // loop can be empty for an infinite loop
    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doScopedStatement(node, node->token, &node->block));
        return true;
    }

    // Reverse loop
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->tokenId, mdfFlags));
    if (mdfFlags.has(MODIFIER_BACK))
    {
        node->addSpecFlag(AstLoop::SPEC_FLAG_BACK);
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
            SWAG_CHECK(checkIsValidVarName(node->expression->children.back()));
            name = node->expression->children.back()->token.text;
            node->expression->release();
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id != TokenId::SymLeftCurly && token.id != TokenId::SymSemiColon, error(token, FMT(Err(Err0536), token.c_str())));
            SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
        }
        else
        {
            Ast::addChildBack(node, node->expression);

            // Missing ':' ?
            if (node->expression->kind == AstNodeKind::IdentifierRef &&
                node->expression->children.size() == 1 &&
                node->expression->children.back()->kind == AstNodeKind::Identifier &&
                token.id == TokenId::LiteralNumber)
            {
                return error(token, FMT(Err(Err0530), node->expression->children.back()->token.c_str()));
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
        const auto var = Ast::newVarDecl(sourceFile, name, node, this, AstNodeKind::VarDecl);
        var->token     = tokenName;
        var->addSpecFlag(AstVarDecl::SPEC_FLAG_CONST_ASSIGN | AstVarDecl::SPEC_FLAG_IS_LET);
        node->specificName = var;

        const auto identifer   = Ast::newNode<AstNode>(this, AstNodeKind::Index, sourceFile, var);
        identifer->semanticFct = Semantic::resolveIndex;
        identifer->inheritTokenLocation(var);

        var->assignment = identifer;
    }

    SWAG_CHECK(doScopedStatement(node, node->token, &node->block));
    return true;
}

bool Parser::doWith(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());
    const auto node = Ast::newNode<AstWith>(this, AstNodeKind::With, sourceFile, parent);
    *result         = node;

    AstNode* id = nullptr;
    if (token.id == TokenId::KwdVar || token.id == TokenId::KwdLet)
    {
        SWAG_CHECK(doVarDecl(node, &id));
        if (id->kind != AstNodeKind::VarDecl)
        {
            const Diagnostic err{id->token.sourceFile, id->children.front()->token.startLocation, id->children.back()->token.endLocation, Err(Err0311)};
            const auto       note = Diagnostic::note(Nte(Nte0014));
            return context->report(err, note);
        }

        SWAG_ASSERT(id->extSemantic()->semanticAfterFct == Semantic::resolveVarDeclAfter);
        id->extSemantic()->semanticAfterFct = Semantic::resolveWithVarDeclAfter;
        node->id.push_back(id->token.text);
    }
    else
    {
        SWAG_CHECK(doAffectExpression(node, &id));

        if (id->kind == AstNodeKind::StatementNoScope)
        {
            const Diagnostic err{node->token.sourceFile, id->children.front()->token.startLocation, id->children.back()->token.endLocation, Err(Err0311)};
            const auto       note = Diagnostic::note(Nte(Nte0014));
            return context->report(err, note);
        }

        if (id->kind != AstNodeKind::IdentifierRef &&
            id->kind != AstNodeKind::VarDecl &&
            id->kind != AstNodeKind::AffectOp)
            return error(node->token, Err(Err0174));

        id->allocateExtension(ExtensionKind::Semantic);
        if (id->kind == AstNodeKind::IdentifierRef)
        {
            SWAG_ASSERT(!id->extSemantic()->semanticAfterFct);
            id->extSemantic()->semanticAfterFct = Semantic::resolveWith;
            for (const auto& child : id->children)
                node->id.push_back(child->token.text);
        }
        else if (id->kind == AstNodeKind::VarDecl)
        {
            SWAG_ASSERT(id->extSemantic()->semanticAfterFct == Semantic::resolveVarDeclAfter);
            id->extSemantic()->semanticAfterFct = Semantic::resolveWithVarDeclAfter;
            node->id.push_back(id->token.text);
        }
        else if (id->kind == AstNodeKind::AffectOp)
        {
            id = id->children.front();
            if (id->extSemantic()->semanticAfterFct == Semantic::resolveAfterKnownType)
                id->extSemantic()->semanticAfterFct = Semantic::resolveWithAfterKnownType;
            else
                id->extSemantic()->semanticAfterFct = Semantic::resolveWith;
            for (const auto& child : id->children)
                node->id.push_back(child->token.text);
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    SWAG_CHECK(doScopedStatement(node, node->token, &dummyResult));
    return true;
}

bool Parser::doDefer(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstDefer>(this, AstNodeKind::Defer, sourceFile, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveDefer;

    SWAG_CHECK(eatToken());

    // Defer kind
    if (token.id == TokenId::SymLeftParen)
    {
        const auto startLoc = token.startLocation;
        SWAG_CHECK(eatToken());
        if (token.text == g_LangSpec->name_err)
            node->deferKind = DeferKind::Error;
        else if (token.text == g_LangSpec->name_noerr)
            node->deferKind = DeferKind::NoError;
        else
            return error(token, FMT(Err(Err0157), token.c_str()));

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, "to end the [[defer]] argument"));
    }

    ScopedFlags scopedFlags(this, AST_IN_DEFER);
    SWAG_CHECK(doScopedStatement(node, node->token, &dummyResult, false));
    return true;
}

bool Parser::doIndex(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstNode>(this, AstNodeKind::Index, sourceFile, parent);
    node->semanticFct = Semantic::resolveIndex;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doFallThrough(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstBreakContinue>(this, AstNodeKind::FallThrough, sourceFile, parent);
    node->semanticFct = Semantic::resolveFallThrough;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doUnreachable(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstNode>(this, AstNodeKind::Unreachable, sourceFile, parent);
    node->semanticFct = Semantic::resolveUnreachable;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doBreak(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstBreakContinue>(this, AstNodeKind::Break, sourceFile, parent);
    node->semanticFct = Semantic::resolveBreak;
    *result           = node;
    SWAG_CHECK(eatToken());
    if (token.flags.has(TOKEN_PARSE_LAST_EOL))
        return true;

    if (token.id != TokenId::SymSemiColon)
    {
        SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0155), token.c_str())));
        node->label = static_cast<Token>(token);
        SWAG_CHECK(eatToken());
    }

    return true;
}

bool Parser::doContinue(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstBreakContinue>(this, AstNodeKind::Continue, sourceFile, parent);
    node->semanticFct = Semantic::resolveContinue;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());
    return true;
}

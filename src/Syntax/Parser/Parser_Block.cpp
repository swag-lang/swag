#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool Parser::doIf(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstIf>(AstNodeKind::If, this, parent);
    node->semanticFct = Semantic::resolveIf;
    *result           = node;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, toErr(Err0522)));

    // If with an assignment
    if (tokenParse.is(TokenId::KwdVar) || tokenParse.is(TokenId::KwdConst) || tokenParse.is(TokenId::KwdLet))
    {
        node->addSpecFlag(AstIf::SPEC_FLAG_ASSIGN);

        const auto      newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
        ParserPushScope scoped(this, newScope);

        AstNode* varDecl;
        SWAG_CHECK(doVarDecl(node, &varDecl));

        SWAG_VERIFY(varDecl->childCount() == 1, error(varDecl->lastChild()->token, toErr(Err0398)));

        node->boolExpression = Ast::newIdentifierRef(varDecl->token.text, this, node);
        node->boolExpression->addAstFlag(AST_GENERATED);
        node->boolExpression->inheritTokenLocation(varDecl->token);

        SWAG_CHECK(doScopedStatement(node, node->token, reinterpret_cast<AstNode**>(&node->ifBlock)));

        if (tokenParse.is(TokenId::KwdElse))
        {
            auto tokenElse = tokenParse;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doScopedStatement(node, tokenElse.token, reinterpret_cast<AstNode**>(&node->elseBlock)));
            FormatAst::inheritFormatBefore(this, node->elseBlock, &tokenElse);
        }
        else if (tokenParse.is(TokenId::KwdElif))
        {
            SWAG_CHECK(doIf(node, reinterpret_cast<AstNode**>(&node->elseBlock)));
        }
    }

    // If with a simple expression
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doScopedStatement(node, node->token, reinterpret_cast<AstNode**>(&node->ifBlock)));

        if (tokenParse.is(TokenId::KwdElse))
        {
            auto tokenElse = tokenParse;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doScopedStatement(node, tokenElse.token, reinterpret_cast<AstNode**>(&node->elseBlock)));
            FormatAst::inheritFormatBefore(this, node->elseBlock, &tokenElse);
        }
        else if (tokenParse.is(TokenId::KwdElif))
        {
            SWAG_CHECK(doIf(node, reinterpret_cast<AstNode**>(&node->elseBlock)));
        }
    }

    return true;
}

bool Parser::doWhile(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstWhile>(AstNodeKind::While, this, parent);
    node->semanticFct = Semantic::resolveWhile;
    *result           = node;

    SWAG_CHECK(eatToken());

    {
        ParserPushBreakable scoped(this, node);
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, formErr(Err0527, tokenParse.token.c_str())));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doScopedStatement(node, node->token, &node->block));
    }

    return true;
}

bool Parser::doSwitch(AstNode* parent, AstNode** result)
{
    const auto switchNode   = Ast::newNode<AstSwitch>(AstNodeKind::Switch, this, parent);
    switchNode->semanticFct = Semantic::resolveSwitch;
    *result                 = switchNode;

    // switch can have no expression
    SWAG_CHECK(eatToken());
    if (tokenParse.isNot(TokenId::SymLeftCurly))
    {
        SWAG_CHECK(doExpression(switchNode, EXPR_FLAG_NONE, &switchNode->expression));
        switchNode->expression->allocateExtension(ExtensionKind::Semantic);
        switchNode->expression->extSemantic()->semanticAfterFct = Semantic::resolveSwitchAfterExpr;
    }

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[switch]] body"));

    AstSwitchCase* defaultCase = nullptr;
    bool           hasDefault  = false;
    while (tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::EndOfFile))
    {
        SWAG_VERIFY(tokenParse.is(TokenId::KwdCase) || tokenParse.is(TokenId::KwdDefault), error(tokenParse, toErr(Err0162)));
        const bool isDefault = tokenParse.is(TokenId::KwdDefault);
        SWAG_VERIFY(!isDefault || !hasDefault, error(tokenParse, toErr(Err0007)));
        if (isDefault)
            hasDefault = true;

        // One case
        auto caseNode         = Ast::newNode<AstSwitchCase>(AstNodeKind::SwitchCase, this, isDefault ? nullptr : switchNode);
        caseNode->specFlags   = isDefault ? AstSwitchCase::SPEC_FLAG_IS_DEFAULT : 0;
        caseNode->ownerSwitch = switchNode;
        caseNode->semanticFct = Semantic::resolveCase;
        const auto prevToken  = tokenParse.token;
        SWAG_CHECK(eatToken());
        if (isDefault)
            defaultCase = caseNode;
        else
            switchNode->cases.push_back(caseNode);
        caseNode->caseIndex = switchNode->cases.size() - 1;

        // Case expressions
        if (!isDefault)
        {
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymColon), error(tokenParse, formErr(Err0519, tokenParse.token.c_str())));
            while (tokenParse.isNot(TokenId::SymColon))
            {
                AstNode* expression;
                SWAG_CHECK(doExpression(caseNode, EXPR_FLAG_NONE, &expression));
                if (tokenParse.is(TokenId::KwdTo) || tokenParse.is(TokenId::KwdUntil))
                    SWAG_CHECK(doRange(caseNode, expression, &expression));
                caseNode->expressions.push_back(expression);
                if (tokenParse.isNot(TokenId::SymComma))
                    break;
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(tokenParse.isNot(TokenId::SymColon), error(tokenParse, toErr(Err0112)));
            }
        }

        if (isDefault)
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'default' statement"));
        else
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'case' statement"));

        // Content
        {
            const auto      newScope = Ast::newScope(switchNode, "", ScopeKind::Statement, currentScope);
            ParserPushScope scoped(this, newScope);

            const auto statement = Ast::newNode<AstSwitchCaseBlock>(AstNodeKind::SwitchCaseBlock, this, caseNode);
            statement->allocateExtension(ExtensionKind::Semantic);
            statement->extSemantic()->semanticBeforeFct = Semantic::resolveScopedStmtBefore;
            statement->extSemantic()->semanticAfterFct  = Semantic::resolveScopedStmtAfter;
            statement->ownerCase                        = caseNode;
            caseNode->block                             = statement;
            newScope->owner                             = statement;

            // Instructions
            ParserPushBreakable scopedBreakable(this, switchNode);
            if (tokenParse.is(TokenId::KwdCase) || tokenParse.is(TokenId::KwdDefault))
                return error(prevToken, isDefault ? toErr(Err0072) : toErr(Err0071), toNte(Nte0030));
            if (tokenParse.is(TokenId::SymRightCurly))
                return error(prevToken, isDefault ? toErr(Err0072) : toErr(Err0071), toNte(Nte0029));
            while (tokenParse.isNot(TokenId::KwdCase) && tokenParse.isNot(TokenId::KwdDefault) && tokenParse.isNot(TokenId::SymRightCurly))
                SWAG_CHECK(doEmbeddedInstruction(statement, &dummyResult));
        }
    }

    // Add the default case as the last one
    if (defaultCase)
    {
        Ast::addChildBack(switchNode, defaultCase);
        switchNode->cases.push_back(defaultCase);
    }

    SWAG_CHECK(eatFormat(switchNode));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[switch]] body"));
    return true;
}

bool Parser::doFor(AstNode* parent, AstNode** result)
{
    const auto      newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    ParserPushScope scoped(this, newScope);

    const auto node = Ast::newNode<AstFor>(AstNodeKind::For, this, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveForBefore;
    node->semanticFct                      = Semantic::resolveFor;
    *result                                = node;

    SWAG_CHECK(eatToken());

    ParserPushBreakable scopedBreakable(this, node);

    // Pre statement. Do not call doScopedCurlyStatement in order to avoid
    // creating a new scope in the case of for { var i = 0; var j = 0 } for example
    if (tokenParse.is(TokenId::SymLeftCurly))
        SWAG_CHECK(doCurlyStatement(node, &node->preExpression));
    else
        SWAG_CHECK(doEmbeddedInstruction(node, &node->preExpression));

    // Boolean expression
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
    SWAG_CHECK(eatSemiCol("[[for]] boolean expression"));

    // Post expression
    if (tokenParse.is(TokenId::SymLeftCurly))
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

bool Parser::doWhere(AstNode* node, AstNode** result)
{
    const auto      newScope = Ast::newScope(node, "", ScopeKind::Statement, currentScope);
    ParserPushScope scoped(this, newScope);
    AstNode*        statement = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, node);
    statement->allocateExtension(ExtensionKind::Semantic);
    statement->extSemantic()->semanticBeforeFct = Semantic::resolveScopedStmtBefore;
    statement->extSemantic()->semanticAfterFct  = Semantic::resolveScopedStmtAfter;
    statement->addSpecFlag(AstStatement::SPEC_FLAG_NEED_SCOPE | AstStatement::SPEC_FLAG_WHERE);
    *result         = statement;
    newScope->owner = statement;
    SWAG_CHECK(doIf(statement, &dummyResult));
    return true;
}

bool Parser::doVisit(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstVisit>(AstNodeKind::Visit, this, parent);
    node->semanticFct = Semantic::resolveVisit;
    *result           = node;

    // Eat visit keyword
    SWAG_CHECK(eatToken());

    // Specialized name
    if (tokenParse.is(TokenId::SymColon))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0164)));
        node->extraNameToken = tokenParse.token;
        SWAG_CHECK(eatToken());
    }

    // Reverse loop
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->token.id, mdfFlags, node));
    if (mdfFlags.has(MODIFIER_BACK))
        node->addSpecFlag(AstVisit::SPEC_FLAG_BACK);

    if (tokenParse.is(TokenId::SymAmpersand))
    {
        node->wantPointerToken = tokenParse.token;
        node->specFlags        = AstVisit::SPEC_FLAG_WANT_POINTER;
        SWAG_CHECK(eatToken());
    }

    // Variable to visit
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, formErr(Err0525, tokenParse.token.c_str())));
    SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));

    if (tokenParse.is(TokenId::SymColon) || tokenParse.is(TokenId::SymComma))
    {
        SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a [[visit]] variable name"));
        SWAG_CHECK(checkIsValidVarName(node->expression->lastChild()));
        node->aliasNames.push_back(node->expression->lastChild()->token);
        node->expression->release();
        while (tokenParse.isNot(TokenId::SymColon))
        {
            SWAG_CHECK(eatToken(TokenId::SymComma, "to define another alias name or ':' to specify the visit variable"));
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymColon), error(prevTokenParse.token, toErr(Err0526)));
            SWAG_CHECK(doIdentifierRef(nullptr, &node->expression));
            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a [[visit]] variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->lastChild()));
            node->aliasNames.push_back(node->expression->lastChild()->token);
            node->expression->release();
        }

        SWAG_CHECK(eatToken(TokenId::SymColon, "to define the [[visit]] variable"));
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly), error(tokenParse, toErr(Err0525)));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
    }
    else
    {
        Ast::addChildBack(node, node->expression);
    }

    if (tokenParse.is(TokenId::KwdWhere))
        SWAG_CHECK(doWhere(node, &node->block));
    else
        SWAG_CHECK(doScopedStatement(node, node->token, &node->block));

    // We do not want semantic on the block part, as this has to be solved when the block
    // is inlined
    node->block->addAstFlag(AST_NO_SEMANTIC);

    return true;
}

bool Parser::doLoop(AstNode* parent, AstNode** result)
{
    const auto      newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    ParserPushScope scoped(this, newScope);

    const auto node = Ast::newNode<AstLoop>(AstNodeKind::Loop, this, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveLoopBefore;
    node->semanticFct                      = Semantic::resolveLoop;
    *result                                = node;
    newScope->owner                        = node;

    SWAG_CHECK(eatToken());

    ParserPushBreakable scopedBreakable(this, node);

    // loop can be empty for an infinite loop
    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        SWAG_CHECK(doScopedStatement(node, node->token, &node->block));
        return true;
    }

    // Reverse loop
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->token.id, mdfFlags));
    if (mdfFlags.has(MODIFIER_BACK))
        node->addSpecFlag(AstLoop::SPEC_FLAG_BACK);

    Utf8  name;
    Token tokenName;
    if (tokenParse.is(TokenId::SymLeftParen))
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
    }
    else
    {
        SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));

        tokenName = node->expression->token;
        if (tokenParse.is(TokenId::SymColon))
        {
            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a [[loop]] variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->lastChild()));
            name = node->expression->lastChild()->token.text;
            node->expression->release();
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, formErr(Err0524, tokenParse.token.c_str())));
            SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
        }
        else
        {
            Ast::addChildBack(node, node->expression);

            // Missing ':' ?
            if (node->expression->is(AstNodeKind::IdentifierRef) &&
                node->expression->childCount() == 1 &&
                node->expression->lastChild()->is(AstNodeKind::Identifier) &&
                tokenParse.is(TokenId::LiteralNumber))
            {
                return error(tokenParse, formErr(Err0518, node->expression->lastChild()->token.c_str()));
            }
        }
    }

    // Range
    if (tokenParse.is(TokenId::KwdTo) || tokenParse.is(TokenId::KwdUntil))
    {
        SWAG_CHECK(doRange(node, node->expression, &node->expression));
    }

    // Creates a variable if we have a named index
    if (!name.empty())
    {
        const auto var = Ast::newVarDecl(name, this, node, AstNodeKind::VarDecl);
        var->token     = tokenName;
        var->addSpecFlag(AstVarDecl::SPEC_FLAG_CONST_ASSIGN | AstVarDecl::SPEC_FLAG_LET);
        node->specificName = var;

        const auto identifier   = Ast::newNode<AstNode>(AstNodeKind::Index, this, var);
        identifier->semanticFct = Semantic::resolveIndex;
        identifier->inheritTokenLocation(var->token);

        var->assignment = identifier;
    }

    if (tokenParse.is(TokenId::KwdWhere))
        SWAG_CHECK(doWhere(node, &node->block));
    else
        SWAG_CHECK(doScopedStatement(node, node->token, &node->block));

    return true;
}

bool Parser::doWith(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstWith>(AstNodeKind::With, this, parent);
    *result         = node;
    SWAG_CHECK(eatToken());

    AstNode* id = nullptr;
    if (tokenParse.is(TokenId::KwdVar) || tokenParse.is(TokenId::KwdLet))
    {
        SWAG_CHECK(doVarDecl(node, &id));

        if (id->isNot(AstNodeKind::VarDecl))
        {
            Diagnostic err{id->token.sourceFile, id->firstChild()->token.startLocation, id->lastChild()->token.endLocation, toErr(Err0306)};
            err.addNote(node, node->token, toNte(Nte0015));
            return context->report(err);
        }

        SWAG_ASSERT(id->extSemantic()->semanticAfterFct == Semantic::resolveVarDeclAfter);
        id->extSemantic()->semanticAfterFct = Semantic::resolveWithVarDeclAfter;
        node->id.push_back(id->token.text);
    }
    else
    {
        SWAG_CHECK(doAffectExpression(node, &id));

        if (id->isNot(AstNodeKind::IdentifierRef) &&
            id->isNot(AstNodeKind::VarDecl) &&
            id->isNot(AstNodeKind::AffectOp))
        {
            const Diagnostic err{id, formErr(Err0169, Naming::aKindName(id).c_str())};
            return context->report(err);
        }

        id->allocateExtension(ExtensionKind::Semantic);
        if (id->is(AstNodeKind::IdentifierRef))
        {
            SWAG_ASSERT(!id->extSemantic()->semanticAfterFct);
            id->extSemantic()->semanticAfterFct = Semantic::resolveWith;
            for (const auto& child : id->children)
                node->id.push_back(child->token.text);
        }
        else if (id->is(AstNodeKind::VarDecl))
        {
            SWAG_ASSERT(id->extSemantic()->semanticAfterFct == Semantic::resolveVarDeclAfter);
            id->extSemantic()->semanticAfterFct = Semantic::resolveWithVarDeclAfter;
            node->id.push_back(id->token.text);
        }
        else if (id->is(AstNodeKind::AffectOp))
        {
            id = id->firstChild();
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
    const auto node   = Ast::newNode<AstDefer>(AstNodeKind::Defer, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveDefer;

    SWAG_CHECK(eatToken());

    // Defer kind
    if (tokenParse.is(TokenId::SymColon))
    {
        SWAG_CHECK(eatToken());
        if (tokenParse.is(g_LangSpec->name_err))
        {
            SWAG_CHECK(eatToken());
            node->deferKind = DeferKind::Error;
        }
        else if (tokenParse.is(g_LangSpec->name_noerr))
        {
            SWAG_CHECK(eatToken());
            node->deferKind = DeferKind::NoError;
        }
        else
        {
            return error(tokenParse, formErr(Err0151, tokenParse.token.c_str()));
        }
    }

    ParserPushAstNodeFlags scopedFlags(this, AST_IN_DEFER);
    SWAG_CHECK(doScopedStatement(node, node->token, &dummyResult, false));
    return true;
}

bool Parser::doIndex(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstNode>(AstNodeKind::Index, this, parent);
    node->semanticFct = Semantic::resolveIndex;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doFallThrough(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstBreakContinue>(AstNodeKind::FallThrough, this, parent);
    node->semanticFct = Semantic::resolveFallThrough;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doUnreachable(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstNode>(AstNodeKind::Unreachable, this, parent);
    node->semanticFct = Semantic::resolveUnreachable;
    *result           = node;
    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::doBreak(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstBreakContinue>(AstNodeKind::Break, this, parent);
    node->semanticFct = Semantic::resolveBreak;
    *result           = node;
    SWAG_CHECK(eatToken());
    if (tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
        return true;

    if (tokenParse.isNot(TokenId::SymSemiColon))
    {
        SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0149)));
        node->label = tokenParse.token;
        FormatAst::inheritFormatAfter(this, node, &tokenParse);
        SWAG_CHECK(eatToken());
    }

    return true;
}

bool Parser::doContinue(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstBreakContinue>(AstNodeKind::Continue, this, parent);
    node->semanticFct = Semantic::resolveContinue;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());
    return true;
}

#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool Parser::doWhereIf(AstNode* node, AstNode** result)
{
    const auto newScope = Ast::newScope(node, "", ScopeKind::Statement, currentScope);

    ParserPushScope scoped(this, newScope);
    AstNode*        statement = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, node);
    statement->allocateExtension(ExtensionKind::Semantic);
    statement->extSemantic()->semanticBeforeFct = Semantic::resolveScopedStmtBefore;
    statement->extSemantic()->semanticAfterFct  = Semantic::resolveScopedStmtAfter;
    statement->addSpecFlag(AstStatement::SPEC_FLAG_NEED_SCOPE | AstStatement::SPEC_FLAG_IS_WHERE);
    *result         = statement;
    newScope->owner = statement;

    const auto nodeIf   = Ast::newNode<AstIf>(AstNodeKind::If, this, statement);
    nodeIf->semanticFct = Semantic::resolveIf;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doExpression(nodeIf, EXPR_FLAG_NONE, &nodeIf->boolExpression));
    return true;
}

bool Parser::doWhereVerifyConstraint(AstNode* parent)
{
    auto       what    = AstNodeKind::Invalid;
    const auto tokenId = tokenParse.token.id;
    switch (tokenId)
    {
        case TokenId::KwdWhere:
            what = AstNodeKind::WhereConstraint;
            break;
        case TokenId::KwdVerify:
            what = AstNodeKind::VerifyConstraint;
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    const auto node = Ast::newNode<AstCompilerSpecFunc>(what, this, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;
    node->semanticFct                      = Semantic::resolveWhereVerifyConstraint;
    node->addAstFlag(AST_NO_BYTECODE_CHILDREN);
    parent->addAstFlag(AST_HAS_CONSTRAINTS);

    if (parent->is(AstNodeKind::FuncDecl))
        castAst<AstFuncDecl>(parent, AstNodeKind::FuncDecl)->constraints.push_back(node);
    else
        castAst<AstStruct>(parent, AstNodeKind::StructDecl)->constraints.push_back(node);

    SWAG_CHECK(eatToken());

    // Not for the 3 special functions
    if (parent->token.is(g_LangSpec->name_opDrop) ||
        parent->token.is(g_LangSpec->name_opPostCopy) ||
        parent->token.is(g_LangSpec->name_opPostMove))
    {
        switch (what)
        {
            case AstNodeKind::WhereConstraint:
                return error(node, formErr(Err0357, parent->token.cstr()));
            case AstNodeKind::VerifyConstraint:
                return error(node, formErr(Err0354, parent->token.cstr()));
            default:
                SWAG_ASSERT(false);
                break;
        }
    }

    ParserPushAstNodeFlags scopedFlags(this, AST_IN_RUN_BLOCK | AST_NO_BACKEND | AST_IN_CONSTRAINT);
    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        AstNode* funcNode;
        SWAG_CHECK(doFuncDecl(node, &funcNode, tokenId));

        const auto idRef           = Ast::newIdentifierRef(funcNode->token.text, this, node);
        const auto identifier      = castAst<AstIdentifier>(idRef->lastChild(), AstNodeKind::Identifier);
        identifier->callParameters = Ast::newFuncCallParams(this, identifier);
        idRef->inheritTokenLocation(node->token);
        identifier->inheritTokenLocation(node->token);
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        switch (what)
        {
            case AstNodeKind::WhereConstraint:
                SWAG_CHECK(eatSemiCol("[[where]] expression"));
                break;
            case AstNodeKind::VerifyConstraint:
                SWAG_CHECK(eatSemiCol("[[verify]] expression"));
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }
    }

    return true;
}

bool Parser::doIf(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstIf>(AstNodeKind::If, this, parent);
    node->semanticFct = Semantic::resolveIf;
    *result           = node;

    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, toErr(Err0433)));

    // If with an assignment
    if (tokenParse.is(TokenId::KwdVar) || tokenParse.is(TokenId::KwdConst) || tokenParse.is(TokenId::KwdLet))
    {
        node->addSpecFlag(AstIf::SPEC_FLAG_ASSIGN);

        const auto      newScope = Ast::newScope(node, "", ScopeKind::Statement, currentScope);
        ParserPushScope scoped(this, newScope);

        AstNode* varDecl;
        SWAG_CHECK(doVarDecl(node, &varDecl));

        SWAG_VERIFY(varDecl->childCount() == 1, error(varDecl->lastChild()->token, toErr(Err0275)));

        node->boolExpression = Ast::newIdentifierRef(varDecl->token.text, this, node);
        node->boolExpression->addAstFlag(AST_GENERATED);
        node->boolExpression->firstChild()->inheritTokenLocation(varDecl->token);
        node->boolExpression->inheritTokenLocation(varDecl->token);

        // 'where' clause
        if (tokenParse.is(TokenId::KwdWhere))
        {
            SWAG_CHECK(doWhereIf(node, &node->ifBlock));
            const auto      nodeIf = castAst<AstIf>(node->ifBlock->firstChild(), AstNodeKind::If);
            ParserPushScope scoped1(this, node->ifBlock->ownerScope);
            SWAG_CHECK(doScopedStatement(nodeIf, nodeIf->token, &nodeIf->ifBlock));
        }
        else
            SWAG_CHECK(doScopedStatement(node, node->token, &node->ifBlock));
    }

    // If with a simple expression
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
        SWAG_CHECK(doScopedStatement(node, node->token, &node->ifBlock));
    }

    if (tokenParse.is(TokenId::KwdElse))
    {
        auto tokenElse = tokenParse;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doScopedStatement(node, tokenElse.token, &node->elseBlock));
        FormatAst::inheritFormatBefore(this, node->elseBlock, &tokenElse);
    }
    else if (tokenParse.is(TokenId::KwdElif))
    {
        SWAG_CHECK(doIf(node, &node->elseBlock));
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
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, formErr(Err0437, tokenParse.cstr())));
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

    VectorNative<AstSwitchCase*> defaultCase;
    uint32_t                     cptDefaultNoWhere = 0;

    while (tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::EndOfFile))
    {
        SWAG_VERIFY(tokenParse.is(TokenId::KwdCase) || tokenParse.is(TokenId::KwdDefault), error(tokenParse, toErr(Err0615)));
        const bool isDefault = tokenParse.is(TokenId::KwdDefault);

        // One case
        auto caseNode         = Ast::newNode<AstSwitchCase>(AstNodeKind::SwitchCase, this, isDefault ? nullptr : switchNode);
        caseNode->specFlags   = isDefault ? AstSwitchCase::SPEC_FLAG_IS_DEFAULT : 0;
        caseNode->ownerSwitch = switchNode;
        caseNode->semanticFct = Semantic::resolveCase;
        const auto prevToken  = tokenParse.token;
        SWAG_CHECK(eatToken());

        if (isDefault)
        {
            defaultCase.push_back(caseNode);
            caseNode->caseIndex = switchNode->cases.size() - 1;
        }
        else
        {
            switchNode->cases.push_back(caseNode);
            caseNode->caseIndex = switchNode->cases.size() - 1;

            SWAG_VERIFY(tokenParse.isNot(TokenId::SymColon), error(tokenParse, formErr(Err0429, tokenParse.cstr())));
            while (tokenParse.isNot(TokenId::SymColon) && tokenParse.isNot(TokenId::KwdAs) && tokenParse.isNot(TokenId::KwdWhere))
            {
                AstNode* expression;
                if (tokenParse.is(TokenId::SymLeftCurly) or getNextToken().is(TokenId::KwdAs))
                    SWAG_CHECK(doTypeExpression(caseNode, EXPR_FLAG_NONE, &expression));
                else
                {
                    SWAG_CHECK(doExpression(caseNode, EXPR_FLAG_NONE, &expression));
                    if (tokenParse.is(TokenId::KwdTo) || tokenParse.is(TokenId::KwdUntil))
                        SWAG_CHECK(doRange(caseNode, expression, &expression));
                }

                expression->setBcNotifyAfter(ByteCodeGen::emitSwitchCaseAfterValue);
                caseNode->expressions.push_back(expression);

                if (tokenParse.is(TokenId::SymColon) || tokenParse.is(TokenId::KwdAs) || tokenParse.is(TokenId::KwdWhere))
                    break;
                SWAG_CHECK(eatTokenError(TokenId::SymComma, toErr(Err0094)));
                SWAG_VERIFY(tokenParse.isNot(TokenId::SymColon), error(tokenParse, toErr(Err0095)));
            }
        }

        const auto      newScope = Ast::newScope(switchNode, "", ScopeKind::Statement, currentScope);
        ParserPushScope scoped(this, newScope);

        // Match name
        if (tokenParse.is(TokenId::KwdAs))
        {
            SWAG_VERIFY(switchNode->expression, error(tokenParse, toErr(Err0333)));
            SWAG_VERIFY(caseNode->expressions.size() == 1, error(tokenParse, toErr(Err0332)));

            caseNode->allocateExtension(ExtensionKind::Semantic);
            caseNode->extSemantic()->semanticBeforeFct = Semantic::resolveCaseBefore;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.is(TokenId::Identifier), error(tokenParse, toErr(Err0272)));
            caseNode->matchVarName = tokenParse.token;
            SWAG_CHECK(eatToken());

            const auto varDecl = Ast::newVarDecl(caseNode->matchVarName.text, this, caseNode);
            varDecl->addSpecFlag(AstVarDecl::SPEC_FLAG_LET);
            varDecl->addAstFlag(AST_GENERATED | AST_NO_BYTECODE);

            const auto front = caseNode->expressions.front();
            SWAG_ASSERT(front->is(AstNodeKind::TypeExpression));
            CloneContext cxt;
            cxt.parent    = varDecl;
            varDecl->type = front->clone(cxt);
        }

        // where clause
        if (tokenParse.is(TokenId::KwdWhere))
        {
            SWAG_VERIFY(switchNode->expression, error(tokenParse, toErr(Err0356)));
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doBoolExpression(caseNode, EXPR_FLAG_NONE, &caseNode->whereClause));
        }
        else if (isDefault)
        {
            SWAG_VERIFY(cptDefaultNoWhere == 0, error(caseNode->token, toErr(Err0007)));
            cptDefaultNoWhere++;
        }

        // Content
        ParserPushBreakable scopedBreakable(this, switchNode);

        const auto statement  = Ast::newNode<AstSwitchCaseBlock>(AstNodeKind::SwitchCaseBlock, this, caseNode);
        AstNode*   parentStmt = statement;
        statement->allocateExtension(ExtensionKind::Semantic);
        statement->extSemantic()->semanticBeforeFct = Semantic::resolveScopedStmtBefore;
        statement->extSemantic()->semanticAfterFct  = Semantic::resolveScopedStmtAfter;
        statement->ownerCase                        = caseNode;
        caseNode->block                             = statement;
        newScope->owner                             = statement;

        if (!isDefault)
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'case' statement"));
        else
            SWAG_CHECK(eatToken(TokenId::SymColon, "after the 'default' statement"));

        // Not empty
        if (tokenParse.is(TokenId::KwdCase) || tokenParse.is(TokenId::KwdDefault))
            return error(prevToken, isDefault ? toErr(Err0062) : toErr(Err0061), toNte(Nte0031));
        if (tokenParse.is(TokenId::SymRightCurly))
            return error(prevToken, isDefault ? toErr(Err0062) : toErr(Err0061), toNte(Nte0030));

        while (tokenParse.isNot(TokenId::KwdCase) && tokenParse.isNot(TokenId::KwdDefault) && tokenParse.isNot(TokenId::SymRightCurly))
            SWAG_CHECK(doEmbeddedInstruction(parentStmt, &dummyResult));
    }

    // Add the default case as the last one
    if (!defaultCase.empty())
    {
        for (const auto c : defaultCase)
        {
            Ast::addChildBack(switchNode, c);
            switchNode->cases.push_back(c);
        }
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

    // Pre statement. Do not call doScopedCurlyStatement to avoid
    // creating a new scope in the case of for { var i = 0; var j = 0 } for example
    SWAG_CHECK(doVarDecl(node, &node->preStatement));

    // Boolean expression
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->boolExpression));
    SWAG_CHECK(eatSemiCol("[[for]] boolean expression"));

    // Post statement
    SWAG_CHECK(doEmbeddedInstruction(node, &node->postStatement));

    // For optimisation purposes in the bytecode generation, we must generate postExpression first,
    // then the bool expression. So here we put the bool expression after the post one.
    Ast::removeFromParent(node->boolExpression);
    Ast::addChildBack(node, node->boolExpression);

    SWAG_CHECK(doScopedStatement(node, node->token, &node->block));
    return true;
}

bool Parser::doForEach(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstVisit>(AstNodeKind::ForEach, this, parent);
    node->semanticFct = Semantic::resolveVisit;
    *result           = node;

    // Eat visit keyword
    SWAG_CHECK(eatToken());

    // Specialized name
    if (tokenParse.is(TokenId::SharpIdentifier))
    {
        tokenParse.token.text.remove(0, 1);
        tokenParse.token.id = TokenId::Identifier;
        SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0134)));
        node->extraNameToken = tokenParse.token;
        SWAG_CHECK(eatToken());
    }

    // Reverse loop
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->token.id, mdfFlags, node));
    if (mdfFlags.has(MODIFIER_REVERSE))
        node->addSpecFlag(AstVisit::SPEC_FLAG_REVERSE);

    if (tokenParse.is(TokenId::SymAmpersand))
    {
        node->wantPointerToken = tokenParse.token;
        node->specFlags        = AstVisit::SPEC_FLAG_WANT_POINTER;
        SWAG_CHECK(eatToken());
    }

    // Variable to visit
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, formErr(Err0431, tokenParse.cstr())));
    SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_SIMPLE, &node->expression));

    if (tokenParse.is(TokenId::KwdIn) || tokenParse.is(TokenId::SymComma))
    {
        SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a [[foreach]] variable name"));
        SWAG_CHECK(checkIsValidVarName(node->expression->lastChild()));
        node->aliasNames.push_back(node->expression->lastChild()->token);
        node->expression->release();
        while (tokenParse.isNot(TokenId::KwdIn))
        {
            SWAG_CHECK(eatToken(TokenId::SymComma, "to define another alias name or [[in]] to specify the variable to visit"));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdIn), error(prevTokenParse.token, toErr(Err0432)));
            SWAG_CHECK(doIdentifierRef(nullptr, &node->expression));
            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a [[foreach]] variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->lastChild()));
            node->aliasNames.push_back(node->expression->lastChild()->token);
            node->expression->release();
        }

        SWAG_CHECK(eatToken(TokenId::KwdIn, "to specify the variable to visit"));
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly), error(tokenParse, toErr(Err0431)));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
    }
    else
    {
        Ast::addChildBack(node, node->expression);
    }

    if (tokenParse.is(TokenId::KwdWhere))
    {
        SWAG_CHECK(doWhereIf(node, &node->block));
        const auto      nodeIf = castAst<AstIf>(node->block->firstChild(), AstNodeKind::If);
        ParserPushScope scoped(this, node->block->ownerScope);
        SWAG_CHECK(doScopedStatement(nodeIf, nodeIf->token, &nodeIf->ifBlock));
    }
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
    if (mdfFlags.has(MODIFIER_REVERSE))
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
        if (tokenParse.is(TokenId::KwdIn))
        {
            SWAG_CHECK(checkIsSingleIdentifier(node->expression, "as a [[loop]] variable name"));
            SWAG_CHECK(checkIsValidVarName(node->expression->lastChild()));
            name = node->expression->lastChild()->token.text;
            node->expression->release();
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, formErr(Err0435, tokenParse.cstr())));
            SWAG_CHECK(doExpression(node, EXPR_FLAG_SIMPLE, &node->expression));
        }
        else
        {
            Ast::addChildBack(node, node->expression);

            // Missing ':'?
            if (node->expression->is(AstNodeKind::IdentifierRef) &&
                node->expression->childCount() == 1 &&
                node->expression->lastChild()->is(AstNodeKind::Identifier) &&
                tokenParse.is(TokenId::LiteralNumber))
            {
                return error(tokenParse, formErr(Err0427, node->expression->lastChild()->token.cstr()));
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
        ParserPushFreezeFormat ff(this);

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
    {
        SWAG_CHECK(doWhereIf(node, &node->block));
        const auto      nodeIf = castAst<AstIf>(node->block->firstChild(), AstNodeKind::If);
        ParserPushScope scoped1(this, node->block->ownerScope);
        SWAG_CHECK(doScopedStatement(nodeIf, nodeIf->token, &nodeIf->ifBlock));
    }
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
            const Diagnostic err{id->token.sourceFile, id->firstChild()->token.startLocation, id->lastChild()->token.endLocation, toErr(Err0277)};
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
            const Diagnostic err{id, formErr(Err0616, Naming::aKindName(id).cstr())};
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

    SWAG_CHECK(doCurlyStatement(node, &dummyResult));
    return true;
}

bool Parser::doDefer(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstDefer>(AstNodeKind::Defer, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveDefer;

    SWAG_CHECK(eatToken());

    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->token.id, mdfFlags, node));
    if (mdfFlags.has(MODIFIER_ERR))
        node->deferKind = DeferKind::Error;
    else if (mdfFlags.has(MODIFIER_NO_ERR))
        node->deferKind = DeferKind::NoError;

    ParserPushAstNodeFlags scopedFlags(this, AST_IN_DEFER);
    SWAG_CHECK(doScopedStatement(node, node->token, &dummyResult, false));
    return true;
}

bool Parser::doCompilerIndex(AstNode* parent, AstNode** result)
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

    if (tokenParse.is(TokenId::KwdTo))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0612)));
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

#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scoped.h"
#include "LanguageSpec.h"
#include "Global.h"
#include "SymTable.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Literal, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveLiteral;
    node->token       = move(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doArrayPointerIndex(AstNode** exprNode)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftSquare));
    while (true)
    {
        auto arrayNode         = Ast::newNode(this, &g_Pool_astPointerDeref, AstNodeKind::ArrayPointerIndex, sourceFile->indexInModule);
        arrayNode->token       = token;
        arrayNode->semanticFct = &SemanticJob::resolveArrayPointerIndex;

        Ast::addChildBack(arrayNode, *exprNode);
        arrayNode->array = *exprNode;
        SWAG_CHECK(doExpression(arrayNode, &arrayNode->access));
        *exprNode = arrayNode;
        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    SWAG_VERIFY(token.id != TokenId::SymLeftSquare, syntaxError(token, "invalid token '['"));

    return true;
}

bool SyntaxJob::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(this, &g_Pool_astProperty, AstNodeKind::IntrinsicProp, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveIntrinsicProperty;
    node->inheritTokenName(token);
    node->prop = g_LangSpec.properties[node->name];
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, &node->expression));
    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::SymLeftParen:
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(parent, result));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        break;

    case TokenId::SymLeftCurly:
        SWAG_CHECK(doExpressionListCurly(parent, result));
        break;
    case TokenId::SymLeftSquare:
        SWAG_CHECK(doExpressionListArray(parent, result));
        break;

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(parent, result));
        break;

    case TokenId::KwdMove:
    {
        AstNode* id;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doIdentifierRef(parent, &id));
        if (result)
            *result = id;
        id->flags |= AST_FORCE_MOVE;
    }
    break;

    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::KwdIndex:
        SWAG_CHECK(doIndex(parent, result));
        break;

    case TokenId::Intrinsic:
        if (g_LangSpec.intrinsics[token.text] == Intrinsic::IntrinsicProp)
        {
            SWAG_CHECK(doIntrinsicProp(parent, result));
        }
        else
        {
            SWAG_CHECK(doIdentifierRef(parent, result));
        }
        break;

    case TokenId::NativeType:
    case TokenId::SymAsterisk:
        SWAG_CHECK(doTypeExpression(parent, result));
        break;

    default:
        return syntaxError(token, format("invalid token '%s'", token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::doPrimaryExpression(AstNode* parent, AstNode** result)
{
    AstNode* exprNode;

    // Take pointer
    if (token.id == TokenId::SymAmpersand)
    {
        exprNode = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::MakePointer, sourceFile->indexInModule);
        exprNode->inheritTokenName(token);
        exprNode->semanticFct = &SemanticJob::resolveMakePointer;
        SWAG_CHECK(tokenizer.getToken(token));

        AstNode* identifierRef;
        SWAG_CHECK(doIdentifierRef(nullptr, &identifierRef));
        forceTakeAddress(identifierRef);

        if (token.id == TokenId::SymLeftSquare)
        {
            SWAG_CHECK(doArrayPointerIndex(&identifierRef));
        }

        Ast::addChildBack(exprNode, identifierRef);
        identifierRef->flags |= AST_TAKE_ADDRESS;
    }
    else
    {
        SWAG_CHECK(doSinglePrimaryExpression(nullptr, &exprNode));
    }

    if (parent)
        Ast::addChildBack(parent, exprNode);
    if (result)
        *result = exprNode;
    return true;
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, AstNode** result)
{
    // Cast
    if (token.id == TokenId::KwdCast)
    {
        SWAG_CHECK(doCast(parent, result));
        return true;
    }

    if (token.id == TokenId::SymMinus || token.id == TokenId::SymExclam || token.id == TokenId::SymTilde)
    {
        auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::SingleOp, sourceFile->indexInModule, parent);
        node->semanticFct = &SemanticJob::resolveUnaryOp;
        node->token       = move(token);
        if (result)
            *result = node;
        SWAG_CHECK(tokenizer.getToken(token));
        return doPrimaryExpression(node);
    }

    return doPrimaryExpression(parent, result);
}

bool SyntaxJob::doFactorExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doUnaryExpression(nullptr, &leftNode));

    bool isBinary = false;
    if ((token.id == TokenId::SymPlus) ||
        (token.id == TokenId::SymMinus) ||
        (token.id == TokenId::SymAsterisk) ||
        (token.id == TokenId::SymSlash) ||
        (token.id == TokenId::SymVertical) ||
        (token.id == TokenId::SymAmpersand) ||
        (token.id == TokenId::SymGreaterGreater) ||
        (token.id == TokenId::SymLowerLower) ||
        (token.id == TokenId::SymCircumflex))
    {
        if (parent && parent->kind == AstNodeKind::FactorOp && parent->token.id != token.id)
            return syntaxError(token, "operator order ambiguity, please add parenthesis");

        auto binaryNode = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FactorOp, sourceFile->indexInModule, parent);
        if (token.id == TokenId::SymGreaterGreater || token.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = &SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = &SemanticJob::resolveFactorExpression;
        binaryNode->token = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doCompareExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doFactorExpression(nullptr, &leftNode));

    bool isBinary = false;
    if ((token.id == TokenId::SymEqualEqual) ||
        (token.id == TokenId::SymExclamEqual) ||
        (token.id == TokenId::SymLowerEqual) ||
        (token.id == TokenId::SymGreaterEqual) ||
        (token.id == TokenId::SymLower) ||
        (token.id == TokenId::SymGreater))
    {
        auto binaryNode         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent);
        binaryNode->semanticFct = &SemanticJob::resolveCompareExpression;
        binaryNode->token       = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doBoolExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doCompareExpression(nullptr, &leftNode));

    bool isBinary = false;
    if ((token.id == TokenId::SymVerticalVertical) || (token.id == TokenId::SymAmpersandAmpersand))
    {
        auto binaryNode         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent);
        binaryNode->semanticFct = &SemanticJob::resolveBoolExpression;
        binaryNode->token       = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doCompareExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::CompilerRun)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        AstNode* expr;
        SWAG_CHECK(doBoolExpression(parent, &expr));
        if (result)
            *result = expr;
        expr->semanticAfterFct = &SemanticJob::forceExecuteNode;
        return true;
    }

    return doBoolExpression(parent, result);
}

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doExpression(parent, result);
}

bool SyntaxJob::doExpressionListCurly(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode(this, &g_Pool_astExpressionList, AstNodeKind::ExpressionList, sourceFile->indexInModule, parent);
    initNode->semanticFct = &SemanticJob::resolveExpressionListCurly;
    initNode->listKind    = TypeInfoListKind::Tuple;
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymRightCurly)
        return syntaxError(token, format("initializer list is empty"));
    if (result)
        *result = initNode;

    while (token.id != TokenId::SymRightCurly)
    {
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListCurly(initNode));
        else
        {
            AstNode* paramExpression;
            SWAG_CHECK(doExpression(nullptr, &paramExpression));

            // Name
            if (token.id == TokenId::SymColon)
            {
                if (paramExpression->kind != AstNodeKind::IdentifierRef || paramExpression->childs.size() != 1)
                    return sourceFile->report({sourceFile, paramExpression, format("invalid named value '%s'", token.text.c_str())});
                auto name = paramExpression->childs.front()->name;
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymLeftCurly)
                    SWAG_CHECK(doExpressionListCurly(initNode, &paramExpression));
                else
                    SWAG_CHECK(doExpression(initNode, &paramExpression));
                paramExpression->name = name;
            }
            else
            {
                Ast::addChildBack(initNode, paramExpression);
            }
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doExpressionListArray(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode(this, &g_Pool_astExpressionList, AstNodeKind::ExpressionList, sourceFile->indexInModule, parent);
    initNode->semanticFct = &SemanticJob::resolveExpressionListArray;
    initNode->listKind    = TypeInfoListKind::Array;
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymRightSquare)
        return syntaxError(token, format("initializer list is empty"));
    if (result)
        *result = initNode;

    while (token.id != TokenId::SymRightSquare)
    {
        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode));
        else
            SWAG_CHECK(doExpression(initNode));

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    return true;
}

bool SyntaxJob::doInitializationExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymQuestion)
    {
        parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
        SWAG_CHECK(eatToken(TokenId::SymQuestion));
        return true;
    }

    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doExpressionListCurly(parent, result));
        return true;
    }

    return doExpression(parent, result);
}

bool SyntaxJob::doLeftExpression(AstNode* parent, AstNode** result)
{
    AstNode* exprNode;
    switch (token.id)
    {
    case TokenId::Intrinsic:
        SWAG_CHECK(doIdentifierRef(parent, result));
        return true;

    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(nullptr, &exprNode));
        break;

    default:
        return syntaxError(token, format("invalid token '%s' in left expression", token.text.c_str()));
    }

    Ast::addChildBack(parent, exprNode);
    if (result)
        *result = exprNode;
    return true;
}

void SyntaxJob::forceTakeAddress(AstNode* node)
{
    node->flags |= AST_TAKE_ADDRESS;
    switch (node->kind)
    {
    case AstNodeKind::IdentifierRef:
        forceTakeAddress(node->childs.back());
        break;
    case AstNodeKind::ArrayPointerIndex:
        forceTakeAddress(static_cast<AstPointerDeRef*>(node)->array);
        break;
    }
}

bool SyntaxJob::doDefer(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Defer, sourceFile->indexInModule, parent);
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(doAffectExpression(node, nullptr));

    auto expr = node->childs.front();
    node->ownerScope->deferedNodes.push_back(expr);
    expr->flags |= AST_NO_BYTECODE;

    return true;
}

bool SyntaxJob::doAffectExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpression(nullptr, &leftNode));

    // Variable declaration and initialization
    if (token.id == TokenId::SymColonEqual)
    {
        SWAG_VERIFY(leftNode->kind == AstNodeKind::IdentifierRef, syntaxError(leftNode->token, "identifier expected"));

        AstVarDecl* varNode  = Ast::newNode(this, &g_Pool_astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent);
        varNode->name        = leftNode->childs.back()->name;
        varNode->semanticFct = &SemanticJob::resolveVarDecl;

        if (result)
            *result = varNode;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doInitializationExpression(varNode, &varNode->assignment));
        varNode->flags |= AST_R_VALUE;

        if (!isContextDisabled())
        {
            currentScope->allocateSymTable();
            SWAG_CHECK(currentScope->symTable->registerSymbolNameNoLock(sourceFile, varNode, SymbolKind::Variable));
        }
    }

    // Affect operator
    else if (token.id == TokenId::SymEqual ||
             token.id == TokenId::SymPlusEqual ||
             token.id == TokenId::SymMinusEqual ||
             token.id == TokenId::SymAsteriskEqual ||
             token.id == TokenId::SymSlashEqual ||
             token.id == TokenId::SymAmpersandEqual ||
             token.id == TokenId::SymVerticalEqual ||
             token.id == TokenId::SymCircumflexEqual ||
             token.id == TokenId::SymTildeEqual ||
             token.id == TokenId::SymPercentEqual ||
             token.id == TokenId::SymLowerLowerEqual ||
             token.id == TokenId::SymGreaterGreaterEqual)
    {
        auto affectNode         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::AffectOp, sourceFile->indexInModule, parent);
        affectNode->semanticFct = &SemanticJob::resolveAffect;
        affectNode->token       = move(token);

        Ast::addChildBack(affectNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(affectNode, &leftNode));

        if (result)
            *result = affectNode;

        auto left = affectNode->childs.front();
        forceTakeAddress(left);
    }
    else
    {
        Ast::addChildBack(parent, leftNode);
        if (result)
            *result = leftNode;
    }

    SWAG_CHECK(eatSemiCol("after left expression"));
    return true;
}

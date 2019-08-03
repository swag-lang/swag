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
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::Literal, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
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
        auto arrayNode = Ast::newNode(&g_Pool_astPointerDeref, AstNodeKind::ArrayPointerIndex, sourceFile->indexInModule);
        arrayNode->inheritOwnersAndFlags(this);
        arrayNode->token       = token;
        arrayNode->semanticFct = &SemanticJob::resolveArrayPointerIndex;

        Ast::addChild(arrayNode, *exprNode);
        arrayNode->array = *exprNode;
        SWAG_CHECK(doExpression(arrayNode, &arrayNode->access));
        *exprNode = arrayNode;
        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    return true;
}

bool SyntaxJob::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astProperty, AstNodeKind::IntrinsicProp, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveIntrinsicProperty;
    node->token       = move(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, &node->expression));
    SWAG_CHECK(eatToken(TokenId::SymComma));

    AstNode* identifier;
    SWAG_CHECK(doIdentifierRef(nullptr, &identifier));
    const auto& name = identifier->childs.back()->name;
    auto        it   = g_LangSpec.properties.find(name);
    if (it == g_LangSpec.properties.end())
        return syntaxError(token, format("invalid property '%s'", name.c_str()));

    node->prop = it->second;
    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::SymLeftParen:
    {
        auto saveLocation = token.startLocation;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(parent, result));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        break;
    }

    case TokenId::SymLeftCurly:
    case TokenId::SymLeftSquare:
        SWAG_CHECK(doExpressionList(parent, result));
        break;

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(parent, result));
        break;

    case TokenId::Identifier:
    case TokenId::IntrinsicPrint:
    case TokenId::IntrinsicAssert:
	case TokenId::IntrinsicAlloc:
	case TokenId::IntrinsicFree:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::KwdIndex:
        SWAG_CHECK(doIndex(parent, result));
        break;

    case TokenId::IntrinsicProp:
        SWAG_CHECK(doIntrinsicProp(parent, result));
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
        exprNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::MakePointer, sourceFile->indexInModule);
        exprNode->inheritOwnersAndFlags(this);
        exprNode->inheritToken(token);
        exprNode->semanticFct = &SemanticJob::resolveMakePointer;
        SWAG_CHECK(tokenizer.getToken(token));

        AstNode* identifierRef;
        SWAG_CHECK(doIdentifierRef(nullptr, &identifierRef));
        forceTakeAddress(identifierRef);

        if (token.id == TokenId::SymLeftSquare)
        {
            SWAG_CHECK(doArrayPointerIndex(&identifierRef));
        }

        Ast::addChild(exprNode, identifierRef);
        identifierRef->flags |= AST_L_VALUE | AST_TAKE_ADDRESS;
    }
    else
    {
        SWAG_CHECK(doSinglePrimaryExpression(nullptr, &exprNode));
        if (token.id == TokenId::SymLeftSquare)
        {
            SWAG_CHECK(doArrayPointerIndex(&exprNode));
        }
    }

    if (parent)
        Ast::addChild(parent, exprNode);
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
        auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::SingleOp, sourceFile->indexInModule, parent);
        node->inheritOwnersAndFlags(this);
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

        auto binaryNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::FactorOp, sourceFile->indexInModule, parent);
        binaryNode->inheritOwnersAndFlags(this);
        if (token.id == TokenId::SymGreaterGreater || token.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = &SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = &SemanticJob::resolveFactorExpression;
        binaryNode->token = move(token);

        Ast::addChild(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChild(parent, leftNode);
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
        auto binaryNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent);
        binaryNode->inheritOwnersAndFlags(this);
        binaryNode->semanticFct = &SemanticJob::resolveCompareExpression;
        binaryNode->token       = move(token);

        Ast::addChild(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChild(parent, leftNode);
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
        auto binaryNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::BinaryOp, sourceFile->indexInModule, parent);
        binaryNode->inheritOwnersAndFlags(this);
        binaryNode->semanticFct = &SemanticJob::resolveBoolExpression;
        binaryNode->token       = move(token);

        Ast::addChild(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doCompareExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChild(parent, leftNode);
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

bool SyntaxJob::doExpressionList(AstNode* parent, AstNode** result)
{
    auto leftId  = token.id;
    auto rightId = token.id == TokenId::SymLeftCurly ? TokenId::SymRightCurly : TokenId::SymRightSquare;

    auto initNode = Ast::newNode(&g_Pool_astExpressionList, AstNodeKind::ExpressionList, sourceFile->indexInModule, parent);
    initNode->inheritOwnersAndFlags(this);
    initNode->semanticFct = &SemanticJob::resolveExpressionList;
    initNode->inheritToken(token);
    SWAG_CHECK(tokenizer.getToken(token));

    if (leftId == TokenId::SymLeftCurly)
        initNode->listKind = TypeInfoListKind::Tuple;
    else
        initNode->listKind = TypeInfoListKind::Array;

    if (token.id == rightId)
        return syntaxError(token, format("initializer list is empty"));
    if (result)
        *result = initNode;

    while (token.id != rightId)
    {
        if (token.id == leftId)
        {
            SWAG_CHECK(doInitializationExpression(initNode));
        }
        else
        {
            SWAG_CHECK(doExpression(initNode));
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    SWAG_CHECK(eatToken(rightId));
    return true;
}

bool SyntaxJob::doInitializationExpression(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymQuestion)
    {
        parent->flags |= AST_DISABLED_INIT;
        SWAG_CHECK(eatToken(TokenId::SymQuestion));
        return true;
    }

    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doExpressionList(parent, result));
        return true;
    }

    return doExpression(parent, result);
}

bool SyntaxJob::doLeftExpression(AstNode* parent, AstNode** result)
{
    AstNode* exprNode;
    switch (token.id)
    {
    case TokenId::IntrinsicPrint:
    case TokenId::IntrinsicAssert:
	case TokenId::IntrinsicAlloc:
	case TokenId::IntrinsicFree:
        SWAG_CHECK(doIdentifierRef(parent, result));
        return true;

    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(nullptr, &exprNode));
        break;

    default:
        return syntaxError(token, format("invalid token '%s' in left expression", token.text.c_str()));
    }

    // Dereference pointer
    if (token.id == TokenId::SymLeftSquare)
    {
        SWAG_CHECK(doArrayPointerIndex(&exprNode));
    }

    Ast::addChild(parent, exprNode);
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
        node->childs.back()->flags |= AST_TAKE_ADDRESS;
        break;
    case AstNodeKind::ArrayPointerIndex:
        forceTakeAddress(static_cast<AstPointerDeRef*>(node)->array);
        break;
    }
}

bool SyntaxJob::doAffectExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpression(nullptr, &leftNode));

    // Variable declaration and initialization
    if (token.id == TokenId::SymColonEqual)
    {
        SWAG_VERIFY(leftNode->kind == AstNodeKind::IdentifierRef, syntaxError(leftNode->token, "identifier expected"));

        AstVarDecl* varNode = Ast::newNode(&g_Pool_astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent);
        varNode->inheritOwnersAndFlags(this);
        varNode->inheritToken(leftNode->childs.back()->token);
        varNode->semanticFct = &SemanticJob::resolveVarDecl;
        if (result)
            *result = varNode;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doInitializationExpression(varNode, &varNode->astAssignment));
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
             token.id == TokenId::SymLowerLowerEqual ||
             token.id == TokenId::SymGreaterGreaterEqual)
    {
        auto affectNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::AffectOp, sourceFile->indexInModule, parent);
        affectNode->inheritOwnersAndFlags(this);
        affectNode->semanticFct = &SemanticJob::resolveAffect;
        affectNode->token       = move(token);

        Ast::addChild(affectNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doExpression(affectNode, &leftNode));

        if (result)
            *result = affectNode;

        auto left = affectNode->childs.front();
        forceTakeAddress(left);
    }
    else
    {
        Ast::addChild(parent, leftNode);
        if (result)
            *result = leftNode;
    }

    SWAG_CHECK(eatSemiCol("after affect expression"));
    return true;
}

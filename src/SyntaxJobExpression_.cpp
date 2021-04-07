#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Module.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Literal, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveLiteral;
    node->token       = move(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::doArrayPointerIndex(AstNode** exprNode)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftSquare));

    AstNode* firstExpr = nullptr;
    SWAG_CHECK(doExpression(nullptr, &firstExpr));

    // Slicing
    if (token.id == TokenId::SymDotDot)
    {
        SWAG_CHECK(eatToken(TokenId::SymDotDot));
        auto arrayNode         = Ast::newNode<AstArrayPointerSlicing>(this, AstNodeKind::ArrayPointerSlicing, sourceFile, nullptr, 3);
        arrayNode->token       = firstExpr->token;
        arrayNode->semanticFct = SemanticJob::resolveArrayPointerSlicing;
        arrayNode->array       = *exprNode;
        Ast::addChildBack(arrayNode, *exprNode);
        Ast::addChildBack(arrayNode, firstExpr);
        arrayNode->lowerBound = firstExpr;
        SWAG_CHECK(doExpression(arrayNode, &arrayNode->upperBound));
        *exprNode = arrayNode;
    }

    // Deref by index
    else
    {
        while (true)
        {
            auto arrayNode         = Ast::newNode<AstArrayPointerIndex>(this, AstNodeKind::ArrayPointerIndex, sourceFile, nullptr, 2);
            arrayNode->token       = firstExpr ? firstExpr->token : token;
            arrayNode->semanticFct = SemanticJob::resolveArrayPointerIndex;

            Ast::addChildBack(arrayNode, *exprNode);
            arrayNode->array = *exprNode;
            if (firstExpr)
            {
                arrayNode->access = firstExpr;
                Ast::addChildBack(arrayNode, firstExpr);
                firstExpr = nullptr;
            }
            else
            {
                SWAG_CHECK(doExpression(arrayNode, &arrayNode->access));
            }

            *exprNode = arrayNode;
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken(TokenId::SymComma));
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    SWAG_VERIFY(token.id != TokenId::SymLeftSquare, syntaxError(token, "invalid token '['"));

    return true;
}

bool SyntaxJob::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstIntrinsicProp>(this, AstNodeKind::IntrinsicProp, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveIntrinsicProperty;
    node->inheritTokenName(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(verifyError(token, token.id != TokenId::SymRightParen, "intrinsic parameter expression cannot be empty"));

    // Three parameters
    if (node->token.id == TokenId::IntrinsicMakeInterface)
    {
        AstNode* params = Ast::newFuncCallParams(sourceFile, node, this);
        SWAG_CHECK(doExpression(params));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(params));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(params));
    }

    // Two parameters
    else if (node->token.id == TokenId::IntrinsicMakeSlice ||
             node->token.id == TokenId::IntrinsicMakeString ||
             node->token.id == TokenId::IntrinsicMakeAny)
    {
        SWAG_CHECK(doExpression(node));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(node));
    }

    // Two parameters
    else if (node->token.id == TokenId::IntrinsicMakeForeign)
    {
        SWAG_CHECK(doTypeExpression(node));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(node));
    }

    // One single parameter
    else
    {
        SWAG_CHECK(doExpression(node));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::CompilerFunction:
    case TokenId::CompilerCallerFunction:
    case TokenId::CompilerCallerLocation:
    case TokenId::CompilerBuildCfg:
    case TokenId::CompilerArch:
    case TokenId::CompilerOs:
    case TokenId::CompilerAbi:
        SWAG_CHECK(doCompilerSpecialFunction(parent, result));
        break;

    case TokenId::CompilerLocation:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::CompilerDefined:
        SWAG_CHECK(doCompilerDefined(parent, result));
        break;

    case TokenId::CompilerLoad:
        SWAG_CHECK(doCompilerLoad(parent, result));
        break;

    case TokenId::CompilerHasTag:
    case TokenId::CompilerTagVal:
        SWAG_CHECK(doCompilerTag(parent, result));
        break;

    case TokenId::SymLeftParen:
    {
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(verifyError(token, token.id != TokenId::SymRightParen, "expression is empty"));
        AstNode* expr;
        SWAG_CHECK(doExpression(parent, &expr));
        expr->flags |= AST_IN_ATOMIC_EXPR;
        if (result)
            *result = expr;
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        break;
    }

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(parent, result));
        break;

    case TokenId::KwdTry:
    case TokenId::KwdAssume:
        SWAG_CHECK(doTryAssume(parent, result));
        break;
    case TokenId::KwdCatch:
        SWAG_CHECK(doCatch(parent, result));
        break;
    case TokenId::SymBackTick:
    case TokenId::Identifier:
    case TokenId::CompilerScopeFct:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;
    case TokenId::SymDot:
    {
        SWAG_CHECK(eatToken());
        AstNode* idref;
        SWAG_CHECK(doIdentifierRef(parent, &idref));
        if (result)
            *result = idref;
        ((AstIdentifierRef*) idref)->autoScope = true;
        break;
    }

    case TokenId::IntrinsicIndex:
        SWAG_CHECK(doIndex(parent, result));
        break;

    case TokenId::IntrinsicGetErr:
        SWAG_CHECK(doGetErr(parent, result));
        break;

    case TokenId::IntrinsicSpread:
    case TokenId::IntrinsicSizeOf:
    case TokenId::IntrinsicAlignOf:
    case TokenId::IntrinsicOffsetOf:
    case TokenId::IntrinsicKindOf:
    case TokenId::IntrinsicCountOf:
    case TokenId::IntrinsicDataOf:
    case TokenId::IntrinsicStringOf:
    case TokenId::IntrinsicMakeAny:
    case TokenId::IntrinsicMakeSlice:
    case TokenId::IntrinsicMakeString:
    case TokenId::IntrinsicMakeCallback:
    case TokenId::IntrinsicMakeForeign:
    case TokenId::IntrinsicMakeInterface:
    case TokenId::IntrinsicAlloc:
    case TokenId::IntrinsicRealloc:
    case TokenId::IntrinsicGetContext:
    case TokenId::IntrinsicArguments:
    case TokenId::IntrinsicCompiler:
    case TokenId::IntrinsicIsByteCode:
    case TokenId::IntrinsicMemCmp:
    case TokenId::IntrinsicCStrLen:
    case TokenId::IntrinsicStrCmp:
    case TokenId::IntrinsicTypeCmp:
    case TokenId::IntrinsicAtomicAdd:
    case TokenId::IntrinsicAtomicAnd:
    case TokenId::IntrinsicAtomicOr:
    case TokenId::IntrinsicAtomicXor:
    case TokenId::IntrinsicAtomicXchg:
    case TokenId::IntrinsicAtomicCmpXchg:
    case TokenId::IntrinsicIsConstExpr:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::IntrinsicTypeOf:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::IntrinsicSqrt:
    case TokenId::IntrinsicSin:
    case TokenId::IntrinsicCos:
    case TokenId::IntrinsicTan:
    case TokenId::IntrinsicSinh:
    case TokenId::IntrinsicCosh:
    case TokenId::IntrinsicTanh:
    case TokenId::IntrinsicASin:
    case TokenId::IntrinsicACos:
    case TokenId::IntrinsicATan:
    case TokenId::IntrinsicLog:
    case TokenId::IntrinsicLog2:
    case TokenId::IntrinsicLog10:
    case TokenId::IntrinsicFloor:
    case TokenId::IntrinsicCeil:
    case TokenId::IntrinsicTrunc:
    case TokenId::IntrinsicRound:
    case TokenId::IntrinsicAbs:
    case TokenId::IntrinsicExp:
    case TokenId::IntrinsicExp2:
    case TokenId::IntrinsicPow:
    case TokenId::IntrinsicMin:
    case TokenId::IntrinsicMax:
    case TokenId::IntrinsicBitCountNz:
    case TokenId::IntrinsicBitCountTz:
    case TokenId::IntrinsicBitCountLz:
    case TokenId::IntrinsicByteSwap:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::KwdConst:
    case TokenId::KwdCode:
    case TokenId::KwdFunc:
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::NativeType:
    case TokenId::SymAsterisk:
    case TokenId::SymLeftSquare:
    case TokenId::SymLeftCurly:
        SWAG_CHECK(doTypeExpression(parent, result));
        break;

    case TokenId::SymLiteralCurly:
        SWAG_CHECK(eatToken(TokenId::SymLiteralCurly));
        SWAG_CHECK(doExpressionListTuple(parent, result));
        break;

    case TokenId::SymLiteralBracket:
        SWAG_CHECK(eatToken(TokenId::SymLiteralBracket));
        SWAG_CHECK(doExpressionListArray(parent, result));
        break;

    case TokenId::SymLiteralParen:
        SWAG_CHECK(doLambdaExpression(parent, result));
        break;

    default:
        return invalidTokenError(InvalidTokenError::PrimaryExpression);
    }

    return true;
}

bool SyntaxJob::doDeRef(AstNode* parent, AstNode** result)
{
    auto identifierRef     = Ast::newIdentifierRef(sourceFile, parent, this);
    auto arrayNode         = Ast::newNode<AstArrayPointerIndex>(this, AstNodeKind::ArrayPointerIndex, sourceFile, identifierRef, 2);
    arrayNode->semanticFct = SemanticJob::resolveArrayPointerIndex;
    arrayNode->isDeref     = true;
    Token savedToken       = token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doUnaryExpression(arrayNode, &arrayNode->array));

    auto literal                   = Ast::newNode<AstNode>(this, AstNodeKind::Literal, sourceFile, arrayNode);
    literal->computedValue.reg.u64 = 0;
    literal->token.literalType     = LiteralType::TT_S32;
    literal->setFlagsValueIsComputed();
    literal->semanticFct = SemanticJob::resolveLiteral;
    literal->inheritTokenLocation(savedToken);
    arrayNode->access = literal;

    if (result)
        *result = identifierRef;
    return true;
}

bool SyntaxJob::doPrimaryExpression(AstNode* parent, AstNode** result)
{
    AstNode* exprNode;

    // Take pointer
    if (token.id == TokenId::SymAmpersand)
    {
        exprNode              = Ast::newNode<AstNode>(this, AstNodeKind::MakePointer, sourceFile, nullptr);
        exprNode->semanticFct = SemanticJob::resolveMakePointer;
        SWAG_CHECK(eatToken());

        AstNode* identifierRef;
        SWAG_CHECK(doIdentifierRef(nullptr, &identifierRef));
        forceTakeAddress(identifierRef);

        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doArrayPointerIndex(&identifierRef));

        Ast::addChildBack(exprNode, identifierRef);
        identifierRef->flags |= AST_TAKE_ADDRESS;
    }

    // Dereference pointer
    else if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(doDeRef(parent, &exprNode));
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
    switch (token.id)
    {
    case TokenId::KwdCast:
        SWAG_CHECK(doCast(parent, result));
        return true;
    case TokenId::KwdAutoCast:
        SWAG_CHECK(doAutoCast(parent, result));
        return true;
    case TokenId::KwdBitCast:
        SWAG_CHECK(doBitCast(parent, result));
        return true;
    case TokenId::SymMinus:
    case TokenId::SymExclam:
    case TokenId::SymTilde:
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::SingleOp, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveUnaryOp;
        node->token       = move(token);
        if (result)
            *result = node;
        SWAG_CHECK(tokenizer.getToken(token));
        return doPrimaryExpression(node);
    }
    }

    return doPrimaryExpression(parent, result);
}

static int getPrecedence(TokenId id)
{
    switch (id)
    {
    case TokenId::SymTilde:
        return 0;
    case TokenId::SymAsterisk:
    case TokenId::SymAsteriskPercent:
    case TokenId::SymSlash:
    case TokenId::SymPercent:
        return 1;
    case TokenId::SymPlus:
    case TokenId::SymPlusPercent:
    case TokenId::SymMinus:
    case TokenId::SymMinusPercent:
        return 2;
    case TokenId::SymGreaterGreater:
    case TokenId::SymGreaterGreaterPercent:
    case TokenId::SymLowerLower:
    case TokenId::SymLowerLowerPercent:
        return 3;
    case TokenId::SymAmpersand:
        return 4;
    case TokenId::SymVertical:
        return 5;
    case TokenId::SymCircumflex:
        return 6;
    case TokenId::SymLowerEqualGreater:
        return 7;
    case TokenId::SymEqualEqual:
    case TokenId::SymExclamEqual:
        return 8;
    case TokenId::SymLower:
    case TokenId::SymLowerEqual:
    case TokenId::SymGreater:
    case TokenId::SymGreaterEqual:
        return 9;
    }

    return -1;
}

static bool isAssociative(TokenId id)
{
    switch (id)
    {
    case TokenId::SymPlus:
    case TokenId::SymPlusPercent:
    case TokenId::SymAsterisk:
    case TokenId::SymAsteriskPercent:
    case TokenId::SymVertical:
    case TokenId::SymCircumflex:
    case TokenId::SymTilde:
        return true;
    }

    return false;
}

bool SyntaxJob::doOperatorPrecedence(AstNode** result)
{
    auto factor = *result;
    if (factor->kind != AstNodeKind::FactorOp && factor->kind != AstNodeKind::BinaryOp)
        return true;

    auto left = factor->childs[0];
    SWAG_CHECK(doOperatorPrecedence(&left));
    auto right = factor->childs[1];
    SWAG_CHECK(doOperatorPrecedence(&right));

    if ((right->kind == AstNodeKind::FactorOp || right->kind == AstNodeKind::BinaryOp) && !(right->flags & AST_IN_ATOMIC_EXPR))
    {
        auto myPrecedence    = getPrecedence(factor->token.id);
        auto rightPrecedence = getPrecedence(right->token.id);

        bool shuffle = false;
        if (myPrecedence < rightPrecedence && myPrecedence != -1 && rightPrecedence != -1)
            shuffle = true;

        // If operation is not associative, then we need to shuffle
        //
        // 2 - 1 - 1 needs to be treated as (2 - 1) - 1 and not 2 - (2 - 1)
        //
        else if (!isAssociative(factor->token.id) && (myPrecedence == rightPrecedence))
            shuffle = true;

        if (shuffle)
        {
            //   *
            //  / \
            // A   +
            //    / \
            //   B   C

            //     +
            //    / \
            //   *   C
            //  / \  
            // A   B

            auto leftRight = right->childs[0];
            Ast::removeFromParent(right);
            if (factor->childParentIdx == 0)
                Ast::addChildFront(factor->parent, right);
            else
                Ast::addChildBack(factor->parent, right);
            Ast::removeFromParent(leftRight);

            Ast::removeFromParent(factor);
            Ast::addChildBack(factor, leftRight);

            Ast::addChildFront(right, factor);
            SWAG_CHECK(doOperatorPrecedence(&right));

            factor = right; // new root
        }
    }

    *result = factor;
    return true;
}

bool SyntaxJob::doFactorExpression(AstNode** parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doUnaryExpression(nullptr, &leftNode));

    bool isBinary = false;
    if ((token.id == TokenId::SymPlus) ||
        (token.id == TokenId::SymPlusPercent) ||
        (token.id == TokenId::SymMinus) ||
        (token.id == TokenId::SymMinusPercent) ||
        (token.id == TokenId::SymAsterisk) ||
        (token.id == TokenId::SymAsteriskPercent) ||
        (token.id == TokenId::SymSlash) ||
        (token.id == TokenId::SymPercent) ||
        (token.id == TokenId::SymAmpersand) ||
        (token.id == TokenId::SymVertical) ||
        (token.id == TokenId::SymGreaterGreater) ||
        (token.id == TokenId::SymGreaterGreaterPercent) ||
        (token.id == TokenId::SymLowerLower) ||
        (token.id == TokenId::SymLowerLowerPercent) ||
        (token.id == TokenId::SymTilde) ||
        (token.id == TokenId::SymCircumflex))
    {
        auto binaryNode = Ast::newNode<AstNode>(this, AstNodeKind::FactorOp, sourceFile, parent ? *parent : nullptr, 2);
        if (token.id == TokenId::SymGreaterGreater ||
            token.id == TokenId::SymGreaterGreaterPercent ||
            token.id == TokenId::SymLowerLower ||
            token.id == TokenId::SymLowerLowerPercent)
            binaryNode->semanticFct = SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = SemanticJob::resolveFactorExpression;
        binaryNode->token = move(token);

        switch (binaryNode->token.id)
        {
        case TokenId::SymGreaterGreaterPercent:
        case TokenId::SymLowerLowerPercent:
        case TokenId::SymPlusPercent:
        case TokenId::SymMinusPercent:
        case TokenId::SymAsteriskPercent:
            binaryNode->attributeFlags |= ATTRIBUTE_SAFETY_OFF_OPERATOR;
            break;
        }

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(&binaryNode));
        SWAG_CHECK(doOperatorPrecedence(&binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }
    else if ((token.id == TokenId::SymEqualEqual) ||
             (token.id == TokenId::SymExclamEqual) ||
             (token.id == TokenId::SymLowerEqual) ||
             (token.id == TokenId::SymGreaterEqual) ||
             (token.id == TokenId::SymLower) ||
             (token.id == TokenId::SymGreater) ||
             (token.id == TokenId::SymLowerEqualGreater))
    {
        auto binaryNode         = Ast::newNode<AstNode>(this, AstNodeKind::BinaryOp, sourceFile, parent ? *parent : nullptr, 2);
        binaryNode->semanticFct = SemanticJob::resolveCompareExpression;
        binaryNode->token       = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFactorExpression(&binaryNode));
        SWAG_CHECK(doOperatorPrecedence(&binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary && parent)
        Ast::addChildBack(*parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doCompareExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doFactorExpression(nullptr, &leftNode));
    SWAG_CHECK(doOperatorPrecedence(&leftNode));
    SWAG_VERIFY(token.id != TokenId::SymEqual, syntaxError(token, "invalid compare operator '=', did you mean '==' ?"));
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
        auto binaryNode         = Ast::newNode<AstBinaryOpNode>(this, AstNodeKind::BinaryOp, sourceFile, parent, 2);
        binaryNode->semanticFct = SemanticJob::resolveBoolExpression;
        binaryNode->token       = move(token);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doBoolExpression(binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doMoveExpression(AstNode* parent, AstNode** result)
{
    // nodrop left
    if (token.id == TokenId::KwdNoDrop)
    {
        auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::NoDrop, sourceFile, parent);
        if (result)
            *result = exprNode;
        exprNode->semanticFct = SemanticJob::resolveMove;
        exprNode->flags |= AST_NO_LEFT_DROP;
        parent = exprNode;
        result = nullptr;
        SWAG_CHECK(eatToken());
    }

    // move
    if (token.id == TokenId::KwdMove)
    {
        auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::Move, sourceFile, parent);
        if (result)
            *result = exprNode;
        exprNode->semanticFct = SemanticJob::resolveMove;
        exprNode->flags |= AST_FORCE_MOVE;
        parent = exprNode;
        result = nullptr;
        SWAG_CHECK(eatToken());

        // nodrop right
        if (token.id == TokenId::KwdNoDrop)
        {
            exprNode = Ast::newNode<AstNode>(this, AstNodeKind::NoDrop, sourceFile, parent);
            if (result)
                *result = exprNode;
            exprNode->semanticFct = SemanticJob::resolveMove;
            exprNode->flags |= AST_NO_RIGHT_DROP;
            parent = exprNode;
            result = nullptr;
            SWAG_CHECK(eatToken());
        }
    }

    SWAG_CHECK(doExpression(parent, result));
    return true;
}

bool SyntaxJob::doExpression(AstNode* parent, AstNode** result)
{
    AstNode* boolExpression = nullptr;
    switch (token.id)
    {
    case TokenId::CompilerRun:
    {
        SWAG_CHECK(eatToken());
        boolExpression              = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerRun, sourceFile, nullptr);
        boolExpression->semanticFct = SemanticJob::resolveCompilerRun;
        SWAG_CHECK(doBoolExpression(boolExpression));
        break;
    }
    case TokenId::CompilerMixin:
    {
        SWAG_CHECK(eatToken());
        boolExpression              = Ast::newNode<AstCompilerMixin>(nullptr, AstNodeKind::CompilerMixin, sourceFile, nullptr);
        boolExpression->semanticFct = SemanticJob::resolveCompilerMixin;
        SWAG_CHECK(doExpression(boolExpression));
        break;
    }
    case TokenId::CompilerCode:
    {
        SWAG_CHECK(eatToken());
        AstNode* block;
        boolExpression = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerCode, sourceFile, nullptr);
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doEmbeddedStatement(boolExpression, &block));
        else
            SWAG_CHECK(doBoolExpression(boolExpression, &block));
        auto typeCode     = allocType<TypeInfoCode>();
        typeCode->content = boolExpression->childs.front();
        typeCode->content->flags |= AST_NO_SEMANTIC;
        boolExpression->typeInfo = typeCode;
        boolExpression->flags |= AST_NO_BYTECODE;
        break;
    }

    default:
        SWAG_CHECK(doBoolExpression(nullptr, &boolExpression));
        break;
    }

    // A ? B : C
    if (token.id == TokenId::SymQuestion)
    {
        SWAG_CHECK(eatToken());
        auto triNode         = Ast::newNode<AstConditionalOpNode>(this, AstNodeKind::ConditionalExpression, sourceFile, parent, 3);
        triNode->semanticFct = SemanticJob::resolveConditionalOp;
        if (result)
            *result = triNode;
        Ast::addChildBack(triNode, boolExpression);

        SWAG_CHECK(doExpression(triNode));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doExpression(triNode));
    }

    // A ?? B
    else if (token.id == TokenId::SymQuestionQuestion)
    {
        SWAG_CHECK(eatToken());
        auto triNode         = Ast::newNode<AstNode>(this, AstNodeKind::NullConditionalExpression, sourceFile, parent, 2);
        triNode->semanticFct = SemanticJob::resolveNullConditionalOp;
        if (result)
            *result = triNode;
        Ast::addChildBack(triNode, boolExpression);
        SWAG_CHECK(doExpression(triNode));
    }
    else
    {
        Ast::addChildBack(parent, boolExpression);
        if (result)
            *result = boolExpression;
    }

    return true;
}

bool SyntaxJob::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doExpression(parent, result);
}

bool SyntaxJob::doExpressionListTuple(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, parent);
    initNode->semanticFct = SemanticJob::resolveExpressionListTuple;
    initNode->forTuple    = true;
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymRightCurly)
        return syntaxError(token, format("initializer list is empty"));
    if (result)
        *result = initNode;

    while (token.id != TokenId::SymRightCurly)
    {
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListTuple(initNode));
        else if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode));
        else
        {
            AstNode* paramExpression;
            SWAG_CHECK(doExpression(nullptr, &paramExpression));

            // Name
            if (token.id == TokenId::SymColon)
            {
                SWAG_VERIFY(paramExpression->kind == AstNodeKind::IdentifierRef, syntaxError(paramExpression, "identifier expected"));
                SWAG_CHECK(checkIsSingleIdentifier(paramExpression, "as a tuple field name"));
                SWAG_CHECK(checkIsValidVarName(paramExpression->childs.back()));
                auto name            = paramExpression->childs.back()->token.text;
                auto namedExpression = paramExpression->childs.back();
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymLeftCurly)
                    SWAG_CHECK(doExpressionListTuple(initNode, &paramExpression));
                else
                    SWAG_CHECK(doExpression(initNode, &paramExpression));
                paramExpression->token.startLocation = namedExpression->token.startLocation;
                paramExpression->token.text          = name;
                paramExpression->flags |= AST_IS_NAMED;
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
    auto initNode         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, parent);
    initNode->semanticFct = SemanticJob::resolveExpressionListArray;
    initNode->forTuple    = false;
    SWAG_CHECK(tokenizer.getToken(token));

    if (token.id == TokenId::SymRightSquare)
        return syntaxError(token, format("initializer list is empty"));
    if (result)
        *result = initNode;

    while (token.id != TokenId::SymRightSquare)
    {
        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode));
        else if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListTuple(initNode));
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
    // var x = ? : not initialized
    if (token.id == TokenId::SymQuestion)
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::ExplicitNoInit, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveExplicitNoInit;
        if (parent)
            parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
        if (result)
            *result = node;
        SWAG_CHECK(eatToken());
        return true;
    }

    return doMoveExpression(parent, result);
}

void SyntaxJob::forceTakeAddress(AstNode* node)
{
    node->flags |= AST_TAKE_ADDRESS;
    if (node->resolvedSymbolOverload)
        node->resolvedSymbolOverload->flags |= OVERLOAD_CAN_CHANGE;
    switch (node->kind)
    {
    case AstNodeKind::IdentifierRef:
        forceTakeAddress(node->childs.back());
        break;
    case AstNodeKind::ArrayPointerIndex:
        forceTakeAddress(static_cast<AstArrayPointerIndex*>(node)->array);
        break;
    }
}

bool SyntaxJob::doDefer(AstNode* parent, AstNode** result)
{
    auto nodeKind = token.id == TokenId::KwdDefer ? AstNodeKind::Defer : AstNodeKind::ErrDefer;
    auto node     = Ast::newNode<AstNode>(this, nodeKind, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveDefer;

    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymLeftCurly)
        SWAG_CHECK(doScopedCurlyStatement(node));
    else
        SWAG_CHECK(doAffectExpression(node));

    return true;
}

bool SyntaxJob::doLeftExpressionVar(AstNode** result, uint32_t identifierFlags)
{
    switch (token.id)
    {
    case TokenId::SymLeftParen:
    {
        auto multi = Ast::newNode<AstNode>(this, AstNodeKind::MultiIdentifierTuple, sourceFile, nullptr);
        *result    = multi;
        SWAG_CHECK(eatToken());
        while (true)
        {
            SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::SymQuestion, syntaxError(token, "variable name or '?' expected in tuple unpacking"));
            SWAG_CHECK(doIdentifierRef(multi, nullptr, identifierFlags));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen, "after tuple unpacking"));
        break;
    }

    case TokenId::Identifier:
    case TokenId::SymBackTick:
    case TokenId::CompilerScopeFct:
    {
        AstNode* exprNode = nullptr;
        AstNode* multi    = nullptr;
        while (true)
        {
            SWAG_CHECK(doIdentifierRef(multi, &exprNode, identifierFlags));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());

            if (!multi)
            {
                multi = Ast::newNode<AstNode>(this, AstNodeKind::MultiIdentifier, sourceFile, nullptr);
                Ast::addChildBack(multi, exprNode);
            }
        }

        *result = multi ? multi : exprNode;
        break;
    }

    default:
        return invalidTokenError(InvalidTokenError::LeftExpressionVar);
    }

    return true;
}

bool SyntaxJob::doLeftExpression(AstNode** result)
{
    switch (token.id)
    {
    case TokenId::IntrinsicPrint:
    case TokenId::IntrinsicAssert:
    case TokenId::IntrinsicDbgBreak:
    case TokenId::IntrinsicPanic:
    case TokenId::IntrinsicErrorMsg:
    case TokenId::IntrinsicFree:
    case TokenId::IntrinsicMemCpy:
    case TokenId::IntrinsicMemMove:
    case TokenId::IntrinsicMemSet:
    case TokenId::IntrinsicSetContext:
    case TokenId::IntrinsicGetContext:
    case TokenId::IntrinsicAtomicAdd:
    case TokenId::IntrinsicAtomicAnd:
    case TokenId::IntrinsicAtomicOr:
    case TokenId::IntrinsicAtomicXor:
    case TokenId::IntrinsicAtomicXchg:
    case TokenId::IntrinsicAtomicCmpXchg:
    case TokenId::IntrinsicSetErr:
        SWAG_CHECK(doIdentifierRef(nullptr, result));
        return true;
    case TokenId::SymLeftParen:
    case TokenId::Identifier:
    case TokenId::SymBackTick:
        SWAG_CHECK(doLeftExpressionVar(result));
        return true;

    case TokenId::KwdTry:
    case TokenId::KwdAssume:
        SWAG_CHECK(doTryAssume(nullptr, result));
        return true;
    case TokenId::KwdThrow:
        SWAG_CHECK(doThrow(nullptr, result));
        return true;
    case TokenId::KwdCatch:
        SWAG_CHECK(doCatch(nullptr, result));
        return true;

    case TokenId::SymColon:
        SWAG_CHECK(doDeRef(nullptr, result));
        return true;

    default:
        return invalidTokenError(InvalidTokenError::LeftExpression);
    }
}

bool SyntaxJob::checkIsValidUserName(AstNode* node)
{
    if (node->parent && (node->parent->flags & AST_GENERATED))
        return true;
    if (node->flags & AST_GENERATED)
        return true;

    // An identifier that starts with '__' is reserved for internal usage !
    if (!sourceFile->generated && !sourceFile->isBootstrapFile && !sourceFile->isRuntimeFile)
    {
        if (node->token.text.length() > 1 && node->token.text[0] == '_' && node->token.text[1] == '_')
            return syntaxError(node->token, format("identifier '%s' starts with '__', and this is reserved by the language", node->token.text.c_str()));
    }

    return true;
}

bool SyntaxJob::checkIsValidVarName(AstNode* node)
{
    if (!checkIsValidUserName(node))
        return false;

    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->genericParameters)
            return syntaxError(identifier->genericParameters, "variable name cannot have generic parameters");
        if (identifier->callParameters)
            return syntaxError(identifier->callParameters, "variable name cannot have call parameters");
    }

    if (node->token.text[0] != '@')
        return true;

    // @alias must be of the form @aliasNUM
    if (node->token.text.length() >= 6)
    {
        if (node->token.text == "@alias")
            return syntaxError(node->token, "'@alias' variable name must be followed by a number");

        if (node->token.text.find("@alias") == 0)
        {
            const char* pz = node->token.text.c_str() + 6;
            while (*pz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return syntaxError(node->token, format("invalid '@alias' variable name '%s', '%s' is not a valid number", node->token.text.c_str(), node->token.text.c_str() + 6));
                pz++;
            }

            return true;
        }
    }

    return syntaxError(node->token, format("invalid variable name '%s', cannot start with '@'", node->token.text.c_str()));
}

bool SyntaxJob::doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, AstNodeKind kind, AstNode** result)
{
    bool acceptDeref = true;
    if (currentScope->kind == ScopeKind::Struct || currentScope->kind == ScopeKind::File)
        acceptDeref = false;

    // Multiple affectation
    if (leftNode->kind == AstNodeKind::MultiIdentifier)
    {
        auto parentNode = parent;
        if (acceptDeref)
        {
            parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;
        }

        // Declare first variable, and affect it
        auto front = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);

        // Then declare all other variables, and assign them to the first one
        bool firstDone = false;
        for (auto child : leftNode->childs)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            ScopedLocation scopedLoc(this, &identifier->token);
            AstVarDecl*    varNode = Ast::newVarDecl(sourceFile, identifier->token.text, parentNode, this);
            varNode->kind          = kind;
            varNode->token         = identifier->token;
            varNode->flags |= AST_R_VALUE;

            if (!firstDone)
            {
                firstDone = true;
                Ast::addChildBack(varNode, type);
                Ast::addChildBack(varNode, assign);
                varNode->type       = type;
                varNode->assignment = assign;
            }

            // We are supposed to be constexpr, so we need to duplicate the assignment instead of generating an
            // affectation to the first variable. If there's no assignment, then just declare the variable
            // without the affectation to avoid a useless copy (and a postCopy in case of structs)
            else if (!acceptDeref || assign == nullptr)
            {
                varNode->type = Ast::clone(type, varNode);
                if (assign)
                    varNode->assignment = Ast::clone(assign, varNode);
            }
            else
            {
                ScopedFlags lk(this, AST_GENERATED);
                varNode->assignment = Ast::newIdentifierRef(sourceFile, front->token.text, varNode, this);
            }

            SemanticJob::setVarDeclResolve(varNode);

            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));
        }
    }

    // Tuple dereference
    else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
    {
        SWAG_VERIFY(acceptDeref, error(leftNode->token, format("cannot decompose a tuple in %s", Scope::getArticleKindName(currentScope->kind))));

        auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::StatementNoScope, sourceFile, parent);
        if (result)
            *result = parentNode;

        // Generate an expression of the form "var __tmp_0 = assignment"
        ScopedLocation scopedLoc(this, &leftNode->childs.front()->token);
        auto           tmpVarName = format("__tmp_%d", g_Global.uniqueID.fetch_add(1));
        AstVarDecl*    orgVarNode = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
        orgVarNode->kind          = kind;

        orgVarNode->token.startLocation = leftNode->childs.front()->token.startLocation;
        orgVarNode->token.endLocation   = leftNode->childs.back()->token.endLocation;
        orgVarNode->allocateExtension();
        orgVarNode->extension->semanticAfterFct = SemanticJob::resolveTupleUnpackBefore;

        // This will avoid to initialize the tuple before the affectation
        orgVarNode->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
        orgVarNode->flags |= AST_R_VALUE;

        Ast::addChildBack(orgVarNode, type);
        orgVarNode->type = type;
        Ast::addChildBack(orgVarNode, assign);
        orgVarNode->assignment = assign;
        orgVarNode->assignment->flags |= AST_NO_LEFT_DROP;
        SemanticJob::setVarDeclResolve(orgVarNode);

        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, orgVarNode, SymbolKind::Variable));

        // And reference that variable, in the form value = __tmp_0.item?
        orgVarNode->publicName = "(";

        int idx = 0;
        for (auto child : leftNode->childs)
        {
            // Ignore field if '?', otherwise check that this is a valid variable name
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            if (child->childs.front()->token.text == "?")
            {
                idx++;
                continue;
            }

            SWAG_CHECK(checkIsValidUserName(child));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            if (idx)
                orgVarNode->publicName += ", ";
            orgVarNode->publicName += identifier->token.text;

            ScopedLocation scopedLoc1(this, &identifier->token);
            auto           varNode = Ast::newVarDecl(sourceFile, identifier->token.text, parentNode, this);
            varNode->kind          = kind;
            varNode->token         = identifier->token;
            varNode->flags |= AST_R_VALUE | AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS;
            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));
            identifier          = Ast::newIdentifierRef(sourceFile, format("%s.item%d", tmpVarName.c_str(), idx++), varNode, this);
            varNode->assignment = identifier;
            SemanticJob::setVarDeclResolve(varNode);
            varNode->assignment->flags |= AST_TUPLE_UNPACK;
        }

        orgVarNode->publicName += ")";
    }

    // Single declaration/affectation
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(leftNode, "as a variable name"));
        auto identifier = leftNode->childs.back();
        SWAG_CHECK(checkIsValidVarName(identifier));
        AstVarDecl* varNode = Ast::newVarDecl(sourceFile, identifier->token.text, parent, this);
        varNode->kind       = kind;
        varNode->inheritTokenLocation(leftNode->token);

        if (result)
            *result = varNode;
        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        SemanticJob::setVarDeclResolve(varNode);
        varNode->flags |= AST_R_VALUE;

        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));
    }

    return true;
}

bool SyntaxJob::doAffectExpression(AstNode* parent, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpression(&leftNode));

    // Variable declaration and initialization by ':='
    if (token.id == TokenId::SymColonEqual)
    {
        AstNode* assign;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doInitializationExpression(nullptr, &assign));
        SWAG_CHECK(doVarDeclExpression(parent, leftNode, nullptr, assign, AstNodeKind::VarDecl, result));
    }

    // Affect operator
    else if (token.id == TokenId::SymEqual ||
             token.id == TokenId::SymPlusEqual ||
             token.id == TokenId::SymPlusPercentEqual ||
             token.id == TokenId::SymMinusEqual ||
             token.id == TokenId::SymMinusPercentEqual ||
             token.id == TokenId::SymAsteriskEqual ||
             token.id == TokenId::SymAsteriskPercentEqual ||
             token.id == TokenId::SymSlashEqual ||
             token.id == TokenId::SymAmpersandEqual ||
             token.id == TokenId::SymVerticalEqual ||
             token.id == TokenId::SymCircumflexEqual ||
             token.id == TokenId::SymPercentEqual ||
             token.id == TokenId::SymLowerLowerEqual ||
             token.id == TokenId::SymLowerLowerPercentEqual ||
             token.id == TokenId::SymGreaterGreaterEqual ||
             token.id == TokenId::SymGreaterGreaterPercentEqual)
    {
        // Multiple affectation
        // like in a, b, c = 0
        if (leftNode->kind == AstNodeKind::MultiIdentifier)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            ScopedLocation lk(this, &token);

            // Generate an expression of the form "var firstVar = assignment", and "secondvar = firstvar" for the rest
            // This avoid to do the right expression multiple times (if this is a function call for example).
            //
            // If this is not '=' operator, then we have to duplicate the affectation for each variable
            AstNode* affectExpression = nullptr;
            bool     firstDone        = false;
            auto     savedtoken       = token;
            auto     front            = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);
            front->computeName();
            while (!leftNode->childs.empty())
            {
                auto child        = leftNode->childs.front();
                auto affectNode   = Ast::newAffectOp(sourceFile, parentNode);
                affectNode->token = savedtoken;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);

                // First create 'firstVar = assignment'
                if (!firstDone)
                {
                    firstDone = true;
                    SWAG_CHECK(tokenizer.getToken(token));
                    if (affectNode->token.id == TokenId::SymEqual)
                        SWAG_CHECK(doMoveExpression(affectNode, &affectExpression));
                    else
                        SWAG_CHECK(doExpression(affectNode, &affectExpression));
                }

                // This is not an initialization, so we need to duplicate the right expression
                else if (affectNode->token.id != TokenId::SymEqual)
                {
                    auto newAffect = Ast::clone(affectExpression, affectNode);
                    newAffect->inheritTokenLocation(affectExpression->token);
                }

                // In case of an affectation, create 'otherVar = firstVar'
                else
                {
                    Ast::newIdentifierRef(sourceFile, front->token.text, affectNode, this)->token = savedtoken;
                }
            }
        }

        // Tuple unpacking
        else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            ScopedLocation lk(this, &token);

            auto savedtoken = token;
            SWAG_CHECK(tokenizer.getToken(token));

            // Get right side
            AstNode* assignment;
            SWAG_CHECK(doExpression(nullptr, &assignment));

            // If right side is an expression list, then simply convert to a bunch of affectations without
            // dealing with a tuple...
            if (assignment->kind == AstNodeKind::ExpressionList)
            {
                SWAG_VERIFY(leftNode->childs.size() == assignment->childs.size(), error(assignment, format("unpacking '%d' variable(s), but the tuple on the right contains '%d' element(s)", leftNode->childs.size(), assignment->childs.size())));
                while (!leftNode->childs.empty())
                {
                    auto child             = leftNode->childs.front();
                    auto affectNode        = Ast::newAffectOp(sourceFile, parentNode);
                    affectNode->token.id   = savedtoken.id;
                    affectNode->token.text = savedtoken.text;
                    Ast::removeFromParent(child);
                    Ast::addChildBack(affectNode, child);
                    forceTakeAddress(child);
                    auto assign = assignment->childs[0];
                    Ast::removeFromParent(assign);
                    Ast::addChildBack(affectNode, assign);
                    leftNode->flags |= AST_NO_SEMANTIC;
                    assignment->flags |= AST_NO_SEMANTIC;
                }
            }
            else
            {
                // Generate an expression of the form "var __tmp_0 = assignment"
                auto        tmpVarName = format("__tmp_%d", g_Global.uniqueID.fetch_add(1));
                AstVarDecl* varNode    = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
                varNode->flags |= AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS;
                Ast::addChildBack(varNode, assignment);
                assignment->inheritOwners(varNode);
                varNode->assignment = assignment;
                varNode->assignment->flags |= AST_NO_LEFT_DROP;

                varNode->token.startLocation = leftNode->childs.front()->token.startLocation;
                varNode->token.endLocation   = leftNode->childs.back()->token.endLocation;
                varNode->allocateExtension();
                varNode->extension->semanticAfterFct = SemanticJob::resolveTupleUnpackBefore;

                // And reference that variable, in the form value = __tmp_0.item?
                int idx = 0;
                while (!leftNode->childs.empty())
                {
                    auto child = leftNode->childs.front();

                    // Ignore field if '?', otherwise check that this is a valid variable name
                    if (child->childs.front()->token.text == "?")
                    {
                        idx++;
                        Ast::removeFromParent(child);
                        Ast::releaseNode(child);
                        continue;
                    }

                    auto affectNode        = Ast::newAffectOp(sourceFile, parentNode);
                    affectNode->token.id   = savedtoken.id;
                    affectNode->token.text = savedtoken.text;
                    Ast::removeFromParent(child);
                    Ast::addChildBack(affectNode, child);
                    forceTakeAddress(child);
                    auto idRef = Ast::newIdentifierRef(sourceFile, format("%s.item%d", tmpVarName.c_str(), idx++), affectNode, this);

                    // Force a move between the generated temporary variable and the real var
                    idRef->flags |= AST_FORCE_MOVE;
                }
            }
        }

        // One normal simple affectation
        else
        {
            auto affectNode   = Ast::newAffectOp(sourceFile, parent, this);
            affectNode->token = move(token);

            Ast::addChildBack(affectNode, leftNode);
            forceTakeAddress(leftNode);

            SWAG_CHECK(tokenizer.getToken(token));
            if (affectNode->token.id == TokenId::SymEqual)
                SWAG_CHECK(doMoveExpression(affectNode));
            else
                SWAG_CHECK(doExpression(affectNode));
            if (result)
                *result = affectNode;
        }
    }
    else
    {
        Ast::addChildBack(parent, leftNode);
        if (result)
            *result = leftNode;
    }

    SWAG_VERIFY(token.id != TokenId::SymEqualEqual, syntaxError(token, "invalid affect operator '==', did you mean '=' ?"));

    if (token.id != TokenId::SymLeftCurly)
        SWAG_CHECK(eatSemiCol("after left expression"));
    return true;
}

bool SyntaxJob::doInit(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstInit>(this, AstNodeKind::Init, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveInit;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, &node->expression));

    if (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, &node->count));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));

    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        SWAG_CHECK(doFuncCallParameters(node, &node->parameters, TokenId::SymRightParen));
    }

    return true;
}

bool SyntaxJob::doDropCopyMove(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstDropCopyMove>(this, AstNodeKind::Drop, sourceFile, parent);
    switch (token.id)
    {
    case TokenId::IntrinsicDrop:
        node->token.text = Utf8("@drop");
        break;
    case TokenId::IntrinsicPostCopy:
        node->token.text = Utf8("@postCopy");
        node->kind       = AstNodeKind::PostCopy;
        break;
    case TokenId::IntrinsicPostMove:
        node->token.text = Utf8("@postMove");
        node->kind       = AstNodeKind::PostMove;
        break;
    }

    node->semanticFct = SemanticJob::resolveDropCopyMove;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, &node->expression));

    if (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, &node->count));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doReloc(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstReloc>(this, AstNodeKind::Reloc, sourceFile, parent);

    node->semanticFct = SemanticJob::resolveReloc;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, &node->expression1));
    SWAG_CHECK(eatToken(TokenId::SymComma));
    SWAG_CHECK(doExpression(node, &node->expression2));

    if (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, &node->count));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

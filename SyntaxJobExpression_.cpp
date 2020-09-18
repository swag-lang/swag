#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "TypeManager.h"
#include "Scoped.h"

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
    else if (node->token.id == TokenId::IntrinsicMakeSlice || node->token.id == TokenId::IntrinsicMakeAny)
    {
        SWAG_CHECK(doExpression(node));
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
    case TokenId::CompilerLocation:
    case TokenId::CompilerBuildCfg:
    case TokenId::CompilerArch:
    case TokenId::CompilerOs:
        SWAG_CHECK(doCompilerSpecialFunction(parent, result));
        break;

    case TokenId::CompilerHasTag:
    case TokenId::CompilerTagVal:
        SWAG_CHECK(doCompilerTag(parent, result));
        break;

    case TokenId::SymLeftParen:
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(verifyError(token, token.id != TokenId::SymRightParen, "expression is empty"));
        SWAG_CHECK(doExpression(parent, result));
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
        break;

    case TokenId::LiteralNumber:
    case TokenId::LiteralCharacter:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(parent, result));
        break;

    case TokenId::SymBackTick:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::IntrinsicIndex:
        SWAG_CHECK(doIndex(parent, result));
        break;

    case TokenId::IntrinsicSizeOf:
    case TokenId::IntrinsicKindOf:
    case TokenId::IntrinsicCountOf:
    case TokenId::IntrinsicDataOf:
    case TokenId::IntrinsicMakeAny:
    case TokenId::IntrinsicMakeSlice:
    case TokenId::IntrinsicMakeInterface:
    case TokenId::IntrinsicAlloc:
    case TokenId::IntrinsicRealloc:
    case TokenId::IntrinsicMemCmp:
    case TokenId::IntrinsicGetContext:
    case TokenId::IntrinsicArguments:
    case TokenId::IntrinsicCompiler:
    case TokenId::IntrinsicIsByteCode:
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
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::KwdConst:
    case TokenId::KwdCode:
    case TokenId::KwdFunc:
    case TokenId::KwdStruct:
    case TokenId::NativeType:
    case TokenId::SymAsterisk:
    case TokenId::SymLeftSquare:
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
    SWAG_CHECK(eatToken());
    auto identifierRef     = Ast::newIdentifierRef(sourceFile, parent, this);
    auto arrayNode         = Ast::newNode<AstArrayPointerIndex>(this, AstNodeKind::ArrayPointerIndex, sourceFile, identifierRef, 2);
    arrayNode->semanticFct = SemanticJob::resolveArrayPointerIndex;
    arrayNode->isDeref     = true;
    SWAG_CHECK(doUnaryExpression(arrayNode, &arrayNode->array));

    auto literal                   = Ast::newNode<AstNode>(this, AstNodeKind::Literal, sourceFile, arrayNode);
    literal->computedValue.reg.u64 = 0;
    literal->token.literalType     = LiteralType::TT_S32;
    literal->setFlagsValueIsComputed();
    literal->semanticFct = SemanticJob::resolveLiteral;
    arrayNode->access    = literal;

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
    else if (token.id == TokenId::KwdDeRef)
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
    // Cast
    if (token.id == TokenId::KwdCast)
    {
        SWAG_CHECK(doCast(parent, result));
        return true;
    }

    // Cast
    if (token.id == TokenId::KwdAutoCast)
    {
        SWAG_CHECK(doAutoCast(parent, result));
        return true;
    }

    if (token.id == TokenId::SymMinus || token.id == TokenId::SymExclam || token.id == TokenId::SymTilde)
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::SingleOp, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveUnaryOp;
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
        (token.id == TokenId::SymPercent) ||
        (token.id == TokenId::SymTilde) ||
        (token.id == TokenId::SymCircumflex))
    {
        if (parent && parent->kind == AstNodeKind::FactorOp)
        {
            if (parent->token.id != token.id)
                return syntaxError(token, "operator order ambiguity, please add parenthesis");

            // Associative operations
            if (token.id != TokenId::SymPlus &&
                token.id != TokenId::SymAsterisk &&
                token.id != TokenId::SymVertical &&
                token.id != TokenId::SymCircumflex &&
                token.id != TokenId::SymTilde)
                return syntaxError(token, "operator order ambiguity, please add parenthesis");
        }

        auto binaryNode = Ast::newNode<AstNode>(this, AstNodeKind::FactorOp, sourceFile, parent, 2);
        if (token.id == TokenId::SymGreaterGreater || token.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = SemanticJob::resolveFactorExpression;
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

    if ((token.id == TokenId::SymEqualEqual) ||
        (token.id == TokenId::SymExclamEqual) ||
        (token.id == TokenId::SymLowerEqual) ||
        (token.id == TokenId::SymLowerEqualGreater) ||
        (token.id == TokenId::SymGreaterEqual) ||
        (token.id == TokenId::SymLower) ||
        (token.id == TokenId::SymGreater))
    {
        AstNode* leftBinary = nullptr;
        while ((token.id == TokenId::SymEqualEqual) ||
               (token.id == TokenId::SymExclamEqual) ||
               (token.id == TokenId::SymLowerEqual) ||
               (token.id == TokenId::SymLowerEqualGreater) ||
               (token.id == TokenId::SymGreaterEqual) ||
               (token.id == TokenId::SymLower) ||
               (token.id == TokenId::SymGreater))
        {
            auto binaryNode         = Ast::newNode<AstNode>(this, AstNodeKind::BinaryOp, sourceFile, parent, 2);
            binaryNode->semanticFct = SemanticJob::resolveCompareExpression;
            binaryNode->token       = move(token);

            // Need to duplicate the left node to replace for example '? <= A <= ?' by '(? <= A) && (A <= N)'
            if (leftBinary)
                leftNode = Ast::clone(leftNode, nullptr);

            Ast::addChildBack(binaryNode, leftNode);
            SWAG_CHECK(tokenizer.getToken(token));
            AstNode* rightNode;
            SWAG_CHECK(doFactorExpression(binaryNode, &rightNode));

            if (leftBinary)
            {
                auto andNode         = Ast::newNode<AstBinaryOpNode>(this, AstNodeKind::BinaryOp, sourceFile, parent, 2);
                andNode->semanticFct = SemanticJob::resolveBoolExpression;
                andNode->token       = rightNode->token;
                andNode->token.id    = TokenId::SymAmpersandAmpersand;
                andNode->token.text  = "&&";
                Ast::removeFromParent(leftBinary);
                Ast::addChildBack(andNode, leftBinary);
                Ast::addChildBack(andNode, binaryNode);
                leftBinary = andNode;
                if (result)
                    *result = andNode;
            }
            else
            {
                leftBinary = binaryNode;
                if (result)
                    *result = binaryNode;
            }

            leftNode = rightNode;
        }

        return true;
    }

    if (token.id == TokenId::SymEqual)
    {
        return syntaxError(token, "invalid compare operator '=', do you mean '==' ?");
    }

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
        auto typeCode     = g_Allocator.alloc<TypeInfoCode>();
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
        auto triNode         = Ast::newNode<AstNode>(this, AstNodeKind::ConditionalExpression, sourceFile, parent, 3);
        triNode->semanticFct = SemanticJob::resolveCondtionalOp;
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
        triNode->semanticFct = SemanticJob::resolveNullCondtionalOp;
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
        else
        {
            AstNode* paramExpression;
            SWAG_CHECK(doExpression(nullptr, &paramExpression));

            // Name
            if (token.id == TokenId::SymColon)
            {
                if (paramExpression->kind != AstNodeKind::IdentifierRef || paramExpression->childs.size() != 1)
                    return sourceFile->report({paramExpression, format("invalid named value '%s'", token.text.c_str())});
                auto name = paramExpression->childs.front()->name;
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymLeftCurly)
                    SWAG_CHECK(doExpressionListTuple(initNode, &paramExpression));
                else
                    SWAG_CHECK(doExpression(initNode, &paramExpression));
                paramExpression->name = name;
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

    initNode->childs.back()->token.endLocation = token.endLocation;
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
        else
            SWAG_CHECK(doExpression(initNode));

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    initNode->childs.back()->token.endLocation = token.endLocation;
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
        SWAG_CHECK(eatToken(TokenId::SymQuestion));
        return true;
    }

    return doMoveExpression(parent, result);
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
        forceTakeAddress(static_cast<AstArrayPointerIndex*>(node)->array);
        break;
    }
}

bool SyntaxJob::doDefer(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Defer, sourceFile, parent);
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymLeftCurly)
        SWAG_CHECK(doScopedCurlyStatement(node, nullptr));
    else
        SWAG_CHECK(doAffectExpression(node, nullptr));

    auto expr = node->childs.front();
    node->ownerScope->deferredNodes.push_back(expr);
    expr->flags |= AST_NO_BYTECODE;

    return true;
}

bool SyntaxJob::doLeftExpressionVar(AstNode** result, bool acceptParameters)
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
            SWAG_CHECK(doIdentifierRef(multi, nullptr, acceptParameters));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen, "after tuple affectation"));
        break;
    }

    case TokenId::Identifier:
    case TokenId::SymBackTick:
    {
        AstNode* exprNode = nullptr;
        AstNode* multi    = nullptr;
        while (true)
        {
            SWAG_CHECK(doIdentifierRef(multi, &exprNode, acceptParameters));
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
    case TokenId::IntrinsicError:
    case TokenId::IntrinsicFree:
    case TokenId::IntrinsicMemCpy:
    case TokenId::IntrinsicMemSet:
    case TokenId::IntrinsicSetContext:
    case TokenId::IntrinsicGetContext:
        SWAG_CHECK(doIdentifierRef(nullptr, result));
        return true;
    case TokenId::SymLeftParen:
    case TokenId::Identifier:
    case TokenId::SymBackTick:
        SWAG_CHECK(doLeftExpressionVar(result));
        return true;

    case TokenId::KwdDeRef:
        SWAG_CHECK(doDeRef(nullptr, result));
        return true;

    default:
        return invalidTokenError(InvalidTokenError::LeftExpression);
    }
}

bool SyntaxJob::isValidUserName(AstNode* node)
{
    // An identifier that starts with '__' is reserved for internal usage !
    if (!sourceFile->generated && !(node->parent->flags & AST_GENERATED))
    {
        if (node->name.length() > 1 && node->name[0] == '_' && node->name[1] == '_')
            return syntaxError(node->token, format("identifier '%s' starts with '__', and this is reserved by the language", node->name.c_str()));
    }

    return true;
}

bool SyntaxJob::isValidVarName(AstNode* node)
{
    if (!isValidUserName(node))
        return false;

    if (node->name[0] != '@')
        return true;

    // @alias must be of the form @aliasNUM
    if (node->name.length() >= 6)
    {
        if (node->name == "@alias")
            return syntaxError(node->token, "@alias variable name must be followed by a number");

        if (node->name.find("@alias") == 0)
        {
            const char* pz = node->name.c_str() + 6;
            while (*pz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return syntaxError(node->token, format("invalid @alias variable name '%s', '%s' is not a valid number", node->name.c_str(), node->name.c_str() + 6));
                pz++;
            }

            return true;
        }
    }

    return syntaxError(node->token, format("invalid variable name '%s', cannot start with '@'", node->name.c_str()));
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
            SWAG_CHECK(checkIsSingleIdentifier(child));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(isValidVarName(identifier));

            ScopedLocation scopedLoc(this, &identifier->token);
            AstVarDecl*    varNode = Ast::newVarDecl(sourceFile, identifier->name, parentNode, this);
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
                varNode->assignment = Ast::newIdentifierRef(sourceFile, front->name, varNode, this);
            }

            if (varNode->assignment)
                varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
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
        auto        savedtoken = token;
        auto        tmpVarName = format("__tmp_%d", g_Global.uniqueID.fetch_add(1));
        AstVarDecl* varNode    = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
        varNode->kind          = kind;
        varNode->token         = savedtoken;
        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        if (assign)
            varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
        varNode->flags |= AST_R_VALUE | AST_GENERATED;
        SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));

        // And reference that variable, in the form value = __tmp_0.item?
        int idx = 0;
        for (auto child : leftNode->childs)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(isValidVarName(identifier));
            varNode        = Ast::newVarDecl(sourceFile, identifier->name, parentNode, this);
            varNode->kind  = kind;
            varNode->token = identifier->token;
            varNode->flags |= AST_R_VALUE;
            SWAG_CHECK(currentScope->symTable.registerSymbolName(&context, varNode, SymbolKind::Variable));
            identifier                            = Ast::newIdentifierRef(sourceFile, format("%s.item%d", tmpVarName.c_str(), idx++), varNode, this);
            identifier->token                     = savedtoken;
            varNode->assignment                   = identifier;
            varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
        }
    }

    // Single declaration/affectation
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(leftNode));
        auto identifier = leftNode->childs.back();
        SWAG_CHECK(isValidVarName(identifier));
        AstVarDecl* varNode = Ast::newVarDecl(sourceFile, identifier->name, parent, this);
        varNode->kind       = kind;
        varNode->inheritTokenLocation(leftNode->token);

        if (result)
            *result = varNode;
        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        if (assign)
            varNode->assignment->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
        varNode->flags |= AST_R_VALUE;
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
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doInitializationExpression(nullptr, &assign));
        SWAG_CHECK(doVarDeclExpression(parent, leftNode, nullptr, assign, AstNodeKind::VarDecl, result));
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
        // Multiple affectation
        // like in a, b, c = 0
        if (leftNode->kind == AstNodeKind::MultiIdentifier)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            ScopedLocation lk(this, &token);

            // Generate an expression of the form "var firstVar = assignment", and "secondvar = firstvar" for the rest
            bool firstDone  = false;
            auto savedtoken = token;
            auto front      = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);
            front->computeName();
            while (!leftNode->childs.empty())
            {
                auto child        = leftNode->childs.front();
                auto affectNode   = Ast::newAffectOp(sourceFile, parentNode);
                affectNode->token = savedtoken;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);

                if (!firstDone)
                {
                    firstDone = true;
                    SWAG_CHECK(tokenizer.getToken(token));
                    if (affectNode->token.id == TokenId::SymEqual)
                        SWAG_CHECK(doMoveExpression(affectNode));
                    else
                        SWAG_CHECK(doExpression(affectNode));
                }
                else
                {
                    Ast::newIdentifierRef(sourceFile, front->name, affectNode, this)->token = savedtoken;
                }
            }
        }

        // Tuple destructuration
        else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            // Generate an expression of the form "var __tmp_0 = assignment"
            auto        savedtoken = token;
            auto        tmpVarName = format("__tmp_%d", g_Global.uniqueID.fetch_add(1));
            AstVarDecl* varNode    = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
            varNode->token         = savedtoken;
            varNode->flags |= AST_GENERATED;
            SWAG_CHECK(tokenizer.getToken(token));
            SWAG_CHECK(doExpression(varNode, &varNode->assignment));

            // And reference that variable, in the form value = __tmp_0.item?
            int idx = 0;
            while (!leftNode->childs.empty())
            {
                auto child        = leftNode->childs.front();
                auto affectNode   = Ast::newAffectOp(sourceFile, parentNode);
                affectNode->token = savedtoken;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);
                Ast::newIdentifierRef(sourceFile, format("%s.item%d", tmpVarName.c_str(), idx++), affectNode, this)->token = savedtoken;
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

    if (token.id != TokenId::SymLeftCurly)
        SWAG_CHECK(eatSemiCol("after left expression"));
    return true;
}

bool SyntaxJob::doInit(AstNode* parent, AstNode** result)
{
    AstInit* node     = Ast::newNode<AstInit>(this, AstNodeKind::Init, sourceFile, parent);
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
        SWAG_CHECK(doFuncCallParameters(node, &node->parameters));
    }

    return true;
}

bool SyntaxJob::doDrop(AstNode* parent, AstNode** result)
{
    AstDrop* node     = Ast::newNode<AstDrop>(this, AstNodeKind::Drop, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveDrop;
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
#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "ErrorIds.h"

bool SyntaxJob::doLiteral(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Literal, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveLiteral;
    node->token       = token;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymQuote)
    {
        SWAG_CHECK(eatToken());

        auto identifierRef = Ast::newIdentifierRef(sourceFile, node, this);
        SWAG_CHECK(doIdentifier(identifierRef, IDENTIFIER_NO_PARAMS | IDENTIFIER_TYPE_DECL));
        identifierRef->childs.back()->semanticFct = SemanticJob::resolveLiteralSuffix;
    }

    return true;
}

bool SyntaxJob::doArrayPointerIndex(AstNode** exprNode)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftSquare));

    AstNode* firstExpr = nullptr;
    SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &firstExpr));

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
        SWAG_CHECK(doExpression(arrayNode, EXPR_FLAG_NONE, &arrayNode->upperBound));
        *exprNode = arrayNode;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
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
                SWAG_CHECK(doExpression(arrayNode, EXPR_FLAG_NONE, &arrayNode->access));
            }

            *exprNode = arrayNode;
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken(TokenId::SymComma));
        }

        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
        SWAG_VERIFY(token.id != TokenId::SymLeftSquare, error(token, Err(Err0826), Hlp(Hlp0006)));
    }

    return true;
}

bool SyntaxJob::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstIntrinsicProp>(this, AstNodeKind::IntrinsicProp, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveIntrinsicProperty;
    node->inheritTokenName(token);
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(verifyError(token, token.id != TokenId::SymRightParen, Err(Err0861)));

    // Three parameters
    if (node->token.id == TokenId::IntrinsicMakeInterface)
    {
        AstNode* params = Ast::newFuncCallParams(sourceFile, node, this);
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE));
    }

    // Two parameters
    else if (node->token.id == TokenId::IntrinsicMakeSlice ||
             node->token.id == TokenId::IntrinsicMakeString ||
             node->token.id == TokenId::IntrinsicMakeAny)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    }

    // Two parameters
    else if (node->token.id == TokenId::IntrinsicCVaArg)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doTypeExpression(node));
    }

    // One single parameter
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doSinglePrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    relaxIdentifier(token);
    switch (token.id)
    {
    case TokenId::CompilerCallerFunction:
    case TokenId::CompilerCallerLocation:
    case TokenId::CompilerBuildCfg:
    case TokenId::CompilerArch:
    case TokenId::CompilerOs:
    case TokenId::CompilerSwagOs:
    case TokenId::CompilerBackend:
        SWAG_CHECK(doCompilerSpecialFunction(parent, result));
        break;

    case TokenId::CompilerLocation:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::IntrinsicDefined:
        SWAG_CHECK(doIntrinsicDefined(parent, result));
        break;

    case TokenId::CompilerInclude:
        SWAG_CHECK(doCompilerInclude(parent, result));
        break;

    case TokenId::CompilerSelf:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::SymLeftParen:
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(verifyError(token, token.id != TokenId::SymRightParen, Err(Err0862)));
        AstNode* expr;
        SWAG_CHECK(doExpression(parent, exprFlags, &expr));
        expr->flags |= AST_IN_ATOMIC_EXPR;
        if (result)
            *result = expr;
        SWAG_CHECK(eatToken(TokenId::SymRightParen, "to close the left expression"));
        break;
    }

    case TokenId::KwdTrue:
    case TokenId::KwdFalse:
    case TokenId::KwdNull:
    case TokenId::CompilerFile:
    case TokenId::CompilerModule:
    case TokenId::CompilerLine:
    case TokenId::CompilerBuildVersion:
    case TokenId::CompilerBuildRevision:
    case TokenId::CompilerBuildNum:
    case TokenId::LiteralNumber:
    case TokenId::LiteralString:
        SWAG_CHECK(doLiteral(parent, result));
        break;

    case TokenId::KwdTry:
    case TokenId::KwdCatch:
    case TokenId::KwdTryCatch:
    case TokenId::KwdAssume:
        SWAG_CHECK(doTryCatchAssume(parent, result));
        break;
    case TokenId::SymBackTick:
    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;
    case TokenId::SymDot:
    {
        SWAG_CHECK(eatToken());
        AstNode* idref;
        SWAG_CHECK(doIdentifierRef(parent, &idref));
        if (result)
            *result = idref;
        CastAst<AstIdentifierRef>(idref, AstNodeKind::IdentifierRef)->specFlags |= AST_SPEC_IDENTIFIERREF_AUTO_SCOPE;
        break;
    }

    case TokenId::IntrinsicIndex:
        SWAG_CHECK(doIndex(parent, result));
        break;

    case TokenId::IntrinsicGetErr:
        SWAG_CHECK(doGetErr(parent, result));
        break;

    case TokenId::IntrinsicHasTag:
    case TokenId::IntrinsicGetTag:
    case TokenId::IntrinsicSafety:
        SWAG_CHECK(doIntrinsicTag(parent, result));
        break;

    case TokenId::IntrinsicSpread:
    case TokenId::IntrinsicSizeOf:
    case TokenId::IntrinsicAlignOf:
    case TokenId::IntrinsicOffsetOf:
    case TokenId::IntrinsicKindOf:
    case TokenId::IntrinsicCountOf:
    case TokenId::IntrinsicDataOf:
    case TokenId::IntrinsicStringOf:
    case TokenId::IntrinsicNameOf:
    case TokenId::IntrinsicRunes:
    case TokenId::IntrinsicMakeAny:
    case TokenId::IntrinsicMakeSlice:
    case TokenId::IntrinsicMakeString:
    case TokenId::IntrinsicMakeCallback:
    case TokenId::IntrinsicMakeInterface:
    case TokenId::IntrinsicAlloc:
    case TokenId::IntrinsicRealloc:
    case TokenId::IntrinsicGetContext:
    case TokenId::IntrinsicGetProcessInfos:
    case TokenId::IntrinsicArguments:
    case TokenId::IntrinsicModules:
    case TokenId::IntrinsicCompiler:
    case TokenId::IntrinsicIsByteCode:
    case TokenId::IntrinsicMemCmp:
    case TokenId::IntrinsicStrLen:
    case TokenId::IntrinsicStrCmp:
    case TokenId::IntrinsicStringCmp:
    case TokenId::IntrinsicTypeCmp:
    case TokenId::IntrinsicAtomicAdd:
    case TokenId::IntrinsicAtomicAnd:
    case TokenId::IntrinsicAtomicOr:
    case TokenId::IntrinsicAtomicXor:
    case TokenId::IntrinsicAtomicXchg:
    case TokenId::IntrinsicAtomicCmpXchg:
    case TokenId::IntrinsicIsConstExpr:
    case TokenId::IntrinsicCVaArg:
    case TokenId::IntrinsicTypeOf:
    case TokenId::IntrinsicItfTableOf:
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
    case TokenId::IntrinsicATan2:
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
    case TokenId::KwdClosure:
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::KwdRef:
    case TokenId::NativeType:
    case TokenId::SymAsterisk:
    case TokenId::SymCircumflex:
    case TokenId::SymLeftSquare:
    case TokenId::SymLeftCurly:
    {
        PushSyntaxContextFlags cf(this, CONTEXT_FLAG_EXPRESSION);
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);
        SWAG_CHECK(doTypeExpression(parent, result));
        break;
    }

    case TokenId::SymLiteralCurly:
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);
        SWAG_CHECK(eatToken(TokenId::SymLiteralCurly));
        SWAG_CHECK(doExpressionListTuple(parent, result));
        break;

    case TokenId::SymLiteralBracket:
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);
        SWAG_CHECK(eatToken(TokenId::SymLiteralBracket));
        SWAG_CHECK(doExpressionListArray(parent, result));
        break;

    case TokenId::SymLiteralParen:
    case TokenId::SymLiteralVertical:
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);
        SWAG_CHECK(doLambdaExpression(parent, result));
        break;

    case TokenId::KwdCast:
        SWAG_CHECK(doCast(parent, result));
        break;

    default:
        return invalidTokenError(InvalidTokenError::PrimaryExpression);
    }

    return true;
}

bool SyntaxJob::doKeepRef(AstNode* parent, AstNode** result)
{
    auto refNode = Ast::newNode<AstNode>(this, AstNodeKind::KeepRef, sourceFile, parent, 2);
    if (result)
        *result = refNode;
    refNode->semanticFct = SemanticJob::resolveKeepRef;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(refNode, EXPR_FLAG_SIMPLE));
    return true;
}

bool SyntaxJob::doDeRef(AstNode* parent, AstNode** result)
{
    auto identifierRef     = Ast::newIdentifierRef(sourceFile, parent, this);
    auto arrayNode         = Ast::newNode<AstArrayPointerIndex>(this, AstNodeKind::ArrayPointerIndex, sourceFile, identifierRef, 2);
    arrayNode->semanticFct = SemanticJob::resolveArrayPointerIndex;
    arrayNode->specFlags   = AST_SPEC_ARRAYPTRIDX_ISDEREF;
    Token savedToken       = token;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doUnaryExpression(arrayNode, EXPR_FLAG_SIMPLE, &arrayNode->array));

    auto literal = Ast::newNode<AstNode>(this, AstNodeKind::Literal, sourceFile, arrayNode);
    literal->setFlagsValueIsComputed();
    literal->computedValue->reg.u64 = 0;
    literal->token.literalType      = LiteralType::TT_S32;
    literal->semanticFct            = SemanticJob::resolveLiteral;
    literal->inheritTokenLocation(arrayNode->array->token);
    arrayNode->access = literal;

    if (result)
        *result = identifierRef;
    return true;
}

bool SyntaxJob::doPrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* exprNode;

    // Take pointer
    if (token.id == TokenId::SymAmpersand)
    {
        exprNode              = Ast::newNode<AstMakePointer>(this, AstNodeKind::MakePointer, sourceFile, nullptr);
        exprNode->semanticFct = SemanticJob::resolveMakePointer;
        SWAG_CHECK(eatToken());

        bool hasDot = false;
        if (token.id == TokenId::SymDot)
        {
            SWAG_CHECK(eatToken());
            hasDot = true;
        }

        AstNode* identifierRef;
        SWAG_CHECK(doIdentifierRef(nullptr, &identifierRef));
        forceTakeAddress(identifierRef);

        if (hasDot)
        {
            CastAst<AstIdentifierRef>(identifierRef, AstNodeKind::IdentifierRef)->specFlags |= AST_SPEC_IDENTIFIERREF_AUTO_SCOPE;
        }

        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doArrayPointerIndex(&identifierRef));

        Ast::addChildBack(exprNode, identifierRef);
        identifierRef->flags |= AST_TAKE_ADDRESS;
        if (parent)
            Ast::addChildBack(parent, exprNode);
    }

    // Dereference pointer
    else if (token.id == TokenId::KwdDeRef)
    {
        SWAG_CHECK(doDeRef(parent, &exprNode));
    }
    // Force ref pointer
    else if (token.id == TokenId::KwdKeepRef)
    {
        SWAG_CHECK(doKeepRef(parent, &exprNode));
    }
    else
    {
        SWAG_CHECK(doSinglePrimaryExpression(parent, exprFlags, &exprNode));
    }

    if (result)
        *result = exprNode;
    return true;
}

bool SyntaxJob::doUnaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
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
        node->token       = token;
        if (result)
            *result = node;
        SWAG_CHECK(eatToken());
        return doPrimaryExpression(node, exprFlags);
    }
    }

    return doPrimaryExpression(parent, exprFlags, result);
}

static int getPrecedence(TokenId id)
{
    switch (id)
    {
    case TokenId::SymTilde:
        return 0;
    case TokenId::SymAsterisk:
    case TokenId::SymSlash:
    case TokenId::SymPercent:
        return 1;
    case TokenId::SymPlus:
    case TokenId::SymMinus:
        return 2;
    case TokenId::SymGreaterGreater:
    case TokenId::SymLowerLower:
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
    case TokenId::SymAsterisk:
    case TokenId::SymVertical:
    case TokenId::SymCircumflex:
    case TokenId::SymPlusPlus:
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
            auto atom = factor->flags & AST_IN_ATOMIC_EXPR;
            factor->flags &= ~AST_IN_ATOMIC_EXPR;

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

            if (atom)
                factor->flags |= AST_IN_ATOMIC_EXPR;
        }
    }

    *result = factor;
    return true;
}

bool SyntaxJob::doModifiers(Token& forNode, uint32_t& mdfFlags)
{
    auto opId = forNode.id;

    mdfFlags = 0;
    while (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        if (token.text == g_LangSpec->name_safe)
        {
            switch (opId)
            {
            case TokenId::SymPlus:
            case TokenId::SymMinus:
            case TokenId::SymAsterisk:
            case TokenId::SymLowerLower:
            case TokenId::SymGreaterGreater:
            case TokenId::SymPlusEqual:
            case TokenId::SymMinusEqual:
            case TokenId::SymAsteriskEqual:
            case TokenId::SymLowerLowerEqual:
            case TokenId::SymGreaterGreaterEqual:
                break;
            default:
                return error(token, Fmt(Err(Err0266), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_SAFE), error(token, Fmt(Err(Err0265), token.ctext())));
            mdfFlags |= MODIFIER_SAFE;
            SWAG_CHECK(eatToken());
            continue;
        }

        if (token.text == g_LangSpec->name_small)
        {
            switch (opId)
            {
            case TokenId::SymLowerLower:
            case TokenId::SymGreaterGreater:
            case TokenId::SymLowerLowerEqual:
            case TokenId::SymGreaterGreaterEqual:
                break;
            default:
                return error(token, Fmt(Err(Err0266), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_SMALL), error(token, Fmt(Err(Err0265), token.ctext())));
            mdfFlags |= MODIFIER_SMALL;
            SWAG_CHECK(eatToken());
            continue;
        }

        if (token.text == g_LangSpec->name_nodrop)
        {
            switch (opId)
            {
            case TokenId::SymEqual:
            case TokenId::SymColonEqual:
                break;
            default:
                return error(token, Fmt(Err(Err0266), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_NO_LEFT_DROP), error(token, Fmt(Err(Err0265), token.ctext())));
            mdfFlags |= MODIFIER_NO_LEFT_DROP;
            SWAG_CHECK(eatToken());
            continue;
        }

        if (token.text == g_LangSpec->name_move)
        {
            switch (opId)
            {
            case TokenId::SymEqual:
            case TokenId::SymColonEqual:
                break;
            default:
                return error(token, Fmt(Err(Err0266), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_MOVE), error(token, Fmt(Err(Err0265), token.ctext())));
            mdfFlags |= MODIFIER_MOVE;
            SWAG_CHECK(eatToken());
            continue;
        }

        if (token.text == g_LangSpec->name_moveraw)
        {
            switch (opId)
            {
            case TokenId::SymEqual:
            case TokenId::SymColonEqual:
                break;
            default:
                return error(token, Fmt(Err(Err0266), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_NO_RIGHT_DROP), error(token, Fmt(Err(Err0265), token.ctext())));
            SWAG_VERIFY(!(mdfFlags & MODIFIER_MOVE), error(token, Fmt(Err(Err0265), g_LangSpec->name_move.c_str())));
            mdfFlags |= MODIFIER_MOVE | MODIFIER_NO_RIGHT_DROP;
            SWAG_CHECK(eatToken());
            continue;
        }

        return error(token, Fmt(Err(Err0264), token.ctext()));
    }

    return true;
}

bool SyntaxJob::doFactorExpression(AstNode** parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_ASSERT(parent);
    SWAG_CHECK(doUnaryExpression(*parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);

    bool isBinary = false;
    if ((token.id == TokenId::SymPlus) ||
        (token.id == TokenId::SymMinus) ||
        (token.id == TokenId::SymAsterisk) ||
        (token.id == TokenId::SymSlash) ||
        (token.id == TokenId::SymPercent) ||
        (token.id == TokenId::SymAmpersand) ||
        (token.id == TokenId::SymVertical) ||
        (token.id == TokenId::SymGreaterGreater) ||
        (token.id == TokenId::SymLowerLower) ||
        (token.id == TokenId::SymPlusPlus) ||
        (token.id == TokenId::SymCircumflex))
    {
        auto binaryNode = Ast::newNode<AstOp>(this, AstNodeKind::FactorOp, sourceFile, *parent, 2);

        if (token.id == TokenId::SymGreaterGreater || token.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = SemanticJob::resolveFactorExpression;
        binaryNode->token = token;
        SWAG_CHECK(eatToken());

        // Modifiers
        uint32_t mdfFlags;
        SWAG_CHECK(doModifiers(binaryNode->token, mdfFlags));
        if (mdfFlags & MODIFIER_SAFE)
        {
            binaryNode->specFlags |= AST_SPEC_OP_SAFE;
            binaryNode->attributeFlags |= ATTRIBUTE_SAFETY_OFF_OPERATOR;
        }

        if (mdfFlags & MODIFIER_SMALL)
        {
            binaryNode->specFlags |= AST_SPEC_OP_SMALL;
        }

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(doFactorExpression((AstNode**) &binaryNode, exprFlags));
        SWAG_CHECK(doOperatorPrecedence((AstNode**) &binaryNode));
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
        binaryNode->token       = token;

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFactorExpression(&binaryNode, exprFlags));
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

bool SyntaxJob::doCompareExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doFactorExpression(&parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);
    SWAG_CHECK(doOperatorPrecedence(&leftNode));
    SWAG_VERIFY(token.id != TokenId::SymEqual, error(token, Err(Err0267)));
    Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;
    return true;
}

bool SyntaxJob::doBoolExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doCompareExpression(parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);

    bool isBinary = false;
    if ((token.id == TokenId::KwdOr) || (token.id == TokenId::KwdAnd))
    {
        auto binaryNode         = Ast::newNode<AstBinaryOpNode>(this, AstNodeKind::BinaryOp, sourceFile, parent, 2);
        binaryNode->semanticFct = SemanticJob::resolveBoolExpression;
        binaryNode->token       = token;

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doBoolExpression(binaryNode, EXPR_FLAG_NONE));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    if (result)
        *result = leftNode;

    return true;
}

bool SyntaxJob::doMoveExpression(Token& forToken, AstNode* parent, AstNode** result)
{
    uint32_t mdfFlags;
    SWAG_CHECK(doModifiers(forToken, mdfFlags));

    // nodrop left
    if (mdfFlags & MODIFIER_NO_LEFT_DROP)
    {
        auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::NoDrop, sourceFile, parent);
        if (result)
            *result = exprNode;
        exprNode->semanticFct = SemanticJob::resolveMove;
        exprNode->flags |= AST_NO_LEFT_DROP;
        parent = exprNode;
        result = nullptr;
    }

    // move
    if (mdfFlags & MODIFIER_MOVE)
    {
        auto exprNode = Ast::newNode<AstNode>(this, AstNodeKind::Move, sourceFile, parent);
        if (result)
            *result = exprNode;
        exprNode->semanticFct = SemanticJob::resolveMove;
        exprNode->flags |= AST_FORCE_MOVE;
        parent = exprNode;
        result = nullptr;

        // nodrop right
        if (mdfFlags & MODIFIER_NO_RIGHT_DROP)
        {
            exprNode = Ast::newNode<AstNode>(this, AstNodeKind::NoDrop, sourceFile, parent);
            if (result)
                *result = exprNode;
            exprNode->semanticFct = SemanticJob::resolveMove;
            exprNode->flags |= AST_NO_RIGHT_DROP;
            parent = exprNode;
            result = nullptr;
        }
    }

    SWAG_CHECK(doExpression(parent, EXPR_FLAG_NONE, result));
    return true;
}

bool SyntaxJob::doExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* boolExpression = nullptr;
    switch (token.id)
    {
    case TokenId::CompilerRun:
    {
        ScopedFlags sf(this, AST_RUN_BLOCK);
        auto        node  = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerRun, sourceFile, nullptr);
        node->semanticFct = SemanticJob::resolveCompilerRun;
        SWAG_CHECK(eatToken());

        // :RunGeneratedExp
        if (token.id == TokenId::SymLeftCurly)
        {
            if (result)
                *result = node;

            node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
            node->allocateExtension(ExtensionKind::Semantic);
            node->extension->misc->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;

            AstNode* funcNode;
            SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerGeneratedRunExp));
            funcNode->attributeFlags |= ATTRIBUTE_COMPILER;

            auto idRef                      = Ast::newIdentifierRef(sourceFile, funcNode->token.text, node, this);
            idRef->token.startLocation      = node->token.startLocation;
            idRef->token.endLocation        = node->token.endLocation;
            auto identifier                 = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
            identifier->callParameters      = Ast::newFuncCallParams(sourceFile, identifier, this);
            identifier->token.startLocation = node->token.startLocation;
            identifier->token.endLocation   = node->token.endLocation;
            return true;
        }

        SWAG_CHECK(doBoolExpression(node, exprFlags));
        boolExpression = node;
        break;
    }
    case TokenId::CompilerMixin:
    {
        auto node         = Ast::newNode<AstCompilerMixin>(this, AstNodeKind::CompilerMixin, sourceFile, nullptr);
        node->semanticFct = SemanticJob::resolveCompilerMixin;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(node, nullptr, IDENTIFIER_NO_PARAMS));
        boolExpression = node;
        break;
    }
    case TokenId::CompilerCode:
    {
        auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerCode, sourceFile, nullptr);
        SWAG_CHECK(eatToken());
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doEmbeddedStatement(node));
        else
            SWAG_CHECK(doBoolExpression(node, exprFlags));
        auto typeCode     = allocType<TypeInfoCode>();
        typeCode->content = node->childs.front();
        typeCode->content->flags |= AST_NO_SEMANTIC;
        node->typeInfo = typeCode;
        node->flags |= AST_NO_BYTECODE;
        boolExpression = node;
        break;
    }

    case TokenId::CompilerAst:
    case TokenId::CompilerFuncInit:
    case TokenId::CompilerFuncDrop:
    case TokenId::CompilerFuncPreMain:
    case TokenId::CompilerFuncMain:
    case TokenId::CompilerFuncTest:
        return error(token, Fmt(Err(Err0055), token.ctext()));

    default:
        SWAG_CHECK(doBoolExpression(parent, exprFlags, &boolExpression));
        Ast::removeFromParent(boolExpression);
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

        SWAG_CHECK(doExpression(triNode, exprFlags));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doExpression(triNode, exprFlags));
    }

    // A orelse B
    else if (token.id == TokenId::KwdOrElse)
    {
        SWAG_CHECK(eatToken());
        auto triNode         = Ast::newNode<AstNode>(this, AstNodeKind::NullConditionalExpression, sourceFile, parent, 2);
        triNode->semanticFct = SemanticJob::resolveNullConditionalOp;
        if (result)
            *result = triNode;
        Ast::addChildBack(triNode, boolExpression);
        SWAG_CHECK(doExpression(triNode, exprFlags));
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
    return doExpression(parent, EXPR_FLAG_NONE, result);
}

bool SyntaxJob::doExpressionListTuple(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, parent);
    initNode->semanticFct = SemanticJob::resolveExpressionListTuple;
    initNode->specFlags |= AST_SPEC_EXPRLIST_FOR_TUPLE;
    SWAG_CHECK(eatToken());

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
            SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &paramExpression));

            // Name
            if (token.id == TokenId::SymColon)
            {
                SWAG_VERIFY(paramExpression->kind == AstNodeKind::IdentifierRef, error(paramExpression, Err(Err0448)));
                SWAG_CHECK(checkIsSingleIdentifier(paramExpression, "as a tuple field name"));
                SWAG_CHECK(checkIsValidVarName(paramExpression->childs.back()));
                auto name            = paramExpression->childs.back()->token.text;
                auto namedExpression = paramExpression->childs.back();
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymLeftCurly)
                    SWAG_CHECK(doExpressionListTuple(initNode, &paramExpression));
                else
                    SWAG_CHECK(doExpression(initNode, EXPR_FLAG_NONE, &paramExpression));
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
    SWAG_CHECK(eatToken());

    if (token.id == TokenId::SymRightSquare)
        return error(token, Fmt(Err(Err0270)));
    if (result)
        *result = initNode;

    while (token.id != TokenId::SymRightSquare)
    {
        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode));
        else if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListTuple(initNode));
        else
            SWAG_CHECK(doExpression(initNode, EXPR_FLAG_NONE));

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    return true;
}

bool SyntaxJob::doInitializationExpression(Token& forToken, AstNode* parent, AstNode** result)
{
    PushSyntaxContextFlags cf(this, CONTEXT_FLAG_EXPRESSION);

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

    return doMoveExpression(forToken, parent, result);
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
    auto node = Ast::newNode<AstDefer>(this, AstNodeKind::Defer, sourceFile, parent);
    if (result)
        *result = node;
    node->semanticFct = SemanticJob::resolveDefer;

    SWAG_CHECK(eatToken());

    // Defer kind
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(eatToken());
        if (token.text == g_LangSpec->name_err)
            node->deferKind = DeferKind::Error;
        else if (token.text == g_LangSpec->name_noerr)
            node->deferKind = DeferKind::NoError;
        else
            return error(token, Fmt(Err(Err0256), token.ctext()), Hlp(Hlp0023));

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    ScopedFlags scopedFlags(this, AST_IN_DEFER);
    if (token.id == TokenId::SymLeftCurly)
        SWAG_CHECK(doScopedCurlyStatement(node));
    else
        SWAG_CHECK(doLeftInstruction(node));

    return true;
}

bool SyntaxJob::doLeftExpressionVar(AstNode* parent, AstNode** result, uint32_t identifierFlags, AstWith* withNode)
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
            SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::SymQuestion, error(token, Fmt(Err(Err0271), token.ctext())));
            SWAG_CHECK(doIdentifierRef(multi, nullptr, identifierFlags | IDENTIFIER_ACCEPT_QUESTION));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen, "after tuple unpacking"));
        break;
    }

    case TokenId::Identifier:
    case TokenId::SymBackTick:
    case TokenId::CompilerSelf:
    {
        AstNode* exprNode     = nullptr;
        AstNode* multi        = nullptr;
        bool     prePrendWith = withNode != nullptr;
        while (true)
        {
            if (token.id == TokenId::SymDot)
            {
                SWAG_VERIFY(withNode, error(token, Err(Err0836)));
                prePrendWith = true;
                eatToken();
            }

            SWAG_CHECK(doIdentifierRef(multi == nullptr ? parent : multi, &exprNode, identifierFlags));
            if (multi == nullptr)
                Ast::removeFromParent(exprNode);

            // Prepend the 'with' identifier
            if (withNode && prePrendWith)
            {
                prePrendWith = false;
                SWAG_ASSERT(exprNode->kind == AstNodeKind::IdentifierRef);
                for (int wi = (int) withNode->id.size() - 1; wi >= 0; wi--)
                {
                    auto id = Ast::newIdentifier(sourceFile, withNode->id[wi], (AstIdentifierRef*) exprNode, exprNode, this);
                    id->flags |= AST_GENERATED;
                    id->specFlags |= AST_SPEC_IDENTIFIER_FROM_WITH;
                    id->inheritTokenLocation(exprNode);
                    exprNode->childs.pop_back();
                    Ast::addChildFront(exprNode, id);
                }
            }

            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());

            if (!multi)
            {
                multi = Ast::newNode<AstNode>(this, AstNodeKind::MultiIdentifier, sourceFile, parent);
                Ast::addChildBack(multi, exprNode);
            }
        }

        Ast::removeFromParent(multi);
        *result = multi ? multi : exprNode;
        break;
    }

    default:
        return invalidTokenError(InvalidTokenError::LeftExpressionVar);
    }

    return true;
}

bool SyntaxJob::doLeftExpressionAffect(AstNode* parent, AstNode** result, AstWith* withNode)
{
    switch (token.id)
    {
    case TokenId::IntrinsicGetContext:
    case TokenId::IntrinsicGetProcessInfos:
        SWAG_CHECK(doIdentifierRef(parent, result));
        return true;
    case TokenId::SymLeftParen:
    case TokenId::Identifier:
    case TokenId::SymBackTick:
    case TokenId::CompilerSelf:
        SWAG_CHECK(doLeftExpressionVar(parent, result, 0, withNode));
        Ast::removeFromParent(*result);
        return true;

    case TokenId::KwdTry:
    case TokenId::KwdCatch:
    case TokenId::KwdTryCatch:
    case TokenId::KwdAssume:
        SWAG_CHECK(doTryCatchAssume(parent, result));
        return true;
    case TokenId::KwdThrow:
        SWAG_CHECK(doThrow(parent, result));
        return true;

    case TokenId::KwdDeRef:
        SWAG_CHECK(doDeRef(parent, result));
        return true;

    default:
        return invalidTokenError(InvalidTokenError::LeftExpression);
    }
}

bool SyntaxJob::doAffectExpression(AstNode* parent, AstNode** result, AstWith* withNode)
{
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpressionAffect(parent, &leftNode, withNode));
    Ast::removeFromParent(leftNode);

    // Variable declaration and initialization by ':='
    if (token.id == TokenId::SymColonEqual)
    {
        auto     saveToken = token;
        AstNode* assign;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doInitializationExpression(saveToken, parent, &assign));
        Ast::removeFromParent(assign);
        SWAG_CHECK(doVarDeclExpression(parent, leftNode, nullptr, assign, saveToken, AstNodeKind::VarDecl, result));
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
             token.id == TokenId::SymPercentEqual ||
             token.id == TokenId::SymLowerLowerEqual ||
             token.id == TokenId::SymGreaterGreaterEqual)
    {
        uint8_t  opFlags     = 0;
        uint64_t opAttrFlags = 0;
        auto     savedtoken  = token;
        SWAG_CHECK(eatToken());

        // Modifiers
        uint32_t mdfFlags = 0;
        if (savedtoken.id != TokenId::SymEqual)
            SWAG_CHECK(doModifiers(savedtoken, mdfFlags));
        if (mdfFlags & MODIFIER_SAFE)
        {
            opFlags |= AST_SPEC_OP_SAFE;
            opAttrFlags |= ATTRIBUTE_SAFETY_OFF_OPERATOR;
        }
        if (mdfFlags & MODIFIER_SMALL)
        {
            opFlags |= AST_SPEC_OP_SMALL;
        }

        // Multiple affectation
        // like in a, b, c = 0
        if (leftNode->kind == AstNodeKind::MultiIdentifier)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            ScopedLocation lk(this, &savedtoken);

            // Generate an expression of the form "var firstVar = assignment", and "secondvar = firstvar" for the rest
            // This will avoid to do the right expression multiple times (if this is a function call for example).
            //
            // If this is not the '=' operator, then we have to duplicate the affectation for each variable
            // If the affect expression is a literal, it's better to duplicate also
            AstNode* affectExpression = nullptr;
            bool     firstDone        = false;
            auto     front            = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);
            front->computeName();

            auto cloneFront = Ast::clone(front, nullptr);

            while (!leftNode->childs.empty())
            {
                auto child        = leftNode->childs.front();
                auto affectNode   = Ast::newAffectOp(sourceFile, parentNode, opFlags, opAttrFlags);
                affectNode->token = savedtoken;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);

                // First create 'firstVar = assignment'
                if (!firstDone)
                {
                    firstDone = true;
                    if (affectNode->token.id == TokenId::SymEqual)
                        SWAG_CHECK(doMoveExpression(affectNode->token, affectNode, &affectExpression));
                    else
                        SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE, &affectExpression));
                }

                // This is not an initialization, so we need to duplicate the right expression
                else if (affectNode->token.id != TokenId::SymEqual || affectExpression->kind == AstNodeKind::Literal)
                {
                    auto newAffect = Ast::clone(affectExpression, affectNode);
                    newAffect->inheritTokenLocation(affectExpression);
                }

                // In case of an affectation, create 'otherVar = firstVar'
                else
                {
                    Ast::clone(cloneFront, affectNode);
                }
            }
        }

        // Tuple unpacking
        else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
        {
            auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            if (result)
                *result = parentNode;

            ScopedLocation lk(this, &savedtoken);

            // Get right side
            AstNode* assignment;
            SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &assignment));

            // Generate an expression of the form "var __tmp_0 = assignment"
            auto        tmpVarName = Fmt("__4tmp_%d", g_UniqueID.fetch_add(1));
            AstVarDecl* varNode    = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
            varNode->flags |= AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS;
            Ast::addChildBack(varNode, assignment);
            assignment->inheritOwners(varNode);
            varNode->assignment = assignment;
            varNode->assignment->flags |= AST_NO_LEFT_DROP;

            varNode->token.startLocation = leftNode->childs.front()->token.startLocation;
            varNode->token.endLocation   = leftNode->childs.back()->token.endLocation;
            varNode->allocateExtension(ExtensionKind::Semantic);
            varNode->extension->misc->semanticAfterFct = SemanticJob::resolveTupleUnpackBefore;

            // And reference that variable, in the form value = __tmp_0.item?
            int idx = 0;
            while (!leftNode->childs.empty())
            {
                auto child = leftNode->childs.front();

                // Ignore field if '?', otherwise check that this is a valid variable name
                if (child->childs.front()->token.text == '?')
                {
                    idx++;
                    Ast::removeFromParent(child);
                    Ast::addChildBack(parentNode, child);
                    child->flags |= AST_NO_SEMANTIC | AST_NO_BYTECODE;
                    continue;
                }

                auto affectNode        = Ast::newAffectOp(sourceFile, parentNode, opFlags, opAttrFlags);
                affectNode->token.id   = savedtoken.id;
                affectNode->token.text = savedtoken.text;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);
                auto idRef = Ast::newIdentifierRef(sourceFile, Fmt("%s.item%d", tmpVarName.c_str(), idx++), affectNode, this);

                // Force a move between the generated temporary variable and the real var
                idRef->flags |= AST_FORCE_MOVE;
            }
        }

        // One normal simple affectation
        else
        {
            auto affectNode   = Ast::newAffectOp(sourceFile, parent, opFlags, opAttrFlags, this);
            affectNode->token = move(savedtoken);

            Ast::addChildBack(affectNode, leftNode);
            forceTakeAddress(leftNode);

            auto front = affectNode->childs.front();
            front->allocateExtension(ExtensionKind::Semantic);
            front->extension->misc->semanticAfterFct = SemanticJob::resolveAfterAffectLeft;

            if (affectNode->token.id == TokenId::SymEqual)
                SWAG_CHECK(doMoveExpression(affectNode->token, affectNode));
            else
                SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE));

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

    SWAG_VERIFY(token.id != TokenId::SymEqualEqual, error(token, Err(Err0454)));

    if (token.id != TokenId::SymLeftCurly)
        SWAG_CHECK(eatSemiCol("left expression"));
    return true;
}

bool SyntaxJob::doInit(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstInit>(this, AstNodeKind::Init, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveInit;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->expression));

    if (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->count));
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
        node->token.text = g_LangSpec->name_atdrop;
        break;
    case TokenId::IntrinsicPostCopy:
        node->token.text = g_LangSpec->name_atpostCopy;
        node->kind       = AstNodeKind::PostCopy;
        break;
    case TokenId::IntrinsicPostMove:
        node->token.text = g_LangSpec->name_atpostMove;
        node->kind       = AstNodeKind::PostMove;
        break;
    }

    node->semanticFct = SemanticJob::resolveDropCopyMove;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->expression));

    if (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->count));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doRange(AstNode* parent, AstNode* expression, AstNode** result)
{
    auto rangeNode         = Ast::newNode<AstRange>(this, AstNodeKind::Range, sourceFile, parent, 2);
    rangeNode->semanticFct = SemanticJob::resolveRange;
    Ast::removeFromParent(expression);
    Ast::addChildBack(rangeNode, expression);
    rangeNode->expressionLow = expression;
    *result                  = rangeNode;

    if (token.id == TokenId::SymDotDotLess)
        rangeNode->specFlags |= AST_SPEC_RANGE_EXCLUDE_UP;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(doExpression(rangeNode, EXPR_FLAG_SIMPLE, &rangeNode->expressionUp));
    return true;
}
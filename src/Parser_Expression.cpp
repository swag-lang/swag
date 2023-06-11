#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "ErrorIds.h"
#include "TypeManager.h"

bool Parser::doLiteral(AstNode* parent, AstNode** result)
{
    auto node          = Ast::newNode<AstLiteral>(this, AstNodeKind::Literal, sourceFile, parent);
    *result            = node;
    node->semanticFct  = SemanticJob::resolveLiteral;
    node->literalType  = token.literalType;
    node->literalValue = token.literalValue;

    SWAG_CHECK(eatToken());
    if (token.id == TokenId::SymQuote)
    {
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::NativeType, error(token, Err(Syn0060)));
        auto identifierRef = Ast::newIdentifierRef(sourceFile, node, this);
        SWAG_CHECK(doIdentifier(identifierRef, IDENTIFIER_NO_PARAMS | IDENTIFIER_TYPE_DECL));
        identifierRef->childs.back()->semanticFct = SemanticJob::resolveLiteralSuffix;
    }

    return true;
}

bool Parser::doArrayPointerIndex(AstNode** exprNode)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftSquare));

    AstNode* firstExpr = nullptr;

    if (token.id == TokenId::SymDotDot || token.id == TokenId::SymDotDotLess)
    {
        auto literal = Ast::newNode<AstLiteral>(this, AstNodeKind::Literal, sourceFile, nullptr);
        firstExpr    = literal;
        firstExpr->allocateComputedValue();
        firstExpr->flags |= AST_GENERATED;
        firstExpr->computedValue->reg.u64 = 0;
        firstExpr->semanticFct            = SemanticJob::resolveLiteral;
        literal->literalType              = LiteralType::TT_U64;
    }
    else
    {
        SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &firstExpr));
    }

    // Slicing
    if (token.id == TokenId::SymDotDot || token.id == TokenId::SymDotDotLess)
    {
        auto arrayNode         = Ast::newNode<AstArrayPointerSlicing>(this, AstNodeKind::ArrayPointerSlicing, sourceFile, nullptr);
        arrayNode->semanticFct = SemanticJob::resolveArrayPointerSlicing;
        arrayNode->array       = *exprNode;
        Ast::addChildBack(arrayNode, *exprNode);
        Ast::addChildBack(arrayNode, firstExpr);
        arrayNode->lowerBound = firstExpr;
        if (token.id == TokenId::SymDotDotLess)
            arrayNode->specFlags |= AstArrayPointerSlicing::SPECFLAG_EXCLUDE_UP;
        SWAG_CHECK(eatToken());

        if (token.id != TokenId::SymRightSquare)
        {
            SWAG_CHECK(doExpression(arrayNode, EXPR_FLAG_NONE, &arrayNode->upperBound));
        }

        // To the end...
        else
        {
            if (arrayNode->specFlags & AstArrayPointerSlicing::SPECFLAG_EXCLUDE_UP)
            {
                Diagnostic diag{sourceFile, token, Err(Syn0185)};
                diag.hint = Hnt(Hnt0098);
                auto note = Diagnostic::help(arrayNode, arrayNode->token, Hlp(Hlp0047));
                return context->report(diag, note);
            }

            arrayNode->upperBound = Ast::newNode<AstNode>(this, AstNodeKind::AutoSlicingUp, sourceFile, arrayNode);
            arrayNode->array->allocateExtension(ExtensionKind::Semantic);
            SWAG_ASSERT(!arrayNode->array->extSemantic()->semanticAfterFct);
            arrayNode->array->extSemantic()->semanticAfterFct = SemanticJob::resolveArrayPointerSlicingUpperBound;
            arrayNode->upperBound->flags |= AST_GENERATED;
        }

        *exprNode = arrayNode;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    }

    // Deref by index
    else
    {
        while (true)
        {
            auto arrayNode         = Ast::newNode<AstArrayPointerIndex>(this, AstNodeKind::ArrayPointerIndex, sourceFile, nullptr);
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
        SWAG_VERIFY(token.id != TokenId::SymLeftSquare, error(token, Err(Syn0139), Hlp(Hlp0006)));
    }

    return true;
}

bool Parser::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstIntrinsicProp>(this, AstNodeKind::IntrinsicProp, sourceFile, parent);
    *result           = node;
    node->semanticFct = SemanticJob::resolveIntrinsicProperty;
    node->inheritTokenName(token);

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_VERIFY(token.id != TokenId::SymRightParen, error(token, Err(Syn0044)));

    // Three parameters
    if (node->tokenId == TokenId::IntrinsicMakeInterface)
    {
        AstNode* params = Ast::newFuncCallParams(sourceFile, node, this);
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
    }

    // Two parameters
    else if (node->tokenId == TokenId::IntrinsicMakeSlice ||
             node->tokenId == TokenId::IntrinsicMakeString ||
             node->tokenId == TokenId::IntrinsicMakeAny)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    // Two parameters
    else if (node->tokenId == TokenId::IntrinsicCVaArg)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    // One single parameter
    else if (node->tokenId == TokenId::IntrinsicTypeOf ||
             node->tokenId == TokenId::IntrinsicKindOf ||
             node->tokenId == TokenId::IntrinsicSizeOf ||
             node->tokenId == TokenId::IntrinsicDeclType)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_TYPEOF, &dummyResult));
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool Parser::doSinglePrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    relaxIdentifier(token);
    switch (token.id)
    {
    case TokenId::CompilerCallerFunction:
    case TokenId::CompilerCallerLocation:
    case TokenId::CompilerBuildCfg:
    case TokenId::CompilerOs:
    case TokenId::CompilerArch:
    case TokenId::CompilerCpu:
    case TokenId::CompilerSwagOs:
    case TokenId::CompilerBackend:
        SWAG_CHECK(doCompilerSpecialValue(parent, result));
        break;

    case TokenId::CompilerLocation:
    case TokenId::IntrinsicLocation:
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
        auto startLoc = token.startLocation;
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id != TokenId::SymRightParen, error(startLoc, token.endLocation, Err(Syn0160)));
        AstNode* expr;
        SWAG_CHECK(doExpression(parent, exprFlags, &expr));
        *result = expr;
        expr->flags |= AST_IN_ATOMIC_EXPR;
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
    case TokenId::CompilerUp:
    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;
    case TokenId::SymDot:
    {
        SWAG_CHECK(eatToken());
        AstNode* idref;
        SWAG_CHECK(doIdentifierRef(parent, &idref));
        *result = idref;
        CastAst<AstIdentifierRef>(idref, AstNodeKind::IdentifierRef)->specFlags |= AstIdentifierRef::SPECFLAG_AUTO_SCOPE;
        break;
    }

    case TokenId::IntrinsicIndex:
        SWAG_CHECK(doIndex(parent, result));
        break;

    case TokenId::IntrinsicGetErrMsg:
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
    case TokenId::IntrinsicGvtd:
    case TokenId::IntrinsicCompiler:
    case TokenId::IntrinsicIsByteCode:
    case TokenId::IntrinsicGetErr:
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
    case TokenId::IntrinsicDbgAlloc:
    case TokenId::IntrinsicSysAlloc:
    case TokenId::IntrinsicRtFlags:
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
    case TokenId::IntrinsicMulAdd:
        SWAG_CHECK(doIdentifierRef(parent, result));
        break;

    case TokenId::CompilerType:
    {
        eatToken();
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);
        AstNode* resNode;
        SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, &resNode));
        *result = resNode;
        resNode->specFlags |= AstType::SPECFLAG_FORCE_TYPE;
        break;
    }

    case TokenId::KwdConst:
    case TokenId::KwdCode:
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::KwdRef:
    case TokenId::NativeType:
    case TokenId::SymAsterisk:
    case TokenId::SymCircumflex:
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);
        SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, result));
        break;

    case TokenId::SymLeftCurly:
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);
        if (exprFlags & EXPR_FLAG_ALIAS)
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, result));
        else
            SWAG_CHECK(doExpressionListTuple(parent, result));
        break;

    case TokenId::SymLeftSquare:
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);

        // In an alias, this is a type (no alias to array literals)
        if (exprFlags & EXPR_FLAG_ALIAS)
        {
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, result));
        }

        // We can differentiate between a literal array and a type array by looking at what's next
        else if (exprFlags & (EXPR_FLAG_TYPEOF | EXPR_FLAG_PARAMETER))
        {
            tokenizer.saveState(token);
            SWAG_CHECK(doExpressionListArray(parent, result));
            if (token.id != TokenId::SymRightParen && token.id != TokenId::SymRightCurly && token.id != TokenId::SymComma)
            {
                tokenizer.restoreState(token);
                Ast::removeFromParent(parent->childs.back());
                SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_NONE, result));
            }
        }
        else
        {
            SWAG_CHECK(doExpressionListArray(parent, result));
        }

        break;

    case TokenId::KwdFunc:
    case TokenId::KwdClosure:
        if (exprFlags & EXPR_FLAG_SIMPLE)
            return invalidTokenError(InvalidTokenError::PrimaryExpression);

        if (exprFlags & (EXPR_FLAG_ALIAS | EXPR_FLAG_TYPEOF))
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, result));
        else
            SWAG_CHECK(doLambdaExpression(parent, exprFlags, result));
        break;

    case TokenId::KwdCast:
        SWAG_CHECK(doCast(parent, result));
        break;

    default:
        return invalidTokenError(InvalidTokenError::PrimaryExpression);
    }

    return true;
}

bool Parser::doKeepRef(AstNode* parent, AstNode** result)
{
    auto refNode         = Ast::newNode<AstNode>(this, AstNodeKind::KeepRef, sourceFile, parent);
    *result              = refNode;
    refNode->semanticFct = SemanticJob::resolveKeepRef;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(refNode, EXPR_FLAG_SIMPLE, &dummyResult));
    return true;
}

bool Parser::doMoveRef(AstNode* parent, AstNode** result)
{
    auto refNode         = Ast::newNode<AstNode>(this, AstNodeKind::MoveRef, sourceFile, parent);
    *result              = refNode;
    refNode->semanticFct = SemanticJob::resolveMoveRef;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(refNode, EXPR_FLAG_SIMPLE, &dummyResult));
    return true;
}

bool Parser::doDeRef(AstNode* parent, AstNode** result)
{
    auto identifierRef     = Ast::newIdentifierRef(sourceFile, parent, this);
    auto arrayNode         = Ast::newNode<AstArrayPointerIndex>(this, AstNodeKind::ArrayPointerIndex, sourceFile, identifierRef);
    arrayNode->semanticFct = SemanticJob::resolveArrayPointerIndex;
    arrayNode->specFlags   = AstArrayPointerIndex::SPECFLAG_ISDEREF;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doUnaryExpression(arrayNode, EXPR_FLAG_SIMPLE, &arrayNode->array));

    auto literal = Ast::newNode<AstLiteral>(this, AstNodeKind::Literal, sourceFile, arrayNode);
    literal->setFlagsValueIsComputed();
    literal->computedValue->reg.u64 = 0;
    literal->literalType            = LiteralType::TT_S32;
    literal->semanticFct            = SemanticJob::resolveLiteral;
    literal->inheritTokenLocation(arrayNode->array->token);
    arrayNode->access = literal;
    *result           = identifierRef;
    return true;
}

bool Parser::doPrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
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
            CastAst<AstIdentifierRef>(identifierRef, AstNodeKind::IdentifierRef)->specFlags |= AstIdentifierRef::SPECFLAG_AUTO_SCOPE;
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
    else if (token.id == TokenId::KwdRef)
    {
        SWAG_CHECK(doKeepRef(parent, &exprNode));
    }

    // Moveref
    else if (token.id == TokenId::KwdMoveRef)
    {
        SWAG_CHECK(doMoveRef(parent, &exprNode));
    }

    else
    {
        SWAG_CHECK(doSinglePrimaryExpression(parent, exprFlags, &exprNode));
    }

    *result = exprNode;
    return true;
}

bool Parser::doUnaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::KwdCast:
        SWAG_CHECK(doCast(parent, result));
        return true;
    case TokenId::KwdAutoCast:
        SWAG_CHECK(doAutoCast(parent, result));
        return true;
    case TokenId::SymMinus:
    case TokenId::SymExclam:
    case TokenId::SymTilde:
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::SingleOp, sourceFile, parent);
        *result           = node;
        node->semanticFct = SemanticJob::resolveUnaryOp;
        node->token       = token;
        SWAG_CHECK(eatToken());
        return doPrimaryExpression(node, exprFlags, &dummyResult);
    }
    default:
        break;
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
    default:
        break;
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
    default:
        break;
    }

    return false;
}

bool Parser::doOperatorPrecedence(AstNode** result)
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
        auto myPrecedence    = getPrecedence(factor->tokenId);
        auto rightPrecedence = getPrecedence(right->tokenId);

        bool shuffle = false;
        if (myPrecedence < rightPrecedence && myPrecedence != -1 && rightPrecedence != -1)
            shuffle = true;

        // If operation is not associative, then we need to shuffle
        //
        // 2 - 1 - 1 needs to be treated as (2 - 1) - 1 and not 2 - (2 - 1)
        //
        else if (!isAssociative(factor->tokenId) && (myPrecedence == rightPrecedence))
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
            if (factor->parent && factor == factor->parent->childs.front())
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

bool Parser::doModifiers(Token& forNode, TokenId tokenId, uint32_t& mdfFlags)
{
    auto opId = tokenId;

    mdfFlags = 0;
    while (token.id == TokenId::SymComma && !(token.flags & TOKENPARSE_LAST_BLANK) && !(token.flags & TOKENPARSE_LAST_EOL))
    {
        SWAG_CHECK(eatToken());

        if (token.text == g_LangSpec->name_up)
        {
            switch (opId)
            {
            case TokenId::SymPlus:
            case TokenId::SymMinus:
            case TokenId::SymAsterisk:
            case TokenId::SymSlash:
            case TokenId::SymPercent:
                break;
            default:
                return error(token, Fmt(Err(Syn0126), token.ctext(), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_UP), error(token, Fmt(Err(Syn0125), token.ctext())));
            mdfFlags |= MODIFIER_UP;
            SWAG_CHECK(eatToken());
            continue;
        }

        if (token.text == g_LangSpec->name_over)
        {
            switch (opId)
            {
            case TokenId::SymPlus:
            case TokenId::SymMinus:
            case TokenId::SymAsterisk:
            case TokenId::SymPlusEqual:
            case TokenId::SymMinusEqual:
            case TokenId::SymAsteriskEqual:
            case TokenId::KwdCast:
                break;
            default:
                return error(token, Fmt(Err(Syn0126), token.ctext(), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_OVERFLOW), error(token, Fmt(Err(Syn0125), token.ctext())));
            mdfFlags |= MODIFIER_OVERFLOW;
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
                return error(token, Fmt(Err(Syn0126), token.ctext(), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_NO_LEFT_DROP), error(token, Fmt(Err(Syn0125), token.ctext())));
            mdfFlags |= MODIFIER_NO_LEFT_DROP;
            SWAG_CHECK(eatToken());
            continue;
        }

        if (token.text == g_LangSpec->name_bit)
        {
            switch (opId)
            {
            case TokenId::KwdCast:
                break;
            default:
                return error(token, Fmt(Err(Syn0126), token.ctext(), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_BIT), error(token, Fmt(Err(Syn0125), token.ctext())));
            mdfFlags |= MODIFIER_BIT;
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
                return error(token, Fmt(Err(Syn0126), token.ctext(), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_MOVE), error(token, Fmt(Err(Syn0125), token.ctext())));
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
                return error(token, Fmt(Err(Syn0126), token.ctext(), forNode.ctext()));
            }

            SWAG_VERIFY(!(mdfFlags & MODIFIER_NO_RIGHT_DROP), error(token, Fmt(Err(Syn0125), token.ctext())));
            SWAG_VERIFY(!(mdfFlags & MODIFIER_MOVE), error(token, Fmt(Err(Syn0125), g_LangSpec->name_move.c_str())));
            mdfFlags |= MODIFIER_MOVE | MODIFIER_NO_RIGHT_DROP;
            SWAG_CHECK(eatToken());
            continue;
        }

        return error(token, Fmt(Err(Syn0178), token.ctext()));
    }

    return true;
}

bool Parser::doFactorExpression(AstNode** parent, uint32_t exprFlags, AstNode** result)
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
        auto binaryNode = Ast::newNode<AstOp>(this, AstNodeKind::FactorOp, sourceFile, *parent);

        if (token.id == TokenId::SymGreaterGreater || token.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = SemanticJob::resolveShiftExpression;
        else
            binaryNode->semanticFct = SemanticJob::resolveFactorExpression;
        binaryNode->token = token;
        SWAG_CHECK(eatToken());

        // Modifiers
        uint32_t mdfFlags = 0;
        SWAG_CHECK(doModifiers(binaryNode->token, binaryNode->tokenId, mdfFlags));
        if (mdfFlags & MODIFIER_OVERFLOW)
        {
            binaryNode->specFlags |= AstOp::SPECFLAG_OVERFLOW;
            binaryNode->attributeFlags |= ATTRIBUTE_CAN_OVERFLOW_ON;
        }

        if (mdfFlags & MODIFIER_UP)
        {
            binaryNode->specFlags |= AstOp::SPECFLAG_UP;
        }

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(doFactorExpression((AstNode**) &binaryNode, exprFlags, &dummyResult));
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
        auto binaryNode         = (AstNode*) Ast::newNode<AstBinaryOpNode>(this, AstNodeKind::BinaryOp, sourceFile, parent ? *parent : nullptr);
        binaryNode->semanticFct = SemanticJob::resolveCompareExpression;
        binaryNode->token       = token;

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFactorExpression(&binaryNode, exprFlags, &dummyResult));
        SWAG_CHECK(doOperatorPrecedence(&binaryNode));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary && parent)
        Ast::addChildBack(*parent, leftNode);

    *result = leftNode;
    return true;
}

bool Parser::doCompareExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doFactorExpression(&parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);
    SWAG_CHECK(doOperatorPrecedence(&leftNode));

    if (exprFlags & EXPR_FLAG_STOP_AFFECT && token.id == TokenId::SymEqual)
    {
        Ast::addChildBack(parent, leftNode);
        *result = leftNode;
        return true;
    }

    SWAG_VERIFY(token.id != TokenId::SymEqual, error(token, Err(Err0432), Hnt(Hnt0082)));
    Ast::addChildBack(parent, leftNode);
    *result = leftNode;
    return true;
}

bool Parser::doBoolExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doCompareExpression(parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);

    bool isBinary = false;
    if ((token.id == TokenId::KwdOr) || (token.id == TokenId::KwdAnd))
    {
        auto binaryNode         = Ast::newNode<AstBinaryOpNode>(this, AstNodeKind::BinaryOp, sourceFile, parent);
        binaryNode->semanticFct = SemanticJob::resolveBoolExpression;
        binaryNode->token       = token;

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doBoolExpression(binaryNode, EXPR_FLAG_NONE, &dummyResult));
        leftNode = binaryNode;
        isBinary = true;
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);

    *result = leftNode;
    return true;
}

bool Parser::doMoveExpression(Token& forToken, TokenId tokenId, AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    uint32_t mdfFlags = 0;
    SWAG_CHECK(doModifiers(forToken, tokenId, mdfFlags));

    // nodrop left
    if (mdfFlags & MODIFIER_NO_LEFT_DROP)
    {
        auto exprNode         = Ast::newNode<AstNode>(this, AstNodeKind::NoDrop, sourceFile, parent);
        *result               = exprNode;
        exprNode->semanticFct = SemanticJob::resolveMove;
        exprNode->flags |= AST_NO_LEFT_DROP;
        parent = exprNode;
        result = &dummyResult;
    }

    // move
    if (mdfFlags & MODIFIER_MOVE)
    {
        auto exprNode         = Ast::newNode<AstNode>(this, AstNodeKind::Move, sourceFile, parent);
        *result               = exprNode;
        exprNode->semanticFct = SemanticJob::resolveMove;
        exprNode->flags |= AST_FORCE_MOVE;
        parent = exprNode;
        result = &dummyResult;

        // nodrop right
        if (mdfFlags & MODIFIER_NO_RIGHT_DROP)
        {
            exprNode              = Ast::newNode<AstNode>(this, AstNodeKind::NoDrop, sourceFile, parent);
            *result               = exprNode;
            exprNode->semanticFct = SemanticJob::resolveMove;
            exprNode->flags |= AST_NO_RIGHT_DROP;
            parent = exprNode;
            result = &dummyResult;
        }
    }

    SWAG_CHECK(doExpression(parent, exprFlags, result));
    return true;
}

bool Parser::doExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    AstNode* boolExpression = nullptr;
    switch (token.id)
    {
    case TokenId::CompilerRun:
    {
        ScopedFlags sf(this, AST_RUN_BLOCK);
        auto        node  = Ast::newNode<AstCompilerSpecFunc>(this, AstNodeKind::CompilerRunExpression, sourceFile, nullptr);
        node->semanticFct = SemanticJob::resolveCompilerRun;
        SWAG_CHECK(eatToken());

        // :RunGeneratedExp
        if (token.id == TokenId::SymLeftCurly)
        {
            *result = node;

            node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
            node->allocateExtension(ExtensionKind::Semantic);
            node->extSemantic()->semanticBeforeFct = SemanticJob::preResolveCompilerInstruction;

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

        SWAG_CHECK(doBoolExpression(node, exprFlags, &dummyResult));
        boolExpression = node;
        break;
    }
    case TokenId::CompilerMixin:
    {
        auto node         = Ast::newNode<AstCompilerMixin>(this, AstNodeKind::CompilerMixin, sourceFile, nullptr);
        node->semanticFct = SemanticJob::resolveCompilerMixin;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(node, &dummyResult, IDENTIFIER_NO_PARAMS));
        boolExpression = node;
        break;
    }
    case TokenId::CompilerCode:
    {
        auto node = Ast::newNode<AstNode>(this, AstNodeKind::CompilerCode, sourceFile, nullptr);
        SWAG_CHECK(eatToken());
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doEmbeddedStatement(node, &dummyResult));
        else
            SWAG_CHECK(doBoolExpression(node, exprFlags, &dummyResult));
        auto typeCode     = makeType<TypeInfoCode>();
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
        return error(token, Fmt(Err(Syn0020), token.ctext()));

    default:
        SWAG_CHECK(doBoolExpression(parent, exprFlags, &boolExpression));
        Ast::removeFromParent(boolExpression);
        break;
    }

    // A ? B : C
    if (token.id == TokenId::SymQuestion)
    {
        SWAG_CHECK(eatToken());
        auto triNode         = Ast::newNode<AstConditionalOpNode>(this, AstNodeKind::ConditionalExpression, sourceFile, parent);
        triNode->semanticFct = SemanticJob::resolveConditionalOp;
        *result              = triNode;
        Ast::addChildBack(triNode, boolExpression);

        SWAG_CHECK(doExpression(triNode, exprFlags, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doExpression(triNode, exprFlags, &dummyResult));
    }

    // A orelse B
    else if (token.id == TokenId::KwdOrElse)
    {
        auto triNode         = Ast::newNode<AstNode>(this, AstNodeKind::NullConditionalExpression, sourceFile, parent);
        triNode->semanticFct = SemanticJob::resolveNullConditionalOp;
        *result              = triNode;
        Ast::addChildBack(triNode, boolExpression);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(triNode, exprFlags, &dummyResult));
    }
    else
    {
        Ast::addChildBack(parent, boolExpression);
        *result = boolExpression;
    }

    return true;
}

bool Parser::doAssignmentExpression(AstNode* parent, AstNode** result)
{
    return doExpression(parent, EXPR_FLAG_NONE, result);
}

bool Parser::doExpressionListTuple(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, parent);
    *result               = initNode;
    initNode->semanticFct = SemanticJob::resolveExpressionListTuple;
    initNode->specFlags |= AstExpressionList::SPECFLAG_FOR_TUPLE;
    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken());

    while (token.id != TokenId::SymRightCurly)
    {
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListTuple(initNode, &dummyResult));
        else if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode, &dummyResult));
        else
        {
            AstNode* paramExpression;
            SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &paramExpression));

            // Name
            if (token.id == TokenId::SymColon)
            {
                SWAG_VERIFY(paramExpression->kind == AstNodeKind::IdentifierRef, error(paramExpression, Err(Syn0077)));
                SWAG_CHECK(checkIsSingleIdentifier(paramExpression, "as a tuple field name"));
                SWAG_CHECK(checkIsValidVarName(paramExpression->childs.back()));
                auto namedToFree     = paramExpression;
                auto namedExpression = namedToFree->childs.back();
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymLeftCurly)
                    SWAG_CHECK(doExpressionListTuple(initNode, &paramExpression));
                else
                    SWAG_CHECK(doExpression(initNode, EXPR_FLAG_NONE, &paramExpression));

                paramExpression->allocateExtension(ExtensionKind::Misc);
                paramExpression->extMisc()->isNamed  = namedExpression;
                paramExpression->token.startLocation = namedExpression->token.startLocation;
                paramExpression->allocateExtension(ExtensionKind::Owner);
                paramExpression->extOwner()->nodesToFree.push_back(namedToFree);
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

    initNode->token.endLocation = token.endLocation;
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the tuple body"));
    return true;
}

bool Parser::doExpressionListArray(AstNode* parent, AstNode** result)
{
    auto initNode         = Ast::newNode<AstExpressionList>(this, AstNodeKind::ExpressionList, sourceFile, parent);
    *result               = initNode;
    initNode->semanticFct = SemanticJob::resolveExpressionListArray;
    SWAG_CHECK(eatToken());

    if (token.id == TokenId::SymRightSquare)
        return error(token, Fmt(Err(Syn0124)));

    while (token.id != TokenId::SymRightSquare)
    {
        if (token.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode, &dummyResult));
        else if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListTuple(initNode, &dummyResult));
        else
            SWAG_CHECK(doExpression(initNode, EXPR_FLAG_NONE, &dummyResult));

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma));
    }

    initNode->token.endLocation = token.endLocation;
    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    return true;
}

bool Parser::doInitializationExpression(TokenParse& forToken, AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    // var x: type = undefined => not initialized
    if (token.id == TokenId::KwdUndefined)
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::ExplicitNoInit, sourceFile, parent);
        *result           = node;
        node->semanticFct = SemanticJob::resolveExplicitNoInit;
        if (parent)
            parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
        SWAG_CHECK(eatToken());
        return true;
    }

    return doMoveExpression(forToken, forToken.id, parent, exprFlags, result);
}

void Parser::forceTakeAddress(AstNode* node)
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
    default:
        break;
    }
}

bool Parser::doDefer(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstDefer>(this, AstNodeKind::Defer, sourceFile, parent);
    *result           = node;
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
            return error(token, Fmt(Err(Syn0142), token.ctext()), Hlp(Hlp0023));

        SWAG_CHECK(eatToken());
        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    ScopedFlags scopedFlags(this, AST_IN_DEFER);
    if (token.id == TokenId::SymLeftCurly)
        SWAG_CHECK(doScopedCurlyStatement(node, &dummyResult));
    else
        SWAG_CHECK(doLeftInstruction(node, &dummyResult));

    return true;
}

bool Parser::doLeftExpressionVar(AstNode* parent, AstNode** result, uint32_t identifierFlags, AstWith* withNode)
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
            SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::SymQuestion, error(token, Fmt(Err(Syn0042), token.ctext())));
            SWAG_CHECK(doIdentifierRef(multi, &dummyResult, identifierFlags | IDENTIFIER_ACCEPT_QUESTION));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen, "after tuple unpacking"));
        break;
    }

    case TokenId::Identifier:
    case TokenId::CompilerUp:
    case TokenId::CompilerSelf:
    {
        AstNode* exprNode     = nullptr;
        AstNode* multi        = nullptr;
        bool     prePrendWith = withNode != nullptr;
        while (true)
        {
            if (token.id == TokenId::SymDot)
            {
                SWAG_VERIFY(withNode, error(token, Err(Syn0180)));
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
                    id->specFlags |= AstIdentifier::SPECFLAG_FROM_WITH;
                    id->allocateIdentifierExtension();
                    id->identifierExtension->fromAlternateVar = withNode->childs.front();
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

bool Parser::doLeftExpressionAffect(AstNode* parent, AstNode** result, AstWith* withNode)
{
    switch (token.id)
    {
    case TokenId::IntrinsicGetContext:
    case TokenId::IntrinsicGetProcessInfos:
    case TokenId::IntrinsicDbgAlloc:
    case TokenId::IntrinsicSysAlloc:
        SWAG_CHECK(doIdentifierRef(parent, result));
        return true;
    case TokenId::SymLeftParen:
    case TokenId::Identifier:
    case TokenId::CompilerUp:
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
    {
        AstNode* id;
        SWAG_CHECK(doDeRef(parent, &id));
        *result = id;
        id->flags |= AST_L_VALUE;
        return true;
    }

    default:
        return invalidTokenError(InvalidTokenError::LeftExpression);
    }
}

bool Parser::doAffectExpression(AstNode* parent, AstNode** result, AstWith* withNode)
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
        SWAG_CHECK(doInitializationExpression(saveToken, parent, EXPR_FLAG_IN_VAR_DECL, &assign));
        Ast::removeFromParent(assign);
        SWAG_CHECK(doVarDeclExpression(parent, leftNode, nullptr, assign, saveToken, AstNodeKind::VarDecl, result));
        leftNode->release();
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
        {
            SWAG_CHECK(doModifiers(savedtoken, savedtoken.id, mdfFlags));
        }

        if (mdfFlags & MODIFIER_OVERFLOW)
        {
            opFlags |= AstOp::SPECFLAG_OVERFLOW;
            opAttrFlags |= ATTRIBUTE_CAN_OVERFLOW_ON;
        }

        // Multiple affectation
        // like in a, b, c = 0
        if (leftNode->kind == AstNodeKind::MultiIdentifier)
        {
            savedtoken.startLocation = token.startLocation;
            auto parentNode          = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            *result                  = parentNode;

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
                auto child          = leftNode->childs.front();
                auto affectNode     = Ast::newAffectOp(sourceFile, parentNode, opFlags, opAttrFlags);
                affectNode->token   = savedtoken;
                affectNode->tokenId = savedtoken.id;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);

                // First create 'firstVar = assignment'
                if (!firstDone)
                {
                    firstDone = true;
                    if (affectNode->tokenId == TokenId::SymEqual)
                        SWAG_CHECK(doMoveExpression(affectNode->token, affectNode->tokenId, affectNode, EXPR_FLAG_NONE, &affectExpression));
                    else
                        SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE, &affectExpression));
                }

                // This is not an initialization, so we need to duplicate the right expression
                else if (affectNode->tokenId != TokenId::SymEqual || affectExpression->kind == AstNodeKind::Literal)
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

            cloneFront->release();
            leftNode->release();
        }

        // Tuple unpacking
        else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
        {
            savedtoken.startLocation = token.startLocation;
            auto parentNode          = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            *result                  = parentNode;

            // Get right side
            AstNode* assignment;
            SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &assignment));

            token.startLocation = savedtoken.startLocation;
            token.endLocation   = savedtoken.endLocation;

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
            varNode->extSemantic()->semanticAfterFct = SemanticJob::resolveTupleUnpackBefore;

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
                affectNode->tokenId    = savedtoken.id;
                affectNode->token.text = savedtoken.text;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                forceTakeAddress(child);
                auto idRef = Ast::newMultiIdentifierRef(sourceFile, Fmt("%s.item%d", tmpVarName.c_str(), idx++), affectNode, this);

                // Force a move between the generated temporary variable and the real var
                idRef->flags |= AST_FORCE_MOVE;
            }

            leftNode->release();
        }

        // One normal simple affectation
        else
        {
            auto affectNode     = Ast::newAffectOp(sourceFile, parent, opFlags, opAttrFlags, this);
            affectNode->tokenId = savedtoken.id;
            affectNode->token   = std::move(savedtoken);

            Ast::addChildBack(affectNode, leftNode);
            forceTakeAddress(leftNode);

            if (affectNode->tokenId == TokenId::SymEqual)
                SWAG_CHECK(doMoveExpression(affectNode->token, affectNode->tokenId, affectNode, EXPR_FLAG_NONE, &dummyResult));
            else
                SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE, &dummyResult));

            // :DeduceLambdaType
            auto back = affectNode->childs.back();
            if (back->kind == AstNodeKind::MakePointerLambda && (back->specFlags & AstMakePointer::SPECFLAG_DEP_TYPE))
            {
                auto front = affectNode->childs.front();
                front->allocateExtension(ExtensionKind::Semantic);
                front->extSemantic()->semanticAfterFct = SemanticJob::resolveAfterKnownType;
            }

            *result = affectNode;
        }
    }
    else
    {
        Ast::addChildBack(parent, leftNode);
        *result = leftNode;
    }

    SWAG_VERIFY(token.id != TokenId::SymEqualEqual, error(token, Err(Syn0149)));

    if (token.id != TokenId::SymLeftCurly)
        SWAG_CHECK(eatSemiCol("left expression"));
    return true;
}

bool Parser::doInit(AstNode* parent, AstNode** result)
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

bool Parser::doDropCopyMove(AstNode* parent, AstNode** result)
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
    default:
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

bool Parser::doRange(AstNode* parent, AstNode* expression, AstNode** result)
{
    auto rangeNode         = Ast::newNode<AstRange>(this, AstNodeKind::Range, sourceFile, parent);
    *result                = rangeNode;
    rangeNode->semanticFct = SemanticJob::resolveRange;
    Ast::removeFromParent(expression);
    Ast::addChildBack(rangeNode, expression);
    rangeNode->expressionLow = expression;

    if (token.id == TokenId::SymDotDotLess)
        rangeNode->specFlags |= AstRange::SPECFLAG_EXCLUDE_UP;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(rangeNode, EXPR_FLAG_SIMPLE, &rangeNode->expressionUp));
    return true;
}
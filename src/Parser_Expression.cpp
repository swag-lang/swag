#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Semantic.h"
#include "TypeManager.h"

bool Parser::doLiteral(AstNode* parent, AstNode** result)
{
    const auto node    = Ast::newNode<AstLiteral>(AstNodeKind::Literal, this, parent);
    *result            = node;
    node->semanticFct  = Semantic::resolveLiteral;
    node->literalType  = tokenParse.literalType;
    node->literalValue = tokenParse.literalValue;

    SWAG_CHECK(eatToken());

    // Type suffix
    if (tokenParse.id == TokenId::SymQuote)
    {
        if (node->token.id == TokenId::LiteralCharacter || node->token.id == TokenId::LiteralNumber)
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.id == TokenId::Identifier || tokenParse.id == TokenId::NativeType, error(tokenParse, FMT(Err(Err0404), tokenParse.c_str())));
            const auto identifierRef = Ast::newIdentifierRef(this, node);
            SWAG_CHECK(doIdentifier(identifierRef, IDENTIFIER_NO_PARAMS | IDENTIFIER_TYPE_DECL));
            identifierRef->children.back()->semanticFct = Semantic::resolveLiteralSuffix;
        }
        else
        {
            switch (node->token.id)
            {
                case TokenId::KwdTrue:
                case TokenId::KwdFalse:
                    return error(tokenParse, FMT(Err(Err0703), "[[bool]] literals"));
                case TokenId::LiteralString:
                    return error(tokenParse, FMT(Err(Err0703), "[[string]] literals"));
                case TokenId::KwdNull:
                    return error(tokenParse, FMT(Err(Err0703), "[[null]]"));
                default:
                    return error(tokenParse, FMT(Err(Err0703), "that kind of literal"));
            }
        }
    }

    return true;
}

bool Parser::doArrayPointerIndex(AstNode** exprNode)
{
    const auto startToken = tokenParse.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftSquare, "to specify the index"));

    AstNode* firstExpr = nullptr;

    if (tokenParse.id == TokenId::SymDotDot || tokenParse.id == TokenId::SymDotDotLess)
    {
        const auto literal = Ast::newNode<AstLiteral>(AstNodeKind::Literal, this, nullptr);
        firstExpr          = literal;
        firstExpr->allocateComputedValue();
        firstExpr->addAstFlag(AST_GENERATED);
        firstExpr->computedValue()->reg.u64 = 0;
        firstExpr->semanticFct              = Semantic::resolveLiteral;
        literal->literalType                = LiteralType::TypeUnsigned64;
    }
    else
    {
        SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &firstExpr));
    }

    // Slicing
    if (tokenParse.id == TokenId::SymDotDot || tokenParse.id == TokenId::SymDotDotLess)
    {
        const auto arrayNode   = Ast::newNode<AstArrayPointerSlicing>(AstNodeKind::ArrayPointerSlicing, this, nullptr);
        arrayNode->semanticFct = Semantic::resolveArrayPointerSlicing;
        arrayNode->array       = *exprNode;
        Ast::addChildBack(arrayNode, *exprNode);
        Ast::addChildBack(arrayNode, firstExpr);
        arrayNode->lowerBound = firstExpr;
        if (tokenParse.id == TokenId::SymDotDotLess)
            arrayNode->addSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP);
        SWAG_CHECK(eatToken());

        if (tokenParse.id != TokenId::SymRightSquare)
        {
            SWAG_CHECK(doExpression(arrayNode, EXPR_FLAG_NONE, &arrayNode->upperBound));
        }

        // To the end...
        else
        {
            if (arrayNode->hasSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP))
            {
                const Diagnostic err{sourceFile, tokenParse, Err(Err0580)};
                return context->report(err);
            }

            arrayNode->upperBound = Ast::newNode<AstNode>(AstNodeKind::AutoSlicingUp, this, arrayNode);
            arrayNode->array->allocateExtension(ExtensionKind::Semantic);
            SWAG_ASSERT(!arrayNode->array->extSemantic()->semanticAfterFct);
            arrayNode->array->extSemantic()->semanticAfterFct = Semantic::resolveArrayPointerSlicingUpperBound;
            arrayNode->upperBound->addAstFlag(AST_GENERATED);
        }

        *exprNode = arrayNode;
        SWAG_CHECK(eatCloseToken(TokenId::SymRightSquare, startToken));
    }

    // Deref by index
    else
    {
        while (true)
        {
            const auto arrayNode   = Ast::newNode<AstArrayPointerIndex>(AstNodeKind::ArrayPointerIndex, this, nullptr);
            arrayNode->token       = firstExpr ? firstExpr->token : tokenParse;
            arrayNode->semanticFct = Semantic::resolveArrayPointerIndex;

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
            if (tokenParse.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.id != TokenId::SymRightSquare, error(tokenParse, Err(Err0128)));
        }

        SWAG_CHECK(eatCloseToken(TokenId::SymRightSquare, startToken));
    }

    return true;
}

bool Parser::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstIntrinsicProp>(AstNodeKind::IntrinsicProp, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveIntrinsicProperty;
    node->inheritTokenName(tokenParse);

    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, Err(Err0528)));
    SWAG_VERIFY(tokenParse.id != TokenId::SymRightParen, error(tokenParse, Err(Err0568)));

    // Three parameters
    if (node->token.id == TokenId::IntrinsicMakeInterface)
    {
        AstNode* params = Ast::newFuncCallParams(this, node);
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the second argument"));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "tp specify the third argument"));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
    }

    // Two parameters
    else if (node->token.id == TokenId::IntrinsicMakeSlice ||
             node->token.id == TokenId::IntrinsicMakeString ||
             node->token.id == TokenId::IntrinsicMakeAny)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the second argument"));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    // Two parameters
    else if (node->token.id == TokenId::IntrinsicCVaArg)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the second argument"));
        SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    // One single parameter
    else if (node->token.id == TokenId::IntrinsicTypeOf ||
             node->token.id == TokenId::IntrinsicKindOf ||
             node->token.id == TokenId::IntrinsicSizeOf ||
             node->token.id == TokenId::IntrinsicDeclType)
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_TYPEOF, &dummyResult));
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doSinglePrimaryExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    switch (tokenParse.id)
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
            const auto startLoc = tokenParse.startLocation;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.id != TokenId::SymRightParen, error(startLoc, tokenParse.endLocation, Err(Err0679)));
            AstNode* expr;
            SWAG_CHECK(doExpression(parent, exprFlags, &expr));
            *result = expr;
            expr->addAstFlag(AST_IN_ATOMIC_EXPR);
            if (parent)
                SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, FMT("to end the [[%s]] expression", parent->token.c_str())));
            else
                SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, "to end the left expression"));
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
        case TokenId::LiteralCharacter:
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
            castAst<AstIdentifierRef>(idref, AstNodeKind::IdentifierRef)->addSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE);
            break;
        }

        case TokenId::IntrinsicIndex:
            SWAG_CHECK(doIndex(parent, result));
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
        case TokenId::IntrinsicRol:
        case TokenId::IntrinsicRor:
        case TokenId::IntrinsicMulAdd:
            SWAG_CHECK(doIdentifierRef(parent, result));
            break;

        case TokenId::CompilerType:
        {
            if (exprFlags.has(EXPR_FLAG_SIMPLE))
                return invalidTokenError(InvalidTokenError::PrimaryExpression, parent);
            eatToken();
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, result));
            (*result)->addSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE);
            break;
        }

        case TokenId::KwdConst:
        case TokenId::KwdCode:
        case TokenId::KwdStruct:
        case TokenId::KwdUnion:
        case TokenId::NativeType:
        case TokenId::SymAsterisk:
        case TokenId::SymCircumflex:
        case TokenId::SymAmpersand:
            if (exprFlags.has(EXPR_FLAG_SIMPLE))
                return invalidTokenError(InvalidTokenError::PrimaryExpression, parent);
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, result));
            break;

        case TokenId::SymLeftCurly:
            SWAG_CHECK(doExpressionListTuple(parent, result));
            break;

        case TokenId::SymLeftSquare:
            if (exprFlags.has(EXPR_FLAG_TYPEOF | EXPR_FLAG_PARAMETER))
            {
                tokenizer.saveState(tokenParse);
                SWAG_CHECK(doExpressionListArray(parent, result));
                if (tokenParse.id != TokenId::SymRightParen && tokenParse.id != TokenId::SymRightCurly && tokenParse.id != TokenId::SymComma)
                {
                    tokenizer.restoreState(tokenParse);
                    Ast::removeFromParent(parent->children.back());
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
            if (exprFlags.has(EXPR_FLAG_SIMPLE))
                return invalidTokenError(InvalidTokenError::PrimaryExpression, parent);

            if (exprFlags.has(EXPR_FLAG_TYPEOF))
                SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_TYPE_EXPR, result));
            else
                SWAG_CHECK(doLambdaExpression(parent, exprFlags, result));
            break;

        case TokenId::KwdCast:
            SWAG_CHECK(doCast(parent, result));
            break;

        default:
            return invalidTokenError(InvalidTokenError::PrimaryExpression, parent);
    }

    return true;
}

bool Parser::doKeepRef(AstNode* parent, AstNode** result)
{
    const auto refNode   = Ast::newNode<AstNode>(AstNodeKind::KeepRef, this, parent);
    *result              = refNode;
    refNode->semanticFct = Semantic::resolveKeepRef;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(refNode, EXPR_FLAG_SIMPLE, &dummyResult));
    return true;
}

bool Parser::doMoveRef(AstNode* parent, AstNode** result)
{
    const auto refNode   = Ast::newNode<AstNode>(AstNodeKind::MoveRef, this, parent);
    *result              = refNode;
    refNode->semanticFct = Semantic::resolveMoveRef;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(refNode, EXPR_FLAG_SIMPLE, &dummyResult));
    return true;
}

bool Parser::doDeRef(AstNode* parent, AstNode** result)
{
    const auto identifierRef = Ast::newIdentifierRef(this, parent);
    const auto arrayNode     = Ast::newNode<AstArrayPointerIndex>(AstNodeKind::ArrayPointerIndex, this, identifierRef);
    arrayNode->semanticFct   = Semantic::resolveArrayPointerIndex;
    arrayNode->specFlags     = AstArrayPointerIndex::SPEC_FLAG_IS_DEREF;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doUnaryExpression(arrayNode, EXPR_FLAG_SIMPLE, &arrayNode->array));

    const auto literal = Ast::newNode<AstLiteral>(AstNodeKind::Literal, this, arrayNode);
    literal->setFlagsValueIsComputed();
    literal->computedValue()->reg.u64 = 0;
    literal->literalType              = LiteralType::TypeSigned32;
    literal->semanticFct              = Semantic::resolveLiteral;
    literal->inheritTokenLocation(arrayNode->array->token);
    arrayNode->access = literal;
    *result           = identifierRef;
    return true;
}

bool Parser::doPrimaryExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    AstNode* exprNode;

    // Take pointer
    if (tokenParse.id == TokenId::SymAmpersand)
    {
        exprNode              = Ast::newNode<AstMakePointer>(AstNodeKind::MakePointer, this, nullptr);
        exprNode->semanticFct = Semantic::resolveMakePointer;
        SWAG_CHECK(eatToken());

        bool hasDot = false;
        if (tokenParse.id == TokenId::SymDot)
        {
            SWAG_CHECK(eatToken());
            hasDot = true;
        }

        AstNode* identifierRef;
        SWAG_CHECK(doIdentifierRef(nullptr, &identifierRef));
        isForceTakeAddress(identifierRef);

        if (hasDot)
            identifierRef->addSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE);

        if (tokenParse.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doArrayPointerIndex(&identifierRef));

        Ast::addChildBack(exprNode, identifierRef);
        identifierRef->addAstFlag(AST_TAKE_ADDRESS);
        if (parent)
            Ast::addChildBack(parent, exprNode);
    }

    // Dereference pointer
    else if (tokenParse.id == TokenId::KwdDeRef)
    {
        SWAG_CHECK(doDeRef(parent, &exprNode));
    }

    // Force ref pointer
    else if (tokenParse.id == TokenId::KwdConst)
    {
        const auto startLoc = tokenParse.startLocation;
        tokenizer.saveState(tokenParse);
        eatToken();
        if (tokenParse.id == TokenId::KwdRef)
        {
            SWAG_CHECK(doKeepRef(parent, &exprNode));
            exprNode->addAstFlag(AST_IS_CONST);
            exprNode->token.startLocation = startLoc;
        }
        else
        {
            tokenizer.restoreState(tokenParse);
            SWAG_CHECK(doSinglePrimaryExpression(parent, exprFlags, &exprNode));
        }
    }
    else if (tokenParse.id == TokenId::KwdRef)
    {
        SWAG_CHECK(doKeepRef(parent, &exprNode));
    }
    else if (tokenParse.id == TokenId::KwdMoveRef)
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

bool Parser::doUnaryExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    switch (tokenParse.id)
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
            const auto node   = Ast::newNode<AstNode>(AstNodeKind::SingleOp, this, parent);
            *result           = node;
            node->semanticFct = Semantic::resolveUnaryOp;
            node->token       = static_cast<Token>(tokenParse);
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.id != prevTokenParse.id, error(tokenParse, FMT(Err(Err0071), tokenParse.c_str())));
            SWAG_VERIFY(tokenParse.id != TokenId::KwdDeRef, error(tokenParse, FMT(Err(Err0282), prevTokenParse.c_str(), tokenParse.c_str(), prevTokenParse.c_str())));
            return doSinglePrimaryExpression(node, exprFlags, &dummyResult);
        }
        default:
            break;
    }

    return doPrimaryExpression(parent, exprFlags, result);
}

bool Parser::doModifiers(const Token& forNode, TokenId tokenId, ModifierFlags& mdfFlags, AstNode* node)
{
    const auto opId = tokenId;

    mdfFlags = 0;
    while (tokenParse.id == TokenId::SymComma && !tokenParse.flags.has(TOKEN_PARSE_LAST_BLANK) && !tokenParse.flags.has(TOKEN_PARSE_LAST_EOL))
    {
        SWAG_CHECK(eatToken());

        if (tokenParse.text == g_LangSpec->name_up)
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
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_UP), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            mdfFlags.add(MODIFIER_UP);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.text == g_LangSpec->name_over)
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
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_OVERFLOW), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            mdfFlags.add(MODIFIER_OVERFLOW);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.text == g_LangSpec->name_nodrop)
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_NO_LEFT_DROP), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            mdfFlags.add(MODIFIER_NO_LEFT_DROP);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.text == g_LangSpec->name_back)
        {
            switch (opId)
            {
                case TokenId::KwdLoop:
                case TokenId::KwdVisit:
                    break;
                default:
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_BACK), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            mdfFlags.add(MODIFIER_BACK);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.text == g_LangSpec->name_bit)
        {
            switch (opId)
            {
                case TokenId::KwdCast:
                    break;
                default:
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_BIT), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            mdfFlags.add(MODIFIER_BIT);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.text == g_LangSpec->name_unconst)
        {
            switch (opId)
            {
                case TokenId::KwdCast:
                    break;
                default:
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_UN_CONST), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            mdfFlags.add(MODIFIER_UN_CONST);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.text == g_LangSpec->name_move)
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_MOVE), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            mdfFlags.add(MODIFIER_MOVE);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.text == g_LangSpec->name_moveraw)
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, FMT(Err(Err0694), tokenParse.c_str(), forNode.c_str()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_NO_RIGHT_DROP), error(tokenParse, FMT(Err(Err0070), tokenParse.c_str())));
            SWAG_VERIFY(!mdfFlags.has(MODIFIER_MOVE), error(tokenParse, FMT(Err(Err0070), g_LangSpec->name_move.c_str())));
            mdfFlags.add(MODIFIER_MOVE | MODIFIER_NO_RIGHT_DROP);
            SWAG_CHECK(eatToken());
            continue;
        }

        return error(tokenParse, FMT(Err(Err0352), tokenParse.c_str()));
    }

    return true;
}

namespace
{
    int getPrecedence(TokenId id)
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
            case TokenId::KwdAnd:
                return 10;
            case TokenId::KwdOr:
                return 11;
            default:
                break;
        }

        return -1;
    }

    bool isAssociative(TokenId id)
    {
        switch (id)
        {
            case TokenId::SymPlus:
            case TokenId::SymAsterisk:
            case TokenId::SymVertical:
            case TokenId::SymCircumflex:
            case TokenId::SymPlusPlus:
            case TokenId::KwdAnd:
            case TokenId::KwdOr:
                return true;
            default:
                break;
        }

        return false;
    }

    bool doOperatorPrecedence(AstNode** result)
    {
        auto factor = *result;
        if (factor->kind != AstNodeKind::FactorOp && factor->kind != AstNodeKind::BinaryOp)
            return true;

        auto left = factor->children[0];
        SWAG_CHECK(doOperatorPrecedence(&left));
        auto right = factor->children[1];
        SWAG_CHECK(doOperatorPrecedence(&right));

        if ((right->kind == AstNodeKind::FactorOp || right->kind == AstNodeKind::BinaryOp) && !right->hasAstFlag(AST_IN_ATOMIC_EXPR))
        {
            const auto myPrecedence    = getPrecedence(factor->token.id);
            const auto rightPrecedence = getPrecedence(right->token.id);

            bool shuffle = false;
            if (myPrecedence < rightPrecedence && myPrecedence != -1 && rightPrecedence != -1)
                shuffle = true;

            // If operation is not associative, then we need to shuffle
            //
            // 2 - 1 - 1 needs to be treated as (2 - 1) - 1 and not 2 - (2 - 1)
            //
            else if (!isAssociative(factor->token.id) && myPrecedence == rightPrecedence)
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
                const auto atom = factor->hasAstFlag(AST_IN_ATOMIC_EXPR);
                factor->removeAstFlag(AST_IN_ATOMIC_EXPR);

                const auto leftRight = right->children[0];
                Ast::removeFromParent(right);
                if (factor->parent && factor == factor->parent->children.front())
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
                    factor->addAstFlag(AST_IN_ATOMIC_EXPR);
            }
        }

        *result = factor;
        return true;
    }
}

bool Parser::doFactorExpression(AstNode** parent, ExprFlags exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_ASSERT(parent);
    SWAG_CHECK(doUnaryExpression(*parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);

    bool isBinary = false;
    if (tokenParse.id == TokenId::SymPlus ||
        tokenParse.id == TokenId::SymMinus ||
        tokenParse.id == TokenId::SymAsterisk ||
        tokenParse.id == TokenId::SymSlash ||
        tokenParse.id == TokenId::SymPercent ||
        tokenParse.id == TokenId::SymAmpersand ||
        tokenParse.id == TokenId::SymVertical ||
        tokenParse.id == TokenId::SymGreaterGreater ||
        tokenParse.id == TokenId::SymLowerLower ||
        tokenParse.id == TokenId::SymPlusPlus ||
        tokenParse.id == TokenId::SymCircumflex)
    {
        auto binaryNode = Ast::newNode<AstOp>(AstNodeKind::FactorOp, this, *parent);

        if (tokenParse.id == TokenId::SymGreaterGreater || tokenParse.id == TokenId::SymLowerLower)
            binaryNode->semanticFct = Semantic::resolveShiftExpression;
        else
            binaryNode->semanticFct = Semantic::resolveFactorExpression;
        binaryNode->token = static_cast<Token>(tokenParse);
        SWAG_CHECK(eatToken());

        // Modifiers
        ModifierFlags mdfFlags = 0;
        SWAG_CHECK(doModifiers(binaryNode->token, binaryNode->token.id, mdfFlags));
        if (mdfFlags.has(MODIFIER_OVERFLOW))
        {
            binaryNode->addSpecFlag(AstOp::SPEC_FLAG_OVERFLOW);
            binaryNode->addAttribute(ATTRIBUTE_CAN_OVERFLOW_ON);
        }

        if (mdfFlags.has(MODIFIER_UP))
        {
            binaryNode->addSpecFlag(AstOp::SPEC_FLAG_UP);
        }

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(doFactorExpression(reinterpret_cast<AstNode**>(&binaryNode), exprFlags, &dummyResult));
        SWAG_CHECK(doOperatorPrecedence(reinterpret_cast<AstNode**>(&binaryNode)));
        leftNode = binaryNode;
        isBinary = true;
    }
    else if (tokenParse.id == TokenId::SymEqualEqual ||
             tokenParse.id == TokenId::SymExclamEqual ||
             tokenParse.id == TokenId::SymLowerEqual ||
             tokenParse.id == TokenId::SymGreaterEqual ||
             tokenParse.id == TokenId::SymLower ||
             tokenParse.id == TokenId::SymGreater ||
             tokenParse.id == TokenId::SymLowerEqualGreater)
    {
        auto binaryNode         = castAst<AstNode>(Ast::newNode<AstBinaryOpNode>(AstNodeKind::BinaryOp, this, parent ? *parent : nullptr));
        binaryNode->semanticFct = Semantic::resolveCompareExpression;
        binaryNode->token       = static_cast<Token>(tokenParse);

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

bool Parser::doCompareExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doFactorExpression(&parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);

    if (exprFlags.has(EXPR_FLAG_STOP_AFFECT) && tokenParse.id == TokenId::SymEqual)
    {
        Ast::addChildBack(parent, leftNode);
        *result = leftNode;
        return true;
    }

    if (!exprFlags.has(EXPR_FLAG_NAMED_PARAM) || leftNode->kind != AstNodeKind::IdentifierRef)
        SWAG_VERIFY(tokenParse.id != TokenId::SymEqual, error(tokenParse, Err(Err0674), Nte(Nte0086)));

    Ast::addChildBack(parent, leftNode);
    *result = leftNode;
    return true;
}

bool Parser::doBoolExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    AstNode* leftNode;
    SWAG_CHECK(doCompareExpression(parent, exprFlags, &leftNode));
    Ast::removeFromParent(leftNode);
    SWAG_CHECK(doOperatorPrecedence(&leftNode));

    bool isBinary = false;
    if (tokenParse.id == TokenId::KwdOr || tokenParse.id == TokenId::KwdAnd)
    {
        const auto binaryNode   = Ast::newNode<AstBinaryOpNode>(AstNodeKind::BinaryOp, this, parent);
        binaryNode->semanticFct = Semantic::resolveBoolExpression;
        binaryNode->token       = static_cast<Token>(tokenParse);

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doBoolExpression(binaryNode, EXPR_FLAG_NONE, &dummyResult));
        leftNode = binaryNode;
        isBinary = true;
    }
    else if (tokenParse.id == TokenId::SymAmpersandAmpersand || tokenParse.id == TokenId::SymVerticalVertical)
    {
        return invalidTokenError(InvalidTokenError::EmbeddedInstruction);
    }

    if (!isBinary)
        Ast::addChildBack(parent, leftNode);
    else
        SWAG_CHECK(doOperatorPrecedence(&leftNode));

    *result = leftNode;
    return true;
}

bool Parser::doMoveExpression(const Token& forToken, TokenId tokenId, AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(forToken, tokenId, mdfFlags));

    // no drop left
    if (mdfFlags.has(MODIFIER_NO_LEFT_DROP))
    {
        const auto exprNode   = Ast::newNode<AstNode>(AstNodeKind::NoDrop, this, parent);
        *result               = exprNode;
        exprNode->semanticFct = Semantic::resolveMove;
        exprNode->addAstFlag(AST_NO_LEFT_DROP);
        parent = exprNode;
        result = &dummyResult;
    }

    // move
    if (mdfFlags.has(MODIFIER_MOVE))
    {
        auto exprNode         = Ast::newNode<AstNode>(AstNodeKind::Move, this, parent);
        *result               = exprNode;
        exprNode->semanticFct = Semantic::resolveMove;
        exprNode->addAstFlag(AST_FORCE_MOVE);
        parent = exprNode;
        result = &dummyResult;

        // no drop right
        if (mdfFlags.has(MODIFIER_NO_RIGHT_DROP))
        {
            exprNode              = Ast::newNode<AstNode>(AstNodeKind::NoDrop, this, parent);
            *result               = exprNode;
            exprNode->semanticFct = Semantic::resolveMove;
            exprNode->addAstFlag(AST_NO_RIGHT_DROP);
            parent = exprNode;
            result = &dummyResult;
        }
    }

    SWAG_CHECK(doExpression(parent, exprFlags, result));
    return true;
}

bool Parser::doExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    AstNode* boolExpression = nullptr;
    switch (tokenParse.id)
    {
        case TokenId::CompilerRun:
        {
            ScopedFlags sf(this, AST_IN_RUN_BLOCK);
            const auto  node  = Ast::newNode<AstCompilerSpecFunc>(AstNodeKind::CompilerRunExpression, this, nullptr);
            node->semanticFct = Semantic::resolveCompilerRun;
            SWAG_CHECK(eatToken());

            // :RunGeneratedExp
            if (tokenParse.id == TokenId::SymLeftCurly)
            {
                *result = node;

                node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
                node->allocateExtension(ExtensionKind::Semantic);
                node->extSemantic()->semanticBeforeFct = Semantic::preResolveCompilerInstruction;

                AstNode* funcNode;
                SWAG_CHECK(doFuncDecl(node, &funcNode, TokenId::CompilerGeneratedRunExp));
                funcNode->addAttribute(ATTRIBUTE_COMPILER);

                const auto idRef                = Ast::newIdentifierRef(funcNode->token.text, this, node);
                idRef->token.startLocation      = node->token.startLocation;
                idRef->token.endLocation        = node->token.endLocation;
                const auto identifier           = castAst<AstIdentifier>(idRef->children.back(), AstNodeKind::Identifier);
                identifier->callParameters      = Ast::newFuncCallParams(this, identifier);
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
            const auto node   = Ast::newNode<AstCompilerMixin>(AstNodeKind::CompilerMixin, this, nullptr);
            node->semanticFct = Semantic::resolveCompilerMixin;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doIdentifierRef(node, &dummyResult, IDENTIFIER_NO_PARAMS));
            boolExpression = node;
            break;
        }
        case TokenId::CompilerCode:
        {
            const auto node = Ast::newNode<AstNode>(AstNodeKind::CompilerCode, this, nullptr);
            SWAG_CHECK(eatToken());
            if (tokenParse.id == TokenId::SymLeftCurly)
                SWAG_CHECK(doScopedCurlyStatement(node, &dummyResult));
            else
                SWAG_CHECK(doBoolExpression(node, exprFlags, &dummyResult));
            const auto typeCode = makeType<TypeInfoCode>();
            typeCode->content   = node->children.front();
            typeCode->content->addAstFlag(AST_NO_SEMANTIC);
            node->typeInfo = typeCode;
            node->addAstFlag(AST_NO_BYTECODE);
            boolExpression = node;
            break;
        }

        case TokenId::CompilerAst:
        case TokenId::CompilerFuncInit:
        case TokenId::CompilerFuncDrop:
        case TokenId::CompilerFuncPreMain:
        case TokenId::CompilerFuncMain:
        case TokenId::CompilerFuncTest:
            return error(tokenParse, FMT(Err(Err0285), tokenParse.c_str()));

        default:
            SWAG_CHECK(doBoolExpression(parent, exprFlags, &boolExpression));
            Ast::removeFromParent(boolExpression);
            break;
    }

    // A ? B : C
    if (tokenParse.id == TokenId::SymQuestion)
    {
        SWAG_CHECK(eatToken());
        const auto triNode   = Ast::newNode<AstConditionalOpNode>(AstNodeKind::ConditionalExpression, this, parent);
        triNode->semanticFct = Semantic::resolveConditionalOp;
        *result              = triNode;
        Ast::addChildBack(triNode, boolExpression);

        SWAG_CHECK(doExpression(triNode, exprFlags, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymColon, "to specify the ternary expression second argument"));
        SWAG_CHECK(doExpression(triNode, exprFlags, &dummyResult));
    }

    // A orelse B
    else if (tokenParse.id == TokenId::KwdOrElse)
    {
        const auto triNode   = Ast::newNode<AstNode>(AstNodeKind::NullConditionalExpression, this, parent);
        triNode->semanticFct = Semantic::resolveNullConditionalOp;
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
    const auto initNode   = Ast::newNode<AstExpressionList>(AstNodeKind::ExpressionList, this, parent);
    *result               = initNode;
    initNode->semanticFct = Semantic::resolveExpressionListTuple;
    initNode->addSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE);
    const auto startLoc = tokenParse.startLocation;
    SWAG_CHECK(eatToken());

    while (tokenParse.id != TokenId::SymRightCurly)
    {
        if (tokenParse.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListTuple(initNode, &dummyResult));
        else if (tokenParse.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode, &dummyResult));
        else
        {
            AstNode* paramExpression;
            SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NAMED_PARAM, &paramExpression));

            // Name
            if (tokenParse.id == TokenId::SymColon)
            {
                SWAG_VERIFY(paramExpression->kind == AstNodeKind::IdentifierRef, error(paramExpression, FMT(Err(Err0310), tokenParse.c_str())));
                SWAG_CHECK(checkIsSingleIdentifier(paramExpression, "as a tuple field name"));
                SWAG_CHECK(checkIsValidVarName(paramExpression->children.back()));
                auto       namedToFree     = paramExpression;
                const auto namedExpression = namedToFree->children.back();
                SWAG_CHECK(eatToken());
                if (tokenParse.id == TokenId::SymLeftCurly)
                    SWAG_CHECK(doExpressionListTuple(initNode, &paramExpression));
                else
                    SWAG_CHECK(doExpression(initNode, EXPR_FLAG_NONE, &paramExpression));

                paramExpression->addExtraPointer(ExtraPointerKind::IsNamed, namedExpression);
                paramExpression->token.startLocation = namedExpression->token.startLocation;
                paramExpression->allocateExtension(ExtensionKind::Owner);
                paramExpression->extOwner()->nodesToFree.push_back(namedToFree);
            }
            else
            {
                Ast::addChildBack(initNode, paramExpression);
            }
        }

        if (tokenParse.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    initNode->token.endLocation = tokenParse.endLocation;
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the tuple body"));
    return true;
}

bool Parser::doExpressionListArray(AstNode* parent, AstNode** result)
{
    const auto initNode   = Ast::newNode<AstExpressionList>(AstNodeKind::ExpressionList, this, parent);
    *result               = initNode;
    initNode->semanticFct = Semantic::resolveExpressionListArray;
    const auto startLoc   = tokenParse.startLocation;
    SWAG_CHECK(eatToken());

    if (tokenParse.id == TokenId::SymRightSquare)
        return error(tokenParse, FMT(Err(Err0077)));

    while (tokenParse.id != TokenId::SymRightSquare)
    {
        if (tokenParse.id == TokenId::SymLeftSquare)
            SWAG_CHECK(doExpressionListArray(initNode, &dummyResult));
        else if (tokenParse.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doExpressionListTuple(initNode, &dummyResult));
        else
            SWAG_CHECK(doExpression(initNode, EXPR_FLAG_NONE, &dummyResult));

        if (tokenParse.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    initNode->token.endLocation = tokenParse.endLocation;
    SWAG_CHECK(eatCloseToken(TokenId::SymRightSquare, startLoc));
    return true;
}

bool Parser::doInitializationExpression(const TokenParse& forToken, AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    // var x: type = undefined => not initialized
    if (tokenParse.id == TokenId::KwdUndefined)
    {
        const auto node   = Ast::newNode<AstNode>(AstNodeKind::ExplicitNoInit, this, parent);
        *result           = node;
        node->semanticFct = Semantic::resolveExplicitNoInit;
        if (parent)
            parent->addAstFlag(AST_EXPLICITLY_NOT_INITIALIZED);
        SWAG_CHECK(eatToken());
        return true;
    }

    return doMoveExpression(forToken, forToken.id, parent, exprFlags, result);
}

void Parser::isForceTakeAddress(AstNode* node)
{
    node->addAstFlag(AST_TAKE_ADDRESS);
    switch (node->kind)
    {
        case AstNodeKind::IdentifierRef:
            isForceTakeAddress(node->children.back());
            break;
        case AstNodeKind::ArrayPointerIndex:
            isForceTakeAddress(castAst<AstArrayPointerIndex>(node)->array);
            break;
        default:
            break;
    }
}

bool Parser::doLeftExpressionVar(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags, const AstWith* withNode)
{
    switch (tokenParse.id)
    {
        case TokenId::SymLeftParen:
        {
            const auto multi    = Ast::newNode<AstNode>(AstNodeKind::MultiIdentifierTuple, this, nullptr);
            *result             = multi;
            const auto startLoc = tokenParse.startLocation;
            SWAG_CHECK(eatToken());
            while (true)
            {
                SWAG_VERIFY(tokenParse.id == TokenId::Identifier || tokenParse.id == TokenId::SymQuestion, error(tokenParse, FMT(Err(Err0698), tokenParse.c_str())));
                SWAG_CHECK(doIdentifierRef(multi, &dummyResult, identifierFlags | IDENTIFIER_ACCEPT_QUESTION));
                if (tokenParse.id != TokenId::SymComma)
                    break;
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(tokenParse.id != TokenId::SymRightParen, error(tokenParse, Err(Err0133)));
            }

            SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, "to end the tuple unpacking"));
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
                if (tokenParse.id == TokenId::SymDot)
                {
                    SWAG_VERIFY(withNode, error(tokenParse, Err(Err0507)));
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
                    for (int wi = static_cast<int>(withNode->id.size()) - 1; wi >= 0; wi--)
                    {
                        const auto id = Ast::newIdentifier(castAst<AstIdentifierRef>(exprNode), withNode->id[wi], this, exprNode);
                        id->addAstFlag(AST_GENERATED);
                        id->addSpecFlag(AstIdentifier::SPEC_FLAG_FROM_WITH);
                        id->allocateIdentifierExtension();
                        id->identifierExtension->fromAlternateVar = withNode->children.front();
                        id->inheritTokenLocation(exprNode->token);
                        exprNode->children.pop_back();
                        Ast::addChildFront(exprNode, id);
                    }
                }

                if (tokenParse.id != TokenId::SymComma)
                    break;
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(tokenParse.id != TokenId::SymEqual && tokenParse.id != TokenId::SymSemiColon, error(tokenParse, Err(Err0134)));

                if (!multi)
                {
                    multi = Ast::newNode<AstNode>(AstNodeKind::MultiIdentifier, this, parent);
                    Ast::addChildBack(multi, exprNode);
                }
            }

            Ast::removeFromParent(multi);
            *result = multi ? multi : exprNode;
            break;
        }

        default:
        {
            Diagnostic err{sourceFile, tokenParse, FMT(Err(Err0408), tokenParse.c_str())};
            if (Tokenizer::isKeyword(tokenParse.id))
                err.addNote(FMT(Nte(Nte0125), tokenParse.c_str()));
            return context->report(err);
        }
    }

    return true;
}

bool Parser::doLeftExpressionAffect(AstNode* parent, AstNode** result, const AstWith* withNode)
{
    switch (tokenParse.id)
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
            id->addAstFlag(AST_L_VALUE);
            return true;
        }

        default:
            return invalidTokenError(InvalidTokenError::LeftExpression);
    }
}

bool Parser::doAffectExpression(AstNode* parent, AstNode** result, const AstWith* withNode)
{
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpressionAffect(parent, &leftNode, withNode));
    Ast::removeFromParent(leftNode);

    // Affect operator
    if (tokenParse.id == TokenId::SymEqual ||
        tokenParse.id == TokenId::SymPlusEqual ||
        tokenParse.id == TokenId::SymMinusEqual ||
        tokenParse.id == TokenId::SymAsteriskEqual ||
        tokenParse.id == TokenId::SymSlashEqual ||
        tokenParse.id == TokenId::SymAmpersandEqual ||
        tokenParse.id == TokenId::SymVerticalEqual ||
        tokenParse.id == TokenId::SymCircumflexEqual ||
        tokenParse.id == TokenId::SymPercentEqual ||
        tokenParse.id == TokenId::SymLowerLowerEqual ||
        tokenParse.id == TokenId::SymGreaterGreaterEqual)
    {
        SpecFlags      opFlags     = 0;
        AttributeFlags opAttrFlags = 0;
        auto           savedtoken  = tokenParse;
        SWAG_CHECK(eatToken());

        // Modifiers
        ModifierFlags mdfFlags = 0;
        if (savedtoken.id != TokenId::SymEqual)
        {
            SWAG_CHECK(doModifiers(savedtoken, savedtoken.id, mdfFlags));
        }

        if (mdfFlags.has(MODIFIER_OVERFLOW))
        {
            opFlags.add(AstOp::SPEC_FLAG_OVERFLOW);
            opAttrFlags.add(ATTRIBUTE_CAN_OVERFLOW_ON);
        }

        // Multiple affectation
        // like in a, b, c = 0
        if (leftNode->kind == AstNodeKind::MultiIdentifier)
        {
            savedtoken.startLocation = tokenParse.startLocation;
            const auto parentNode    = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
            *result                  = parentNode;

            // Generate an expression of the form "var firstVar = assignment", and "secondVar = firstVar" for the rest
            // This will avoid to do the right expression multiple times (if this is a function call for example).
            //
            // If this is not the '=' operator, then we have to duplicate the affectation for each variable
            // If the affect expression is a literal, it's better to duplicate also
            AstNode*   affectExpression = nullptr;
            bool       firstDone        = false;
            const auto front            = castAst<AstIdentifierRef>(leftNode->children.front(), AstNodeKind::IdentifierRef);
            front->computeName();

            const auto cloneFront = Ast::clone(front, nullptr);

            while (!leftNode->children.empty())
            {
                const auto child      = leftNode->children.front();
                const auto affectNode = Ast::newAffectOp(opFlags, opAttrFlags, nullptr, parentNode);
                affectNode->token     = static_cast<Token>(savedtoken);
                affectNode->token.id  = savedtoken.id;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                isForceTakeAddress(child);

                // First create 'firstVar = assignment'
                if (!firstDone)
                {
                    firstDone = true;
                    if (affectNode->token.id == TokenId::SymEqual)
                        SWAG_CHECK(doMoveExpression(affectNode->token, affectNode->token.id, affectNode, EXPR_FLAG_NONE, &affectExpression));
                    else
                        SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE, &affectExpression));
                }

                // This is not an initialization, so we need to duplicate the right expression
                else if (affectNode->token.id != TokenId::SymEqual || affectExpression->kind == AstNodeKind::Literal)
                {
                    const auto newAffect = Ast::clone(affectExpression, affectNode);
                    newAffect->inheritTokenLocation(affectExpression->token);
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
            savedtoken.startLocation = tokenParse.startLocation;
            const auto parentNode    = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
            *result                  = parentNode;

            // Get right side
            AstNode* assignment;
            SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &assignment));

            tokenParse.startLocation = savedtoken.startLocation;
            tokenParse.endLocation   = savedtoken.endLocation;

            // Generate an expression of the form "var __tmp_0 = assignment"
            const auto  tmpVarName = FMT("__4tmp_%d", g_UniqueID.fetch_add(1));
            AstVarDecl* varNode    = Ast::newVarDecl(tmpVarName, this, parentNode);
            varNode->addAstFlag(AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS);
            Ast::addChildBack(varNode, assignment);
            assignment->inheritOwners(varNode);
            varNode->assignment = assignment;
            varNode->assignment->addAstFlag(AST_NO_LEFT_DROP);

            varNode->token.startLocation = leftNode->children.front()->token.startLocation;
            varNode->token.endLocation   = leftNode->children.back()->token.endLocation;
            varNode->allocateExtension(ExtensionKind::Semantic);
            varNode->extSemantic()->semanticAfterFct = Semantic::resolveTupleUnpackBefore;

            // And reference that variable, in the form value = __tmp_0.item?
            int idx = 0;
            while (!leftNode->children.empty())
            {
                const auto child = leftNode->children.front();

                // Ignore field if '?', otherwise check that this is a valid variable name
                if (child->children.front()->token.text == '?')
                {
                    idx++;
                    Ast::removeFromParent(child);
                    Ast::addChildBack(parentNode, child);
                    child->addAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE);
                    continue;
                }

                const auto affectNode  = Ast::newAffectOp(opFlags, opAttrFlags, nullptr, parentNode);
                affectNode->token.id   = savedtoken.id;
                affectNode->token.text = savedtoken.text;
                Ast::removeFromParent(child);
                Ast::addChildBack(affectNode, child);
                isForceTakeAddress(child);
                const auto idRef = Ast::newMultiIdentifierRef(FMT("%s.item%u", tmpVarName.c_str(), idx++), this, affectNode);

                // Force a move between the generated temporary variable and the real var
                idRef->addAstFlag(AST_FORCE_MOVE);
            }

            leftNode->release();
        }

        // One normal simple affectation
        else
        {
            const auto affectNode = Ast::newAffectOp(opFlags, opAttrFlags, this, parent);
            affectNode->token.id  = savedtoken.id;
            affectNode->token     = static_cast<Token>(std::move(savedtoken));

            Ast::addChildBack(affectNode, leftNode);
            isForceTakeAddress(leftNode);

            if (affectNode->token.id == TokenId::SymEqual)
                SWAG_CHECK(doMoveExpression(affectNode->token, affectNode->token.id, affectNode, EXPR_FLAG_NONE, &dummyResult));
            else
                SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE, &dummyResult));

            // :DeduceLambdaType
            const auto back = affectNode->children.back();
            if (back->kind == AstNodeKind::MakePointerLambda && back->hasSpecFlag(AstMakePointer::SPEC_FLAG_DEP_TYPE))
            {
                const auto front = affectNode->children.front();
                front->allocateExtension(ExtensionKind::Semantic);
                front->extSemantic()->semanticAfterFct = Semantic::resolveAfterKnownType;
            }

            *result = affectNode;
        }
    }
    else
    {
        Ast::addChildBack(parent, leftNode);
        *result = leftNode;
    }

    SWAG_VERIFY(tokenParse.id != TokenId::SymEqualEqual, error(tokenParse, Err(Err0677)));

    if (tokenParse.id != TokenId::SymLeftCurly && tokenParse.id != TokenId::KwdDo)
        SWAG_CHECK(eatSemiCol("left expression"));
    return true;
}

bool Parser::doInit(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstInit>(AstNodeKind::Init, this, parent);
    node->semanticFct = Semantic::resolveInit;
    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, Err(Err0528)));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->expression));

    if (tokenParse.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->count));
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));

    if (tokenParse.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFuncCallParameters(node, &node->parameters, TokenId::SymRightParen));
    }

    return true;
}

bool Parser::doDropCopyMove(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstDropCopyMove>(AstNodeKind::Drop, this, parent);
    switch (tokenParse.id)
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

    node->semanticFct = Semantic::resolveDropCopyMove;
    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, Err(Err0528)));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->expression));

    if (tokenParse.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->count));
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doRange(AstNode* parent, AstNode* expression, AstNode** result)
{
    const auto rangeNode   = Ast::newNode<AstRange>(AstNodeKind::Range, this, parent);
    *result                = rangeNode;
    rangeNode->semanticFct = Semantic::resolveRange;
    Ast::removeFromParent(expression);
    Ast::addChildBack(rangeNode, expression);
    rangeNode->expressionLow = expression;

    if (tokenParse.id == TokenId::KwdUntil)
        rangeNode->addSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP);
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(rangeNode, EXPR_FLAG_SIMPLE, &rangeNode->expressionUp));
    return true;
}

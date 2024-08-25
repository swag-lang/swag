#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool Parser::doLiteral(AstNode* parent, AstNode** result)
{
    const auto node    = Ast::newNode<AstLiteral>(AstNodeKind::Literal, this, parent);
    *result            = node;
    node->semanticFct  = Semantic::resolveLiteral;
    node->literalType  = tokenParse.literalType;
    node->literalValue = tokenParse.literalValue;

    SWAG_CHECK(eatToken());

    // Type suffix
    if (tokenParse.is(TokenId::SymQuote))
    {
        if (node->token.is(TokenId::LiteralCharacter) || node->token.is(TokenId::LiteralNumber))
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.is(TokenId::Identifier) || tokenParse.is(TokenId::NativeType), error(tokenParse, toErr(Err0241)));
            const auto identifierRef = Ast::newIdentifierRef(this, node);
            SWAG_CHECK(doIdentifier(identifierRef, IDENTIFIER_NO_PARAMS | IDENTIFIER_TYPE_DECL));
            identifierRef->lastChild()->semanticFct = Semantic::resolveLiteralSuffix;
        }
        else
        {
            switch (node->token.id)
            {
                case TokenId::KwdTrue:
                case TokenId::KwdFalse:
                    return error(tokenParse, formErr(Err0700, "[[bool]] literals"));
                case TokenId::LiteralString:
                    return error(tokenParse, formErr(Err0700, "[[string]] literals"));
                case TokenId::KwdNull:
                    return error(tokenParse, formErr(Err0700, "[[null]]"));
                default:
                    return error(tokenParse, formErr(Err0700, "that kind of literal"));
            }
        }
    }

    return true;
}

bool Parser::doArrayPointerIndex(AstNode** exprNode)
{
    const auto startToken = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftSquare, "to specify the index"));

    AstNode* firstExpr = nullptr;

    if (tokenParse.is(TokenId::SymDotDot) || tokenParse.is(TokenId::SymDotDotLess))
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
    if (tokenParse.is(TokenId::SymDotDot) || tokenParse.is(TokenId::SymDotDotLess))
    {
        const auto arrayNode   = Ast::newNode<AstArrayPointerSlicing>(AstNodeKind::ArrayPointerSlicing, this, nullptr);
        arrayNode->semanticFct = Semantic::resolveArrayPointerSlicing;
        arrayNode->array       = *exprNode;
        Ast::addChildBack(arrayNode, *exprNode);
        Ast::addChildBack(arrayNode, firstExpr);
        arrayNode->lowerBound = firstExpr;
        if (tokenParse.is(TokenId::SymDotDotLess))
            arrayNode->addSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP);
        SWAG_CHECK(eatToken());

        if (tokenParse.isNot(TokenId::SymRightSquare))
        {
            SWAG_CHECK(doExpression(arrayNode, EXPR_FLAG_NONE, &arrayNode->upperBound));
        }

        // To the end...
        else
        {
            if (arrayNode->hasSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP))
            {
                const Diagnostic err{sourceFile, tokenParse, toErr(Err0483)};
                return context->report(err);
            }

            arrayNode->upperBound = Ast::newNode<AstNode>(AstNodeKind::AutoSlicingUp, this, arrayNode);
            arrayNode->array->allocateExtension(ExtensionKind::Semantic);
            SWAG_ASSERT(!arrayNode->array->extSemantic()->semanticAfterFct);
            arrayNode->array->extSemantic()->semanticAfterFct = Semantic::resolveArrayPointerSlicingUpperBound;
            arrayNode->upperBound->addAstFlag(AST_GENERATED);
        }

        *exprNode = arrayNode;
        FormatAst::inheritFormatAfter(this, arrayNode, &tokenParse);
        SWAG_CHECK(eatCloseToken(TokenId::SymRightSquare, startToken));
    }

    // Deref by index
    else
    {
        while (true)
        {
            const auto arrayNode   = Ast::newNode<AstArrayPointerIndex>(AstNodeKind::ArrayPointerIndex, this, nullptr);
            arrayNode->token       = firstExpr ? firstExpr->token : tokenParse.token;
            arrayNode->semanticFct = Semantic::resolveArrayPointerIndex;

            Ast::addChildBack(arrayNode, *exprNode);
            arrayNode->array = *exprNode;
            if (arrayNode->array->is(AstNodeKind::ArrayPointerIndex))
                arrayNode->array->addSpecFlag(AstArrayPointerIndex::SPEC_FLAG_MULTI_ACCESS);

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
            if (tokenParse.is(TokenId::SymRightSquare))
                break;
            SWAG_CHECK(eatTokenError(TokenId::SymComma, toErr(Err0106)));
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightSquare), error(tokenParse, toErr(Err0107)));
        }

        FormatAst::inheritFormatAfter(this, (*exprNode)->lastChild(), &tokenParse);
        SWAG_CHECK(eatCloseToken(TokenId::SymRightSquare, startToken));
    }

    return true;
}

bool Parser::doIntrinsicProp(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstIntrinsicProp>(AstNodeKind::IntrinsicProp, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveIntrinsicProperty;
    node->inheritTokenName(tokenParse.token);

    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, toErr(Err0431)));
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(tokenParse, formErr(Err0467, node->token.cstr())));

    // Three parameters
    if (node->token.is(TokenId::IntrinsicMakeInterface))
    {
        AstNode* params = Ast::newFuncCallParams(this, node);
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the second argument"));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "tp specify the third argument"));
        SWAG_CHECK(doExpression(params, EXPR_FLAG_NONE, &dummyResult));
    }

    // Two parameters
    else if (node->token.is(TokenId::IntrinsicMakeSlice) ||
             node->token.is(TokenId::IntrinsicMakeString) ||
             node->token.is(TokenId::IntrinsicMakeAny))
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the second argument"));
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    // Two parameters
    else if (node->token.is(TokenId::IntrinsicCVaArg))
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
        SWAG_CHECK(eatToken(TokenId::SymComma, "to specify the second argument"));
        SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    // One single parameter
    else if (node->token.is(TokenId::IntrinsicTypeOf) ||
             node->token.is(TokenId::IntrinsicKindOf) ||
             node->token.is(TokenId::IntrinsicSizeOf) ||
             node->token.is(TokenId::IntrinsicDeclType))
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_TYPEOF, &dummyResult));
    }
    else
    {
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    }

    FormatAst::inheritFormatAfter(this, node, &tokenParse);
    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doParenthesisExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken());
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(startLoc, tokenParse.token.endLocation, toErr(Err0052)));

    SWAG_CHECK(doExpression(parent, exprFlags, result));
    (*result)->addAstFlag(AST_EXPR_IN_PARENTS);
    FormatAst::inheritFormatAfter(this, *result, &tokenParse);

    Utf8 msg;
    if (parent && parent->isNot(AstNodeKind::Statement))
        msg = form("to end the expression after [[%s]]", parent->token.cstr());
    else
        msg = "to end the left expression";

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, msg, parent));
    return true;
}

bool Parser::doSinglePrimaryExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    switch (tokenParse.token.id)
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
            SWAG_CHECK(doParenthesisExpression(parent, exprFlags, result));
            break;

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
            if (tokenParse.isNot(TokenId::Identifier))
                return error(tokenParse, formErr(Err0193, ".").cstr());

            AstNode* idref;
            SWAG_CHECK(doIdentifierRef(parent, &idref));
            *result = idref;
            SWAG_ASSERT(idref->is(AstNodeKind::IdentifierRef));
            idref->addSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE);
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
                return invalidTokenError(InvalidTokenError::PrimaryExpression);
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
                return invalidTokenError(InvalidTokenError::PrimaryExpression);
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
                if (tokenParse.isNot(TokenId::SymRightParen) && tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::SymComma))
                {
                    tokenizer.restoreState(tokenParse);
                    Ast::removeFromParent(parent->lastChild());
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
                return invalidTokenError(InvalidTokenError::PrimaryExpression);

            if (exprFlags.has(EXPR_FLAG_TYPEOF))
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
    if (tokenParse.is(TokenId::SymAmpersand))
    {
        exprNode              = Ast::newNode<AstMakePointer>(AstNodeKind::MakePointer, this, nullptr);
        exprNode->semanticFct = Semantic::resolveMakePointer;
        SWAG_CHECK(eatToken());

        bool hasDot = false;
        if (tokenParse.is(TokenId::SymDot))
        {
            SWAG_CHECK(eatToken());
            hasDot = true;
        }

        AstNode* identifierRef;
        SWAG_CHECK(doIdentifierRef(nullptr, &identifierRef));
        setForceTakeAddress(identifierRef);

        if (hasDot)
            identifierRef->addSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE);

        if (tokenParse.is(TokenId::SymLeftSquare))
            SWAG_CHECK(doArrayPointerIndex(&identifierRef));

        Ast::addChildBack(exprNode, identifierRef);
        identifierRef->addAstFlag(AST_TAKE_ADDRESS);
        if (parent)
            Ast::addChildBack(parent, exprNode);
    }

    // Dereference pointer
    else if (tokenParse.is(TokenId::KwdDeRef))
    {
        SWAG_CHECK(doDeRef(parent, &exprNode));
    }

    // moveref modifier
    else if (tokenParse.is(TokenId::KwdMoveRef))
    {
        SWAG_VERIFY(exprFlags.has(EXPR_FLAG_IN_CALL), error(tokenParse, toErr(Err0343)));
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
    switch (tokenParse.token.id)
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
            node->token       = tokenParse.token;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.token.isNot(prevTokenParse.token.id), error(tokenParse, formErr(Err0043, tokenParse.cstr())));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdDeRef), error(tokenParse, formErr(Err0176, prevTokenParse.cstr(), prevTokenParse.cstr())));
            return doSinglePrimaryExpression(node, exprFlags, &dummyResult);
        }
    }

    return doPrimaryExpression(parent, exprFlags, result);
}

bool Parser::doModifiers(const Token& forNode, TokenId tokenId, ModifierFlags& mdfFlags, AstNode* /*node*/)
{
    const auto opId = tokenId;

    mdfFlags = 0;
    while (Tokenizer::isModifier(tokenParse.token.id))
    {
        if (tokenParse.is(TokenId::ModifierPromote))
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
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_PROM), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            mdfFlags.add(MODIFIER_PROM);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierOver))
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
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_OVERFLOW), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            mdfFlags.add(MODIFIER_OVERFLOW);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierNoDrop))
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_NO_LEFT_DROP), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            mdfFlags.add(MODIFIER_NO_LEFT_DROP);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierRef))
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_REF), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            SWAG_VERIFY(!mdfFlags.has(MODIFIER_CONST_REF), error(tokenParse, formErr(Err0036, "#ref", "#constref")));
            mdfFlags.add(MODIFIER_REF);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierConstRef))
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_CONST_REF), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            SWAG_VERIFY(!mdfFlags.has(MODIFIER_REF), error(tokenParse, formErr(Err0036, "#constref", "#ref")));
            mdfFlags.add(MODIFIER_CONST_REF);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierBack))
        {
            switch (opId)
            {
                case TokenId::KwdLoop:
                case TokenId::KwdVisit:
                    break;
                default:
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_BACK), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            mdfFlags.add(MODIFIER_BACK);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierUnconst))
        {
            switch (opId)
            {
                case TokenId::KwdCast:
                    break;
                default:
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_UN_CONST), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            mdfFlags.add(MODIFIER_UN_CONST);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierMove))
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_MOVE), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            SWAG_VERIFY(!mdfFlags.has(MODIFIER_MOVE_RAW), error(tokenParse, formErr(Err0036, "#move", "#moveraw")));
            mdfFlags.add(MODIFIER_MOVE);
            SWAG_CHECK(eatToken());
            continue;
        }

        if (tokenParse.is(TokenId::ModifierMoveRaw))
        {
            switch (opId)
            {
                case TokenId::SymEqual:
                    break;
                default:
                    return error(tokenParse, formErr(Err0681, tokenParse.cstr(), forNode.cstr()));
            }

            SWAG_VERIFY(!mdfFlags.has(MODIFIER_MOVE_RAW), error(tokenParse, formErr(Err0041, tokenParse.cstr())));
            SWAG_VERIFY(!mdfFlags.has(MODIFIER_MOVE), error(tokenParse, formErr(Err0036, "#moveraw", "#move")));
            mdfFlags.add(MODIFIER_MOVE_RAW);
            SWAG_CHECK(eatToken());
            continue;
        }

        SWAG_ASSERT(false);
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
        if (factor->isNot(AstNodeKind::FactorOp) && factor->isNot(AstNodeKind::BinaryOp))
            return true;

        auto left = factor->firstChild();
        SWAG_CHECK(doOperatorPrecedence(&left));
        auto right = factor->secondChild();
        SWAG_CHECK(doOperatorPrecedence(&right));

        if ((right->is(AstNodeKind::FactorOp) || right->is(AstNodeKind::BinaryOp)) && !right->hasAstFlag(AST_EXPR_IN_PARENTS))
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
                const auto atom = factor->hasAstFlag(AST_EXPR_IN_PARENTS);
                factor->removeAstFlag(AST_EXPR_IN_PARENTS);

                const auto leftRight = right->firstChild();
                Ast::removeFromParent(right);
                if (factor->parent && factor == factor->parent->firstChild())
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
                    factor->addAstFlag(AST_EXPR_IN_PARENTS);
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
    if (tokenParse.is(TokenId::SymPlus) ||
        tokenParse.is(TokenId::SymMinus) ||
        tokenParse.is(TokenId::SymAsterisk) ||
        tokenParse.is(TokenId::SymSlash) ||
        tokenParse.is(TokenId::SymPercent) ||
        tokenParse.is(TokenId::SymAmpersand) ||
        tokenParse.is(TokenId::SymVertical) ||
        tokenParse.is(TokenId::SymGreaterGreater) ||
        tokenParse.is(TokenId::SymLowerLower) ||
        tokenParse.is(TokenId::SymPlusPlus) ||
        tokenParse.is(TokenId::SymCircumflex))
    {
        auto binaryNode = Ast::newNode<AstOp>(AstNodeKind::FactorOp, this, *parent);

        if (tokenParse.is(TokenId::SymGreaterGreater) || tokenParse.is(TokenId::SymLowerLower))
            binaryNode->semanticFct = Semantic::resolveShiftExpression;
        else
            binaryNode->semanticFct = Semantic::resolveFactorExpression;
        binaryNode->token = tokenParse.token;
        SWAG_CHECK(eatToken());

        // Modifiers
        ModifierFlags mdfFlags = 0;
        SWAG_CHECK(doModifiers(binaryNode->token, binaryNode->token.id, mdfFlags));
        if (mdfFlags.has(MODIFIER_OVERFLOW))
        {
            binaryNode->addSpecFlag(AstOp::SPEC_FLAG_OVERFLOW);
            binaryNode->addAttribute(ATTRIBUTE_CAN_OVERFLOW_ON);
        }

        if (mdfFlags.has(MODIFIER_PROM))
        {
            binaryNode->addSpecFlag(AstOp::SPEC_FLAG_PROM);
        }

        Ast::addChildBack(binaryNode, leftNode);

        auto     savedToken = tokenParse;
        AstNode* expr;
        SWAG_CHECK(doFactorExpression(reinterpret_cast<AstNode**>(&binaryNode), exprFlags, &expr));
        FormatAst::inheritFormatBefore(this, expr, &savedToken);

        SWAG_CHECK(doOperatorPrecedence(reinterpret_cast<AstNode**>(&binaryNode)));
        leftNode = binaryNode;
        isBinary = true;
    }
    else if (tokenParse.is(TokenId::SymEqualEqual) ||
             tokenParse.is(TokenId::SymExclamEqual) ||
             tokenParse.is(TokenId::SymLowerEqual) ||
             tokenParse.is(TokenId::SymGreaterEqual) ||
             tokenParse.is(TokenId::SymLower) ||
             tokenParse.is(TokenId::SymGreater) ||
             tokenParse.is(TokenId::SymLowerEqualGreater))
    {
        auto binaryNode         = castAst<AstNode>(Ast::newNode<AstBinaryOpNode>(AstNodeKind::BinaryOp, this, parent ? *parent : nullptr));
        binaryNode->semanticFct = Semantic::resolveCompareExpression;
        binaryNode->token       = tokenParse.token;

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

    if (exprFlags.has(EXPR_FLAG_STOP_AFFECT) && tokenParse.is(TokenId::SymEqual))
    {
        Ast::addChildBack(parent, leftNode);
        *result = leftNode;
        return true;
    }

    if (!exprFlags.has(EXPR_FLAG_NAMED_PARAM) || leftNode->isNot(AstNodeKind::IdentifierRef))
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymEqual), error(tokenParse, toErr(Err0652)));

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
    if (tokenParse.is(TokenId::KwdOr) || tokenParse.is(TokenId::KwdAnd))
    {
        const auto binaryNode   = Ast::newNode<AstBinaryOpNode>(AstNodeKind::BinaryOp, this, parent);
        binaryNode->semanticFct = Semantic::resolveBoolExpression;
        binaryNode->token       = tokenParse.token;

        Ast::addChildBack(binaryNode, leftNode);
        SWAG_CHECK(eatToken());

        auto     savedToken = tokenParse;
        AstNode* expr;
        SWAG_CHECK(doBoolExpression(binaryNode, EXPR_FLAG_NONE, &expr));
        FormatAst::inheritFormatBefore(this, expr, &savedToken);

        leftNode = binaryNode;
        isBinary = true;
    }
    else if (tokenParse.is(TokenId::SymAmpersandAmpersand) || tokenParse.is(TokenId::SymVerticalVertical))
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
    if (mdfFlags.has(MODIFIER_MOVE | MODIFIER_MOVE_RAW))
    {
        auto exprNode         = Ast::newNode<AstNode>(AstNodeKind::Move, this, parent);
        *result               = exprNode;
        exprNode->semanticFct = Semantic::resolveMove;
        exprNode->addAstFlag(AST_FORCE_MOVE);
        parent = exprNode;
        result = &dummyResult;

        // no drop right
        if (mdfFlags.has(MODIFIER_MOVE_RAW))
        {
            exprNode              = Ast::newNode<AstNode>(AstNodeKind::NoDrop, this, parent);
            *result               = exprNode;
            exprNode->semanticFct = Semantic::resolveMove;
            exprNode->addAstFlag(AST_NO_RIGHT_DROP);
            parent = exprNode;
            result = &dummyResult;
        }
    }

    // ref
    if (mdfFlags.has(MODIFIER_REF))
    {
        const auto exprNode   = Ast::newNode<AstNode>(AstNodeKind::KeepRef, this, parent);
        *result               = exprNode;
        exprNode->semanticFct = Semantic::resolveKeepRef;
        parent                = exprNode;
        result                = &dummyResult;
    }

    // constref
    if (mdfFlags.has(MODIFIER_CONST_REF))
    {
        const auto exprNode   = Ast::newNode<AstNode>(AstNodeKind::KeepRef, this, parent);
        *result               = exprNode;
        exprNode->semanticFct = Semantic::resolveKeepRef;
        exprNode->addAstFlag(AST_CONST);
        parent = exprNode;
        result = &dummyResult;
    }

    SWAG_CHECK(doExpression(parent, exprFlags, result));
    return true;
}

bool Parser::doExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    AstNode* boolExpression = nullptr;
    switch (tokenParse.token.id)
    {
        case TokenId::CompilerRun:
        {
            ParserPushAstNodeFlags sf(this, AST_IN_RUN_BLOCK);

            const auto node   = Ast::newNode<AstCompilerSpecFunc>(AstNodeKind::CompilerRunExpression, this, nullptr);
            node->semanticFct = Semantic::resolveCompilerRun;
            SWAG_CHECK(eatToken());

            // :RunGeneratedExp
            if (tokenParse.is(TokenId::SymLeftCurly))
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
                const auto identifier           = castAst<AstIdentifier>(idRef->lastChild(), AstNodeKind::Identifier);
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
            const auto node = Ast::newNode<AstCompilerCode>(AstNodeKind::CompilerCode, this, nullptr);
            SWAG_CHECK(eatToken());
            if (tokenParse.is(TokenId::SymLeftCurly))
                SWAG_CHECK(doScopedCurlyStatement(node, &dummyResult));
            else
                SWAG_CHECK(doBoolExpression(node, exprFlags, &dummyResult));
            const auto typeCode = makeType<TypeInfoCode>();
            typeCode->content   = node->firstChild();
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
            return error(tokenParse, formErr(Err0175, tokenParse.cstr()));

        default:
            SWAG_CHECK(doBoolExpression(parent, exprFlags, &boolExpression));
            Ast::removeFromParent(boolExpression);
            break;
    }

    // A ? B : C
    if (tokenParse.is(TokenId::SymQuestion))
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
    else if (tokenParse.is(TokenId::KwdOrElse))
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
    const auto node   = Ast::newNode<AstExpressionList>(AstNodeKind::ExpressionList, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveExpressionListTuple;
    node->addSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE);
    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken());

    while (tokenParse.isNot(TokenId::SymRightCurly))
    {
        if (tokenParse.is(TokenId::SymLeftCurly))
            SWAG_CHECK(doExpressionListTuple(node, &dummyResult));
        else if (tokenParse.is(TokenId::SymLeftSquare))
            SWAG_CHECK(doExpressionListArray(node, &dummyResult));
        else
        {
            const auto tokenExpr = tokenParse;

            AstNode* paramExpression;
            SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NAMED_PARAM, &paramExpression));

            // Name
            if (tokenParse.is(TokenId::SymColon))
            {
                SWAG_CHECK(checkIsSingleIdentifier(paramExpression, "as a tuple field name"));
                SWAG_CHECK(checkIsValidVarName(paramExpression->lastChild()));
                auto       namedToFree     = paramExpression;
                const auto namedExpression = namedToFree->lastChild();
                SWAG_CHECK(eatToken());

                if (tokenParse.is(TokenId::SymLeftCurly))
                    SWAG_CHECK(doExpressionListTuple(node, &paramExpression));
                else
                    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &paramExpression));

                paramExpression->addExtraPointer(ExtraPointerKind::IsNamed, namedExpression);
                paramExpression->token.startLocation = namedExpression->token.startLocation;
                paramExpression->allocateExtension(ExtensionKind::Owner);
                paramExpression->extOwner()->nodesToFree.push_back(namedToFree);
            }
            else
            {
                Ast::addChildBack(node, paramExpression);
            }
        }

        if (tokenParse.isNot(TokenId::SymComma))
            break;
        SWAG_CHECK(eatToken());
    }

    node->token.endLocation = tokenParse.token.endLocation;
    SWAG_CHECK(eatFormat(node));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the tuple body"));
    return true;
}

bool Parser::doExpressionListArray(AstNode* parent, AstNode** result)
{
    const auto node     = Ast::newNode<AstExpressionList>(AstNodeKind::ExpressionList, this, parent);
    *result             = node;
    node->semanticFct   = Semantic::resolveExpressionListArray;
    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken());

    if (tokenParse.is(TokenId::SymRightSquare))
        return error(tokenParse, toErr(Err0049));

    TokenParse lastToken;
    while (tokenParse.isNot(TokenId::SymRightSquare))
    {
        AstNode* subExpr = nullptr;
        if (tokenParse.is(TokenId::SymLeftSquare))
            SWAG_CHECK(doExpressionListArray(node, &subExpr));
        else if (tokenParse.is(TokenId::SymLeftCurly))
            SWAG_CHECK(doExpressionListTuple(node, &subExpr));
        else
            SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &subExpr));

        if (tokenParse.is(TokenId::SymRightSquare))
            break;
        FormatAst::inheritFormatBefore(this, subExpr, &lastToken);
        FormatAst::inheritFormatAfter(this, subExpr, &tokenParse);
        SWAG_CHECK(eatTokenError(TokenId::SymComma, toErr(Err0108)));
        lastToken = tokenParse;
    }

    node->token.endLocation = tokenParse.token.endLocation;
    SWAG_CHECK(eatFormat(node));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightSquare, startLoc));
    return true;
}

bool Parser::doInitializationExpression(const TokenParse& forToken, AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    // var x: type = undefined => not initialized
    if (tokenParse.is(TokenId::KwdUndefined))
    {
        const auto node   = Ast::newNode<AstNode>(AstNodeKind::ExplicitNoInit, this, parent);
        *result           = node;
        node->semanticFct = Semantic::resolveExplicitNoInit;
        if (parent)
            parent->addAstFlag(AST_EXPLICITLY_NOT_INITIALIZED);
        SWAG_CHECK(eatToken());
        return true;
    }

    return doMoveExpression(forToken.token, forToken.token.id, parent, exprFlags, result);
}

void Parser::setForceTakeAddress(AstNode* node)
{
    node->addAstFlag(AST_TAKE_ADDRESS);
    switch (node->kind)
    {
        case AstNodeKind::IdentifierRef:
            setForceTakeAddress(node->lastChild());
            break;
        case AstNodeKind::ArrayPointerIndex:
            setForceTakeAddress(castAst<AstArrayPointerIndex>(node)->array);
            break;
    }
}

bool Parser::isGeneratedName(const Utf8& name)
{
    return name.length() >= 2 && name[0] == '_' && name[1] == '_';
}

bool Parser::doLeftExpressionVar(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags, const AstWith* withNode)
{
    switch (tokenParse.token.id)
    {
        case TokenId::SymLeftParen:
        {
            const auto multi    = Ast::newNode<AstNode>(AstNodeKind::MultiIdentifierTuple, this, nullptr);
            *result             = multi;
            const auto startLoc = tokenParse.token.startLocation;
            SWAG_CHECK(eatToken());
            while (true)
            {
                SWAG_VERIFY(tokenParse.is(TokenId::Identifier) || tokenParse.is(TokenId::SymQuestion), error(tokenParse, toErr(Err0695)));
                SWAG_CHECK(doIdentifierRef(multi, &dummyResult, identifierFlags | IDENTIFIER_ACCEPT_QUESTION));
                if (tokenParse.is(TokenId::SymRightParen))
                    break;
                SWAG_CHECK(eatTokenError(TokenId::SymComma, toErr(Err0112)));
                SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(tokenParse, toErr(Err0113)));
            }

            SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, "to end the tuple unpacking", parent));
            break;
        }

        case TokenId::Identifier:
        case TokenId::CompilerUp:
        case TokenId::CompilerSelf:
        {
            AstNode* exprNode    = nullptr;
            AstNode* multi       = nullptr;
            bool     prependWith = withNode != nullptr;
            while (true)
            {
                if (tokenParse.is(TokenId::SymDot))
                {
                    SWAG_VERIFY(withNode, error(tokenParse, toErr(Err0407)));
                    prependWith = true;
                    eatToken();
                }

                SWAG_CHECK(doIdentifierRef(multi == nullptr ? parent : multi, &exprNode, identifierFlags));
                if (multi == nullptr)
                    Ast::removeFromParent(exprNode);

                // Prepend the 'with' identifier
                if (withNode && prependWith)
                {
                    prependWith = false;
                    SWAG_ASSERT(exprNode->is(AstNodeKind::IdentifierRef));
                    for (uint32_t wi = withNode->id.size() - 1; wi != UINT32_MAX; wi--)
                    {
                        const auto id = Ast::newIdentifier(castAst<AstIdentifierRef>(exprNode), withNode->id[wi], this, exprNode);
                        id->addAstFlag(AST_GENERATED);
                        id->addSpecFlag(AstIdentifier::SPEC_FLAG_FROM_WITH);
                        id->allocateIdentifierExtension();
                        id->identifierExtension->fromAlternateVar = withNode->firstChild();
                        id->inheritTokenLocation(exprNode->token);
                        exprNode->children.pop_back();
                        Ast::addChildFront(exprNode, id);
                    }
                }

                if (tokenParse.isNot(TokenId::SymComma))
                    break;

                SWAG_CHECK(eatToken());
                SWAG_VERIFY(tokenParse.isNot(TokenId::SymEqual) && tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse, toErr(Err0114)));
                SWAG_VERIFY(tokenParse.is(TokenId::Identifier) || tokenParse.is(TokenId::SymDot), invalidIdentifierError(tokenParse, toErr(Err0269)));

                if (!multi)
                {
                    multi = Ast::newNode<AstNode>(AstNodeKind::MultiIdentifier, this, parent);
                    Ast::addChildBack(multi, exprNode);
                    FormatAst::inheritFormatBefore(this, multi, exprNode);
                }
            }

            Ast::removeFromParent(multi);
            *result = multi ? multi : exprNode;
            break;
        }

        default:
            return invalidIdentifierError(tokenParse, toErr(Err0268));
    }

    return true;
}

bool Parser::doLeftExpressionAffect(AstNode* parent, AstNode** result, const AstWith* withNode)
{
    switch (tokenParse.token.id)
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

bool Parser::doMultiIdentifierAffect(AstNode* parent, AstNode** result, AstNode* leftNode, SpecFlags opFlags, const AttributeFlags& opAttrFlags, TokenParse& savedToken)
{
    savedToken.token.startLocation = tokenParse.token.startLocation;
    const auto parentNode          = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
    *result                        = parentNode;
    parentNode->addSpecFlag(AstStatement::SPEC_FLAG_MULTI_AFFECT);
    FormatAst::inheritFormatBefore(this, parentNode, leftNode);

    // Generate an expression of the form "var firstVar = assignment", and "secondVar = firstVar" for the rest
    // This will avoid to do the right expression multiple times (if this is a function call for example).
    //
    // If this is not the '=' operator, then we have to duplicate the affectation for each variable
    // If the affect expression is a literal, it's better to duplicate also
    AstNode*   affectExpression = nullptr;
    bool       firstDone        = false;
    const auto front            = castAst<AstIdentifierRef>(leftNode->firstChild(), AstNodeKind::IdentifierRef);
    front->computeName();

    const auto cloneFront = Ast::clone(front, nullptr);

    while (!leftNode->children.empty())
    {
        const auto child      = leftNode->firstChild();
        const auto affectNode = Ast::newAffectOp(opFlags, opAttrFlags, nullptr, parentNode);
        affectNode->token     = savedToken.token;
        Ast::removeFromParent(child);
        Ast::addChildBack(affectNode, child);
        setForceTakeAddress(child);

        // First create 'firstVar = assignment'
        if (!firstDone)
        {
            firstDone = true;
            if (affectNode->token.is(TokenId::SymEqual))
                SWAG_CHECK(doMoveExpression(affectNode->token, affectNode->token.id, affectNode, EXPR_FLAG_NONE, &affectExpression));
            else
                SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE, &affectExpression));
        }

        // This is not an initialization, so we need to duplicate the right expression
        else if (affectNode->token.isNot(TokenId::SymEqual) || affectExpression->is(AstNodeKind::Literal))
        {
            const auto newAffect = Ast::clone(affectExpression, affectNode);
            newAffect->inheritTokenLocation(affectExpression->token);
        }

        // In case of an affectation, create 'otherVar = firstVar'
        else
        {
            (void) Ast::clone(cloneFront, affectNode);
        }
    }

    cloneFront->release();
    leftNode->release();
    return true;
}

bool Parser::doTupleUnpacking(AstNode* parent, AstNode** result, AstNode* leftNode, SpecFlags opFlags, const AttributeFlags& opAttrFlags, TokenParse& savedToken)
{
    savedToken.token.startLocation = tokenParse.token.startLocation;
    const auto parentNode          = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
    *result                        = parentNode;
    parentNode->addSpecFlag(AstStatement::SPEC_FLAG_TUPLE_UNPACKING);
    FormatAst::inheritFormatBefore(this, parentNode, leftNode);

    // Get right side
    AstNode* assignment;
    SWAG_CHECK(doExpression(nullptr, EXPR_FLAG_NONE, &assignment));

    tokenParse.token.startLocation = savedToken.token.startLocation;
    tokenParse.token.endLocation   = savedToken.token.endLocation;

    // Generate an expression of the form "var __tmp_0 = assignment"
    const auto  tmpVarName = form("__4tmp_%d", g_UniqueID.fetch_add(1));
    AstVarDecl* varNode    = Ast::newVarDecl(tmpVarName, this, parentNode);
    varNode->addAstFlag(AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS);
    Ast::addChildBack(varNode, assignment);
    assignment->inheritOwners(varNode);
    varNode->assignment = assignment;
    varNode->assignment->addAstFlag(AST_NO_LEFT_DROP);

    varNode->token.startLocation = leftNode->firstChild()->token.startLocation;
    varNode->token.endLocation   = leftNode->lastChild()->token.endLocation;
    varNode->allocateExtension(ExtensionKind::Semantic);
    varNode->extSemantic()->semanticAfterFct = Semantic::resolveTupleUnpackBefore;

    // And reference that variable, in the form value = __tmp_0.item?
    int idx = 0;
    while (!leftNode->children.empty())
    {
        const auto child = leftNode->firstChild();

        // Ignore field if '?', otherwise check that this is a valid variable name
        if (child->firstChild()->token.is("?"))
        {
            idx++;
            Ast::removeFromParent(child);
            Ast::addChildBack(parentNode, child);
            child->addAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE);
            continue;
        }

        const auto affectNode  = Ast::newAffectOp(opFlags, opAttrFlags, nullptr, parentNode);
        affectNode->token.id   = savedToken.token.id;
        affectNode->token.text = savedToken.token.text;
        Ast::removeFromParent(child);
        Ast::addChildBack(affectNode, child);
        setForceTakeAddress(child);

        // Force a move between the generated temporary variable and the real var
        const auto idRef = Ast::newMultiIdentifierRef(form("%s.item%u", tmpVarName.cstr(), idx++), this, affectNode);
        idRef->addAstFlag(AST_FORCE_MOVE);
    }

    leftNode->release();
    return true;
}

bool Parser::doSingleIdentifierAffect(AstNode* parent, AstNode** result, AstNode* leftNode, SpecFlags opFlags, const AttributeFlags& opAttrFlags, const TokenParse& savedToken)
{
    const auto affectNode = Ast::newAffectOp(opFlags, opAttrFlags, this, parent);
    affectNode->token     = savedToken.token;

    Ast::addChildBack(affectNode, leftNode);
    setForceTakeAddress(leftNode);

    if (affectNode->token.is(TokenId::SymEqual))
        SWAG_CHECK(doMoveExpression(affectNode->token, affectNode->token.id, affectNode, EXPR_FLAG_NONE, &dummyResult));
    else
        SWAG_CHECK(doExpression(affectNode, EXPR_FLAG_NONE, &dummyResult));

    // :DeduceLambdaType
    const auto back = affectNode->lastChild();
    if (back->is(AstNodeKind::MakePointerLambda) && back->hasSpecFlag(AstMakePointer::SPEC_FLAG_DEP_TYPE))
    {
        const auto front = affectNode->firstChild();
        front->allocateExtension(ExtensionKind::Semantic);
        front->extSemantic()->semanticAfterFct = Semantic::resolveAfterKnownType;
    }

    *result = affectNode;
    return true;
}

bool Parser::doAffectExpression(AstNode* parent, AstNode** result, const AstWith* withNode)
{
    bool     leftAlone = false;
    AstNode* leftNode;
    SWAG_CHECK(doLeftExpressionAffect(parent, &leftNode, withNode));
    Ast::removeFromParent(leftNode);

    // Affect operator
    if (tokenParse.is(TokenId::SymEqual) ||
        tokenParse.is(TokenId::SymPlusEqual) ||
        tokenParse.is(TokenId::SymMinusEqual) ||
        tokenParse.is(TokenId::SymAsteriskEqual) ||
        tokenParse.is(TokenId::SymSlashEqual) ||
        tokenParse.is(TokenId::SymAmpersandEqual) ||
        tokenParse.is(TokenId::SymVerticalEqual) ||
        tokenParse.is(TokenId::SymCircumflexEqual) ||
        tokenParse.is(TokenId::SymPercentEqual) ||
        tokenParse.is(TokenId::SymLowerLowerEqual) ||
        tokenParse.is(TokenId::SymGreaterGreaterEqual))
    {
        SpecFlags      opFlags     = 0;
        AttributeFlags opAttrFlags = 0;
        auto           savedToken  = tokenParse;
        SWAG_CHECK(eatToken());

        // Modifiers
        ModifierFlags mdfFlags = 0;
        if (savedToken.token.isNot(TokenId::SymEqual))
        {
            SWAG_CHECK(doModifiers(savedToken.token, savedToken.token.id, mdfFlags));
        }

        if (mdfFlags.has(MODIFIER_OVERFLOW))
        {
            opFlags.add(AstOp::SPEC_FLAG_OVERFLOW);
            opAttrFlags.add(ATTRIBUTE_CAN_OVERFLOW_ON);
        }

        // Multiple affectation
        // like in a, b, c = 0
        if (leftNode->is(AstNodeKind::MultiIdentifier))
        {
            SWAG_CHECK(doMultiIdentifierAffect(parent, result, leftNode, opFlags, opAttrFlags, savedToken));
        }

        // Tuple unpacking
        else if (leftNode->is(AstNodeKind::MultiIdentifierTuple))
        {
            SWAG_CHECK(doTupleUnpacking(parent, result, leftNode, opFlags, opAttrFlags, savedToken));
        }

        // One normal simple affectation
        else
        {
            SWAG_CHECK(doSingleIdentifierAffect(parent, result, leftNode, opFlags, opAttrFlags, savedToken));
            FormatAst::inheritFormatBefore(this, *result, leftNode);
            FormatAst::inheritFormatAfter(this, *result, &tokenParse);
        }
    }
    else
    {
        Ast::addChildBack(parent, leftNode);
        *result   = leftNode;
        leftAlone = true;
    }

    if (tokenParse.is(TokenId::SymLeftCurly) || tokenParse.is(TokenId::SymColon))
        return true;
    if (Tokenizer::isStartOfNewStatement(tokenParse))
        return eatSemiCol("left expression");

    return endOfLineError(leftNode, leftAlone);
}

bool Parser::doInit(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstInit>(AstNodeKind::Init, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveInit;
    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, toErr(Err0431)));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->expression));

    if (tokenParse.is(TokenId::SymComma))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->count));
    }

    FormatAst::inheritFormatAfter(this, node, &tokenParse);
    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));

    if (tokenParse.is(TokenId::SymLeftParen))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doFuncCallArguments(node, &node->parameters, TokenId::SymRightParen));
        FormatAst::inheritFormatAfter(this, node, node->parameters);
    }

    return true;
}

bool Parser::doDropCopyMove(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstDropCopyMove>(AstNodeKind::Drop, this, parent);
    *result         = node;
    switch (tokenParse.token.id)
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

    node->semanticFct = Semantic::resolveDropCopyMove;
    SWAG_CHECK(eatToken());

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, toErr(Err0431)));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->expression));

    if (tokenParse.is(TokenId::SymComma))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &node->count));
    }

    FormatAst::inheritFormatAfter(this, node, &tokenParse);
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

    if (tokenParse.is(TokenId::KwdUntil))
        rangeNode->addSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP);
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doExpression(rangeNode, EXPR_FLAG_SIMPLE, &rangeNode->expressionUp));
    return true;
}

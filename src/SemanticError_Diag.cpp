#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "TypeManager.h"

struct ErrorParam
{
    OneTryMatch*               oneTry;
    Vector<const Diagnostic*>* result0;
    Vector<const Diagnostic*>* result1;

    AstNode*          errorNode   = nullptr;
    AstFuncCallParam* failedParam = nullptr;
    int               badParamIdx = 0;
    uint32_t          getFlags;
    AstFuncDecl*      destFuncDecl   = nullptr;
    AstTypeLambda*    destLambdaDecl = nullptr;
    AstAttrDecl*      destAttrDecl   = nullptr;
    AstNode*          destParameters = nullptr;
    Utf8              explicitCastMsg;

    void addResult1(Diagnostic* note)
    {
        if (note)
            result1->push_back(note);
    }
};

static void errorValidIfFailed(SemanticContext* context, ErrorParam& errorParam)
{
    auto destFuncDecl = errorParam.destFuncDecl;
    auto msg          = Fmt(Err(Err0004), destFuncDecl->validif->token.ctext(), destFuncDecl->token.ctext(), destFuncDecl->validif->token.ctext());
    auto diag         = new Diagnostic{errorParam.errorNode, errorParam.errorNode->token, msg};
    errorParam.result0->push_back(diag);
    errorParam.addResult1(Diagnostic::hereIs(destFuncDecl->validif));
}

static void errorMissingNamedParameter(SemanticContext* context, ErrorParam& errorParam)
{
    SWAG_ASSERT(errorParam.failedParam);
    SWAG_ASSERT(errorParam.badParamIdx >= 2);
    auto msg  = Fmt(Err(Err0006), Naming::niceArgumentRank(errorParam.badParamIdx).c_str());
    auto diag = new Diagnostic{errorParam.failedParam, msg};
    diag->addRange(errorParam.oneTry->callParameters->childs[errorParam.badParamIdx - 2], Nte(Nte1030));
    errorParam.result0->push_back(diag);
}

static void errorInvalidNamedParameter(SemanticContext* context, ErrorParam& errorParam)
{
    SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtMisc() && errorParam.failedParam->extMisc()->isNamed);
    auto msg  = Fmt(Err(Err0008), errorParam.failedParam->extMisc()->isNamed->token.ctext());
    auto diag = new Diagnostic{errorParam.failedParam->extMisc()->isNamed, msg};
    errorParam.result0->push_back(diag);
    errorParam.addResult1(Diagnostic::hereIs(errorParam.oneTry->overload));
}

static void errorDuplicatedNamedParameter(SemanticContext* context, ErrorParam& errorParam)
{
    SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtMisc() && errorParam.failedParam->extMisc()->isNamed);
    auto   msg   = Fmt(Err(Err0011), errorParam.failedParam->extMisc()->isNamed->token.ctext());
    auto   diag  = new Diagnostic{errorParam.failedParam->extMisc()->isNamed, msg};
    size_t other = errorParam.oneTry->symMatchContext.badSignatureInfos.badSignatureNum1;
    SWAG_ASSERT(other < errorParam.oneTry->callParameters->childs.size());
    diag->addRange(errorParam.oneTry->callParameters->childs[other], Nte(Nte1059));
    errorParam.result0->push_back(diag);
}

static void errorMissingParameters(SemanticContext* context, ErrorParam& errorParam)
{
    auto node = errorParam.oneTry->callParameters;
    if (!node)
        node = errorParam.errorNode;

    auto overload = errorParam.oneTry->overload;
    Utf8 niceName = "the " + Naming::kindName(overload);
    auto diag     = new Diagnostic{node, Fmt(Err(Err0020), niceName.c_str())};
    errorParam.result0->push_back(diag);
    errorParam.addResult1(Diagnostic::hereIs(overload));
}

static void errorNotEnoughParameters(SemanticContext* context, ErrorParam& errorParam)
{
    auto        node           = errorParam.errorNode;
    auto        overload       = errorParam.oneTry->overload;
    auto        callParameters = errorParam.oneTry->callParameters;
    const auto& match          = errorParam.oneTry->symMatchContext;
    Utf8        niceName       = "the " + Naming::kindName(overload);

    Diagnostic* diag;
    if (!callParameters || callParameters->childs.empty())
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0020), niceName.c_str())};
    else if (errorParam.destAttrDecl)
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0157), niceName.c_str())};
    else
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0016), niceName.c_str())};

    errorParam.result0->push_back(diag);
    auto note = Diagnostic::hereIs(overload);
    errorParam.addResult1(note);

    for (uint32_t si = 0; si < match.solvedParameters.size(); si++)
    {
        if (si >= match.firstDefault)
            break;
        if (match.solvedParameters[si])
            continue;
        if (!errorParam.destParameters)
            continue;

        if (errorParam.destParameters->childs[si]->specFlags & AstVarDecl::SPECFLAG_UNNAMED)
            diag->remarks.push_back(Fmt(Nte(Nte0089), Naming::niceParameterRank(si + 1).c_str(), errorParam.destParameters->childs[si]->typeInfo->getDisplayNameC()));
        else
            diag->remarks.push_back(Fmt(Nte(Nte0088), errorParam.destParameters->childs[si]->token.ctext(), errorParam.destParameters->childs[si]->typeInfo->getDisplayNameC()));
        if (note && !errorParam.destParameters->childs[si]->isGeneratedSelf())
            note->addRange(errorParam.destParameters->childs[si], "missing");
    }
}

static void errorNotEnoughGenericParameters(SemanticContext* context, ErrorParam& errorParam)
{
    auto overload          = errorParam.oneTry->overload;
    Utf8 niceName          = "the " + Naming::kindName(overload);
    auto genericParameters = errorParam.oneTry->genericParameters;

    AstNode* errNode = genericParameters;
    if (!errNode)
        errNode = errorParam.errorNode;
    if (!errNode)
        errNode = context->node;

    Diagnostic* diag;
    if (errorParam.destFuncDecl && errorParam.destFuncDecl->isSpecialFunctionName())
        diag = new Diagnostic{errNode, errNode->token, Fmt(Err(Err0352), niceName.c_str(), errorParam.destFuncDecl->token.ctext())};
    else if (genericParameters)
        diag = new Diagnostic{genericParameters, Fmt(Err(Err0035), niceName.c_str())};
    else
        diag = new Diagnostic{errNode, errNode->token, Fmt(Err(Err0049), niceName.c_str())};
    errorParam.result0->push_back(diag);
    errorParam.addResult1(Diagnostic::hereIs(overload));
}

static void errorTooManyParameters(SemanticContext* context, ErrorParam& errorParam)
{
    auto        overload = errorParam.oneTry->overload;
    const auto& match    = errorParam.oneTry->symMatchContext;

    AstNode* site = errorParam.failedParam;
    if (!site)
        site = errorParam.oneTry->callParameters;
    SWAG_ASSERT(site);

    auto msg  = Fmt(Err(Err0026), match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);
    auto diag = new Diagnostic{site, msg};
    errorParam.result0->push_back(diag);
    errorParam.addResult1(Diagnostic::hereIs(overload));
}

static void errorTooManyGenericParameters(SemanticContext* context, ErrorParam& errorParam)
{
    auto        overload          = errorParam.oneTry->overload;
    auto        symbol            = errorParam.oneTry->overload->symbol;
    Utf8        niceName          = "the " + Naming::kindName(overload);
    auto        genericParameters = errorParam.oneTry->genericParameters;
    const auto& match             = errorParam.oneTry->symMatchContext;

    AstNode* errNode = genericParameters;
    if (!errNode)
        errNode = errorParam.errorNode;
    if (!errNode)
        errNode = context->node;

    Diagnostic* diag;
    if (!match.badSignatureInfos.badSignatureNum2)
    {
        auto msg = Fmt(Err(Err0135), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str());
        diag     = new Diagnostic{errNode, msg};
    }
    else
    {
        if (genericParameters)
            errNode = genericParameters->childs[match.badSignatureInfos.badSignatureNum2];
        auto msg = Fmt(Err(Err0044), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str(), match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);
        diag     = new Diagnostic{errNode, msg};
    }

    errorParam.result0->push_back(diag);
    errorParam.addResult1(Diagnostic::hereIs(overload));
}

static void errorMismatchGenericValue(SemanticContext* context, ErrorParam& errorParam)
{
    const BadSignatureInfos& bi = errorParam.oneTry->symMatchContext.badSignatureInfos;

    auto msg  = Fmt(Err(Err0123), bi.badGenMatch.c_str(), Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue1).c_str(), Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue2).c_str());
    auto diag = new Diagnostic{bi.badNode, msg};
    errorParam.result0->push_back(diag);
}

static void errorCannotDeduceGenericType(SemanticContext* context, ErrorParam& errorParam)
{
    const auto&              match = errorParam.oneTry->symMatchContext;
    const BadSignatureInfos& bi    = match.badSignatureInfos;

    auto msg  = Fmt(Err(Err0618), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
    auto diag = new Diagnostic{match.parameters[bi.badSignatureParameterIdx], msg};
    errorParam.result0->push_back(diag);
}

static void errorBadGenericSignature(SemanticContext* context, ErrorParam& errorParam)
{
    auto                     overload  = errorParam.oneTry->overload;
    const auto&              match     = errorParam.oneTry->symMatchContext;
    const BadSignatureInfos& bi        = match.badSignatureInfos;
    Utf8                     niceName  = "the " + Naming::kindName(overload);
    Utf8                     niceArg   = Naming::niceArgumentRank(errorParam.badParamIdx);
    auto                     errorNode = match.genericParameters[bi.badSignatureParameterIdx];

    Diagnostic* diag;
    if (match.flags & SymbolMatchContext::MATCH_ERROR_VALUE_TYPE)
    {
        auto msg = Fmt(Err(Err0054), niceArg.c_str(), niceName.c_str());
        diag     = new Diagnostic{errorNode, msg};
    }
    else if (match.flags & SymbolMatchContext::MATCH_ERROR_TYPE_VALUE)
    {
        auto msg = Fmt(Err(Err0057), niceArg.c_str(), niceName.c_str());
        diag     = new Diagnostic{errorNode, msg};
    }
    else
    {
        auto msg   = Fmt(Err(Err0070), niceArg.c_str(), niceName.c_str(), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
        diag       = new Diagnostic{errorNode, msg};
        diag->hint = errorParam.explicitCastMsg;
    }

    errorParam.result0->push_back(diag);

    // Here is
    if (errorParam.getFlags & GDFM_HERE_IS)
    {
        if (errorParam.destFuncDecl && bi.badSignatureParameterIdx < (int) errorParam.destFuncDecl->genericParameters->childs.size())
        {
            auto reqParam = errorParam.destFuncDecl->genericParameters->childs[bi.badSignatureParameterIdx];
            auto note     = Diagnostic::note(reqParam, Fmt(Nte(Nte0068), reqParam->token.ctext(), niceName.c_str()));
            errorParam.addResult1(note);
        }
        else
        {
            errorParam.addResult1(Diagnostic::hereIs(overload));
        }
    }
}

static void errorBadSignature(SemanticContext* context, ErrorParam& errorParam)
{
    auto                     overload = errorParam.oneTry->overload;
    const auto&              match    = errorParam.oneTry->symMatchContext;
    const BadSignatureInfos& bi       = match.badSignatureInfos;

    // In case of lambda, replace undefined with the corresponding match, if possible
    if (bi.badSignatureRequestedType->isLambdaClosure() && bi.badSignatureGivenType->isLambdaClosure())
    {
        auto type1 = CastTypeInfo<TypeInfoFuncAttr>(bi.badSignatureRequestedType, TypeInfoKind::LambdaClosure);
        auto type2 = CastTypeInfo<TypeInfoFuncAttr>(bi.badSignatureGivenType, TypeInfoKind::LambdaClosure);
        for (uint32_t i = 0; i < min(type1->parameters.count, type2->parameters.count); i++)
        {
            if (type2->parameters[i]->typeInfo->isUndefined())
                type2->parameters[i]->typeInfo = type1->parameters[i]->typeInfo;
        }

        if (type2->returnType && type1->returnType && type2->returnType->isUndefined())
            type2->returnType = type1->returnType;
    }

    AstNode* destParamNode = nullptr;
    if (errorParam.destParameters && bi.badSignatureParameterIdx < errorParam.destParameters->childs.size())
        destParamNode = errorParam.destParameters->childs[bi.badSignatureParameterIdx];

    SWAG_ASSERT(errorParam.oneTry->callParameters);
    auto callParamNode = match.parameters[bi.badSignatureParameterIdx];

    Diagnostic* diag;
    Utf8        hintMsg = errorParam.explicitCastMsg;

    if (overload->typeInfo->isStruct())
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
        typeStruct->flattenUsingFields();
        auto msg = Fmt(Err(Err0723), typeStruct->flattenFields[errorParam.badParamIdx - 1]->name.c_str(), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
        diag     = new Diagnostic{callParamNode, msg};
    }
    else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0 && errorParam.oneTry->dependentVar)
    {
        auto msg = Fmt(Err(Err0095), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
        diag     = new Diagnostic{context->node, context->node->token, msg};
        hintMsg  = Fmt(Nte(Nte1093), bi.badSignatureGivenType->getDisplayNameC());

        if (errorParam.oneTry->dependentVar->isGeneratedSelf())
        {
            if (bi.badSignatureGivenType->isConst())
            {
                auto note = Diagnostic::note(errorParam.oneTry->dependentVar->ownerFct, errorParam.oneTry->dependentVar->ownerFct->token, Nte(Nte0084));
                errorParam.addResult1(note);
            }
            else
            {
                auto note = Diagnostic::note(errorParam.oneTry->dependentVar->ownerFct, errorParam.oneTry->dependentVar->ownerFct->token, Nte(Nte0073));
                errorParam.addResult1(note);
            }
        }
        else
        {
            auto note = Diagnostic::note(errorParam.oneTry->dependentVar, Nte(Nte0074));
            errorParam.addResult1(note);
        }
    }
    else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0)
    {
        if (bi.castErrorType == CastErrorType::Const)
        {
            diag = new Diagnostic{callParamNode, Fmt(Err(Err1165), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC())};
            if (!overload->node->isSpecialFunctionName())
                diag->addRange(callParamNode->parent->childs.back()->token, Fmt(Nte(Nte1056), bi.badSignatureGivenType->getDisplayNameC()));
            hintMsg = Diagnostic::isType(bi.badSignatureGivenType);
            if (callParamNode->resolvedSymbolOverload && callParamNode->resolvedSymbolOverload->flags & OVERLOAD_IS_LET)
            {
                auto note = Diagnostic::note(callParamNode->resolvedSymbolOverload->node, callParamNode->resolvedSymbolOverload->node->token, Nte(Nte1050));
                errorParam.addResult1(note);
            }
        }
        else
        {
            diag = new Diagnostic{callParamNode, Fmt(Err(Err0095), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC())};
            if (!overload->node->isSpecialFunctionName())
                diag->addRange(callParamNode->parent->childs.back()->token, Fmt(Nte(Nte1093), bi.badSignatureGivenType->getDisplayNameC()));
            hintMsg = Diagnostic::isType(bi.badSignatureGivenType);
        }
    }
    else if (destParamNode && destParamNode->typeInfo->isSelf() && bi.badSignatureParameterIdx == 0)
    {
        if (callParamNode->kind == AstNodeKind::FuncDeclParam)
            callParamNode = errorParam.errorNode;
        diag = new Diagnostic{callParamNode, Fmt(Err(Err0106), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC())};
    }
    else
    {
        diag = new Diagnostic{callParamNode, Fmt(Err(Err0053), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC())};
    }

    diag->hint = hintMsg;
    errorParam.result0->push_back(diag);

    if (bi.genMatchFromNode)
    {
        auto note = Diagnostic::note(bi.genMatchFromNode, Fmt(Nte(Nte0075), bi.genMatchFromNode->typeInfo->getDisplayNameC()));
        errorParam.addResult1(note);
    }

    // A more specific cast message ?
    Utf8         castMsg, castHint;
    Vector<Utf8> castRemarks;
    TypeManager::getCastErrorMsg(castMsg, castHint, castRemarks, bi.castErrorToType, bi.castErrorFromType, bi.castErrorFlags, bi.castErrorType, true);
    if (!castMsg.empty())
    {
        if (errorParam.oneTry->dependentVar)
        {
            if (errorParam.oneTry->dependentVar->isGeneratedSelf())
            {
                errorParam.addResult1(Diagnostic::note(castMsg));
            }
            else
            {
                auto note  = Diagnostic::note(errorParam.oneTry->dependentVar, castMsg);
                note->hint = castHint;
                errorParam.addResult1(note);
            }
        }
        else if (castHint.empty())
        {
            errorParam.addResult1(Diagnostic::note(castMsg));
        }
        else
        {
            auto note  = Diagnostic::note(diag->sourceNode, diag->sourceNode->token, castMsg);
            note->hint = castHint;
            errorParam.addResult1(note);
        }
    }
    else if (diag->hint.empty())
        diag->hint = castHint;

    // Here is
    if (errorParam.getFlags & GDFM_HERE_IS)
    {
        if (destParamNode && destParamNode->isGeneratedSelf())
        {
            SWAG_ASSERT(errorParam.destFuncDecl);
            errorParam.addResult1(Diagnostic::hereIs(errorParam.destFuncDecl));
        }
        else if (destParamNode)
        {
            auto        msg  = Fmt(Nte(Nte0066), destParamNode->token.ctext(), Naming::kindName(overload).c_str());
            Diagnostic* note = Diagnostic::note(destParamNode, destParamNode->token, msg);
            errorParam.addResult1(note);
        }
        else
        {
            errorParam.addResult1(Diagnostic::hereIs(overload));
        }
    }
}

static int getBadParamIdx(const OneTryMatch& oneTry, AstNode* callParameters)
{
    const BadSignatureInfos& bi = oneTry.symMatchContext.badSignatureInfos;

    // If the first parameter of the call has been generated by ufcs, then decrease the call parameter index by 1, so
    // that the message is more relevant
    int badParamIdx = bi.badSignatureParameterIdx;
    if (badParamIdx &&
        callParameters &&
        !callParameters->childs.empty() &&
        callParameters->childs.front()->flags & (AST_FROM_UFCS | AST_TO_UFCS) &&
        !(callParameters->childs.front()->flags & AST_UFCS_FCT))
    {
        badParamIdx--;
    }
    else if (badParamIdx && oneTry.ufcs)
        badParamIdx--;

    // This is a closure with a generated first parameter
    if (oneTry.symMatchContext.flags & SymbolMatchContext::MATCH_CLOSURE_PARAM)
        badParamIdx--;

    return badParamIdx;
}

void SemanticError::getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& result0, Vector<const Diagnostic*>& result1, uint32_t getFlags)
{
    BadSignatureInfos& bi        = oneTry.symMatchContext.badSignatureInfos;
    bi.badSignatureGivenType     = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.badSignatureGivenType);
    bi.badSignatureRequestedType = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.badSignatureRequestedType);
    bi.castErrorFromType         = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.castErrorFromType);
    bi.castErrorToType           = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.castErrorToType);

    ErrorParam errorParam;
    errorParam.oneTry   = &oneTry;
    errorParam.result0  = &result0;
    errorParam.result1  = &result1;
    errorParam.getFlags = getFlags;

    // Get the call parameter that failed
    auto callParameters    = oneTry.callParameters;
    errorParam.badParamIdx = getBadParamIdx(oneTry, callParameters);
    if (oneTry.callParameters && errorParam.badParamIdx >= 0 && errorParam.badParamIdx < callParameters->childs.size())
        errorParam.failedParam = static_cast<AstFuncCallParam*>(callParameters->childs[errorParam.badParamIdx]);
    errorParam.badParamIdx += 1;

    // Error node
    errorParam.errorNode = context->node;
    if (errorParam.errorNode->isSilentCall())
    {
        errorParam.errorNode = errorParam.errorNode->parent->childs[errorParam.errorNode->childParentIdx() - 1];
        SWAG_ASSERT(errorParam.errorNode->kind == AstNodeKind::ArrayPointerIndex);
    }

    // Get parameters of destination symbol
    SymbolOverload* overload = oneTry.overload;
    if (overload->node->kind == AstNodeKind::FuncDecl)
    {
        errorParam.destFuncDecl   = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);
        errorParam.destParameters = errorParam.destFuncDecl->parameters;
    }
    else if (overload->node->kind == AstNodeKind::AttrDecl)
    {
        errorParam.destAttrDecl   = CastAst<AstAttrDecl>(overload->node, AstNodeKind::AttrDecl);
        errorParam.destParameters = errorParam.destAttrDecl->parameters;
    }
    else if (overload->node->kind == AstNodeKind::VarDecl)
    {
        errorParam.destLambdaDecl = CastAst<AstTypeLambda>(overload->node->typeInfo->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
        errorParam.destParameters = errorParam.destLambdaDecl->parameters;
    }

    // See if it would have worked with an explicit cast, to give a hint in the error message
    switch (oneTry.symMatchContext.result)
    {
    case MatchResult::BadSignature:
    case MatchResult::BadGenericSignature:
        if (bi.badSignatureGivenType->isPointer())
            break;

        if (bi.badSignatureRequestedType->isNative())
        {
            if (TypeManager::makeCompatibles(context, bi.badSignatureRequestedType, bi.badSignatureGivenType, nullptr, nullptr, CASTFLAG_TRY_COERCE | CASTFLAG_JUST_CHECK))
            {
                errorParam.explicitCastMsg = Fmt(Nte(Nte1025), bi.badSignatureRequestedType->name.c_str());
                break;
            }
        }

        if (bi.badSignatureRequestedType->isStruct())
        {
            if (TypeManager::makeCompatibles(context, bi.badSignatureRequestedType, bi.badSignatureGivenType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK))
            {
                errorParam.explicitCastMsg = Fmt(Nte(Nte1025), bi.badSignatureRequestedType->name.c_str());
                break;
            }
        }

        break;

    default:
        break;
    }

    switch (oneTry.symMatchContext.result)
    {
    case MatchResult::ValidIfFailed:
        errorValidIfFailed(context, errorParam);
        break;

    case MatchResult::MissingNamedParameter:
        errorMissingNamedParameter(context, errorParam);
        break;

    case MatchResult::InvalidNamedParameter:
        errorInvalidNamedParameter(context, errorParam);
        break;

    case MatchResult::DuplicatedNamedParameter:
        errorDuplicatedNamedParameter(context, errorParam);
        break;

    case MatchResult::MissingParameters:
        errorMissingParameters(context, errorParam);
        break;

    case MatchResult::NotEnoughParameters:
        errorNotEnoughParameters(context, errorParam);
        break;

    case MatchResult::NotEnoughGenericParameters:
        errorNotEnoughGenericParameters(context, errorParam);
        break;

    case MatchResult::TooManyParameters:
        errorTooManyParameters(context, errorParam);
        break;

    case MatchResult::TooManyGenericParameters:
        errorTooManyGenericParameters(context, errorParam);
        break;

    case MatchResult::MismatchGenericValue:
        errorMismatchGenericValue(context, errorParam);
        break;

    case MatchResult::CannotDeduceGenericType:
        errorCannotDeduceGenericType(context, errorParam);
        break;

    case MatchResult::BadGenericSignature:
        errorBadGenericSignature(context, errorParam);
        break;

    case MatchResult::BadSignature:
        errorBadSignature(context, errorParam);
        break;

    default:
        SWAG_ASSERT(false);
        break;
    }
}

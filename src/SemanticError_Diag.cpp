#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

struct ErrorParam
{
    OneTryMatch*               oneTry;
    Vector<const Diagnostic*>* diagError;
    Vector<const Diagnostic*>* diagNote;

    AstNode*          errorNode      = nullptr;
    AstFuncCallParam* failedParam    = nullptr;
    int               badParamIdx    = 0;
    AstFuncDecl*      destFuncDecl   = nullptr;
    AstTypeLambda*    destLambdaDecl = nullptr;
    AstAttrDecl*      destAttrDecl   = nullptr;
    AstStruct*        destStructDecl = nullptr;
    AstNode*          destParameters = nullptr;
    Utf8              explicitCastMsg;

    void addError(Diagnostic* note)
    {
        SWAG_ASSERT(note);
        diagError->push_back(note);
    }

    void addNote(Diagnostic* note)
    {
        if (note)
            diagNote->push_back(note);
    }
};

static void errorValidIfFailed(SemanticContext* context, ErrorParam& errorParam)
{
    const auto destFuncDecl = errorParam.destFuncDecl;
    const auto msg          = Fmt(Err(Err0085), destFuncDecl->validif->token.ctext(), destFuncDecl->token.ctext(), destFuncDecl->validif->token.ctext());
    const auto diag         = new Diagnostic{errorParam.errorNode, errorParam.errorNode->token, msg};
    errorParam.addError(diag);
    errorParam.addNote(Diagnostic::hereIs(destFuncDecl->validif));
}

static void errorMissingNamedParameter(SemanticContext* context, ErrorParam& errorParam)
{
    SWAG_ASSERT(errorParam.failedParam);
    SWAG_ASSERT(errorParam.badParamIdx >= 2);
    const auto msg  = Fmt(Err(Err0570), Naming::niceArgumentRank(errorParam.badParamIdx).c_str());
    const auto diag = new Diagnostic{errorParam.failedParam, msg};
    errorParam.addError(diag);

    const auto note = Diagnostic::note(errorParam.oneTry->callParameters->childs[errorParam.badParamIdx - 2], Nte(Nte0151));
    errorParam.addNote(note);
}

static void errorInvalidNamedParameter(SemanticContext* context, ErrorParam& errorParam)
{
    SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtMisc() && errorParam.failedParam->extMisc()->isNamed);

    const auto isNamed  = errorParam.failedParam->extMisc()->isNamed;
    const auto typeInfo = errorParam.oneTry->type;
    SWAG_ASSERT(typeInfo);

    if (errorParam.destStructDecl)
    {
        errorParam.addError(new Diagnostic{isNamed, Fmt(Err(Err0735), isNamed->token.ctext(), typeInfo->getDisplayNameC())});
        if (typeInfo->declNode->hasExtMisc() && typeInfo->declNode->extMisc()->exportNode)
            errorParam.addNote(Diagnostic::note(typeInfo->declNode->extMisc()->exportNode, Nte(Nte0078)));
        else if (errorParam.oneTry->overload)
            errorParam.addNote(Diagnostic::hereIs(errorParam.oneTry->overload));

        AlternativeScope altScope;
        altScope.scope = errorParam.destStructDecl->scope;
        const auto msg = SemanticError::findClosestMatchesMsg(isNamed->token.text, {altScope}, IdentifierSearchFor::Whatever);
        errorParam.addNote(Diagnostic::note(msg));
    }
    else
    {
        errorParam.addError(new Diagnostic{isNamed, Fmt(Err(Err0724), isNamed->token.ctext())});
        errorParam.addNote(Diagnostic::hereIs(errorParam.oneTry->overload));
    }
}

static void errorDuplicatedNamedParameter(SemanticContext* context, ErrorParam& errorParam)
{
    SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtMisc() && errorParam.failedParam->extMisc()->isNamed);
    const auto msg  = Fmt(Err(Err0023), errorParam.failedParam->extMisc()->isNamed->token.ctext());
    const auto diag = new Diagnostic{errorParam.failedParam->extMisc()->isNamed, msg};
    errorParam.addError(diag);

    const size_t other = errorParam.oneTry->symMatchContext.badSignatureInfos.badSignatureNum1;
    SWAG_ASSERT(other < errorParam.oneTry->callParameters->childs.size());
    const auto note = Diagnostic::note(errorParam.oneTry->callParameters->childs[other], Nte(Nte0165));
    errorParam.addNote(note);
}

static void errorMissingParameters(SemanticContext* context, ErrorParam& errorParam)
{
    auto node = errorParam.oneTry->callParameters;
    if (!node)
        node = errorParam.errorNode;

    const auto overload = errorParam.oneTry->overload;
    const auto diag     = new Diagnostic{node, Fmt(Err(Err0540), Naming::kindName(overload).c_str())};
    errorParam.addError(diag);
    errorParam.addNote(Diagnostic::hereIs(overload));
}

static void errorNotEnoughParameters(SemanticContext* context, ErrorParam& errorParam)
{
    const auto  node           = errorParam.errorNode;
    const auto  overload       = errorParam.oneTry->overload;
    const auto  callParameters = errorParam.oneTry->callParameters;
    const auto& match          = errorParam.oneTry->symMatchContext;
    const Utf8  niceName       = "the " + Naming::kindName(overload);

    Diagnostic* diag;
    if (!callParameters || callParameters->childs.empty())
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0540), Naming::kindName(overload).c_str())};
    else if (errorParam.destAttrDecl)
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0591), niceName.c_str())};
    else
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0592), niceName.c_str())};

    errorParam.addError(diag);
    const auto note = Diagnostic::hereIs(overload);
    errorParam.addNote(note);

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
            diag->remarks.push_back(Fmt(Nte(Nte0090), errorParam.destParameters->childs[si]->token.ctext(), errorParam.destParameters->childs[si]->typeInfo->getDisplayNameC()));
        if (note && !errorParam.destParameters->childs[si]->isGeneratedSelf())
            note->addRange(errorParam.destParameters->childs[si], "missing");
    }
}

static void errorNotEnoughGenericParameters(SemanticContext* context, ErrorParam& errorParam)
{
    const auto overload          = errorParam.oneTry->overload;
    const Utf8 niceName          = "the " + Naming::kindName(overload);
    const auto genericParameters = errorParam.oneTry->genericParameters;

    AstNode* errNode = genericParameters;
    if (!errNode)
        errNode = errorParam.errorNode;
    if (!errNode)
        errNode = context->node;

    Diagnostic* diag;
    if (errorParam.destFuncDecl && errorParam.destFuncDecl->isSpecialFunctionName())
        diag = new Diagnostic{errNode, errNode->token, Fmt(Err(Err0298), niceName.c_str(), errorParam.destFuncDecl->token.ctext())};
    else if (genericParameters)
        diag = new Diagnostic{genericParameters, Fmt(Err(Err0593), niceName.c_str())};
    else
        diag = new Diagnostic{errNode, errNode->token, Fmt(Err(Err0555), niceName.c_str())};
    errorParam.addError(diag);
    errorParam.addNote(Diagnostic::hereIs(overload));
}

static void errorTooManyParameters(SemanticContext* context, ErrorParam& errorParam)
{
    const auto  overload = errorParam.oneTry->overload;
    const auto& match    = errorParam.oneTry->symMatchContext;

    AstNode* site = errorParam.failedParam;
    if (!site)
        site = errorParam.oneTry->callParameters;
    SWAG_ASSERT(site);

    const auto msg  = Fmt(Err(Err0630), match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);
    const auto diag = new Diagnostic{site, msg};
    errorParam.addError(diag);
    errorParam.addNote(Diagnostic::hereIs(overload));
}

static void errorTooManyGenericParameters(SemanticContext* context, ErrorParam& errorParam)
{
    const auto  overload          = errorParam.oneTry->overload;
    const auto  symbol            = errorParam.oneTry->overload->symbol;
    Utf8        niceName          = "the " + Naming::kindName(overload);
    const auto  genericParameters = errorParam.oneTry->genericParameters;
    const auto& match             = errorParam.oneTry->symMatchContext;

    AstNode* errNode = genericParameters;
    if (!errNode)
        errNode = errorParam.errorNode;
    if (!errNode)
        errNode = context->node;

    Diagnostic* diag;
    if (!match.badSignatureInfos.badSignatureNum2)
    {
        const auto msg = Fmt(Err(Err0504), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str());
        diag           = new Diagnostic{errNode, msg};
    }
    else
    {
        if (genericParameters)
            errNode = genericParameters->childs[match.badSignatureInfos.badSignatureNum2];
        const auto msg = Fmt(Err(Err0631), match.badSignatureInfos.badSignatureNum2, Naming::kindName(symbol->kind).c_str(), symbol->name.c_str(),
                             match.badSignatureInfos.badSignatureNum1);
        diag = new Diagnostic{errNode, msg};
    }

    errorParam.addError(diag);
    errorParam.addNote(Diagnostic::hereIs(overload));
}

static void errorMismatchGenericValue(SemanticContext* context, ErrorParam& errorParam)
{
    const BadSignatureInfos& bi = errorParam.oneTry->symMatchContext.badSignatureInfos;

    const auto msg = Fmt(Err(Err0302), bi.badGenMatch.c_str(), Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue1).c_str(),
                         Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue2).c_str());
    const auto diag = new Diagnostic{bi.badNode, msg};
    errorParam.addError(diag);
}

static void errorCannotDeduceGenericType(SemanticContext* context, ErrorParam& errorParam)
{
    const auto&              match = errorParam.oneTry->symMatchContext;
    const BadSignatureInfos& bi    = match.badSignatureInfos;

    const auto msg  = Fmt(Err(Err0299), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
    const auto diag = new Diagnostic{match.parameters[bi.badSignatureParameterIdx], msg};
    errorParam.addError(diag);
}

static void errorBadGenericSignature(SemanticContext* context, ErrorParam& errorParam)
{
    const auto               overload  = errorParam.oneTry->overload;
    const auto&              match     = errorParam.oneTry->symMatchContext;
    const BadSignatureInfos& bi        = match.badSignatureInfos;
    const Utf8               niceArg   = Naming::niceArgumentRank(errorParam.badParamIdx);
    const auto               errorNode = match.genericParameters[bi.badSignatureParameterIdx];

    Diagnostic* diag;
    if (match.matchFlags & SymbolMatchContext::MATCH_ERROR_VALUE_TYPE)
    {
        const auto msg = Fmt(Err(Err0304));
        diag           = new Diagnostic{errorNode, msg};
    }
    else if (match.matchFlags & SymbolMatchContext::MATCH_ERROR_TYPE_VALUE)
    {
        const auto msg = Fmt(Err(Err0305));
        diag           = new Diagnostic{errorNode, msg};
    }
    else
    {
        const auto msg = Fmt(Err(Err0115), bi.badSignatureRequestedType->getDisplayNameC(), niceArg.c_str(), bi.badSignatureGivenType->getDisplayNameC());
        diag           = new Diagnostic{errorNode, msg};
        diag->hint     = errorParam.explicitCastMsg;
    }

    errorParam.addError(diag);

    // Here is
    if (errorParam.destFuncDecl &&
        bi.badSignatureParameterIdx < (int) errorParam.destFuncDecl->genericParameters->childs.size())
    {
        const auto reqParam = errorParam.destFuncDecl->genericParameters->childs[bi.badSignatureParameterIdx];
        const auto note     = Diagnostic::note(reqParam, Fmt(Nte(Nte0068), reqParam->token.ctext(), Naming::kindName(overload).c_str()));
        errorParam.addNote(note);
    }
    else
    {
        errorParam.addNote(Diagnostic::hereIs(overload));
    }
}

static void errorBadSignature(SemanticContext* context, ErrorParam& errorParam)
{
    SWAG_ASSERT(errorParam.oneTry->callParameters);

    const auto               overload = errorParam.oneTry->overload;
    const auto&              match    = errorParam.oneTry->symMatchContext;
    const BadSignatureInfos& bi       = match.badSignatureInfos;

    // In case of lambda, replace undefined with the corresponding match, if possible
    if (bi.badSignatureRequestedType->isLambdaClosure() && bi.badSignatureGivenType->isLambdaClosure())
    {
        const auto type1 = CastTypeInfo<TypeInfoFuncAttr>(bi.badSignatureRequestedType, TypeInfoKind::LambdaClosure);
        const auto type2 = CastTypeInfo<TypeInfoFuncAttr>(bi.badSignatureGivenType, TypeInfoKind::LambdaClosure);
        for (uint32_t i = 0; i < min(type1->parameters.count, type2->parameters.count); i++)
        {
            if (type2->parameters[i]->typeInfo->isUndefined())
                type2->parameters[i]->typeInfo = type1->parameters[i]->typeInfo;
        }

        if (type2->returnType && type1->returnType && type2->returnType->isUndefined())
            type2->returnType = type1->returnType;
    }

    AstNode* destParamNode = nullptr;
    if (errorParam.destParameters && bi.badSignatureParameterIdx < (int) errorParam.destParameters->childs.size())
        destParamNode = errorParam.destParameters->childs[bi.badSignatureParameterIdx];
    const auto callParamNode = match.parameters[bi.badSignatureParameterIdx];

    Diagnostic* diag               = nullptr;
    bool        addSpecificCastErr = true;
    if (overload->typeInfo->isStruct())
    {
        const auto typeStruct = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
        typeStruct->flattenUsingFields();
        const auto msg = Fmt(Err(Err0653), bi.badSignatureRequestedType->getDisplayNameC(), typeStruct->flattenFields[errorParam.badParamIdx - 1]->name.c_str(),
                             bi.badSignatureGivenType->getDisplayNameC());
        diag = new Diagnostic{callParamNode, msg};
    }
    else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0 && bi.castErrorType == CastErrorType::Const)
    {
        const auto msg     = Fmt(Err(Err0055), bi.badSignatureGivenType->getDisplayNameC());
        diag               = new Diagnostic{callParamNode, callParamNode->token, msg};
        addSpecificCastErr = false;
    }
    else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0)
    {
        const auto msg = Fmt(Err(Err0648), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
        diag           = new Diagnostic{callParamNode, callParamNode->token, msg};
    }
    else
    {
        const auto msg = Fmt(Err(Err0649), bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
        diag           = new Diagnostic{callParamNode, msg};
    }

    diag->hint = errorParam.explicitCastMsg;
    errorParam.addError(diag);

    // Let var with a const problem
    if (bi.castErrorType == CastErrorType::Const)
    {
        const auto callOver = callParamNode->resolvedSymbolOverload;
        if (callOver && callOver->flags & OVERLOAD_IS_LET)
            errorParam.addNote(Diagnostic::note(callOver->node, callOver->node->token, Nte(Nte0017)));
    }

    // Generic comes from
    if (bi.genMatchFromNode)
    {
        const auto msg  = Fmt(Nte(Nte0069), bi.genMatchFromNode->typeInfo->getDisplayNameC());
        const auto note = Diagnostic::note(bi.genMatchFromNode, msg);
        errorParam.addNote(note);
    }

    // A more specific cast message ?
    if (addSpecificCastErr)
    {
        Utf8         castMsg, castHint;
        Vector<Utf8> castRemarks;
        TypeManager::getCastErrorMsg(castMsg, castHint, castRemarks, bi.castErrorToType, bi.castErrorFromType, callParamNode, bi.castErrorFlags, bi.castErrorType, true);
        if (!castMsg.empty())
        {
            Vector<Utf8> parts;
            Diagnostic::tokenizeError(castMsg, parts);
            if (parts.size() > 1)
                castMsg = parts[1];

            if (errorParam.oneTry->dependentVar)
            {
                if (errorParam.oneTry->dependentVar->isGeneratedSelf())
                {
                    errorParam.addNote(Diagnostic::note(castMsg));
                }
                else
                {
                    const auto note = Diagnostic::note(errorParam.oneTry->dependentVar, castMsg);
                    note->hint      = castHint;
                    errorParam.addNote(note);
                }
            }
            else
            {
                const auto note = Diagnostic::note(diag->sourceNode, diag->sourceNode->token, castMsg);
                note->hint      = castHint;
                errorParam.addNote(note);
            }
        }
        else if (!castHint.empty() && diag->hint.empty())
        {
            diag->hint = castHint;
        }
    }

    // Here is
    if (destParamNode && destParamNode->isGeneratedSelf())
    {
        SWAG_ASSERT(errorParam.destFuncDecl);
        errorParam.addNote(Diagnostic::hereIs(errorParam.destFuncDecl));
    }
    else if (destParamNode && (destParamNode->flags & AST_GENERATED))
    {
        Diagnostic* note = Diagnostic::note(destParamNode, destParamNode->token, Nte(Nte0065));
        errorParam.addNote(note);
    }
    else if (destParamNode)
    {
        const auto  msg  = Fmt(Nte(Nte0064), destParamNode->token.ctext());
        Diagnostic* note = Diagnostic::note(destParamNode, destParamNode->token, msg);
        errorParam.addNote(note);
    }
    else
    {
        errorParam.addNote(Diagnostic::hereIs(overload));
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
    if (oneTry.symMatchContext.matchFlags & SymbolMatchContext::MATCH_CLOSURE_PARAM)
        badParamIdx--;

    return badParamIdx;
}

void SemanticError::getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& diagError, Vector<const Diagnostic*>& diagNote)
{
    BadSignatureInfos& bi        = oneTry.symMatchContext.badSignatureInfos;
    bi.badSignatureGivenType     = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.badSignatureGivenType);
    bi.badSignatureRequestedType = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.badSignatureRequestedType);
    bi.castErrorFromType         = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.castErrorFromType);
    bi.castErrorToType           = Generic::replaceGenericTypes(oneTry.symMatchContext.genericReplaceTypes, bi.castErrorToType);

    ErrorParam errorParam;
    errorParam.oneTry    = &oneTry;
    errorParam.diagError = &diagError;
    errorParam.diagNote  = &diagNote;

    if (!oneTry.type && oneTry.overload)
        oneTry.type = oneTry.overload->typeInfo;

    // Get the call parameter that failed
    const auto callParameters = oneTry.callParameters;
    errorParam.badParamIdx    = getBadParamIdx(oneTry, callParameters);
    if (oneTry.callParameters && errorParam.badParamIdx >= 0 && errorParam.badParamIdx < (int) callParameters->childs.size())
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
    AstNode* declNode = oneTry.overload ? oneTry.overload->node : oneTry.type->declNode;
    SWAG_ASSERT(declNode);
    if (declNode->kind == AstNodeKind::FuncDecl)
    {
        errorParam.destFuncDecl   = CastAst<AstFuncDecl>(declNode, AstNodeKind::FuncDecl);
        errorParam.destParameters = errorParam.destFuncDecl->parameters;
    }
    else if (declNode->kind == AstNodeKind::AttrDecl)
    {
        errorParam.destAttrDecl   = CastAst<AstAttrDecl>(declNode, AstNodeKind::AttrDecl);
        errorParam.destParameters = errorParam.destAttrDecl->parameters;
    }
    else if (declNode->kind == AstNodeKind::VarDecl)
    {
        errorParam.destLambdaDecl = CastAst<AstTypeLambda>(declNode->typeInfo->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
        errorParam.destParameters = errorParam.destLambdaDecl->parameters;
    }
    else if (declNode->kind == AstNodeKind::StructDecl)
    {
        errorParam.destStructDecl = CastAst<AstStruct>(declNode, AstNodeKind::StructDecl);
    }

    // See if it would have worked with an explicit cast, to give a hint in the error message
    switch (oneTry.symMatchContext.result)
    {
    case MatchResult::BadSignature:
    case MatchResult::BadGenericSignature:
        if (bi.badSignatureGivenType->isPointer())
            break;

        if (bi.badSignatureRequestedType->isNative() || bi.badSignatureRequestedType->isStruct())
        {
            if (TypeManager::makeCompatibles(context, bi.badSignatureRequestedType, bi.badSignatureGivenType, nullptr, nullptr, CASTFLAG_TRY_COERCE | CASTFLAG_JUST_CHECK))
            {
                errorParam.explicitCastMsg = Fmt(Nte(Nte0033), bi.badSignatureRequestedType->name.c_str());
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

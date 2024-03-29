#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"

struct ErrorParam
{
    OneTryMatch*               oneTry;
    Vector<const Diagnostic*>* diagError;
    Vector<const Diagnostic*>* diagNote;

    AstNode*          errorNode      = nullptr;
    AstFuncCallParam* failedParam    = nullptr;
    uint32_t          badParamIdx    = 0;
    AstFuncDecl*      destFuncDecl   = nullptr;
    AstTypeLambda*    destLambdaDecl = nullptr;
    AstAttrDecl*      destAttrDecl   = nullptr;
    AstStruct*        destStructDecl = nullptr;
    AstNode*          destParameters = nullptr;
    Utf8              explicitCastMsg;

    void addError(const Diagnostic* note) const
    {
        SWAG_ASSERT(note);
        diagError->push_back(note);
    }

    void addNote(const Diagnostic* note) const
    {
        if (note)
            diagNote->push_back(note);
    }
};

namespace
{
    void errorValidIfFailed(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto destFuncDecl = errorParam.destFuncDecl;
        const auto msg          = formErr(Err0085, destFuncDecl->validIf->token.c_str(), destFuncDecl->token.c_str(), destFuncDecl->validIf->token.c_str());
        const auto err          = new Diagnostic{errorParam.errorNode, errorParam.errorNode->token, msg};
        errorParam.addError(err);
        errorParam.addNote(Diagnostic::hereIs(destFuncDecl->validIf));
    }

    void errorMissingNamedParameter(SemanticContext*, const ErrorParam& errorParam)
    {
        SWAG_ASSERT(errorParam.failedParam);
        SWAG_ASSERT(errorParam.badParamIdx >= 2);
        const auto msg = formErr(Err0570, Naming::niceArgumentRank(errorParam.badParamIdx).c_str());
        const auto err = new Diagnostic{errorParam.failedParam, msg};
        errorParam.addError(err);

        const auto note = Diagnostic::note(errorParam.oneTry->callParameters->children[errorParam.badParamIdx - 2], toNte(Nte0151));
        errorParam.addNote(note);
    }

    void errorInvalidNamedParameter(SemanticContext*, const ErrorParam& errorParam)
    {
        SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtraPointer(ExtraPointerKind::IsNamed));

        const auto isNamed  = errorParam.failedParam->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
        const auto typeInfo = errorParam.oneTry->type;
        SWAG_ASSERT(typeInfo);

        if (errorParam.destStructDecl)
        {
            errorParam.addError(new Diagnostic{isNamed, formErr(Err0735, isNamed->token.c_str(), typeInfo->getDisplayNameC())});
            if (typeInfo->declNode->hasExtraPointer(ExtraPointerKind::ExportNode))
                errorParam.addNote(Diagnostic::note(typeInfo->declNode->extraPointer<AstNode>(ExtraPointerKind::ExportNode), toNte(Nte0078)));
            else if (errorParam.oneTry->overload)
                errorParam.addNote(Diagnostic::hereIs(errorParam.oneTry->overload));

            AlternativeScope altScope;
            altScope.scope = errorParam.destStructDecl->scope;
            const auto msg = SemanticError::findClosestMatchesMsg(isNamed->token.text, {altScope}, IdentifierSearchFor::Whatever);
            errorParam.addNote(Diagnostic::note(msg));
        }
        else
        {
            errorParam.addError(new Diagnostic{isNamed, formErr(Err0724, isNamed->token.c_str())});
            errorParam.addNote(Diagnostic::hereIs(errorParam.oneTry->overload));
        }
    }

    void errorDuplicatedNamedParameter(SemanticContext*, const ErrorParam& errorParam)
    {
        SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtraPointer(ExtraPointerKind::IsNamed));
        const auto isNamed = errorParam.failedParam->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
        const auto msg     = formErr(Err0023, isNamed->token.c_str());
        const auto err     = new Diagnostic{isNamed, msg};
        errorParam.addError(err);

        const size_t other = errorParam.oneTry->symMatchContext.badSignatureInfos.badSignatureNum1;
        SWAG_ASSERT(other < errorParam.oneTry->callParameters->childCount());
        const auto note = Diagnostic::note(errorParam.oneTry->callParameters->children[other], toNte(Nte0165));
        errorParam.addNote(note);
    }

    void errorMissingParameters(SemanticContext*, const ErrorParam& errorParam)
    {
        auto node = errorParam.oneTry->callParameters;
        if (!node)
            node = errorParam.errorNode;

        const auto overload = errorParam.oneTry->overload;
        const auto err      = new Diagnostic{node, formErr(Err0540, Naming::kindName(overload).c_str())};
        errorParam.addError(err);
        errorParam.addNote(Diagnostic::hereIs(overload));
    }

    void errorNotEnoughParameters(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto  node           = errorParam.errorNode;
        const auto  overload       = errorParam.oneTry->overload;
        const auto  callParameters = errorParam.oneTry->callParameters;
        const auto& match          = errorParam.oneTry->symMatchContext;
        const Utf8  niceName       = "the " + Naming::kindName(overload);

        Diagnostic* err;
        if (!callParameters || callParameters->children.empty())
            err = new Diagnostic{node, node->token, formErr(Err0540, Naming::kindName(overload).c_str())};
        else if (errorParam.destAttrDecl)
            err = new Diagnostic{node, node->token, formErr(Err0591, niceName.c_str())};
        else
            err = new Diagnostic{node, node->token, formErr(Err0592, niceName.c_str())};

        errorParam.addError(err);
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

            if (errorParam.destParameters->children[si]->hasSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED))
                err->remarks.push_back(formNte(Nte0089, Naming::niceParameterRank(si + 1).c_str(), errorParam.destParameters->children[si]->typeInfo->getDisplayNameC()));
            else
                err->remarks.push_back(formNte(Nte0090, errorParam.destParameters->children[si]->token.c_str(),
                                               errorParam.destParameters->children[si]->typeInfo->getDisplayNameC()));
            if (note && !errorParam.destParameters->children[si]->isGeneratedSelf())
                note->addNote(errorParam.destParameters->children[si], "missing");
        }
    }

    void errorNotEnoughGenericParameters(const SemanticContext* context, const ErrorParam& errorParam)
    {
        const auto overload          = errorParam.oneTry->overload;
        const Utf8 niceName          = "the " + Naming::kindName(overload);
        const auto genericParameters = errorParam.oneTry->genericParameters;

        AstNode* errNode = genericParameters;
        if (!errNode)
            errNode = errorParam.errorNode;
        if (!errNode)
            errNode = context->node;

        Diagnostic* err;
        if (errorParam.destFuncDecl && errorParam.destFuncDecl->isSpecialFunctionName())
            err = new Diagnostic{errNode, errNode->token, formErr(Err0298, niceName.c_str(), errorParam.destFuncDecl->token.c_str())};
        else if (genericParameters)
            err = new Diagnostic{genericParameters, formErr(Err0593, niceName.c_str())};
        else
            err = new Diagnostic{errNode, errNode->token, formErr(Err0555, niceName.c_str())};
        errorParam.addError(err);
        errorParam.addNote(Diagnostic::hereIs(overload));
    }

    void errorTooManyParameters(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto  overload = errorParam.oneTry->overload;
        const auto& match    = errorParam.oneTry->symMatchContext;

        AstNode* site = errorParam.failedParam;
        if (!site)
            site = errorParam.oneTry->callParameters;
        SWAG_ASSERT(site);

        const auto msg = formErr(Err0630, match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);
        const auto err = new Diagnostic{site, msg};
        errorParam.addError(err);
        errorParam.addNote(Diagnostic::hereIs(overload));
    }

    void errorTooManyGenericParameters(const SemanticContext* context, const ErrorParam& errorParam)
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

        Diagnostic* err;
        if (!match.badSignatureInfos.badSignatureNum2)
        {
            const auto msg = formErr(Err0504, Naming::kindName(symbol->kind).c_str(), symbol->name.c_str());
            err            = new Diagnostic{errNode, msg};
        }
        else
        {
            if (genericParameters)
                errNode = genericParameters->children[match.badSignatureInfos.badSignatureNum2];
            const auto msg = formErr(Err0631, match.badSignatureInfos.badSignatureNum2, Naming::kindName(symbol->kind).c_str(), symbol->name.c_str(),
                                     match.badSignatureInfos.badSignatureNum1);
            err            = new Diagnostic{errNode, msg};
        }

        errorParam.addError(err);
        errorParam.addNote(Diagnostic::hereIs(overload));
    }

    void errorMismatchGenericValue(SemanticContext*, const ErrorParam& errorParam)
    {
        const BadSignatureInfos& bi = errorParam.oneTry->symMatchContext.badSignatureInfos;

        const auto msg = formErr(Err0302, bi.badGenMatch.c_str(), Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue1).c_str(),
                                 Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue2).c_str());
        const auto err = new Diagnostic{bi.badNode, msg};
        errorParam.addError(err);
    }

    void errorCannotDeduceGenericType(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto&              match = errorParam.oneTry->symMatchContext;
        const BadSignatureInfos& bi    = match.badSignatureInfos;

        const auto msg = formErr(Err0299, bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
        const auto err = new Diagnostic{match.parameters[bi.badSignatureParameterIdx], msg};
        errorParam.addError(err);
    }

    void errorBadGenericSignature(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto               overload  = errorParam.oneTry->overload;
        const auto&              match     = errorParam.oneTry->symMatchContext;
        const BadSignatureInfos& bi        = match.badSignatureInfos;
        const Utf8               niceArg   = Naming::niceArgumentRank(errorParam.badParamIdx);
        const auto               errorNode = match.genericParameters[bi.badSignatureParameterIdx];

        Diagnostic* err;
        if (match.matchFlags & SymbolMatchContext::MATCH_ERROR_VALUE_TYPE)
        {
            const auto msg = form(toErr(Err0304));
            err            = new Diagnostic{errorNode, msg};
        }
        else if (match.matchFlags & SymbolMatchContext::MATCH_ERROR_TYPE_VALUE)
        {
            const auto msg = form(toErr(Err0305));
            err            = new Diagnostic{errorNode, msg};
        }
        else
        {
            const auto msg = formErr(Err0115, bi.badSignatureRequestedType->getDisplayNameC(), niceArg.c_str(), bi.badSignatureGivenType->getDisplayNameC());
            err            = new Diagnostic{errorNode, msg};
            err->hint      = errorParam.explicitCastMsg;
        }

        errorParam.addError(err);

        // Here is
        if (errorParam.destFuncDecl && bi.badSignatureParameterIdx < errorParam.destFuncDecl->genericParameters->childCount())
        {
            const auto reqParam = errorParam.destFuncDecl->genericParameters->children[bi.badSignatureParameterIdx];
            const auto note     = Diagnostic::note(reqParam, formNte(Nte0068, reqParam->token.c_str(), Naming::kindName(overload).c_str()));
            errorParam.addNote(note);
        }
        else
        {
            errorParam.addNote(Diagnostic::hereIs(overload));
        }
    }

    void errorBadSignature(SemanticContext* context, const ErrorParam& errorParam)
    {
        SWAG_ASSERT(errorParam.oneTry->callParameters);

        const auto               overload = errorParam.oneTry->overload;
        const auto&              match    = errorParam.oneTry->symMatchContext;
        const BadSignatureInfos& bi       = match.badSignatureInfos;

        // In case of lambda, replace undefined with the corresponding match, if possible
        if (bi.badSignatureRequestedType->isLambdaClosure() && bi.badSignatureGivenType->isLambdaClosure())
        {
            const auto type1 = castTypeInfo<TypeInfoFuncAttr>(bi.badSignatureRequestedType, TypeInfoKind::LambdaClosure);
            const auto type2 = castTypeInfo<TypeInfoFuncAttr>(bi.badSignatureGivenType, TypeInfoKind::LambdaClosure);
            for (uint32_t i = 0; i < min(type1->parameters.count, type2->parameters.count); i++)
            {
                if (type2->parameters[i]->typeInfo->isUndefined())
                    type2->parameters[i]->typeInfo = type1->parameters[i]->typeInfo;
            }

            if (type2->returnType && type1->returnType && type2->returnType->isUndefined())
                type2->returnType = type1->returnType;
        }

        AstNode* destParamNode = nullptr;
        if (errorParam.destParameters && bi.badSignatureParameterIdx < errorParam.destParameters->childCount())
            destParamNode = errorParam.destParameters->children[bi.badSignatureParameterIdx];
        const auto callParamNode = match.parameters[bi.badSignatureParameterIdx];

        Diagnostic* err;
        bool        addSpecificCastErr = true;
        if (overload->typeInfo->isStruct())
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
            typeStruct->flattenUsingFields();
            const auto msg = formErr(Err0653, bi.badSignatureRequestedType->getDisplayNameC(), typeStruct->flattenFields[errorParam.badParamIdx - 1]->name.c_str(),
                                     bi.badSignatureGivenType->getDisplayNameC());
            err            = new Diagnostic{callParamNode, msg};
        }
        else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0 && bi.castErrorType == CastErrorType::Const)
        {
            const auto msg     = formErr(Err0055, bi.badSignatureGivenType->getDisplayNameC());
            err                = new Diagnostic{callParamNode, callParamNode->token, msg};
            addSpecificCastErr = false;
        }
        else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0)
        {
            const auto msg = formErr(Err0648, bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
            err            = new Diagnostic{callParamNode, callParamNode->token, msg};
        }
        else
        {
            const auto msg = formErr(Err0649, bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
            err            = new Diagnostic{callParamNode, msg};
        }

        err->hint = errorParam.explicitCastMsg;
        errorParam.addError(err);

        // Let var with a const problem
        if (bi.castErrorType == CastErrorType::Const)
        {
            const auto callOver = callParamNode->resolvedSymbolOverload();
            if (callOver && callOver->hasFlag(OVERLOAD_IS_LET))
                errorParam.addNote(Diagnostic::note(callOver->node, callOver->node->token, toNte(Nte0017)));
        }

        // Generic comes from
        if (bi.genMatchFromNode)
        {
            const auto msg  = formNte(Nte0069, bi.genMatchFromNode->typeInfo->getDisplayNameC());
            const auto note = Diagnostic::note(bi.genMatchFromNode, msg);
            errorParam.addNote(note);
        }

        // A more specific cast message ?
        if (addSpecificCastErr)
        {
            Utf8         castMsg, castHint;
            Vector<Utf8> castRemarks;
            TypeManager::getCastErrorMsg(castMsg, castHint, castRemarks, bi.castErrorToType, bi.castErrorFromType, callParamNode, bi.castErrorFlags, bi.castErrorType);
            Diagnostic::removeErrorId(castMsg);

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
                    const auto note = Diagnostic::note(err->sourceNode, err->sourceNode->token, castMsg);
                    note->hint      = castHint;
                    errorParam.addNote(note);
                }
            }
            else if (!castHint.empty() && err->hint.empty())
            {
                err->hint = castHint;
            }

            // Is there an explicit cast possible ?
            if (!bi.castErrorFlags.has(CAST_FLAG_EXPLICIT) || bi.castErrorFlags.has(CAST_FLAG_COERCE))
            {
                if (bi.castErrorToType && bi.castErrorFromType)
                {
                    if (TypeManager::makeCompatibles(context, bi.castErrorToType, bi.castErrorFromType, nullptr, nullptr, CAST_FLAG_EXPLICIT | CAST_FLAG_JUST_CHECK))
                        errorParam.addNote(Diagnostic::note(callParamNode, formNte(Nte0030, bi.castErrorToType->getDisplayNameC())));
                }
            }
        }

        // Here is
        if (destParamNode && destParamNode->isGeneratedSelf())
        {
            SWAG_ASSERT(errorParam.destFuncDecl);
            errorParam.addNote(Diagnostic::hereIs(errorParam.destFuncDecl));
        }
        else if (destParamNode && destParamNode->hasAstFlag(AST_GENERATED))
        {
            const Diagnostic* note = Diagnostic::note(destParamNode, destParamNode->token, toNte(Nte0065));
            errorParam.addNote(note);
        }
        else if (destParamNode)
        {
            const auto        msg  = formNte(Nte0064, destParamNode->token.c_str());
            const Diagnostic* note = Diagnostic::note(destParamNode, destParamNode->token, msg);
            errorParam.addNote(note);
        }
        else
        {
            errorParam.addNote(Diagnostic::hereIs(overload));
        }
    }

    uint32_t getBadParamIdx(const OneTryMatch& oneTry, const AstNode* callParameters)
    {
        const BadSignatureInfos& bi = oneTry.symMatchContext.badSignatureInfos;

        // If the first parameter of the call has been generated by UFCS, then decrease the call parameter index by 1, so
        // that the message is more relevant
        auto badParamIdx = bi.badSignatureParameterIdx;
        if (badParamIdx &&
            callParameters &&
            !callParameters->children.empty() &&
            callParameters->firstChild()->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) &&
            !callParameters->firstChild()->hasAstFlag(AST_UFCS_FCT))
        {
            badParamIdx--;
        }
        else if (badParamIdx && oneTry.ufcs)
            badParamIdx--;

        // This is a closure with a generated first parameter
        if (badParamIdx && oneTry.symMatchContext.matchFlags & SymbolMatchContext::MATCH_CLOSURE_PARAM)
            badParamIdx--;

        return badParamIdx;
    }
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
    if (oneTry.callParameters && errorParam.badParamIdx < callParameters->childCount())
        errorParam.failedParam = castAst<AstFuncCallParam>(callParameters->children[errorParam.badParamIdx]);
    errorParam.badParamIdx += 1;

    // Error node
    errorParam.errorNode = context->node;
    if (errorParam.errorNode->isSilentCall())
    {
        errorParam.errorNode = errorParam.errorNode->parent->children[errorParam.errorNode->childParentIdx() - 1];
        SWAG_ASSERT(errorParam.errorNode->is(AstNodeKind::ArrayPointerIndex));
    }

    // Get parameters of destination symbol
    AstNode* declNode = oneTry.overload ? oneTry.overload->node : oneTry.type->declNode;
    SWAG_ASSERT(declNode);
    if (declNode->is(AstNodeKind::FuncDecl))
    {
        errorParam.destFuncDecl   = castAst<AstFuncDecl>(declNode, AstNodeKind::FuncDecl);
        errorParam.destParameters = errorParam.destFuncDecl->parameters;
    }
    else if (declNode->is(AstNodeKind::AttrDecl))
    {
        errorParam.destAttrDecl   = castAst<AstAttrDecl>(declNode, AstNodeKind::AttrDecl);
        errorParam.destParameters = errorParam.destAttrDecl->parameters;
    }
    else if (declNode->is(AstNodeKind::VarDecl))
    {
        errorParam.destLambdaDecl = castAst<AstTypeLambda>(declNode->typeInfo->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
        errorParam.destParameters = errorParam.destLambdaDecl->parameters;
    }
    else if (declNode->is(AstNodeKind::StructDecl))
    {
        errorParam.destStructDecl = castAst<AstStruct>(declNode, AstNodeKind::StructDecl);
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
                if (TypeManager::makeCompatibles(context, bi.badSignatureRequestedType, bi.badSignatureGivenType, nullptr, nullptr, CAST_FLAG_TRY_COERCE | CAST_FLAG_JUST_CHECK))
                {
                    errorParam.explicitCastMsg = formNte(Nte0033, bi.badSignatureRequestedType->name.c_str());
                    break;
                }
            }

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

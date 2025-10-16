#include "pch.h"

#include "Backend/LLVM/LlvmInc.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser.h"

namespace
{
    void errorMissingNamedArgument(SemanticContext*, const ErrorParam& errorParam)
    {
        SWAG_ASSERT(errorParam.failedParam);
        SWAG_ASSERT(errorParam.badParamIdx >= 2);
        const auto msg = formErr(Err0462, Naming::niceArgumentRank(errorParam.badParamIdx).cstr());
        const auto err = new Diagnostic{errorParam.failedParam, msg};
        errorParam.addError(err);

        const auto note = Diagnostic::note(errorParam.oneTry->callParameters->children[errorParam.badParamIdx - 2], "this argument has been named");
        errorParam.addNote(note);
    }

    void errorInvalidNamedArgument(SemanticContext*, const ErrorParam& errorParam)
    {
        SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtraPointer(ExtraPointerKind::IsNamed));

        const auto isNamed  = errorParam.failedParam->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
        const auto typeInfo = errorParam.oneTry->type;
        SWAG_ASSERT(typeInfo);

        if (errorParam.destStructDecl)
        {
            errorParam.addError(new Diagnostic{isNamed, formErr(Err0677, isNamed->token.cstr(), typeInfo->getDisplayNameC())});

            CollectedScope altScope;
            altScope.scope = errorParam.destStructDecl->scope;
            const auto msg = SemanticError::findClosestMatchesMsg(isNamed->token.text, {altScope}, IdentifierSearchFor::Whatever);
            errorParam.addNote(Diagnostic::note(msg));

            if (typeInfo->declNode->hasExtraPointer(ExtraPointerKind::ExportNode))
                errorParam.addNote(Diagnostic::note(typeInfo->declNode->extraPointer<AstNode>(ExtraPointerKind::ExportNode), "this is the tuple definition"));
            else if (errorParam.oneTry->overload)
                errorParam.addNote(Diagnostic::hereIs(errorParam.oneTry->overload));
        }
        else
        {
            errorParam.addError(new Diagnostic{isNamed, formErr(Err0689, isNamed->token.cstr())});

            CollectedScope altScope;
            altScope.scope = errorParam.destFuncDecl->scope;
            const auto msg = SemanticError::findClosestMatchesMsg(isNamed->token.text, {altScope}, IdentifierSearchFor::Whatever);
            errorParam.addNote(Diagnostic::note(msg));

            errorParam.addNote(Diagnostic::hereIs(errorParam.oneTry->overload));
        }
    }

    void errorDuplicatedNamedArgument(SemanticContext*, const ErrorParam& errorParam)
    {
        SWAG_ASSERT(errorParam.failedParam && errorParam.failedParam->hasExtraPointer(ExtraPointerKind::IsNamed));
        const auto isNamed = errorParam.failedParam->extraPointer<AstNode>(ExtraPointerKind::IsNamed);

        Utf8 msg;
        if (errorParam.destStructDecl)
            msg = formErr(Err0011, isNamed->token.cstr());
        else
            msg = formErr(Err0058, isNamed->token.cstr());
        const auto err = new Diagnostic{isNamed, msg};
        errorParam.addError(err);

        const size_t other = errorParam.oneTry->symMatchContext.badSignatureInfos.badSignatureNum1;
        SWAG_ASSERT(other < errorParam.oneTry->callParameters->childCount());
        const auto otherNode = errorParam.oneTry->callParameters->children[other];
        const auto isNamed2  = otherNode->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
        const auto note      = Diagnostic::note(isNamed2 ? isNamed2 : otherNode, "this is the first usage");
        errorParam.addNote(note);
    }

    void errorNotEnoughArguments(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto  node           = errorParam.errorNode;
        const auto  overload       = errorParam.oneTry->overload;
        const auto  callParameters = errorParam.oneTry->callParameters;
        const auto& match          = errorParam.oneTry->symMatchContext;

        Diagnostic* err;
        if (!callParameters)
            err = new Diagnostic{node, node->token, formErr(Err0439, Naming::kindName(overload).cstr(), node->token.cstr())};
        else if (errorParam.destAttrDecl)
            err = new Diagnostic{node, node->token, formErr(Err0486, node->token.cstr())};
        else
            err = new Diagnostic{node, node->token, formErr(Err0487, Naming::kindName(overload).cstr(), node->token.cstr())};

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

            const auto destParam = errorParam.destParameters->children[si];
            if (destParam->hasSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED) || Parser::isGeneratedName(destParam->token.text))
                err->remarks.push_back(form("missing %s of type [[%s]]", Naming::niceParameterRank(si + 1).cstr(), destParam->typeInfo->getDisplayNameC()));
            else
                err->remarks.push_back(form("missing parameter [[%s]] of type [[%s]]", destParam->token.cstr(), destParam->typeInfo->getDisplayNameC()));

            if (note && !destParam->isGeneratedMe())
                note->addNote(destParam, "missing");

            if (err->remarks.size() > 3)
            {
                err->remarks.push_back("...");
                break;
            }
        }

        if (!callParameters && errorParam.destFuncDecl)
        {
            if (node->findParent(AstNodeKind::Return) ||
                node->findParent(AstNodeKind::AffectOp) ||
                node->findParent(AstNodeKind::VarDecl))
            {
                err->addNote(form("hint: use [[&%s]] if you want a function pointer for a lambda or a closure", errorParam.destFuncDecl->token.cstr()));
            }
        }
    }

    void errorNotEnoughGenericArguments(const SemanticContext* context, const ErrorParam& errorParam)
    {
        const auto overload          = errorParam.oneTry->overload;
        const Utf8 niceName          = Naming::kindName(overload);
        const auto genericParameters = errorParam.oneTry->genericParameters;

        AstNode* errNode = genericParameters;
        if (!errNode)
            errNode = errorParam.errorNode;
        if (!errNode)
            errNode = context->node;

        Diagnostic* err;
        if (errorParam.destFuncDecl && errorParam.destFuncDecl->isSpecialFunctionName())
            err = new Diagnostic{errNode, errNode->token, formErr(Err0705, niceName.cstr(), errorParam.destFuncDecl->token.cstr())};
        else if (genericParameters)
            err = new Diagnostic{genericParameters, formErr(Err0488, niceName.cstr(), overload->symbol->name.cstr())};
        else
            err = new Diagnostic{errNode, errNode->token, formErr(Err0721, niceName.cstr())};
        errorParam.addError(err);
        const auto note = Diagnostic::hereIs(overload);
        errorParam.addNote(note);

        if (genericParameters && errorParam.destGenericParameters && genericParameters->childCount() < errorParam.destGenericParameters->childCount())
        {
            for (uint32_t si = genericParameters->childCount(); si < errorParam.destGenericParameters->childCount(); si++)
            {
                const auto destParam = errorParam.destGenericParameters->children[si];
                err->remarks.push_back(form("missing generic parameter [[%s]]", destParam->token.cstr()));
            }
        }
    }

    void errorTooManyArguments(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto  overload = errorParam.oneTry->overload;
        const auto& match    = errorParam.oneTry->symMatchContext;

        AstNode* site = errorParam.failedParam;
        if (!site)
            site = errorParam.oneTry->callParameters;
        SWAG_ASSERT(site);

        Utf8 msg;
        if (errorParam.destStructDecl && errorParam.destStructDecl->typeInfo->isTuple())
            msg = formErr(Err0523, match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);
        else if (errorParam.destStructDecl)
            msg = formErr(Err0521, errorParam.destStructDecl->typeInfo->getDisplayNameC(), match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);
        else
            msg = formErr(Err0517, match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);

        const auto err = new Diagnostic{site, msg};
        errorParam.addError(err);
        errorParam.addNote(Diagnostic::hereIs(overload));
    }

    void errorTooManyGenericArguments(const SemanticContext* context, const ErrorParam& errorParam)
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
            const auto msg = formErr(Err0400, Naming::kindName(symbol->kind).cstr(), symbol->name.cstr());
            err            = new Diagnostic{errNode, msg};
        }
        else
        {
            if (genericParameters)
                errNode = genericParameters->children[match.badSignatureInfos.badSignatureNum2];
            const auto msg = formErr(Err0518, Naming::kindName(symbol->kind).cstr(), symbol->name.cstr(), match.badSignatureInfos.badSignatureNum2, match.badSignatureInfos.badSignatureNum1);
            err            = new Diagnostic{errNode, msg};
        }

        errorParam.addError(err);
        errorParam.addNote(Diagnostic::hereIs(overload));
    }

    void errorMismatchGenericValue(SemanticContext*, const ErrorParam& errorParam)
    {
        const BadSignatureInfos& bi = errorParam.oneTry->symMatchContext.badSignatureInfos;

        const auto msg = formErr(Err0193,
                                 Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue2).cstr(),
                                 bi.badGenMatch.cstr(),
                                 Ast::literalToString(bi.badSignatureGivenType, *bi.badGenValue1).cstr());
        const auto err = new Diagnostic{bi.badNode, msg};
        errorParam.addError(err);
        errorParam.addNote(Diagnostic::isType(bi.badNode));
    }

    void errorBadGenericSignature(SemanticContext*, const ErrorParam& errorParam)
    {
        const auto               overload  = errorParam.oneTry->overload;
        const auto&              match     = errorParam.oneTry->symMatchContext;
        const BadSignatureInfos& bi        = match.badSignatureInfos;
        const Utf8               niceArg   = Naming::niceArgumentRank(errorParam.badParamIdx);
        const auto               errorNode = match.genericParameters[bi.badSignatureParameterIdx];

        Diagnostic* err;
        if (match.matchFlags.has(SymbolMatchContext::MATCH_ERROR_VALUE_TYPE))
        {
            err = new Diagnostic{errorNode, toErr(Err0398)};
        }
        else if (match.matchFlags.has(SymbolMatchContext::MATCH_ERROR_TYPE_VALUE))
        {
            err = new Diagnostic{errorNode, toErr(Err0399)};
        }
        else
        {
            const auto msg = formErr(Err0592, bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
            err            = new Diagnostic{errorNode, msg};
            err->addNote(errorParam.explicitCastMsg);
        }

        errorParam.addError(err);

        // Here is
        if (errorParam.destFuncDecl && bi.badSignatureParameterIdx < errorParam.destFuncDecl->genericParameters->childCount())
        {
            const auto reqParam = errorParam.destFuncDecl->genericParameters->children[bi.badSignatureParameterIdx];
            const auto note     = Diagnostic::note(reqParam, form("this is the generic parameter [[%s]] of the %s", reqParam->token.cstr(), Naming::kindName(overload).cstr()));
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

        // In the case of lambda, replace undefined with the corresponding match, if possible
        if (bi.badSignatureRequestedType->isLambdaClosure() && bi.badSignatureGivenType->isLambdaClosure())
        {
            const auto type1 = castTypeInfo<TypeInfoFuncAttr>(bi.badSignatureRequestedType, TypeInfoKind::LambdaClosure);
            const auto type2 = castTypeInfo<TypeInfoFuncAttr>(bi.badSignatureGivenType, TypeInfoKind::LambdaClosure);
            for (uint32_t i = 0; i < std::min(type1->parameters.count, type2->parameters.count); i++)
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

        Diagnostic* err             = nullptr;
        bool        addCastErrorMsg = true;
        if (overload->typeInfo->isStruct())
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
            typeStruct->flattenUsingFields();
            const auto fieldName = typeStruct->flattenFields[errorParam.badParamIdx - 1]->name;
            const auto msg       = formErr(Err0591, bi.badSignatureGivenType->getDisplayNameC(), fieldName.cstr(), fieldName.cstr(), bi.badSignatureRequestedType->getDisplayNameC());
            err                  = new Diagnostic{callParamNode, msg};
        }
        else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0 && bi.castErrorType == CastErrorType::Const)
        {
            const auto tname = errorParam.destFuncDecl->getTokenName().cstr();
            const auto msg   = formErr(Err0579, tname, tname, bi.badSignatureGivenType->getDisplayNameC());
            err              = new Diagnostic{callParamNode, callParamNode->token, msg};
            addCastErrorMsg  = false;
        }
        else if (errorParam.oneTry->ufcs && bi.badSignatureParameterIdx == 0)
        {
            const auto name = errorParam.oneTry->overload->node->token.cstr();
            const auto msg  = formErr(Err0537, name, bi.badSignatureGivenType->getDisplayNameC(), name, bi.badSignatureRequestedType->getDisplayNameC());
            err             = new Diagnostic{callParamNode, callParamNode->token, msg};
        }
        else if (bi.badSignatureParameterIdx == 0 &&
                 !bi.badSignatureGivenType->isPointerTo(TypeInfoKind::Struct) &&
                 errorParam.oneTry->overload->node->isMethod())
        {
            const auto name = errorParam.oneTry->overload->node->token.cstr();
            auto       n    = formErr(Err0561, bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
            err             = new Diagnostic{callParamNode, n};
            n               = form("[[%s]] is a method, call it with a pointer to [[%s]] as a first argument", name, bi.badSignatureRequestedType->getDisplayNameC());
            const auto note = Diagnostic::note(context->node, context->node->token, n);
            errorParam.addNote(note);
            errorParam.addNote(Diagnostic::note("could [[me]] be missing?"));
            addCastErrorMsg = false;
        }
        else
        {
            const auto n = formErr(Err0561, bi.badSignatureRequestedType->getDisplayNameC(), bi.badSignatureGivenType->getDisplayNameC());
            err          = new Diagnostic{callParamNode, n};
        }

        err->addNote(errorParam.explicitCastMsg);
        errorParam.addError(err);

        // Let var with a const problem
        if (bi.castErrorType == CastErrorType::Const)
        {
            const auto callOver = callParamNode->resolvedSymbolOverload();
            if (callOver && callOver->hasFlag(OVERLOAD_VAR_IS_LET))
            {
                const auto note = Diagnostic::note(callOver->node, callOver->node->token, "a [[let]] variable is immutable and cannot be modified");
                errorParam.addNote(note);
            }
        }

        // Generic comes from
        if (bi.genMatchFromNode)
        {
            const auto n    = form("the type [[%s]] is expected because it was inferred here during instantiation", bi.genMatchFromNode->typeInfo->getDisplayNameC());
            const auto note = Diagnostic::note(bi.genMatchFromNode, n);
            errorParam.addNote(note, false);
        }

        // A more specific cast message?
        if (addCastErrorMsg)
        {
            Utf8                      castMsg, castNte;
            Vector<Utf8>              castRemarks;
            Vector<const Diagnostic*> notes;
            TypeManager::getCastErrorMsg(castMsg, castNte, castRemarks, bi.castErrorToType, bi.castErrorFromType, bi.castErrorFlags, bi.castErrorType, notes);
            Diagnostic::removeErrorId(castMsg);

            if (!castMsg.empty())
            {
                Vector<Utf8> parts;
                Diagnostic::tokenizeError(castMsg, parts);
                castMsg = parts[0];

                if (errorParam.oneTry->dependentVar)
                {
                    if (errorParam.oneTry->dependentVar->isGeneratedMe())
                    {
                        errorParam.addNote(Diagnostic::note(castMsg));
                    }
                    else
                    {
                        const auto note = Diagnostic::note(errorParam.oneTry->dependentVar, castMsg);
                        note->addNote(castNte);
                        errorParam.addNote(note);
                    }
                }
                else
                {
                    const auto note = Diagnostic::note(err->sourceNode, err->sourceNode->token, castMsg);
                    note->addNote(castNte);
                    errorParam.addNote(note);
                }
            }
            else if (!castNte.empty())
            {
                err->addNote(castNte);
            }

            // Is there an explicit cast possible?
            if (!bi.castErrorFlags.has(CAST_FLAG_EXPLICIT) || bi.castErrorFlags.has(CAST_FLAG_COERCE))
            {
                if (bi.castErrorToType && bi.castErrorFromType && !bi.castErrorToType->isGeneric())
                {
                    if (TypeManager::makeCompatibles(context, bi.castErrorToType, bi.castErrorFromType, nullptr, nullptr, CAST_FLAG_EXPLICIT | CAST_FLAG_JUST_CHECK))
                    {
                        const auto n = form("hint: add an explicit [[cast(%s)]] if necessary", bi.castErrorToType->getDisplayNameC());
                        errorParam.addNote(Diagnostic::note(callParamNode, n));
                    }
                }
            }
        }

        // Here is
        if (!destParamNode)
        {
            errorParam.addNote(Diagnostic::hereIs(overload));
        }
        else if (destParamNode->isGeneratedMe())
        {
            SWAG_ASSERT(errorParam.destFuncDecl);
            errorParam.addNote(Diagnostic::hereIs(errorParam.destFuncDecl));
        }
        else if (destParamNode->hasAstFlag(AST_GENERATED) || Parser::isGeneratedName(destParamNode->token.cstr()))
        {
            const auto note = Diagnostic::note(destParamNode, destParamNode->token, "this is the corresponding parameter");
            errorParam.addNote(note);
        }
        else
        {
            const auto n    = form("this is the corresponding parameter [[%s]]", destParamNode->token.cstr());
            const auto note = Diagnostic::note(destParamNode, destParamNode->token, n);
            errorParam.addNote(note);
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
        if (badParamIdx && oneTry.symMatchContext.matchFlags.has(SymbolMatchContext::MATCH_CLOSURE_PARAM))
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
    errorParam.bi        = &oneTry.symMatchContext.badSignatureInfos;

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
        errorParam.destFuncDecl          = castAst<AstFuncDecl>(declNode, AstNodeKind::FuncDecl);
        errorParam.destParameters        = errorParam.destFuncDecl->parameters;
        errorParam.destGenericParameters = errorParam.destFuncDecl->genericParameters;
    }
    else if (declNode->is(AstNodeKind::AttrDecl))
    {
        errorParam.destAttrDecl   = castAst<AstAttrDecl>(declNode, AstNodeKind::AttrDecl);
        errorParam.destParameters = errorParam.destAttrDecl->parameters;
    }
    else if (declNode->is(AstNodeKind::VarDecl))
    {
        const auto type           = declNode->typeInfo->getConcreteAlias();
        errorParam.destLambdaDecl = castAst<AstTypeExpression>(type->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
        errorParam.destParameters = errorParam.destLambdaDecl->parameters;
    }
    else if (declNode->is(AstNodeKind::StructDecl))
    {
        errorParam.destStructDecl        = castAst<AstStruct>(declNode, AstNodeKind::StructDecl);
        errorParam.destGenericParameters = errorParam.destStructDecl->genericParameters;
    }

    // See if it had worked with an explicit cast, to give a hint in the error message
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
                    errorParam.explicitCastMsg = form("hint: cast with [[cast(%s)]]", bi.badSignatureRequestedType->name.cstr());
                    break;
                }
            }

            break;
    }

    switch (oneTry.symMatchContext.result)
    {
        case MatchResult::WhereFailed:
            errorConstraintFailed(context, errorParam, bi.failedWhere);
            break;

        case MatchResult::MissingNamedArgument:
            errorMissingNamedArgument(context, errorParam);
            break;

        case MatchResult::InvalidNamedArgument:
            errorInvalidNamedArgument(context, errorParam);
            break;

        case MatchResult::DuplicatedNamedArgument:
            errorDuplicatedNamedArgument(context, errorParam);
            break;

        case MatchResult::MissingArguments:
        case MatchResult::NotEnoughArguments:
            errorNotEnoughArguments(context, errorParam);
            break;

        case MatchResult::NotEnoughGenericArguments:
            errorNotEnoughGenericArguments(context, errorParam);
            break;

        case MatchResult::TooManyArguments:
            errorTooManyArguments(context, errorParam);
            break;

        case MatchResult::TooManyGenericArguments:
            errorTooManyGenericArguments(context, errorParam);
            break;

        case MatchResult::MismatchGenericValue:
            errorMismatchGenericValue(context, errorParam);
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

#include "pch.h"
#include "TypeMatch.h"
#include "Ast.h"
#include "Generic.h"
#include "Semantic.h"
#include "TypeManager.h"

static void matchParameters(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& parameters, uint64_t forceCastFlags = 0)
{
    // One boolean per used parameter
    context.doneParameters.set_size_clear((int) parameters.size());
    context.solvedParameters.set_size_clear((int) parameters.size());
    context.solvedCallParameters.set_size_clear((int) parameters.size());
    context.resetTmp();

    // Solve unnamed parameters
    bool isAfterVariadic = false;
    auto numParams       = context.parameters.size();
    for (size_t i = 0; i < numParams; i++)
    {
        auto callParameter = context.parameters[i];

        if (callParameter->hasExtMisc() && callParameter->extMisc()->isNamed)
        {
            context.hasNamedParameters = true;
            break;
        }

        if (i >= parameters.size() && !isAfterVariadic)
        {
            if (context.result == MatchResult::BadSignature &&
                (context.flags & SymbolMatchContext::MATCH_UFCS) &&
                context.badSignatureInfos.badSignatureParameterIdx == 0)
            {
                return;
            }

            context.badSignatureInfos.badSignatureParameterIdx = (int) i;
            context.badSignatureInfos.badSignatureNum1         = (int) numParams;
            context.badSignatureInfos.badSignatureNum2         = (int) parameters.size();
            context.result                                     = MatchResult::TooManyParameters;
            return;
        }

        if (callParameter->semFlags & SEMFLAG_AUTO_CODE_PARAM)
        {
            context.cptResolved                             = (int) context.parameters.size();
            auto param                                      = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);
            param->resolvedParameter                        = parameters.back();
            param->indexParam                               = (int) parameters.size() - 1;
            context.doneParameters[param->indexParam]       = true;
            context.solvedParameters[param->indexParam]     = parameters.back();
            context.solvedCallParameters[param->indexParam] = parameters.back();
            return;
        }

        auto wantedParameter = isAfterVariadic ? parameters.back() : parameters[i];
        auto wantedTypeInfo  = wantedParameter->typeInfo;

        if (wantedTypeInfo->isVariadic() || wantedTypeInfo->isCVariadic())
        {
            context.cptResolved = (int) context.parameters.size();
            return;
        }

        auto callTypeInfo = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);

        // For a typed variadic, cast against the underlying type
        // In case of a spread, match the underlying type too
        if (wantedTypeInfo->isTypedVariadic())
        {
            if (!callTypeInfo->isTypedVariadic() && !(callTypeInfo->flags & TYPEINFO_SPREAD))
                wantedTypeInfo = ((TypeInfoVariadic*) wantedTypeInfo)->rawType;
            isAfterVariadic = true;
        }

        // If we pass a @spread, must be match to a TypedVariadic !
        else if (callTypeInfo->flags & TYPEINFO_SPREAD)
        {
            context.badSignatureInfos.badSignatureParameterIdx  = (int) i;
            context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
            context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
            SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
            context.result = MatchResult::BadSignature;
        }

        uint64_t castFlags = CASTFLAG_JUST_CHECK | CASTFLAG_ACCEPT_PENDING | CASTFLAG_FOR_AFFECT;
        if (context.flags & SymbolMatchContext::MATCH_UNCONST)
            castFlags |= CASTFLAG_UNCONST;
        if (context.flags & SymbolMatchContext::MATCH_UFCS && i == 0)
            castFlags |= CASTFLAG_UFCS;
        if (callParameter->semFlags & SEMFLAG_LITERAL_SUFFIX)
            castFlags |= CASTFLAG_LITERAL_SUFFIX;
        if (callParameter->flags & AST_TRANSIENT && wantedTypeInfo->isPointerMoveRef())
            castFlags |= CASTFLAG_ACCEPT_MOVE_REF;
        castFlags |= forceCastFlags | CASTFLAG_PARAMS | CASTFLAG_PTR_REF;

        context.semContext->castFlagsResult   = 0;
        context.semContext->castErrorToType   = nullptr;
        context.semContext->castErrorFromType = nullptr;
        context.semContext->castErrorFlags    = 0;
        context.semContext->castErrorType     = CastErrorType::Zero;

        bool same = TypeManager::makeCompatibles(context.semContext, wantedTypeInfo, callTypeInfo, nullptr, nullptr, castFlags);
        if (context.semContext->result != ContextResult::Done)
            return;

        context.flags |= context.semContext->castFlagsResult;
        if (context.cptResolved < (int) context.doneParameters.size())
            context.doneParameters[context.cptResolved] = true;

        if (!same)
        {
            // Keep the first error
            if (context.result == MatchResult::Ok)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = (int) i;
                context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                context.badSignatureInfos.castErrorToType           = context.semContext->castErrorToType;
                context.badSignatureInfos.castErrorFromType         = context.semContext->castErrorFromType;
                context.badSignatureInfos.castErrorFlags            = context.semContext->castErrorFlags;
                context.badSignatureInfos.castErrorType             = context.semContext->castErrorType;
                SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
            }

            context.result = MatchResult::BadSignature;
        }
        else if (wantedTypeInfo->isGeneric())
        {
            Generic::deduceGenericParam(context, callParameter, callTypeInfo, wantedTypeInfo, (int) i, castFlags & (CASTFLAG_ACCEPT_PENDING | CASTFLAG_AUTO_OPCAST));
            if (context.semContext->result != ContextResult::Done)
                return;
        }

        if (context.cptResolved < (int) context.solvedParameters.size())
        {
            context.solvedParameters[context.cptResolved]     = wantedParameter;
            context.solvedCallParameters[context.cptResolved] = wantedParameter;
        }

        if (callParameter->kind == AstNodeKind::FuncCallParam)
        {
            auto param               = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);
            param->resolvedParameter = wantedParameter;
            param->indexParam        = context.cptResolved;
        }

        context.cptResolved++;
    }
}

static void matchNamedParameter(SymbolMatchContext& context, AstFuncCallParam* callParameter, int parameterIndex, VectorNative<TypeInfoParam*>& parameters, uint64_t forceCastFlags = 0)
{
    for (size_t j = 0; j < parameters.size(); j++)
    {
        auto wantedParameter = parameters[j];
        if (callParameter->hasExtMisc() &&
            callParameter->extMisc()->isNamed &&
            parameters[j]->name == callParameter->extMisc()->isNamed->token.text)
        {
            if (context.doneParameters[j])
            {
                context.badSignatureInfos.badSignatureNum1 = (int) j;
                for (size_t k = 0; k < context.parameters.size(); k++)
                {
                    auto checkParam = context.parameters[k];
                    if (checkParam->hasExtMisc() &&
                        checkParam->extMisc()->isNamed &&
                        checkParam->extMisc()->isNamed->token.text == parameters[j]->name)
                    {
                        context.badSignatureInfos.badSignatureNum1 = (int) k;
                        break;
                    }
                }
                context.badSignatureInfos.badSignatureParameterIdx = parameterIndex;
                context.result                                     = MatchResult::DuplicatedNamedParameter;
                return;
            }

            auto callTypeInfo   = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);
            auto wantedTypeInfo = wantedParameter->typeInfo;

            // For a typed variadic, cast against the underlying type
            // In case of a spread, match the underlying type too
            if (wantedTypeInfo->isTypedVariadic())
            {
                if (!callTypeInfo->isTypedVariadic() && !(callTypeInfo->flags & TYPEINFO_SPREAD))
                    wantedTypeInfo = ((TypeInfoVariadic*) wantedTypeInfo)->rawType;
            }

            // If we pass a @spread, must be match to a TypedVariadic !
            else if (callTypeInfo->flags & TYPEINFO_SPREAD)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = parameterIndex;
                context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                context.result = MatchResult::BadSignature;
            }

            uint64_t castFlags = CASTFLAG_JUST_CHECK | CASTFLAG_PARAMS;
            castFlags |= forceCastFlags;
            bool same = TypeManager::makeCompatibles(context.semContext, wantedParameter->typeInfo, callTypeInfo, nullptr, nullptr, castFlags);
            if (context.semContext->result != ContextResult::Done)
                return;

            if (!same)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = parameterIndex;
                context.badSignatureInfos.badSignatureRequestedType = wantedParameter->typeInfo;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                context.result = MatchResult::BadSignature;
            }
            else if (wantedTypeInfo->isGeneric())
            {
                Generic::deduceGenericParam(context, callParameter, callTypeInfo, wantedTypeInfo, (int) j, castFlags & (CASTFLAG_ACCEPT_PENDING | CASTFLAG_AUTO_OPCAST));
            }

            context.solvedParameters[j]                  = wantedParameter;
            context.solvedCallParameters[parameterIndex] = wantedParameter;
            callParameter->resolvedParameter             = wantedParameter;
            callParameter->indexParam                    = (int) j;
            context.doneParameters[j]                    = true;
            context.cptResolved++;
            return;
        }

        // Search inside a sub structure marked with 'using'
        if (parameters[j]->typeInfo->isStruct() && parameters[j]->declNode->flags & AST_DECL_USING)
        {
            auto subStruct = CastTypeInfo<TypeInfoStruct>(parameters[j]->typeInfo, TypeInfoKind::Struct);
            matchNamedParameter(context, callParameter, parameterIndex, subStruct->fields, forceCastFlags);
            if (callParameter->resolvedParameter)
                return;
        }

        callParameter->resolvedParameter = nullptr;
    }
}

static void matchNamedParameters(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& parameters, uint64_t forceCastFlags = 0)
{
    if (!context.hasNamedParameters)
        return;

    auto callParameter = context.parameters[0];
    callParameter->parent->flags |= AST_MUST_SORT_CHILDS;

    AstFuncCallParam fakeParam;
    Ast::constructNode(&fakeParam);
    fakeParam.kind = AstNodeKind::FuncCallParam;

    auto startResolved = context.cptResolved;
    for (size_t i = startResolved; i < context.parameters.size(); i++)
    {
        callParameter = context.parameters[i];

        if (callParameter->kind != AstNodeKind::FuncCallParam)
        {
            fakeParam.typeInfo  = callParameter->typeInfo;
            fakeParam.extension = callParameter->extension;
            callParameter       = &fakeParam;
        }

        auto param = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);

        // If this is a code paramater added by the semantic, force to match the last parameter
        // of the function
        if (param->semFlags & SEMFLAG_AUTO_CODE_PARAM)
        {
            context.cptResolved                             = (int) context.parameters.size();
            param->resolvedParameter                        = parameters.back();
            param->indexParam                               = (int) parameters.size() - 1;
            context.solvedParameters[param->indexParam]     = parameters.back();
            context.solvedCallParameters[param->indexParam] = parameters.back();
            break;
        }

        if (!param->hasExtMisc() || !param->extMisc()->isNamed)
        {
            context.badSignatureInfos.badSignatureParameterIdx = (int) i;
            context.result                                     = MatchResult::MissingNamedParameter;
            return;
        }

        matchNamedParameter(context, param, (int) i, parameters, forceCastFlags);
        if (context.result != MatchResult::DuplicatedNamedParameter)
        {
            if (!param->resolvedParameter)
            {
                context.badSignatureInfos.badSignatureParameterIdx = (int) i;
                context.result                                     = MatchResult::InvalidNamedParameter;
                return;
            }
        }
    }
}

static void matchGenericParameters(SymbolMatchContext& context, TypeInfo* myTypeInfo, VectorNative<TypeInfoParam*>& genericParameters)
{
    // Solve generic parameters
    int wantedNumGenericParams = (int) genericParameters.size();
    int userGenericParams      = (int) context.genericParameters.size();

    // It's valid to not specify generic parameters. They will be deduced.
    // A reference to a generic without specifying the generic parameters is a match
    // (we deduce the type)
    if (!userGenericParams && wantedNumGenericParams)
    {
        if (myTypeInfo->isGeneric() ||
            (context.flags & SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC) ||
            !context.genericReplaceTypes.empty())
        {
            context.flags |= SymbolMatchContext::MATCH_GENERIC_AUTO;
            if (!myTypeInfo->isGeneric() || !context.parameters.size())
            {
                for (int i = 0; i < wantedNumGenericParams; i++)
                {
                    auto symbolParameter = genericParameters[i];
                    auto genType         = symbolParameter->typeInfo;
                    SWAG_ASSERT(genType);

                    // If we try to match a generic type, and there's a contextual generic type replacement,
                    // then match with the replacement
                    if (genType->isKindGeneric())
                    {
                        auto it = context.genericReplaceTypes.find(genType->name);
                        if (it != context.genericReplaceTypes.end())
                        {
                            context.genericParametersCallTypes[i]        = it->second.typeInfoReplace;
                            context.genericParametersCallFrom[i]         = it->second.fromNode;
                            context.mapGenericTypeToIndex[genType->name] = i;
                            continue;
                        }
                    }

                    // We try to match an instance
                    else
                    {
                        // If there is a contextual value replacement, this must match, otherwise it's an irrelevant instance
                        auto it = context.genericReplaceValues.find(symbolParameter->name);
                        if (it != context.genericReplaceValues.end() && it->second)
                        {
                            if (symbolParameter->value && !Semantic::valueEqualsTo(it->second, symbolParameter->value, symbolParameter->typeInfo, 0))
                            {
                                context.result = MatchResult::NotEnoughGenericParameters;
                                return;
                            }

                            context.genericParametersCallValues[i] = it->second;

                            auto it1 = context.genericReplaceTypes.find(genType->name);
                            if (it1 != context.genericReplaceTypes.end())
                            {
                                context.genericParametersCallFrom[i] = it1->second.fromNode;
                            }

                            continue;
                        }

                        // If there is a contextual type replacement, this must match, otherwise it's an irrelevant instance
                        auto it1 = context.genericReplaceTypes.find(symbolParameter->name);
                        if (it1 != context.genericReplaceTypes.end())
                        {
                            if (genType != it1->second.typeInfoReplace)
                            {
                                context.result = MatchResult::NotEnoughGenericParameters;
                                return;
                            }
                        }
                    }

                    // Otherwise take the type, this is a match (genType can be either a generic type or a contextual match)
                    context.genericParametersCallTypes[i] = genType;
                    context.flags &= ~SymbolMatchContext::MATCH_GENERIC_AUTO;
                }

                context.result = MatchResult::Ok;
                return;
            }
        }
    }

    if (myTypeInfo->isStruct() && userGenericParams < wantedNumGenericParams)
    {
        if (userGenericParams || !myTypeInfo->isGeneric())
        {
            context.result = MatchResult::NotEnoughGenericParameters;
            return;
        }
    }

    if (!userGenericParams && wantedNumGenericParams)
    {
        for (int i = 0; i < wantedNumGenericParams; i++)
        {
            auto symbolParameter = genericParameters[i];
            if (!symbolParameter->typeInfo->isNative(NativeTypeKind::Undefined))
            {
                auto it = context.genericReplaceTypes.find(symbolParameter->typeInfo->name);
                if (it == context.genericReplaceTypes.end())
                {
                    // When we try to match an untyped generic lambda with a typed instance, we must fail.
                    // This will force a new instance with deduced types if necessary
                    if (context.genericReplaceTypes.empty() && context.flags & SymbolMatchContext::MATCH_FOR_LAMBDA)
                    {
                        context.result = MatchResult::NotEnoughGenericParameters;
                        return;
                    }
                    else if (myTypeInfo->isGeneric())
                    {
                        context.result = MatchResult::NotEnoughGenericParameters;
                        return;
                    }
                }
            }
        }
    }

    // If there's no specified generic parameters, and we try to match a concrete function, then we must to
    // be sure that the current contextual types match the contextual types of the concrete function (if they are any)
    else if (!userGenericParams && !myTypeInfo->isGeneric() && !context.genericReplaceTypes.empty())
    {
        if (myTypeInfo->declNode->kind == AstNodeKind::FuncDecl)
        {
            auto myFunc     = CastAst<AstFuncDecl>(myTypeInfo->declNode, AstNodeKind::FuncDecl);
            auto typeMyFunc = CastTypeInfo<TypeInfoFuncAttr>(myFunc->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeMyFunc->replaceTypes.empty()))
            {
                for (auto one : context.genericReplaceTypes)
                {
                    auto it = typeMyFunc->replaceTypes.find(one.first);
                    if (it == typeMyFunc->replaceTypes.end() || it->second.typeInfoReplace != one.second.typeInfoReplace)
                    {
                        context.result = MatchResult::NotEnoughGenericParameters;
                        return;
                    }
                }
            }
        }
    }

    for (int i = 0; i < userGenericParams; i++)
    {
        auto callParameter   = context.genericParameters[i];
        auto typeInfo        = TypeManager::concreteType(context.genericParametersCallTypes[i], CONCRETE_FUNC);
        auto symbolParameter = genericParameters[i];

        if (myTypeInfo->isGeneric())
        {
            auto firstChild = callParameter->childs.empty() ? nullptr : callParameter->childs.front();
            if (firstChild)
            {
                bool isValue = false;
                if (firstChild->kind == AstNodeKind::Literal)
                    isValue = true;

                if (firstChild->kind == AstNodeKind::ExpressionList)
                {
                    isValue = true;
                    if (!firstChild->hasComputedValue())
                    {
                        uint32_t     storageOffset  = UINT32_MAX;
                        DataSegment* storageSegment = Semantic::getConstantSegFromContext(firstChild);
                        firstChild->setFlagsValueIsComputed();
                        Semantic::reserveAndStoreToSegment(context.semContext, storageSegment, storageOffset, firstChild->computedValue, firstChild->typeInfo, firstChild);

                        auto typeList                             = CastTypeInfo<TypeInfoList>(firstChild->typeInfo, TypeInfoKind::TypeListArray);
                        firstChild->computedValue->reg.u64        = typeList->subTypes.size();
                        firstChild->computedValue->storageOffset  = storageOffset;
                        firstChild->computedValue->storageSegment = storageSegment;
                        callParameter->inheritComputedValue(firstChild);
                    }
                }

                if (firstChild->hasComputedValue() && !firstChild->isConstantGenTypeInfo())
                    isValue = true;
                if (firstChild->resolvedSymbolOverload &&
                    firstChild->resolvedSymbolOverload->node &&
                    firstChild->resolvedSymbolOverload->node->kind == AstNodeKind::FuncDeclParam &&
                    firstChild->resolvedSymbolOverload->node->specFlags & AstVarDecl::SPECFLAG_GENERIC_CONSTANT)
                    isValue = true;

                if ((symbolParameter->typeInfo->isKindGeneric()) && isValue)
                {
                    context.badSignatureInfos.badSignatureParameterIdx  = i;
                    context.badSignatureInfos.badSignatureRequestedType = symbolParameter->typeInfo;
                    context.badSignatureInfos.badSignatureGivenType     = typeInfo;
                    context.result                                      = MatchResult::BadGenericSignature;
                    context.flags |= SymbolMatchContext::MATCH_ERROR_VALUE_TYPE;
                    continue;
                }

                if (!symbolParameter->typeInfo->isKindGeneric() && !isValue)
                {
                    context.badSignatureInfos.badSignatureParameterIdx  = i;
                    context.badSignatureInfos.badSignatureRequestedType = symbolParameter->typeInfo;
                    context.badSignatureInfos.badSignatureGivenType     = typeInfo;
                    context.result                                      = MatchResult::BadGenericSignature;
                    context.flags |= SymbolMatchContext::MATCH_ERROR_TYPE_VALUE;
                    continue;
                }

                // If value has been deduced from the call parameters, and if value is also specified by the caller with
                // the generic parameters, then be sure this matches
                if (isValue)
                {
                    auto it1 = context.genericReplaceValues.find(symbolParameter->name);
                    if (it1 != context.genericReplaceValues.end())
                    {
                        if (!Semantic::valueEqualsTo(it1->second, callParameter))
                        {
                            context.badSignatureInfos.badNode               = callParameter;
                            context.badSignatureInfos.badGenMatch           = symbolParameter->name;
                            context.badSignatureInfos.badGenValue1          = it1->second;
                            context.badSignatureInfos.badGenValue2          = callParameter->computedValue;
                            context.badSignatureInfos.badSignatureGivenType = typeInfo;
                            context.result                                  = MatchResult::MismatchGenericValue;
                            return;
                        }
                    }
                }
            }
        }

        bool same = false;

        // Any can only match to any or to a generic
        if (typeInfo->isAny() && !symbolParameter->typeInfo->isGeneric() && !symbolParameter->typeInfo->isAny())
            same = false;
        else
            same = TypeManager::makeCompatibles(context.semContext, symbolParameter->typeInfo, typeInfo, nullptr, nullptr, CASTFLAG_FOR_GENERIC | CASTFLAG_JUST_CHECK | CASTFLAG_ACCEPT_PENDING);
        if (context.semContext->result != ContextResult::Done)
            return;

        if (!same)
        {
            context.badSignatureInfos.badSignatureParameterIdx  = i;
            context.badSignatureInfos.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureInfos.badSignatureGivenType     = typeInfo;
            context.result                                      = MatchResult::BadGenericSignature;
        }

        // We do not test computedValue for a struct, because it will contain the 'typeinfo', which can be different
        // for tuples
        else if (myTypeInfo->isGeneric() ||
                 symbolParameter->typeInfo->isStruct() ||
                 callParameter->typeInfo->isAlias() ||
                 !(symbolParameter->flags & TYPEINFOPARAM_DEFINED_VALUE) ||
                 (Semantic::valueEqualsTo(symbolParameter->value, callParameter)))
        {
            auto it = context.genericReplaceTypes.find(symbolParameter->typeInfo->name);
            if (it == context.genericReplaceTypes.end())
            {
                context.genericParametersCallTypes[i] = callParameter->typeInfo;
                context.genericParametersCallFrom[i]  = callParameter;
            }
        }
        else
        {
            context.badSignatureInfos.badSignatureParameterIdx  = i;
            context.badSignatureInfos.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureInfos.badSignatureGivenType     = typeInfo;
            context.result                                      = MatchResult::BadGenericSignature;
        }
    }
}

static void matchParametersAndNamed(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& parameters, uint64_t castFlags)
{
    matchParameters(context, parameters, castFlags);
    if (context.result == MatchResult::Ok)
        matchNamedParameters(context, parameters, castFlags);
}

void Match::match(TypeInfoFuncAttr* typeFunc, SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    Generic::setUserGenericTypeReplacement(context, typeFunc->genericParameters);
    if (context.result != MatchResult::Ok)
        return;

    // For a lambda
    if (context.flags & SymbolMatchContext::MATCH_FOR_LAMBDA)
    {
        if (!(typeFunc->flags & TYPEINFO_GENERIC))
            matchGenericParameters(context, typeFunc, typeFunc->genericParameters);
        return;
    }

    // Very special case because of automatic cast and generics.
    // We match in priority without an implicit automatic cast. If this does not match, then we
    // try with an implicit cast.
    context.autoOpCast = false;
    if (typeFunc->declNode && typeFunc->declNode->kind == AstNodeKind::FuncDecl)
    {
        auto funcNode = CastAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);

        uint32_t castFlags = CASTFLAG_DEFAULT;
        if (!funcNode->canOverload())
            castFlags |= CASTFLAG_TRY_COERCE;

        if (funcNode->parameters && (funcNode->parameters->flags & AST_IS_GENERIC))
        {
            SymbolMatchContext cpyContext = context;
            matchParametersAndNamed(cpyContext, typeFunc->parameters, castFlags);
            if (cpyContext.result == MatchResult::BadSignature)
            {
                matchParametersAndNamed(context, typeFunc->parameters, castFlags | CASTFLAG_AUTO_OPCAST);
                if (context.semContext->result != ContextResult::Done)
                    return;

                // We have a match with an automatic cast (opAffect or opCast).
                if (context.result == MatchResult::Ok)
                    context.autoOpCast = true;
            }
            else
            {
                context = std::move(cpyContext);
            }
        }
        else
        {
            matchParametersAndNamed(context, typeFunc->parameters, castFlags | CASTFLAG_AUTO_OPCAST);
        }
    }
    else
    {
        matchParametersAndNamed(context, typeFunc->parameters, CASTFLAG_TRY_COERCE | CASTFLAG_AUTO_OPCAST);
    }

    int cptDone = 0;
    if (!context.doneParameters.empty())
    {
        for (auto b : context.doneParameters)
        {
            if (!b)
                break;
            cptDone++;
        }
    }

    context.cptResolved = min(context.cptResolved, cptDone);

    // Not enough parameters
    size_t firstDefault  = typeFunc->firstDefaultValueIdx == UINT32_MAX ? typeFunc->parameters.size() : typeFunc->firstDefaultValueIdx;
    context.firstDefault = (uint32_t) firstDefault;
    if (context.cptResolved < (int) firstDefault && typeFunc->parameters.size() && context.result == MatchResult::Ok)
    {
        auto back = typeFunc->parameters.back()->typeInfo;
        if (!back->isVariadic() && !back->isTypedVariadic() && !back->isCVariadic())
        {
            context.result = MatchResult::NotEnoughParameters;
            return;
        }

        if (typeFunc->parameters.size() > 1 && context.cptResolved < (int) min(typeFunc->parameters.size() - 1, firstDefault))
        {
            context.result = MatchResult::NotEnoughParameters;
            return;
        }
    }

    if (context.result != MatchResult::Ok)
        return;

    matchGenericParameters(context, typeFunc, typeFunc->genericParameters);
    if (context.result != MatchResult::Ok)
        return;
}

void Match::match(TypeInfoStruct* typeStruct, SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    Generic::setUserGenericTypeReplacement(context, typeStruct->genericParameters);
    if (context.result != MatchResult::Ok)
        return;

    typeStruct->flattenUsingFields();
    matchParameters(context, typeStruct->flattenFields, CASTFLAG_TRY_COERCE | CASTFLAG_FORCE_UNCONST);
    if (context.result != MatchResult::Ok)
        return;

    matchNamedParameters(context, typeStruct->flattenFields, CASTFLAG_TRY_COERCE | CASTFLAG_FORCE_UNCONST);
    if (context.result != MatchResult::Ok)
        return;

    matchGenericParameters(context, typeStruct, typeStruct->genericParameters);
}
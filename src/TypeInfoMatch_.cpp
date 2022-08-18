#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Module.h"
#include "SemanticJob.h"

static void matchParameters(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& parameters, uint32_t forceCastFlags = 0)
{
    // One boolean per used parameter
    context.doneParameters.set_size_clear((int) parameters.size());
    context.solvedParameters.set_size_clear((int) parameters.size());
    context.resetTmp();

    // Solve unnamed parameters
    bool isAfterVariadic = false;
    int  numParams       = (int) context.parameters.size();
    for (int i = 0; i < numParams; i++)
    {
        auto callParameter = context.parameters[i];

        AstFuncCallParam* param = nullptr;
        if (callParameter->kind == AstNodeKind::FuncCallParam)
        {
            param = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);
            if (!param->namedParam.empty())
            {
                context.hasNamedParameters = true;
                break;
            }
        }

        if (i >= parameters.size() && !isAfterVariadic)
        {
            context.badSignatureInfos.badSignatureParameterIdx = i;
            context.badSignatureInfos.badSignatureNum1         = numParams;
            context.badSignatureInfos.badSignatureNum2         = (int) parameters.size();
            context.result                                     = MatchResult::TooManyParameters;
            return;
        }

        if (callParameter->semFlags & AST_SEM_AUTO_CODE_PARAM)
        {
            context.cptResolved                         = (int) context.parameters.size();
            param->resolvedParameter                    = parameters.back();
            param->indexParam                           = (int) parameters.size() - 1;
            context.solvedParameters[param->indexParam] = parameters.back();
            return;
        }

        auto wantedParameter = isAfterVariadic ? parameters.back() : parameters[i];
        auto wantedTypeInfo  = wantedParameter->typeInfo;

        if (wantedTypeInfo->kind == TypeInfoKind::Variadic || wantedTypeInfo->kind == TypeInfoKind::CVariadic)
        {
            context.cptResolved = (int) context.parameters.size();
            return;
        }

        auto callTypeInfo = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);

        // For a typed variadic, cast against the underlying type
        // In case of a spread, match the underlying type too
        if (wantedTypeInfo->kind == TypeInfoKind::TypedVariadic)
        {
            if (callTypeInfo->kind != TypeInfoKind::TypedVariadic && !(callTypeInfo->flags & TYPEINFO_SPREAD))
                wantedTypeInfo = ((TypeInfoVariadic*) wantedTypeInfo)->rawType;
            isAfterVariadic = true;
        }

        // If we pass a @spread, must be match to a TypedVariadic !
        else if (callTypeInfo->flags & TYPEINFO_SPREAD)
        {
            context.badSignatureInfos.badSignatureParameterIdx  = i;
            context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
            context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
            SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
            context.result = MatchResult::BadSignature;
        }

        uint32_t castFlags = CASTFLAG_NO_ERROR | CASTFLAG_ACCEPT_PENDING;
        if (context.flags & SymbolMatchContext::MATCH_UNCONST)
            castFlags |= CASTFLAG_UNCONST;
        if (context.flags & SymbolMatchContext::MATCH_UFCS && i == 0)
            castFlags |= CASTFLAG_UFCS;
        if (callParameter->semFlags & AST_SEM_LITERAL_SUFFIX)
            castFlags |= CASTFLAG_LITERAL_SUFFIX;
        castFlags |= forceCastFlags | CASTFLAG_PARAMS;

        context.semContext->castFlagsResult   = 0;
        context.semContext->castErrorToType   = nullptr;
        context.semContext->castErrorFromType = nullptr;
        context.semContext->castErrorFlags    = 0;

        bool same = TypeManager::makeCompatibles(context.semContext, wantedTypeInfo, callTypeInfo, nullptr, nullptr, castFlags);
        if (context.semContext->result != ContextResult::Done)
            return;

        context.flags |= context.semContext->castFlagsResult;

        if (!same)
        {
            // Keep the first error
            if (context.result == MatchResult::Ok)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = i;
                context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                context.badSignatureInfos.castErrorToType           = context.semContext->castErrorToType;
                context.badSignatureInfos.castErrorFromType         = context.semContext->castErrorFromType;
                context.badSignatureInfos.castErrorFlags            = context.semContext->castErrorFlags;
                SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
            }

            context.result = MatchResult::BadSignature;
        }
        else
        {
            if (context.cptResolved < context.doneParameters.size())
                context.doneParameters[context.cptResolved] = true;

            // This is a generic type match
            if (wantedTypeInfo->flags & TYPEINFO_GENERIC)
            {
                // Need to register inside types when the generic type is a compound
                VectorNative<TypeInfo*> symbolTypeInfos;
                symbolTypeInfos.push_back(wantedTypeInfo);
                VectorNative<TypeInfo*> typeInfos;
                typeInfos.push_back(callTypeInfo);
                while (symbolTypeInfos.size())
                {
                    wantedTypeInfo = symbolTypeInfos.back();

                    // When we have a reference, we match with the real type, as we do not want a generic function/struct to have a
                    // reference as a concrete type
                    callTypeInfo = TypeManager::concreteReference(typeInfos.back());

                    symbolTypeInfos.pop_back();
                    typeInfos.pop_back();

                    // Do we already have mapped the generic parameter to something ?
                    auto it = context.genericReplaceTypes.find(wantedTypeInfo->name);
                    if (it != context.genericReplaceTypes.end())
                    {
                        // If user type is undefined, then we consider this is ok, because the undefined type will be changed to the generic one
                        // Match is in fact the other way
                        if (callTypeInfo->isNative(NativeTypeKind::Undefined))
                            same = true;

                        // Yes, and the map is not the same, then this is an error
                        else
                            same = TypeManager::makeCompatibles(context.semContext, it->second, callTypeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK | CASTFLAG_FORCE_UNCONST);
                        if (!same)
                        {
                            context.badSignatureInfos.badSignatureParameterIdx  = i;
                            context.badSignatureInfos.badSignatureRequestedType = it->second;
                            context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                            context.badSignatureInfos.badGenMatch               = wantedTypeInfo->name;
                            SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                            context.result = MatchResult::BadGenericMatch;
                        }
                    }
                    else
                    {
                        bool canReg = true;
                        if (wantedTypeInfo->kind == TypeInfoKind::Pointer)
                            canReg = false;
                        else if (wantedTypeInfo->kind == TypeInfoKind::Struct && callTypeInfo->kind == TypeInfoKind::Struct)
                            canReg = wantedTypeInfo->isSame(callTypeInfo, ISSAME_CAST);

                        // Do not register type replacement if the concrete type is a pending lambda typing (we do not know
                        // yet the type of parameters)
                        if (callTypeInfo->declNode && (callTypeInfo->declNode->semFlags & AST_SEM_PENDING_LAMBDA_TYPING))
                            canReg = false;

                        if (canReg)
                        {
                            // We could have match a non const against a const
                            // We need to instantiate with the const equivalent, so make the call type const
                            if (!callTypeInfo->isConst() && wantedTypeInfo->isConst())
                            {
                                callTypeInfo = callTypeInfo->clone();
                                callTypeInfo->setConst();
                            }

                            // Associate the generic type with that concrete one
                            context.genericReplaceTypes[wantedTypeInfo->name] = callTypeInfo;

                            // If this is a valid generic argument, register it at the correct call position
                            auto itIdx = context.mapGenericTypesIndex.find(wantedTypeInfo->name);
                            if (itIdx != context.mapGenericTypesIndex.end())
                            {
                                context.genericParametersCallTypes[itIdx->second] = callTypeInfo;
                            }
                        }
                    }

                    switch (wantedTypeInfo->kind)
                    {
                    case TypeInfoKind::Struct:
                    {
                        auto symbolStruct = CastTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
                        if (callTypeInfo->kind == TypeInfoKind::Struct)
                        {
                            auto typeStruct = CastTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
                            if (!typeStruct->genericParameters.empty())
                            {
                                auto num = min(symbolStruct->genericParameters.size(), typeStruct->genericParameters.size());
                                for (int idx = 0; idx < num; idx++)
                                {
                                    auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                                    auto rawTypeInfo = typeStruct->genericParameters[idx]->typeInfo;
                                    symbolTypeInfos.push_back(genTypeInfo);
                                    typeInfos.push_back(rawTypeInfo);
                                }
                            }
                            else
                            {
                                auto num = min(symbolStruct->genericParameters.size(), typeStruct->deducedGenericParameters.size());
                                for (int idx = 0; idx < num; idx++)
                                {
                                    auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                                    auto rawTypeInfo = typeStruct->deducedGenericParameters[idx];
                                    symbolTypeInfos.push_back(genTypeInfo);
                                    typeInfos.push_back(rawTypeInfo);
                                }
                            }
                        }
                        else if (callTypeInfo->kind == TypeInfoKind::TypeListTuple)
                        {
                            auto typeList = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListTuple);
                            auto num      = min(symbolStruct->genericParameters.size(), typeList->subTypes.size());
                            for (int idx = 0; idx < num; idx++)
                            {
                                // A tuple typelist like @{a: 1, b: 2} can have named parameters, which means that the order of
                                // fields is irrelevant, as we can write @{b: 2, a: 1} too.
                                //
                                // We have a generic parameter. We search in the struct the field that correspond to that type, in
                                // order to get the corresponding field name. Then we will search for the name in the typelist (if
                                // specified).
                                auto p       = symbolStruct->genericParameters[idx];
                                Utf8 nameVar = p->namedParam;
                                for (int idx1 = 0; idx1 < (int) symbolStruct->fields.size(); idx1++)
                                {
                                    if (symbolStruct->fields[idx1]->typeInfo->name == symbolStruct->genericParameters[idx]->typeInfo->name)
                                    {
                                        nameVar = symbolStruct->fields[idx1]->namedParam;
                                        break;
                                    }
                                }

                                // Then the corresponding field name is searched in the typelist in case the user has specified one.
                                auto p1        = typeList->subTypes[idx];
                                auto typeField = p1->typeInfo;
                                for (int j = 0; j < typeList->subTypes.size(); j++)
                                {
                                    if (nameVar == typeList->subTypes[j]->namedParam)
                                    {
                                        typeField = typeList->subTypes[j]->typeInfo;
                                        break;
                                    }
                                }

                                auto genTypeInfo = p->typeInfo;
                                auto rawTypeInfo = typeField;
                                symbolTypeInfos.push_back(genTypeInfo);
                                typeInfos.push_back(rawTypeInfo);
                            }
                        }

                        break;
                    }

                    case TypeInfoKind::Pointer:
                    {
                        auto symbolPtr = CastTypeInfo<TypeInfoPointer>(wantedTypeInfo, TypeInfoKind::Pointer);
                        if (callTypeInfo->kind == TypeInfoKind::Pointer)
                        {
                            auto typePtr = CastTypeInfo<TypeInfoPointer>(callTypeInfo, TypeInfoKind::Pointer);
                            if (symbolPtr->isPointerTo(TypeInfoKind::Struct) && typePtr->isPointerTo(TypeInfoKind::Struct))
                            {
                                // Because of using var cast, we can have here *A and *B with a match.
                                // But we do not want A and B to match in generic replacement.
                                // So we check they are the same.
                                auto canNext = symbolPtr->pointedType->isSame(typePtr->pointedType, ISSAME_CAST);
                                if (canNext)
                                {
                                    symbolTypeInfos.push_back(symbolPtr->pointedType);
                                    typeInfos.push_back(typePtr->pointedType);
                                }
                            }
                            else
                            {
                                symbolTypeInfos.push_back(symbolPtr->pointedType);
                                typeInfos.push_back(typePtr->pointedType);
                            }
                        }
                        else if (callTypeInfo->kind == TypeInfoKind::Struct)
                        {
                            // Because of using var cast, we can have here *A and *B with a match.
                            // But we do not want A and B to match in generic replacement.
                            // So we check they are the same.
                            auto canNext = symbolPtr->pointedType->isSame(callTypeInfo, ISSAME_CAST);
                            if (canNext)
                            {
                                symbolTypeInfos.push_back(symbolPtr->pointedType);
                                typeInfos.push_back(callTypeInfo);
                            }
                        }
                        else
                        {
                            symbolTypeInfos.push_back(symbolPtr->pointedType);
                            typeInfos.push_back(callTypeInfo);
                        }
                        break;
                    }

                    case TypeInfoKind::Array:
                    {
                        auto symbolArray = CastTypeInfo<TypeInfoArray>(wantedTypeInfo, TypeInfoKind::Array);
                        auto typeArray   = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                        symbolTypeInfos.push_back(symbolArray->finalType);
                        typeInfos.push_back(typeArray->finalType);

                        // Array dimension was a generic symbol. Set the corresponding symbol in order to check its value
                        if (symbolArray->flags & TYPEINFO_GENERIC && symbolArray->flags & TYPEINFO_GENERIC_COUNT)
                        {
                            SWAG_ASSERT(symbolArray->sizeNode);
                            SWAG_ASSERT(symbolArray->sizeNode->resolvedSymbolName);

                            ComputedValue* cv = g_Allocator.alloc<ComputedValue>();
                            cv->reg.s64       = typeArray->count;

                            // Constant already defined ?
                            auto& cstName = symbolArray->sizeNode->resolvedSymbolName->name;
                            auto  it1     = context.genericReplaceValues.find(cstName);
                            if (it1 != context.genericReplaceValues.end())
                            {
                                if (!SemanticJob::valueEqualsTo(it1->second.first, cv, symbolArray->sizeNode->typeInfo, 0))
                                {
                                    context.badSignatureInfos.badNode               = callParameter;
                                    context.badSignatureInfos.badGenMatch           = cstName;
                                    context.badSignatureInfos.badGenValue1          = it1->second.first;
                                    context.badSignatureInfos.badGenValue2          = cv;
                                    context.badSignatureInfos.badSignatureGivenType = it1->second.second;
                                    context.result                                  = MatchResult::MismatchGenericValue;
                                }
                                else
                                {
                                    g_Allocator.free<ComputedValue>(cv);
                                    context.genericReplaceTypes[it1->second.second->name] = it1->second.second;
                                }
                            }
                            else
                            {
                                context.genericReplaceValues[cstName]                              = {cv, symbolArray->sizeNode->typeInfo};
                                context.genericReplaceTypes[symbolArray->sizeNode->typeInfo->name] = symbolArray->sizeNode->typeInfo;
                            }
                        }

                        break;
                    }

                    case TypeInfoKind::Slice:
                    {
                        auto symbolSlice = CastTypeInfo<TypeInfoSlice>(wantedTypeInfo, TypeInfoKind::Slice);
                        if (callTypeInfo->kind == TypeInfoKind::Slice)
                        {
                            auto typeSlice = CastTypeInfo<TypeInfoSlice>(callTypeInfo, TypeInfoKind::Slice);
                            symbolTypeInfos.push_back(symbolSlice->pointedType);
                            typeInfos.push_back(typeSlice->pointedType);
                        }
                        else if (callTypeInfo->kind == TypeInfoKind::Array)
                        {
                            auto typeArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                            symbolTypeInfos.push_back(symbolSlice->pointedType);
                            typeInfos.push_back(typeArray->pointedType);
                        }
                        else if (callTypeInfo->kind != TypeInfoKind::TypeListArray)
                        {
                            symbolTypeInfos.push_back(symbolSlice->pointedType);
                            typeInfos.push_back(callTypeInfo);
                        }
                        break;
                    }

                    case TypeInfoKind::Lambda:
                    {
                        auto symbolLambda = CastTypeInfo<TypeInfoFuncAttr>(wantedTypeInfo, TypeInfoKind::Lambda);
                        auto typeLambda   = CastTypeInfo<TypeInfoFuncAttr>(callTypeInfo, TypeInfoKind::Lambda);
                        if (symbolLambda->returnType && (symbolLambda->returnType->flags & TYPEINFO_GENERIC))
                        {
                            symbolTypeInfos.push_back(symbolLambda->returnType);
                            typeInfos.push_back(typeLambda->returnType);
                        }

                        auto num = symbolLambda->parameters.size();
                        for (int idx = 0; idx < num; idx++)
                        {
                            if (symbolLambda->isClosure() && !idx)
                                continue;
                            TypeInfoParam* symbolParam = symbolLambda->parameters[idx];

                            TypeInfoParam* typeParam;
                            if (symbolLambda->isClosure() && typeLambda->isLambda())
                                typeParam = typeLambda->parameters[idx - 1];
                            else
                                typeParam = typeLambda->parameters[idx];

                            if (symbolParam->typeInfo->flags & TYPEINFO_GENERIC &&
                                !typeParam->typeInfo->isNative(NativeTypeKind::Undefined)) // For lambda literals, with deduced types
                            {
                                symbolTypeInfos.push_back(symbolParam->typeInfo);
                                typeInfos.push_back(typeParam->typeInfo);
                            }
                        }
                        break;
                    }
                    }
                }
            }
        }

        if (context.cptResolved < context.solvedParameters.size())
            context.solvedParameters[context.cptResolved] = wantedParameter;

        if (param)
        {
            param->resolvedParameter = wantedParameter;
            param->indexParam        = context.cptResolved;
        }

        context.cptResolved++;
    }
}

static void matchNamedParameter(SymbolMatchContext& context, AstFuncCallParam* callParameter, int parameterIndex, VectorNative<TypeInfoParam*>& parameters, uint32_t forceCastFlags = 0)
{
    for (int j = 0; j < parameters.size(); j++)
    {
        auto wantedParameter = parameters[j];
        if (parameters[j]->namedParam == callParameter->namedParam)
        {
            if (context.doneParameters[j])
            {
                context.badSignatureInfos.badSignatureParameterIdx = parameterIndex;
                context.result                                     = MatchResult::DuplicatedNamedParameter;
                return;
            }

            auto callTypeInfo   = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);
            auto wantedTypeInfo = wantedParameter->typeInfo;

            // For a typed variadic, cast against the underlying type
            // In case of a spread, match the underlying type too
            if (wantedTypeInfo->kind == TypeInfoKind::TypedVariadic)
            {
                if (callTypeInfo->kind != TypeInfoKind::TypedVariadic && !(callTypeInfo->flags & TYPEINFO_SPREAD))
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

            uint32_t castFlags = CASTFLAG_NO_ERROR;
            castFlags |= forceCastFlags;
            bool same = TypeManager::makeCompatibles(context.semContext, wantedParameter->typeInfo, callTypeInfo, nullptr, nullptr, castFlags);
            if (!same)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = parameterIndex;
                context.badSignatureInfos.badSignatureRequestedType = wantedParameter->typeInfo;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                context.result = MatchResult::BadSignature;
            }

            context.solvedParameters[j]      = wantedParameter;
            callParameter->resolvedParameter = wantedParameter;
            callParameter->indexParam        = j;
            context.doneParameters[j]        = true;
            context.cptResolved++;
            return;
        }

        // Search inside a sub structure marked with 'using'
        if (parameters[j]->typeInfo->kind == TypeInfoKind::Struct && parameters[j]->declNode->flags & AST_DECL_USING)
        {
            auto subStruct = CastTypeInfo<TypeInfoStruct>(parameters[j]->typeInfo, TypeInfoKind::Struct);
            matchNamedParameter(context, callParameter, parameterIndex, subStruct->fields, forceCastFlags);
            if (callParameter->resolvedParameter)
                return;
        }

        callParameter->resolvedParameter = nullptr;
    }
}

static void matchNamedParameters(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& parameters, uint32_t forceCastFlags = 0)
{
    if (!context.hasNamedParameters)
        return;

    auto callParameter = context.parameters[0];
    callParameter->parent->flags |= AST_MUST_SORT_CHILDS;

    auto startResolved = context.cptResolved;
    for (int i = startResolved; i < context.parameters.size(); i++)
    {
        callParameter = context.parameters[i];
        if (callParameter->kind != AstNodeKind::FuncCallParam)
            continue;

        auto param = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);

        // If this is a code paramater added by the semantic, force to match the last parameter
        // of the function
        if (param->semFlags & AST_SEM_AUTO_CODE_PARAM)
        {
            context.cptResolved                         = (int) context.parameters.size();
            param->resolvedParameter                    = parameters.back();
            param->indexParam                           = (int) parameters.size() - 1;
            context.solvedParameters[param->indexParam] = parameters.back();
            break;
        }

        if (param->namedParam.empty())
        {
            context.badSignatureInfos.badSignatureParameterIdx = i;
            context.result                                     = MatchResult::MissingNamedParameter;
            return;
        }

        matchNamedParameter(context, param, i, parameters, forceCastFlags);
        if (context.result != MatchResult::DuplicatedNamedParameter)
        {
            if (!param->resolvedParameter)
            {
                context.badSignatureInfos.badSignatureParameterIdx = i;
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

    // It's valid to not specify generic parameters. They will be deduced
    // A reference to a generic without specifying the generic parameters is a match
    // (we deduce the type)
    if (!userGenericParams && wantedNumGenericParams && !(context.flags & SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC))
    {
        if ((myTypeInfo->flags & TYPEINFO_GENERIC) ||
            (context.flags & SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC) ||
            !context.genericReplaceTypes.empty())
        {
            if (!(myTypeInfo->flags & TYPEINFO_GENERIC) || !context.parameters.size())
            {
                context.flags |= SymbolMatchContext::MATCH_GENERIC_AUTO;
                for (int i = 0; i < wantedNumGenericParams; i++)
                {
                    auto symbolParameter = genericParameters[i];
                    auto genType         = symbolParameter->typeInfo;
                    SWAG_ASSERT(genType);

                    // If we try to match a generic type, and there's a contextual generic type replacement,
                    // then match with the replacement
                    if (genType->kind == TypeInfoKind::Generic)
                    {
                        auto it = context.genericReplaceTypes.find(genType->name);
                        if (it != context.genericReplaceTypes.end())
                        {
                            context.genericParametersCallTypes[i]       = it->second;
                            context.mapGenericTypesIndex[genType->name] = i;
                            continue;
                        }
                    }

                    // We try to match an instance. If there is a contextual type replacement, this
                    // must match, otherwise it's an irrelevant instance
                    else
                    {
                        auto it = context.genericReplaceTypes.find(symbolParameter->name);
                        if (it != context.genericReplaceTypes.end())
                        {
                            if (genType != it->second)
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

    if (myTypeInfo->kind == TypeInfoKind::Struct && userGenericParams < wantedNumGenericParams)
    {
        // In that case, we want to match the generic version of the type
        if (!userGenericParams && wantedNumGenericParams && (context.flags & SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC))
        {
            if (myTypeInfo->flags & TYPEINFO_GENERIC)
                return;
        }

        context.result = MatchResult::NotEnoughGenericParameters;
        return;
    }

    if (myTypeInfo->kind != TypeInfoKind::Struct)
    {
        if (!userGenericParams && wantedNumGenericParams)
        {
            if (!(context.flags & SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC))
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
                            else if (myTypeInfo->flags & TYPEINFO_GENERIC)
                            {
                                context.result = MatchResult::NotEnoughGenericParameters;
                                return;
                            }
                        }
                    }
                }
            }
        }

        // If there's no specified generic parameters, and we try to match a concrete function, then we must to
        // be sure that the current contextual types match the contextual types of the concrete function (if they are any)
        else if (!userGenericParams && !(myTypeInfo->flags & TYPEINFO_GENERIC) && !context.genericReplaceTypes.empty())
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
                        if (it == typeMyFunc->replaceTypes.end() || it->second != one.second)
                        {
                            context.result = MatchResult::NotEnoughGenericParameters;
                            return;
                        }
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

        if (myTypeInfo->flags & TYPEINFO_GENERIC)
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
                    if (!(firstChild->flags & AST_VALUE_COMPUTED))
                    {
                        uint32_t     storageOffset  = UINT32_MAX;
                        DataSegment* storageSegment = SemanticJob::getConstantSegFromContext(firstChild);
                        firstChild->setFlagsValueIsComputed();
                        SemanticJob::reserveAndStoreToSegment(context.semContext, storageSegment, storageOffset, firstChild->computedValue, firstChild->typeInfo, firstChild);

                        auto typeList                             = CastTypeInfo<TypeInfoList>(firstChild->typeInfo, TypeInfoKind::TypeListArray);
                        firstChild->computedValue->reg.u64        = typeList->subTypes.size();
                        firstChild->computedValue->storageOffset  = storageOffset;
                        firstChild->computedValue->storageSegment = storageSegment;
                        callParameter->inheritComputedValue(firstChild);
                    }
                }

                if ((symbolParameter->typeInfo->kind == TypeInfoKind::Generic) && isValue)
                {
                    context.badSignatureInfos.badSignatureParameterIdx  = i;
                    context.badSignatureInfos.badSignatureRequestedType = symbolParameter->typeInfo;
                    context.badSignatureInfos.badSignatureGivenType     = typeInfo;
                    context.result                                      = MatchResult::BadGenericSignature;
                    context.flags |= SymbolMatchContext::MATCH_ERROR_VALUE_TYPE;
                    continue;
                }

                if (firstChild->flags & AST_VALUE_COMPUTED && !(firstChild->flags & AST_VALUE_IS_TYPEINFO))
                    isValue = true;

                if ((symbolParameter->typeInfo->kind != TypeInfoKind::Generic) && !isValue)
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
                    auto it = context.genericReplaceValues.find(symbolParameter->namedParam);
                    if (it != context.genericReplaceValues.end())
                    {
                        if (!SemanticJob::valueEqualsTo(it->second.first, callParameter))
                        {
                            context.badSignatureInfos.badNode               = callParameter;
                            context.badSignatureInfos.badGenMatch           = symbolParameter->namedParam;
                            context.badSignatureInfos.badGenValue1          = it->second.first;
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
        if (typeInfo->isNative(NativeTypeKind::Any) && !symbolParameter->typeInfo->isGeneric() && !symbolParameter->typeInfo->isNative(NativeTypeKind::Any))
            same = false;
        else
            same = TypeManager::makeCompatibles(context.semContext, symbolParameter->typeInfo, typeInfo, nullptr, nullptr, CASTFLAG_NO_USING_ST | CASTFLAG_NO_ITF | CASTFLAG_NO_ERROR | CASTFLAG_ACCEPT_PENDING);

        if (context.semContext->result == ContextResult::Pending)
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
        else if ((myTypeInfo->flags & TYPEINFO_GENERIC) ||
                 symbolParameter->typeInfo->kind == TypeInfoKind::Struct ||
                 callParameter->typeInfo->kind == TypeInfoKind::Alias ||
                 !(symbolParameter->flags & TYPEINFO_DEFINED_VALUE) ||
                 (SemanticJob::valueEqualsTo(symbolParameter->value, callParameter)))
        {
            auto it = context.genericReplaceTypes.find(symbolParameter->typeInfo->name);
            if (it == context.genericReplaceTypes.end())
                context.genericParametersCallTypes[i] = callParameter->typeInfo;
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

static void fillUserGenericParams(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& genericParameters)
{
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();

    context.genericParametersCallTypes.expand_clear(wantedNumGenericParams);
    context.genericParametersGenTypes.set_size_clear(wantedNumGenericParams);

    if (numGenericParams > wantedNumGenericParams)
    {
        context.badSignatureInfos.badSignatureNum1 = numGenericParams;
        context.badSignatureInfos.badSignatureNum2 = wantedNumGenericParams;
        context.result                             = MatchResult::TooManyGenericParameters;
        return;
    }

    for (int i = 0; i < wantedNumGenericParams; i++)
    {
        auto genType = genericParameters[i];

        context.mapGenericTypesIndex[genType->name] = i;
        context.genericParametersGenTypes[i]        = genType->typeInfo;
    }

    for (int i = 0; i < numGenericParams; i++)
    {
        auto genType = context.genericParameters[i];
        if (!context.genericParametersCallTypes[i])
        {
            context.genericReplaceTypes[genericParameters[i]->name] = genType->typeInfo;
            context.genericParametersCallTypes[i]                   = genType->typeInfo;
        }
        else
            context.genericReplaceTypes[genericParameters[i]->name] = context.genericParametersCallTypes[i];
    }
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    fillUserGenericParams(context, genericParameters);
    if (context.result != MatchResult::Ok)
        return;

    // For a lambda
    if (context.flags & SymbolMatchContext::MATCH_FOR_LAMBDA)
    {
        if (!(flags & TYPEINFO_GENERIC))
            matchGenericParameters(context, this, genericParameters);
        return;
    }

    // If function is a generic instantiation, then we do not authorize CASTFLAG_AUTO_OPCAST (opCast), because
    // it can lead to ambiguities depending on the instantiation order :
    // First we instantate A, then B, then we call A which has an opCast to B (2558)
    uint32_t castFlags = CASTFLAG_AUTO_OPCAST;
    if (declNode && declNode->flags & AST_IS_GENERIC)
        castFlags &= ~CASTFLAG_AUTO_OPCAST;

    matchParameters(context, parameters, castFlags);
    if (context.result == MatchResult::Ok)
        matchNamedParameters(context, parameters);

    // Not enough parameters
    int firstDefault = firstDefaultValueIdx == -1 ? (int) parameters.size() : firstDefaultValueIdx;
    if (context.cptResolved < firstDefault && parameters.size() && (context.result == MatchResult::Ok || context.result == MatchResult::BadSignature))
    {
        auto back = parameters.back()->typeInfo;
        if (back->kind != TypeInfoKind::Variadic && back->kind != TypeInfoKind::TypedVariadic && back->kind != TypeInfoKind::CVariadic)
        {
            if (context.result == MatchResult::Ok)
                context.result = MatchResult::MissingSomeParameters;
            else
                context.result = MatchResult::NotEnoughParameters;
            return;
        }

        if (parameters.size() > 1 && context.cptResolved < min(parameters.size() - 1, firstDefault))
        {
            if (context.result == MatchResult::Ok)
                context.result = MatchResult::MissingSomeParameters;
            else
                context.result = MatchResult::NotEnoughParameters;
            return;
        }
    }

    if (context.result != MatchResult::Ok)
        return;

    matchGenericParameters(context, this, genericParameters);
    if (context.result != MatchResult::Ok)
        return;
}

void TypeInfoStruct::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    fillUserGenericParams(context, genericParameters);
    if (context.result != MatchResult::Ok)
        return;

    matchParameters(context, fields, CASTFLAG_TRY_COERCE | CASTFLAG_FORCE_UNCONST);
    if (context.result != MatchResult::Ok)
        return;

    matchNamedParameters(context, fields, CASTFLAG_TRY_COERCE | CASTFLAG_FORCE_UNCONST);
    if (context.result != MatchResult::Ok)
        return;

    matchGenericParameters(context, this, genericParameters);
}

TypeInfoParam* TypeInfoStruct::findChildByNameNoLock(const Utf8& childName)
{
    for (auto child : fields)
    {
        if (child->namedParam == childName)
            return child;
    }

    return nullptr;
}

TypeInfoParam* TypeInfoStruct::hasInterface(TypeInfoStruct* itf)
{
    SharedLock lk(mutex);
    return hasInterfaceNoLock(itf);
}

TypeInfoParam* TypeInfoStruct::hasInterfaceNoLock(TypeInfoStruct* itf)
{
    for (auto child : interfaces)
    {
        if (child->typeInfo->isSame(itf, ISSAME_CAST))
            return child;
    }

    return nullptr;
}
#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Module.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "LanguageSpec.h"

static void deduceGenericParam(SymbolMatchContext& context, AstNode* callParameter, TypeInfo* callTypeInfo, TypeInfo* wantedTypeInfo, int idxParam, uint64_t castFlags)
{
    SWAG_ASSERT(wantedTypeInfo->isGeneric());

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
        callTypeInfo = TypeManager::concretePtrRef(typeInfos.back());

        symbolTypeInfos.pop_back();
        typeInfos.pop_back();

        if (!callTypeInfo)
            continue;

        // Do we already have mapped the generic parameter to something ?
        auto it = context.genericReplaceTypes.find(wantedTypeInfo->name);
        if (it != context.genericReplaceTypes.end())
        {
            bool same = false;

            // If user type is undefined, then we consider this is ok, because the undefined type will be changed to the generic one
            // Match is in fact the other way
            if (callTypeInfo->isNative(NativeTypeKind::Undefined))
                same = true;

            // Yes, and the Map is not the same, then this is an error
            else
            {
                same = TypeManager::makeCompatibles(context.semContext, it->second, callTypeInfo, nullptr, nullptr, CASTFLAG_JUST_CHECK | CASTFLAG_PARAMS | castFlags);
                if (context.semContext->result != ContextResult::Done)
                    return;
            }

            if (!same)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = idxParam;
                context.badSignatureInfos.badSignatureRequestedType = it->second;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                context.badSignatureInfos.badGenMatch               = wantedTypeInfo->name;
                SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);

                auto it1 = context.genericReplaceTypesFrom.find(wantedTypeInfo->name);
                if (it1 != context.genericReplaceTypesFrom.end())
                    context.badSignatureInfos.genMatchFromNode = it1->second;

                context.result = MatchResult::BadSignature;
            }
        }
        else
        {
            bool canReg = true;
            if (wantedTypeInfo->isPointer())
                canReg = false;
            else if (wantedTypeInfo->isStruct() && callTypeInfo->isStruct())
                canReg = wantedTypeInfo->isSame(callTypeInfo, CASTFLAG_CAST);

            // Do not register type replacement if the concrete type is a pending lambda typing (we do not know
            // yet the type of parameters)
            if (callTypeInfo->declNode && (callTypeInfo->declNode->semFlags & SEMFLAG_PENDING_LAMBDA_TYPING))
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

                auto regTypeInfo = callTypeInfo;

                // :DupGen
                if (wantedTypeInfo->isStruct() && callTypeInfo->isStruct())
                {
                    auto callStruct   = CastTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
                    auto wantedStruct = CastTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
                    if (callStruct->genericParameters.size() == wantedStruct->genericParameters.size() && callStruct->genericParameters.size())
                    {
                        auto newStructType = (TypeInfoStruct*) callStruct->clone();
                        for (size_t i = 0; i < callStruct->genericParameters.size(); i++)
                        {
                            newStructType->genericParameters[i]->name = wantedStruct->genericParameters[i]->typeInfo->name;
                        }

                        regTypeInfo = newStructType;
                    }
                }

                if (wantedTypeInfo->isKindGeneric() && regTypeInfo->isListTuple())
                {
                    context.badSignatureInfos.badSignatureParameterIdx  = idxParam;
                    context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
                    context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                    SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                    context.result = MatchResult::CannotDeduceGenericType;
                    return;
                }

                // Associate the generic type with that concrete one
                context.genericReplaceTypes[wantedTypeInfo->name]     = regTypeInfo;
                context.genericReplaceTypesFrom[wantedTypeInfo->name] = callParameter;

                // If this is a valid generic argument, register it at the correct call position
                auto itIdx = context.mapGenericTypesIndex.find(wantedTypeInfo->name);
                if (itIdx != context.mapGenericTypesIndex.end())
                {
                    context.genericParametersCallTypes[itIdx->second]     = callTypeInfo;
                    context.genericParametersCallTypesFrom[itIdx->second] = callParameter;
                }
            }
        }

        switch (wantedTypeInfo->kind)
        {
        case TypeInfoKind::Struct:
        {
            auto symbolStruct = CastTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
            if (callTypeInfo->isStruct())
            {
                auto typeStruct = CastTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
                if (!typeStruct->genericParameters.empty())
                {
                    auto num = min(symbolStruct->genericParameters.size(), typeStruct->genericParameters.size());
                    for (size_t idx = 0; idx < num; idx++)
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
                    for (size_t idx = 0; idx < num; idx++)
                    {
                        auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                        auto rawTypeInfo = typeStruct->deducedGenericParameters[idx];
                        symbolTypeInfos.push_back(genTypeInfo);
                        typeInfos.push_back(rawTypeInfo);
                    }
                }
            }
            else if (callTypeInfo->isListTuple())
            {
                auto typeList = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListTuple);
                auto num      = min(symbolStruct->genericParameters.size(), typeList->subTypes.size());
                for (size_t idx = 0; idx < num; idx++)
                {
                    // A tuple typelist like {a: 1, b: 2} can have named parameters, which means that the order of
                    // fields is irrelevant, as we can write {b: 2, a: 1} too.
                    //
                    // We have a generic parameter. We search in the struct the field that correspond to that type, in
                    // order to get the corresponding field name. Then we will search for the name in the typelist (if
                    // specified).
                    auto p       = symbolStruct->genericParameters[idx];
                    Utf8 nameVar = p->name;
                    for (size_t idx1 = 0; idx1 < symbolStruct->fields.size(); idx1++)
                    {
                        if (symbolStruct->fields[idx1]->typeInfo->name == symbolStruct->genericParameters[idx]->typeInfo->name)
                        {
                            nameVar = symbolStruct->fields[idx1]->name;
                            break;
                        }
                    }

                    // Then the corresponding field name is searched in the typelist in case the user has specified one.
                    auto p1        = typeList->subTypes[idx];
                    auto typeField = p1->typeInfo;
                    for (size_t j = 0; j < typeList->subTypes.size(); j++)
                    {
                        if (nameVar == typeList->subTypes[j]->name)
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
            if (callTypeInfo->isPointer())
            {
                auto typePtr = CastTypeInfo<TypeInfoPointer>(callTypeInfo, TypeInfoKind::Pointer);
                if (symbolPtr->isPointerTo(TypeInfoKind::Struct) && typePtr->isPointerTo(TypeInfoKind::Struct))
                {
                    // Because of using var cast, we can have here *A and *B with a match.
                    // But we do not want A and B to match in generic replacement.
                    // So we check they are the same.
                    auto canNext = symbolPtr->pointedType->isSame(typePtr->pointedType, CASTFLAG_CAST);
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
            else if (callTypeInfo->isStruct())
            {
                // Because of using var cast, we can have here *A and *B with a match.
                // But we do not want A and B to match in generic replacement.
                // So we check they are the same.
                auto canNext = symbolPtr->pointedType->isSame(callTypeInfo, CASTFLAG_CAST);
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
            symbolTypeInfos.push_back(symbolArray->finalType);

            uint32_t count = 0;
            if (callTypeInfo->isArray())
            {
                auto typeArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                typeInfos.push_back(typeArray->finalType);
                count = typeArray->count;
            }
            else
            {
                SWAG_ASSERT(callTypeInfo->isListArray());
                auto typeArray = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
                typeInfos.push_back(typeArray->subTypes[0]->typeInfo);
                count = typeArray->subTypes.count;
            }

            // Array dimension was a generic symbol. Set the corresponding symbol in order to check its value
            if (symbolArray->isGeneric() && symbolArray->flags & TYPEINFO_GENERIC_COUNT)
            {
                SWAG_ASSERT(symbolArray->sizeNode);
                SWAG_ASSERT(symbolArray->sizeNode->resolvedSymbolName);

                ComputedValue* cv = Allocator::alloc<ComputedValue>();
                cv->reg.s64       = count;

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
                        Allocator::free<ComputedValue>(cv);
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
            if (callTypeInfo->isSlice())
            {
                auto typeSlice = CastTypeInfo<TypeInfoSlice>(callTypeInfo, TypeInfoKind::Slice);
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(typeSlice->pointedType);
            }
            else if (callTypeInfo->isArray())
            {
                auto typeArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(typeArray->pointedType);
            }
            else if (callTypeInfo->isListArray())
            {
                auto typeArray = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(typeArray->subTypes[0]->typeInfo);
            }
            else
            {
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(callTypeInfo);
            }
            break;
        }

        case TypeInfoKind::LambdaClosure:
        {
            auto symbolLambda = CastTypeInfo<TypeInfoFuncAttr>(wantedTypeInfo, TypeInfoKind::LambdaClosure);
            auto typeLambda   = CastTypeInfo<TypeInfoFuncAttr>(callTypeInfo, TypeInfoKind::LambdaClosure);
            if (symbolLambda->returnType && symbolLambda->returnType->isGeneric())
            {
                symbolTypeInfos.push_back(symbolLambda->returnType);
                typeInfos.push_back(typeLambda->returnType);
            }

            auto num = symbolLambda->parameters.size();
            for (size_t idx = 0; idx < num; idx++)
            {
                if (symbolLambda->isClosure() && !idx)
                    continue;
                TypeInfoParam* symbolParam = symbolLambda->parameters[idx];

                TypeInfoParam* typeParam;
                if (symbolLambda->isClosure() && typeLambda->isLambda())
                    typeParam = typeLambda->parameters[idx - 1];
                else
                    typeParam = typeLambda->parameters[idx];

                if (symbolParam->typeInfo->isGeneric() &&
                    !typeParam->typeInfo->isNative(NativeTypeKind::Undefined))
                {
                    symbolTypeInfos.push_back(symbolParam->typeInfo);
                    typeInfos.push_back(typeParam->typeInfo);
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

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

        // If we are in a generic context, we need to be sure that this will create a valid type
        if (context.genericReplaceTypes.size())
        {
            bool      invalidType      = false;
            TypeInfo* concreteTypeInfo = nullptr;
            switch (wantedTypeInfo->kind)
            {
            case TypeInfoKind::Slice:
            {
                concreteTypeInfo = Generic::doTypeSubstitution(context.genericReplaceTypes, wantedTypeInfo);
                auto typeSlice   = CastTypeInfo<TypeInfoSlice>(concreteTypeInfo, TypeInfoKind::Slice);
                if (typeSlice->pointedType->isArray() ||
                    typeSlice->pointedType->isSlice())
                {
                    invalidType = true;
                }
                break;
            }

            case TypeInfoKind::Array:
            {
                concreteTypeInfo = Generic::doTypeSubstitution(context.genericReplaceTypes, wantedTypeInfo);
                auto typeArry    = CastTypeInfo<TypeInfoArray>(concreteTypeInfo, TypeInfoKind::Array);
                if (typeArry->pointedType->isSlice())
                {
                    invalidType = true;
                }
                break;
            }

            default:
                break;
            }

            if (invalidType)
            {
                if (context.result == MatchResult::Ok)
                {
                    context.badSignatureInfos.badSignatureParameterIdx  = (int) i;
                    context.badSignatureInfos.badSignatureRequestedType = concreteTypeInfo;
                    context.badSignatureInfos.badSignatureGivenType     = wantedTypeInfo;
                    SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                }

                context.result = MatchResult::BadGenericType;
            }
        }

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

            if (context.result != MatchResult::BadGenericType)
                context.result = MatchResult::BadSignature;
        }
        else if (wantedTypeInfo->isGeneric())
        {
            deduceGenericParam(context, callParameter, callTypeInfo, wantedTypeInfo, (int) i, castFlags & (CASTFLAG_ACCEPT_PENDING | CASTFLAG_AUTO_OPCAST));
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
                deduceGenericParam(context, callParameter, callTypeInfo, wantedTypeInfo, (int) j, castFlags & (CASTFLAG_ACCEPT_PENDING | CASTFLAG_AUTO_OPCAST));
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

    // It's valid to not specify generic parameters. They will be deduced
    // A reference to a generic without specifying the generic parameters is a match
    // (we deduce the type)
    if (!userGenericParams && wantedNumGenericParams && !(context.flags & SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC))
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
                            context.genericParametersCallTypes[i]       = it->second;
                            context.mapGenericTypesIndex[genType->name] = i;

                            auto it1 = context.genericReplaceTypesFrom.find(genType->name);
                            if (it1 != context.genericReplaceTypesFrom.end())
                            {
                                context.genericReplaceTypesFrom[genType->name] = it1->second;
                                context.genericParametersCallTypesFrom[i]      = it1->second;
                            }

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

    if (myTypeInfo->isStruct() && userGenericParams < wantedNumGenericParams)
    {
        // If we have SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC, and if we do not have generic parameters (but
        // this is a generic struct), then we want to match the generic version of the type.
        // For example in @typeof(A) where A is generic.
        if (myTypeInfo->isGeneric() &&
            !userGenericParams &&
            wantedNumGenericParams &&
            (context.flags & SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC))
        {
            return;
        }

        if (userGenericParams || !myTypeInfo->isGeneric())
        {
            context.result = MatchResult::NotEnoughGenericParameters;
            return;
        }
    }

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
                        else if (myTypeInfo->isGeneric())
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
                    if (it == typeMyFunc->replaceTypes.end() || it->second != one.second)
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

                if ((symbolParameter->typeInfo->isKindGeneric()) && isValue)
                {
                    context.badSignatureInfos.badSignatureParameterIdx  = i;
                    context.badSignatureInfos.badSignatureRequestedType = symbolParameter->typeInfo;
                    context.badSignatureInfos.badSignatureGivenType     = typeInfo;
                    context.result                                      = MatchResult::BadGenericSignature;
                    context.flags |= SymbolMatchContext::MATCH_ERROR_VALUE_TYPE;
                    continue;
                }

                if (firstChild->hasComputedValue() && !firstChild->isConstantGenTypeInfo())
                    isValue = true;

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
                    auto it = context.genericReplaceValues.find(symbolParameter->name);
                    if (it != context.genericReplaceValues.end())
                    {
                        if (!SemanticJob::valueEqualsTo(it->second.first, callParameter))
                        {
                            context.badSignatureInfos.badNode               = callParameter;
                            context.badSignatureInfos.badGenMatch           = symbolParameter->name;
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
                 (SemanticJob::valueEqualsTo(symbolParameter->value, callParameter)))
        {
            auto it = context.genericReplaceTypes.find(symbolParameter->typeInfo->name);
            if (it == context.genericReplaceTypes.end())
            {
                context.genericParametersCallTypes[i]     = callParameter->typeInfo;
                context.genericParametersCallTypesFrom[i] = callParameter;
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

static void fillUserGenericParams(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& genericParameters)
{
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();

    context.genericParametersCallTypes.expand_clear(wantedNumGenericParams);
    context.genericParametersCallTypesFrom.expand_clear(wantedNumGenericParams);
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

        context.mapGenericTypesIndex[genType->typeInfo->name] = i;
        context.genericParametersGenTypes[i]                  = genType->typeInfo;
    }

    for (int i = 0; i < numGenericParams; i++)
    {
        auto genType = context.genericParameters[i];
        if (!context.genericParametersCallTypes[i])
        {
            context.genericReplaceTypes[genericParameters[i]->typeInfo->name]     = genType->typeInfo;
            context.genericReplaceTypesFrom[genericParameters[i]->typeInfo->name] = genType;
            context.genericParametersCallTypes[i]                                 = genType->typeInfo;
        }
        else
        {
            context.genericReplaceTypes[genericParameters[i]->typeInfo->name]     = context.genericParametersCallTypes[i];
            context.genericReplaceTypesFrom[genericParameters[i]->typeInfo->name] = context.genericParametersCallTypesFrom[i];
        }
    }

    for (auto i = numGenericParams; i < wantedNumGenericParams; i++)
    {
        if (context.genericParametersCallTypes[numGenericParams])
        {
            context.genericReplaceTypes[genericParameters[i]->typeInfo->name]     = context.genericParametersCallTypes[i];
            context.genericReplaceTypesFrom[genericParameters[i]->typeInfo->name] = context.genericParametersCallTypesFrom[i];
        }
    }
}

static void matchParametersAndNamed(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& parameters, uint64_t castFlags)
{
    matchParameters(context, parameters, castFlags);
    if (context.result == MatchResult::Ok)
        matchNamedParameters(context, parameters, castFlags);
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

    // Very special case because of automatic cast and generics.
    // We match in priority without an implicit automatic cast. If this does not match, then we
    // try with an implicit cast.
    context.autoOpCast = false;
    if (declNode && declNode->kind == AstNodeKind::FuncDecl)
    {
        auto funcNode = CastAst<AstFuncDecl>(declNode, AstNodeKind::FuncDecl);
        if (funcNode->parameters && (funcNode->parameters->flags & AST_IS_GENERIC))
        {
            SymbolMatchContext cpyContext = context;
            matchParametersAndNamed(cpyContext, parameters, CASTFLAG_DEFAULT);
            if (cpyContext.result == MatchResult::BadSignature)
            {
                matchParametersAndNamed(context, parameters, CASTFLAG_AUTO_OPCAST);
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
            matchParametersAndNamed(context, parameters, CASTFLAG_AUTO_OPCAST);
        }
    }
    else
    {
        matchParametersAndNamed(context, parameters, CASTFLAG_AUTO_OPCAST);
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
    size_t firstDefault = firstDefaultValueIdx == UINT32_MAX ? parameters.size() : firstDefaultValueIdx;
    if (context.cptResolved < (int) firstDefault && parameters.size() && (context.result == MatchResult::Ok || context.result == MatchResult::BadSignature))
    {
        auto back = parameters.back()->typeInfo;
        if (!back->isVariadic() && !back->isTypedVariadic() && !back->isCVariadic())
        {
            if (context.result == MatchResult::Ok)
                context.result = MatchResult::MissingSomeParameters;
            else
                context.result = MatchResult::NotEnoughParameters;
            return;
        }

        if (parameters.size() > 1 && context.cptResolved < (int) min(parameters.size() - 1, firstDefault))
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
        if (child->name == childName)
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
        if (child->typeInfo->isSame(itf, CASTFLAG_CAST))
            return child;
    }

    return nullptr;
}
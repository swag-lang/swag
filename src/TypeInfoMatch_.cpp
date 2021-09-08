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
            context.result                                     = MatchResult::TooManyParameters;
            return;
        }

        if (callParameter->semFlags & AST_SEM_AUTO_CODE_PARAM)
        {
            context.cptResolved                               = (int) context.parameters.size();
            context.solvedParameters[context.cptResolved - 1] = parameters.back();
            param->resolvedParameter                          = parameters.back();
            param->indexParam                                 = (int) parameters.size() - 1;
            return;
        }

        auto wantedParameter = isAfterVariadic ? parameters.back() : parameters[i];
        auto wantedTypeInfo  = wantedParameter->typeInfo;

        if (wantedTypeInfo->kind == TypeInfoKind::Variadic)
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

        uint32_t castFlags = CASTFLAG_NO_ERROR;
        if (context.flags & SymbolMatchContext::MATCH_UNCONST)
            castFlags |= CASTFLAG_UNCONST;
        if (context.flags & SymbolMatchContext::MATCH_UFCS && i == 0)
            castFlags |= CASTFLAG_UFCS;
        castFlags |= forceCastFlags;

        bool same = TypeManager::makeCompatibles(context.semContext, wantedTypeInfo, callTypeInfo, nullptr, nullptr, castFlags);
        if (context.semContext->result != ContextResult::Done)
            return;

        if (!same)
        {
            // Keep the first error
            if (context.result == MatchResult::Ok)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = i;
                context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
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
                            same = TypeManager::makeCompatibles(context.semContext, it->second, callTypeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK);
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

                        // Do not register type replacement if the concrete type is a pending lambda typing (we do not know
                        // yet the type of parameters)
                        if (callTypeInfo->declNode && (callTypeInfo->declNode->semFlags & AST_SEM_PENDING_LAMBDA_TYPING))
                            canReg = false;

                        if (canReg)
                        {
                            // We could have match a non const against a const
                            // We need to instantiate with the const equivalent, so make the call type const
                            auto trCallTypeInfo = callTypeInfo;
                            if (!callTypeInfo->isConst() && wantedTypeInfo->isConst())
                            {
                                trCallTypeInfo = callTypeInfo->clone();
                                trCallTypeInfo->setConst();
                            }

                            // Associate the generic type with that concrete one
                            context.genericReplaceTypes[wantedTypeInfo->name] = trCallTypeInfo;

                            // If this is a valid generic argument, register it at the correct call position
                            auto itIdx = context.mapGenericTypesIndex.find(wantedTypeInfo->name);
                            if (itIdx != context.mapGenericTypesIndex.end())
                            {
                                context.genericParametersCallTypes[itIdx->second] = trCallTypeInfo;
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
                            auto num        = symbolStruct->genericParameters.size();
                            for (int idx = 0; idx < num; idx++)
                            {
                                auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                                auto rawTypeInfo = typeStruct->genericParameters[idx]->typeInfo;
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
                            symbolTypeInfos.push_back(symbolPtr->pointedType);
                            typeInfos.push_back(typePtr->pointedType);
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
                            auto symbolParam = symbolLambda->parameters[idx];
                            auto typeParam   = typeLambda->parameters[idx];
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
            context.cptResolved                               = (int) context.parameters.size();
            context.solvedParameters[context.cptResolved - 1] = parameters.back();
            param->resolvedParameter                          = parameters.back();
            param->indexParam                                 = (int) parameters.size() - 1;
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

static bool valueEqualsTo(const ComputedValue* value, AstNode* node)
{
    if (!value && !node->computedValue)
        return true;

    // Types
    if (node->flags & AST_VALUE_IS_TYPEINFO)
    {
        if (!value)
            return false;
        if (value->reg.u64 == node->computedValue->reg.u64)
            return true;

        auto typeInfo1 = (TypeInfo*) value->reg.u64;
        auto typeInfo2 = (TypeInfo*) node->computedValue->reg.u64;
        if (!typeInfo1 || !typeInfo2)
            return false;

        if (typeInfo1->isSame(typeInfo2, ISSAME_EXACT))
            return true;
    }

    if (node->typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        if (!value)
            return false;
        if (value->storageSegment != node->computedValue->storageSegment)
            return false;
        if (value->storageOffset == UINT32_MAX && node->computedValue->storageOffset != UINT32_MAX)
            return false;
        if (value->storageOffset != UINT32_MAX && node->computedValue->storageOffset == UINT32_MAX)
            return false;
        if (value->storageOffset == node->computedValue->storageOffset)
            return true;

        void* addr1 = value->storageSegment->address(value->storageOffset);
        void* addr2 = node->computedValue->storageSegment->address(node->computedValue->storageOffset);
        return memcmp(addr1, addr2, node->typeInfo->sizeOf) == 0;
    }

    node->allocateComputedValue();
    return *value == *node->computedValue;
}

static void matchGenericParameters(SymbolMatchContext& context, TypeInfo* myTypeInfo, VectorNative<TypeInfoParam*>& genericParameters)
{
    // Solve generic parameters
    int wantedNumGenericParams = (int) genericParameters.size();
    int userGenericParams      = (int) context.genericParameters.size();

    // It's valid to not specify generic parameters. They will be deduced
    if (myTypeInfo->kind == TypeInfoKind::Struct)
    {
        // A reference to a generic without specifying the generic parameters is a match
        // (we deduce the type)
        if (!userGenericParams && wantedNumGenericParams)
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

        if (userGenericParams < wantedNumGenericParams)
        {
            context.result = MatchResult::NotEnoughGenericParameters;
            return;
        }
    }
    else
    {
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
                        else if (myTypeInfo->flags & TYPEINFO_GENERIC)
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
        auto symbolParameter = genericParameters[i];
        auto typeInfo        = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);

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
            }
        }

        bool same = TypeManager::makeCompatibles(context.semContext, symbolParameter->typeInfo, typeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_ACCEPT_PENDING);
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
                 (valueEqualsTo(symbolParameter->value, callParameter)))
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

    context.genericParametersCallTypes.set_size_clear(wantedNumGenericParams);
    context.genericParametersGenTypes.set_size_clear(wantedNumGenericParams);

    // It's valid to not specify generic parameters. They will be deduced
    if (numGenericParams && numGenericParams < wantedNumGenericParams)
    {
        context.result = MatchResult::NotEnoughGenericParameters;
        return;
    }

    if (numGenericParams > wantedNumGenericParams)
    {
        context.result = MatchResult::TooManyGenericParameters;
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

        context.genericReplaceTypes[genericParameters[i]->name] = genType->typeInfo;
        context.genericParametersCallTypes[i]                   = genType->typeInfo;
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
    if (context.cptResolved < firstDefault && parameters.size())
    {
        auto back = parameters.back()->typeInfo;
        if (back->kind != TypeInfoKind::Variadic && back->kind != TypeInfoKind::TypedVariadic)
        {
            context.result = MatchResult::NotEnoughParameters;
            return;
        }

        if (parameters.size() > 1 && context.cptResolved < min(parameters.size() - 1, firstDefault))
        {
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
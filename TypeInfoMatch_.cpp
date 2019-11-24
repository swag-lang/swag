#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"

static void matchParameters(SymbolMatchContext& context, vector<TypeInfoParam*>& parameters)
{
    // One boolean per used parameter
    context.doneParameters.clear();
    context.mapGenericTypes.clear();
    context.doneParameters.resize(parameters.size(), false);
    context.solvedParameters.resize(parameters.size(), nullptr);
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
            context.badSignatureParameterIdx = i;
            context.result                   = MatchResult::TooManyParameters;
            return;
        }

        auto symbolParameter = isAfterVariadic ? parameters.back() : parameters[i];
        auto symbolTypeInfo  = symbolParameter->typeInfo;

        if (symbolTypeInfo->kind == TypeInfoKind::Variadic)
        {
            context.cptResolved = (int) context.parameters.size();
            return;
        }

        auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);

        if (symbolTypeInfo->kind == TypeInfoKind::TypedVariadic)
        {
            if (typeInfo->kind != TypeInfoKind::TypedVariadic)
                symbolTypeInfo = ((TypeInfoVariadic*) symbolTypeInfo)->rawType;
            isAfterVariadic = true;
        }

        uint32_t castFlags = CASTFLAG_NO_ERROR;
        if (context.flags & SymbolMatchContext::MATCH_UNCONST)
            castFlags |= CASTFLAG_UNCONST;

        bool same = TypeManager::makeCompatibles(nullptr, symbolTypeInfo, typeInfo, nullptr, nullptr, castFlags);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolTypeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadSignature;
        }
        else
        {
            if (context.cptResolved < context.doneParameters.size())
                context.doneParameters[context.cptResolved] = true;

            // This is a generic type match
            if (symbolTypeInfo->flags & TYPEINFO_GENERIC)
            {
                // Do we already have mapped the generic parameter to something ?
                auto it = context.mapGenericTypes.find(symbolTypeInfo);
                if (it != context.mapGenericTypes.end())
                {
                    // Yes, and the map is not the same, then this is an error
                    same = TypeManager::makeCompatibles(nullptr, it->second.toType, typeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK);
                    if (!same)
                    {
                        context.badSignatureParameterIdx  = i;
                        context.badSignatureRequestedType = it->second.toType;
                        context.badSignatureGivenType     = typeInfo;
                        context.result                    = MatchResult::BadSignature;
                    }
                    else
                    {
                        context.maxGenericParam = i;
                        it->second.parameterIndex.push_back(i);
                    }
                }
                else
                {
                    context.maxGenericParam = i;

                    // Associate the generic type with the mapped one, and remember the parameter
                    // index
                    SymbolMatchContext::MapGenType mapGenType;
                    mapGenType.toType = typeInfo;
                    mapGenType.parameterIndex.push_back(i);
                    context.mapGenericTypes[symbolTypeInfo] = mapGenType;

                    // Need to register inside types when the generic type is a compound
                    vector<TypeInfo*> symbolTypeInfos{symbolTypeInfo};
                    vector<TypeInfo*> typeInfos{typeInfo};
                    while (symbolTypeInfos.size())
                    {
                        symbolTypeInfo = symbolTypeInfos.back();
                        typeInfo       = typeInfos.back();
                        symbolTypeInfos.pop_back();
                        typeInfos.pop_back();

                        it = context.mapGenericTypes.find(symbolTypeInfo);
                        if (it == context.mapGenericTypes.end())
                        {
                            mapGenType.toType = typeInfo;
                            mapGenType.parameterIndex.clear();
                            context.mapGenericTypes[symbolTypeInfo] = mapGenType;
                        }

                        switch (symbolTypeInfo->kind)
                        {
                        case TypeInfoKind::Struct:
                        {
                            auto symbolStruct = CastTypeInfo<TypeInfoStruct>(symbolTypeInfo, TypeInfoKind::Struct);
                            auto typeStruct   = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
                            auto num          = symbolStruct->genericParameters.size();
                            for (int idx = 0; idx < num; idx++)
                            {
                                auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                                auto rawTypeInfo = typeStruct->genericParameters[idx]->typeInfo;
                                symbolTypeInfos.push_back(genTypeInfo);
                                typeInfos.push_back(rawTypeInfo);
                            }
                            break;
                        }

                        case TypeInfoKind::Pointer:
                        {
                            auto symbolPtr = CastTypeInfo<TypeInfoPointer>(symbolTypeInfo, TypeInfoKind::Pointer);
                            if (typeInfo->kind == TypeInfoKind::Pointer)
                            {
                                auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
                                symbolTypeInfos.push_back(symbolPtr->pointedType);
                                typeInfos.push_back(typePtr->pointedType);
                            }
                            else if (typeInfo->kind == TypeInfoKind::Struct)
                            {
                                symbolTypeInfos.push_back(symbolPtr->pointedType);
                                typeInfos.push_back(typeInfo);
                            }
                            break;
                        }

                        case TypeInfoKind::Array:
                        {
                            auto symbolArray = CastTypeInfo<TypeInfoArray>(symbolTypeInfo, TypeInfoKind::Array);
                            auto typeArray   = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
                            symbolTypeInfos.push_back(symbolArray->finalType);
                            typeInfos.push_back(typeArray->finalType);
                            break;
                        }

                        case TypeInfoKind::Slice:
                        {
                            auto symbolSlice = CastTypeInfo<TypeInfoSlice>(symbolTypeInfo, TypeInfoKind::Slice);
                            auto typeSlice   = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
                            symbolTypeInfos.push_back(symbolSlice->pointedType);
                            typeInfos.push_back(typeSlice->pointedType);
                            break;
                        }

                        case TypeInfoKind::Lambda:
                        {
                            auto symbolLambda = CastTypeInfo<TypeInfoFuncAttr>(symbolTypeInfo, TypeInfoKind::Lambda);
                            auto typeLambda   = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::Lambda);
                            if (symbolLambda->returnType && (symbolLambda->returnType->flags & TYPEINFO_GENERIC))
                            {
                                symbolTypeInfos.push_back(symbolLambda->returnType);
                                typeInfos.push_back(typeLambda->returnType);
                            }

                            auto num = symbolLambda->parameters.size();
                            for (int idx = 0; idx < num; idx++)
                            {
                                auto symbolParam = CastTypeInfo<TypeInfoParam>(symbolLambda->parameters[idx], TypeInfoKind::Param);
                                auto typeParam   = CastTypeInfo<TypeInfoParam>(typeLambda->parameters[idx], TypeInfoKind::Param);
                                if (symbolParam->typeInfo->flags & TYPEINFO_GENERIC)
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
        }

        if (context.cptResolved < context.solvedParameters.size())
            context.solvedParameters[context.cptResolved] = symbolParameter;

        if (param)
        {
            param->resolvedParameter = symbolParameter;
            param->index             = context.cptResolved;
        }

        context.cptResolved++;
    }
}

static void matchNamedParameters(SymbolMatchContext& context, vector<TypeInfoParam*>& parameters)
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
        if (param->namedParam.empty())
        {
            context.badSignatureParameterIdx = i;
            context.result                   = MissingNamedParameter;
            return;
        }

        for (int j = startResolved; j < parameters.size(); j++)
        {
            auto symbolParameter = parameters[j];
            if (parameters[j]->namedParam == param->namedParam)
            {
                if (context.doneParameters[j])
                {
                    context.badSignatureParameterIdx = i;
                    context.result                   = DuplicatedNamedParameter;
                    return;
                }

                auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);
                bool same     = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR);
                if (!same)
                {
                    context.badSignatureParameterIdx  = i;
                    context.badSignatureRequestedType = symbolParameter->typeInfo;
                    context.badSignatureGivenType     = typeInfo;
                    context.result                    = MatchResult::BadSignature;
                }

                context.solvedParameters[j] = symbolParameter;
                param->resolvedParameter    = symbolParameter;
                param->index                = j;
                context.doneParameters[j]   = true;
                context.cptResolved++;
                break;
            }
        }

        if (!param->resolvedParameter)
        {
            context.badSignatureParameterIdx = i;
            context.result                   = InvalidNamedParameter;
            return;
        }
    }
}

static void matchGenericParameters(SymbolMatchContext& context, TypeInfo* myTypeInfo, vector<TypeInfoParam*>& genericParameters)
{
    // Solve generic parameters
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();

    context.genericParametersCallValues.resize(wantedNumGenericParams);
    context.genericParametersCallTypes.resize(wantedNumGenericParams);
    context.genericParametersGenTypes.resize(wantedNumGenericParams);

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

    // It's valid to not specify generic parameters. They will be deduced
    if (myTypeInfo->kind == TypeInfoKind::Struct)
    {
        if (numGenericParams < wantedNumGenericParams)
        {
            // A reference to a generic without specifying the generic parameters is a match
            // (we deduce type)
            context.result = MatchResult::NotEnoughGenericParameters;
            if (!numGenericParams)
            {
                if ((myTypeInfo->flags & TYPEINFO_GENERIC) || (context.flags & SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC))
                {
                    if (!(myTypeInfo->flags & TYPEINFO_GENERIC) || !context.parameters.size())
                    {
                        for (int i = 0; i < wantedNumGenericParams; i++)
                        {
                            auto symbolParameter                  = genericParameters[i];
                            auto genType                          = symbolParameter->typeInfo;
                            context.genericParametersCallTypes[i] = genType ? genType : symbolParameter->typeInfo;
                            context.genericParametersGenTypes[i]  = symbolParameter->typeInfo;
                        }

                        context.flags |= SymbolMatchContext::MATCH_WAS_PARTIAL;
                        context.result = MatchResult::Ok;
                    }
                }
            }

            return;
        }
    }
    else
    {
        // Deduce type of generic parameters from actual parameters
        if (!numGenericParams && wantedNumGenericParams)
        {
            for (int i = 0; i < wantedNumGenericParams; i++)
            {
                auto symbolParameter = genericParameters[i];
                auto it              = context.mapGenericTypes.find(symbolParameter->typeInfo);
                if (it == context.mapGenericTypes.end())
                {
                    if (myTypeInfo->flags & TYPEINFO_GENERIC)
                    {
                        context.result = MatchResult::NotEnoughGenericParameters;
                        return;
                    }

                    auto typeParam                        = CastTypeInfo<TypeInfoParam>(genericParameters[i], TypeInfoKind::Param);
                    context.genericParametersCallTypes[i] = typeParam->typeInfo;
                    context.genericParametersGenTypes[i]  = symbolParameter->typeInfo;
                }
                else
                {
                    context.genericParametersCallTypes[i] = it->second.toType;
                    context.genericParametersGenTypes[i]  = symbolParameter->typeInfo;
                }
            }
        }
        else if (context.mapGenericTypes.size())
        {
            if (context.maxGenericParam + 1 > context.genericParametersCallValues.size())
            {
                context.genericParametersCallValues.resize(context.maxGenericParam + 1);
                context.genericParametersCallTypes.resize(context.maxGenericParam + 1);
                context.genericParametersGenTypes.resize(context.maxGenericParam + 1);
            }

            for (auto it : context.mapGenericTypes)
            {
                for (auto paramIdx : it.second.parameterIndex)
                {
                    context.genericParametersCallTypes[paramIdx] = it.second.toType;
                    context.genericParametersGenTypes[paramIdx]  = it.first;
                }
            }
        }
    }

    for (int i = 0; i < numGenericParams; i++)
    {
        auto callParameter   = context.genericParameters[i];
        auto symbolParameter = genericParameters[i];
        auto typeInfo        = TypeManager::concreteType(callParameter->typeInfo, CONCRETE_FUNC);

        if (myTypeInfo->flags & TYPEINFO_GENERIC)
        {
            auto firstChild = callParameter->childs.empty() ? nullptr : callParameter->childs.front();
            if (firstChild)
            {
                if ((symbolParameter->typeInfo->kind == TypeInfoKind::Generic) && (firstChild->kind == AstNodeKind::Literal))
                {
                    context.badSignatureParameterIdx  = i;
                    context.badSignatureRequestedType = symbolParameter->typeInfo;
                    context.badSignatureGivenType     = typeInfo;
                    context.result                    = MatchResult::BadGenericSignature;
                    context.flags |= SymbolMatchContext::MATCH_ERROR_VALUE_TYPE;
                    continue;
                }

                if ((symbolParameter->typeInfo->kind != TypeInfoKind::Generic) && (firstChild->kind != AstNodeKind::Literal))
                {
                    context.badSignatureParameterIdx  = i;
                    context.badSignatureRequestedType = symbolParameter->typeInfo;
                    context.badSignatureGivenType     = typeInfo;
                    context.result                    = MatchResult::BadGenericSignature;
                    context.flags |= SymbolMatchContext::MATCH_ERROR_TYPE_VALUE;
                    continue;
                }
            }
        }

        bool same = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadGenericSignature;
        }
        else if ((myTypeInfo->flags & TYPEINFO_GENERIC) || (symbolParameter->value == callParameter->computedValue))
        {
            // We already have a match, and they do not match with that type, error
            auto it = context.mapGenericTypes.find(symbolParameter->typeInfo);
            if (it != context.mapGenericTypes.end())
            {
                same = TypeManager::makeCompatibles(nullptr, typeInfo, it->second.toType, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK);
                if (same)
                {
                    context.genericParametersCallValues[i] = callParameter->computedValue;
                    context.genericParametersCallTypes[i]  = callParameter->typeInfo;
                    context.genericParametersGenTypes[i]   = symbolParameter->typeInfo;
                }
                else
                {
                    context.badSignatureParameterIdx  = it->second.parameterIndex.front();
                    context.badSignatureRequestedType = typeInfo;
                    context.badSignatureGivenType     = it->second.toType;
                    context.result                    = MatchResult::BadSignature;
                }
            }
            else
            {
                context.genericParametersCallValues[i] = callParameter->computedValue;
                context.genericParametersCallTypes[i]  = callParameter->typeInfo;
                context.genericParametersGenTypes[i]   = symbolParameter->typeInfo;
            }
        }
        else
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadGenericSignature;
        }
    }
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    // For a lambda
    if (context.flags & SymbolMatchContext::MATCH_FOR_LAMBDA)
        return;

    matchParameters(context, parameters);
    if (context.result != MatchResult::Ok)
        return;
    matchNamedParameters(context, parameters);
    if (context.result != MatchResult::Ok)
        return;

    // Not enough parameters
    int firstDefault = firstDefaultValueIdx;
    if (firstDefault == -1)
        firstDefault = (int) parameters.size();
    if (context.cptResolved < firstDefault)
    {
        context.result = MatchResult::NotEnoughParameters;
        return;
    }

    matchGenericParameters(context, this, genericParameters);
}

void TypeInfoStruct::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;
    matchParameters(context, childs);
    if (context.result != MatchResult::Ok)
        return;
    matchNamedParameters(context, childs);
    if (context.result != MatchResult::Ok)
        return;
    matchGenericParameters(context, this, genericParameters);
}

TypeInfoParam* TypeInfoStruct::findChildByNameNoLock(const Utf8& childName)
{
    for (auto child : childs)
    {
        if (child->namedParam == childName)
            return child;
    }

    return nullptr;
}

TypeInfoParam* TypeInfoStruct::hasInterface(TypeInfoStruct* itf)
{
    scoped_lock lk(mutex);
    for (auto child : interfaces)
    {
        if (child->typeInfo->isSame(itf, ISSAME_CAST))
            return child;
    }

    return nullptr;
}
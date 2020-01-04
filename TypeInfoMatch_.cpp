#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"

static void matchParameters(SymbolMatchContext& context, vector<TypeInfoParam*>& parameters)
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
                // Need to register inside types when the generic type is a compound
                vector<TypeInfo*> symbolTypeInfos{symbolTypeInfo};
                vector<TypeInfo*> typeInfos{typeInfo};
                while (symbolTypeInfos.size())
                {
                    symbolTypeInfo = symbolTypeInfos.back();
                    typeInfo       = typeInfos.back();
                    symbolTypeInfos.pop_back();
                    typeInfos.pop_back();

                    // Do we already have mapped the generic parameter to something ?
                    auto it = context.genericReplaceTypes.find(symbolTypeInfo->name);
                    if (it != context.genericReplaceTypes.end())
                    {
                        // Yes, and the map is not the same, then this is an error
                        same = TypeManager::makeCompatibles(nullptr, it->second, typeInfo, nullptr, nullptr, CASTFLAG_NO_ERROR | CASTFLAG_JUST_CHECK);
                        if (!same)
                        {
                            context.badSignatureParameterIdx  = i;
                            context.badSignatureRequestedType = it->second;
                            context.badSignatureGivenType     = typeInfo;
                            context.result                    = MatchResult::BadSignature;
                        }
                    }
                    else
                    {
                        context.genericReplaceTypes[symbolTypeInfo->name] = typeInfo;

                        // If this is a valid generic argument, register it
                        auto itIdx = context.mapGenericTypesIndex.find(symbolTypeInfo->name);
                        if (itIdx != context.mapGenericTypesIndex.end())
                        {
                            context.genericParametersCallTypes[itIdx->second] = typeInfo;
                        }
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
            context.result                   = MatchResult::MissingNamedParameter;
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
                    context.result                   = MatchResult::DuplicatedNamedParameter;
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
            context.result                   = MatchResult::InvalidNamedParameter;
            return;
        }
    }
}

static void matchGenericParameters(SymbolMatchContext& context, TypeInfo* myTypeInfo, vector<TypeInfoParam*>& genericParameters)
{
    // Solve generic parameters
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();

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
        if (!numGenericParams && wantedNumGenericParams)
        {
            for (int i = 0; i < wantedNumGenericParams; i++)
            {
                auto symbolParameter = genericParameters[i];
                auto it              = context.genericReplaceTypes.find(symbolParameter->typeInfo->name);
                if (it == context.genericReplaceTypes.end())
                {
                    if (myTypeInfo->flags & TYPEINFO_GENERIC)
                    {
                        context.result = MatchResult::NotEnoughGenericParameters;
                        return;
                    }
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
            auto it = context.genericReplaceTypes.find(symbolParameter->typeInfo->name);
            if (it == context.genericReplaceTypes.end())
            {
                context.genericParametersCallTypes[i] = callParameter->typeInfo;
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

static void fillUserGenericParams(SymbolMatchContext& context, vector<TypeInfoParam*>& genericParameters)
{
    context.mapGenericTypesIndex.clear();
    context.genericReplaceTypes.clear();

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

    // For a lambda
    if (context.flags & SymbolMatchContext::MATCH_FOR_LAMBDA)
        return;

    fillUserGenericParams(context, genericParameters);
    if (context.result != MatchResult::Ok)
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

    fillUserGenericParams(context, genericParameters);
    if (context.result != MatchResult::Ok)
        return;

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
    shared_lock lk(mutex);
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
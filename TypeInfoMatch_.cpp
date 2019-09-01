#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"

static void matchParameters(SymbolMatchContext& context, vector<TypeInfoParam*>& parameters)
{
    // One boolean per used parameter
    context.doneParameters.clear();
    context.mapGenericTypes.clear();
    context.doneParameters.resize(parameters.size(), false);
    context.resetTmp();

    // Solve unnamed parameters
    int numParams = (int) context.parameters.size();
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

        if (i >= parameters.size())
        {
            context.badSignatureParameterIdx = i;
            context.result                   = MatchResult::TooManyParameters;
            return;
        }

        auto symbolParameter = parameters[i];
		if (symbolParameter->typeInfo == g_TypeMgr.typeInfoVariadic)
		{
			context.cptResolved = (int) context.parameters.size();
			return;
		}

        auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
        bool same     = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadSignature;
        }

        context.doneParameters[context.cptResolved] = true;

        // This is a generic type match
        if (symbolParameter->typeInfo->flags & TYPEINFO_GENERIC)
        {
            auto it = context.mapGenericTypes.find(symbolParameter->typeInfo);
            if (it != context.mapGenericTypes.end() && !it->second.first->isSame(typeInfo, ISSAME_CAST))
            {
                context.badSignatureParameterIdx  = i;
                context.badSignatureRequestedType = it->second.first;
                context.badSignatureGivenType     = typeInfo;
                context.result                    = MatchResult::BadSignature;
            }
            else
            {
                context.maxGenericParam                            = i;
                context.mapGenericTypes[symbolParameter->typeInfo] = {typeInfo, i};
            }
        }

        if (param)
        {
            param->resolvedParameter = symbolParameter;
            param->index             = context.cptResolved;
        }

        context.cptResolved++;
    }
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    // For a lambda
    if (context.flags & SymbolMatchContext::MATCH_FOR_LAMBDA)
        return;

    matchParameters(context, parameters);

    // Named parameters
    if (context.hasNamedParameters)
    {
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
                // After the last named parameters, we must have the first default value, or nothing
                if (i < firstDefaultValueIdx)
                {
                    context.badSignatureParameterIdx = i;
                    context.result                   = MissingNamedParameter;
                    return;
                }

                break;
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

                    auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
                    bool same     = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
                    if (!same)
                    {
                        context.badSignatureParameterIdx  = i;
                        context.badSignatureRequestedType = symbolParameter->typeInfo;
                        context.badSignatureGivenType     = typeInfo;
                        context.result                    = MatchResult::BadSignature;
                    }

                    param->resolvedParameter  = symbolParameter;
                    param->index              = j;
                    context.doneParameters[j] = true;
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

    // Not enough parameters
    int firstDefault = firstDefaultValueIdx;
    if (firstDefault == -1)
        firstDefault = (int) parameters.size();
    if (context.cptResolved < firstDefault)
        context.result = MatchResult::NotEnoughParameters;

    // Solve generic parameters
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();

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

    context.genericParametersCallValues.resize(wantedNumGenericParams);
    context.genericParametersCallTypes.resize(wantedNumGenericParams);
    context.genericParametersGenTypes.resize(wantedNumGenericParams);

    // Deduce type of generic parameters from actual parameters
    if (!numGenericParams && wantedNumGenericParams)
    {
        for (int i = 0; i < wantedNumGenericParams; i++)
        {
            auto symbolParameter = genericParameters[i];
            auto it              = context.mapGenericTypes.find(symbolParameter->typeInfo);
            if (it == context.mapGenericTypes.end())
            {
                if (flags & TYPEINFO_GENERIC)
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
                context.genericParametersCallTypes[i] = it->second.first;
                context.genericParametersGenTypes[i]  = symbolParameter->typeInfo;
            }
        }
    }
    else if (context.mapGenericTypes.size())
    {
        context.genericParametersCallValues.resize(context.maxGenericParam + 1);
        context.genericParametersCallTypes.resize(context.maxGenericParam + 1);
        context.genericParametersGenTypes.resize(context.maxGenericParam + 1);
        for (auto it : context.mapGenericTypes)
        {
            context.genericParametersCallTypes[it.second.second] = it.second.first;
            context.genericParametersGenTypes[it.second.second]  = it.first;
        }
    }

    for (int i = 0; i < numGenericParams; i++)
    {
        auto callParameter   = context.genericParameters[i];
        auto symbolParameter = genericParameters[i];
        auto typeInfo        = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
        bool same            = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadGenericSignature;
        }
        else if ((flags & TYPEINFO_GENERIC) || (symbolParameter->genericValue == callParameter->computedValue))
        {
            // We already have a match, and they do not match with that type, error
            auto it = context.mapGenericTypes.find(symbolParameter->typeInfo);
            if (it != context.mapGenericTypes.end() && !it->second.first->isSame(typeInfo, ISSAME_CAST))
            {
                context.badSignatureParameterIdx  = it->second.second;
                context.badSignatureRequestedType = typeInfo;
                context.badSignatureGivenType     = it->second.first;
                context.result                    = MatchResult::BadSignature;
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

void TypeInfoStruct::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    matchParameters(context, childs);
    if (context.result != MatchResult::Ok)
        return;

    // Named parameters
    if (context.hasNamedParameters)
    {
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

            for (int j = startResolved; j < childs.size(); j++)
            {
                auto symbolParameter = childs[j];
                if (childs[j]->namedParam == param->namedParam)
                {
                    if (context.doneParameters[j])
                    {
                        context.badSignatureParameterIdx = i;
                        context.result                   = DuplicatedNamedParameter;
                        return;
                    }

                    auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
                    bool same     = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
                    if (!same)
                    {
                        context.badSignatureParameterIdx  = i;
                        context.badSignatureRequestedType = symbolParameter->typeInfo;
                        context.badSignatureGivenType     = typeInfo;
                        context.result                    = MatchResult::BadSignature;
                    }

                    param->resolvedParameter  = symbolParameter;
                    param->index              = j;
                    context.doneParameters[j] = true;
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

    // Solve generic parameters
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();

    context.genericParametersCallValues.resize(wantedNumGenericParams);
    context.genericParametersCallTypes.resize(wantedNumGenericParams);
    context.genericParametersGenTypes.resize(wantedNumGenericParams);

    // It's valid to not specify generic parameters. They will be deduced
    if (numGenericParams < wantedNumGenericParams)
    {
        // A reference to a generic without specifying the generic parameters is a match
        // (we deduce type)
        context.result = MatchResult::NotEnoughGenericParameters;
        if (!numGenericParams)
        {
            if ((flags & TYPEINFO_GENERIC) || (context.flags & SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC))
            {
                if (!(flags & TYPEINFO_GENERIC) || !context.parameters.size())
                {
                    for (int i = 0; i < wantedNumGenericParams; i++)
                    {
                        auto symbolParameter                  = genericParameters[i];
                        context.genericParametersCallTypes[i] = symbolParameter->typeInfo;
                        context.genericParametersGenTypes[i]  = symbolParameter->typeInfo;
                    }

                    context.flags |= SymbolMatchContext::MATCH_WAS_PARTIAL;
                    context.result = MatchResult::Ok;
                }
            }
        }

        return;
    }

    if (numGenericParams > wantedNumGenericParams)
    {
        context.result = MatchResult::TooManyGenericParameters;
        return;
    }

    for (int i = 0; i < numGenericParams; i++)
    {
        auto callParameter   = context.genericParameters[i];
        auto symbolParameter = genericParameters[i];
        auto typeInfo        = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
        bool same            = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadGenericSignature;
        }
        else if ((flags & TYPEINFO_GENERIC) || (symbolParameter->genericValue == callParameter->computedValue))
        {
            // We already have a match, and they do not match with that type, error
            auto it = context.mapGenericTypes.find(symbolParameter->typeInfo);
            if (it != context.mapGenericTypes.end() && !it->second.first->isSame(typeInfo, ISSAME_CAST))
            {
                context.badSignatureParameterIdx  = it->second.second;
                context.badSignatureRequestedType = typeInfo;
                context.badSignatureGivenType     = it->second.first;
                context.result                    = MatchResult::BadSignature;
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

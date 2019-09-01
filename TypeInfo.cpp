#include "pch.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "Ast.h"

Pool<TypeInfoFuncAttr>  g_Pool_typeInfoFuncAttr;
Pool<TypeInfoNamespace> g_Pool_typeInfoNamespace;
Pool<TypeInfoEnum>      g_Pool_typeInfoEnum;
Pool<TypeInfoParam>     g_Pool_typeInfoParam;
Pool<TypeInfoPointer>   g_Pool_typeInfoPointer;
Pool<TypeInfoArray>     g_Pool_typeInfoArray;
Pool<TypeInfoSlice>     g_Pool_typeInfoSlice;
Pool<TypeInfoList>      g_Pool_typeInfoList;
Pool<TypeInfoNative>    g_Pool_typeInfoNative;
Pool<TypeInfoVariadic>  g_Pool_typeInfoVariadic;
Pool<TypeInfoGeneric>   g_Pool_typeInfoGeneric;
Pool<TypeInfoStruct>    g_Pool_typeInfoStruct;
Pool<TypeInfoAlias>     g_Pool_typeInfoAlias;

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other, uint32_t isSameFlags)
{
    if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
        return false;
    if (parameters.size() != other->parameters.size())
        return false;
    if (stackSize != other->stackSize)
        return false;

    for (int i = 0; i < genericParameters.size(); i++)
    {
        if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
            return false;
        if (!(genericParameters[i]->genericValue == other->genericParameters[i]->genericValue))
            return false;
    }

    for (int i = 0; i < parameters.size(); i++)
    {
        if (!parameters[i]->typeInfo->isSame(other->parameters[i]->typeInfo, isSameFlags))
            return false;
    }
    return true;
}

bool TypeInfoFuncAttr::isSame(TypeInfo* from, uint32_t isSameFlags)
{
    if (this == from)
        return true;
    if (!TypeInfo::isSame(from, isSameFlags))
        return false;

    auto fromFunc = static_cast<TypeInfoFuncAttr*>(from);
    SWAG_ASSERT(from->kind == TypeInfoKind::FuncAttr || from->kind == TypeInfoKind::Lambda);
    if (!isSame(fromFunc, isSameFlags))
        return false;

    if (isSameFlags & ISSAME_EXACT)
    {
        if (returnType && returnType != g_TypeMgr.typeInfoVoid && !fromFunc->returnType)
            return false;
        if (!returnType && fromFunc->returnType && fromFunc->returnType != g_TypeMgr.typeInfoVoid)
            return false;
        if (returnType && fromFunc->returnType && !returnType->isSame(fromFunc->returnType, isSameFlags))
            return false;
    }

    return true;
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;

    // For a lambda
    if (context.flags & SymbolMatchContext::MATCH_FOR_LAMBDA)
        return;

    // One boolean per used parameter
    context.doneParameters.clear();
    context.mapGenericTypes.clear();
    context.doneParameters.resize(parameters.size(), false);

    // Solve unnamed parameters
    int numParams = (int) context.parameters.size();

    int  maxGenericParam    = 0;
    int  cptResolved        = 0;
    bool hasNamedParameters = false;
    for (int i = 0; i < numParams; i++)
    {
        auto callParameter = context.parameters[i];

        AstFuncCallParam* param = nullptr;
        if (callParameter->kind == AstNodeKind::FuncCallParam)
        {
            param = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);
            if (!param->namedParam.empty())
            {
                hasNamedParameters = true;
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
            return;

        auto typeInfo = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
        bool same     = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadSignature;
        }

        context.doneParameters[cptResolved] = true;

        // This is a generic type match
        if (symbolParameter->typeInfo->flags & TYPEINFO_GENERIC)
        {
            auto it = context.mapGenericTypes.find(symbolParameter->typeInfo);
            if (it != context.mapGenericTypes.end() && !it->second.first->isSame(typeInfo, ISSAME_FORCAST))
            {
                context.badSignatureParameterIdx  = i;
                context.badSignatureRequestedType = it->second.first;
                context.badSignatureGivenType     = typeInfo;
                context.result                    = MatchResult::BadSignature;
            }
            else
            {
                maxGenericParam                                    = i;
                context.mapGenericTypes[symbolParameter->typeInfo] = {typeInfo, i};
            }
        }

        if (param)
        {
            param->resolvedParameter = symbolParameter;
            param->index             = cptResolved;
        }

        cptResolved++;
    }

    // Named parameters
    if (hasNamedParameters)
    {
        auto callParameter = context.parameters[0];
        callParameter->parent->flags |= AST_MUST_SORT_CHILDS;

        auto startResolved = cptResolved;
        for (int i = startResolved; i < numParams; i++)
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
                    cptResolved++;
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
    if (cptResolved < firstDefault)
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
        context.genericParametersCallValues.resize(maxGenericParam + 1);
        context.genericParametersCallTypes.resize(maxGenericParam + 1);
        context.genericParametersGenTypes.resize(maxGenericParam + 1);
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
            if (it != context.mapGenericTypes.end() && !it->second.first->isSame(typeInfo, ISSAME_FORCAST))
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

TypeInfo* TypeInfoNative::clone()
{
    auto newType = g_Pool_typeInfoNative.alloc();
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoNamespace::clone()
{
    auto newType   = g_Pool_typeInfoNamespace.alloc();
    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoEnum::clone()
{
    auto newType     = g_Pool_typeInfoEnum.alloc();
    newType->scope   = scope;
    newType->rawType = rawType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoAlias::clone()
{
    auto newType     = g_Pool_typeInfoAlias.alloc();
    newType->rawType = rawType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoParam::clone()
{
    auto newType        = g_Pool_typeInfoParam.alloc();
    newType->namedParam = namedParam;
    newType->typeInfo   = typeInfo;
    newType->index      = index;
    newType->offset     = offset;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoFuncAttr::clone()
{
    auto newType                  = g_Pool_typeInfoFuncAttr.alloc();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->returnType           = returnType;
    newType->stackSize            = stackSize;

    for (int i = 0; i < genericParameters.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(genericParameters[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->genericParameters.push_back(param);
    }

    for (int i = 0; i < parameters.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(parameters[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->parameters.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

void TypeInfoFuncAttr::computeName()
{
    name.clear();

    if (genericParameters.size())
    {
        name += "!(";
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                name += ", ";
            name += genericParameters[i]->typeInfo ? genericParameters[i]->typeInfo->name : genericParameters[i]->name;
        }

        name += ")";
    }

    name += format("(");
    for (int i = 0; i < parameters.size(); i++)
    {
        if (i)
            name += ", ";
        name += parameters[i]->typeInfo->name;
    }

    name += ")";
    if (returnType)
        name += format("->%s", returnType->name.c_str());
    else
        name += "->void";
}

TypeInfo* TypeInfoPointer::clone()
{
    auto newType         = g_Pool_typeInfoPointer.alloc();
    newType->pointedType = pointedType;
    newType->ptrCount    = ptrCount;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoArray::clone()
{
    auto newType         = g_Pool_typeInfoArray.alloc();
    newType->pointedType = pointedType;
    newType->rawType     = rawType;
    newType->count       = count;
    newType->totalCount  = totalCount;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoSlice::clone()
{
    auto newType         = g_Pool_typeInfoSlice.alloc();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoList::clone()
{
    auto newType      = g_Pool_typeInfoList.alloc();
    newType->childs   = childs;
    newType->names    = names;
    newType->scope    = scope;
    newType->listKind = listKind;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoVariadic::clone()
{
    auto newType = g_Pool_typeInfoVariadic.alloc();
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoGeneric::clone()
{
    auto newType = g_Pool_typeInfoGeneric.alloc();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

TypeInfo* TypeInfoStruct::clone()
{
    auto newType        = g_Pool_typeInfoStruct.alloc();
    newType->scope      = scope;
    newType->structNode = structNode;
    newType->opInitFct  = opInitFct;

    for (int i = 0; i < genericParameters.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(genericParameters[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->genericParameters.push_back(param);
    }

    for (int i = 0; i < childs.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(childs[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->childs.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

bool TypeInfoStruct::isSame(TypeInfo* from, uint32_t isSameFlags)
{
    if (this == from)
        return true;
    if (!TypeInfo::isSame(from, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoStruct*>(from);
    if (genericParameters.size() != other->genericParameters.size())
        return false;
    for (int i = 0; i < genericParameters.size(); i++)
    {
        if (isSameFlags & ISSAME_FORCAST)
        {
            if (other->genericParameters[i]->typeInfo->kind == TypeInfoKind::Generic)
                continue;
        }

        if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
            return false;
    }

    if (!(isSameFlags & ISSAME_FORCAST))
    {
        if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
            return false;
        if (scope != other->scope)
            return false;
        if (childs.size() != other->childs.size())
            return false;
        for (int i = 0; i < childs.size(); i++)
        {
            if (!childs[i]->isSame(other->childs[i], isSameFlags))
                return false;
        }
    }

    return true;
}

void TypeInfoStruct::match(SymbolMatchContext& context)
{
    context.result = MatchResult::Ok;
    context.mapGenericTypes.clear();

    // One boolean per used parameter
    context.doneParameters.clear();
    context.mapGenericTypes.clear();
    context.doneParameters.resize(childs.size(), false);

    // Solve unnamed parameters
    int numParams = (int) context.parameters.size();

    int  maxGenericParam    = 0;
    int  cptResolved        = 0;
    bool hasNamedParameters = false;
    for (int i = 0; i < numParams; i++)
    {
        auto callParameter = context.parameters[i];

        AstFuncCallParam* param = nullptr;
        if (callParameter->kind == AstNodeKind::FuncCallParam)
        {
            param = CastAst<AstFuncCallParam>(callParameter, AstNodeKind::FuncCallParam);
            if (!param->namedParam.empty())
            {
                hasNamedParameters = true;
                break;
            }
        }

        if (i >= childs.size())
        {
            context.badSignatureParameterIdx = i;
            context.result                   = MatchResult::TooManyParameters;
            return;
        }

        auto symbolParameter = childs[i];
        auto typeInfo        = TypeManager::concreteType(callParameter->typeInfo, MakeConcrete::FlagFunc);
        bool same            = TypeManager::makeCompatibles(nullptr, symbolParameter->typeInfo, typeInfo, nullptr, CASTFLAG_NOERROR);
        if (!same)
        {
            context.badSignatureParameterIdx  = i;
            context.badSignatureRequestedType = symbolParameter->typeInfo;
            context.badSignatureGivenType     = typeInfo;
            context.result                    = MatchResult::BadSignature;
        }

        context.doneParameters[cptResolved] = true;

        // This is a generic type match
        if (symbolParameter->typeInfo->flags & TYPEINFO_GENERIC)
        {
            auto it = context.mapGenericTypes.find(symbolParameter->typeInfo);
            if (it != context.mapGenericTypes.end() && !it->second.first->isSame(typeInfo, ISSAME_FORCAST))
            {
                context.badSignatureParameterIdx  = i;
                context.badSignatureRequestedType = it->second.first;
                context.badSignatureGivenType     = typeInfo;
                context.result                    = MatchResult::BadSignature;
            }
            else
            {
                maxGenericParam                                    = i;
                context.mapGenericTypes[symbolParameter->typeInfo] = {typeInfo, i};
            }
        }

        if (param)
        {
            param->resolvedParameter = symbolParameter;
            param->index             = cptResolved;
        }

        cptResolved++;
    }

    if (context.result != MatchResult::Ok)
        return;

    // Named parameters
    if (hasNamedParameters)
    {
        auto callParameter = context.parameters[0];
        callParameter->parent->flags |= AST_MUST_SORT_CHILDS;

        auto startResolved = cptResolved;
        for (int i = startResolved; i < numParams; i++)
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
                    cptResolved++;
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
            if (it != context.mapGenericTypes.end() && !it->second.first->isSame(typeInfo, ISSAME_FORCAST))
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

const char* TypeInfo::getArticleKindName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        return "a namespace";
    case TypeInfoKind::Enum:
        return "an enum";
    case TypeInfoKind::Array:
        return "an array";
    case TypeInfoKind::Pointer:
        return "a pointer";
    case TypeInfoKind::FuncAttr:
        return "a function";
    case TypeInfoKind::TypeList:
        return "a tuple";
    case TypeInfoKind::Variadic:
        return "a variadic";
    case TypeInfoKind::Struct:
        return "a struct";
    case TypeInfoKind::Generic:
        return "a generic type";
    case TypeInfoKind::Alias:
        return "an alias type";
    case TypeInfoKind::Native:
        return "a type";
    }

    return "<type>";
}

const char* TypeInfo::getNakedKindName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        return "namespace";
    case TypeInfoKind::Enum:
        return "enum";
    case TypeInfoKind::Array:
        return "array";
    case TypeInfoKind::Pointer:
        return "pointer";
    case TypeInfoKind::FuncAttr:
        return "function";
    case TypeInfoKind::TypeList:
        return "tuple";
    case TypeInfoKind::Variadic:
        return "variadic";
    case TypeInfoKind::Struct:
        return "struct";
    case TypeInfoKind::Generic:
        return "generic type";
    case TypeInfoKind::Alias:
        return "alias type";
    case TypeInfoKind::Native:
        return "type";
    }

    return "<type>";
}
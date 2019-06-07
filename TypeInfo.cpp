#include "pch.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "ThreadManager.h"
#include "Global.h"

Pool<TypeInfoFuncAttr>      g_Pool_typeInfoFuncAttr;
Pool<TypeInfoNamespace>     g_Pool_typeInfoNamespace;
Pool<TypeInfoEnum>          g_Pool_typeInfoEnum;
Pool<TypeInfoEnumValue>     g_Pool_typeInfoEnumValue;
Pool<TypeInfoFuncAttrParam> g_Pool_typeInfoFuncAttrParam;
Pool<SymbolMatchParameter>  g_Pool_symbolMatchParameter;

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other)
{
    if (parameters.size() != other->parameters.size())
        return false;
    for (int i = 0; i < parameters.size(); i++)
    {
        if (!parameters[i]->typeInfo->isSame(other->parameters[i]->typeInfo))
            return false;
    }

    return true;
}

bool TypeInfoFuncAttr::isSame(TypeInfo* from)
{
    if (kind != from->kind)
        return false;
    auto fromFunc = CastTypeInfo<TypeInfoFuncAttr>(from, TypeInfoKind::FuncAttr);
    return isSame(fromFunc);
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    int  firstUserNamed = 0;
    int  cptResolved    = 0;
    bool badSignature   = false;

    // First we solve unnamed parameters
    for (int i = 0; i < context.parameters.size(); i++)
    {
        auto param = context.parameters[i];
        if (!param->name.empty())
        {
            firstUserNamed = i;
            break;
        }

        if (i >= parameters.size())
        {
            context.result = MatchResult::TooManyParameters;
            return;
        }

        if (!parameters[i]->typeInfo->isSame(context.parameters[i]->typeInfo))
        {
            context.badSignatureParameterIdx  = i;
            context.basSignatureRequestedType = parameters[i]->typeInfo;
            context.basSignatureGivenType     = context.parameters[i]->typeInfo;
            badSignature                      = true;
        }

        cptResolved++;
    }

    // Named parameters

    // Not enough parameters
    int firstDefault = firstDefaultValueIdx;
    if (firstDefault == -1)
        firstDefault = (int) parameters.size();
    if (cptResolved < firstDefault)
    {
        context.result = MatchResult::NotEnoughParameters;
        return;
    }

    context.result = badSignature ? MatchResult::BadSignature : MatchResult::Ok;
}

const char* TypeInfo::getNakedName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Namespace:
        return "namespace";
    case TypeInfoKind::Enum:
        return "enum";
    case TypeInfoKind::EnumValue:
        return "enum value";
    }

    return "???";
}
#include "pch.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "ThreadManager.h"
#include "Global.h"
#include "TypeManager.h"
#include "Ast.h"

Pool<TypeInfoFuncAttr>      g_Pool_typeInfoFuncAttr;
Pool<TypeInfoNamespace>     g_Pool_typeInfoNamespace;
Pool<TypeInfoEnum>          g_Pool_typeInfoEnum;
Pool<TypeInfoEnumValue>     g_Pool_typeInfoEnumValue;
Pool<TypeInfoFuncAttrParam> g_Pool_typeInfoFuncAttrParam;
Pool<TypeInfoPointer>       g_Pool_typeInfoPointer;

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
    int numParams = (int) context.parameters.size();
    for (int i = 0; i < numParams; i++)
    {
        auto param = CastAst<AstFuncCallParam>(context.parameters[i], AstNodeKind::FuncCallParam);
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

        auto typeInfo = TypeManager::concreteType(context.parameters[i]->typeInfo);
        if (!parameters[i]->typeInfo->isSame(typeInfo))
        {
            context.badSignatureParameterIdx  = i;
            context.basSignatureRequestedType = parameters[i]->typeInfo;
            context.basSignatureGivenType     = context.parameters[i]->typeInfo;
            badSignature                      = true;
        }

		param->resolvedParameter = parameters[i];
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
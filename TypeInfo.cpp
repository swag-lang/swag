#include "pch.h"
#include "PoolFactory.h"

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
    auto fromFunc = CastTypeInfo<TypeInfoFuncAttr>(from, TypeInfoKind::FunctionAttribute);
    return isSame(fromFunc);
}

void TypeInfoFuncAttr::match(SymbolMatchContext& context)
{
    int firstUserNamed = 0;
    int cptResolved    = 0;

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
            context.result = MATCH_ERROR_TOO_MANY_PARAMETERS;
            return;
        }

        if (!parameters[i]->typeInfo->isSame(context.parameters[i]->typeInfo))
        {
            context.result = MATCH_ERROR_BAD_SIGNATURE;
            return;
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
        context.result = MATCH_ERROR_NOT_ENOUGH_PARAMETERS;
        return;
    }

    context.result = MATCH_OK;
}

#include "pch.h"
#include "TypeManager.h"

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
        auto param = static_cast<TypeInfoParam*>(genericParameters[i]->clone());
        newType->genericParameters.push_back(param);
    }

    for (int i = 0; i < parameters.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(parameters[i]->clone());
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

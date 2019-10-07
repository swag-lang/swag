#include "pch.h"
#include "TypeManager.h"

TypeInfo* TypeInfoNative::clone()
{
    auto newType = g_Pool_typeInfoNative.alloc();
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoNative::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (isSameFlags & ISSAME_CAST)
    {
        if (to->kind == TypeInfoKind::Generic)
            return true;
    }

    if (to->kind != TypeInfoKind::Native)
        return false;

    if ((isSameFlags & ISSAME_EXACT) && !(isSameFlags & ISSAME_CONCRETE))
    {
        if ((flags & TYPEINFO_UNTYPED_INTEGER) != (to->flags & TYPEINFO_UNTYPED_INTEGER))
            return false;
        if ((flags & TYPEINFO_UNTYPED_FLOAT) != (to->flags & TYPEINFO_UNTYPED_FLOAT))
            return false;
    }

    auto other = static_cast<TypeInfoNative*>(to);
    if (nativeType != other->nativeType)
        return false;

    return true;
}

TypeInfo* TypeInfoNamespace::clone()
{
    auto newType   = g_Pool_typeInfoNamespace.alloc();
    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoEnum::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoEnum*>(to);
    return rawType->isSame(other->rawType, isSameFlags);
}

TypeInfo* TypeInfoAlias::clone()
{
    auto newType     = g_Pool_typeInfoAlias.alloc();
    newType->rawType = rawType;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoAlias::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoAlias*>(to);
    return rawType->isSame(other->rawType, isSameFlags);
}

TypeInfo* TypeInfoParam::clone()
{
    auto newType        = g_Pool_typeInfoParam.alloc();
    newType->namedParam = namedParam;
    newType->typeInfo   = typeInfo;
    newType->index      = index;
    newType->offset     = offset;
    newType->attributes = attributes;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoParam::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoParam*>(to);
    return typeInfo->isSame(other->typeInfo, isSameFlags);
}

TypeInfo* TypeInfoPointer::clone()
{
    auto newType         = g_Pool_typeInfoPointer.alloc();
    newType->finalType   = finalType;
    newType->pointedType = pointedType;
    newType->ptrCount    = ptrCount;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoPointer::computePointedType()
{
    if (ptrCount == 1)
        return finalType;
    auto result = (TypeInfoPointer*) clone();
    result->ptrCount--;
	result->computeName();
    return result;
}

bool TypeInfoPointer::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    if (isSameFlags & ISSAME_CAST)
    {
        if (this == g_TypeMgr.typeInfoNull)
            return true;
        if (to == g_TypeMgr.typeInfoNull)
            return true;
    }

    auto other = static_cast<TypeInfoPointer*>(to);
    if (ptrCount != other->ptrCount)
        return false;

    if (isSameFlags & ISSAME_CAST)
    {
        if (other->finalType == g_TypeMgr.typeInfoVoid)
            return true;
    }

    return finalType->isSame(other->finalType, isSameFlags);
}

TypeInfo* TypeInfoArray::clone()
{
    auto newType         = g_Pool_typeInfoArray.alloc();
    newType->pointedType = pointedType;
    newType->finalType   = finalType;
    newType->count       = count;
    newType->totalCount  = totalCount;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoArray::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoArray*>(to);
    if (count != other->count)
        return false;
    return pointedType->isSame(other->pointedType, isSameFlags);
}

TypeInfo* TypeInfoSlice::clone()
{
    auto newType         = g_Pool_typeInfoSlice.alloc();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoSlice::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto castedFrom = static_cast<TypeInfoSlice*>(to);
    return pointedType->isSame(castedFrom->pointedType, isSameFlags);
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

bool TypeInfoList::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoList*>(to);
    if (childs.size() != other->childs.size())
        return false;
    if (listKind != other->listKind)
        return false;

    for (int i = 0; i < childs.size(); i++)
    {
        if (!childs[i]->isSame(other->childs[i], isSameFlags))
            return false;
    }

    if (names.size() != other->names.size())
        return false;
    for (int i = 0; i < names.size(); i++)
    {
        if (names[i] != other->names[i])
            return false;
    }

    if (isSameFlags & ISSAME_EXACT)
    {
        if (scope != other->scope)
            return false;
    }

    return true;
}

TypeInfo* TypeInfoVariadic::clone()
{
    auto newType = g_Pool_typeInfoVariadic.alloc();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

bool TypeInfoVariadic::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoVariadic*>(to);
    if (rawType && !other->rawType)
        return false;
    if (!rawType && other->rawType)
        return false;
    if (rawType == other->rawType)
        return true;
    return rawType->isSame(other->rawType, isSameFlags);
}

TypeInfo* TypeInfoGeneric::clone()
{
    auto newType = g_Pool_typeInfoGeneric.alloc();
    newType->copyFrom(this);
    newType->rawType = rawType;
    return newType;
}

bool TypeInfoGeneric::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (to->kind == kind)
        return name == to->name;
    if (isSameFlags & ISSAME_EXACT)
        return name == to->name;
    return true;
}

TypeInfo* TypeInfoFuncAttr::clone()
{
    auto newType                  = g_Pool_typeInfoFuncAttr.alloc();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->returnType           = returnType;
    newType->stackSize            = stackSize;
    newType->attributes           = attributes;

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

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other, uint32_t isSameFlags)
{
    if (parameters.size() != other->parameters.size())
        return false;
    if (stackSize != other->stackSize)
        return false;

    for (int i = 0; i < genericParameters.size(); i++)
    {
        if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
            return false;
        if (!(genericParameters[i]->value == other->genericParameters[i]->value))
            return false;
    }

    for (int i = 0; i < parameters.size(); i++)
    {
        if (!parameters[i]->typeInfo->isSame(other->parameters[i]->typeInfo, isSameFlags))
            return false;
    }
    return true;
}

bool TypeInfoFuncAttr::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoFuncAttr*>(to);
    SWAG_ASSERT(to->kind == TypeInfoKind::FuncAttr || to->kind == TypeInfoKind::Lambda);
    if (!isSame(other, isSameFlags))
        return false;

    if ((isSameFlags & ISSAME_EXACT) || (to->kind == TypeInfoKind::Lambda))
    {
        if (returnType && returnType != g_TypeMgr.typeInfoVoid && !other->returnType)
            return false;
        if (!returnType && other->returnType && other->returnType != g_TypeMgr.typeInfoVoid)
            return false;
        if (returnType && other->returnType && !returnType->isSame(other->returnType, isSameFlags))
            return false;
    }

    return true;
}

TypeInfo* TypeInfoEnum::clone()
{
    auto newType        = g_Pool_typeInfoEnum.alloc();
    newType->scope      = scope;
    newType->rawType    = rawType;
    newType->attributes = attributes;

    for (int i = 0; i < values.size(); i++)
    {
        auto param = static_cast<TypeInfoParam*>(values[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->values.push_back(param);
    }

    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoStruct::clone()
{
    auto newType               = g_Pool_typeInfoStruct.alloc();
    newType->scope             = scope;
    newType->structNode        = structNode;
    newType->opInitFct         = opInitFct;
    newType->opUserPostCopyFct = opUserPostCopyFct;
    newType->opPostCopy        = opPostCopy;
    newType->opUserPostMoveFct = opUserPostMoveFct;
    newType->opPostMove        = opPostMove;
    newType->opUserDropFct     = opUserDropFct;
    newType->opDrop            = opDrop;
    newType->attributes        = attributes;

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

bool TypeInfoStruct::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

	if (isSameFlags & ISSAME_CAST)
    {
        if (to->kind == TypeInfoKind::Generic)
            return true;
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    if ((flags & TYPEINFO_STRUCT_IS_TUPLE) != (to->flags & TYPEINFO_STRUCT_IS_TUPLE))
        return false;

    auto other = static_cast<TypeInfoStruct*>(to);

    if (genericParameters.size() != other->genericParameters.size())
        return false;
    for (int i = 0; i < genericParameters.size(); i++)
    {
        if (isSameFlags & ISSAME_CAST)
        {
            if (other->genericParameters[i]->typeInfo->kind == TypeInfoKind::Generic)
                continue;
        }

        if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
            return false;
    }

    if (!(isSameFlags & ISSAME_CAST))
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

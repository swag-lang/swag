#include "pch.h"
#include "TypeManager.h"
#include "Allocator.h"
#include "AstNode.h"

TypeInfo* TypeInfoNative::clone()
{
    auto newType = g_Allocator.alloc<TypeInfoNative>();
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

    if (to->kind == TypeInfoKind::Alias)
        to = TypeManager::concreteType(to, CONCRETE_ALIAS);

    if (to->kind != TypeInfoKind::Native)
        return false;

    auto other = static_cast<TypeInfoNative*>(to);
    if (nativeType != other->nativeType)
        return false;

    return true;
}

TypeInfo* TypeInfoNamespace::clone()
{
    auto newType   = g_Allocator.alloc<TypeInfoNamespace>();
    newType->scope = scope;
    newType->copyFrom(this);
    return newType;
}

TypeInfo* TypeInfoCode::clone()
{
    auto newType = g_Allocator.alloc<TypeInfoGeneric>();
    newType->copyFrom(this);
    return newType;
}

bool TypeInfoCode::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    if (isSameFlags & ISSAME_CAST)
        return true;
    return false;
}

void TypeInfoAlias::computeScopedName()
{
    unique_lock lk(mutex);
    if (rawType->kind == TypeInfoKind::Native)
        scopedName = name;
    else
        TypeInfo::computeScopedNameNoLock();
}

void TypeInfoAlias::computeName()
{
    unique_lock lk(mutex);

    rawType->computeName();
    preName   = rawType->preName;
    nakedName = rawType->nakedName;
    name      = rawType->name;
}

TypeInfo* TypeInfoAlias::clone()
{
    auto newType     = g_Allocator.alloc<TypeInfoAlias>();
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
    auto newType        = g_Allocator.alloc<TypeInfoParam>();
    newType->namedParam = namedParam;
    newType->typeInfo   = typeInfo;
    newType->index      = index;
    newType->offset     = offset;
    newType->attributes = attributes;
    newType->node       = node;
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

TypeInfo* TypeInfoReference::clone()
{
    auto newType         = g_Allocator.alloc<TypeInfoReference>();
    newType->pointedType = pointedType;
    newType->copyFrom(this);
    return newType;
}

void TypeInfoReference::computeScopedName()
{
    unique_lock lk(mutex);
    if (!scopedName.empty())
        return;

    pointedType->computeScopedName();
    scopedName = preName;
    scopedName += pointedType->scopedName;
}

void TypeInfoReference::computeName()
{
    unique_lock lk(mutex);

    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";

    preName += "&";

    nakedName = pointedType->nakedName;
    pointedType->computeName();

    name = preName;
    name += pointedType->name;
}

bool TypeInfoReference::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;

    auto other = static_cast<TypeInfoReference*>(to);
    return pointedType->isSame(other->pointedType, isSameFlags);
}

TypeInfo* TypeInfoPointer::clone()
{
    auto newType         = g_Allocator.alloc<TypeInfoPointer>();
    newType->finalType   = finalType;
    newType->pointedType = pointedType;
    newType->ptrCount    = ptrCount;
    newType->copyFrom(this);
    return newType;
}

void TypeInfoPointer::computePointedType()
{
    if (ptrCount == 1)
    {
        pointedType = finalType;
    }
    else
    {
        auto result = (TypeInfoPointer*) clone();
        result->ptrCount--;
        result->computeName();
        result->computePointedType();
        pointedType = result;
    }
}

void TypeInfoPointer::computeScopedName()
{
    unique_lock lk(mutex);
    if (!scopedName.empty())
        return;

    if (!finalType) // "null"
        scopedName = name;
    else
    {
        finalType->computeScopedName();
        scopedName = preName;
        scopedName += finalType->scopedName;
    }
}

void TypeInfoPointer::computeName()
{
    unique_lock lk(mutex);

    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";

    for (uint32_t i = 0; i < ptrCount; i++)
        preName += "*";

    nakedName = finalType->nakedName;
    finalType->computeName();

    name = preName;
    name += finalType->name;
}

bool TypeInfoPointer::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (isSameFlags & ISSAME_CAST)
    {
        if (this == g_TypeMgr.typeInfoNull && to->kind == TypeInfoKind::Lambda)
            return true;
    }

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
    auto newType         = g_Allocator.alloc<TypeInfoArray>();
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
    if (!pointedType->isSame(other->pointedType, isSameFlags))
        return false;
    SWAG_ASSERT(!sizeOf || !other->sizeOf || sizeOf == other->sizeOf);
    return true;
}

void TypeInfoArray::computeScopedName()
{
    unique_lock lk(mutex);
    if (!scopedName.empty())
        return;

    pointedType->computeScopedName();
    scopedName = preName;
    scopedName += finalType->scopedName;
}

void TypeInfoArray::computeName()
{
    unique_lock lk(mutex);

    pointedType->computeName();

    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";

    if (count == UINT32_MAX)
    {
        preName += "[] ";
        nakedName = pointedType->nakedName;
    }
    else
    {
        preName += format("[%d", count);
        auto pType = pointedType;
        while (pType->kind == TypeInfoKind::Array)
        {
            auto subType = CastTypeInfo<TypeInfoArray>(pType, TypeInfoKind::Array);
            preName += format(",%d", subType->count);
            pType = subType->pointedType;
        }

        preName += "] ";
        nakedName = pType->nakedName;
    }

    name = preName;
    name += nakedName;
}

void TypeInfoSlice::computeName()
{
    unique_lock lk(mutex);

    pointedType->computeName();
    nakedName = pointedType->nakedName;

    preName.clear();
    if (flags & TYPEINFO_CONST)
        preName = "const ";
    preName += "[..] ";

    name = preName;
    name += nakedName;
}

TypeInfo* TypeInfoSlice::clone()
{
    auto newType         = g_Allocator.alloc<TypeInfoSlice>();
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
    auto newType      = g_Allocator.alloc<TypeInfoList>();
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

    // Can cast from typelist curly to struct
    // The real check will be done later
    if (isSameFlags & ISSAME_CAST)
    {
        if (to->kind == TypeInfoKind::Struct && listKind == TypeInfoListKind::Curly)
            return true;
    }

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
    auto newType = g_Allocator.alloc<TypeInfoVariadic>();
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
    auto newType = g_Allocator.alloc<TypeInfoGeneric>();
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
    auto newType                  = g_Allocator.alloc<TypeInfoFuncAttr>();
    newType->firstDefaultValueIdx = firstDefaultValueIdx;
    newType->returnType           = returnType;
    newType->stackSize            = stackSize;
    newType->attributes           = attributes;
    newType->attributeUsage       = attributeUsage;

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
    unique_lock lk(mutex);

    nakedName.clear();
    if (genericParameters.size() == 1)
    {
        nakedName += "'";
        nakedName += genericParameters[0]->typeInfo ? genericParameters[0]->typeInfo->name : genericParameters[0]->name;
    }
    else if (genericParameters.size())
    {
        nakedName += "'(";
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                nakedName += ", ";
            nakedName += genericParameters[i]->typeInfo ? genericParameters[i]->typeInfo->name : genericParameters[i]->name;
        }

        nakedName += ")";
    }

    nakedName += format("(");
    for (int i = 0; i < parameters.size(); i++)
    {
        if (i)
            nakedName += ", ";
        nakedName += parameters[i]->typeInfo->name;
    }

    nakedName += ")";
    if (returnType)
        nakedName += format("->%s", returnType->name.c_str());
    else
        nakedName += "->void";

    name = nakedName;
}

bool TypeInfoFuncAttr::isSame(TypeInfoFuncAttr* other, uint32_t isSameFlags)
{
    if (parameters.size() != other->parameters.size())
        return false;
    if (genericParameters.size() != other->genericParameters.size())
        return false;

    if (isSameFlags & ISSAME_EXACT)
    {
        if (!returnType && other->returnType && !other->returnType->isNative(NativeTypeKind::Void))
            return false;
        if (returnType && !returnType->isNative(NativeTypeKind::Void) && !other->returnType)
            return false;
        if (returnType && other->returnType && !returnType->isSame(other->returnType, isSameFlags))
            return false;
    }

    for (int i = 0; i < genericParameters.size(); i++)
    {
        if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
            return false;
        if (!(genericParameters[i]->value == other->genericParameters[i]->value))
            return false;
    }

    for (int i = 0; i < parameters.size(); i++)
    {
        if (parameters[i]->typeInfo->isNative(NativeTypeKind::Undefined))
            continue;
        if (other->parameters[i]->typeInfo->isNative(NativeTypeKind::Undefined))
            continue;
        if (!parameters[i]->typeInfo->isSame(other->parameters[i]->typeInfo, isSameFlags))
            return false;
    }

    return true;
}

bool TypeInfoFuncAttr::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;

    if (isSameFlags & ISSAME_CAST)
    {
        if (kind == TypeInfoKind::Lambda && to == g_TypeMgr.typeInfoNull)
            return true;
    }

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
    auto newType        = g_Allocator.alloc<TypeInfoEnum>();
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

bool TypeInfoEnum::isSame(TypeInfo* to, uint32_t isSameFlags)
{
    if (this == to)
        return true;
    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    auto other = static_cast<TypeInfoEnum*>(to);
    if (!rawType->isSame(other->rawType, isSameFlags))
        return false;
    if (values.size() != other->values.size())
        return false;

    if (!(isSameFlags & ISSAME_CAST))
    {
        int childSize = (int) values.size();
        if (childSize != other->values.size())
            return false;
        for (int i = 0; i < childSize; i++)
        {
            if (!values[i]->isSame(other->values[i], isSameFlags))
                return false;
        }
    }

    return true;
}

TypeInfo* TypeInfoStruct::clone()
{
    auto newType               = g_Allocator.alloc<TypeInfoStruct>();
    newType->scope             = scope;
    newType->opInit            = opInit;
    newType->opUserPostCopyFct = opUserPostCopyFct;
    newType->opPostCopy        = opPostCopy;
    newType->opUserPostMoveFct = opUserPostMoveFct;
    newType->opPostMove        = opPostMove;
    newType->opUserDropFct     = opUserDropFct;
    newType->opDrop            = opDrop;
    newType->attributes        = attributes;
    newType->itable            = itable;
    newType->maxPaddingSize    = maxPaddingSize;
    newType->structName        = structName;

    int size = (int) genericParameters.size();
    newType->genericParameters.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = static_cast<TypeInfoParam*>(genericParameters[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->genericParameters.push_back(param);
    }

    size = (int) fields.size();
    newType->fields.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = static_cast<TypeInfoParam*>(fields[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->fields.push_back(param);
    }

    size = (int) methods.size();
    newType->methods.reserve(size);
    for (int i = 0; i < size; i++)
    {
        auto param = static_cast<TypeInfoParam*>(methods[i]);
        param      = static_cast<TypeInfoParam*>(param->clone());
        newType->methods.push_back(param);
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

    if (isSameFlags & ISSAME_INTERFACE)
    {
        if (kind == TypeInfoKind::Interface && to->kind == TypeInfoKind::Struct)
        {
            auto other = static_cast<TypeInfoStruct*>(to);
            if (other->hasInterface(this))
                return true;
        }
    }

    if (!TypeInfo::isSame(to, isSameFlags))
        return false;
    if ((flags & TYPEINFO_STRUCT_IS_TUPLE) != (to->flags & TYPEINFO_STRUCT_IS_TUPLE))
        return false;

    auto other = static_cast<TypeInfoStruct*>(to);

    int childCount = (int) fields.size();
    if (childCount != other->fields.size())
        return false;
    if (structName != other->structName)
        return false;

    // Compare generic parameters
    auto numGenParams = genericParameters.size();
    if (numGenParams != other->genericParameters.size())
        return false;
    for (int i = 0; i < numGenParams; i++)
    {
        if (isSameFlags & ISSAME_CAST)
        {
            if (other->genericParameters[i]->typeInfo->kind == TypeInfoKind::Generic)
                continue;
        }

        if (!genericParameters[i]->typeInfo->isSame(other->genericParameters[i]->typeInfo, isSameFlags))
            return false;
    }

    // Compare field by field
    if (!(isSameFlags & ISSAME_CAST))
    {
        if ((flags & TYPEINFO_GENERIC) != (other->flags & TYPEINFO_GENERIC))
            return false;
        if (scope != other->scope)
            return false;
        for (int i = 0; i < childCount; i++)
        {
            if (!fields[i]->isSame(other->fields[i], isSameFlags))
                return false;
        }
    }

    return true;
}

void TypeInfoStruct::computeName()
{
    unique_lock lk(mutex);

    nakedName = structName;
    if (genericParameters.size() > 0)
    {
        nakedName += "'(";
        for (int i = 0; i < genericParameters.size(); i++)
        {
            if (i)
                nakedName += ", ";
            nakedName += genericParameters[i]->typeInfo ? genericParameters[i]->typeInfo->name : genericParameters[i]->name;
        }

        nakedName += ")";
    }

    name = nakedName;
}
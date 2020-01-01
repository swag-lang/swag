#include "pch.h"
#include "TypeInfo.h"

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
    case TypeInfoKind::Slice:
        return "a slice";
    case TypeInfoKind::Pointer:
        return "a pointer";
    case TypeInfoKind::FuncAttr:
        return "a function";
    case TypeInfoKind::Lambda:
        return "a lambda";
    case TypeInfoKind::TypeList:
        if (static_cast<TypeInfoList*>(typeInfo)->listKind == TypeInfoListKind::Curly)
            return "a tuple";
        return "an array";
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
        return "a variadic";
    case TypeInfoKind::Struct:
        return "a struct";
    case TypeInfoKind::Interface:
        return "an interface";
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
    case TypeInfoKind::Slice:
        return "slice";
    case TypeInfoKind::Pointer:
        return "pointer";
    case TypeInfoKind::FuncAttr:
        return "function";
    case TypeInfoKind::Lambda:
        return "lambda";
    case TypeInfoKind::TypeList:
        if (static_cast<TypeInfoList*>(typeInfo)->listKind == TypeInfoListKind::Curly)
            return "tuple";
        return "array";
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
        return "variadic";
    case TypeInfoKind::Struct:
        return "struct";
    case TypeInfoKind::Interface:
        return "interface";
    case TypeInfoKind::Generic:
        return "generic type";
    case TypeInfoKind::Alias:
        return "type alias";
    case TypeInfoKind::Native:
        return "type";
    }

    return "<type>";
}

bool TypeInfo::isPointerTo(TypeInfoKind pointerKind)
{
    if (kind != TypeInfoKind::Pointer)
        return false;
    auto ptr = (TypeInfoPointer*) this;
    if (ptr->ptrCount != 1)
        return false;
    if (ptr->finalType->kind != pointerKind)
        return false;
    return true;
}

bool TypeInfo::isInitializerList()
{
    if (kind != TypeInfoKind::TypeList)
        return false;
    return ((TypeInfoList*) this)->listKind == TypeInfoListKind::Curly;
}
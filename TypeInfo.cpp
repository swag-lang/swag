#include "pch.h"
#include "TypeInfo.h"

thread_local Pool<TypeInfoFuncAttr>  g_Pool_typeInfoFuncAttr;
thread_local Pool<TypeInfoNamespace> g_Pool_typeInfoNamespace;
thread_local Pool<TypeInfoEnum>      g_Pool_typeInfoEnum;
thread_local Pool<TypeInfoParam>     g_Pool_typeInfoParam;
thread_local Pool<TypeInfoPointer>   g_Pool_typeInfoPointer;
thread_local Pool<TypeInfoArray>     g_Pool_typeInfoArray;
thread_local Pool<TypeInfoSlice>     g_Pool_typeInfoSlice;
thread_local Pool<TypeInfoList>      g_Pool_typeInfoList;
thread_local Pool<TypeInfoNative>    g_Pool_typeInfoNative;
thread_local Pool<TypeInfoVariadic>  g_Pool_typeInfoVariadic;
thread_local Pool<TypeInfoGeneric>   g_Pool_typeInfoGeneric;
thread_local Pool<TypeInfoStruct>    g_Pool_typeInfoStruct;
thread_local Pool<TypeInfoAlias>     g_Pool_typeInfoAlias;
thread_local Pool<TypeInfoCode>      g_Pool_typeInfoCode;

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
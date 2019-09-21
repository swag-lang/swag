#include "pch.h"
#include "TypeInfo.h"

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
        if (static_cast<TypeInfoList*>(typeInfo)->listKind == TypeInfoListKind::Tuple)
            return "a tuple";
        return "an array";
    case TypeInfoKind::Variadic:
	case TypeInfoKind::TypedVariadic:
    case TypeInfoKind::VariadicValue:
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
        if (static_cast<TypeInfoList*>(typeInfo)->listKind == TypeInfoListKind::Tuple)
            return "tuple";
        return "array";
    case TypeInfoKind::Variadic:
	case TypeInfoKind::TypedVariadic:
    case TypeInfoKind::VariadicValue:
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
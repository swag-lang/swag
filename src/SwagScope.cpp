#include "pch.h"
#include "SwagScope.h"
#include "TypeManager.h"

void SwagScope::registerType(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    if (typeInfo->name == "TypeInfo")
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfo->flags |= TYPEINFO_STRUCT_TYPEINFO;
        g_TypeMgr.registerTypeType();
    }
    else if (typeInfo->name == "TypeInfoNative")
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoNative->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoPointer")
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoPointer->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoReference")
    {
        SWAG_ASSERT(!regTypeInfoReference);
        regTypeInfoReference = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoReference->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoStruct")
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoStruct->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoParam")
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoParam->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoFunc")
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoFunc->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoEnum")
    {
        SWAG_ASSERT(!regTypeInfoEnum);
        regTypeInfoEnum = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoEnum->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoVariadic")
    {
        SWAG_ASSERT(!regTypeInfoVariadic);
        regTypeInfoVariadic = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoVariadic->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoArray")
    {
        SWAG_ASSERT(!regTypeInfoArray);
        regTypeInfoArray = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoArray->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoSlice")
    {
        SWAG_ASSERT(!regTypeInfoSlice);
        regTypeInfoSlice = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoSlice->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoGeneric")
    {
        SWAG_ASSERT(!regTypeInfoGeneric);
        regTypeInfoGeneric = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoGeneric->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "TypeInfoAlias")
    {
        SWAG_ASSERT(!regTypeInfoAlias);
        regTypeInfoAlias = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoAlias->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
    else if (typeInfo->name == "CompilerSourceLocation")
    {
        SWAG_ASSERT(!regTypeInfoSourceLoc);
        regTypeInfoSourceLoc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoSourceLoc->flags |= TYPEINFO_STRUCT_TYPEINFO;
    }
}

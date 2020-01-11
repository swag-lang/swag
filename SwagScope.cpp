#include "pch.h"
#include "SwagScope.h"
#include "TypeInfo.h"
#include "ThreadManager.h"

void SwagScope::registerType(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    if (typeInfo->name == "TypeInfo")
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoNative")
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoPointer")
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoStruct")
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoParam")
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoFunc")
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoEnum")
    {
        SWAG_ASSERT(!regTypeInfoEnum);
        regTypeInfoEnum = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoVariadic")
    {
        SWAG_ASSERT(!regTypeInfoVariadic);
        regTypeInfoVariadic = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoArray")
    {
        SWAG_ASSERT(!regTypeInfoArray);
        regTypeInfoArray = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoSlice")
    {
        SWAG_ASSERT(!regTypeInfoSlice);
        regTypeInfoSlice = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeInfoGeneric")
    {
        SWAG_ASSERT(!regTypeInfoGeneric);
        regTypeInfoGeneric = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
}

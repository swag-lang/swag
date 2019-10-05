#include "pch.h"
#include "SwagScope.h"
#include "TypeInfo.h"
#include "ThreadManager.h"

void SwagScope::registerType(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    if (typeInfo->name == "Typeinfo")
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoNative")
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoPointer")
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoStruct")
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoParam")
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoFunc")
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoEnum")
    {
        SWAG_ASSERT(!regTypeInfoEnum);
        regTypeInfoEnum = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoVariadic")
    {
        SWAG_ASSERT(!regTypeInfoVariadic);
        regTypeInfoVariadic = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoArray")
    {
        SWAG_ASSERT(!regTypeInfoArray);
		regTypeInfoArray = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
    else if (typeInfo->name == "TypeinfoSlice")
    {
        SWAG_ASSERT(!regTypeInfoSlice);
        regTypeInfoSlice = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }
}

#include "pch.h"
#include "SwagScope.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

void SwagScope::registerType(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    if (typeInfo->name == g_LangSpec->name_TypeInfo)
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfo->flags |= TYPEINFO_STRUCT_TYPEINFO;
        g_TypeMgr->registerTypeType();
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfo));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoNative)
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoNative->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoNative));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoPointer)
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoPointer->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoPointer));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoReference)
    {
        SWAG_ASSERT(!regTypeInfoReference);
        regTypeInfoReference = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoReference->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoReference));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoStruct)
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoStruct->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoStruct));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoParam)
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoParam->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoParam));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoFunc)
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoFunc->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoFunc));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoEnum)
    {
        SWAG_ASSERT(!regTypeInfoEnum);
        regTypeInfoEnum = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoEnum->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoEnum));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoVariadic)
    {
        SWAG_ASSERT(!regTypeInfoVariadic);
        regTypeInfoVariadic = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoVariadic->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoVariadic));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoArray)
    {
        SWAG_ASSERT(!regTypeInfoArray);
        regTypeInfoArray = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoArray->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoArray));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoSlice)
    {
        SWAG_ASSERT(!regTypeInfoSlice);
        regTypeInfoSlice = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoSlice->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoSlice));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoGeneric)
    {
        SWAG_ASSERT(!regTypeInfoGeneric);
        regTypeInfoGeneric = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoGeneric->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoGeneric));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoAlias)
    {
        SWAG_ASSERT(!regTypeInfoAlias);
        regTypeInfoAlias = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoAlias->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ConcreteTypeInfoAlias));
    }
    else if (typeInfo->name == g_LangSpec->name_CompilerSourceLocation)
    {
        SWAG_ASSERT(!regTypeInfoSourceLoc);
        regTypeInfoSourceLoc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoSourceLoc->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(SwagCompilerSourceLocation));
    }
    else if (typeInfo->name == g_LangSpec->name_BuildCfg)
    {
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(BuildCfg));
    }
}

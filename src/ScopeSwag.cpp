#include "pch.h"
#include "ScopeSwag.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

void ScopeSwag::registerType(TypeInfo* typeInfo)
{
    if (!typeInfo->isStruct() && !typeInfo->isEnum())
        return;

    if (typeInfo->name == g_LangSpec->name_TypeInfo)
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfo->flags |= TYPEINFO_STRUCT_TYPEINFO;
        g_TypeMgr->registerTypeType();
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfo));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoNative)
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoNative->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoNative));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoPointer)
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoPointer->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoPointer));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoStruct)
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoStruct->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoStruct));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoParam)
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoParam->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeValue));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoFunc)
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoFunc->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoFunc));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoEnum)
    {
        SWAG_ASSERT(!regTypeInfoEnum);
        regTypeInfoEnum = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoEnum->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoEnum));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoVariadic)
    {
        SWAG_ASSERT(!regTypeInfoVariadic);
        regTypeInfoVariadic = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoVariadic->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoVariadic));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoArray)
    {
        SWAG_ASSERT(!regTypeInfoArray);
        regTypeInfoArray = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoArray->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoArray));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoSlice)
    {
        SWAG_ASSERT(!regTypeInfoSlice);
        regTypeInfoSlice = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoSlice->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoSlice));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoGeneric)
    {
        SWAG_ASSERT(!regTypeInfoGeneric);
        regTypeInfoGeneric = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoGeneric->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoGeneric));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoAlias)
    {
        SWAG_ASSERT(!regTypeInfoAlias);
        regTypeInfoAlias = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoAlias->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoAlias));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoNamespace)
    {
        SWAG_ASSERT(!regTypeInfoNamespace);
        regTypeInfoNamespace = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoNamespace->flags |= TYPEINFO_STRUCT_TYPEINFO;
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoNamespace));
    }
    else if (typeInfo->name == g_LangSpec->name_SourceCodeLocation)
    {
        SWAG_ASSERT(!regTypeInfoSourceLoc);
        regTypeInfoSourceLoc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(SwagSourceCodeLocation));
    }
    else if (typeInfo->name == g_LangSpec->name_TargetArch)
    {
        SWAG_ASSERT(!regTypeInfoTargetArch);
        regTypeInfoTargetArch = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    }
    else if (typeInfo->name == g_LangSpec->name_TargetOs)
    {
        SWAG_ASSERT(!regTypeInfoTargetOs);
        regTypeInfoTargetOs = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    }
    else if (typeInfo->name == g_LangSpec->name_Backend)
    {
        SWAG_ASSERT(!regTypeInfoBackend);
        regTypeInfoBackend = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    }
    else if (typeInfo->name == g_LangSpec->name_BuildCfg)
    {
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(BuildCfg));
    }
}

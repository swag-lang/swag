#include "pch.h"
#include "Semantic/ScopeSwag.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

void ScopeSwag::registerType(TypeInfo* typeInfo)
{
    if (!typeInfo->isStruct() && !typeInfo->isEnum())
        return;

    if (typeInfo->name == g_LangSpec->name_TypeInfo)
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfo->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        g_TypeMgr->registerTypeType();
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfo));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoNative)
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoNative->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoNative));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoPointer)
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoPointer->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoPointer));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoStruct)
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoStruct->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoStruct));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoParam)
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoParam->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeValue));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoFunc)
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoFunc->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoFunc));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoEnum)
    {
        SWAG_ASSERT(!regTypeInfoEnum);
        regTypeInfoEnum = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoEnum->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoEnum));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoVariadic)
    {
        SWAG_ASSERT(!regTypeInfoVariadic);
        regTypeInfoVariadic = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoVariadic->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoVariadic));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoArray)
    {
        SWAG_ASSERT(!regTypeInfoArray);
        regTypeInfoArray = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoArray->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoArray));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoSlice)
    {
        SWAG_ASSERT(!regTypeInfoSlice);
        regTypeInfoSlice = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoSlice->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoSlice));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoGeneric)
    {
        SWAG_ASSERT(!regTypeInfoGeneric);
        regTypeInfoGeneric = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoGeneric->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoGeneric));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoCodeBlock)
    {
        SWAG_ASSERT(!regTypeInfoCodeBlock);
        regTypeInfoCodeBlock = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoCodeBlock->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoCode));
    }    
    else if (typeInfo->name == g_LangSpec->name_TypeInfoAlias)
    {
        SWAG_ASSERT(!regTypeInfoAlias);
        regTypeInfoAlias = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoAlias->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoAlias));
    }
    else if (typeInfo->name == g_LangSpec->name_TypeInfoNamespace)
    {
        SWAG_ASSERT(!regTypeInfoNamespace);
        regTypeInfoNamespace = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        regTypeInfoNamespace->addFlag(TYPEINFO_STRUCT_TYPEINFO);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(ExportedTypeInfoNamespace));
    }
    else if (typeInfo->name == g_LangSpec->name_SourceCodeLocation)
    {
        SWAG_ASSERT(!regTypeInfoSourceLoc);
        regTypeInfoSourceLoc = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(SwagSourceCodeLocation));
    }
    else if (typeInfo->name == g_LangSpec->name_TargetArch)
    {
        SWAG_ASSERT(!regTypeInfoTargetArch);
        regTypeInfoTargetArch = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    }
    else if (typeInfo->name == g_LangSpec->name_TargetOs)
    {
        SWAG_ASSERT(!regTypeInfoTargetOs);
        regTypeInfoTargetOs = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    }
    else if (typeInfo->name == g_LangSpec->name_Backend)
    {
        SWAG_ASSERT(!regTypeInfoBackend);
        regTypeInfoBackend = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
    }
    else if (typeInfo->name == g_LangSpec->name_BuildCfg)
    {
        SWAG_ASSERT(typeInfo->sizeOf == sizeof(BuildCfg));
    }
}

#include "pch.h"
#include "SwagScope.h"
#include "TypeInfo.h"
#include "ThreadManager.h"

void SwagScope::registerType(TypeInfo* typeInfo)
{
    scoped_lock lock(mutex);
    if (fullySolved)
        return;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    if (typeInfo->name == "typeinfo")
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_native")
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_pointer")
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_struct")
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_param")
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_func")
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_enum")
    {
        SWAG_ASSERT(!regTypeInfoEnum);
        regTypeInfoEnum = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_variadic")
    {
        SWAG_ASSERT(!regTypeInfoVariadic);
        regTypeInfoVariadic = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "typeinfo_array")
    {
        SWAG_ASSERT(!regTypeInfoArray);
		regTypeInfoArray = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }

    if (cptSolved == 9)
    {
        fullySolved = true;
        for (auto job : dependentJobs)
            g_ThreadMgr.addJob(job);
        dependentJobs.clear();
    }
}

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

    if (typeInfo->name == "TypeInfo")
    {
        SWAG_ASSERT(!regTypeInfo);
        regTypeInfo = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "TypeInfoNative")
    {
        SWAG_ASSERT(!regTypeInfoNative);
        regTypeInfoNative = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "TypeInfoPointer")
    {
        SWAG_ASSERT(!regTypeInfoPointer);
        regTypeInfoPointer = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "TypeInfoStruct")
    {
        SWAG_ASSERT(!regTypeInfoStruct);
        regTypeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "TypeInfoParam")
    {
        SWAG_ASSERT(!regTypeInfoParam);
        regTypeInfoParam = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "TypeInfoFunc")
    {
        SWAG_ASSERT(!regTypeInfoFunc);
        regTypeInfoFunc = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }
    else if (typeInfo->name == "TypeInfoEnum")
    {
        SWAG_ASSERT(!regTypeInfoEnum);
		regTypeInfoEnum = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        cptSolved++;
    }

    if (cptSolved == 7)
    {
        fullySolved = true;
        for (auto job : dependentJobs)
            g_ThreadMgr.addJob(job);
        dependentJobs.clear();
    }
}

#include "pch.h"
#include "TypeInfo.h"
#include "TypeList.h"
#include "TypeManager.h"

void TypeList::registerInit()
{
    registerType(g_TypeMgr.typeInfoS8);
    registerType(g_TypeMgr.typeInfoS16);
    registerType(g_TypeMgr.typeInfoS32);
    registerType(g_TypeMgr.typeInfoS64);
    registerType(g_TypeMgr.typeInfoU8);
    registerType(g_TypeMgr.typeInfoU16);
    registerType(g_TypeMgr.typeInfoU32);
    registerType(g_TypeMgr.typeInfoU64);
    registerType(g_TypeMgr.typeInfoF32);
    registerType(g_TypeMgr.typeInfoF64);
    registerType(g_TypeMgr.typeInfoBool);
    registerType(g_TypeMgr.typeInfoString);
}

TypeInfo* TypeList::registerType(TypeInfo* newTypeInfo)
{
    unique_lock lk(mutex);

    // Sanity check
    SWAG_ASSERT(newTypeInfo->kind != TypeInfoKind::Pointer || ((TypeInfoPointer*) newTypeInfo)->pointedType);

    auto& oneList = allTypes[(uint32_t) newTypeInfo->kind];
    if (newTypeInfo->kind != TypeInfoKind::Enum && newTypeInfo->kind != TypeInfoKind::Struct)
    {
        for (auto typeInfo : oneList)
        {
            if (typeInfo->isSame(newTypeInfo, sameFlags))
            {
                return typeInfo;
            }
        }
    }

    newTypeInfo->flags |= hereFlag;
    oneList.push_back(newTypeInfo);
    return newTypeInfo;
}

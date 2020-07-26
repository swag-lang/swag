#include "pch.h"
#include "TypeContainer.h"

TypeInfo* TypeContainer::registerType(TypeInfo* newTypeInfo)
{
    unique_lock lk(mutex);

    newTypeInfo->computeScopedName();
    auto it = allTypes.find(newTypeInfo->scopedName);
    if (it != allTypes.end())
        return it->second;

    allTypes[newTypeInfo->scopedName] = newTypeInfo;
    return newTypeInfo;
}

#include "pch.h"
#include "TypeManager.h"
#include "TypeInfo.h"

TypeInfo* TypeManager::flattenType(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Enum)
        return typeInfo;
    return static_cast<TypeInfoEnum*>(typeInfo)->rawType;
}
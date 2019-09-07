#pragma once
#include "SpinLock.h"
struct TypeInfo;

struct TypeTable
{
    TypeInfo* registerType(TypeInfo* typeInfo);
    TypeInfo* makeConcreteTypeInfo(TypeInfo* typeInfo);

    SpinLock          mutexTypes;
    vector<TypeInfo*> allTypes;
};

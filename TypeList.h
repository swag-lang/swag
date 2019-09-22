#pragma once
#include "SpinLock.h"
#include "TypeInfo.h"

struct TypeList
{
    TypeInfo* registerType(TypeInfo* typeInfo);

    uint32_t          hereFlag      = 0;
    uint32_t          sameFlags     = 0;
    bool              releaseIfHere = false;
    SpinLock          mutexTypes;

    vector<TypeInfo*> allTypes[(uint32_t) TypeInfoKind::Count];
};

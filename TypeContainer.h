#pragma once
#include "TypeInfo.h"

struct TypeContainer
{
    TypeInfo* registerType(TypeInfo* typeInfo);
    void      registerInit();

    uint32_t     hereFlag  = 0;
    uint32_t     sameFlags = 0;
    shared_mutex mutex;

    vector<TypeInfo*> allTypes[(uint32_t) TypeInfoKind::Count];
};

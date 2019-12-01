#pragma once
#include "TypeInfo.h"

struct TypeList
{
    TypeInfo* registerType(TypeInfo* typeInfo);
    void      registerInit();

    uint32_t     hereFlag      = 0;
    uint32_t     sameFlags     = 0;
    bool         releaseIfHere = false;
    shared_mutex mutex;

    vector<TypeInfo*> allTypes[(uint32_t) TypeInfoKind::Count];
};

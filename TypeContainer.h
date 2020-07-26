#pragma once
#include "TypeInfo.h"

struct TypeContainer
{
    TypeInfo* registerType(TypeInfo* typeInfo);

    shared_mutex            mutex;
    map<Utf8Crc, TypeInfo*> allTypes;
};

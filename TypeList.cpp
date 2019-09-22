#include "pch.h"
#include "TypeInfo.h"
#include "TypeList.h"

TypeInfo* TypeList::registerType(TypeInfo* newTypeInfo)
{
    scoped_lock lk(mutexTypes);

    auto& oneList = allTypes[(uint32_t) newTypeInfo->kind];
    for (auto typeInfo : oneList)
    {
        if (typeInfo->isSame(newTypeInfo, sameFlags))
        {
            if ((newTypeInfo != typeInfo) && !(newTypeInfo->flags & hereFlag))
            {
                if (releaseIfHere)
                    newTypeInfo->release();
            }

            return typeInfo;
        }
    }

    newTypeInfo->flags |= hereFlag;
    oneList.push_back(newTypeInfo);
    return newTypeInfo;
}

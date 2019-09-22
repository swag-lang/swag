#include "pch.h"
#include "TypeInfo.h"
#include "TypeList.h"

TypeInfo* TypeList::registerType(TypeInfo* newTypeInfo)
{
    scoped_lock lk(mutexTypes);

	// @Speed
    for (auto typeInfo : allTypes)
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
    allTypes.push_back(newTypeInfo);
    return newTypeInfo;
}

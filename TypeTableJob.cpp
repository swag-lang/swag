#include "pch.h"
#include "Module.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "TypeTableJob.h"

thread_local Pool<TypeTableJob> g_Pool_typeTableJob;

bool TypeTableJob::computeStruct()
{
    auto concreteType = (ConcreteTypeInfoStruct*) concreteTypeInfoValue;
    auto realType     = (TypeInfoStruct*) typeInfo;

    unique_lock lk(typeTable->mutexTypes);
    unique_lock lk1(module->constantSegment.mutex);

    SWAG_CHECK(typeTable->makeConcreteAttributes(baseContext, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes)));

    // Fields
    concreteType->fields.buffer = nullptr;
    concreteType->fields.count  = realType->fields.size();
    if (concreteType->fields.count)
    {
        uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) concreteType->fields.count * sizeof(void*));
        ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
        concreteType->fields.buffer     = addrArray;
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->fields.buffer), storageArray);
        for (int idx = 0; idx < concreteType->fields.count; idx++)
        {
            SWAG_CHECK(typeTable->makeConcreteSubTypeInfo(baseContext, addrArray, storageArray, addrArray + idx, realType->fields[idx]));
        }
    }

    // Methods
    concreteType->methods.buffer = nullptr;
    concreteType->methods.count  = realType->methods.size();
    if (concreteType->methods.count)
    {
        uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) concreteType->methods.count * sizeof(void*));
        ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
        concreteType->methods.buffer    = addrArray;
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->methods.buffer), storageArray);
        for (int idx = 0; idx < concreteType->methods.count; idx++)
        {
            SWAG_CHECK(typeTable->makeConcreteSubTypeInfo(baseContext, addrArray, storageArray, addrArray + idx, realType->methods[idx]));
        }
    }

    // Interfaces
    concreteType->interfaces.buffer = nullptr;
    concreteType->interfaces.count  = realType->interfaces.size();
    if (concreteType->interfaces.count)
    {
        uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) concreteType->interfaces.count * sizeof(void*));
        ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
        concreteType->interfaces.buffer = addrArray;
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->interfaces.buffer), storageArray);
        for (int idx = 0; idx < concreteType->interfaces.count; idx++)
        {
            auto typeItf = realType->interfaces[idx];
            SWAG_CHECK(typeTable->makeConcreteSubTypeInfo(baseContext, addrArray, storageArray, addrArray + idx, typeItf));
        }
    }

    return true;
}

JobResult TypeTableJob::execute()
{
    JobContext context;
    context.sourceFile = sourceFile;
    context.baseJob    = this;
    baseContext        = &context;

    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Interface);
    computeStruct();

    return JobResult::ReleaseJob;
}

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
    auto segment      = typeTable->getConstantSegment(module, cflags);

    unique_lock lk(typeTable->mutexTypes);
    unique_lock lk1(segment->mutex);

    SWAG_CHECK(typeTable->makeConcreteAttributes(baseContext, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes), cflags));

    // Fields
    concreteType->fields.buffer = nullptr;
    concreteType->fields.count  = realType->fields.size();
    if (concreteType->fields.count)
    {
        uint32_t               storageArray = segment->reserveNoLock((uint32_t) concreteType->fields.count * sizeof(ConcreteTypeInfoParam));
        ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) segment->addressNoLock(storageArray);
        concreteType->fields.buffer         = addrArray;
        segment->addInitPtr(OFFSETOF(concreteType->fields.buffer), storageArray);
        for (int param = 0; param < concreteType->fields.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->fields[param], cflags));
            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    // Methods
    concreteType->methods.buffer = nullptr;
    concreteType->methods.count  = realType->methods.size();
    if (concreteType->methods.count)
    {
        uint32_t               storageArray = segment->reserveNoLock((uint32_t) concreteType->methods.count * sizeof(ConcreteTypeInfoParam));
        ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) segment->addressNoLock(storageArray);
        concreteType->methods.buffer        = addrArray;
        segment->addInitPtr(OFFSETOF(concreteType->methods.buffer), storageArray);
        for (int param = 0; param < concreteType->methods.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->methods[param], cflags));
            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    // Interfaces
    concreteType->interfaces.buffer = nullptr;
    concreteType->interfaces.count  = realType->interfaces.size();
    if (concreteType->interfaces.count)
    {
        uint32_t               storageArray = segment->reserveNoLock((uint32_t) concreteType->interfaces.count * sizeof(ConcreteTypeInfoParam));
        ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) segment->addressNoLock(storageArray);
        concreteType->interfaces.buffer     = addrArray;
        segment->addInitPtr(OFFSETOF(concreteType->interfaces.buffer), storageArray);
        for (int param = 0; param < concreteType->interfaces.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->interfaces[param], cflags));

            // Compute the storage of the interface for swag_runtime_interfaceof
            // Not needed if we are computing a compiler type
            if (!(cflags & CONCRETE_FOR_COMPILER))
            {
                uint32_t fieldOffset   = offsetof(ConcreteTypeInfoParam, value);
                uint32_t valueOffset   = storageArray + (param * sizeof(ConcreteTypeInfoParam)) + fieldOffset;
                addrArray[param].value = segment->addressNoLock(realType->interfaces[param]->offset);
                segment->addInitPtr(valueOffset, realType->interfaces[param]->offset);
            }

            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    return true;
}

JobResult TypeTableJob::execute()
{
    JobContext context;
    context.sourceFile = sourceFile;
    context.baseJob    = this;
    context.node       = nodes.front();
    baseContext        = &context;

    if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Interface)
    {
        // Need to wait for all interfaces & methods to be registered
        auto realType = (TypeInfoStruct*) typeInfo;
        waitForAllStructInterfaces(realType);
        if (baseContext->result == ContextResult::Pending)
            return JobResult::KeepJobAlive;
        waitForAllStructMethods(realType);
        if (baseContext->result == ContextResult::Pending)
            return JobResult::KeepJobAlive;
        computeStruct();
    }
    else
    {
        SWAG_ASSERT(false);
    }

    return JobResult::ReleaseJob;
}

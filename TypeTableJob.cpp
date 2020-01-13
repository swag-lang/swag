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
        uint32_t               storageArray = module->constantSegment.reserveNoLock((uint32_t) concreteType->fields.count * sizeof(ConcreteTypeInfoParam));
        ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) module->constantSegment.addressNoLock(storageArray);
        concreteType->fields.buffer         = addrArray;
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->fields.buffer), storageArray);
        for (int param = 0; param < concreteType->fields.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->fields[param]));
            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    // Methods
    concreteType->methods.buffer = nullptr;
    concreteType->methods.count  = realType->methods.size();
    if (concreteType->methods.count)
    {
        uint32_t               storageArray = module->constantSegment.reserveNoLock((uint32_t) concreteType->methods.count * sizeof(ConcreteTypeInfoParam));
        ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) module->constantSegment.addressNoLock(storageArray);
        concreteType->methods.buffer        = addrArray;
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->methods.buffer), storageArray);
        for (int param = 0; param < concreteType->methods.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->methods[param]));
            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    // Interfaces
    concreteType->interfaces.buffer = nullptr;
    concreteType->interfaces.count  = realType->interfaces.size();
    if (concreteType->interfaces.count)
    {
        uint32_t               storageArray = module->constantSegment.reserveNoLock((uint32_t) concreteType->interfaces.count * sizeof(ConcreteTypeInfoParam));
        ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) module->constantSegment.addressNoLock(storageArray);
        concreteType->interfaces.buffer     = addrArray;
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->interfaces.buffer), storageArray);
        for (int param = 0; param < concreteType->interfaces.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->interfaces[param]));
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

    typeTable->typeTableJobDone();
    return JobResult::ReleaseJob;
}

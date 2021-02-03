#include "pch.h"
#include "Module.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "TypeTableJob.h"
#include "Generic.h"

thread_local Pool<TypeTableJob> g_Pool_typeTableJob;

bool TypeTableJob::computeStruct()
{
    auto concreteType = (ConcreteTypeInfoStruct*) concreteTypeInfoValue;
    auto realType     = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
    auto segment      = typeTable->getSegmentStorage(module, cflags);

    if (realType->opPostCopy || realType->opUserPostCopyFct)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::HasPostCopy;
    if (realType->opPostMove || realType->opUserPostMoveFct)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::HasPostMove;
    if (realType->opDrop || realType->opUserDropFct)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::HasDrop;

    // First and main pass, by locking only the type segment
    {
        unique_lock lk1(segment->mutex);

        // Update methods with types if generic
        if (!realType->replaceTypes.empty())
        {
            for (auto method : realType->methods)
            {
                method->typeInfo = Generic::doTypeSubstitution(realType->replaceTypes, method->typeInfo);
            }

            // Update field  with types if generic
            for (auto field : realType->fields)
            {
                field->typeInfo = Generic::doTypeSubstitution(realType->replaceTypes, field->typeInfo);
            }
        }

        SWAG_CHECK(typeTable->makeConcreteAttributes(baseContext, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes), cflags));

        // Generics
        concreteType->generics.buffer = 0;
        concreteType->generics.count  = realType->genericParameters.size();
        if (concreteType->generics.count)
        {
            uint32_t count = (uint32_t) concreteType->generics.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageOffset, &concreteType->generics.buffer, cflags, storageArray);
            for (int param = 0; param < concreteType->generics.count; param++)
            {
                SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->genericParameters[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        // Fields
        concreteType->fields.buffer = 0;
        concreteType->fields.count  = realType->fields.size();
        if (concreteType->fields.count)
        {
            uint32_t count = (uint32_t) concreteType->fields.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageOffset, &concreteType->fields.buffer, cflags, storageArray);
            for (int param = 0; param < concreteType->fields.count; param++)
            {
                SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->fields[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        // Methods
        concreteType->methods.buffer = 0;
        concreteType->methods.count  = realType->methods.size();
        if (concreteType->methods.count)
        {
            uint32_t count = (uint32_t) concreteType->methods.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageOffset, &concreteType->methods.buffer, cflags, storageArray);
            for (int param = 0; param < concreteType->methods.count; param++)
            {
                SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->methods[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        // Interfaces
        concreteType->interfaces.buffer = 0;
        concreteType->interfaces.count  = realType->interfaces.size();
        if (concreteType->interfaces.count)
        {
            uint32_t count = (uint32_t) concreteType->interfaces.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageOffset, &concreteType->interfaces.buffer, cflags, storageArray);
            for (int param = 0; param < concreteType->interfaces.count; param++)
            {
                SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageArray, realType->interfaces[param], cflags));

                // Compute the storage of the interface for swag_runtime_interfaceof
                // Not needed if we are computing a compiler type
                if (!(cflags & CONCRETE_FOR_COMPILER))
                {
                    uint32_t fieldOffset = offsetof(ConcreteTypeInfoParam, value);
                    uint32_t valueOffset = storageArray + fieldOffset;
                    segment->addInitPtr(valueOffset, realType->interfaces[param]->offset, SegmentKind::Constant);
                }

                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }
    }

    // Second pass on interfaces.
    // We do not want that pass to occur while locking the type segment, because this pass will lock also the constant segment
    // (by calling constantSegment.address), and this can create a dead lock with SemanticJob::storeToSegmentNoLock (which first
    // lock the constant segment, and then can lock the type segment for the 'any' type)
    //
    // This pass is used to store the address of each function of the interface in the 'value' field of the ConcreteTypeInfoParam.
    if (concreteType->interfaces.count && !(cflags & CONCRETE_FOR_COMPILER))
    {
        ConcreteTypeInfoParam* addrArray = (ConcreteTypeInfoParam*) concreteType->interfaces.buffer;
        for (int param = 0; param < concreteType->interfaces.count; param++)
            addrArray[param].value = module->constantSegment.address(realType->interfaces[param]->offset);
    }

    // Job is done, remove it from the map
    {
        unique_lock lk1(segment->mutex);
        typeTable->tableJobDone(this);
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

    if (typeInfo->kind == TypeInfoKind::Struct ||
        typeInfo->kind == TypeInfoKind::Interface ||
        typeInfo->kind == TypeInfoKind::TypeSet)
    {
        // Need to wait for all interfaces & methods to be registered
        auto realType = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
        waitForAllStructInterfaces(realType);
        if (baseContext->result == ContextResult::Pending)
            return JobResult::KeepJobAlive;
        waitForAllStructMethods(realType);
        if (baseContext->result == ContextResult::Pending)
            return JobResult::KeepJobAlive;
        waitStructGenerated(realType);
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

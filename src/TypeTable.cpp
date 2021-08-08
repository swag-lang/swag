#include "pch.h"
#include "TypeTable.h"
#include "SemanticJob.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "TypeTableJob.h"
#include "TypeManager.h"
#include "Module.h"
#include "ErrorIds.h"

TypeTable::MapPerSeg& TypeTable::getMapPerSeg(DataSegment* segment)
{
    return mapPerSegment[segment->kind == SegmentKind::Compiler ? 1 : 0];
}

bool TypeTable::makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storage, uint32_t cflags, TypeInfo** ptrTypeInfo)
{
    auto&      mapPerSeg = getMapPerSeg(storageSegment);
    ScopedLock lk(mapPerSeg.mutex);
    return makeConcreteTypeInfoNoLock(context, nullptr, typeInfo, storageSegment, storage, cflags, ptrTypeInfo);
}

bool TypeTable::makeConcreteStruct(JobContext* context, const auto& typeName, ConcreteTypeInfo* concreteTypeInfoValue, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t storageOffset, uint32_t cflags)
{
    SWAG_ASSERT(context);
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    auto& mapPerSeg            = getMapPerSeg(storageSegment);
    auto  job                  = g_Allocator.alloc<TypeTableJob>();
    job->module                = module;
    job->typeTable             = this;
    job->sourceFile            = sourceFile;
    job->concreteTypeInfoValue = concreteTypeInfoValue;
    job->typeInfo              = typeInfo;
    job->storageOffset         = storageOffset;
    job->storageSegment        = storageSegment;
    job->cflags                = cflags & ~MAKE_CONCRETE_SHOULD_WAIT;
    job->typeName              = typeName;
    job->nodes.push_back(context->node);
    mapPerSeg.concreteTypesJob[typeName] = job;
    if (g_CommandLine.stats)
        g_Stats.totalConcreteStructTypes++;

    if (cflags & MAKE_CONCRETE_SHOULD_WAIT)
    {
        SWAG_ASSERT(context->result == ContextResult::Done || !strcmp(context->baseJob->waitingId, "MAKE_CONCRETE_SHOULD_WAIT"));
        job->dependentJob = context->baseJob;
        context->baseJob->setPending(nullptr, "MAKE_CONCRETE_SHOULD_WAIT", nullptr, typeInfo);
        context->baseJob->jobsToAdd.push_back(job);
    }
    else
    {
        // No need to wait for the result. We then register the top level job as the job to wait for this
        // new one. We CANNOT just register the current dependJob, as it can already be waiting
        job->dependentJob = context->baseJob->dependentJob;
        while (job->dependentJob && job->dependentJob->dependentJob)
            job->dependentJob = job->dependentJob->dependentJob;
        g_ThreadMgr.addJob(job);
    }

    return true;
}

bool TypeTable::makeConcreteTypeInfoNoLock(JobContext* context, ConcreteTypeInfo** result, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storage, uint32_t cflags, TypeInfo** ptrTypeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::TypeListArray:
        typeInfo = TypeManager::convertTypeListToArray(context, (TypeInfoList*) typeInfo, true);
        break;
    case TypeInfoKind::TypeListTuple:
        typeInfo = TypeManager::convertTypeListToStruct(context, (TypeInfoList*) typeInfo, true);
        break;
    }

    if (!(cflags & MAKE_CONCRETE_FORCE_NO_SCOPE))
    {
        typeInfo->computeScopedName();
        SWAG_ASSERT(!typeInfo->scopedName.empty());
    }

    auto& typeName = getTypeName(typeInfo, cflags & MAKE_CONCRETE_FORCE_NO_SCOPE);
    SWAG_ASSERT(!typeName.empty());

    auto& mapPerSeg = getMapPerSeg(storageSegment);
    if (typeInfo->kind != TypeInfoKind::Param)
    {
        // Already computed ?
        auto it = mapPerSeg.concreteTypes.find(typeName);
        if (it != mapPerSeg.concreteTypes.end())
        {
            if (ptrTypeInfo)
                *ptrTypeInfo = it->second.newRealType;
            if (result)
                *result = it->second.concreteType;
            *storage = it->second.storageOffset;

            // The registered type is the full version, so exit, and wait for the job to complete if necessary
            if (cflags & MAKE_CONCRETE_SHOULD_WAIT)
            {
                SWAG_ASSERT(context);
                if (context->baseJob->baseContext->result != ContextResult::Pending)
                {
                    auto itJob = mapPerSeg.concreteTypesJob.find(typeName);
                    if (itJob != mapPerSeg.concreteTypesJob.end())
                    {
                        itJob->second->addDependentJob(context->baseJob);
                        context->baseJob->setPending(nullptr, "MAKE_CONCRETE_SHOULD_WAIT", nullptr, typeInfo);
                    }
                }
            }

            return true;
        }
    }

    if (storageSegment->kind != SegmentKind::Compiler)
    {
        if (context && g_CommandLine.verboseConcreteTypes)
            g_Log.verbose(Utf8::format("%s %s\n", context->sourceFile->module->name.c_str(), typeName.c_str()));
        if (g_CommandLine.stats)
            g_Stats.totalConcreteTypes++;
    }

    auto& swagScope = g_Workspace.swagScope;

    TypeInfoStruct* typeStruct = nullptr;
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        typeStruct = swagScope.regTypeInfoNative;
        break;
    case TypeInfoKind::Pointer:
        typeStruct = swagScope.regTypeInfoPointer;
        break;
    case TypeInfoKind::Reference:
        typeStruct = swagScope.regTypeInfoReference;
        break;
    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
        typeStruct = swagScope.regTypeInfoStruct;
        break;
    case TypeInfoKind::Param:
        typeStruct = swagScope.regTypeInfoParam;
        break;
    case TypeInfoKind::FuncAttr:
    case TypeInfoKind::Lambda:
        typeStruct = swagScope.regTypeInfoFunc;
        break;
    case TypeInfoKind::Enum:
        typeStruct = swagScope.regTypeInfoEnum;
        break;
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
        typeStruct = swagScope.regTypeInfoVariadic;
        break;
    case TypeInfoKind::Array:
        typeStruct = swagScope.regTypeInfoArray;
        break;
    case TypeInfoKind::Slice:
        typeStruct = swagScope.regTypeInfoSlice;
        break;
    case TypeInfoKind::Generic:
    case TypeInfoKind::Code:
    case TypeInfoKind::NameAlias:
        typeStruct = swagScope.regTypeInfoGeneric;
        break;
    case TypeInfoKind::Alias:
        typeStruct = swagScope.regTypeInfoAlias;
        break;
    default:
    {
        auto node = context->node;
        context->report({node, Utf8::format(Msg0537, typeInfo->getDisplayName().c_str())});
        return false;
    }
    }

    // Build concrete structure content
    ConcreteTypeInfo* concreteTypeInfoValue;
    uint32_t          storageOffset = storageSegment->reserve(typeStruct->sizeOf, (uint8_t**) &concreteTypeInfoValue);

    SWAG_ASSERT(!typeName.empty());
    SWAG_CHECK(makeConcreteString(context, &concreteTypeInfoValue->name, typeName, storageSegment, OFFSETOF(concreteTypeInfoValue->name)));
    SWAG_CHECK(makeConcreteString(context, &concreteTypeInfoValue->flatName, typeInfo->getName(), storageSegment, OFFSETOF(concreteTypeInfoValue->flatName)));
    concreteTypeInfoValue->kind   = typeInfo->kind;
    concreteTypeInfoValue->sizeOf = typeInfo->sizeOf;

    // Setup useful flags
    concreteTypeInfoValue->flags = (uint16_t) TypeInfoFlags::None;
    if (typeInfo->isPointerToTypeInfo())
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::TypeInfoPtr;
    if (typeInfo->isNativeInteger())
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Integer;
    if (typeInfo->isNativeFloat())
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Float;
    if (typeInfo->isNativeIntegerUnsigned())
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Unsigned;
    if (typeInfo->isStrict())
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Strict;

    // True by default, will be removed by struct if necessary
    concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::CanCopy;

    // Register type and value
    // Do it now to break recursive references
    auto    typePtr = allocType<TypeInfoPointer>();
    MapType mapType;
    mapType.realType      = typeInfo;
    mapType.newRealType   = typePtr;
    mapType.concreteType  = concreteTypeInfoValue;
    mapType.storageOffset = storageOffset;
    if (result)
        *result = mapType.concreteType;

    mapPerSeg.concreteTypes[typeName]                     = mapType;
    mapPerSeg.concreteTypesReverse[concreteTypeInfoValue] = typeInfo;

    // Build pointer type to structure
    typePtr->flags |= TYPEINFO_CONST;
    typePtr->pointedType = typeStruct;
    typePtr->computeName();
    typePtr->sizeOf = sizeof(void*);

    // Register type and value
    if (ptrTypeInfo)
        *ptrTypeInfo = typePtr;
    *storage = storageOffset;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
    {
        auto concreteType        = (ConcreteTypeInfoNative*) concreteTypeInfoValue;
        concreteType->nativeKind = typeInfo->nativeType;
        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto concreteType = (ConcreteTypeInfoPointer*) concreteTypeInfoValue;
        auto realType     = (TypeInfoPointer*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->pointedType, concreteTypeInfoValue, storageSegment, storageOffset, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::Reference:
    {
        auto concreteType = (ConcreteTypeInfoReference*) concreteTypeInfoValue;
        auto realType     = (TypeInfoReference*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->pointedType, concreteTypeInfoValue, storageSegment, storageOffset, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::Alias:
    {
        auto concreteType = (ConcreteTypeInfoAlias*) concreteTypeInfoValue;
        auto realType     = (TypeInfoAlias*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->rawType, concreteTypeInfoValue, storageSegment, storageOffset, realType->rawType, cflags));
        break;
    }

    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    {
        SWAG_CHECK(makeConcreteStruct(context, typeName, concreteTypeInfoValue, typeInfo, storageSegment, storageOffset, cflags));
        break;
    }

    case TypeInfoKind::Lambda:
    case TypeInfoKind::FuncAttr:
    {
        auto concreteType = (ConcreteTypeInfoFunc*) concreteTypeInfoValue;
        auto realType     = (TypeInfoFuncAttr*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

        // Generics
        concreteType->generics.buffer = 0;
        concreteType->generics.count  = realType->genericParameters.size();
        if (concreteType->generics.count)
        {
            uint32_t count = (uint32_t) concreteType->generics.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) makeConcreteSlice(context, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->generics.buffer, storageArray);
            for (int param = 0; param < concreteType->generics.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageSegment, storageArray, realType->genericParameters[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        // Parameters
        concreteType->parameters.buffer = 0;
        concreteType->parameters.count  = realType->parameters.size();
        if (concreteType->parameters.count)
        {
            uint32_t count = (uint32_t) realType->parameters.size();
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) makeConcreteSlice(context, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->parameters.buffer, storageArray);
            for (int param = 0; param < concreteType->parameters.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageSegment, storageArray, realType->parameters[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->returnType, concreteTypeInfoValue, storageSegment, storageOffset, realType->returnType, cflags));
        break;
    }

    case TypeInfoKind::Enum:
    {
        auto concreteType = (ConcreteTypeInfoEnum*) concreteTypeInfoValue;
        auto realType     = (TypeInfoEnum*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

        concreteType->values.buffer = 0;
        concreteType->values.count  = realType->values.size();
        if (concreteType->values.count)
        {
            uint32_t count = (uint32_t) realType->values.size();
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) makeConcreteSlice(context, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->values.buffer, storageArray);
            for (int param = 0; param < concreteType->values.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageSegment, storageArray, realType->values[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        concreteType->rawType = 0;
        if (realType->rawType)
            SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->rawType, concreteTypeInfoValue, storageSegment, storageOffset, realType->rawType, cflags));
        break;
    }

    case TypeInfoKind::Array:
    {
        auto concreteType        = (ConcreteTypeInfoArray*) concreteTypeInfoValue;
        auto realType            = (TypeInfoArray*) typeInfo;
        concreteType->count      = realType->count;
        concreteType->totalCount = realType->totalCount;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->pointedType, concreteTypeInfoValue, storageSegment, storageOffset, realType->pointedType, cflags));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->finalType, concreteTypeInfoValue, storageSegment, storageOffset, realType->finalType, cflags));
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto concreteType = (ConcreteTypeInfoSlice*) concreteTypeInfoValue;
        auto realType     = (TypeInfoSlice*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->pointedType, concreteTypeInfoValue, storageSegment, storageOffset, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::TypedVariadic:
    {
        auto concreteType = (ConcreteTypeInfoVariadic*) concreteTypeInfoValue;
        auto realType     = (TypeInfoVariadic*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->rawType, concreteTypeInfoValue, storageSegment, storageOffset, realType->rawType, cflags));
        break;
    }
    }

    return true;
}

bool TypeTable::makeConcreteSubTypeInfo(JobContext* context, ConcreteTypeInfo** result, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfo* typeInfo, uint32_t cflags)
{
    if (!typeInfo)
    {
        *result = 0;
        return true;
    }

    uint32_t tmpStorageOffset;
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, result, typeInfo, storageSegment, &tmpStorageOffset, cflags));
    storageSegment->addInitPtr(OFFSETOFR(result), tmpStorageOffset);

    return true;
}

bool TypeTable::makeConcreteString(JobContext* context, SwagSlice* result, const Utf8& str, DataSegment* storageSegment, uint32_t offsetInBuffer)
{
    if (str.empty())
    {
        result->buffer = nullptr;
        result->count  = 0;
        return true;
    }

    auto offset = storageSegment->addString(str, (uint8_t**) &result->buffer);
    storageSegment->addInitPtr(offsetInBuffer, offset);
    SWAG_ASSERT(result->buffer);
    result->count = str.length();
    return true;
}

void* TypeTable::makeConcreteSlice(JobContext* context, uint32_t sizeOf, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray)
{
    uint8_t* addr;
    storageArray = storageSegment->reserve(sizeOf, &addr);

    // Offset for bytecode run
    *result = addr;

    // Offset for native
    storageSegment->addInitPtr(OFFSETOFR(result), storageArray);
    return addr;
}

void* TypeTable::makeConcreteSlice(JobContext* context, uint32_t sizeOf, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray)
{
    uint8_t* addr;
    storageArray = storageSegment->reserve(sizeOf, &addr);

    // Offset for bytecode run
    *result = addr;

    // Offset for native
    storageSegment->addInitPtr(storageOffset, storageArray);
    return addr;
}

bool TypeTable::makeConcreteAny(JobContext* context, ConcreteAny* ptrAny, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        auto storageOffsetValue = storageSegment->addComputedValue(sourceFile, typeInfo, computedValue, (uint8_t**) &ptrAny->value);
        storageSegment->addInitPtr(storageOffset, storageOffsetValue);
    }
    else
        ptrAny->value = nullptr;

    // Type of the value
    SWAG_CHECK(makeConcreteSubTypeInfo(context, &ptrAny->type, ptrAny, storageSegment, storageOffset, typeInfo, cflags));
    return true;
}

bool TypeTable::makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags)
{
    auto sourceFile   = context->sourceFile;
    auto concreteType = (ConcreteTypeInfoParam*) concreteTypeInfoValue;

    concreteType->offsetOf = realType->offset;

    SWAG_CHECK(makeConcreteString(context, &concreteType->name, realType->namedParam, storageSegment, OFFSETOF(concreteType->name)));
    SWAG_CHECK(makeConcreteSubTypeInfo(context, &concreteType->pointedType, concreteTypeInfoValue, storageSegment, storageOffset, realType->typeInfo, cflags));
    SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

    // Value
    if (realType->flags & TYPEINFO_DEFINED_VALUE)
    {
        if (realType->typeInfo->kind == TypeInfoKind::Array || realType->typeInfo->kind == TypeInfoKind::TypeListArray)
        {
            SWAG_ASSERT(realType->value.storageOffset != UINT32_MAX);
            SWAG_ASSERT(realType->value.storageSegment);
            concreteType->value = realType->value.storageSegment->address(realType->value.storageOffset);
            storageSegment->addInitPtr(OFFSETOF(concreteType->value), realType->value.storageOffset, SegmentKind::Constant);
        }
        else if (realType->typeInfo->kind == TypeInfoKind::Slice)
        {
            SWAG_ASSERT(realType->value.storageOffset != UINT32_MAX);
            SWAG_ASSERT(realType->value.storageSegment);

            // :SliceLiteral
            auto count         = realType->value.reg.u64;
            auto offsetContent = realType->value.storageOffset;

            uint64_t* ptrSlice;
            auto      offsetSlice = storageSegment->reserve(2 * sizeof(uint64_t), (uint8_t**) &ptrSlice);
            ptrSlice[0]           = (uint64_t) realType->value.storageSegment->address(offsetContent);
            ptrSlice[1]           = count;

            concreteType->value = ptrSlice;
            storageSegment->addInitPtr(offsetSlice, offsetContent, SegmentKind::Constant);
            storageSegment->addInitPtr(OFFSETOF(concreteType->value), offsetSlice);
        }
        else
        {
            auto storageOffsetValue = storageSegment->addComputedValue(sourceFile, realType->typeInfo, realType->value, (uint8_t**) &concreteType->value);
            storageSegment->addInitPtr(OFFSETOF(concreteType->value), storageOffsetValue);
        }
    }

    return true;
}

bool TypeTable::makeConcreteAttributes(JobContext* context, AttributeList& attributes, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, SwagSlice* result, uint32_t cflags)
{
    if (attributes.empty())
        return true;

    result->buffer = nullptr;
    result->count  = attributes.size();
    if (!result->count)
        return true;

    uint32_t count = (uint32_t) result->count;
    uint32_t storageOffsetAttributes;
    auto     ptrStorageAttributes = (uint8_t*) makeConcreteSlice(context, count * sizeof(ConcreteAttribute), concreteTypeInfoValue, storageSegment, storageOffset, &result->buffer, storageOffsetAttributes);

    uint32_t curOffsetAttributes = storageOffsetAttributes;
    for (auto& one : attributes.allAttributes)
    {
        // Name of the attribute
        auto ptrString = (SwagSlice*) ptrStorageAttributes;
        SWAG_CHECK(makeConcreteString(context, ptrString, one.name, storageSegment, curOffsetAttributes));
        curOffsetAttributes += sizeof(SwagSlice);
        ptrStorageAttributes += sizeof(SwagSlice);

        // Slice to all parameters
        auto ptrParamsAttribute    = (SwagSlice*) ptrStorageAttributes;
        ptrParamsAttribute->buffer = nullptr;
        ptrParamsAttribute->count  = one.parameters.size();

        // Parameters
        if (!one.parameters.empty())
        {
            count = (uint32_t) one.parameters.size();
            uint32_t storageOffsetParams;
            auto     ptrStorageAllParams = (uint8_t*) makeConcreteSlice(context, count * sizeof(ConcreteAttributeParameter), storageSegment, curOffsetAttributes, &ptrParamsAttribute->buffer, storageOffsetParams);

            uint32_t curOffsetParams = storageOffsetParams;
            for (auto& oneParam : one.parameters)
            {
                // Name of the parameter
                ptrString = (SwagSlice*) ptrStorageAllParams;
                SWAG_CHECK(makeConcreteString(context, ptrString, oneParam.name, storageSegment, curOffsetParams));
                curOffsetParams += sizeof(SwagSlice);
                ptrStorageAllParams += sizeof(SwagSlice);

                // Value of the parameter
                makeConcreteAny(context, (ConcreteAny*) ptrStorageAllParams, storageSegment, curOffsetParams, oneParam.value, oneParam.typeInfo, cflags);

                curOffsetParams += sizeof(ConcreteAny);
                ptrStorageAllParams += sizeof(ConcreteAny);
            }
        }

        // Next attribute (zap slice of all parameters)
        curOffsetAttributes += sizeof(SwagSlice);
        ptrStorageAttributes += sizeof(SwagSlice);
    }

    return true;
}

Utf8& TypeTable::getTypeName(TypeInfo* typeInfo, bool forceNoScope)
{
    if (forceNoScope)
        return typeInfo->name;
    if (!typeInfo->declNode)
        return typeInfo->scopedName;
    if (typeInfo->declNode->kind == AstNodeKind::TypeExpression || typeInfo->declNode->kind == AstNodeKind::TypeLambda)
        return typeInfo->name;
    return typeInfo->scopedName;
}

void TypeTable::tableJobDone(TypeTableJob* job, DataSegment* segment)
{
    auto& mapPerSeg = getMapPerSeg(segment);
    auto  it        = mapPerSeg.concreteTypesJob.find(job->typeName);
    SWAG_ASSERT(it != mapPerSeg.concreteTypesJob.end());
    mapPerSeg.concreteTypesJob.erase(it);
    for (auto it1 : job->patchMethods)
        segment->addPatchMethod(it1.first, it1.second);
}

TypeInfo* TypeTable::getRealType(DataSegment* segment, ConcreteTypeInfo* concreteType)
{
    auto&      mapPerSeg = getMapPerSeg(segment);
    SharedLock lk(mapPerSeg.mutex);
    auto       it = mapPerSeg.concreteTypesReverse.find(concreteType);
    if (it == mapPerSeg.concreteTypesReverse.end())
        return nullptr;
    return it->second;
}

void TypeTable::initFrom(Module* module, TypeTable* other)
{
    mapPerSegment[0].concreteTypes = other->mapPerSegment[0].concreteTypes;
    for (auto& it : mapPerSegment[0].concreteTypes)
    {
        it.second.concreteType                                        = (ConcreteTypeInfo*) module->constantSegment.address(it.second.storageOffset);
        mapPerSegment[0].concreteTypesReverse[it.second.concreteType] = it.second.realType;
    }

    mapPerSegment[1].concreteTypes = other->mapPerSegment[1].concreteTypes;
    for (auto& it : mapPerSegment[1].concreteTypes)
    {
        it.second.concreteType                                        = (ConcreteTypeInfo*) module->compilerSegment.address(it.second.storageOffset);
        mapPerSegment[1].concreteTypesReverse[it.second.concreteType] = it.second.realType;
    }
}
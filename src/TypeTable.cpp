#include "pch.h"
#include "TypeTable.h"
#include "SemanticJob.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "TypeTableJob.h"
#include "TypeManager.h"
#include "Module.h"
#include "ByteCode.h"
#include "ErrorIds.h"

DataSegment* TypeTable::getSegmentStorage(Module* module, uint32_t flags)
{
    if (flags & CONCRETE_FOR_COMPILER)
        return &module->compilerSegment;
    return &module->typeSegment;
}

bool TypeTable::makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo, uint32_t cflags)
{
    if (!typeInfo)
    {
        *result = nullptr;
        return true;
    }

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    TypeInfo* typePtr;
    uint32_t  tmpStorageOffset;
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, typeInfo, &typePtr, &tmpStorageOffset, cflags));
    *result = (ConcreteTypeInfo*) segment->addressNoLock(tmpStorageOffset);

    // We have a pointer in the type segment, so we need to register it for backend setup
    segment->addInitPtr(concreteTypeInfoValue ? OFFSETOF(*result) : storageOffset, tmpStorageOffset);
    return true;
}

bool TypeTable::makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, void** result, TypeInfo* typeInfo, uint32_t cflags)
{
    if (!typeInfo)
    {
        *result = 0;
        return true;
    }

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    TypeInfo* typePtr;
    uint32_t  tmpStorageOffset;
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, typeInfo, &typePtr, &tmpStorageOffset, cflags));

    // Offset for bytecode run
    auto ptr = segment->addressNoLock(tmpStorageOffset);
    *result  = ptr;

    // Offset for native
    segment->addInitPtr(OFFSETOFR(result), tmpStorageOffset);

    return true;
}

bool TypeTable::makeConcreteString(JobContext* context, SwagSlice* result, const Utf8& str, uint32_t offsetInBuffer, uint32_t cflags)
{
    if (str.empty())
    {
        result->buffer = nullptr;
        result->count  = 0;
        return true;
    }

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    auto offset = segment->addStringNoLock(str);
    segment->addInitPtr(offsetInBuffer, offset);
    result->buffer = segment->addressNoLock(offset);
    SWAG_ASSERT(result->buffer);
    result->count = str.length();
    return true;
}

void* TypeTable::makeConcreteSlice(JobContext* context, uint32_t sizeOf, void* concreteTypeInfoValue, uint32_t storageOffset, void** result, uint32_t cflags, uint32_t& storageArray)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    storageArray = segment->reserveNoLock(sizeOf);
    auto addr    = segment->addressNoLock(storageArray);

    // Offset for bytecode run
    *result = addr;

    // Offset for native
    segment->addInitPtr(OFFSETOFR(result), storageArray);
    return addr;
}

void* TypeTable::makeConcreteSlice(JobContext* context, uint32_t sizeOf, uint32_t storageOffset, void** result, uint32_t cflags, uint32_t& storageArray)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    storageArray = segment->reserveNoLock(sizeOf);
    auto addr    = segment->addressNoLock(storageArray);

    // Offset for bytecode run
    *result = addr;

    // Offset for native
    segment->addInitPtr(storageOffset, storageArray);
    return addr;
}

bool TypeTable::makeConcreteAny(JobContext* context, ConcreteAny* ptrAny, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        auto storageOffsetValue = segment->addComputedValueNoLock(sourceFile, typeInfo, computedValue);
        ptrAny->value           = segment->addressNoLock(storageOffsetValue);
        segment->addInitPtr(storageOffset, storageOffsetValue);
    }
    else
        ptrAny->value = nullptr;

    // Type of the value
    SWAG_CHECK(makeConcreteSubTypeInfo(context, nullptr, storageOffset + sizeof(void*), &ptrAny->type, typeInfo, cflags));
    return true;
}

bool TypeTable::makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    ConcreteTypeInfoParam* concreteType = (ConcreteTypeInfoParam*) concreteTypeInfoValue;

    concreteType->offsetOf = realType->offset;

    SWAG_CHECK(makeConcreteString(context, &concreteType->name, realType->namedParam, OFFSETOF(concreteType->name), cflags));
    SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->typeInfo, cflags));
    SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, concreteTypeInfoValue, storageOffset, &concreteType->attributes, cflags));

    // Value
    if (realType->flags & TYPEINFO_DEFINED_VALUE)
    {
        if (realType->typeInfo->kind == TypeInfoKind::Array || realType->typeInfo->kind == TypeInfoKind::TypeListArray)
        {
            concreteType->value = module->constantSegment.address(realType->value.storageOffset);
            segment->addInitPtr(OFFSETOF(concreteType->value), realType->value.storageOffset, SegmentKind::Constant);
        }
        else if (realType->typeInfo->kind == TypeInfoKind::Slice)
        {
            // :SliceLiteral
            auto count         = realType->value.reg.u64;
            auto offsetContent = realType->value.storageOffset;

            auto offsetSlice = segment->reserveNoLock(2 * sizeof(uint64_t));
            auto ptrSlice    = (uint64_t*) segment->addressNoLock(offsetSlice);
            ptrSlice[0]      = (uint64_t) module->constantSegment.address(offsetContent);
            ptrSlice[1]      = count;

            concreteType->value = ptrSlice;
            segment->addInitPtr(offsetSlice, offsetContent, SegmentKind::Constant);
            segment->addInitPtr(OFFSETOF(concreteType->value), offsetSlice);
        }
        else
        {
            auto storageOffsetValue = segment->addComputedValueNoLock(sourceFile, realType->typeInfo, realType->value);
            concreteType->value     = segment->addressNoLock(storageOffsetValue);
            segment->addInitPtr(OFFSETOF(concreteType->value), storageOffsetValue);
        }
    }

    return true;
}

bool TypeTable::makeConcreteAttributes(JobContext* context, SymbolAttributes& attributes, void* concreteTypeInfoValue, uint32_t storageOffset, SwagSlice* result, uint32_t cflags)
{
    if (attributes.empty())
        return true;

    result->buffer = nullptr;
    result->count  = attributes.size();
    if (!result->count)
        return true;

    uint32_t count = (uint32_t) result->count;
    uint32_t storageOffsetAttributes;
    auto     ptrStorageAttributes = (uint8_t*) makeConcreteSlice(context, count * sizeof(ConcreteAttribute), concreteTypeInfoValue, storageOffset, &result->buffer, cflags, storageOffsetAttributes);

    uint32_t curOffsetAttributes = storageOffsetAttributes;
    for (auto& one : attributes.attributes)
    {
        // Name of the attribute
        auto ptrString = (SwagSlice*) ptrStorageAttributes;
        SWAG_CHECK(makeConcreteString(context, ptrString, one.name, curOffsetAttributes, cflags));
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
            auto     ptrStorageAllParams = (uint8_t*) makeConcreteSlice(context, count * sizeof(ConcreteAttributeParameter), curOffsetAttributes, &ptrParamsAttribute->buffer, cflags, storageOffsetParams);

            uint32_t curOffsetParams = storageOffsetParams;
            for (auto& oneParam : one.parameters)
            {
                // Name of the parameter
                ptrString = (SwagSlice*) ptrStorageAllParams;
                SWAG_CHECK(makeConcreteString(context, ptrString, oneParam.name, curOffsetParams, cflags));
                curOffsetParams += sizeof(SwagSlice);
                ptrStorageAllParams += sizeof(SwagSlice);

                // Value of the parameter
                makeConcreteAny(context, (ConcreteAny*) ptrStorageAllParams, curOffsetParams, oneParam.value, oneParam.typeInfo, cflags);

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

bool TypeTable::makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    unique_lock lk(segment->mutex);
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, typeInfo, ptrTypeInfo, storage, cflags));
    return true;
}

bool TypeTable::makeConcreteTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, uint32_t cflags)
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

    if (!(cflags & CONCRETE_FORCE_NO_SCOPE))
    {
        typeInfo->computeScopedName();
        SWAG_ASSERT(!typeInfo->scopedName.empty());
    }

    auto& typeName = getTypeName(typeInfo, cflags & CONCRETE_FORCE_NO_SCOPE);
    SWAG_ASSERT(!typeName.empty());

    // Already computed ?
    auto& storedMap    = cflags & CONCRETE_FOR_COMPILER ? concreteTypesCompiler : concreteTypes;
    auto& storedMapJob = cflags & CONCRETE_FOR_COMPILER ? concreteTypesJobCompiler : concreteTypesJob;
    if (typeInfo->kind != TypeInfoKind::Param)
    {
        auto it = storedMap.find(typeName);
        if (it != storedMap.end())
        {
            if (ptrTypeInfo)
                *ptrTypeInfo = it->second.newRealType;
            *storage = it->second.storageOffset;

            // A compute job is pending, need to wait
            if (cflags & CONCRETE_SHOULD_WAIT)
            {
                if (context->baseJob->baseContext->result != ContextResult::Pending)
                {
                    auto itJob = storedMapJob.find(typeName);
                    if (itJob != storedMapJob.end())
                    {
                        itJob->second->addDependentJob(context->baseJob);
                        context->baseJob->setPending(nullptr, "CONCRETE_SHOULD_WAIT", nullptr, typeInfo);
                    }
                }
            }

            return true;
        }
    }

    if (!(cflags & CONCRETE_FOR_COMPILER))
    {
        if (g_CommandLine.verboseConcreteTypes)
            g_Log.verbose(format("%s %s\n", context->sourceFile->module->name.c_str(), typeName.c_str()));
        g_Stats.totalConcreteTypes++;
    }

    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getSegmentStorage(module, cflags);

    auto&           swagScope  = g_Workspace.swagScope;
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
        context->report({node, format(Msg0537, typeInfo->getDisplayName().c_str())});
        return false;
    }

    // Build concrete structure content
    uint32_t          storageOffset         = segment->reserveNoLock(typeStruct->sizeOf);
    ConcreteTypeInfo* concreteTypeInfoValue = (ConcreteTypeInfo*) segment->addressNoLock(storageOffset);

    SWAG_ASSERT(!typeName.empty());
    SWAG_CHECK(makeConcreteString(context, &concreteTypeInfoValue->name, typeName, OFFSETOF(concreteTypeInfoValue->name), cflags));
    SWAG_CHECK(makeConcreteString(context, &concreteTypeInfoValue->flatName, typeInfo->getName(), OFFSETOF(concreteTypeInfoValue->flatName), cflags));
    concreteTypeInfoValue->kind   = typeInfo->kind;
    concreteTypeInfoValue->sizeOf = typeInfo->sizeOf;

    // Setup useful flags
    concreteTypeInfoValue->flags = (uint16_t) TypeInfoFlags::None;
    if (typeInfo->isPointerToTypeInfo())
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::TypeInfoPtr;
    if (typeInfo->flags & TYPEINFO_INTEGER)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Integer;
    if (typeInfo->flags & TYPEINFO_FLOAT)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Float;
    if (typeInfo->flags & TYPEINFO_UNSIGNED)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Unsigned;
    if (typeInfo->flags & TYPEINFO_STRICT)
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
    storedMap[typeName]   = mapType;

    {
        unique_lock lk(lockReverse);
        concreteTypesReverse[concreteTypeInfoValue] = typeInfo;
    }

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
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::Reference:
    {
        auto concreteType = (ConcreteTypeInfoReference*) concreteTypeInfoValue;
        auto realType     = (TypeInfoReference*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::Alias:
    {
        auto concreteType = (ConcreteTypeInfoAlias*) concreteTypeInfoValue;
        auto realType     = (TypeInfoAlias*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->rawType, realType->rawType, cflags));
        break;
    }

    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    {
        auto job                   = g_Pool_typeTableJob.alloc();
        job->module                = module;
        job->typeTable             = this;
        job->sourceFile            = sourceFile;
        job->concreteTypeInfoValue = concreteTypeInfoValue;
        job->typeInfo              = typeInfo;
        job->storageOffset         = storageOffset;
        job->cflags                = cflags & ~CONCRETE_SHOULD_WAIT;
        job->typeName              = typeName;
        job->nodes.push_back(context->node);
        storedMapJob[typeName] = job;

        if (cflags & CONCRETE_SHOULD_WAIT)
        {
            SWAG_ASSERT(context->result == ContextResult::Done || !strcmp(context->baseJob->waitingId, "CONCRETE_SHOULD_WAIT"));
            job->dependentJob = context->baseJob;
            context->baseJob->setPending(nullptr, "CONCRETE_SHOULD_WAIT", nullptr, typeInfo);
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

        break;
    }

    case TypeInfoKind::Lambda:
    case TypeInfoKind::FuncAttr:
    {
        auto concreteType = (ConcreteTypeInfoFunc*) concreteTypeInfoValue;
        auto realType     = (TypeInfoFuncAttr*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, concreteTypeInfoValue, storageOffset, &concreteType->attributes, cflags));

        // Generics
        concreteType->generics.buffer = 0;
        concreteType->generics.count  = realType->genericParameters.size();
        if (concreteType->generics.count)
        {
            uint32_t count = (uint32_t) concreteType->generics.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) makeConcreteSlice(context, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageOffset, &concreteType->generics.buffer, cflags, storageArray);
            for (int param = 0; param < concreteType->generics.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageArray, realType->genericParameters[param], cflags));
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
            auto     addrArray = (ConcreteTypeInfoParam*) makeConcreteSlice(context, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageOffset, &concreteType->parameters.buffer, cflags, storageArray);
            for (int param = 0; param < concreteType->parameters.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageArray, realType->parameters[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->returnType, realType->returnType, cflags));
        break;
    }

    case TypeInfoKind::Enum:
    {
        auto concreteType = (ConcreteTypeInfoEnum*) concreteTypeInfoValue;
        auto realType     = (TypeInfoEnum*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, concreteTypeInfoValue, storageOffset, &concreteType->attributes, cflags));

        concreteType->values.buffer = 0;
        concreteType->values.count  = realType->values.size();
        if (concreteType->values.count)
        {
            uint32_t count = (uint32_t) realType->values.size();
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) makeConcreteSlice(context, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageOffset, &concreteType->values.buffer, cflags, storageArray);
            for (int param = 0; param < concreteType->values.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageArray, realType->values[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        concreteType->rawType = 0;
        if (realType->rawType)
            SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->rawType, realType->rawType, cflags));
        break;
    }

    case TypeInfoKind::Array:
    {
        auto concreteType        = (ConcreteTypeInfoArray*) concreteTypeInfoValue;
        auto realType            = (TypeInfoArray*) typeInfo;
        concreteType->count      = realType->count;
        concreteType->totalCount = realType->totalCount;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, cflags));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->finalType, realType->finalType, cflags));
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto concreteType = (ConcreteTypeInfoSlice*) concreteTypeInfoValue;
        auto realType     = (TypeInfoSlice*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::TypedVariadic:
    {
        auto concreteType = (ConcreteTypeInfoVariadic*) concreteTypeInfoValue;
        auto realType     = (TypeInfoVariadic*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->rawType, realType->rawType, cflags));
        break;
    }
    }

    return true;
}

void TypeTable::tableJobDone(TypeTableJob* job, DataSegment* segment)
{
    auto& storedMap = job->cflags & CONCRETE_FOR_COMPILER ? concreteTypesJobCompiler : concreteTypesJob;
    auto  it        = storedMap.find(job->typeName);
    SWAG_ASSERT(it != storedMap.end());
    storedMap.erase(it);
    for (auto it1 : job->patchMethods)
        segment->addPatchMethod(it1.first, it1.second);
}

TypeInfo* TypeTable::getRealType(ConcreteTypeInfo* concreteType)
{
    shared_lock lk(lockReverse);
    auto        it = concreteTypesReverse.find(concreteType);
    if (it == concreteTypesReverse.end())
        return nullptr;
    return it->second;
}
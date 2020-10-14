#include "pch.h"
#include "TypeTable.h"
#include "SemanticJob.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "TypeTableJob.h"
#include "TypeManager.h"
#include "Module.h"

DataSegment* TypeTable::getConstantSegment(Module* module, uint32_t flags)
{
    if (flags & CONCRETE_FOR_COMPILER)
        return &module->constantSegmentCompiler;
    return &module->typeSegment;
}

bool TypeTable::makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo, bool forceNoScope, uint32_t cflags)
{
    if (!typeInfo)
    {
        *result = nullptr;
        return true;
    }

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getConstantSegment(module, cflags);

    TypeInfo* typePtr;
    uint32_t  tmpStorageOffset;
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, typeInfo, &typePtr, &tmpStorageOffset, forceNoScope, cflags));
    *result = (ConcreteTypeInfo*) segment->addressNoLock(tmpStorageOffset);

    // We have a pointer in the constant segment, so we need to register it for backend setup
    segment->addInitPtr(concreteTypeInfoValue ? OFFSETOF(*result) : storageOffset, tmpStorageOffset);
    return true;
}

bool TypeTable::makeConcreteAny(JobContext* context, ConcreteAny* ptrAny, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getConstantSegment(module, cflags);

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        auto storageOffsetValue = segment->addComputedValueNoLock(sourceFile, typeInfo, computedValue);
        ptrAny->value           = segment->addressNoLock(storageOffsetValue);
        segment->addInitPtr(storageOffset, storageOffsetValue);
    }
    else
        ptrAny->value = nullptr;

    // Type of the value
    SWAG_CHECK(makeConcreteSubTypeInfo(context, nullptr, storageOffset + sizeof(void*), &ptrAny->type, typeInfo, false, cflags));
    return true;
}

bool TypeTable::makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getConstantSegment(module, cflags);

    ConcreteTypeInfoParam* concreteType = (ConcreteTypeInfoParam*) concreteTypeInfoValue;

    concreteType->offsetOf = realType->offset;

    SWAG_CHECK(makeConcreteString(context, &concreteType->name, realType->namedParam, OFFSETOF(concreteType->name), cflags));
    SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->typeInfo, false, cflags));
    SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes), cflags));

    // Value
    if (realType->flags & TYPEINFO_DEFINED_VALUE)
    {
        auto storageOffsetValue = segment->addComputedValueNoLock(sourceFile, realType->typeInfo, realType->value);
        concreteType->value     = segment->addressNoLock(storageOffsetValue);
        segment->addInitPtr(OFFSETOF(concreteType->value), storageOffsetValue);
    }

    return true;
}

bool TypeTable::makeConcreteAttributes(JobContext* context, SymbolAttributes& attributes, ConcreteSlice* result, uint32_t offset, uint32_t cflags)
{
    if (attributes.empty())
        return true;

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getConstantSegment(module, cflags);

    result->count = attributes.size();

    uint32_t storageOffsetAttributes = segment->reserveNoLock((uint32_t) result->count * sizeof(ConcreteAttribute));
    uint8_t* ptrStorageAttributes    = segment->addressNoLock(storageOffsetAttributes);
    result->buffer                   = ptrStorageAttributes;
    segment->addInitPtr(offset, storageOffsetAttributes);

    uint32_t curOffsetAttributes = storageOffsetAttributes;
    for (auto& one : attributes.attributes)
    {
        // Name of the attribute
        auto ptrString = (ConcreteSlice*) ptrStorageAttributes;
        SWAG_CHECK(makeConcreteString(context, ptrString, one.name, curOffsetAttributes, cflags));
        curOffsetAttributes += sizeof(ConcreteSlice);
        ptrStorageAttributes += sizeof(ConcreteSlice);

        // Slice to all parameters
        auto ptrParamsAttribute    = (ConcreteSlice*) ptrStorageAttributes;
        ptrParamsAttribute->buffer = nullptr;
        ptrParamsAttribute->count  = (uint32_t) one.parameters.size();

        // Parameters
        if (!one.parameters.empty())
        {
            // Slice for all parameters
            uint32_t storageOffsetParams = segment->reserveNoLock((uint32_t) one.parameters.size() * sizeof(ConcreteAttributeParameter));
            uint8_t* ptrStorageAllParams = segment->addressNoLock(storageOffsetParams);
            ptrParamsAttribute->buffer   = ptrStorageAllParams;
            segment->addInitPtr(curOffsetAttributes, storageOffsetParams);

            uint32_t curOffsetParams = storageOffsetParams;
            for (auto& oneParam : one.parameters)
            {
                // Name of the parameter
                ptrString = (ConcreteSlice*) ptrStorageAllParams;
                SWAG_CHECK(makeConcreteString(context, ptrString, oneParam.name, curOffsetParams, cflags));
                curOffsetParams += sizeof(ConcreteSlice);
                ptrStorageAllParams += sizeof(ConcreteSlice);

                // Value of the parameter
                makeConcreteAny(context, (ConcreteAny*) ptrStorageAllParams, curOffsetParams, oneParam.value, oneParam.typeInfo, cflags);

                curOffsetParams += sizeof(ConcreteAny);
                ptrStorageAllParams += sizeof(ConcreteAny);
            }
        }

        // Next attribute (zap slice of all parameters
        curOffsetAttributes += sizeof(ConcreteSlice);
        ptrStorageAttributes += sizeof(ConcreteSlice);
    }

    return true;
}

bool TypeTable::makeConcreteString(JobContext* context, ConcreteSlice* result, const Utf8& str, uint32_t offsetInBuffer, uint32_t cflags)
{
    if (str.empty())
    {
        result->buffer = nullptr;
        result->count  = 0;
        return true;
    }

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getConstantSegment(module, cflags);

    auto offset = segment->addStringNoLock(str);
    segment->addInitPtr(offsetInBuffer, offset);
    result->buffer = (void*) str.c_str();
    SWAG_ASSERT(result->buffer);
    result->count = str.length();
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
    auto segment    = getConstantSegment(module, cflags);

    unique_lock lk(segment->mutex);
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, typeInfo, ptrTypeInfo, storage, false, cflags));
    return true;
}

bool TypeTable::makeConcreteTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool forceNoScope, uint32_t cflags)
{
    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);
    switch (typeInfo->kind)
    {
    case TypeInfoKind::TypeListArray:
        typeInfo = TypeManager::convertTypeListToArray(context, (TypeInfoList*) typeInfo, true);
        break;
    case TypeInfoKind::TypeListTuple:
        typeInfo = TypeManager::convertTypeListToStruct(context, (TypeInfoList*) typeInfo, true);
        break;
    }

    if (!forceNoScope)
        typeInfo->computeScopedName();

    auto& typeName = getTypeName(typeInfo, forceNoScope);

    // Already computed ?
    auto& storedMap    = cflags & CONCRETE_FOR_COMPILER ? concreteTypesCompiler : concreteTypes;
    auto& storedMapJob = cflags & CONCRETE_FOR_COMPILER ? concreteTypesJobCompiler : concreteTypesJob;
    if (typeInfo->kind != TypeInfoKind::Param)
    {
        auto it = storedMap.find(typeName);
        if (it != storedMap.end())
        {
            if (ptrTypeInfo)
                *ptrTypeInfo = it->second.first;
            *storage = it->second.second;

            // A compute job is pending, need to wait
            if (cflags & CONCRETE_SHOULD_WAIT)
            {
                if (context->baseJob->baseContext->result != ContextResult::Pending)
                {
                    auto itJob = storedMapJob.find(typeName);
                    if (itJob != storedMapJob.end())
                    {
                        itJob->second->addDependentJob(context->baseJob);
                        context->baseJob->setPending(nullptr);
                    }
                }
            }

            return true;
        }
    }

    if (!(cflags & CONCRETE_FOR_COMPILER))
        g_Stats.totalConcreteTypes++;

    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto segment    = getConstantSegment(module, cflags);

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
    case TypeInfoKind::TypeSet:
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
        typeStruct = swagScope.regTypeInfoGeneric;
        break;
    default:
        context->report({node, format("cannot convert typeinfo '%s' to runtime typeinfo", typeInfo->name.c_str())});
        return false;
    }

    // Build concrete structure content
    uint32_t          storageOffset         = segment->reserveNoLock(typeStruct->sizeOf);
    ConcreteTypeInfo* concreteTypeInfoValue = (ConcreteTypeInfo*) segment->addressNoLock(storageOffset);

    SWAG_ASSERT(!typeName.empty());
    SWAG_CHECK(makeConcreteString(context, &concreteTypeInfoValue->name, typeName, OFFSETOF(concreteTypeInfoValue->name), cflags));
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

    // Register type and value
    // Do it now to break recursive references
    auto typePtr        = g_Allocator.alloc<TypeInfoPointer>();
    storedMap[typeName] = {typePtr, storageOffset};

    // Build pointer type to structure
    typePtr->flags |= TYPEINFO_CONST;
    typePtr->ptrCount    = 1;
    typePtr->finalType   = typeStruct;
    typePtr->pointedType = typePtr->finalType;
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
        auto concreteType      = (ConcreteTypeInfoPointer*) concreteTypeInfoValue;
        auto realType          = (TypeInfoPointer*) typeInfo;
        concreteType->ptrCount = realType->ptrCount;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->finalType, realType->finalType, false, cflags));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, false, cflags));
        break;
    }

    case TypeInfoKind::Reference:
    {
        auto concreteType = (ConcreteTypeInfoReference*) concreteTypeInfoValue;
        auto realType     = (TypeInfoReference*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, false, cflags));
        break;
    }

    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    case TypeInfoKind::TypeSet:
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
            job->dependentJob = context->baseJob;
            context->baseJob->setPending(nullptr);
            context->baseJob->jobsToAdd.push_back(job);
        }
        else
        {
            job->dependentJob = context->baseJob->dependentJob;
            g_ThreadMgr.addJob(job);
        }

        break;
    }

    case TypeInfoKind::Lambda:
    case TypeInfoKind::FuncAttr:
    {
        auto concreteType = (ConcreteTypeInfoFunc*) concreteTypeInfoValue;
        auto realType     = (TypeInfoFuncAttr*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes), cflags));

        // Generics
        concreteType->generics.buffer = nullptr;
        concreteType->generics.count = realType->genericParameters.size();
        if (concreteType->generics.count)
        {
            uint32_t               storageArray = segment->reserveNoLock((uint32_t)concreteType->generics.count * sizeof(ConcreteTypeInfoParam));
            ConcreteTypeInfoParam* addrArray = (ConcreteTypeInfoParam*)segment->addressNoLock(storageArray);
            concreteType->generics.buffer = addrArray;
            segment->addInitPtr(OFFSETOF(concreteType->generics.buffer), storageArray);
            for (int param = 0; param < concreteType->generics.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageArray, realType->genericParameters[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        // Parameters
        concreteType->parameters.buffer = nullptr;
        concreteType->parameters.count  = realType->parameters.size();
        if (concreteType->parameters.count)
        {
            uint32_t               storageArray = segment->reserveNoLock((uint32_t) realType->parameters.size() * sizeof(ConcreteTypeInfoParam));
            ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) segment->addressNoLock(storageArray);
            concreteType->parameters.buffer     = addrArray;
            segment->addInitPtr(OFFSETOF(concreteType->parameters.buffer), storageArray);
            for (int param = 0; param < concreteType->parameters.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageArray, realType->parameters[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->rawType, realType, true, cflags));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->returnType, realType->returnType, false, cflags));
        break;
    }

    case TypeInfoKind::Enum:
    {
        auto concreteType = (ConcreteTypeInfoEnum*) concreteTypeInfoValue;
        auto realType     = (TypeInfoEnum*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes), cflags));

        concreteType->values.buffer = nullptr;
        concreteType->values.count  = realType->values.size();
        if (concreteType->values.count)
        {
            uint32_t               storageArray = segment->reserveNoLock((uint32_t) realType->values.size() * sizeof(ConcreteTypeInfoParam));
            ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) segment->addressNoLock(storageArray);
            concreteType->values.buffer         = addrArray;
            segment->addInitPtr(OFFSETOF(concreteType->values.buffer), storageArray);
            for (int param = 0; param < concreteType->values.count; param++)
            {
                SWAG_CHECK(makeConcreteParam(context, addrArray + param, storageArray, realType->values[param], cflags));
                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }

        concreteType->rawType = nullptr;
        if (realType->rawType)
            SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->rawType, realType->rawType, false, cflags));
        break;
    }

    case TypeInfoKind::Array:
    {
        auto concreteType        = (ConcreteTypeInfoArray*) concreteTypeInfoValue;
        auto realType            = (TypeInfoArray*) typeInfo;
        concreteType->count      = realType->count;
        concreteType->totalCount = realType->totalCount;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, false, cflags));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->finalType, realType->finalType, false, cflags));
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto concreteType = (ConcreteTypeInfoSlice*) concreteTypeInfoValue;
        auto realType     = (TypeInfoSlice*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType, false, cflags));
        break;
    }
    }

    return true;
}

void TypeTable::tableJobDone(TypeTableJob* job)
{
    auto& storedMap = job->cflags & CONCRETE_FOR_COMPILER ? concreteTypesJobCompiler : concreteTypesJob;
    auto  it        = storedMap.find(job->typeName);
    SWAG_ASSERT(it != storedMap.end());
    storedMap.erase(it);
}
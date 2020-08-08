#include "pch.h"
#include "TypeTable.h"
#include "SemanticJob.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "TypeTableJob.h"
#include "TypeManager.h"

TypeTable::TypeTable()
{
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

    TypeInfo* typePtr;
    uint32_t  tmpStorageOffset;
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, typeInfo, &typePtr, &tmpStorageOffset, forceNoScope, cflags));
    *result = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(tmpStorageOffset);

    // We have a pointer in the constant segment, so we need to register it for backend setup
    module->constantSegment.addInitPtr(concreteTypeInfoValue ? OFFSETOF(*result) : storageOffset, tmpStorageOffset);
    return true;
}

bool TypeTable::makeConcreteAny(JobContext* context, ConcreteAny* ptrAny, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        auto storageOffsetValue = module->constantSegment.addComputedValueNoLock(sourceFile, typeInfo, computedValue);
        ptrAny->value           = module->constantSegment.addressNoLock(storageOffsetValue);
        module->constantSegment.addInitPtr(storageOffset, storageOffsetValue);
    }
    else
        ptrAny->value = nullptr;

    // Type of the value
    SWAG_CHECK(makeConcreteSubTypeInfo(context, nullptr, storageOffset + sizeof(void*), &ptrAny->type, typeInfo, false, cflags));
    return true;
}

bool TypeTable::makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags)
{
    auto                   sourceFile   = context->sourceFile;
    auto                   module       = sourceFile->module;
    ConcreteTypeInfoParam* concreteType = (ConcreteTypeInfoParam*) concreteTypeInfoValue;

    concreteType->offsetOf = realType->offset;

    SWAG_CHECK(makeConcreteString(context, &concreteType->name, realType->namedParam, OFFSETOF(concreteType->name), cflags));
    SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->typeInfo, false, cflags));
    SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes), cflags));

    // Value
    if (realType->flags & TYPEINFO_DEFINED_VALUE)
    {
        auto storageOffsetValue = module->constantSegment.addComputedValueNoLock(sourceFile, realType->typeInfo, realType->value);
        concreteType->value     = module->constantSegment.addressNoLock(storageOffsetValue);
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->value), storageOffsetValue);
    }

    return true;
}

bool TypeTable::makeConcreteAttributes(JobContext* context, SymbolAttributes& attributes, ConcreteSlice* result, uint32_t offset, uint32_t cflags)
{
    if (attributes.empty())
        return true;

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    result->count = attributes.size();

    uint32_t storageOffsetAttributes = module->constantSegment.reserveNoLock((uint32_t) result->count * sizeof(ConcreteAttribute));
    uint8_t* ptrStorageAttributes    = module->constantSegment.addressNoLock(storageOffsetAttributes);
    result->buffer                   = ptrStorageAttributes;
    module->constantSegment.addInitPtr(offset, storageOffsetAttributes);

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
            uint32_t storageOffsetParams = module->constantSegment.reserveNoLock((uint32_t) one.parameters.size() * sizeof(ConcreteAttributeParameter));
            uint8_t* ptrStorageAllParams = module->constantSegment.addressNoLock(storageOffsetParams);
            ptrParamsAttribute->buffer   = ptrStorageAllParams;
            module->constantSegment.addInitPtr(curOffsetAttributes, storageOffsetParams);

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
    auto offset     = module->constantSegment.addStringNoLock(str);
    module->constantSegment.addInitPtr(offsetInBuffer, offset);
    result->buffer = (void*) str.c_str();
    SWAG_ASSERT(result->buffer);
    result->count = str.length();
    return true;
}

bool TypeTable::makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, uint32_t cflags)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    unique_lock lk(mutexTypes);
    unique_lock lk1(module->constantSegment.mutex);
    SWAG_CHECK(makeConcreteTypeInfoNoLock(context, typeInfo, ptrTypeInfo, storage, false, cflags));
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

    // Already computed
    if (!forceNoScope)
        typeInfo->computeScopedName();
    if (typeInfo->kind != TypeInfoKind::Param)
    {
        auto it = concreteTypes.find(getTypeName(typeInfo, forceNoScope));
        if (it != concreteTypes.end())
        {
            *ptrTypeInfo = it->second.first;
            *storage     = it->second.second;
            return true;
        }
    }

    g_Stats.totalConcreteTypes++;

    auto            node       = context->node;
    auto            sourceFile = context->sourceFile;
    auto            module     = sourceFile->module;
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
        typeStruct = swagScope.regTypeInfoGeneric;
        break;
    default:
        context->report({node, format("cannot convert typeinfo '%s' to runtime typeinfo", typeInfo->name.c_str())});
        return false;
    }

    // Build concrete structure content
    uint32_t          storageOffset         = module->constantSegment.reserveNoLock(typeStruct->sizeOf);
    ConcreteTypeInfo* concreteTypeInfoValue = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(storageOffset);

    auto& typeName = getTypeName(typeInfo, forceNoScope);
    SWAG_ASSERT(!typeName.empty());
    SWAG_CHECK(makeConcreteString(context, &concreteTypeInfoValue->name, typeName, OFFSETOF(concreteTypeInfoValue->name), cflags));
    concreteTypeInfoValue->kind   = typeInfo->kind;
    concreteTypeInfoValue->sizeOf = typeInfo->sizeOf;

    // Register type and value
    // Do it now to break recursive references
    auto typePtr            = g_Allocator.alloc<TypeInfoPointer>();
    concreteTypes[typeName] = {typePtr, storageOffset};

    // Build pointer type to structure
    typePtr->flags |= TYPEINFO_CONST | TYPEINFO_TYPEINFO_PTR;
    typePtr->ptrCount    = 1;
    typePtr->finalType   = typeStruct;
    typePtr->pointedType = typePtr->finalType;
    typePtr->computeName();
    typePtr->sizeOf = sizeof(void*);

    // Register type and value
    *ptrTypeInfo = typePtr;
    *storage     = storageOffset;

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
    {
        auto job                   = g_Pool_typeTableJob.alloc();
        job->module                = module;
        job->typeTable             = this;
        job->sourceFile            = sourceFile;
        job->concreteTypeInfoValue = concreteTypeInfoValue;
        job->typeInfo              = typeInfo;
        job->storageOffset         = storageOffset;
        job->nodes.push_back(context->node);

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

        concreteType->parameters.buffer = nullptr;
        concreteType->parameters.count  = realType->parameters.size();
        if (concreteType->parameters.count)
        {
            uint32_t               storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->parameters.size() * sizeof(ConcreteTypeInfoParam));
            ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) module->constantSegment.addressNoLock(storageArray);
            concreteType->parameters.buffer     = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->parameters.buffer), storageArray);
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
            uint32_t               storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->values.size() * sizeof(ConcreteTypeInfoParam));
            ConcreteTypeInfoParam* addrArray    = (ConcreteTypeInfoParam*) module->constantSegment.addressNoLock(storageArray);
            concreteType->values.buffer         = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->values.buffer), storageArray);
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

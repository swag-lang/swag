#include "pch.h"
#include "TypeGen.h"
#include "Crc32.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Log.h"
#include "Module.h"
#include "ThreadManager.h"
#include "TypeGenStructJob.h"
#include "TypeManager.h"
#include "Workspace.h"

bool TypeGen::genExportedTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storage, uint32_t cflags, TypeInfo** ptrTypeInfo)
{
    auto&      mapPerSeg = getMapPerSeg(storageSegment);
    ScopedLock lk(mapPerSeg.mutex);
    return genExportedTypeInfoNoLock(context, nullptr, typeInfo, storageSegment, storage, cflags, ptrTypeInfo);
}

bool TypeGen::genExportedTypeInfoNoLock(JobContext*        context,
                                        ExportedTypeInfo** result,
                                        TypeInfo*          typeInfo,
                                        DataSegment*       storageSegment,
                                        uint32_t*          storage,
                                        uint32_t           cflags,
                                        TypeInfo**         ptrTypeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::TypeListArray:
        typeInfo = TypeManager::convertTypeListToArray(context, static_cast<TypeInfoList*>(typeInfo), true);
        break;
    case TypeInfoKind::TypeListTuple:
        typeInfo = TypeManager::convertTypeListToStruct(context, static_cast<TypeInfoList*>(typeInfo), true);
        break;
    default:
        break;
    }

    if (!(cflags & GEN_EXPORTED_TYPE_FORCE_NO_SCOPE))
    {
        typeInfo->computeScopedName();
        SWAG_ASSERT(!typeInfo->scopedName.empty());
    }

    auto       typeName           = typeInfo->getTypeName(cflags & GEN_EXPORTED_TYPE_FORCE_NO_SCOPE);
    const auto nonPartialTypeName = typeName;
    if (cflags & GEN_EXPORTED_TYPE_PARTIAL)
        typeName += "__partial";
    SWAG_ASSERT(!typeName.empty());

    auto& mapPerSeg = getMapPerSeg(storageSegment);

    // Already computed ?
    const auto it = mapPerSeg.exportedTypes.find(typeName);
    if (it != mapPerSeg.exportedTypes.end())
    {
        if (ptrTypeInfo)
            *ptrTypeInfo = it->second.newRealType;
        if (result)
            *result = it->second.exportedType;
        *storage = it->second.storageOffset;

        // The registered type is the full version, so exit, and wait for the job to complete if necessary
        if (cflags & GEN_EXPORTED_TYPE_SHOULD_WAIT)
        {
            SWAG_ASSERT(context);
            if (context->baseJob->baseContext->result != ContextResult::Pending)
            {
                const auto itJob = mapPerSeg.exportedTypesJob.find(typeName);
                if (itJob != mapPerSeg.exportedTypesJob.end())
                {
                    itJob->second->addDependentJob(context->baseJob);
                    context->baseJob->setPending(JobWaitKind::GenExportedType, nullptr, nullptr, typeInfo);
                }
            }
        }

        return true;
    }

    if (storageSegment->kind != SegmentKind::Compiler)
    {
        if (context && g_CommandLine.verboseConcreteTypes)
            g_Log.messageVerbose(FMT("%s %s\n", context->sourceFile->module->name.c_str(), typeName.c_str()));
#ifdef SWAG_STATS
        ++g_Stats.totalConcreteTypes;
#endif
    }

    const auto& swagScope = g_Workspace->swagScope;

    TypeInfoStruct* typeStruct = nullptr;
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        typeStruct = swagScope.regTypeInfoNative;
        break;
    case TypeInfoKind::Pointer:
        typeStruct = swagScope.regTypeInfoPointer;
        break;
    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
        typeStruct = swagScope.regTypeInfoStruct;
        break;
    case TypeInfoKind::FuncAttr:
    case TypeInfoKind::LambdaClosure:
        typeStruct = swagScope.regTypeInfoFunc;
        break;
    case TypeInfoKind::Enum:
        typeStruct = swagScope.regTypeInfoEnum;
        break;
    case TypeInfoKind::Variadic:
    case TypeInfoKind::TypedVariadic:
    case TypeInfoKind::CVariadic:
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
    case TypeInfoKind::Alias:
        typeStruct = swagScope.regTypeInfoAlias;
        break;
    case TypeInfoKind::Namespace:
        typeStruct = swagScope.regTypeInfoNamespace;
        break;
    default:
        return context->report({context->node, FMT(Err(Err0372), typeInfo->getDisplayNameC())});
    }

    // Build concrete structure content
    ExportedTypeInfo* exportedTypeInfoValue;
    const uint32_t    storageOffset = storageSegment->reserve(typeStruct->sizeOf, (uint8_t**) &exportedTypeInfoValue);

    SWAG_ASSERT(!typeName.empty());
    SWAG_CHECK(genExportedString(context, &exportedTypeInfoValue->fullName, nonPartialTypeName, storageSegment, OFFSET_OF(exportedTypeInfoValue->fullName)));
    SWAG_CHECK(genExportedString(context, &exportedTypeInfoValue->name, typeInfo->getName(), storageSegment, OFFSET_OF(exportedTypeInfoValue->name)));
    exportedTypeInfoValue->crc32 = Crc32::compute(static_cast<const uint8_t*>(exportedTypeInfoValue->fullName.buffer), static_cast<uint32_t>(exportedTypeInfoValue->fullName.count));

    if (typeInfo->hasFlag(TYPEINFO_FUNC_IS_ATTR))
        exportedTypeInfoValue->kind = TypeInfoKind::Attribute;
    else
        exportedTypeInfoValue->kind = typeInfo->kind;

    exportedTypeInfoValue->sizeOf = typeInfo->sizeOf;

    // Setup useful flags
    exportedTypeInfoValue->flags = static_cast<uint16_t>(ExportedTypeInfoFlags::None);
    if (typeInfo->isPointerToTypeInfo())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::PointerTypeInfo);
    if (typeInfo->isPointerRef())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::PointerRef);
    if (typeInfo->isPointerMoveRef())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::PointerMoveRef);
    if (typeInfo->isPointerArithmetic())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::PointerArithmetic);
    if (typeInfo->isCString())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::CString);
    if (typeInfo->isNativeInteger())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Integer);
    if (typeInfo->isNativeFloat())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Float);
    if (typeInfo->isNativeIntegerUnsigned())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Unsigned);
    if (typeInfo->isStrict())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Strict);
    if (typeInfo->isGeneric())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Generic);
    if (typeInfo->isTuple())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Tuple);
    if (typeInfo->isCharacter())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Character);
    if (typeInfo->isConst())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Const);

    // True by default, will be removed by struct if necessary
    exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::CanCopy);

    // Register type and value
    // Do it now to break recursive references
    const auto typePtr = makeType<TypeInfoPointer>();
    MapType    mapType;
    mapType.realType      = typeInfo;
    mapType.newRealType   = typePtr;
    mapType.exportedType  = exportedTypeInfoValue;
    mapType.storageOffset = storageOffset;
    if (result)
        *result = mapType.exportedType;

    mapPerSeg.exportedTypes[typeName]                     = mapType;
    mapPerSeg.exportedTypesReverse[exportedTypeInfoValue] = typeInfo;

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
        const auto concreteType  = (ExportedTypeInfoNative*) exportedTypeInfoValue;
        concreteType->nativeKind = typeInfo->nativeType;
        break;
    }

    case TypeInfoKind::Pointer:
    {
        const auto concreteType = (ExportedTypeInfoPointer*) exportedTypeInfoValue;
        const auto realType     = static_cast<TypeInfoPointer*>(typeInfo);
        SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::Alias:
    {
        const auto concreteType = (ExportedTypeInfoAlias*) exportedTypeInfoValue;
        const auto realType     = static_cast<TypeInfoAlias*>(typeInfo);
        SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->rawType, exportedTypeInfoValue, storageSegment, storageOffset, realType->rawType, cflags));
        break;
    }

    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    {
        SWAG_CHECK(genExportedStruct(context, typeName, exportedTypeInfoValue, typeInfo, storageSegment, storageOffset, cflags));
        break;
    }

    case TypeInfoKind::LambdaClosure:
    case TypeInfoKind::FuncAttr:
    {
        const auto concreteType = (ExportedTypeInfoFunc*) exportedTypeInfoValue;
        const auto realType     = static_cast<TypeInfoFuncAttr*>(typeInfo);

        SWAG_CHECK(genExportedAttributes(context, realType->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

        // Generics
        concreteType->generics.buffer = nullptr;
        concreteType->generics.count  = realType->genericParameters.size();
        if (concreteType->generics.count)
        {
            const uint32_t count = static_cast<uint32_t>(concreteType->generics.count);
            uint32_t       storageArray;
            const auto     addrArray = static_cast<ExportedTypeValue*>(genExportedSlice(context, count * sizeof(ExportedTypeValue), exportedTypeInfoValue, storageSegment, storageOffset,
                                                                                        &concreteType->generics.buffer, storageArray));
            for (size_t param = 0; param < concreteType->generics.count; param++)
            {
                SWAG_CHECK(genExportedTypeValue(context, addrArray + param, storageSegment, storageArray, realType->genericParameters[param], cflags));
                storageArray += sizeof(ExportedTypeValue);
            }
        }

        // Parameters
        concreteType->parameters.buffer = nullptr;
        concreteType->parameters.count  = realType->parameters.size();

        // Do not count the first generated parameter of a closure
        int firstParam = 0;
        if (realType->isClosure())
        {
            concreteType->parameters.count--;
            firstParam = 1;
        }

        if (concreteType->parameters.count)
        {
            const uint32_t count = static_cast<uint32_t>(realType->parameters.size());
            uint32_t       storageArray;
            const auto     addrArray = static_cast<ExportedTypeValue*>(genExportedSlice(context, count * sizeof(ExportedTypeValue), exportedTypeInfoValue, storageSegment, storageOffset,
                                                                                        &concreteType->parameters.buffer, storageArray));
            for (size_t param = firstParam; param < realType->parameters.size(); param++)
            {
                SWAG_CHECK(genExportedTypeValue(context, addrArray + param - firstParam, storageSegment, storageArray, realType->parameters[param], cflags));
                storageArray += sizeof(ExportedTypeValue);
            }
        }

        SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->returnType, exportedTypeInfoValue, storageSegment, storageOffset, realType->returnType, cflags));
        break;
    }

    case TypeInfoKind::Enum:
    {
        const auto concreteType = (ExportedTypeInfoEnum*) exportedTypeInfoValue;
        const auto realType     = static_cast<TypeInfoEnum*>(typeInfo);

        SWAG_CHECK(genExportedAttributes(context, realType->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

        concreteType->values.buffer = nullptr;
        concreteType->values.count  = realType->values.size();
        if (concreteType->values.count)
        {
            const uint32_t count = static_cast<uint32_t>(realType->values.size());
            uint32_t       storageArray;
            const auto     addrArray = static_cast<ExportedTypeValue*>(genExportedSlice(context, count * sizeof(ExportedTypeValue), exportedTypeInfoValue, storageSegment, storageOffset,
                                                                                        &concreteType->values.buffer, storageArray));
            for (size_t param = 0; param < concreteType->values.count; param++)
            {
                SWAG_CHECK(genExportedTypeValue(context, addrArray + param, storageSegment, storageArray, realType->values[param], cflags));
                storageArray += sizeof(ExportedTypeValue);
            }
        }

        concreteType->rawType = nullptr;
        if (realType->rawType)
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->rawType, exportedTypeInfoValue, storageSegment, storageOffset, realType->rawType, cflags));
        break;
    }

    case TypeInfoKind::Array:
    {
        const auto concreteType  = (ExportedTypeInfoArray*) exportedTypeInfoValue;
        const auto realType      = static_cast<TypeInfoArray*>(typeInfo);
        concreteType->count      = realType->count;
        concreteType->totalCount = realType->totalCount;
        SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, realType->pointedType, cflags));
        SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->finalType, exportedTypeInfoValue, storageSegment, storageOffset, realType->finalType, cflags));
        break;
    }

    case TypeInfoKind::Slice:
    {
        const auto concreteType = (ExportedTypeInfoSlice*) exportedTypeInfoValue;
        const auto realType     = static_cast<TypeInfoSlice*>(typeInfo);
        SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, realType->pointedType, cflags));
        break;
    }

    case TypeInfoKind::TypedVariadic:
    {
        const auto concreteType = (ExportedTypeInfoVariadic*) exportedTypeInfoValue;
        const auto realType     = static_cast<TypeInfoVariadic*>(typeInfo);
        SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->rawType, exportedTypeInfoValue, storageSegment, storageOffset, realType->rawType, cflags));
        break;
    }
    default:
        break;
    }

    return true;
}

bool TypeGen::genExportedSubTypeInfo(JobContext*        context,
                                     ExportedTypeInfo** result,
                                     void*              exportedTypeInfoValue,
                                     DataSegment*       storageSegment,
                                     uint32_t           storageOffset,
                                     TypeInfo*          typeInfo,
                                     uint32_t           cflags)
{
    if (!typeInfo)
    {
        *result = nullptr;
        return true;
    }

    uint32_t tmpStorageOffset;
    SWAG_CHECK(genExportedTypeInfoNoLock(context, result, typeInfo, storageSegment, &tmpStorageOffset, cflags));
    storageSegment->addInitPtr(OFFSET_OF_R(result), tmpStorageOffset);

    return true;
}

bool TypeGen::genExportedString(JobContext* context, SwagSlice* result, const Utf8& str, DataSegment* storageSegment, uint32_t offsetInBuffer)
{
    if (str.empty())
    {
        result->buffer = nullptr;
        result->count  = 0;
        return true;
    }

    const auto offset = storageSegment->addString(str, (uint8_t**) &result->buffer);
    storageSegment->addInitPtr(offsetInBuffer, offset);
    SWAG_ASSERT(result->buffer);
    result->count = str.length();
    return true;
}

void* TypeGen::genExportedSlice(JobContext*  context,
                                uint32_t     sizeOf,
                                void*        exportedTypeInfoValue,
                                DataSegment* storageSegment,
                                uint32_t     storageOffset,
                                void**       result,
                                uint32_t&    storageArray)
{
    uint8_t* addrDst;
    storageArray = storageSegment->reserve(sizeOf, &addrDst);

    // Offset for bytecode run
    *result = addrDst;

    // Offset for native
    storageSegment->addInitPtr(OFFSET_OF_R(result), storageArray);
    return addrDst;
}

void* TypeGen::genExportedSlice(JobContext* context, uint32_t sizeOf, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray)
{
    uint8_t* addrDst;
    storageArray = storageSegment->reserve(sizeOf, &addrDst);

    // Offset for bytecode run
    *result = addrDst;

    // Offset for native
    storageSegment->addInitPtr(storageOffset, storageArray);
    return addrDst;
}

bool TypeGen::genExportedAny(JobContext*    context,
                             SwagAny*       ptrAny,
                             DataSegment*   storageSegment,
                             uint32_t       storageOffset,
                             ComputedValue& computedValue,
                             TypeInfo*      typeInfo,
                             uint32_t       cflags)
{
    const auto sourceFile = context->sourceFile;
    ptrAny->value         = nullptr;

    if (typeInfo->isNative())
    {
        const auto storageOffsetValue = storageSegment->addComputedValue(sourceFile, typeInfo, computedValue, (uint8_t**) &ptrAny->value);
        storageSegment->addInitPtr(storageOffset, storageOffsetValue);
    }

    // Type of the value
    SWAG_CHECK(genExportedSubTypeInfo(context, &ptrAny->type, ptrAny, storageSegment, storageOffset, typeInfo, cflags));
    return true;
}

bool TypeGen::genExportedTypeValue(JobContext* context, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags)
{
    const auto sourceFile   = context->sourceFile;
    const auto concreteType = static_cast<ExportedTypeValue*>(exportedTypeInfoValue);

    concreteType->offsetOf = realType->offset;

    SWAG_CHECK(genExportedString(context, &concreteType->name, realType->name, storageSegment, OFFSET_OF(concreteType->name)));
    concreteType->crc32 = Crc32::compute(static_cast<const uint8_t*>(concreteType->name.buffer), static_cast<uint32_t>(concreteType->name.count));
    SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, realType->typeInfo, cflags));
    SWAG_CHECK(genExportedAttributes(context, realType->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

    // Value
    if (realType->flags & TYPEINFOPARAM_DEFINED_VALUE)
    {
        if (realType->typeInfo->isArray() || realType->typeInfo->isListArray() || realType->typeInfo->isSlice())
        {
            SWAG_ASSERT(realType->value);
            concreteType->value = realType->value->getStorageAddr();
            storageSegment->addInitPtr(OFFSET_OF(concreteType->value), realType->value->storageOffset, SegmentKind::Constant);
        }
        else
        {
            SWAG_ASSERT(realType->value);
            const auto storageOffsetValue = storageSegment->addComputedValue(sourceFile, realType->typeInfo, *realType->value, (uint8_t**) &concreteType->value);
            storageSegment->addInitPtr(OFFSET_OF(concreteType->value), storageOffsetValue);
        }
    }

    return true;
}

bool TypeGen::genExportedAttributes(JobContext*    context,
                                    AttributeList& attributes,
                                    void*          exportedTypeInfoValue,
                                    DataSegment*   storageSegment,
                                    uint32_t       storageOffset,
                                    SwagSlice*     result,
                                    uint32_t       cflags)
{
    if (attributes.empty())
        return true;

    result->buffer = nullptr;
    result->count  = attributes.size();
    if (!result->count)
        return true;

    uint32_t count = static_cast<uint32_t>(result->count);
    uint32_t storageOffsetAttributes;
    auto     ptrStorageAttributes = static_cast<uint8_t*>(genExportedSlice(context, count * sizeof(ExportedAttribute), exportedTypeInfoValue, storageSegment, storageOffset, &result->buffer,
                                                                           storageOffsetAttributes));

    uint32_t curOffsetAttributes = storageOffsetAttributes;
    for (auto& one : attributes.allAttributes)
    {
        const auto attrAddr        = reinterpret_cast<ExportedAttribute*>(ptrStorageAttributes);
        const auto offsetStartAttr = curOffsetAttributes;

        // Type of the attribute
        SWAG_CHECK(genExportedSubTypeInfo(context, reinterpret_cast<ExportedTypeInfo**>(ptrStorageAttributes), attrAddr, storageSegment, offsetStartAttr, one.typeFunc, cflags));
        curOffsetAttributes += sizeof(ExportedTypeInfo*);
        ptrStorageAttributes += sizeof(ExportedTypeInfo*);

        // Slice to all parameters
        const auto ptrParamsAttribute = reinterpret_cast<SwagSlice*>(ptrStorageAttributes);
        ptrParamsAttribute->buffer    = nullptr;
        ptrParamsAttribute->count     = one.parameters.size();

        // Parameters
        if (!one.parameters.empty())
        {
            count = static_cast<uint32_t>(one.parameters.size());
            uint32_t storageOffsetParams;
            auto     ptrStorageAllParams = static_cast<uint8_t*>(genExportedSlice(context, count * sizeof(ExportedAttributeParameter), storageSegment, curOffsetAttributes,
                                                                                  &ptrParamsAttribute->buffer, storageOffsetParams));

            uint32_t curOffsetParams = storageOffsetParams;
            uint32_t cptParam        = 0;
            for (auto& oneParam : one.parameters)
            {
                // Name of the parameter
                const auto ptrString = reinterpret_cast<SwagSlice*>(ptrStorageAllParams);
                SWAG_CHECK(genExportedString(context, ptrString, oneParam.token.text, storageSegment, curOffsetParams));
                curOffsetParams += sizeof(SwagSlice);
                ptrStorageAllParams += sizeof(SwagSlice);

                auto typeValue = oneParam.typeInfo;

                // This is a typed variadic
                if (one.typeFunc && one.typeFunc->hasFlag(TYPEINFO_TYPED_VARIADIC) && cptParam >= one.typeFunc->parameters.size() - 1)
                {
                    const auto typeVariadic = castTypeInfo<TypeInfoVariadic>(one.typeFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
                    typeValue               = typeVariadic->rawType;
                }

                // typeinfo
                if (typeValue->isPointerToTypeInfo())
                {
                    const auto addr      = oneParam.value.getStorageAddr();
                    auto&      mapPerSeg = getMapPerSeg(oneParam.value.storageSegment);
                    if (oneParam.value.storageSegment != storageSegment)
                        mapPerSeg.mutex.lock();
                    const auto it = mapPerSeg.exportedTypesReverse.find(static_cast<ExportedTypeInfo*>(addr));
                    SWAG_ASSERT(it != mapPerSeg.exportedTypesReverse.end());
                    typeValue = it->second;
                    if (oneParam.value.storageSegment != storageSegment)
                        mapPerSeg.mutex.unlock();
                }

                // Value of the parameter
                genExportedAny(context, reinterpret_cast<SwagAny*>(ptrStorageAllParams), storageSegment, curOffsetParams, oneParam.value, typeValue, cflags);

                curOffsetParams += sizeof(SwagAny);
                ptrStorageAllParams += sizeof(SwagAny);
                cptParam++;
            }
        }

        // Next attribute (zap slice of all parameters)
        curOffsetAttributes += sizeof(SwagSlice);
        ptrStorageAttributes += sizeof(SwagSlice);
    }

    return true;
}

void TypeGen::tableJobDone(const TypeGenStructJob* job, DataSegment* segment)
{
    auto&      mapPerSeg = getMapPerSeg(segment);
    const auto it        = mapPerSeg.exportedTypesJob.find(job->typeName);
    SWAG_ASSERT(it != mapPerSeg.exportedTypesJob.end());
    mapPerSeg.exportedTypesJob.erase(it);
    for (const auto it1 : job->patchMethods)
        segment->addPatchMethod(it1.first, it1.second);
}

TypeInfo* TypeGen::getRealType(const DataSegment* segment, const ExportedTypeInfo* concreteType)
{
    auto&      mapPerSeg = getMapPerSeg(segment);
    SharedLock lk(mapPerSeg.mutex);
    const auto it = mapPerSeg.exportedTypesReverse.find(concreteType);
    if (it == mapPerSeg.exportedTypesReverse.end())
        return nullptr;
    return it->second;
}

void TypeGen::initFrom(Module* module, TypeGen* other)
{
    if (mapPerSegment.count == 0)
        setup(name);

    mapPerSegment[0]->exportedTypes = other->mapPerSegment[0]->exportedTypes;
    for (auto& val : mapPerSegment[0]->exportedTypes | views::values)
    {
        val.exportedType                                         = reinterpret_cast<ExportedTypeInfo*>(module->constantSegment.address(val.storageOffset));
        mapPerSegment[0]->exportedTypesReverse[val.exportedType] = val.realType;
    }

    mapPerSegment[1]->exportedTypes = other->mapPerSegment[1]->exportedTypes;
    for (auto& val : mapPerSegment[1]->exportedTypes | views::values)
    {
        val.exportedType                                         = reinterpret_cast<ExportedTypeInfo*>(module->compilerSegment.address(val.storageOffset));
        mapPerSegment[1]->exportedTypesReverse[val.exportedType] = val.realType;
    }
}

void TypeGen::setup(const Utf8& moduleName)
{
    name = moduleName;
    mapPerSegment.set_size_clear(2 + g_ThreadMgr.numWorkers);
    for (uint32_t i      = 0; i < mapPerSegment.count; i++)
        mapPerSegment[i] = new MapPerSeg;
}

void TypeGen::release()
{
    for (const auto mps : mapPerSegment)
    {
        mps->exportedTypes.release();
        mps->exportedTypesJob.release();
        mps->exportedTypesReverse.release();
    }
}

TypeGen::MapPerSeg& TypeGen::getMapPerSeg(const DataSegment* segment)
{
    if (segment->kind != SegmentKind::Compiler)
        return *mapPerSegment[0];
    if (segment->compilerThreadIdx == UINT32_MAX)
        return *mapPerSegment[1];
    return *mapPerSegment[2 + segment->compilerThreadIdx];
}

bool TypeGen::genExportedStruct(const JobContext* context,
                                const Utf8&       typeName,
                                ExportedTypeInfo* exportedTypeInfoValue,
                                TypeInfo*         typeInfo,
                                DataSegment*      storageSegment,
                                uint32_t          storageOffset,
                                uint32_t          cflags)
{
    // If we are already waiting for a job to finish, then we must... wait, and not generate new jobs
    if (context->baseJob->waitingKind == JobWaitKind::GenExportedType && context->result != ContextResult::Done)
        return true;

    SWAG_ASSERT(context);
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;

    auto&      mapPerSeg       = getMapPerSeg(storageSegment);
    const auto job             = Allocator::alloc<TypeGenStructJob>();
    job->module                = module;
    job->typeGen               = this;
    job->sourceFile            = sourceFile;
    job->exportedTypeInfoValue = exportedTypeInfoValue;
    job->typeInfo              = typeInfo;
    job->storageOffset         = storageOffset;
    job->storageSegment        = storageSegment;
    job->affinity              = storageSegment->compilerThreadIdx;
    job->cflags                = cflags & ~GEN_EXPORTED_TYPE_SHOULD_WAIT;
    job->typeName              = typeName;
    job->nodes.push_back(context->node);
    mapPerSeg.exportedTypesJob[typeName] = job;

#ifdef SWAG_STATS
    if (storageSegment->kind != SegmentKind::Compiler)
        ++g_Stats.totalConcreteStructTypes;
#endif

    if (cflags & GEN_EXPORTED_TYPE_SHOULD_WAIT)
    {
        SWAG_ASSERT(context->result == ContextResult::Done || context->baseJob->waitingKind == JobWaitKind::GenExportedType1);
        job->dependentJob = context->baseJob;
        context->baseJob->setPending(JobWaitKind::GenExportedType1, nullptr, nullptr, typeInfo);
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

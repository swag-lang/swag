#include "pch.h"
#include "Semantic/Type/TypeGen.h"
#include "Core/Crc32.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeGenStructJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

bool TypeGen::genExportedTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* resultStorageOffset, GenExportFlags genFlags, TypeInfo** ptrTypeInfo, ExportedTypeInfo** resultPtr)
{
    auto&      mapPerSeg = getMapPerSeg(storageSegment);
    ScopedLock lk(mapPerSeg.mutex);
    return genExportedTypeInfoNoLock(context, typeInfo, storageSegment, resultStorageOffset, genFlags, ptrTypeInfo, resultPtr);
}

bool TypeGen::genExportedTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* resultStorageOffset, GenExportFlags genFlags, TypeInfo** ptrTypeInfo, ExportedTypeInfo** resultPtr)
{
    switch (typeInfo->kind)
    {
        case TypeInfoKind::TypeListArray:
            typeInfo = TypeManager::convertTypeListToArray(context, castTypeInfo<TypeInfoList>(typeInfo), true);
            break;
        case TypeInfoKind::TypeListTuple:
            typeInfo = TypeManager::convertTypeListToStruct(context, castTypeInfo<TypeInfoList>(typeInfo));
            break;
    }

    auto       typeName            = typeInfo->getScopedName();
    const auto undecoratedTypeName = typeName;
    SWAG_ASSERT(!typeName.empty());

    if (typeInfo->flags.has(TYPEINFO_NULLABLE))
        typeName += ".nullable";
    if (genFlags.has(GEN_EXPORTED_TYPE_PARTIAL))
        typeName += ".partial";

    SWAG_ASSERT(!typeName.empty());

    auto& mapPerSeg = getMapPerSeg(storageSegment);

    // Already computed?
    const auto it = mapPerSeg.exportedTypes.find(typeName);
    if (it != mapPerSeg.exportedTypes.end())
    {
        if (ptrTypeInfo)
            *ptrTypeInfo = it->second.newRealType;
        if (resultPtr)
            *resultPtr = it->second.exportedType;
        *resultStorageOffset = it->second.storageOffset;

#ifdef SWAG_DEV_MODE
        bool ok = false;
        if (it->second.realType->kind == typeInfo->kind)
            ok = true;
        else if (it->second.realType->kind == TypeInfoKind::FuncAttr && typeInfo->kind == TypeInfoKind::LambdaClosure)
            ok = true;
        else if (it->second.realType->kind == TypeInfoKind::LambdaClosure && typeInfo->kind == TypeInfoKind::FuncAttr)
            ok = true;
        SWAG_ASSERT(ok);
#endif

        // The registered type is the full version, so exit, and wait for the job to complete if necessary
        if (genFlags.has(GEN_EXPORTED_TYPE_SHOULD_WAIT))
        {
            SWAG_ASSERT(context);
            if (context->baseJob->baseContext->result != ContextResult::Pending)
            {
                const auto itJob = mapPerSeg.exportedTypesJob.find(typeName);
                if (itJob != mapPerSeg.exportedTypesJob.end())
                {
                    itJob->second->addDependentJobOnce(context->baseJob);
                    context->baseJob->setPending(JobWaitKind::GenExportedType, nullptr, nullptr, typeInfo);
                }
            }
        }

        return true;
    }

    if (storageSegment->kind != SegmentKind::Compiler)
    {
        if (context && g_CommandLine.verboseConcreteTypes)
            g_Log.messageVerbose(form("%s %s\n", context->sourceFile->module->name.cstr(), typeName.cstr()));
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
            typeStruct = swagScope.regTypeInfoGeneric;
            break;
        case TypeInfoKind::CodeBlock:
            typeStruct = swagScope.regTypeInfoCodeBlock;
            break;
        case TypeInfoKind::Alias:
            typeStruct = swagScope.regTypeInfoAlias;
            break;
        case TypeInfoKind::Namespace:
            typeStruct = swagScope.regTypeInfoNamespace;
            break;
        default:
            return context->report({context->node, formErr(Err0746, typeInfo->getDisplayNameC())});
    }

    // Build concrete structure content
    ExportedTypeInfo* exportedTypeInfoValue;
    const uint32_t    storageOffset = storageSegment->reserve(typeStruct->sizeOf, reinterpret_cast<uint8_t**>(&exportedTypeInfoValue));

    SWAG_ASSERT(!typeName.empty());
    SWAG_CHECK(genExportedString(context, &exportedTypeInfoValue->fullName, undecoratedTypeName, storageSegment, OFFSET_OF(exportedTypeInfoValue->fullName)));
    SWAG_CHECK(genExportedString(context, &exportedTypeInfoValue->name, typeInfo->getName(), storageSegment, OFFSET_OF(exportedTypeInfoValue->name)));
    exportedTypeInfoValue->crc = Crc32::compute(static_cast<const uint8_t*>(exportedTypeInfoValue->fullName.buffer), static_cast<uint32_t>(exportedTypeInfoValue->fullName.count));

    if (typeInfo->hasFlag(TYPEINFO_FUNC_IS_ATTR))
        exportedTypeInfoValue->kind = TypeInfoKind::Attribute;
    else if (typeInfo->isLambdaClosure())
        exportedTypeInfoValue->kind = TypeInfoKind::FuncAttr;
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
    if (typeInfo->isNullable())
        exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::Nullable);

    // True by default. Will be removed by struct if necessary
    exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::CanCopy);

    // Register type and value
    // Do it now to break recursive references
    const auto typePtr = makeType<TypeInfoPointer>();
    MapType    mapType;
    mapType.realType      = typeInfo;
    mapType.newRealType   = typePtr;
    mapType.exportedType  = exportedTypeInfoValue;
    mapType.storageOffset = storageOffset;
    if (resultPtr)
    {
        SWAG_FORCE_ASSERT(*resultPtr == nullptr);
        *resultPtr = mapType.exportedType;
    }

    SWAG_RACE_CONDITION_WRITE_TYPEGEN(mapPerSeg.raceC);
    mapPerSeg.exportedTypes[typeName]                     = mapType;
    mapPerSeg.exportedTypesReverse[exportedTypeInfoValue] = typeInfo;

    // Build the pointer type to the structure
    typePtr->addFlag(TYPEINFO_CONST);
    typePtr->pointedType = typeStruct;
    typePtr->computeName();
    typePtr->sizeOf = sizeof(void*);

    // Register type and value
    if (ptrTypeInfo)
        *ptrTypeInfo = typePtr;
    *resultStorageOffset = storageOffset;

    switch (typeInfo->kind)
    {
        case TypeInfoKind::Native:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoNative));
            const auto concreteType  = reinterpret_cast<ExportedTypeInfoNative*>(exportedTypeInfoValue);
            concreteType->nativeKind = typeInfo->nativeType;
            break;
        }

        case TypeInfoKind::Pointer:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoPointer));
            const auto concreteType = reinterpret_cast<ExportedTypeInfoPointer*>(exportedTypeInfoValue);
            const auto realType     = castTypeInfo<TypeInfoPointer>(typeInfo);
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, realType->pointedType, genFlags));
            break;
        }

        case TypeInfoKind::Alias:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoAlias));
            const auto concreteType = reinterpret_cast<ExportedTypeInfoAlias*>(exportedTypeInfoValue);
            const auto realType     = castTypeInfo<TypeInfoAlias>(typeInfo);
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->rawType, exportedTypeInfoValue, storageSegment, storageOffset, realType->rawType, genFlags));
            break;
        }

        case TypeInfoKind::Struct:
        case TypeInfoKind::Interface:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoStruct));
            SWAG_CHECK(genExportedStruct(context, typeName, exportedTypeInfoValue, typeInfo, storageSegment, storageOffset, genFlags));
            break;
        }

        case TypeInfoKind::LambdaClosure:
        case TypeInfoKind::FuncAttr:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoFunc));
            const auto concreteType = reinterpret_cast<ExportedTypeInfoFunc*>(exportedTypeInfoValue);
            const auto realType     = castTypeInfo<TypeInfoFuncAttr>(typeInfo);

            SWAG_CHECK(genExportedAttributes(context, realType->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, genFlags));

            // Generics
            concreteType->generics.buffer = nullptr;
            concreteType->generics.count  = realType->genericParameters.size();
            if (concreteType->generics.count)
            {
                uint32_t   storageArray;
                const auto count     = static_cast<uint32_t>(concreteType->generics.count * sizeof(ExportedTypeValue));
                const auto genSlice  = genExportedSlice(context, count, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->generics.buffer, storageArray);
                const auto addrArray = static_cast<ExportedTypeValue*>(genSlice);
                for (uint32_t param = 0; param < concreteType->generics.count; param++)
                {
                    SWAG_CHECK(genExportedTypeValue(context, addrArray + param, storageSegment, storageArray, realType->genericParameters[param], genFlags));
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
                uint32_t   storageArray;
                const auto count     = static_cast<uint32_t>(realType->parameters.size() * sizeof(ExportedTypeValue));
                const auto genSlice  = genExportedSlice(context, count, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->parameters.buffer, storageArray);
                const auto addrArray = static_cast<ExportedTypeValue*>(genSlice);
                for (uint32_t param = firstParam; param < realType->parameters.size(); param++)
                {
                    SWAG_CHECK(genExportedTypeValue(context, addrArray + param - firstParam, storageSegment, storageArray, realType->parameters[param], genFlags));
                    storageArray += sizeof(ExportedTypeValue);
                }
            }

            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->returnType, exportedTypeInfoValue, storageSegment, storageOffset, realType->returnType, genFlags));
            break;
        }

        case TypeInfoKind::Enum:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoEnum));
            const auto concreteType = reinterpret_cast<ExportedTypeInfoEnum*>(exportedTypeInfoValue);
            const auto realType     = castTypeInfo<TypeInfoEnum>(typeInfo);

            SWAG_CHECK(genExportedAttributes(context, realType->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, genFlags));

            concreteType->values.buffer = nullptr;
            concreteType->values.count  = realType->values.size();
            if (concreteType->values.count)
            {
                uint32_t   storageArray;
                const auto count     = static_cast<uint32_t>(realType->values.size() * sizeof(ExportedTypeValue));
                const auto genSlice  = genExportedSlice(context, count, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->values.buffer, storageArray);
                const auto addrArray = static_cast<ExportedTypeValue*>(genSlice);
                for (uint32_t param = 0; param < concreteType->values.count; param++)
                {
                    SWAG_CHECK(genExportedTypeValue(context, addrArray + param, storageSegment, storageArray, realType->values[param], genFlags));
                    storageArray += sizeof(ExportedTypeValue);
                }
            }

            concreteType->rawType = nullptr;
            if (realType->rawType)
                SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->rawType, exportedTypeInfoValue, storageSegment, storageOffset, realType->rawType, genFlags));
            break;
        }

        case TypeInfoKind::Array:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoArray));
            const auto concreteType  = reinterpret_cast<ExportedTypeInfoArray*>(exportedTypeInfoValue);
            const auto realType      = castTypeInfo<TypeInfoArray>(typeInfo);
            concreteType->count      = realType->count;
            concreteType->totalCount = realType->totalCount;
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, realType->pointedType, genFlags));
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->finalType, exportedTypeInfoValue, storageSegment, storageOffset, realType->finalType, genFlags));
            break;
        }

        case TypeInfoKind::Slice:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoSlice));
            const auto concreteType = reinterpret_cast<ExportedTypeInfoSlice*>(exportedTypeInfoValue);
            const auto realType     = castTypeInfo<TypeInfoSlice>(typeInfo);
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, realType->pointedType, genFlags));
            break;
        }

        case TypeInfoKind::TypedVariadic:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoVariadic));
            const auto concreteType = reinterpret_cast<ExportedTypeInfoVariadic*>(exportedTypeInfoValue);
            const auto realType     = castTypeInfo<TypeInfoVariadic>(typeInfo);
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->rawType, exportedTypeInfoValue, storageSegment, storageOffset, realType->rawType, genFlags));
            break;
        }

        case TypeInfoKind::CodeBlock:
        {
            SWAG_ASSERT(typeStruct->sizeOf >= sizeof(ExportedTypeInfoCode));
            const auto concreteType = reinterpret_cast<ExportedTypeInfoCode*>(exportedTypeInfoValue);
            const auto realType     = castTypeInfo<TypeInfoCodeBlock>(typeInfo);
            SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->rawType, exportedTypeInfoValue, storageSegment, storageOffset, realType->rawType, genFlags));
            break;
        }
    }

    return true;
}

bool TypeGen::genExportedSubTypeInfo(JobContext* context, ExportedTypeInfo** resultPtr, void* exportedValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfo* typeInfo, GenExportFlags genFlags)
{
    if (!typeInfo)
    {
        *resultPtr = nullptr;
        return true;
    }

    uint32_t tmpStorageOffset;
    SWAG_CHECK(genExportedTypeInfoNoLock(context, typeInfo, storageSegment, &tmpStorageOffset, genFlags, nullptr, resultPtr));
    storageSegment->addInitPtr(OFFSET_OF_R(resultPtr), tmpStorageOffset);

    return true;
}

bool TypeGen::genExportedString(JobContext*, SwagSlice* result, const Utf8& str, DataSegment* storageSegment, uint32_t offsetInBuffer)
{
    if (str.empty())
    {
        result->buffer = nullptr;
        result->count  = 0;
        return true;
    }

    const auto offset = storageSegment->addString(str, reinterpret_cast<uint8_t**>(&result->buffer));
    storageSegment->addInitPtr(offsetInBuffer, offset);
    SWAG_ASSERT(result->buffer);
    result->count = str.length();
    return true;
}

void* TypeGen::genExportedSlice(JobContext*, uint32_t sizeOf, void* exportedValue, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray)
{
    uint8_t* addrDst;
    storageArray = storageSegment->reserve(sizeOf, &addrDst);

    // Offset for bytecode run
    *result = addrDst;

    // Offset for native
    storageSegment->addInitPtr(OFFSET_OF_R(result), storageArray);
    return addrDst;
}

void* TypeGen::genExportedSlice(JobContext*, uint32_t sizeOf, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray)
{
    uint8_t* addrDst;
    storageArray = storageSegment->reserve(sizeOf, &addrDst);

    // Offset for bytecode run
    *result = addrDst;

    // Offset for native
    storageSegment->addInitPtr(storageOffset, storageArray);
    return addrDst;
}

bool TypeGen::genExportedAny(JobContext* context, SwagAny* ptrAny, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, GenExportFlags genFlags)
{
    ptrAny->value = nullptr;

    if (typeInfo->isNative())
    {
        const auto storageOffsetValue = storageSegment->addComputedValue(typeInfo, computedValue, reinterpret_cast<uint8_t**>(&ptrAny->value));
        storageSegment->addInitPtr(storageOffset, storageOffsetValue);
    }

    // Type of the value
    SWAG_CHECK(genExportedSubTypeInfo(context, &ptrAny->type, ptrAny, storageSegment, storageOffset, typeInfo, genFlags));
    return true;
}

bool TypeGen::genExportedTypeValue(JobContext* context, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* param, GenExportFlags genFlags, uint32_t offset)
{
    const auto concreteType = static_cast<ExportedTypeValue*>(exportedTypeInfoValue);

    if (offset != UINT32_MAX)
        concreteType->offsetOf = offset;
    else
        concreteType->offsetOf = param->offset;

    SWAG_CHECK(genExportedString(context, &concreteType->name, param->name, storageSegment, OFFSET_OF(concreteType->name)));
    concreteType->crc = Crc32::compute(static_cast<const uint8_t*>(concreteType->name.buffer), static_cast<uint32_t>(concreteType->name.count));
    SWAG_CHECK(genExportedSubTypeInfo(context, &concreteType->pointedType, exportedTypeInfoValue, storageSegment, storageOffset, param->typeInfo, genFlags));
    SWAG_CHECK(genExportedAttributes(context, param->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, genFlags));

    // Value
    if (param->flags.has(TYPEINFOPARAM_DEFINED_VALUE))
    {
        if (param->typeInfo->isArray() || param->typeInfo->isListArray() || param->typeInfo->isSlice())
        {
            SWAG_ASSERT(param->value);
            concreteType->value = param->value->getStorageAddr();
            storageSegment->addInitPtr(OFFSET_OF(concreteType->value), param->value->storageOffset, SegmentKind::Constant);
        }
        else
        {
            SWAG_ASSERT(param->value);
            const auto storageOffsetValue = storageSegment->addComputedValue(param->typeInfo, *param->value, reinterpret_cast<uint8_t**>(&concreteType->value));
            storageSegment->addInitPtr(OFFSET_OF(concreteType->value), storageOffsetValue);
        }
    }

    // Flags
    if (param->flags.has(TYPEINFOPARAM_AUTO_NAME))
        concreteType->flags |= static_cast<uint32_t>(ExportedTypeValueFlags::AutoName);
    if (param->flags.has(TYPEINFOPARAM_HAS_USING))
        concreteType->flags |= static_cast<uint32_t>(ExportedTypeValueFlags::HasUsing);

    return true;
}

bool TypeGen::genExportedAttributes(JobContext* context, AttributeList& attributes, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, SwagSlice* result, GenExportFlags genFlags)
{
    if (attributes.empty())
        return true;

    result->buffer = nullptr;
    result->count  = attributes.size();
    if (!result->count)
        return true;

    uint32_t storageOffsetAttributes;
    auto     count                = static_cast<uint32_t>(result->count * sizeof(ExportedAttribute));
    auto     genSlice             = genExportedSlice(context, count, exportedTypeInfoValue, storageSegment, storageOffset, &result->buffer, storageOffsetAttributes);
    auto     ptrStorageAttributes = static_cast<uint8_t*>(genSlice);
    uint32_t curOffsetAttributes  = storageOffsetAttributes;
    for (auto& one : attributes.allAttributes)
    {
        const auto attrAddr        = reinterpret_cast<ExportedAttribute*>(ptrStorageAttributes);
        const auto offsetStartAttr = curOffsetAttributes;

        // Type of the attribute
        SWAG_CHECK(genExportedSubTypeInfo(context, reinterpret_cast<ExportedTypeInfo**>(ptrStorageAttributes), attrAddr, storageSegment, offsetStartAttr, one.type, genFlags));
        curOffsetAttributes += sizeof(ExportedTypeInfo*);
        ptrStorageAttributes += sizeof(ExportedTypeInfo*);

        // Slice to all parameters
        const auto ptrParamsAttribute = reinterpret_cast<SwagSlice*>(ptrStorageAttributes);
        ptrParamsAttribute->buffer    = nullptr;
        ptrParamsAttribute->count     = one.parameters.size();

        // Parameters
        if (!one.parameters.empty())
        {
            uint32_t storageOffsetParams;
            count                        = one.parameters.size() * sizeof(ExportedAttributeParameter);
            genSlice                     = genExportedSlice(context, count, storageSegment, curOffsetAttributes, &ptrParamsAttribute->buffer, storageOffsetParams);
            auto     ptrStorageAllParams = static_cast<uint8_t*>(genSlice);
            uint32_t curOffsetParams     = storageOffsetParams;
            uint32_t cptParam            = 0;
            for (auto& oneParam : one.parameters)
            {
                // Name of the parameter
                const auto ptrString = reinterpret_cast<SwagSlice*>(ptrStorageAllParams);
                SWAG_CHECK(genExportedString(context, ptrString, oneParam.token.text, storageSegment, curOffsetParams));
                curOffsetParams += sizeof(SwagSlice);
                ptrStorageAllParams += sizeof(SwagSlice);

                auto typeValue = oneParam.typeInfo;

                // This is a typed variadic
                if (one.type && one.type->hasFlag(TYPEINFO_TYPED_VARIADIC) && cptParam >= one.type->parameters.size() - 1)
                {
                    const auto typeVariadic = castTypeInfo<TypeInfoVariadic>(one.type->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
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
                genExportedAny(context, reinterpret_cast<SwagAny*>(ptrStorageAllParams), storageSegment, curOffsetParams, oneParam.value, typeValue, genFlags);

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
    auto& mapPerSeg = getMapPerSeg(segment);

    SWAG_RACE_CONDITION_WRITE_TYPEGEN(mapPerSeg.raceC);
    const auto it = mapPerSeg.exportedTypesJob.find(job->typeName);
    SWAG_ASSERT(it != mapPerSeg.exportedTypesJob.end());
    mapPerSeg.exportedTypesJob.erase(it);
    for (const auto it1 : job->patchMethods)
        segment->addPatchMethod(it1.first, it1.second);
}

TypeInfo* TypeGen::getRealType(const DataSegment* segment, const ExportedTypeInfo* concreteType)
{
    auto& mapPerSeg = getMapPerSeg(segment);

    SharedLock lk(mapPerSeg.mutex);
    SWAG_RACE_CONDITION_READ_TYPEGEN(mapPerSeg.raceC);
    const auto it = mapPerSeg.exportedTypesReverse.find(concreteType);
    if (it == mapPerSeg.exportedTypesReverse.end())
        return nullptr;
    return it->second;
}

void TypeGen::initFrom(Module* module, TypeGen* other)
{
    if (mapPerSegment.count == 0)
        setup(name);

    {
        SWAG_RACE_CONDITION_WRITE_TYPEGEN(mapPerSegment[0]->raceC);
        SWAG_RACE_CONDITION_READ1_TYPEGEN(other->mapPerSegment[0]->raceC);

        mapPerSegment[0]->exportedTypes = other->mapPerSegment[0]->exportedTypes;
        for (auto& val : mapPerSegment[0]->exportedTypes | std::views::values)
        {
            val.exportedType                                         = reinterpret_cast<ExportedTypeInfo*>(module->constantSegment.address(val.storageOffset));
            mapPerSegment[0]->exportedTypesReverse[val.exportedType] = val.realType;
        }
    }

    {
        SWAG_RACE_CONDITION_WRITE_TYPEGEN(mapPerSegment[1]->raceC);
        SWAG_RACE_CONDITION_READ1_TYPEGEN(other->mapPerSegment[1]->raceC);

        mapPerSegment[1]->exportedTypes = other->mapPerSegment[1]->exportedTypes;
        for (auto& val : mapPerSegment[1]->exportedTypes | std::views::values)
        {
            val.exportedType                                         = reinterpret_cast<ExportedTypeInfo*>(module->compilerSegment.address(val.storageOffset));
            mapPerSegment[1]->exportedTypesReverse[val.exportedType] = val.realType;
        }
    }
}

void TypeGen::setup(const Utf8& moduleName)
{
    name = moduleName;
    mapPerSegment.set_size_clear(2 + g_ThreadMgr.numWorkers);
    for (uint32_t i = 0; i < mapPerSegment.count; i++)
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
                                GenExportFlags    genFlags)
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
    job->genFlags              = genFlags.maskInvert(GEN_EXPORTED_TYPE_SHOULD_WAIT);
    job->typeName              = typeName;
    job->nodes.push_back(context->node);
    mapPerSeg.exportedTypesJob[typeName] = job;

#ifdef SWAG_STATS
    if (storageSegment->kind != SegmentKind::Compiler)
        ++g_Stats.totalConcreteStructTypes;
#endif

    if (genFlags.has(GEN_EXPORTED_TYPE_SHOULD_WAIT))
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

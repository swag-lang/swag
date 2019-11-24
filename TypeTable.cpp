#include "pch.h"
#include "TypeTable.h"
#include "SemanticJob.h"
#include "Workspace.h"

TypeTable::TypeTable()
{
    concreteList.hereFlag      = TYPEINFO_IN_CONCRETE_LIST;
    concreteList.sameFlags     = ISSAME_EXACT;
    concreteList.releaseIfHere = false;
    concreteList.registerInit();
}

struct ConcreteStringSlice
{
    void*    buffer;
    uint64_t count;
};

struct ConcreteTypeInfo
{
    ConcreteStringSlice name;
    TypeInfoKind        kind;
    uint32_t            sizeOf;
};

struct ConcreteAny
{
    void*             value;
    ConcreteTypeInfo* type;
};

struct ConcreteTypeInfoNative
{
    ConcreteTypeInfo base;
    NativeTypeKind   nativeKind;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* finalType;
    ConcreteTypeInfo* pointedType;
    uint32_t          ptrCount;
};

struct ConcreteTypeInfoParam
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice namedParam;
    ConcreteTypeInfo*   pointedType;
    void*               value;
    ConcreteStringSlice attributes;
    uint32_t            offsetOf;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice fields;
    ConcreteStringSlice attributes;
    ConcreteStringSlice interfaces;
};

struct ConcreteTypeInfoFunc
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice parameters;
    ConcreteTypeInfo*   returnType;
    ConcreteStringSlice attributes;
};

struct ConcreteTypeInfoEnum
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice values;
    ConcreteTypeInfo*   rawType;
    ConcreteStringSlice attributes;
};

struct ConcreteTypeInfoArray
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
    ConcreteTypeInfo* finalType;
    uint32_t          count;
    uint32_t          totalCount;
};

struct ConcreteTypeInfoSlice
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))

bool TypeTable::makeConcreteSubTypeInfo(SemanticContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo)
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
    SWAG_CHECK(makeConcreteTypeInfo(context, typeInfo, &typePtr, &tmpStorageOffset, false));
    *result = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(tmpStorageOffset);

    // We have a pointer in the constant segment, so we need to register it for backend setup
    module->constantSegment.addInitPtr(concreteTypeInfoValue ? OFFSETOF(*result) : storageOffset, tmpStorageOffset);
    return true;
}

bool TypeTable::makeConcreteAttributes(SemanticContext* context, SymbolAttributes& attributes, ConcreteStringSlice* result, uint32_t offset)
{
    if (attributes.values.size() == 0)
        return true;

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    result->count = (uint32_t) attributes.values.size();

    uint32_t storageOffset = module->constantSegment.reserveNoLock((uint32_t)(result->count * (sizeof(ConcreteStringSlice) + sizeof(ConcreteAny))));
    uint8_t* ptr           = module->constantSegment.addressNoLock(storageOffset);
    result->buffer         = ptr;
    module->constantSegment.addInitPtr(offset, storageOffset);

    uint32_t curOffset = storageOffset;
    for (auto& one : attributes.values)
    {
        auto ptrString = (ConcreteStringSlice*) ptr;
        SWAG_CHECK(makeConcreteString(context, ptrString, one.first, curOffset));
        curOffset += sizeof(ConcreteStringSlice);
        ptr += sizeof(ConcreteStringSlice);

        auto ptrAny        = (ConcreteAny*) ptr;
        auto typeAttribute = one.second.first;
        if (typeAttribute->kind == TypeInfoKind::Native)
        {
            auto storageOffsetValue = module->constantSegment.addComputedValueNoLock(sourceFile, typeAttribute, one.second.second);
            ptrAny->value           = module->constantSegment.addressNoLock(storageOffsetValue);
            module->constantSegment.addInitPtr(curOffset, storageOffsetValue);
        }
        else
            ptrAny->value = nullptr;

        SWAG_CHECK(makeConcreteSubTypeInfo(context, nullptr, curOffset + sizeof(void*), &ptrAny->type, typeAttribute));
        curOffset += sizeof(ConcreteAny);
        ptr += sizeof(ConcreteAny);
    }

    return true;
}

bool TypeTable::makeConcreteString(SemanticContext* context, ConcreteStringSlice* result, const Utf8& str, uint32_t offsetInBuffer)
{
    if (str.empty())
    {
        result->buffer = nullptr;
        result->count  = 0;
        return true;
    }

    auto sourceFile  = context->sourceFile;
    auto module      = sourceFile->module;
    auto stringIndex = module->reserveString(str);
    module->constantSegment.addInitString(offsetInBuffer, stringIndex);
    result->buffer = (void*) str.c_str();
    result->count  = str.size();
    return true;
}

bool TypeTable::makeConcreteTypeInfo(SemanticContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool lock)
{
    if (typeInfo->kind != TypeInfoKind::Param)
        typeInfo = concreteList.registerType(typeInfo);

    // Already computed
    if (lock)
        mutexTypes.lock();
    auto it = concreteTypes.find(typeInfo);
    if (it != concreteTypes.end())
    {
        *ptrTypeInfo = it->second.first;
        *storage     = it->second.second;
        if (lock)
            mutexTypes.unlock();
        return true;
    }

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
        typeStruct = swagScope.regTypeInfoVariadic;
        break;
    case TypeInfoKind::Array:
        typeStruct = swagScope.regTypeInfoArray;
        break;
    case TypeInfoKind::Slice:
        typeStruct = swagScope.regTypeInfoSlice;
        break;
    default:
        context->report({node, format("cannot convert typeinfo '%s' to runtime typeinfo", typeInfo->name.c_str())});
        return false;
    }

    if (lock)
        module->constantSegment.mutex.lock();

    // Build concrete structure content
    uint32_t          storageOffset         = module->constantSegment.reserveNoLock(typeStruct->sizeOf);
    ConcreteTypeInfo* concreteTypeInfoValue = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(storageOffset);

    SWAG_CHECK(makeConcreteString(context, &concreteTypeInfoValue->name, typeInfo->name, OFFSETOF(concreteTypeInfoValue->name)));
    concreteTypeInfoValue->kind   = typeInfo->kind;
    concreteTypeInfoValue->sizeOf = typeInfo->sizeOf;

    // Register type and value
	// Do it now to break recursive references 
    auto typePtr            = g_Pool_typeInfoPointer.alloc();
    concreteTypes[typeInfo] = {typePtr, storageOffset};

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
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->finalType, realType->finalType));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType));
        break;
    }
    case TypeInfoKind::Param:
    {
        auto concreteType = (ConcreteTypeInfoParam*) concreteTypeInfoValue;
        auto realType     = (TypeInfoParam*) typeInfo;

        concreteType->offsetOf = realType->offset;
        SWAG_CHECK(makeConcreteString(context, &concreteType->namedParam, realType->namedParam, OFFSETOF(concreteType->namedParam)));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->typeInfo));
        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes)));

        // Value
        if (realType->flags & TYPEINFO_DEFINED_VALUE)
        {
            concreteType->value = nullptr;
            if (realType->typeInfo->isNative(NativeTypeKind::String))
            {
                auto tmpStorageOffset = module->constantSegment.reserveNoLock(sizeof(ConcreteStringSlice));
                concreteType->value   = module->constantSegment.addressNoLock(tmpStorageOffset);
                module->constantSegment.addInitPtr(OFFSETOF(concreteType->value), tmpStorageOffset);
                SWAG_CHECK(makeConcreteString(context, (ConcreteStringSlice*) concreteType->value, realType->value.text, tmpStorageOffset));
            }
            else
            {
                auto tmpStorageOffset = module->constantSegment.reserveNoLock(realType->typeInfo->sizeOf);
                concreteType->value   = module->constantSegment.addressNoLock(tmpStorageOffset);
                module->constantSegment.addInitPtr(OFFSETOF(concreteType->value), tmpStorageOffset);
                switch (realType->typeInfo->sizeOf)
                {
                case 1:
                    *(uint8_t*) concreteType->value = realType->value.reg.u8;
                    break;
                case 2:
                    *(uint16_t*) concreteType->value = realType->value.reg.u16;
                    break;
                case 4:
                    *(uint32_t*) concreteType->value = realType->value.reg.u32;
                    break;
                case 8:
                    *(uint64_t*) concreteType->value = realType->value.reg.u64;
                    break;
                }
            }
        }

        break;
    }
    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    {
        auto concreteType = (ConcreteTypeInfoStruct*) concreteTypeInfoValue;
        auto realType     = (TypeInfoStruct*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes)));

        concreteType->fields.buffer = nullptr;
        concreteType->fields.count  = realType->childs.size();
        if (concreteType->fields.count)
        {
            uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->childs.size() * sizeof(void*));
            ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
            concreteType->fields.buffer     = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->fields.buffer), storageArray);
            for (int field = 0; field < concreteType->fields.count; field++)
                SWAG_CHECK(makeConcreteSubTypeInfo(context, addrArray, storageArray, addrArray + field, realType->childs[field]));
        }

        concreteType->interfaces.buffer = nullptr;
        concreteType->interfaces.count  = realType->interfaces.size();
        if (concreteType->interfaces.count)
        {
            uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->interfaces.size() * sizeof(void*));
            ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
            concreteType->interfaces.buffer = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->interfaces.buffer), storageArray);
            for (int field = 0; field < concreteType->interfaces.count; field++)
            {
                auto typeItf = realType->interfaces[field];
                SWAG_CHECK(makeConcreteSubTypeInfo(context, addrArray, storageArray, addrArray + field, typeItf));
            }
        }

        break;
    }
    case TypeInfoKind::Lambda:
    case TypeInfoKind::FuncAttr:
    {
        auto concreteType = (ConcreteTypeInfoFunc*) concreteTypeInfoValue;
        auto realType     = (TypeInfoFuncAttr*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes)));
        concreteType->parameters.buffer = nullptr;
        concreteType->parameters.count  = realType->parameters.size();
        if (concreteType->parameters.count)
        {
            uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->parameters.size() * sizeof(void*));
            ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
            concreteType->parameters.buffer = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->parameters.buffer), storageArray);
            for (int param = 0; param < concreteType->parameters.count; param++)
                SWAG_CHECK(makeConcreteSubTypeInfo(context, addrArray, storageArray, addrArray + param, realType->parameters[param]));
        }

        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->returnType, realType->returnType));
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto concreteType = (ConcreteTypeInfoEnum*) concreteTypeInfoValue;
        auto realType     = (TypeInfoEnum*) typeInfo;

        SWAG_CHECK(makeConcreteAttributes(context, realType->attributes, &concreteType->attributes, OFFSETOF(concreteType->attributes)));
        concreteType->values.buffer = nullptr;
        concreteType->values.count  = realType->values.size();
        if (concreteType->values.count)
        {
            uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->values.size() * sizeof(void*));
            ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
            concreteType->values.buffer     = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->values.buffer), storageArray);
            for (int param = 0; param < concreteType->values.count; param++)
                SWAG_CHECK(makeConcreteSubTypeInfo(context, addrArray, storageArray, addrArray + param, realType->values[param]));
        }

        concreteType->rawType = nullptr;
        if (realType->rawType)
            SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->rawType, realType->rawType));
        break;
    }

    case TypeInfoKind::Array:
    {
        auto concreteType        = (ConcreteTypeInfoArray*) concreteTypeInfoValue;
        auto realType            = (TypeInfoArray*) typeInfo;
        concreteType->count      = realType->count;
        concreteType->totalCount = realType->totalCount;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType));
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->finalType, realType->finalType));
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto concreteType = (ConcreteTypeInfoSlice*) concreteTypeInfoValue;
        auto realType     = (TypeInfoSlice*) typeInfo;
        SWAG_CHECK(makeConcreteSubTypeInfo(context, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType));
        break;
    }
    }

    if (lock)
        module->constantSegment.mutex.unlock();

    // Build pointer type to structure
    typePtr->flags |= TYPEINFO_CONST;
    typePtr->ptrCount    = 1;
    typePtr->finalType   = ((TypeInfoParam*) typeStruct->childs[0])->typeInfo; // Always returns the TypeInfo* pointer, not the typed one
    typePtr->pointedType = typePtr->finalType;
    typePtr->computeName();
    typePtr->sizeOf = sizeof(void*);

    // Register type and value
    *ptrTypeInfo = typePtr;
    *storage     = storageOffset;

    if (lock)
        mutexTypes.unlock();

    return true;
}
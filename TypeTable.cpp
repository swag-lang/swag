#include "pch.h"
#include "TypeTable.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "Module.h"
#include "Workspace.h"

TypeInfo* TypeTable::registerType(TypeInfo* newTypeInfo)
{
    scoped_lock lk(mutexTypes);
    for (auto typeInfo : allTypes)
    {
        if (typeInfo->isSame(newTypeInfo, ISSAME_EXACT))
        {
            if ((newTypeInfo != typeInfo) && !(newTypeInfo->flags & TYPEINFO_IN_MANAGER))
            {
                newTypeInfo->release();
            }

            return typeInfo;
        }
    }

    newTypeInfo->flags |= TYPEINFO_IN_MANAGER;
    allTypes.push_back(newTypeInfo);
    return newTypeInfo;
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

struct ConcreteTypeInfoNative
{
    ConcreteTypeInfo base;
    NativeTypeKind   nativeKind;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
    uint32_t          ptrCount;
};

struct ConcreteTypeInfoParam
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice namedParam;
    ConcreteTypeInfo*   pointedType;
    void*               value;
    uint32_t            offsetOf;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice fields;
};

struct ConcreteTypeInfoFunc
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice parameters;
    ConcreteTypeInfo*   returnType;
};

struct ConcreteTypeInfoEnum
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice values;
    ConcreteTypeInfo*   rawType;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))

bool TypeTable::makeConcreteSubTypeInfo(ErrorContext* errorContext, AstNode* node, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo)
{
    auto sourceFile = errorContext->sourceFile;
    auto module     = sourceFile->module;

    TypeInfo* typePtr;
    uint32_t  tmpStorageOffset;
    SWAG_CHECK(makeConcreteTypeInfo(errorContext, node, typeInfo, &typePtr, &tmpStorageOffset, false));
    *result = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(tmpStorageOffset);

    // We have a pointer in the constant segment, so we need to register it for backend setup
    module->constantSegment.addInitPtr(OFFSETOF(*result), tmpStorageOffset);
    return true;
}

bool TypeTable::makeConcreteTypeInfo(ErrorContext* errorContext, AstNode* node, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool lock)
{
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

    auto            sourceFile = errorContext->sourceFile;
    auto            module     = sourceFile->module;
    auto&           swagScope  = module->workspace->swagScope;
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
        typeStruct = swagScope.regTypeInfoStruct;
        break;
    case TypeInfoKind::Param:
        typeStruct = swagScope.regTypeInfoParam;
        break;
    case TypeInfoKind::FuncAttr:
        typeStruct = swagScope.regTypeInfoFunc;
        break;
    case TypeInfoKind::Enum:
        typeStruct = swagScope.regTypeInfoEnum;
        break;
    default:
        errorContext->report({sourceFile, node, format("cannot convert typeinfo '%s' to runtime typeinfo", typeInfo->name.c_str())});
        return false;
    }

    if (lock)
        module->constantSegment.mutex.lock();

    // Build structure content
    uint32_t          storageOffset         = module->constantSegment.reserveNoLock(typeStruct->sizeOf);
    ConcreteTypeInfo* concreteTypeInfoValue = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(storageOffset);

    auto stringIndex = module->reserveString(typeInfo->name);
    module->constantSegment.addInitString(OFFSETOF(concreteTypeInfoValue->name), stringIndex);

    concreteTypeInfoValue->name.buffer = (void*) typeInfo->name.c_str();
    concreteTypeInfoValue->name.count  = typeInfo->name.size();
    concreteTypeInfoValue->kind        = typeInfo->kind;
    concreteTypeInfoValue->sizeOf      = typeInfo->sizeOf;

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
        SWAG_CHECK(makeConcreteSubTypeInfo(errorContext, node, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->pointedType));
        break;
    }
    case TypeInfoKind::Param:
    {
        auto concreteType      = (ConcreteTypeInfoParam*) concreteTypeInfoValue;
        auto realType          = (TypeInfoParam*) typeInfo;
        concreteType->offsetOf = realType->offset;

        if (!realType->namedParam.empty())
        {
            stringIndex = module->reserveString(realType->namedParam);
            module->constantSegment.addInitString(OFFSETOF(concreteType->namedParam), stringIndex);
            concreteType->namedParam.buffer = (void*) realType->namedParam.c_str();
            concreteType->namedParam.count  = realType->namedParam.size();
        }

        SWAG_CHECK(makeConcreteSubTypeInfo(errorContext, node, concreteTypeInfoValue, storageOffset, &concreteType->pointedType, realType->typeInfo));

        // Value
        if (realType->flags & TYPEINFO_DEFINED_VALUE)
        {
            concreteType->value = nullptr;
            if (realType->typeInfo->isNative(NativeTypeKind::String))
            {
                auto tmpStorageOffset = module->constantSegment.reserveNoLock(2 * sizeof(void*));
                stringIndex           = module->reserveString(realType->value.text);
                module->constantSegment.addInitString(tmpStorageOffset, stringIndex);

                concreteType->value                  = module->constantSegment.addressNoLock(tmpStorageOffset);
                *(const char**) concreteType->value  = realType->value.text.c_str();
                ((uint64_t*) concreteType->value)[1] = realType->value.text.size();
                module->constantSegment.addInitPtr(OFFSETOF(concreteType->value), tmpStorageOffset);
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
    {
        auto concreteType = (ConcreteTypeInfoStruct*) concreteTypeInfoValue;
        auto realType     = (TypeInfoStruct*) typeInfo;

        concreteType->fields.buffer = nullptr;
        concreteType->fields.count  = realType->childs.size();
        if (concreteType->fields.count)
        {
            uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->childs.size() * sizeof(void*));
            ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
            concreteType->fields.buffer     = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->fields.buffer), storageArray);
            for (int field = 0; field < concreteType->fields.count; field++)
                SWAG_CHECK(makeConcreteSubTypeInfo(errorContext, node, addrArray, storageArray, addrArray + field, realType->childs[field]));
        }

        break;
    }
    case TypeInfoKind::FuncAttr:
    {
        auto concreteType = (ConcreteTypeInfoFunc*) concreteTypeInfoValue;
        auto realType     = (TypeInfoFuncAttr*) typeInfo;

        concreteType->parameters.buffer = nullptr;
        concreteType->parameters.count  = realType->parameters.size();
        if (concreteType->parameters.count)
        {
            uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->parameters.size() * sizeof(void*));
            ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
            concreteType->parameters.buffer = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->parameters.buffer), storageArray);
            for (int param = 0; param < concreteType->parameters.count; param++)
                SWAG_CHECK(makeConcreteSubTypeInfo(errorContext, node, addrArray, storageArray, addrArray + param, realType->parameters[param]));
        }

        SWAG_CHECK(makeConcreteSubTypeInfo(errorContext, node, concreteTypeInfoValue, storageOffset, &concreteType->returnType, realType->returnType));
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto concreteType = (ConcreteTypeInfoEnum*) concreteTypeInfoValue;
        auto realType     = (TypeInfoEnum*) typeInfo;

        concreteType->values.buffer = nullptr;
        concreteType->values.count  = realType->values.size();
        if (concreteType->values.count)
        {
            uint32_t           storageArray = module->constantSegment.reserveNoLock((uint32_t) realType->values.size() * sizeof(void*));
            ConcreteTypeInfo** addrArray    = (ConcreteTypeInfo**) module->constantSegment.addressNoLock(storageArray);
            concreteType->values.buffer     = addrArray;
            module->constantSegment.addInitPtr(OFFSETOF(concreteType->values.buffer), storageArray);
            for (int param = 0; param < concreteType->values.count; param++)
                SWAG_CHECK(makeConcreteSubTypeInfo(errorContext, node, addrArray, storageArray, addrArray + param, realType->values[param]));
        }

        concreteType->rawType = nullptr;
        if (realType->rawType)
            SWAG_CHECK(makeConcreteSubTypeInfo(errorContext, node, concreteTypeInfoValue, storageOffset, &concreteType->rawType, realType->rawType));
        break;
    }
    }

    if (lock)
        module->constantSegment.mutex.unlock();

    // Build pointer type to structure
    auto typePtr = g_Pool_typeInfoPointer.alloc();
    typePtr->flags |= TYPEINFO_CONST;
    typePtr->ptrCount    = 1;
    typePtr->pointedType = ((TypeInfoParam*) typeStruct->childs[0])->typeInfo; // Always returns the TypeInfo* pointer, not the typed one
    typePtr->computeName();
    typePtr->sizeOf = sizeof(void*);

    // Register type and value
    concreteTypes[typeInfo] = {typePtr, storageOffset};
    *ptrTypeInfo            = typePtr;
    *storage                = storageOffset;

    if (lock)
        mutexTypes.unlock();

    return true;
}
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
    ConcreteTypeInfo base;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice fields;
};

bool TypeTable::makeConcreteTypeInfo(SemanticContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool lock)
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

    auto            sourceFile = context->sourceFile;
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
    default:
        context->errorContext.report({sourceFile, context->node, format("cannot convert typeinfo '%s' to runtime typeinfo", typeInfo->name.c_str())});
        return false;
    }

    if (lock)
        module->constantSegment.mutex.lock();

    // Build structure content
    uint32_t          storageOffset         = module->constantSegment.reserveNoLock(typeStruct->sizeOf);
    ConcreteTypeInfo* concreteTypeInfoValue = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(storageOffset);

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) &__field - (uint64_t) concreteTypeInfoValue))

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

        TypeInfo* typePtr;
        uint32_t  tmpStorageOffset;
        SWAG_CHECK(makeConcreteTypeInfo(context, realType->pointedType, &typePtr, &tmpStorageOffset, false));
        concreteType->pointedType = (ConcreteTypeInfo*) module->constantSegment.addressNoLock(tmpStorageOffset);

        // We have a pointer in the constant segment, so we need to register it for backend setup
        module->constantSegment.addInitPtr(OFFSETOF(concreteType->pointedType), tmpStorageOffset);
        break;
    }
    case TypeInfoKind::Param:
    {
        auto concreteType = (ConcreteTypeInfoParam*) concreteTypeInfoValue;
        auto realType     = (TypeInfoParam*) typeInfo;
        break;
    }
    case TypeInfoKind::Struct:
    {
        auto concreteType          = (ConcreteTypeInfoStruct*) concreteTypeInfoValue;
        auto realType              = (TypeInfoStruct*) typeInfo;
        concreteType->fields.count = realType->childs.size();
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
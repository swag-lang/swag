#include "pch.h"
#include "TypeTable.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "Module.h"

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

struct ConcreteTypeInfo
{
    TypeInfoKind kind;
    uint32_t     sizeOf;
};

struct ConcreteTypeInfoNative
{
    ConcreteTypeInfo base;
    NativeTypeKind   nativeKind;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo  base;
    uint32_t          ptrCount;
    ConcreteTypeInfo* pointedType;
};

const char* TypeTable::getConcreteTypeInfoName(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        return "swag.TypeInfoNative";
    case TypeInfoKind::Pointer:
        return "swag.TypeInfoPointer";
    }

    return nullptr;
}

void TypeTable::makeConcreteTypeInfo(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, TypeInfo* concreteTypeInfo)
{
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(concreteTypeInfo, TypeInfoKind::Struct);

    scoped_lock lk(mutexTypes);
    auto        it = concreteTypes.find(typeInfo);
    if (it != concreteTypes.end())
    {
        node->typeInfo              = it->second.first;  // Concrete type info
        node->computedValue.reg.u64 = it->second.second; // Index of the structure in the constant segment
        return;
    }

    // Build structure content
    uint32_t storageOffset = 0;
    auto     sourceFile    = context->sourceFile;
    storageOffset          = sourceFile->module->reserveConstantSegment(typeStruct->sizeOf);

    {
        scoped_lock       lock(sourceFile->module->mutexConstantSeg);
        ConcreteTypeInfo* concreteTypeInfoValue = (ConcreteTypeInfo*) &sourceFile->module->constantSegment[storageOffset];

        concreteTypeInfoValue->kind   = typeInfo->kind;
        concreteTypeInfoValue->sizeOf = typeInfo->sizeOf;

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
            break;
        }
        }
    }

    // Build pointer type to structure
    auto typePtr = g_Pool_typeInfoPointer.alloc();
    typePtr->flags |= TYPEINFO_CONST;
    typePtr->ptrCount    = 1;
    typePtr->pointedType = ((TypeInfoParam*) typeStruct->childs[0])->typeInfo; // Always returns the TypeInfo* pointer, not the typed one
    typePtr->computeName();
    typePtr->sizeOf = sizeof(void*);

    // Register type and value
    concreteTypes[concreteTypeInfo] = {typePtr, storageOffset};
    if (node)
    {
        node->typeInfo              = typePtr;
        node->computedValue.reg.u64 = storageOffset;
    }
}
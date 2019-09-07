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

bool TypeTable::makeConcreteTypeInfo(SemanticContext* context, TypeInfo* typeInfo, TypeInfo** concreteTypeInfo, uint32_t* storage)
{
    auto            sourceFile = context->sourceFile;
    auto&           swagScope  = sourceFile->module->workspace->swagScope;
    TypeInfoStruct* typeStruct = nullptr;
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        typeStruct = swagScope.regTypeInfoNative;
        break;
    case TypeInfoKind::Pointer:
        typeStruct = swagScope.regTypeInfoPointer;
        break;
    default:
        context->errorContext.report({sourceFile, context->node, "errororor"});
        return false;
    }

    scoped_lock lk(mutexTypes);
    auto        it = concreteTypes.find(typeInfo);
    if (it != concreteTypes.end())
    {
        *concreteTypeInfo = it->second.first;
        *storage          = it->second.second;
        return true;
    }

    // Build structure content
    uint32_t storageOffset = 0;
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
    concreteTypes[typeInfo] = {typePtr, storageOffset};
    *concreteTypeInfo       = typePtr;
    *storage                = storageOffset;
	return true;
}
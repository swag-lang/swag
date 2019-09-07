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

enum ConcreteTypeInfoKind : uint32_t
{
    Native = 666,
};

struct ConcreteTypeInfo
{
    ConcreteTypeInfoKind kind;
};

const char* TypeTable::getConcreteTypeInfoName(TypeInfo* typeInfo)
{
	return "TypeInfoNative";
}

void TypeTable::makeConcreteTypeInfo(SemanticContext* context, AstNode* node, TypeInfo* typeStruct)
{
    SWAG_ASSERT(typeStruct && typeStruct->kind == TypeInfoKind::Struct);

    scoped_lock lk(mutexTypes);
    auto        typeInfo = node->typeInfo;
    auto        it       = concreteTypes.find(typeInfo);
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
        ConcreteTypeInfo* concreteTypeInfo = (ConcreteTypeInfo*) &sourceFile->module->constantSegment[storageOffset];
        concreteTypeInfo->kind             = ConcreteTypeInfoKind::Native;
    }

    // Build pointer type to structure
    auto typePtr = g_Pool_typeInfoPointer.alloc();
    typePtr->flags |= TYPEINFO_CONST;
    typePtr->ptrCount    = 1;
    typePtr->pointedType = typeStruct;
    typePtr->computeName();
    typePtr->sizeOf = sizeof(void*);

    // Register type and value
    concreteTypes[typeInfo]     = {typePtr, storageOffset};
    node->typeInfo              = typePtr;
    node->computedValue.reg.u64 = storageOffset;
}
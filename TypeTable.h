#pragma once
#include "SpinLock.h"
struct TypeInfo;
struct ErrorContext;
struct AstNode;
struct ConcreteTypeInfo;

struct TypeTable
{
    TypeInfo* registerType(TypeInfo* typeInfo);
    bool      makeConcreteSubTypeInfo(ErrorContext* errorContext, AstNode* node, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo);
    bool      makeConcreteTypeInfo(ErrorContext* errorContext, AstNode* node, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool lock = true);

    SpinLock                                  mutexTypes;
    vector<TypeInfo*>                         allTypes;
    map<TypeInfo*, pair<TypeInfo*, uint32_t>> concreteTypes;
};

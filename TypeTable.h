#pragma once
#include "SpinLock.h"
struct TypeInfo;
struct SemanticContext;
struct AstNode;
struct ConcreteTypeInfo;

struct TypeTable
{
    TypeInfo* registerType(TypeInfo* typeInfo);
    bool      makeConcreteSubTypeInfo(SemanticContext* context, ConcreteTypeInfo* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo);
    bool      makeConcreteTypeInfo(SemanticContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool lock = true);

    SpinLock                                  mutexTypes;
    vector<TypeInfo*>                         allTypes;
    map<TypeInfo*, pair<TypeInfo*, uint32_t>> concreteTypes;
};

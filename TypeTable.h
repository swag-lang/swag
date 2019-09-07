#pragma once
#include "SpinLock.h"
struct TypeInfo;
struct SemanticContext;
struct AstNode;

struct TypeTable
{
    TypeInfo* registerType(TypeInfo* typeInfo);
    bool      makeConcreteTypeInfo(SemanticContext* context, TypeInfo* typeInfo, TypeInfo** concreteTypeInfo, uint32_t* storage);

    SpinLock                                  mutexTypes;
    vector<TypeInfo*>                         allTypes;
    map<TypeInfo*, pair<TypeInfo*, uint32_t>> concreteTypes;
};

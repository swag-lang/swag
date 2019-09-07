#pragma once
#include "SpinLock.h"
struct TypeInfo;
struct SemanticContext;
struct AstNode;

struct TypeTable
{
    TypeInfo*   registerType(TypeInfo* typeInfo);
    void        makeConcreteTypeInfo(SemanticContext* context, AstNode* node, TypeInfo* typeStruct);
    const char* getConcreteTypeInfoName(TypeInfo* typeInfo);

    SpinLock                                  mutexTypes;
    vector<TypeInfo*>                         allTypes;
    map<TypeInfo*, pair<TypeInfo*, uint32_t>> concreteTypes;
};

#pragma once
#include "SpinLock.h"
#include "TypeList.h"
struct TypeInfo;
struct SemanticContext;
struct AstNode;
struct ConcreteTypeInfo;
struct SymbolAttributes;
struct ConcreteStringSlice;

struct TypeTable
{
    TypeTable();
    bool makeConcreteSubTypeInfo(SemanticContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo);
    bool makeConcreteAttributes(SemanticContext* context, SymbolAttributes& attributes, ConcreteStringSlice* result, uint32_t offset);
    bool makeConcreteString(SemanticContext* context, ConcreteStringSlice* result, const Utf8& str, uint32_t offsetInBuffer);
    bool makeConcreteTypeInfo(SemanticContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool lock = true);

    TypeList concreteList;

    SpinLock                                  mutexTypes;
    vector<TypeInfo*>                         allTypes;
    map<TypeInfo*, pair<TypeInfo*, uint32_t>> concreteTypes;
};

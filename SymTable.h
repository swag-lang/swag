#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "Utf8crc.h"
#include "Register.h"
#include "SourceLocation.h"

struct Scope;
struct SourceFile;
struct PoolFactory;
struct Token;
struct TypeInfo;
struct Utf8Crc;
struct Job;
struct TypeInfoFuncAttr;

struct SymbolOverload : public PoolElement
{
    TypeInfo*              typeInfo;
    SourceFile*            sourceFile;
    SourceLocation         startLocation;
    SourceLocation         endLocation;
    ComputedValue          computedValue;
    set<TypeInfoFuncAttr*> attributes;
};

enum class SymbolKind
{
    Variable,
    Type,
    Namespace,
    Enum,
    EnumValue,
    Function,
    Attribute,
};

struct SymbolName : public PoolElement
{
    SpinLock                mutex;
    Utf8Crc                 name;
    SymbolOverload          defaultOverload;
    SymbolKind              kind;
    int                     cptOverloads;
    vector<SymbolOverload*> overloads;
    vector<Job*>            dependentJobs;

    void reset() override
    {
        name.clear();
        cptOverloads = 0;
        overloads.clear();
    }

    SymbolOverload* addOverloadNoLock(SourceFile* sourceFile, const Token& token, TypeInfo* typeInfo, ComputedValue* computedValue);
    SymbolOverload* findOverload(TypeInfo* typeInfo);
};

struct SymTable
{
    SymTable(Scope* scope);

    SymbolName*     registerSymbolNameNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, SymbolKind kind);
    SymbolOverload* addSymbolTypeInfo(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr);
    SymbolOverload* addSymbolTypeInfoNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr);
    bool            checkHiddenSymbol(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind kind);
    bool            checkHiddenSymbolNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, bool checkSameName = false);
    SymbolName*     find(const Utf8Crc& name);
    SymbolName*     findNoLock(const Utf8Crc& name);

    static const char* getArticleKindName(SymbolKind kind);
    static const char* getNakedKindName(SymbolKind kind);

    static const int           HASH_SIZE = 512;
    SpinLock                   mutex;
    Scope*                     scope;
    map<Utf8Crc, SymbolName*>* mapNames[HASH_SIZE];
};

#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "Utf8crc.h"
#include "SourceFile.h"

struct PoolFactory;
struct Token;
struct TypeInfo;
struct Utf8Crc;
struct Job;

struct SymbolOverload : public PoolElement
{
    TypeInfo*      typeInfo;
    SourceFile*    sourceFile;
    SourceLocation startLocation;
    SourceLocation endLocation;
};

enum class SymbolKind
{
    Variable,
    TypeDecl,
    Namespace,
	Enum,
	Function,
};

struct SymbolName : public PoolElement
{
    SpinLock                mutex;
    Utf8Crc                 name;
    SymbolOverload          defaultOverload;
    SymbolKind              kind;
    atomic<int>             cptOverloads;
    vector<SymbolOverload*> overloads;
    vector<Job*>            dependentJobs;

    void reset() override
    {
        name.clear();
        cptOverloads = 0;
        overloads.clear();
    }

    void addOverloadNoLock(SourceFile* sourceFile, const Token& token, TypeInfo* typeInfo);

    SymbolOverload* findOverload(TypeInfo* typeInfo)
    {
        for (auto it : overloads)
        {
            if (it->typeInfo == typeInfo)
                return it;
        }

        return nullptr;
    }
};

struct SymTable
{
    SymTable();

    SymbolName* registerSymbolNameNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, SymbolKind type);
    bool        addSymbolTypeInfo(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type);
    bool        addSymbolTypeInfoNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type);
    bool        checkHiddenSymbol(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type);
    bool        checkHiddenSymbolNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type, SymbolName* symbol);
    SymbolName* find(const Utf8Crc& name);
    SymbolName* findNoLock(const Utf8Crc& name);

    static const int           HASH_SIZE = 512;
    SpinLock                   mutex;
    map<Utf8Crc, SymbolName*>* mapNames[HASH_SIZE];
};

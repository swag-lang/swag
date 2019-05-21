#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "Utf8.h"
#include "SourceFile.h"

struct PoolFactory;
struct Token;
struct TypeInfo;
struct utf8crc;

struct SymbolOverload : public PoolElement
{
    TypeInfo*      typeInfo;
    SourceFile*    sourceFile;
    SourceLocation startLocation;
    SourceLocation endLocation;
};

enum class SymbolType
{
    Variable,
};

struct SymbolName : public PoolElement
{
    SpinLock                mutex;
    utf8                    name;
    SymbolType              type;
    atomic<int>             cptOverloads;
    vector<SymbolOverload*> overloads;

    void reset() override
    {
        name.clear();
        cptOverloads = 0;
        overloads.clear();
    }

    void addOverload(SourceFile* sourceFile, const Token& token, TypeInfo* typeInfo);

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

    SymbolName* registerSymbolName(PoolFactory* factory, const utf8crc& name, SymbolType type);
    bool        addSymbol(SourceFile* sourceFile, const Token& token, const utf8crc& name, TypeInfo* typeInfo, SymbolType type);
    SymbolName* find(const utf8crc& name);

    static const int           HASH_SIZE = 512;
    SpinLock                   mutex;
    map<utf8crc, SymbolName*>* mapNames[HASH_SIZE];
};

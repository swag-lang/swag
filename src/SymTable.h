#pragma once
#include "Mutex.h"
#include "Symbol.h"
#include "SymTableHash.h"

struct AstNode;
struct ComputedValue;
struct ErrorContext;
struct Scope;
struct TypeInfo;
struct TypeInfoStruct;
struct Utf8;

struct AddSymbolTypeInfo
{
    Utf8           aliasName;
    AstNode*       node           = nullptr;
    TypeInfo*      typeInfo       = nullptr;
    ComputedValue* computedValue  = nullptr;
    SymbolName*    symbolName     = nullptr;
    DataSegment*   storageSegment = nullptr;
    OverloadFlags  flags          = 0;
    uint32_t       storageOffset  = 0;
    SymbolKind     kind           = SymbolKind::Invalid;
};

struct StructToDrop
{
    SymbolOverload* overload;
    TypeInfo*       typeInfo;
    TypeInfoStruct* typeStruct;
    uint32_t        storageOffset;
};

struct SymTable
{
    void release();

    SymbolName*     find(const Utf8& name, uint32_t crc = 0) const;
    SymbolName*     findNoLock(const Utf8& name, uint32_t crc = 0) const;
    SymbolName*     registerSymbolName(ErrorContext* context, AstNode* node, SymbolKind kind, const Utf8* aliasName = nullptr);
    SymbolName*     registerSymbolNameNoLock(ErrorContext* context, AstNode* node, SymbolKind kind, const Utf8* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfo(ErrorContext* context, AddSymbolTypeInfo& toAdd);
    SymbolOverload* addSymbolTypeInfoNoLock(ErrorContext* context, AddSymbolTypeInfo& toAdd);

    bool        acceptGhostSymbolNoLock(ErrorContext* context, const AstNode* node, SymbolKind kind, const SymbolName* symbol) const;
    bool        checkHiddenSymbolNoLock(ErrorContext* context, AstNode* node, const TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol) const;
    void        addVarToDrop(SymbolOverload* overload, TypeInfo* typeInfo, uint32_t storageOffset);
    static bool registerNameAlias(ErrorContext* context, const AstNode* node, SymbolName* symbol, SymbolName* otherSymbol, SymbolOverload* otherOverload);

    static void disabledIfBlockOverloadNoLock(AstNode* node, SymbolName* symbol);

    mutable SharedMutex        mutex;
    SymTableHash               mapNames;
    VectorNative<StructToDrop> structVarsToDrop;
    VectorNative<SymbolName*>  allSymbols;

    Scope* scope    = nullptr;
    bool   occupied = false;

    bool tryRead() const
    {
        if (occupied)
            return false;
        return mutex.mt.try_lock_shared();
    }

    void endRead() const
    {
        mutex.mt.unlock_shared();
    }
};

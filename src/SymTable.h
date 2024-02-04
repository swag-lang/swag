#pragma once
#include "Mutex.h"
#include "Symbol.h"
#include "SymTableHash.h"

struct Scope;
struct TypeInfo;
struct Utf8;
struct AstNode;
struct ErrorContext;
struct ComputedValue;
struct TypeInfoStruct;

struct AddSymbolTypeInfo
{
    AstNode*       node           = nullptr;
    TypeInfo*      typeInfo       = nullptr;
    ComputedValue* computedValue  = nullptr;
    SymbolName*    symbolName     = nullptr;
    DataSegment*   storageSegment = nullptr;
    Utf8*          aliasName      = nullptr;
    uint32_t       flags          = 0;
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

    SymbolName*     find(const Utf8& name, uint32_t crc = 0);
    SymbolName*     findNoLock(const Utf8& name, uint32_t crc = 0);
    SymbolName*     registerSymbolName(ErrorContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName = nullptr);
    SymbolName*     registerSymbolNameNoLock(ErrorContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfo(ErrorContext* context, AddSymbolTypeInfo& toAdd);
    SymbolOverload* addSymbolTypeInfoNoLock(ErrorContext* context, AddSymbolTypeInfo& toAdd);

    bool        acceptGhostSymbolNoLock(ErrorContext* context, AstNode* node, SymbolKind kind, SymbolName* symbol) const;
    bool        checkHiddenSymbolNoLock(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, uint32_t overFlags);
    void        addVarToDrop(SymbolOverload* overload, TypeInfo* typeInfo, uint32_t storageOffset);
    static bool registerNameAlias(ErrorContext* context, AstNode* node, SymbolName* symbol, SymbolName* otherSymbol, SymbolOverload* otherOverload);

    static void disabledIfBlockOverloadNoLock(AstNode* node, SymbolName* symbol);

    SharedMutex                mutex;
    SymTableHash               mapNames;
    VectorNative<StructToDrop> structVarsToDrop;
    VectorNative<SymbolName*>  allSymbols;

    Scope* scope    = nullptr;
    bool   occupied = false;

    bool tryRead()
    {
        if (occupied)
            return false;
        return mutex.mt.try_lock_shared();
    }

    void endRead()
    {
        mutex.mt.unlock_shared();
    }
};

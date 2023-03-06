#pragma once
#include "DependentJobs.h"
#include "Register.h"
#include "Mutex.h"
#include "SymTableHash.h"
#include "Symbol.h"
struct Scope;
struct TypeInfo;
struct Utf8;
struct AstNode;
struct ErrorContext;
struct ComputedValue;
struct TypeInfoStruct;

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

    SymbolName*     registerSymbolName(ErrorContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName = nullptr);
    SymbolName*     registerSymbolNameNoLock(ErrorContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfo(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, DataSegment* storageSegment = nullptr, Utf8* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfoNoLock(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, DataSegment* storageSegment = nullptr, Utf8* aliasName = nullptr);
    bool            acceptGhostSymbolNoLock(ErrorContext* context, AstNode* node, SymbolKind kind, SymbolName* symbol);
    bool            checkHiddenSymbolNoLock(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, uint32_t overFlags);
    SymbolName*     find(const Utf8& name, uint32_t crc = 0);
    SymbolName*     findNoLock(const Utf8& name, uint32_t crc = 0);
    void            addVarToDrop(SymbolOverload* overload, TypeInfo* typeInfo, uint32_t storageOffset);
    void            addVarToDrop(StructToDrop& st);
    bool            registerUsingAliasOverload(ErrorContext* context, AstNode* node, SymbolName* symbol, SymbolOverload* overload);
    static void     decreaseOverloadNoLock(SymbolName* symbol);
    static void     disabledIfBlockOverloadNoLock(AstNode* node, SymbolName* symbol);

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

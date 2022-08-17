#pragma once
#include "DependentJobs.h"
#include "Register.h"
#include "RegisterList.h"
#include "Mutex.h"
struct Scope;
struct SourceFile;
struct Token;
struct TypeInfo;
struct Utf8;
struct Job;
struct TypeInfoFuncAttr;
struct ByteCodeGenJob;
struct AstNode;
struct SymTable;
struct JobContext;
struct SymbolName;
struct TypeInfoStruct;

static const uint32_t OVERLOAD_BYTECODE_GENERATED = 0x00000001;
static const uint32_t OVERLOAD_VAR_FUNC_PARAM     = 0x00000002;
static const uint32_t OVERLOAD_VAR_GLOBAL         = 0x00000004;
static const uint32_t OVERLOAD_VAR_LOCAL          = 0x00000008;
static const uint32_t OVERLOAD_COMPUTED_VALUE     = 0x00000010;
static const uint32_t OVERLOAD_CONST_ASSIGN       = 0x00000020;
static const uint32_t OVERLOAD_VAR_STRUCT         = 0x00000040;
static const uint32_t OVERLOAD_GENERIC            = 0x00000080;
static const uint32_t OVERLOAD_INCOMPLETE         = 0x00000100;
static const uint32_t OVERLOAD_VAR_INLINE         = 0x00000200;
static const uint32_t OVERLOAD_VAR_TLS            = 0x00000400;
static const uint32_t OVERLOAD_VAR_BSS            = 0x00000800;
static const uint32_t OVERLOAD_IMPL_IN_STRUCT     = 0x00001000;
static const uint32_t OVERLOAD_RETVAL             = 0x00002000;
static const uint32_t OVERLOAD_EMITTED            = 0x00004000;
static const uint32_t OVERLOAD_TUPLE_UNPACK       = 0x00008000;
static const uint32_t OVERLOAD_STORE_SYMBOLS      = 0x00010000;
static const uint32_t OVERLOAD_VAR_COMPILER       = 0x00020000;
static const uint32_t OVERLOAD_CONSTANT           = 0x00040000;
static const uint32_t OVERLOAD_CAN_CHANGE         = 0x00080000;
static const uint32_t OVERLOAD_USED               = 0x00100000;
static const uint32_t OVERLOAD_STRUCT_AFFECT      = 0x00200000;
static const uint32_t OVERLOAD_VAR_CAPTURE        = 0x00400000;

enum class SymbolKind : uint8_t
{
    Invalid,
    Variable,
    TypeAlias,
    Alias,
    Namespace,
    Enum,
    EnumValue,
    Function,
    Attribute,
    Struct,
    Interface,
    GenericType,
    Label,
    PlaceHolder,
};

const uint16_t SYMBOL_ATTRIBUTE_GEN = 0x0001;

struct SymbolOverload
{
    ComputedValue computedValue;
    RegisterList  registers;
    TypeInfo*     typeInfo       = nullptr;
    AstNode*      node           = nullptr;
    SymbolName*   symbol         = nullptr;
    uint64_t      attributeFlags = 0;
    uint32_t      flags          = 0;
    uint32_t      storageIndex   = 0;
    SharedMutex   mutexIncomplete;
};

struct SymbolName
{
    SymbolOverload* addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue);
    SymbolOverload* findOverload(TypeInfo* typeInfo);
    void            addDependentJob(Job* job);
    void            addDependentJobNoLock(Job* job);
    Utf8            getFullName();
    void            unregisterNode(AstNode* node);

    SharedMutex                   mutex;
    VectorNative<SymbolOverload*> overloads;
    Utf8                          name;
    DependentJobs                 dependentJobs;
    VectorNative<AstNode*>        nodes;

    SymTable* ownerTable = nullptr;

    uint32_t   cptOverloads     = 0;
    uint32_t   cptOverloadsInit = 0;
    uint32_t   cptIfBlock       = 0;
    uint16_t   flags            = 0;
    uint8_t    padding;
    SymbolKind kind = SymbolKind::Invalid;
};

struct SymTableHash
{
    void        clone(SymTableHash* from);
    SymbolName* find(const Utf8& str, uint32_t crc = 0);
    void        addElem(SymbolName* data, uint32_t crc = 0);
    void        add(SymbolName* data);
    void        remove(SymbolName* data);

    struct Entry
    {
        SymbolName* symbolName;
        uint32_t    hash;
    };

    Entry*   buffer     = nullptr;
    uint32_t allocated  = 0;
    uint32_t count      = 0;
    uint32_t fastReject = 0;
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
    void            removeSymbolName(SymbolName* sym);
    SymbolName*     registerSymbolName(JobContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName = nullptr);
    SymbolName*     registerSymbolNameNoLock(JobContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfo(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, DataSegment* storageSegment = nullptr, Utf8* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfoNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, DataSegment* storageSegment = nullptr, Utf8* aliasName = nullptr);
    bool            acceptGhostSymbolNoLock(JobContext* context, AstNode* node, SymbolKind kind, SymbolName* symbol);
    bool            checkHiddenSymbol(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind);
    bool            checkHiddenSymbolNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, bool checkSameName = false);
    SymbolName*     find(const Utf8& name, uint32_t crc = 0);
    SymbolName*     findNoLock(const Utf8& name, uint32_t crc = 0);
    void            addVarToDrop(SymbolOverload* overload, TypeInfo* typeInfo, uint32_t storageOffset);
    void            addVarToDrop(StructToDrop& st);
    bool            registerUsingAliasOverload(JobContext* context, AstNode* node, SymbolName* symbol, SymbolOverload* overload);
    static void     decreaseOverloadNoLock(SymbolName* symbol);
    static void     disabledIfBlockOverloadNoLock(AstNode* node, SymbolName* symbol);

    static const char* getArticleKindName(SymbolKind kind);
    static const char* getNakedKindName(SymbolKind kind);
    static const char* getNakedKindName(SymbolOverload* overload);

    SharedMutex                mutex;
    SymTableHash               mapNames;
    VectorNative<StructToDrop> structVarsToDrop;

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

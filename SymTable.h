#pragma once
#include "DependentJobs.h"
#include "Pool.h"
#include "Utf8crc.h"
#include "Register.h"
#include "RegisterList.h"
#include "SourceLocation.h"
#include "RaceCondition.h"
struct Scope;
struct SourceFile;
struct Token;
struct TypeInfo;
struct Utf8Crc;
struct Job;
struct TypeInfoFuncAttr;
struct ByteCodeGenJob;
struct AstNode;
struct SymTable;
struct JobContext;
struct SymbolName;

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
static const uint32_t OVERLOAD_PUBLIC             = 0x00000400;
static const uint32_t OVERLOAD_VAR_BSS            = 0x00000800;

struct SymbolOverload
{
    ComputedValue computedValue;
    RegisterList  registers;
    TypeInfo*     typeInfo       = nullptr;
    AstNode*      node           = nullptr;
    SymbolName*   symbol         = nullptr;
    uint32_t      flags          = 0;
    uint32_t      storageOffset  = UINT32_MAX;
    uint32_t      storageIndex   = 0;
    uint32_t      attributeFlags = 0;
};

enum class SymbolKind
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
};

struct SymbolName
{
    SymbolOverload* addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue);
    SymbolOverload* findOverload(TypeInfo* typeInfo);
    void            addDependentJob(Job* job);
    void            addDependentJobNoLock(Job* job);

    shared_mutex                  mutex;
    VectorNative<SymbolOverload*> overloads;
    Utf8                          fullName;
    Utf8Crc                       name;
    SymbolOverload                defaultOverload;
    DependentJobs                 dependentJobs;

    SymTable* ownerTable = nullptr;

    SymbolKind kind             = SymbolKind::Invalid;
    uint32_t   cptOverloads     = 0;
    uint32_t   cptOverloadsInit = 0;
};

struct SymTableHash
{
    static const int          MAX_HASH = 47;
    VectorNative<SymbolName*> map[MAX_HASH];

    SymbolName* find(const Utf8Crc& str)
    {
        int idx = str.crc % MAX_HASH;
        for (auto one : map[idx])
        {
            if (one->name.crc != str.crc)
                continue;
            if (!strcmp((const char*) one->name.buffer, (const char*) str.buffer))
                return one;
        }

        return nullptr;
    }

    void add(SymbolName* data)
    {
        int idx = data->name.crc % MAX_HASH;
        map[idx].push_back(data);
    }
};

struct SymTable
{
    SymbolName*     registerSymbolName(JobContext* context, AstNode* node, SymbolKind kind, Utf8Crc* aliasName = nullptr);
    SymbolName*     registerSymbolNameNoLock(JobContext* context, AstNode* node, SymbolKind kind, Utf8Crc* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfo(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, Utf8Crc* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfoNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, Utf8Crc* aliasName = nullptr);
    bool            checkHiddenSymbol(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind);
    bool            checkHiddenSymbolNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, bool checkSameName = false);
    SymbolName*     find(const Utf8Crc& name);
    SymbolName*     findNoLock(const Utf8Crc& name);
    static void     decreaseOverloadNoLock(SymbolName* symbol);
    bool            registerUsingAliasOverload(JobContext* context, AstNode* node, SymbolName* symbol, SymbolOverload* overload);

    static const char* getArticleKindName(SymbolKind kind);
    static const char* getNakedKindName(SymbolKind kind);

    SymTableHash                  mapNames;
    VectorNative<SymbolOverload*> structVarsToDrop;
    shared_mutex                  mutex;

    Scope* scope;
    SWAG_RACE_CONDITION_INSTANCE(raceCondition);
};

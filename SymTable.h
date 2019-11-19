#pragma once
#include "DependentJobs.h"
#include "Pool.h"
#include "SpinLock.h"
#include "Utf8crc.h"
#include "Register.h"
#include "RegisterList.h"
#include "SourceLocation.h"
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

struct SymbolOverload : public PoolElement
{
    void reset() override
    {
        typeInfo      = nullptr;
        flags         = 0;
        node          = nullptr;
        storageOffset = UINT32_MAX;
        storageIndex  = 0;
        overloadIndex = 0;
        registers.clear();
    }

    TypeInfo*     typeInfo;
    ComputedValue computedValue;
    uint32_t      flags;
    AstNode*      node;
    uint32_t      storageOffset;
    uint32_t      storageIndex;
    uint32_t      overloadIndex;
    RegisterList  registers;
};

enum class SymbolKind
{
    Variable,
    TypeAlias,
    Namespace,
    Enum,
    EnumValue,
    Function,
    Attribute,
    FuncParam,
    Struct,
    GenericType,
	Label,
};

struct SymbolName : public PoolElement
{
    SpinLock                mutex;
    Utf8                    fullName;
    Utf8Crc                 name;
    SymbolOverload          defaultOverload;
    SymbolKind              kind;
    uint32_t                cptOverloads;
    uint32_t                cptOverloadsInit;
    vector<SymbolOverload*> overloads;
    DependentJobs           dependentJobs;
    SymTable*               ownerTable;

    void reset() override
    {
        name.clear();
        cptOverloads     = 0;
        cptOverloadsInit = 0;
        ownerTable       = nullptr;
        overloads.clear();
    }

    SymbolOverload* addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue);
    SymbolOverload* findOverload(TypeInfo* typeInfo);
};

struct SymTable
{
    SymTable(Scope* scope);

    SymbolName*     registerSymbolNameNoLock(JobContext* context, AstNode* node, SymbolKind kind, Utf8Crc* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfo(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, Utf8Crc* aliasName = nullptr);
    SymbolOverload* addSymbolTypeInfoNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, Utf8Crc* aliasName = nullptr);
    bool            checkHiddenSymbol(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind);
    bool            checkHiddenSymbolNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, bool checkSameName = false);
    SymbolName*     find(const Utf8Crc& name);
    SymbolName*     findNoLock(const Utf8Crc& name);
    static void     decreaseOverloadNoLock(SymbolName* symbol);

    static const char* getArticleKindName(SymbolKind kind);
    static const char* getNakedKindName(SymbolKind kind);

    static const int           HASH_SIZE = 512;
    SpinLock                   mutex;
    Scope*                     scope;
    map<Utf8Crc, SymbolName*>* mapNames[HASH_SIZE];
    vector<SymbolOverload*>    allStructs;
};

extern Pool<SymbolOverload> g_Pool_symOverload;
extern Pool<SymbolName>     g_Pool_symName;
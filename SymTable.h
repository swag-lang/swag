#pragma once
#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "Utf8crc.h"
#include "Register.h"
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

static const uint32_t OVERLOAD_BYTECODE_GENERATED = 0x00000001;
static const uint32_t OVERLOAD_VAR_FUNC_PARAM     = 0x00000002;
static const uint32_t OVERLOAD_VAR_GLOBAL         = 0x00000004;
static const uint32_t OVERLOAD_VAR_LOCAL          = 0x00000008;
static const uint32_t OVERLOAD_COMPUTED_VALUE     = 0x00000010;
static const uint32_t OVERLOAD_CONST              = 0x00000020;
static const uint32_t OVERLOAD_VAR_STRUCT         = 0x00000040;
static const uint32_t OVERLOAD_GENERIC            = 0x00000080;

struct SymbolAttributes
{
    set<TypeInfoFuncAttr*>     attributes;
    map<string, ComputedValue> values;

    bool getValue(const string& fullName, ComputedValue& value);
};

struct SymbolOverload : public PoolElement
{
    void reset() override
    {
        typeInfo   = nullptr;
        sourceFile = nullptr;
        flags      = 0;
        node       = nullptr;
        attributes.attributes.clear();
        attributes.values.clear();
        storageOffset = UINT32_MAX;
        storageIndex  = 0;
    }

    TypeInfo*        typeInfo;
    SourceFile*      sourceFile;
    ComputedValue    computedValue;
    uint32_t         flags;
    AstNode*         node;
    SymbolAttributes attributes;
    uint32_t         storageOffset;
    uint32_t         storageIndex;
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
    FuncParam,
    Struct,
};

struct SymbolName : public PoolElement
{
    SpinLock                mutex;
    Utf8                    fullName;
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

    SymbolOverload* addOverloadNoLock(SourceFile* sourceFile, AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue);
    SymbolOverload* findOverload(TypeInfo* typeInfo);
};

struct SymTable
{
    SymTable(Scope* scope);

    SymbolName*     registerSymbolNameNoLock(SourceFile* sourceFile, AstNode* node, SymbolKind kind);
    SymbolOverload* addSymbolTypeInfo(SourceFile* sourceFile, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, SymbolAttributes* attributes = nullptr);
    SymbolOverload* addSymbolTypeInfoNoLock(SourceFile* sourceFile, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0, SymbolName** resultName = nullptr, uint32_t storageOffset = 0, SymbolAttributes* attributes = nullptr);
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

extern Pool<SymbolOverload> g_Pool_symOverload;
extern Pool<SymbolName>     g_Pool_symName;
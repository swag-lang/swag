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

struct SymbolOverload : public PoolElement
{
    TypeInfo*              typeInfo;
    SourceFile*            sourceFile;
    ComputedValue          computedValue;
    set<TypeInfoFuncAttr*> attributes;
    uint32_t               flags;
    AstNode*               node;
    int                    stackOffset = -1;
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
    GlobalVar,
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

    SymbolOverload* addOverloadNoLock(SourceFile* sourceFile, AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue);
    SymbolOverload* findOverload(TypeInfo* typeInfo);
};

struct SymTable
{
    SymTable(Scope* scope);

    SymbolName*     registerSymbolNameNoLock(SourceFile* sourceFile, AstNode* node, SymbolKind kind);
    SymbolOverload* addSymbolTypeInfo(SourceFile* sourceFile, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0);
    SymbolOverload* addSymbolTypeInfoNoLock(SourceFile* sourceFile, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue = nullptr, uint32_t flags = 0);
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
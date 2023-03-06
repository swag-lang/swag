#pragma once
#include "DependentJobs.h"
#include "Register.h"
#include "ComputedValue.h"
#include "Mutex.h"
#include "Symbol.h"
#include "Utf8.h"
struct TypeInfo;
struct Job;
struct AstNode;
struct SymTable;
struct SymbolName;

const uint32_t OVERLOAD_BYTECODE_GENERATED = 0x00000001;
const uint32_t OVERLOAD_VAR_FUNC_PARAM     = 0x00000002;
const uint32_t OVERLOAD_VAR_GLOBAL         = 0x00000004;
const uint32_t OVERLOAD_VAR_LOCAL          = 0x00000008;
const uint32_t OVERLOAD_COMPUTED_VALUE     = 0x00000010;
const uint32_t OVERLOAD_CONST_ASSIGN       = 0x00000020;
const uint32_t OVERLOAD_VAR_STRUCT         = 0x00000040;
const uint32_t OVERLOAD_GENERIC            = 0x00000080;
const uint32_t OVERLOAD_INCOMPLETE         = 0x00000100;
const uint32_t OVERLOAD_VAR_INLINE         = 0x00000200;
const uint32_t OVERLOAD_VAR_TLS            = 0x00000400;
const uint32_t OVERLOAD_VAR_BSS            = 0x00000800;
const uint32_t OVERLOAD_IMPL_IN_STRUCT     = 0x00001000;
const uint32_t OVERLOAD_RETVAL             = 0x00002000;
const uint32_t OVERLOAD_EMITTED            = 0x00004000;
const uint32_t OVERLOAD_TUPLE_UNPACK       = 0x00008000;
const uint32_t OVERLOAD_STORE_SYMBOLS      = 0x00010000;
const uint32_t OVERLOAD_VAR_COMPILER       = 0x00020000;
const uint32_t OVERLOAD_CONSTANT           = 0x00040000;
const uint32_t OVERLOAD_CAN_CHANGE         = 0x00080000;
const uint32_t OVERLOAD_USED               = 0x00100000;
const uint32_t OVERLOAD_STRUCT_AFFECT      = 0x00200000;
const uint32_t OVERLOAD_VAR_CAPTURE        = 0x00400000;

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
const uint16_t SYMBOL_USED          = 0x0002;

struct SymbolOverload
{
    ComputedValue computedValue;
    SharedMutex   mutexIncomplete;
    RegisterList  registers;

    TypeInfo*   typeInfo        = nullptr;
    AstNode*    node            = nullptr;
    SymbolName* symbol          = nullptr;
    AstNode*    fromInlineParam = nullptr;

    uint64_t attributeFlags = 0;
    uint32_t flags          = 0;
    uint32_t storageIndex   = 0;
    uint8_t  hintRegister   = 0;

    void from(SymbolOverload* other)
    {
        computedValue   = other->computedValue;
        registers       = other->registers;
        typeInfo        = other->typeInfo;
        node            = other->node;
        symbol          = other->symbol;
        fromInlineParam = other->fromInlineParam;
        attributeFlags  = other->attributeFlags;
        flags           = other->flags;
        storageIndex    = other->storageIndex;
        hintRegister    = other->hintRegister;
    }
};

struct SymbolName
{
    void            decreaseOverloadNoLock();
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

    uint32_t cptOverloads     = 0;
    uint32_t cptOverloadsInit = 0;

    uint32_t   cptIfBlock = 0;
    uint16_t   flags      = 0;
    SymbolKind kind       = SymbolKind::Invalid;
    uint8_t    padding;
};

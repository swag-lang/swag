#pragma once
#include "Backend/ByteCode/Register.h"
#include "Jobs/DependentJobs.h"
#include "Syntax/ComputedValue.h"
#include "Threading/Mutex.h"

struct AstNode;
struct Job;
struct SymTable;
struct SymbolName;
struct TypeInfo;

using OverloadFlags = Flags<uint32_t>;
using SymbolFlags   = Flags<uint16_t>;

enum class SymbolKind : uint8_t
{
    Invalid,
    Variable,
    TypeAlias,
    NameAlias,
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

constexpr OverloadFlags OVERLOAD_VAR_BSS          = 0x00000001;
constexpr OverloadFlags OVERLOAD_VAR_CAPTURE      = 0x00000002;
constexpr OverloadFlags OVERLOAD_VAR_COMPILER     = 0x00000004;
constexpr OverloadFlags OVERLOAD_VAR_FUNC_PARAM   = 0x00000008;
constexpr OverloadFlags OVERLOAD_VAR_GLOBAL       = 0x00000010;
constexpr OverloadFlags OVERLOAD_VAR_INLINE       = 0x00000020;
constexpr OverloadFlags OVERLOAD_VAR_LOCAL        = 0x00000040;
constexpr OverloadFlags OVERLOAD_VAR_STRUCT       = 0x00000080;
constexpr OverloadFlags OVERLOAD_VAR_TLS          = 0x00000100;
constexpr OverloadFlags OVERLOAD_COMPUTED_VALUE   = 0x00000200;
constexpr OverloadFlags OVERLOAD_CONST_ASSIGN     = 0x00000400;
constexpr OverloadFlags OVERLOAD_CONSTANT         = 0x00000800;
constexpr OverloadFlags OVERLOAD_EMITTED          = 0x00001000;
constexpr OverloadFlags OVERLOAD_GENERIC          = 0x00002000;
constexpr OverloadFlags OVERLOAD_IMPL_IN_STRUCT   = 0x00004000;
constexpr OverloadFlags OVERLOAD_INCOMPLETE       = 0x00008000;
constexpr OverloadFlags OVERLOAD_RETVAL           = 0x00010000;
constexpr OverloadFlags OVERLOAD_STRUCT_AFFECT    = 0x00020000;
constexpr OverloadFlags OVERLOAD_TUPLE_UNPACK     = 0x00040000;
constexpr OverloadFlags OVERLOAD_USED             = 0x00080000;
constexpr OverloadFlags OVERLOAD_NOT_INITIALIZED  = 0x00100000;
constexpr OverloadFlags OVERLOAD_HAS_AFFECT       = 0x00200000;
constexpr OverloadFlags OVERLOAD_IS_LET           = 0x00400000;
constexpr OverloadFlags OVERLOAD_HAS_MAKE_POINTER = 0x00800000;
constexpr OverloadFlags OVERLOAD_VAR_HAS_ASSIGN   = 0x01000000;
constexpr OverloadFlags OVERLOAD_PERSISTENT_REG   = 0x02000000;
constexpr OverloadFlags OVERLOAD_HINT_REG         = 0x04000000;
constexpr OverloadFlags OVERLOAD_INLINE_REG       = 0x08000000;
constexpr OverloadFlags OVERLOAD_UNDEFINED        = 0x10000000;

constexpr SymbolFlags SYMBOL_ATTRIBUTE_GEN = 0x0001;
constexpr SymbolFlags SYMBOL_USED          = 0x0002;

struct SymbolOverload
{
    void from(const SymbolOverload* other);
    void setRegisters(const RegisterList& reg, OverloadFlags fl);

    bool hasFlag(OverloadFlags fl) const { return flags.has(fl); }

    ComputedValue computedValue;
    RegisterList  symRegisters;

    TypeInfo*   typeInfo        = nullptr;
    AstNode*    node            = nullptr;
    SymbolName* symbol          = nullptr;
    AstNode*    fromInlineParam = nullptr;

    uint32_t      storageIndex = 0;
    OverloadFlags flags        = 0;
};

struct SymbolName
{
    SymbolOverload* findOverload(const TypeInfo* typeInfo);
    Utf8            getFullName() const;

    SymbolOverload* addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, const ComputedValue* computedValue);
    void            decreaseOverloadNoLock();
    void            addDependentJob(Job* job);
    void            addDependentJobNoLock(Job* job);
    void            unregisterNode(const AstNode* node);

    bool is(SymbolKind what) const { return kind == what; }
    bool isNot(SymbolKind what) const { return kind != what; }
    bool hasFlag(SymbolFlags fl) const { return flags.has(fl); }

    SharedMutex                   mutex;
    VectorNative<SymbolOverload*> overloads;
    Utf8                          name;
    DependentJobs                 dependentJobs;
    VectorNative<AstNode*>        nodes;

    SymTable* ownerTable = nullptr;

    uint32_t cptOverloads     = 0;
    uint32_t cptOverloadsInit = 0;

    uint32_t    cptIfBlock = 0;
    SymbolFlags flags      = 0;
    SymbolKind  kind       = SymbolKind::Invalid;
    uint8_t     padding    = 0;
};

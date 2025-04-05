#pragma once
#include "Backend/CallConv.h"
#include "Backend/Runtime.h"
#include "Core/AtomicFlags.h"
#include "Report/Assert.h"
#include "Semantic/Attribute.h"
#include "Semantic/Type/TypeMatch.h"
#include "Threading/Mutex.h"
#include "Threading/RaceCondition.h"

struct AstFuncDecl;
struct AstNode;
struct BadSignatureInfos;
struct ByteCode;
struct ComputedValue;
struct Job;
struct JobContext;
struct Scope;
struct SemanticContext;
struct SymbolMatchContext;
struct SymbolOverload;
struct TypeInfo;
struct TypeInfoFuncAttr;
struct TypeInfoParam;
struct TypeInfoStruct;

using TypeInfoFlagsV = uint64_t;
using TypeInfoFlags  = AtomicFlags<TypeInfoFlagsV>;
using TypeParamFlags = Flags<uint32_t>;

constexpr TypeInfoFlagsV TYPEINFO_IS_SELF                  = 0x00000000'00000001;
constexpr TypeInfoFlagsV TYPEINFO_UNTYPED_BIN_HEX          = 0x00000000'00000002;
constexpr TypeInfoFlagsV TYPEINFO_INTEGER                  = 0x00000000'00000004;
constexpr TypeInfoFlagsV TYPEINFO_FLOAT                    = 0x00000000'00000008;
constexpr TypeInfoFlagsV TYPEINFO_UNSIGNED                 = 0x00000000'00000010;
constexpr TypeInfoFlagsV TYPEINFO_CONST                    = 0x00000000'00000020;
constexpr TypeInfoFlagsV TYPEINFO_AUTO_NAME                = 0x00000000'00000040;
constexpr TypeInfoFlagsV TYPEINFO_VARIADIC                 = 0x00000000'00000080;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_HAS_INIT_VALUES   = 0x00000000'00000100;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_ALL_UNINITIALIZED = 0x00000000'00000200;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_NO_POST_COPY      = 0x00000000'00000400;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_NO_POST_MOVE      = 0x00000000'00000800;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_NO_DROP           = 0x00000000'00001000;
constexpr TypeInfoFlagsV TYPEINFO_GENERIC                  = 0x00000000'00002000;
constexpr TypeInfoFlagsV TYPEINFO_POINTER_MOVE_REF         = 0x00000000'00004000;
constexpr TypeInfoFlagsV TYPEINFO_UNTYPED_INTEGER          = 0x00000000'00008000;
constexpr TypeInfoFlagsV TYPEINFO_UNTYPED_FLOAT            = 0x00000000'00010000;
constexpr TypeInfoFlagsV TYPEINFO_DEFINED_VALUE            = 0x00000000'00020000;
constexpr TypeInfoFlagsV TYPEINFO_AUTO_CAST                = 0x00000000'00040000;
constexpr TypeInfoFlagsV TYPEINFO_TYPED_VARIADIC           = 0x00000000'00080000;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_TYPEINFO          = 0x00000000'00100000;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_IS_TUPLE          = 0x00000000'00200000;
constexpr TypeInfoFlagsV TYPEINFO_ENUM_FLAGS               = 0x00000000'00400000;
constexpr TypeInfoFlagsV TYPEINFO_FLATTEN_DONE             = 0x00000000'00800000;
constexpr TypeInfoFlagsV TYPEINFO_UNDEFINED                = 0x00000000'01000000;
constexpr TypeInfoFlagsV TYPEINFO_ENUM_INDEX               = 0x00000000'02000000;
constexpr TypeInfoFlagsV TYPEINFO_STRICT                   = 0x00000000'04000000;
constexpr TypeInfoFlagsV TYPEINFO_CONST_ALIAS              = 0x00000000'08000000;
constexpr TypeInfoFlagsV TYPEINFO_HAS_USING                = 0x00000000'10000000;
constexpr TypeInfoFlagsV TYPEINFO_FUNC_IS_ATTR             = 0x00000000'20000000;
constexpr TypeInfoFlagsV TYPEINFO_FROM_GENERIC             = 0x00000000'40000000;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_NO_COPY           = 0x00000000'80000000;
constexpr TypeInfoFlagsV TYPEINFO_CAN_THROW                = 0x00000001'00000000;
constexpr TypeInfoFlagsV TYPEINFO_GENERIC_COUNT            = 0x00000002'00000000;
constexpr TypeInfoFlagsV TYPEINFO_POINTER_ACCEPT_MOVE_REF  = 0x00000004'00000000;
constexpr TypeInfoFlagsV TYPEINFO_C_STRING                 = 0x00000008'00000000;
constexpr TypeInfoFlagsV TYPEINFO_C_VARIADIC               = 0x00000010'00000000;
constexpr TypeInfoFlagsV TYPEINFO_GENERATED_TUPLE          = 0x00000020'00000000;
constexpr TypeInfoFlagsV TYPEINFO_CLOSURE                  = 0x00000040'00000000;
constexpr TypeInfoFlagsV TYPEINFO_INCOMPLETE               = 0x00000080'00000000;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_IS_ITABLE         = 0x00000100'00000000;
constexpr TypeInfoFlagsV TYPEINFO_CAN_PROMOTE_816          = 0x00000200'00000000;
constexpr TypeInfoFlagsV TYPEINFO_POINTER_ARITHMETIC       = 0x00000400'00000000;
constexpr TypeInfoFlagsV TYPEINFO_LIST_ARRAY_ARRAY         = 0x00000800'00000000;
constexpr TypeInfoFlagsV TYPEINFO_POINTER_REF              = 0x00001000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_POINTER_AUTO_REF         = 0x00002000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_NO_INIT           = 0x00004000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_SPEC_OP_GENERATED        = 0x00008000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_STRUCT_EMPTY             = 0x00010000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_CHARACTER                = 0x00020000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_ENUM_HAS_USING           = 0x00040000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_GHOST_TUPLE              = 0x00080000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_GENERATED_OP_EQUALS      = 0x00100000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_WAS_UNTYPED              = 0x00200000'00000000;
constexpr TypeInfoFlagsV TYPEINFO_NULLABLE                 = 0x00400000'00000000;

constexpr TypeParamFlags TYPEINFOPARAM_DEFINED_VALUE    = 0x00000001;
constexpr TypeParamFlags TYPEINFOPARAM_HAS_USING        = 0x00000002;
constexpr TypeParamFlags TYPEINFOPARAM_AUTO_NAME        = 0x00000004;
constexpr TypeParamFlags TYPEINFOPARAM_GENERIC_TYPE     = 0x00000008;
constexpr TypeParamFlags TYPEINFOPARAM_GENERIC_CONSTANT = 0x00000010;
constexpr TypeParamFlags TYPEINFOPARAM_FROM_GENERIC     = 0x00000020;

enum class ComputeNameKind
{
    Name,
    ScopedName,
    ScopedNameExport,
    DisplayName,
};

struct TypeInfo
{
    TypeInfo()          = default;
    virtual ~TypeInfo() = default;

    explicit TypeInfo(const char* name, TypeInfoKind kind) :
        name{name},
        kind{kind}
    {
    }

    explicit TypeInfo(TypeInfoKind kind) :
        kind{kind}
    {
    }

    bool isPointerTo(NativeTypeKind pointerKind);
    bool isPointerTo(TypeInfoKind pointerKind);
    bool isPointerTo(const TypeInfo* finalType);
    bool isPointerNull() const;
    bool isPointerVoid();
    bool isPointerConstVoid();
    bool isPointerToTypeInfo() const;
    bool isInitializerList() const;
    bool isArrayOfStruct() const;
    bool isArrayOfEnum() const;
    bool isMethod() const;

    TypeInfo*       getFinalType();
    TypeInfoStruct* getStructOrPointedStruct() const;
    TypeInfo*       getConstAlias();
    const TypeInfo* getConstAlias() const;
    TypeInfo*       getConcreteAlias() const;

    bool hasFlag(const TypeInfoFlags& fl) const { return flags.has(fl); }
    void addFlag(const TypeInfoFlags& fl) { flags.add(fl); }
    void removeFlag(const TypeInfoFlags& fl) { flags.remove(fl); }

    bool isSlice() const { return kind == TypeInfoKind::Slice; }
    bool isInterface() const { return kind == TypeInfoKind::Interface; }
    bool isStruct() const { return kind == TypeInfoKind::Struct; }
    bool isPointer() const { return kind == TypeInfoKind::Pointer; }
    bool isNative() const { return kind == TypeInfoKind::Native; }
    bool isArray() const { return kind == TypeInfoKind::Array; }
    bool isEnum() const { return kind == TypeInfoKind::Enum; }
    bool isNamespace() const { return kind == TypeInfoKind::Namespace; }
    bool isAlias() const { return kind == TypeInfoKind::Alias; }
    bool isFuncAttr() const { return kind == TypeInfoKind::FuncAttr; }
    bool isListArray() const { return kind == TypeInfoKind::TypeListArray; }
    bool isListTuple() const { return kind == TypeInfoKind::TypeListTuple; }
    bool isVariadic() const { return kind == TypeInfoKind::Variadic; }
    bool isTypedVariadic() const { return kind == TypeInfoKind::TypedVariadic; }
    bool isCVariadic() const { return kind == TypeInfoKind::CVariadic; }
    bool isCode() const { return kind == TypeInfoKind::Code; }
    bool isLambdaClosure() const { return kind == TypeInfoKind::LambdaClosure; }
    bool isKindGeneric() const { return kind == TypeInfoKind::Generic; }
    bool isNative(NativeTypeKind native) const { return kind == TypeInfoKind::Native && nativeType == native; }
    bool isAny() const { return isNative(NativeTypeKind::Any); }
    bool isString() const { return isNative(NativeTypeKind::String); }
    bool isBool() const { return isNative(NativeTypeKind::Bool); }
    bool isRune() const { return isNative(NativeTypeKind::Rune); }
    bool isVoid() const { return isNative(NativeTypeKind::Void); }
    bool isUndefined() const { return isNative(NativeTypeKind::Undefined); }
    bool isCString() const { return kind == TypeInfoKind::Pointer && flags.has(TYPEINFO_C_STRING); }
    bool isLambda() const { return kind == TypeInfoKind::LambdaClosure && !isClosure(); }
    bool isClosure() const { return flags.has(TYPEINFO_CLOSURE); }
    bool isNativeInteger() const { return flags.has(TYPEINFO_INTEGER); }
    bool isNativeIntegerUnsignedOrRune() const { return (flags.has(TYPEINFO_INTEGER) && flags.has(TYPEINFO_UNSIGNED)) || isRune(); }
    bool isNativeIntegerUnsigned() const { return flags.has(TYPEINFO_INTEGER) && flags.has(TYPEINFO_UNSIGNED); }
    bool isNativeIntegerSigned() const { return flags.has(TYPEINFO_INTEGER) && !flags.has(TYPEINFO_UNSIGNED); }
    bool isNativeIntegerOrRune() const { return flags.has(TYPEINFO_INTEGER) || isRune(); }
    bool isNativeFloat() const { return flags.has(TYPEINFO_FLOAT); }
    bool isConst() const { return flags.has(TYPEINFO_CONST); }
    bool isStrict() const { return flags.has(TYPEINFO_STRICT); }
    bool isGeneric() const { return flags.has(TYPEINFO_GENERIC); }
    bool isFromGeneric() const { return flags.has(TYPEINFO_FROM_GENERIC); }
    bool isTuple() const { return flags.has(TYPEINFO_STRUCT_IS_TUPLE); }
    bool isPointerRef() const { return flags.has(TYPEINFO_POINTER_REF); }
    bool isPointerMoveRef() const { return flags.has(TYPEINFO_POINTER_REF) && flags.has(TYPEINFO_POINTER_MOVE_REF); }
    bool isConstPointerRef() const { return flags.has(TYPEINFO_POINTER_REF) && flags.has(TYPEINFO_CONST); }
    bool isAutoConstPointerRef() const { return flags.has(TYPEINFO_POINTER_REF) && flags.has(TYPEINFO_CONST) && flags.has(TYPEINFO_POINTER_AUTO_REF); }
    bool isPointerArithmetic() const { return flags.has(TYPEINFO_POINTER_ARITHMETIC); }
    bool isSelf() const { return flags.has(TYPEINFO_IS_SELF); }
    bool isUntypedInteger() const { return flags.has(TYPEINFO_UNTYPED_INTEGER); }
    bool isUntypedFloat() const { return flags.has(TYPEINFO_UNTYPED_FLOAT); }
    bool isUntypedBinHex() const { return flags.has(TYPEINFO_UNTYPED_BIN_HEX); }
    bool isConstAlias() const { return flags.has(TYPEINFO_CONST_ALIAS); }
    bool isCharacter() const { return flags.has(TYPEINFO_CHARACTER); }
    bool isNullable() const;
    bool couldBeNull() const;

    virtual bool      isSame(const TypeInfo* from, CastFlags castFlags) const;
    virtual TypeInfo* clone() = 0;
    virtual uint32_t  numRegisters() const;
    virtual Utf8      getDisplayName();
    virtual void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind);

    const char* getDisplayNameC();
    void        copyFrom(const TypeInfo* from);
    void        setConst();

    void computeName() { computeWhateverName(ComputeNameKind::Name); }
    void computeScopedName() { computeWhateverName(ComputeNameKind::ScopedName); }
    void computeScopedNameExport() { computeWhateverName(ComputeNameKind::ScopedNameExport); }

    void        removeGenericFlag();
    void        clearName();
    void        forceComputeName();
    void        computeScopedName(Utf8& newName) const;
    Utf8        getName();
    Utf8        getScopedName();
    const Utf8& computeWhateverName(ComputeNameKind nameKind);
    const Utf8& computeWhateverNameNoLock(ComputeNameKind nameKind);

    mutable SharedMutex mutex;

    Utf8 name;
    Utf8 displayName;
    Utf8 scopedName;
    Utf8 scopedNameExport;

    AstNode*      declNode = nullptr;
    TypeInfoFlags flags    = 0;

    uint32_t       sizeOf     = 0;
    TypeInfoKind   kind       = TypeInfoKind::Invalid;
    NativeTypeKind nativeType = NativeTypeKind::Void;
    uint8_t        padding[2] = {};

    SWAG_RACE_CONDITION_INSTANCE(raceName);
};

struct TypeInfoParam
{
    uint32_t       numRegisters() const;
    bool           isSame(const TypeInfoParam* to, CastFlags castFlags) const;
    TypeInfoParam* clone() const;
    void           allocateComputedValue();

    Utf8          name;
    AttributeList attributes;

    ComputedValue* value    = nullptr;
    TypeInfo*      typeInfo = nullptr;
    AstNode*       declNode = nullptr;

    uint32_t       index   = 0;
    uint32_t       offset  = UINT32_MAX;
    TypeParamFlags flags   = 0;
    uint32_t       padding = 0;
};

struct TypeInfoNative final : TypeInfo
{
    TypeInfoNative() :
        TypeInfo{TypeInfoKind::Native}
    {
        valueInteger = 0;
    }

    TypeInfoNative(NativeTypeKind type, int sof, const TypeInfoFlags& fl)
    {
        kind         = TypeInfoKind::Native;
        nativeType   = type;
        sizeOf       = sof;
        flags        = fl;
        valueInteger = 0;
        computeWhateverName(name, ComputeNameKind::Name);
    }

    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;
    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;

    union
    {
        int32_t valueInteger;
        float   valueFloat;
    };
};

struct TypeInfoNamespace final : TypeInfo
{
    TypeInfoNamespace() :
        TypeInfo{TypeInfoKind::Namespace}
    {
    }

    Utf8      getDisplayName() override;
    TypeInfo* clone() override;

    Scope* scope = nullptr;
};

struct TypeInfoEnum final : TypeInfo
{
    TypeInfoEnum() :
        TypeInfo{TypeInfoKind::Enum}
    {
    }

    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    Utf8      getDisplayName() override;
    TypeInfo* clone() override;

    bool contains(const Utf8& valueName);
    void collectEnums(VectorNative<TypeInfoEnum*>& collect);

    VectorNative<TypeInfoParam*> values;
    AttributeList                attributes;

    Scope*    scope   = nullptr;
    TypeInfo* rawType = nullptr;
};

struct TypeInfoFuncAttr final : TypeInfo
{
    TypeInfoFuncAttr() :
        TypeInfo{TypeInfoKind::FuncAttr}
    {
        sizeOf = sizeof(void*);
    }

    uint32_t  numRegisters() const override { return 1; }
    TypeInfo* clone() override;
    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;
    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;

    bool            isSame(const TypeInfoFuncAttr* other, CastFlags castFlags, BadSignatureInfos& bi) const;
    bool            isSame(const TypeInfoFuncAttr* other, CastFlags castFlags) const;
    TypeInfo*       concreteReturnType() const;
    bool            isFctVariadic() const;
    bool            isFctCVariadic() const;
    uint32_t        registerIdxToParamIdx(uint32_t argIdx) const;
    TypeInfo*       registerIdxToType(uint32_t argIdx) const;
    uint32_t        numParamsRegisters() const;
    uint32_t        numReturnRegisters() const;
    uint32_t        numTotalRegisters() const;
    const CallConv* getCallConv() const;
    bool            structParamByValue(const TypeInfo* typeParam) const;
    bool            returnByAddress() const;
    bool            returnByStackAddress() const;
    bool            returnNeedsStack() const;
    bool            returnByValue() const;
    bool            returnStructByValue() const;

    VectorNative<TypeInfoParam*>        capture;
    VectorNative<TypeInfoParam*>        genericParameters;
    VectorNative<TypeInfoParam*>        parameters;
    AttributeList                       attributes;
    VectorMap<Utf8, GenericReplaceType> replaceTypes;
    VectorMap<Utf8, ComputedValue*>     replaceValues;

    TypeInfo* returnType = nullptr;

    uint32_t       firstDefaultValueIdx = UINT32_MAX;
    AttrUsageFlags attributeUsage       = ATTR_USAGE_ALL;
    CallConvKind   callConv             = CallConvKind::Swag;
};

struct TypeInfoPointer final : TypeInfo
{
    TypeInfoPointer() :
        TypeInfo{TypeInfoKind::Pointer}
    {
    }

    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;
    Utf8      getDisplayName() override;
    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

struct TypeInfoArray final : TypeInfo
{
    TypeInfoArray() :
        TypeInfo{TypeInfoKind::Array}
    {
    }

    uint32_t  numRegisters() const override { return 1; }
    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;
    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
    TypeInfo* finalType   = nullptr;
    AstNode*  sizeNode    = nullptr;

    uint32_t count      = 0;
    uint32_t totalCount = 0;
};

struct TypeInfoSlice final : TypeInfo
{
    TypeInfoSlice() :
        TypeInfo{TypeInfoKind::Slice}
    {
        sizeOf = 2 * sizeof(void*);
    }

    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;
    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

struct TypeInfoList final : TypeInfo
{
    TypeInfoList() = default;

    uint32_t  numRegisters() const override { return 1; }
    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;
    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;

    VectorNative<TypeInfoParam*> subTypes;

    Scope* scope = nullptr;
};

struct TypeInfoVariadic final : TypeInfo
{
    TypeInfoVariadic() :
        TypeInfo{TypeInfoKind::Variadic}
    {
    }

    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;
    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoGeneric final : TypeInfo
{
    TypeInfoGeneric() :
        TypeInfo{TypeInfoKind::Generic}
    {
        flags.add(TYPEINFO_GENERIC);
    }

    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoStruct final : TypeInfo
{
    TypeInfoStruct() :
        TypeInfo{TypeInfoKind::Struct}
    {
    }

    uint32_t numRegisters() const override
    {
        if (kind == TypeInfoKind::Interface)
            return 2;
        return 1;
    }

    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;
    Utf8      getDisplayName() override;
    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;

    TypeInfoParam* findChildByNameNoLock(const Utf8& childName) const;
    TypeInfoParam* hasInterface(const TypeInfoStruct* itf) const;
    TypeInfoParam* hasInterfaceNoLock(const TypeInfoStruct* itf) const;
    static Utf8    computeTupleDisplayName(const VectorNative<TypeInfoParam*>& fields, ComputeNameKind nameKind);
    bool           canRawCopy() const;
    bool           isPlainOldData() const;
    void           flattenUsingFields();
    void           collectUsingFields(VectorNative<std::pair<TypeInfoParam*, uint32_t>>& result, uint32_t offset = 0);

    VectorNative<TypeInfoParam*>          genericParameters;
    VectorNative<TypeInfo*>               deducedGenericParameters;
    VectorNative<TypeInfoParam*>          fields;
    VectorNative<TypeInfoParam*>          constDecl;
    VectorNative<TypeInfoParam*>          methods;
    VectorNative<TypeInfoParam*>          interfaces;
    VectorNative<TypeInfoParam*>          flattenFields;
    VectorMap<Utf8, GenericReplaceType>   replaceTypes;
    VectorMap<Utf8, ComputedValue*>       replaceValues;
    VectorMap<TypeInfo*, SymbolOverload*> mapOpCast;
    VectorMap<TypeInfo*, SymbolOverload*> mapOpAffect[4];
    AttributeList                         attributes;
    Utf8                                  structName;
    SharedMutex                           mutexCache;
    SharedMutex                           mutexGen;

    TypeInfoStruct* itable            = nullptr;
    TypeInfoStruct* fromGeneric       = nullptr;
    Scope*          scope             = nullptr;
    ByteCode*       opInit            = nullptr;
    AstFuncDecl*    opUserInitFct     = nullptr;
    ByteCode*       opDrop            = nullptr;
    AstFuncDecl*    opUserDropFct     = nullptr;
    ByteCode*       opPostCopy        = nullptr;
    AstFuncDecl*    opUserPostCopyFct = nullptr;
    ByteCode*       opPostMove        = nullptr;
    AstFuncDecl*    opUserPostMoveFct = nullptr;

    uint32_t alignOf                    = 0;
    uint32_t cptRemainingInterfaces     = 0;
    uint32_t cptRemainingInterfacesReg  = 0;
    uint32_t cptRemainingMethods        = 0;
    uint32_t cptRemainingSpecialMethods = 0;

    SWAG_RACE_CONDITION_INSTANCE(raceFields);
};

struct TypeInfoAlias final : TypeInfo
{
    TypeInfoAlias() :
        TypeInfo{TypeInfoKind::Alias}
    {
    }

    Utf8      getDisplayName() override;
    void      computeWhateverName(Utf8& resName, ComputeNameKind nameKind) override;
    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;

    uint32_t numRegisters() const override
    {
        return rawType->numRegisters();
    }

    Utf8      aliasName;
    TypeInfo* rawType = nullptr;
};

struct TypeInfoCode final : TypeInfo
{
    TypeInfoCode() :
        TypeInfo{"code", TypeInfoKind::Code}
    {
    }

    bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
    TypeInfo* clone() override;

    AstNode* content = nullptr;
};

template<typename T>
T* castTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
    SWAG_ASSERT(ptr);
    T* cast = static_cast<T*>(ptr->getConstAlias());
    SWAG_ASSERT(cast->kind == kind);
    return cast;
}

template<typename T>
T* castTypeInfo(TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
    SWAG_ASSERT(ptr);
    T* cast = static_cast<T*>(ptr->getConstAlias());
    SWAG_ASSERT(cast->kind == kind1 || cast->kind == kind2);
    return cast;
}

template<typename T>
const T* castTypeInfo(const TypeInfo* ptr, TypeInfoKind kind)
{
    SWAG_ASSERT(ptr);
    const T* cast = static_cast<const T*>(ptr->getConstAlias());
    SWAG_ASSERT(cast->kind == kind);
    return cast;
}

template<typename T>
const T* castTypeInfo(const TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
    SWAG_ASSERT(ptr);
    const T* cast = static_cast<const T*>(ptr->getConstAlias());
    SWAG_ASSERT(cast->kind == kind1 || cast->kind == kind2);
    return cast;
}

template<typename T>
T* castTypeInfo(TypeInfo* ptr)
{
    return static_cast<T*>(ptr);
}

template<typename T>
const T* castTypeInfo(const TypeInfo* ptr)
{
    return static_cast<const T*>(ptr);
}

#pragma once
#include "Utf8.h"
#include "Log.h"
#include "Register.h"
#include "Attribute.h"
#include "VectorNative.h"
#include "Runtime.h"
#include "CommandLine.h"
#include "Mutex.h"
#include "RaceCondition.h"
#include "CallConv.h"

struct Scope;
struct TypeInfo;
struct SymbolMatchContext;
struct Job;
struct AstNode;
struct ByteCode;
struct TypeInfo;
struct TypeInfoParam;
struct TypeInfoFuncAttr;
struct AstFuncDecl;
struct JobContext;
struct SemanticContext;
struct TypeInfoStruct;
enum class Intrisic;

static const int COMPUTE_NAME               = 0;
static const int COMPUTE_SCOPED_NAME        = 1;
static const int COMPUTE_SCOPED_NAME_EXPORT = 2;
static const int COMPUTE_DISPLAY_NAME       = 3;

static const uint64_t TYPEINFO_SELF                     = 0x00000000'00000001;
static const uint64_t TYPEINFO_UNTYPED_BINHEXA          = 0x00000000'00000002;
static const uint64_t TYPEINFO_INTEGER                  = 0x00000000'00000004;
static const uint64_t TYPEINFO_FLOAT                    = 0x00000000'00000008;
static const uint64_t TYPEINFO_UNSIGNED                 = 0x00000000'00000010;
static const uint64_t TYPEINFO_CONST                    = 0x00000000'00000020;
static const uint64_t TYPEINFO_AUTO_NAME                = 0x00000000'00000040;
static const uint64_t TYPEINFO_VARIADIC                 = 0x00000000'00000080;
static const uint64_t TYPEINFO_STRUCT_HAS_INIT_VALUES   = 0x00000000'00000100;
static const uint64_t TYPEINFO_STRUCT_ALL_UNINITIALIZED = 0x00000000'00000200;
static const uint64_t TYPEINFO_STRUCT_NO_POST_COPY      = 0x00000000'00000400;
static const uint64_t TYPEINFO_STRUCT_NO_POST_MOVE      = 0x00000000'00000800;
static const uint64_t TYPEINFO_STRUCT_NO_DROP           = 0x00000000'00001000;
static const uint64_t TYPEINFO_GENERIC                  = 0x00000000'00002000;
static const uint64_t TYPEINFO_RETURN_BY_COPY           = 0x00000000'00004000;
static const uint64_t TYPEINFO_UNTYPED_INTEGER          = 0x00000000'00008000;
static const uint64_t TYPEINFO_UNTYPED_FLOAT            = 0x00000000'00010000;
static const uint64_t TYPEINFO_DEFINED_VALUE            = 0x00000000'00020000;
static const uint64_t TYPEINFO_AUTO_CAST                = 0x00000000'00040000;
static const uint64_t TYPEINFO_TYPED_VARIADIC           = 0x00000000'00080000;
static const uint64_t TYPEINFO_STRUCT_TYPEINFO          = 0x00000000'00100000;
static const uint64_t TYPEINFO_STRUCT_IS_TUPLE          = 0x00000000'00200000;
static const uint64_t TYPEINFO_ENUM_FLAGS               = 0x00000000'00400000;
static const uint64_t TYPEINFO_SPREAD                   = 0x00000000'00800000;
static const uint64_t TYPEINFO_UNDEFINED                = 0x00000000'01000000;
static const uint64_t TYPEINFO_ENUM_INDEX               = 0x00000000'02000000;
static const uint64_t TYPEINFO_STRICT                   = 0x00000000'04000000;
static const uint64_t TYPEINFO_FAKE_ALIAS               = 0x00000000'08000000;
static const uint64_t TYPEINFO_HAS_USING                = 0x00000000'10000000;
static const uint64_t TYPEINFO_FUNC_IS_ATTR             = 0x00000000'20000000;
static const uint64_t TYPEINFO_FROM_GENERIC             = 0x00000000'40000000;
static const uint64_t TYPEINFO_STRUCT_NO_COPY           = 0x00000000'80000000;
static const uint64_t TYPEINFO_CAN_THROW                = 0x00000001'00000000;
static const uint64_t TYPEINFO_GENERIC_COUNT            = 0x00000002'00000000;
static const uint64_t TYPEINFO_SHARED                   = 0x00000004'00000000;
static const uint64_t TYPEINFO_C_STRING                 = 0x00000008'00000000;
static const uint64_t TYPEINFO_C_VARIADIC               = 0x00000010'00000000;
static const uint64_t TYPEINFO_GENERATED_TUPLE          = 0x00000020'00000000;
static const uint64_t TYPEINFO_CLOSURE                  = 0x00000040'00000000;
static const uint64_t TYPEINFO_INCOMPLETE               = 0x00000080'00000000;
static const uint64_t TYPEINFO_STRUCT_IS_ITABLE         = 0x00000100'00000000;
static const uint64_t TYPEINFO_CAN_PROMOTE_816          = 0x00000200'00000000;
static const uint64_t TYPEINFO_POINTER_ARITHMETIC       = 0x00000400'00000000;

static const uint32_t ISSAME_EXACT       = 0x00000001;
static const uint32_t ISSAME_CAST        = 0x00000002;
static const uint32_t ISSAME_INTERFACE   = 0x00000004;
static const uint32_t ISSAME_FOR_AFFECT  = 0x00000008;
static const uint32_t ISSAME_FOR_GENERIC = 0x00000010;

enum class MatchResult
{
    Ok,
    TooManyParameters,
    TooManyGenericParameters,
    NotEnoughParameters,
    MissingParameters,
    MissingSomeParameters,
    NotEnoughGenericParameters,
    BadSignature,
    BadGenericMatch,
    BadGenericSignature,
    InvalidNamedParameter,
    MissingNamedParameter,
    DuplicatedNamedParameter,
    MismatchGenericValue,
    SelectIfFailed,
};

struct BadSignatureInfos
{
    Utf8           badGenMatch;
    AstNode*       badNode;
    TypeInfo*      badSignatureRequestedType;
    TypeInfo*      badSignatureGivenType;
    TypeInfo*      castErrorToType;
    TypeInfo*      castErrorFromType;
    ComputedValue* badGenValue1;
    ComputedValue* badGenValue2;

    uint32_t castErrorFlags;
    int      badSignatureParameterIdx;
    int      badSignatureNum1;
    int      badSignatureNum2;

    void clear()
    {
        badGenMatch.clear();
        badSignatureParameterIdx  = -1;
        badNode                   = nullptr;
        badSignatureRequestedType = nullptr;
        badSignatureGivenType     = nullptr;
        badGenValue1              = nullptr;
        badGenValue2              = nullptr;
        badSignatureNum1          = 0;
        badSignatureNum2          = 0;
        castErrorFlags            = 0;
        castErrorToType           = nullptr;
        castErrorFromType         = nullptr;
    }
};

struct SymbolMatchContext
{
    static const uint32_t MATCH_ACCEPT_NO_GENERIC        = 0x00000001;
    static const uint32_t MATCH_FOR_LAMBDA               = 0x00000002;
    static const uint32_t MATCH_GENERIC_AUTO             = 0x00000004;
    static const uint32_t MATCH_ERROR_VALUE_TYPE         = 0x00000008;
    static const uint32_t MATCH_ERROR_TYPE_VALUE         = 0x00000010;
    static const uint32_t MATCH_UNCONST                  = 0x00000020;
    static const uint32_t MATCH_UFCS                     = 0x00000040;
    static const uint32_t MATCH_CLOSURE_PARAM            = 0x00000080;
    static const uint32_t MATCH_DO_NOT_ACCEPT_NO_GENERIC = 0x00000100;

    SymbolMatchContext()
    {
        reset();
    }

    void reset()
    {
        genericParameters.clear();
        parameters.clear();
        solvedParameters.clear();
        doneParameters.clear();
        genericParametersCallTypes.clear();
        genericParametersGenTypes.clear();
        genericReplaceTypes.clear();
        mapGenericTypesIndex.clear();
        badSignatureInfos.clear();
        flags              = 0;
        result             = MatchResult::Ok;
        cptResolved        = 0;
        hasNamedParameters = false;
        semContext         = nullptr;
    }

    void resetTmp()
    {
        cptResolved        = 0;
        hasNamedParameters = false;
    }

    VectorNative<AstNode*>                     genericParameters;
    VectorNative<AstNode*>                     parameters;
    VectorNative<TypeInfoParam*>               solvedParameters;
    VectorNative<bool>                         doneParameters;
    VectorNative<TypeInfo*>                    genericParametersCallTypes;
    VectorNative<TypeInfo*>                    genericParametersGenTypes;
    map<Utf8, TypeInfo*>                       genericReplaceTypes;
    map<Utf8, uint32_t>                        mapGenericTypesIndex;
    map<Utf8, pair<ComputedValue*, TypeInfo*>> genericReplaceValues;
    BadSignatureInfos                          badSignatureInfos;

    SemanticContext* semContext = nullptr;

    uint32_t    flags;
    MatchResult result;
    int         cptResolved;

    bool hasNamedParameters;
};

struct TypeInfo
{
    // clang-format off
    TypeInfo() = default;
    TypeInfo(const char* name, TypeInfoKind kind) : name{ name }, kind { kind } {}
    TypeInfo(TypeInfoKind kind) : kind{kind} {}
    // clang-format on

    bool            isPointerTo(NativeTypeKind pointerKind);
    bool            isPointerTo(TypeInfoKind pointerKind);
    bool            isPointerTo(TypeInfo* finalType);
    bool            isPointerVoid();
    bool            isPointerConstVoid();
    bool            isPointerToTypeInfo();
    bool            isCString();
    bool            isInitializerList();
    bool            isArrayOfStruct();
    bool            isArrayOfEnum();
    bool            isMethod();
    bool            isClosure();
    bool            isLambda();
    TypeInfoStruct* getStructOrPointedStruct();

    // clang-format off
    bool isNative(NativeTypeKind native)    { return (kind == TypeInfoKind::Native) && (nativeType == native); }
    bool isNativeInteger()                  { return (flags & TYPEINFO_INTEGER); }
    bool isNativeIntegerUnsignedOrRune()    { return ((flags & TYPEINFO_INTEGER) && (flags & TYPEINFO_UNSIGNED)) || isNative(NativeTypeKind::Rune); }
    bool isNativeIntegerUnsigned()          { return (flags & TYPEINFO_INTEGER) && (flags & TYPEINFO_UNSIGNED); }
    bool isNativeIntegerSigned()            { return (flags & TYPEINFO_INTEGER) && !(flags & TYPEINFO_UNSIGNED); }
    bool isNativeIntegerOrRune()            { return (flags & TYPEINFO_INTEGER) || isNative(NativeTypeKind::Rune); }
    bool isNativeFloat()                    { return (flags & TYPEINFO_FLOAT); }
    bool isConst()                          { return (flags & TYPEINFO_CONST); }
    bool isStrict()                         { return (flags & TYPEINFO_STRICT); }
    bool isGeneric()                        { return (flags & TYPEINFO_GENERIC); }
    bool isSlice()                          { return kind == TypeInfoKind::Slice; }
    bool isInterface()                      { return kind == TypeInfoKind::Interface; }
    // clang-format on

    virtual bool        isSame(TypeInfo* from, uint32_t isSameFlags);
    virtual TypeInfo*   clone() = 0;
    virtual int         numRegisters();
    virtual Utf8        getDisplayName();
    virtual const char* getDisplayNameC();
    virtual void        computeWhateverName(Utf8& resName, uint32_t nameType);

    void copyFrom(TypeInfo* from);
    void setConst();

    // clang-format off
    void            computeName() { computeWhateverName(COMPUTE_NAME); }
    void            computeScopedName() { computeWhateverName(COMPUTE_SCOPED_NAME); }
    void            computeScopedNameExport() { computeWhateverName(COMPUTE_SCOPED_NAME_EXPORT); }
    // clang-format on

    void               removeGenericFlag();
    void               clearName();
    void               forceComputeName();
    void               getScopedName(Utf8& name);
    Utf8               getName();
    const Utf8&        computeWhateverName(uint32_t nameType);
    const Utf8&        computeWhateverNameNoLock(uint32_t nameType);
    static const char* getArticleKindName(TypeInfo* typeInfo);
    static const char* getNakedKindName(TypeInfo* typeInfo);

    SharedMutex mutex;

    Utf8 name;
    Utf8 displayName;
    Utf8 scopedName;
    Utf8 scopedNameExport;

    AstNode*  declNode     = nullptr;
    uint64_t  flags        = 0;
    TypeInfo* promotedFrom = nullptr;

    uint32_t       sizeOf     = 0;
    TypeInfoKind   kind       = TypeInfoKind::Invalid;
    NativeTypeKind nativeType = NativeTypeKind::Void;
    uint8_t        padding[2];

    SWAG_RACE_CONDITION_INSTANCE(raceName);
};

struct TypeInfoParam
{
    int            numRegisters();
    bool           isSame(TypeInfoParam* to, uint32_t isSameFlags);
    TypeInfoParam* clone();
    void           allocateComputedValue();

    Utf8          name;
    Utf8          namedParam;
    AttributeList attributes;

    ComputedValue* value    = nullptr;
    TypeInfo*      typeInfo = nullptr;
    AstNode*       declNode = nullptr;

    uint64_t flags  = 0;
    int      index  = 0;
    int      offset = 0;
};

struct TypeInfoNative : public TypeInfo
{
    TypeInfoNative()
        : TypeInfo{TypeInfoKind::Native}
    {
        valueInteger = 0;
    }

    TypeInfoNative(NativeTypeKind type, const char* tname, int sof, uint64_t fl)
    {
        kind       = TypeInfoKind::Native;
        nativeType = type;
        name.setView(tname, (int) strlen(tname));
        sizeOf       = sof;
        flags        = fl;
        valueInteger = 0;
    }

    bool      isSame(TypeInfo* from, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    union
    {
        int32_t valueInteger;
        float   valueFloat;
    };
};

struct TypeInfoNamespace : public TypeInfo
{
    TypeInfoNamespace()
        : TypeInfo{TypeInfoKind::Namespace}
    {
    }

    TypeInfo* clone() override;

    Scope* scope = nullptr;
};

struct TypeInfoEnum : public TypeInfo
{
    TypeInfoEnum()
        : TypeInfo{TypeInfoKind::Enum}
    {
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;
    bool      contains(const Utf8& valueName);

    VectorNative<TypeInfoParam*> values;
    AttributeList                attributes;

    Scope*    scope   = nullptr;
    TypeInfo* rawType = nullptr;
};

struct TypeInfoFuncAttr : public TypeInfo
{
    TypeInfoFuncAttr()
        : TypeInfo{TypeInfoKind::FuncAttr}
    {
    }

    TypeInfo* clone() override;
    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* from, uint32_t isSameFlags) override;
    bool      isSame(TypeInfoFuncAttr* from, uint32_t isSameFlags);
    void      match(SymbolMatchContext& context);
    bool      returnByCopy();
    bool      returnByValue();
    TypeInfo* concreteReturnType();
    bool      isVariadic();
    bool      isCVariadic();
    uint32_t  registerIdxToParamIdx(int argIdx);
    TypeInfo* registerIdxToType(int argIdx);
    int       numParamsRegisters();
    int       numReturnRegisters();
    int       numTotalRegisters();

    VectorNative<TypeInfoParam*> capture;
    VectorNative<TypeInfoParam*> genericParameters;
    VectorNative<TypeInfoParam*> parameters;
    AttributeList                attributes;
    map<Utf8, TypeInfo*>         replaceTypes;

    TypeInfo* returnType = nullptr;

    int               firstDefaultValueIdx = -1;
    int               stackSize            = 0;
    uint32_t          attributeUsage       = AttributeUsage::All;
    CallingConvention callConv             = CallingConvention::Swag;
};

struct TypeInfoPointer : public TypeInfo
{
    TypeInfoPointer()
        : TypeInfo{TypeInfoKind::Pointer}
    {
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

struct TypeInfoReference : public TypeInfo
{
    TypeInfoReference()
        : TypeInfo{TypeInfoKind::Reference}
    {
        sizeOf = sizeof(void*);
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

struct TypeInfoArray : public TypeInfo
{
    TypeInfoArray()
        : TypeInfo{TypeInfoKind::Array}
    {
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    int numRegisters() override
    {
        return 1;
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
    TypeInfo* finalType   = nullptr;
    AstNode*  sizeNode    = nullptr;

    uint32_t count      = 0;
    uint32_t totalCount = 0;
};

struct TypeInfoSlice : public TypeInfo
{
    TypeInfoSlice()
        : TypeInfo{TypeInfoKind::Slice}
    {
        sizeOf = 2 * sizeof(void*);
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

struct TypeInfoList : public TypeInfo
{
    TypeInfoList()
    {
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    int numRegisters() override
    {
        return 1;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;
    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    Utf8      computeTupleName(JobContext* context);

    VectorNative<TypeInfoParam*> subTypes;

    Scope* scope = nullptr;
};

struct TypeInfoVariadic : public TypeInfo
{
    TypeInfoVariadic()
        : TypeInfo{TypeInfoKind::Variadic}
    {
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;
    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoGeneric : public TypeInfo
{
    TypeInfoGeneric()
        : TypeInfo{TypeInfoKind::Generic}
    {
        flags |= TYPEINFO_GENERIC;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoStruct : public TypeInfo
{
    TypeInfoStruct()
        : TypeInfo{TypeInfoKind::Struct}
    {
    }

    int numRegisters() override
    {
        if (kind == TypeInfoKind::Interface)
            return 2;
        return 1;
    }

    bool           isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo*      clone() override;
    void           computeWhateverName(Utf8& resName, uint32_t nameType) override;
    void           match(SymbolMatchContext& context);
    TypeInfoParam* findChildByNameNoLock(const Utf8& childName);
    TypeInfoParam* hasInterface(TypeInfoStruct* itf);
    TypeInfoParam* hasInterfaceNoLock(TypeInfoStruct* itf);
    const char*    getDisplayNameC() override;
    Utf8           getDisplayName() override;
    bool           canRawCopy();
    bool           isPlainOldData();

    VectorNative<TypeInfoParam*> genericParameters;
    VectorNative<TypeInfo*>      deducedGenericParameters;
    VectorNative<TypeInfoParam*> fields;
    VectorNative<TypeInfoParam*> consts;
    VectorNative<TypeInfoParam*> methods;
    VectorNative<TypeInfoParam*> interfaces;
    map<Utf8, TypeInfo*>         replaceTypes;
    AttributeList                attributes;
    Utf8                         structName;
    SharedMutex                  mutexGen;

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
};

struct TypeInfoAlias : public TypeInfo
{
    TypeInfoAlias()
        : TypeInfo{TypeInfoKind::Alias}
    {
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    int numRegisters() override
    {
        if (flags & TYPEINFO_FAKE_ALIAS)
            return rawType->numRegisters();
        return TypeInfo::numRegisters();
    }

    TypeInfo* rawType = nullptr;
};

struct TypeInfoCode : public TypeInfo
{
    TypeInfoCode()
        : TypeInfo{"code", TypeInfoKind::Code}
    {
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    AstNode* content = nullptr;
};

template<typename T>
T* allocType(TypeInfoKind k = TypeInfoKind::Invalid)
{
    auto newType = g_Allocator.alloc<T>();
    if (k != TypeInfoKind::Invalid)
        newType->kind = k;
    if (g_CommandLine->stats)
    {
        g_Stats.memTypes += sizeof(T);
#ifdef SWAG_DEV_MODE
        SWAG_ASSERT(newType->kind != TypeInfoKind::Invalid);
        SWAG_ASSERT((int) newType->kind < sizeof(g_Stats.countTypesByKind) / sizeof(g_Stats.countTypesByKind[0]));
        g_Stats.countTypesByKind[(int) newType->kind] += 1;
#endif
    }
    return newType;
}

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
    SWAG_ASSERT(ptr);
    T* casted;
    if (ptr->flags & TYPEINFO_FAKE_ALIAS)
        casted = static_cast<T*>(((TypeInfoAlias*) ptr)->rawType);
    else
        casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted->kind == kind);
    return casted;
}

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
    SWAG_ASSERT(ptr);
    T* casted;
    if (ptr->flags & TYPEINFO_FAKE_ALIAS)
        casted = static_cast<T*>(((TypeInfoAlias*) ptr)->rawType);
    else
        casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted->kind == kind1 || casted->kind == kind2);
    return casted;
}

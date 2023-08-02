#pragma once
#include "Utf8.h"
#include "Register.h"
#include "Attribute.h"
#include "VectorNative.h"
#include "Runtime.h"
#include "CommandLine.h"
#include "Mutex.h"
#include "CallConv.h"
#include "Map.h"

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
struct BadSignatureInfos;
struct SymbolOverload;
enum class Intrisic;

const int COMPUTE_NAME               = 0;
const int COMPUTE_SCOPED_NAME        = 1;
const int COMPUTE_SCOPED_NAME_EXPORT = 2;
const int COMPUTE_DISPLAY_NAME       = 3;

const uint64_t TYPEINFO_SELF                     = 0x00000000'00000001;
const uint64_t TYPEINFO_UNTYPED_BINHEXA          = 0x00000000'00000002;
const uint64_t TYPEINFO_INTEGER                  = 0x00000000'00000004;
const uint64_t TYPEINFO_FLOAT                    = 0x00000000'00000008;
const uint64_t TYPEINFO_UNSIGNED                 = 0x00000000'00000010;
const uint64_t TYPEINFO_CONST                    = 0x00000000'00000020;
const uint64_t TYPEINFO_AUTO_NAME                = 0x00000000'00000040;
const uint64_t TYPEINFO_VARIADIC                 = 0x00000000'00000080;
const uint64_t TYPEINFO_STRUCT_HAS_INIT_VALUES   = 0x00000000'00000100;
const uint64_t TYPEINFO_STRUCT_ALL_UNINITIALIZED = 0x00000000'00000200;
const uint64_t TYPEINFO_STRUCT_NO_POST_COPY      = 0x00000000'00000400;
const uint64_t TYPEINFO_STRUCT_NO_POST_MOVE      = 0x00000000'00000800;
const uint64_t TYPEINFO_STRUCT_NO_DROP           = 0x00000000'00001000;
const uint64_t TYPEINFO_GENERIC                  = 0x00000000'00002000;
const uint64_t TYPEINFO_POINTER_MOVE_REF         = 0x00000000'00004000;
const uint64_t TYPEINFO_UNTYPED_INTEGER          = 0x00000000'00008000;
const uint64_t TYPEINFO_UNTYPED_FLOAT            = 0x00000000'00010000;
const uint64_t TYPEINFO_DEFINED_VALUE            = 0x00000000'00020000;
const uint64_t TYPEINFO_AUTO_CAST                = 0x00000000'00040000;
const uint64_t TYPEINFO_TYPED_VARIADIC           = 0x00000000'00080000;
const uint64_t TYPEINFO_STRUCT_TYPEINFO          = 0x00000000'00100000;
const uint64_t TYPEINFO_STRUCT_IS_TUPLE          = 0x00000000'00200000;
const uint64_t TYPEINFO_ENUM_FLAGS               = 0x00000000'00400000;
const uint64_t TYPEINFO_SPREAD                   = 0x00000000'00800000;
const uint64_t TYPEINFO_UNDEFINED                = 0x00000000'01000000;
const uint64_t TYPEINFO_ENUM_INDEX               = 0x00000000'02000000;
const uint64_t TYPEINFO_STRICT                   = 0x00000000'04000000;
const uint64_t TYPEINFO_FAKE_ALIAS               = 0x00000000'08000000;
const uint64_t TYPEINFO_HAS_USING                = 0x00000000'10000000;
const uint64_t TYPEINFO_FUNC_IS_ATTR             = 0x00000000'20000000;
const uint64_t TYPEINFO_FROM_GENERIC             = 0x00000000'40000000;
const uint64_t TYPEINFO_STRUCT_NO_COPY           = 0x00000000'80000000;
const uint64_t TYPEINFO_CAN_THROW                = 0x00000001'00000000;
const uint64_t TYPEINFO_GENERIC_COUNT            = 0x00000002'00000000;
const uint64_t TYPEINFO_POINTER_ACCEPT_MOVE_REF  = 0x00000004'00000000;
const uint64_t TYPEINFO_C_STRING                 = 0x00000008'00000000;
const uint64_t TYPEINFO_C_VARIADIC               = 0x00000010'00000000;
const uint64_t TYPEINFO_GENERATED_TUPLE          = 0x00000020'00000000;
const uint64_t TYPEINFO_CLOSURE                  = 0x00000040'00000000;
const uint64_t TYPEINFO_INCOMPLETE               = 0x00000080'00000000;
const uint64_t TYPEINFO_STRUCT_IS_ITABLE         = 0x00000100'00000000;
const uint64_t TYPEINFO_CAN_PROMOTE_816          = 0x00000200'00000000;
const uint64_t TYPEINFO_POINTER_ARITHMETIC       = 0x00000400'00000000;
const uint64_t TYPEINFO_LISTARRAY_ARRAY          = 0x00000800'00000000;
const uint64_t TYPEINFO_POINTER_REF              = 0x00001000'00000000;
const uint64_t TYPEINFO_POINTER_AUTO_REF         = 0x00002000'00000000;
const uint64_t TYPEINFO_STRUCT_NO_INIT           = 0x00004000'00000000;
const uint64_t TYPEINFO_SPECOP_GENERATED         = 0x00008000'00000000;
const uint64_t TYPEINFO_STRUCT_EMPTY             = 0x00010000'00000000;

const uint64_t TYPEINFOPARAM_DEFINED_VALUE = 0x00000000'00000001;
const uint64_t TYPEINFOPARAM_HAS_USING     = 0x00000000'00000002;
const uint64_t TYPEINFOPARAM_AUTO_NAME     = 0x00000000'00000004;

enum class MatchResult
{
    Ok,
    TooManyParameters,
    TooManyGenericParameters,
    NotEnoughParameters,
    MissingParameters,
    NotEnoughGenericParameters,
    BadSignature,
    BadGenericType,
    CannotDeduceGenericType,
    BadGenericSignature,
    InvalidNamedParameter,
    MissingNamedParameter,
    DuplicatedNamedParameter,
    MismatchGenericValue,
    ValidIfFailed,
    MismatchThrow,
    NoReturnType,
    MissingReturnType,
    MismatchReturnType,
};

enum class CastErrorType
{
    Zero = 0,
    Const,
    SliceArray,
    SafetyCastAny,
};

struct BadSignatureInfos
{
    Utf8           badGenMatch;
    AstNode*       badNode                   = nullptr;
    AstNode*       genMatchFromNode          = nullptr;
    TypeInfo*      badSignatureRequestedType = nullptr;
    TypeInfo*      badSignatureGivenType     = nullptr;
    TypeInfo*      castErrorToType           = nullptr;
    TypeInfo*      castErrorFromType         = nullptr;
    ComputedValue* badGenValue1              = nullptr;
    ComputedValue* badGenValue2              = nullptr;
    MatchResult    matchResult               = MatchResult::Ok;

    uint64_t      castErrorFlags           = 0;
    CastErrorType castErrorType            = CastErrorType::Zero;
    int           badSignatureParameterIdx = 0;
    int           badSignatureNum1         = 0;
    int           badSignatureNum2         = 0;

    void clear()
    {
        badGenMatch.clear();
        badNode                   = nullptr;
        genMatchFromNode          = nullptr;
        badSignatureRequestedType = nullptr;
        badSignatureGivenType     = nullptr;
        castErrorToType           = nullptr;
        castErrorFromType         = nullptr;
        badGenValue1              = nullptr;
        badGenValue2              = nullptr;
        matchResult               = MatchResult::Ok;
        castErrorFlags            = 0;
        castErrorType             = CastErrorType::Zero;
        badSignatureParameterIdx  = -1;
        badSignatureNum1          = 0;
        badSignatureNum2          = 0;
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
        solvedCallParameters.clear();
        doneParameters.clear();
        genericParametersCallTypes.clear();
        genericParametersCallTypesFrom.clear();
        genericParametersGenTypes.clear();
        genericReplaceTypes.clear();
        genericReplaceTypesFrom.clear();
        mapGenericTypesIndex.clear();
        genericReplaceValues.clear();
        badSignatureInfos.clear();

        semContext   = nullptr;
        flags        = 0;
        result       = MatchResult::Ok;
        cptResolved  = 0;
        firstDefault = UINT32_MAX;

        hasNamedParameters = false;
        autoOpCast         = false;
    }

    void resetTmp()
    {
        cptResolved        = 0;
        hasNamedParameters = false;
    }

    VectorNative<AstNode*>                           genericParameters;
    VectorNative<AstNode*>                           parameters;
    VectorNative<TypeInfoParam*>                     solvedParameters;
    VectorNative<TypeInfoParam*>                     solvedCallParameters;
    VectorNative<bool>                               doneParameters;
    VectorNative<TypeInfo*>                          genericParametersCallTypes;
    VectorNative<AstNode*>                           genericParametersCallTypesFrom;
    VectorNative<TypeInfo*>                          genericParametersGenTypes;
    VectorMap<Utf8, TypeInfo*>                       genericReplaceTypes;
    VectorMap<Utf8, AstNode*>                        genericReplaceTypesFrom;
    VectorMap<Utf8, uint32_t>                        mapGenericTypesIndex;
    VectorMap<Utf8, pair<ComputedValue*, TypeInfo*>> genericReplaceValues;
    BadSignatureInfos                                badSignatureInfos;

    SemanticContext* semContext = nullptr;

    uint32_t    flags;
    MatchResult result;
    int         cptResolved;
    uint32_t    firstDefault;

    bool hasNamedParameters;
    bool autoOpCast;
};

struct TypeInfo
{
    // clang-format off
    TypeInfo() = default;
    TypeInfo(const char* name, TypeInfoKind kind) : name{ name }, kind { kind } {}
    TypeInfo(TypeInfoKind kind) : kind{kind} {}
    // clang-format on

    bool isPointerTo(NativeTypeKind pointerKind);
    bool isPointerTo(TypeInfoKind pointerKind);
    bool isPointerTo(TypeInfo* finalType);
    bool isPointerNull();
    bool isPointerVoid();
    bool isPointerConstVoid();
    bool isPointerToTypeInfo();
    bool isInitializerList();
    bool isArrayOfStruct();
    bool isArrayOfEnum();
    bool isMethod();

    TypeInfoStruct* getStructOrPointedStruct();
    TypeInfo*       getFakeAlias();
    TypeInfo*       getCA();

    // clang-format off
    bool isSlice()                          { return kind == TypeInfoKind::Slice; }
    bool isInterface()                      { return kind == TypeInfoKind::Interface; }
    bool isStruct()                         { return kind == TypeInfoKind::Struct; }
    bool isPointer()                        { return kind == TypeInfoKind::Pointer; }
    bool isNative()                         { return kind == TypeInfoKind::Native; }
    bool isArray()                          { return kind == TypeInfoKind::Array; }
    bool isEnum()                           { return kind == TypeInfoKind::Enum; }
    bool isAlias()                          { return kind == TypeInfoKind::Alias; }
    bool isFuncAttr()                       { return kind == TypeInfoKind::FuncAttr; }
    bool isListArray()                      { return kind == TypeInfoKind::TypeListArray; }
    bool isListTuple()                      { return kind == TypeInfoKind::TypeListTuple; }
    bool isVariadic()                       { return kind == TypeInfoKind::Variadic; }
    bool isTypedVariadic()                  { return kind == TypeInfoKind::TypedVariadic; }
    bool isCVariadic()                      { return kind == TypeInfoKind::CVariadic; }
    bool isCode()                           { return kind == TypeInfoKind::Code; }
    bool isLambdaClosure()                  { return kind == TypeInfoKind::LambdaClosure; }
    bool isKindGeneric()                    { return kind == TypeInfoKind::Generic; }
    bool isNative(NativeTypeKind native)    { return (kind == TypeInfoKind::Native) && (nativeType == native); }
    bool isAny()                            { return isNative(NativeTypeKind::Any); }
    bool isString()                         { return isNative(NativeTypeKind::String); }
    bool isBool()                           { return isNative(NativeTypeKind::Bool); }
    bool isRune()                           { return isNative(NativeTypeKind::Rune); }
    bool isVoid()                           { return isNative(NativeTypeKind::Void); }
    bool isCString()                        { return (kind == TypeInfoKind::Pointer) && (flags & TYPEINFO_C_STRING); }
    bool isLambda()                         { return (kind == TypeInfoKind::LambdaClosure) && !isClosure(); }
    bool isClosure()                        { return (flags & TYPEINFO_CLOSURE); }
    bool isNativeInteger()                  { return (flags & TYPEINFO_INTEGER); }
    bool isNativeIntegerUnsignedOrRune()    { return ((flags & TYPEINFO_INTEGER) && (flags & TYPEINFO_UNSIGNED)) || isRune(); }
    bool isNativeIntegerUnsigned()          { return (flags & TYPEINFO_INTEGER) && (flags & TYPEINFO_UNSIGNED); }
    bool isNativeIntegerSigned()            { return (flags & TYPEINFO_INTEGER) && !(flags & TYPEINFO_UNSIGNED); }
    bool isNativeIntegerOrRune()            { return (flags & TYPEINFO_INTEGER) || isRune(); }
    bool isNativeFloat()                    { return (flags & TYPEINFO_FLOAT); }
    bool isConst()                          { return (flags & TYPEINFO_CONST); }
    bool isStrict()                         { return (flags & TYPEINFO_STRICT); }
    bool isGeneric()                        { return (flags & TYPEINFO_GENERIC); }
    bool isFromGeneric()                    { return (flags & TYPEINFO_FROM_GENERIC); }
    bool isTuple()                          { return (flags & TYPEINFO_STRUCT_IS_TUPLE); }
    bool isPointerRef()                     { return (flags & TYPEINFO_POINTER_REF); }
    bool isPointerMoveRef()                 { return (flags & TYPEINFO_POINTER_REF) && (flags & TYPEINFO_POINTER_MOVE_REF); }
    bool isConstPointerRef()                { return (flags & TYPEINFO_POINTER_REF) && (flags & TYPEINFO_CONST); }
    bool isAutoConstPointerRef()            { return (flags & TYPEINFO_POINTER_REF) && (flags & TYPEINFO_CONST) && (flags & TYPEINFO_POINTER_AUTO_REF); }
    bool isPointerArithmetic()              { return (flags & TYPEINFO_POINTER_ARITHMETIC); }
    bool isSelf()                           { return (flags & TYPEINFO_SELF); }
    bool isUntypedInteger()                 { return (flags & TYPEINFO_UNTYPED_INTEGER); }
    bool isUntypedFloat()                   { return (flags & TYPEINFO_UNTYPED_FLOAT); }
    bool isUntypedBinHex()                  { return (flags & TYPEINFO_UNTYPED_BINHEXA); }
    bool isFakeAlias()                      { return (flags & TYPEINFO_FAKE_ALIAS); }
    // clang-format on

    virtual bool        isSame(TypeInfo* from, uint64_t castFlags);
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

    void        removeGenericFlag();
    void        clearName();
    void        forceComputeName();
    void        getScopedName(Utf8& name);
    Utf8        getName();
    Utf8        getTypeName(bool forceNoScope);
    const Utf8& computeWhateverName(uint32_t nameType);
    const Utf8& computeWhateverNameNoLock(uint32_t nameType);

    SharedMutex mutex;

    Utf8 name;
    Utf8 displayName;
    Utf8 scopedName;
    Utf8 scopedNameExport;

    AstNode* declNode = nullptr;
    uint64_t flags    = 0;

    uint32_t       sizeOf     = 0;
    TypeInfoKind   kind       = TypeInfoKind::Invalid;
    NativeTypeKind nativeType = NativeTypeKind::Void;
    uint8_t        padding[2];

    SWAG_RACE_CONDITION_INSTANCE(raceName);
};

struct TypeInfoParam
{
    int            numRegisters();
    bool           isSame(TypeInfoParam* to, uint64_t castFlags);
    TypeInfoParam* clone();
    void           allocateComputedValue();

    Utf8          name;
    AttributeList attributes;

    ComputedValue* value    = nullptr;
    TypeInfo*      typeInfo = nullptr;
    AstNode*       declNode = nullptr;

    uint32_t flags : 8  = 0;
    uint32_t index : 24 = 0;
    uint32_t offset     = 0;
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

    bool      isSame(TypeInfo* from, uint64_t castFlags) override;
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

    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
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

    int numRegisters() override
    {
        return 1;
    }

    TypeInfo* clone() override;
    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* from, uint64_t castFlags) override;

    bool            isSame(TypeInfoFuncAttr* other, uint64_t castFlags, BadSignatureInfos& bi);
    bool            isSame(TypeInfoFuncAttr* from, uint64_t castFlags);
    void            match(SymbolMatchContext& context);
    TypeInfo*       concreteReturnType();
    bool            isVariadic();
    bool            isCVariadic();
    uint32_t        registerIdxToParamIdx(int argIdx);
    TypeInfo*       registerIdxToType(int argIdx);
    int             numParamsRegisters();
    int             numReturnRegisters();
    int             numTotalRegisters();
    const CallConv& getCallConv();

    VectorNative<TypeInfoParam*> capture;
    VectorNative<TypeInfoParam*> genericParameters;
    VectorNative<TypeInfoParam*> parameters;
    AttributeList                attributes;
    VectorMap<Utf8, TypeInfo*>   replaceTypes;
    VectorMap<Utf8, AstNode*>    replaceTypesFrom;

    TypeInfo* returnType = nullptr;

    uint32_t     firstDefaultValueIdx = UINT32_MAX;
    int          stackSize            = 0;
    uint32_t     attributeUsage       = AttributeUsage::All;
    CallConvKind callConv             = CallConvKind::Swag;
};

struct TypeInfoPointer : public TypeInfo
{
    TypeInfoPointer()
        : TypeInfo{TypeInfoKind::Pointer}
    {
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

struct TypeInfoArray : public TypeInfo
{
    TypeInfoArray()
        : TypeInfo{TypeInfoKind::Array}
    {
    }

    int numRegisters() override
    {
        return 1;
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
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
    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

struct TypeInfoList : public TypeInfo
{
    TypeInfoList()
    {
    }

    int numRegisters() override
    {
        return 1;
    }

    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
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

    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
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

    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
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

    bool           isSame(TypeInfo* to, uint64_t castFlags) override;
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

    VectorNative<TypeInfoParam*>          genericParameters;
    VectorNative<TypeInfo*>               deducedGenericParameters;
    VectorNative<TypeInfoParam*>          fields;
    VectorNative<TypeInfoParam*>          consts;
    VectorNative<TypeInfoParam*>          methods;
    VectorNative<TypeInfoParam*>          interfaces;
    VectorMap<Utf8, TypeInfo*>            replaceTypes;
    VectorMap<Utf8, AstNode*>             replaceTypesFrom;
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
};

struct TypeInfoAlias : public TypeInfo
{
    TypeInfoAlias()
        : TypeInfo{TypeInfoKind::Alias}
    {
    }

    void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
    TypeInfo* clone() override;

    int numRegisters() override
    {
        return rawType->numRegisters();
    }

    TypeInfo* rawType = nullptr;
};

struct TypeInfoCode : public TypeInfo
{
    TypeInfoCode()
        : TypeInfo{"code", TypeInfoKind::Code}
    {
    }

    bool      isSame(TypeInfo* to, uint64_t castFlags) override;
    TypeInfo* clone() override;

    AstNode* content = nullptr;
};

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
    SWAG_ASSERT(ptr);
    T* casted = static_cast<T*>(ptr->getFakeAlias());
    SWAG_ASSERT(casted->kind == kind);
    return casted;
}

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
    SWAG_ASSERT(ptr);
    T* casted = static_cast<T*>(ptr->getFakeAlias());
    SWAG_ASSERT(casted->kind == kind1 || casted->kind == kind2);
    return casted;
}

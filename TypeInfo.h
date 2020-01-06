#pragma once
#include "Pool.h"
#include "Utf8Crc.h"
#include "Log.h"
#include "Register.h"
#include "Attribute.h"
#include "VectorNative.h"
struct Scope;
struct TypeInfo;
struct SymbolMatchContext;
struct Job;
enum class Intrisic;
struct AstNode;
struct ByteCode;
struct TypeInfoFuncAttr;
struct AstFuncDecl;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class TypeInfoKind
{
    Invalid,
    Native,
    Namespace,
    Enum,
    FuncAttr,
    Param,
    Lambda,
    Pointer,
    Array,
    Slice,
    TypeList,
    Variadic,
    TypedVariadic,
    Struct,
    Generic,
    Alias,
    Code,
    Interface,
    Count,
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class NativeTypeKind
{
    Void,
    S8,
    S16,
    S32,
    S64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Bool,
    Char,
    String,
    Any,
    Count,
};

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && casted->kind == kind);
    return casted;
}

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2));
    return casted;
}

//static const uint32_t TYPEINFO_ATTRIBUTE_FUNC           = 0x00000001;
static const uint32_t TYPEINFO_UNTYPED_BINHEXA          = 0x00000002;
static const uint32_t TYPEINFO_INTEGER                  = 0x00000004;
static const uint32_t TYPEINFO_FLOAT                    = 0x00000008;
static const uint32_t TYPEINFO_UNSIGNED                 = 0x00000010;
static const uint32_t TYPEINFO_CONST                    = 0x00000020;
static const uint32_t TYPEINFO_IN_EXACT_LIST            = 0x00000040;
static const uint32_t TYPEINFO_VARIADIC                 = 0x00000080;
static const uint32_t TYPEINFO_STRUCT_HAS_INIT_VALUES   = 0x00000100;
static const uint32_t TYPEINFO_STRUCT_ALL_UNINITIALIZED = 0x00000200;
static const uint32_t TYPEINFO_STRUCT_NO_POST_COPY      = 0x00000400;
static const uint32_t TYPEINFO_STRUCT_NO_POST_MOVE      = 0x00000800;
static const uint32_t TYPEINFO_STRUCT_NO_DROP           = 0x00001000;
static const uint32_t TYPEINFO_GENERIC                  = 0x00002000;
static const uint32_t TYPEINFO_RETURN_BY_COPY           = 0x00004000;
static const uint32_t TYPEINFO_UNTYPED_INTEGER          = 0x00008000;
static const uint32_t TYPEINFO_UNTYPED_FLOAT            = 0x00010000;
static const uint32_t TYPEINFO_DEFINED_VALUE            = 0x00020000;
static const uint32_t TYPEINFO_AUTO_CAST                = 0x00040000;
static const uint32_t TYPEINFO_TYPED_VARIADIC           = 0x00080000;
static const uint32_t TYPEINFO_IN_CONCRETE_LIST         = 0x00100000;
static const uint32_t TYPEINFO_STRUCT_IS_TUPLE          = 0x00200000;
static const uint32_t TYPEINFO_ENUM_FLAGS               = 0x00400000;

static const uint32_t ISSAME_EXACT     = 0x00000001;
static const uint32_t ISSAME_CAST      = 0x00000002;
static const uint32_t ISSAME_INTERFACE = 0x00000004;

struct TypeInfo
{
    virtual bool isSame(TypeInfo* from, uint32_t isSameFlags)
    {
        if (this == from)
            return true;
        if (kind != from->kind)
            return false;

        if (isSameFlags & ISSAME_EXACT)
        {
            if ((flags & TYPEINFO_CONST) != (from->flags & TYPEINFO_CONST))
                return false;
            if ((flags & TYPEINFO_GENERIC) != (from->flags & TYPEINFO_GENERIC))
                return false;
            if ((flags & TYPEINFO_AUTO_CAST) != (from->flags & TYPEINFO_AUTO_CAST))
                return false;
        }

        return true;
    }

    bool isPointerTo(TypeInfoKind pointerKind);
    bool isInitializerList();

    bool isNative(NativeTypeKind native)
    {
        return (kind == TypeInfoKind::Native) && (nativeType == native);
    }

    bool isNativeInteger()
    {
        return flags & TYPEINFO_INTEGER;
    }

    bool isConst()
    {
        return flags & TYPEINFO_CONST;
    }

    void setConst()
    {
        flags |= TYPEINFO_CONST;
        name     = "const " + name;
        fullname = "const " + fullname;
    }

    virtual int numRegisters()
    {
        if (sizeOf == 0)
            return 0;
        return max(sizeOf, sizeof(void*)) / sizeof(void*);
    }

    virtual void computeName()
    {
    }

    void copyFrom(TypeInfo* from)
    {
        flags      = from->flags & ~TYPEINFO_IN_EXACT_LIST;
        kind       = from->kind;
        nativeType = from->nativeType;
        name       = from->name;
        fullname   = from->fullname;
        sizeOf     = from->sizeOf;
    }

    const Utf8& getFullName()
    {
        if (!fullname.empty())
            return fullname;
        return name;
    }

    virtual TypeInfo*  clone() = 0;
    static const char* getArticleKindName(TypeInfo* typeInfo);
    static const char* getNakedKindName(TypeInfo* typeInfo);

    Utf8         name;
    Utf8         fullname;
    shared_mutex mutex;
    TypeInfo*    constCopy = nullptr;

    TypeInfoKind   kind       = TypeInfoKind::Invalid;
    NativeTypeKind nativeType = NativeTypeKind::Void;
    uint32_t       flags      = 0;
    uint32_t       sizeOf     = 0;
};

struct TypeInfoNative : public TypeInfo
{
    TypeInfoNative()
    {
        kind         = TypeInfoKind::Native;
        valueInteger = 0;
    }

    TypeInfoNative(NativeTypeKind type, const char* tname, int sof, uint32_t fl)
    {
        kind         = TypeInfoKind::Native;
        nativeType   = type;
        name         = tname;
        sizeOf       = sof;
        flags        = fl;
        valueInteger = 0;
    }

    bool      isSame(TypeInfo* from, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    union {
        int32_t valueInteger;
        float   valueFloat;
    };
};

struct TypeInfoNamespace : public TypeInfo
{
    TypeInfoNamespace()
    {
        kind = TypeInfoKind::Namespace;
    }

    TypeInfo* clone() override;

    Scope* scope = nullptr;
};

struct TypeInfoParam : public TypeInfo
{
    TypeInfoParam()
    {
        kind = TypeInfoKind::Param;
    }

    int numRegisters() override
    {
        return typeInfo->numRegisters();
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    Utf8             namedParam;
    ComputedValue    value;
    SymbolAttributes attributes;

    TypeInfo* typeInfo = nullptr;
    AstNode*  node     = nullptr;

    int index  = 0;
    int offset = 0;
};

struct TypeInfoEnum : public TypeInfo
{
    TypeInfoEnum()
    {
        kind = TypeInfoKind::Enum;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    VectorNative<TypeInfoParam*> values;
    SymbolAttributes             attributes;

    Scope*    scope   = nullptr;
    TypeInfo* rawType = nullptr;
};

enum class MatchResult
{
    Ok,
    TooManyParameters,
    TooManyGenericParameters,
    NotEnoughParameters,
    MissingParameters,
    NotEnoughGenericParameters,
    BadSignature,
    BadGenericSignature,
    InvalidNamedParameter,
    MissingNamedParameter,
    DuplicatedNamedParameter,
};

struct SymbolMatchContext
{
    static const uint32_t MATCH_ACCEPT_NO_GENERIC = 0x00000001;
    static const uint32_t MATCH_FOR_LAMBDA        = 0x00000002;
    static const uint32_t MATCH_WAS_PARTIAL       = 0x00000004;
    static const uint32_t MATCH_ERROR_VALUE_TYPE  = 0x00000008;
    static const uint32_t MATCH_ERROR_TYPE_VALUE  = 0x00000010;
    static const uint32_t MATCH_UNCONST           = 0x00000020;

    SymbolMatchContext()
    {
        reset();
    }

    void reset()
    {
        badSignatureParameterIdx  = 0;
        badSignatureRequestedType = nullptr;
        badSignatureGivenType     = nullptr;
        result                    = MatchResult::Ok;
        genericParameters.clear();
        parameters.clear();
        doneParameters.clear();
        genericParametersCallTypes.clear();
        genericParametersGenTypes.clear();
        solvedParameters.clear();
        flags = 0;
        resetTmp();
    }

    void resetTmp()
    {
        cptResolved        = 0;
        hasNamedParameters = false;
    }

    VectorNative<AstNode*>       genericParameters;
    VectorNative<AstNode*>       parameters;
    VectorNative<TypeInfoParam*> solvedParameters;
    VectorNative<bool>           doneParameters;
    VectorNative<TypeInfo*>      genericParametersCallTypes;
    VectorNative<TypeInfo*>      genericParametersGenTypes;
    map<Utf8Crc, TypeInfo*>      genericReplaceTypes;
    map<Utf8Crc, uint32_t>       mapGenericTypesIndex;
    MatchResult                  result;

    TypeInfo* badSignatureRequestedType;
    TypeInfo* badSignatureGivenType;

    uint32_t flags;
    int      cptResolved;
    int      badSignatureParameterIdx;

    bool hasNamedParameters;
};

static const uint32_t TYPEINFO_ATTRIBUTE_FUNC      = 0x00000001;
static const uint32_t TYPEINFO_ATTRIBUTE_VAR       = 0x00000002;
static const uint32_t TYPEINFO_ATTRIBUTE_STRUCT    = 0x00000004;
static const uint32_t TYPEINFO_ATTRIBUTE_ENUM      = 0x00000008;
static const uint32_t TYPEINFO_ATTRIBUTE_ENUMVALUE = 0x00000010;
static const uint32_t TYPEINFO_ATTRIBUTE_STRUCTVAR = 0x00000020;
static const uint32_t TYPEINFO_ATTRIBUTE_INTERFACE = 0x00000040;
static const uint32_t TYPEINFO_ATTRIBUTE_GLOBALVAR = 0x00000080;

struct TypeInfoFuncAttr : public TypeInfo
{
    TypeInfoFuncAttr()
    {
        kind = TypeInfoKind::FuncAttr;
    }

    int numParamsRegisters()
    {
        int total = 0;
        for (auto param : parameters)
            total += param->numRegisters();
        return total;
    }

    int numReturnRegisters()
    {
        return returnType ? returnType->numRegisters() : 0;
    }

    bool      isSame(TypeInfo* from, uint32_t isSameFlags) override;
    TypeInfo* clone() override;
    void      computeName() override;
    void      match(SymbolMatchContext& context);
    bool      isSame(TypeInfoFuncAttr* from, uint32_t isSameFlags);

    VectorNative<TypeInfoParam*> genericParameters;
    VectorNative<TypeInfoParam*> parameters;
    SymbolAttributes             attributes;

    TypeInfo* returnType = nullptr;

    int      firstDefaultValueIdx = -1;
    int      stackSize            = 0;
    uint32_t attributeFlags       = 0;
};

struct TypeInfoPointer : public TypeInfo
{
    TypeInfoPointer()
    {
        kind = TypeInfoKind::Pointer;
    }

    void      computeName() override;
    TypeInfo* computePointedType();
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* finalType   = nullptr;
    TypeInfo* pointedType = nullptr;

    uint32_t ptrCount = 0;
};

struct TypeInfoArray : public TypeInfo
{
    TypeInfoArray()
    {
        kind = TypeInfoKind::Array;
    }

    int numRegisters() override
    {
        return 1;
    }

    void      computeName() override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
    TypeInfo* finalType   = nullptr;

    uint32_t count      = 0;
    uint32_t totalCount = 0;
};

struct TypeInfoSlice : public TypeInfo
{
    TypeInfoSlice()
    {
        kind = TypeInfoKind::Slice;
    }

    void      computeName() override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
};

enum class TypeInfoListKind
{
    Invalid,
    Bracket,
    Curly,
};

struct TypeInfoList : public TypeInfo
{
    TypeInfoList()
    {
        kind = TypeInfoKind::TypeList;
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    int numRegisters() override
    {
        return 1;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    VectorNative<TypeInfo*> childs;
    vector<Utf8>            names;

    Scope* scope = nullptr;

    TypeInfoListKind listKind = TypeInfoListKind::Bracket;
};

struct TypeInfoVariadic : public TypeInfo
{
    TypeInfoVariadic()
    {
        kind = TypeInfoKind::Variadic;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoGeneric : public TypeInfo
{
    TypeInfoGeneric()
    {
        kind = TypeInfoKind::Generic;
        flags |= TYPEINFO_GENERIC;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoStruct : public TypeInfo
{
    TypeInfoStruct()
    {
        kind = TypeInfoKind::Struct;
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    int numRegisters() override
    {
        return 1;
    }

    bool           isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo*      clone() override;
    void           match(SymbolMatchContext& context);
    TypeInfoParam* findChildByNameNoLock(const Utf8& childName);
    TypeInfoParam* hasInterface(TypeInfoStruct* itf);
    TypeInfoParam* hasInterfaceNoLock(TypeInfoStruct* itf);

    VectorNative<TypeInfoParam*> genericParameters;
    VectorNative<TypeInfoParam*> fields;
    VectorNative<TypeInfoParam*> methods;
    VectorNative<TypeInfoParam*> interfaces;
    SymbolAttributes             attributes;

    TypeInfoStruct* itable            = nullptr;
    Scope*          scope             = nullptr;
    AstNode*        structNode        = nullptr;
    ByteCode*       opInit            = nullptr;
    AstFuncDecl*    opUserPostCopyFct = nullptr;
    ByteCode*       opPostCopy        = nullptr;
    AstFuncDecl*    opUserPostMoveFct = nullptr;
    ByteCode*       opPostMove        = nullptr;
    AstFuncDecl*    opUserDropFct     = nullptr;
    ByteCode*       opDrop            = nullptr;

    uint32_t maxPaddingSize         = 0;
    uint32_t cptRemainingInterfaces = 0;
    uint32_t cptRemainingMethods    = 0;
};

struct TypeInfoAlias : public TypeInfo
{
    TypeInfoAlias()
    {
        kind = TypeInfoKind::Alias;
    }

    void computeName() override
    {
        rawType->computeName();
        name     = rawType->name;
        fullname = rawType->getFullName();
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoCode : public TypeInfo
{
    TypeInfoCode()
    {
        name = "code";
        kind = TypeInfoKind::Code;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    AstNode* content = nullptr;
};

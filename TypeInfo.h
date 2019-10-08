#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SpinLock.h"
#include "Log.h"
#include "Register.h"
#include "Attribute.h"
struct Scope;
struct TypeInfo;
struct SymbolMatchContext;
struct Job;
enum class Intrisic;
struct AstNode;
struct ByteCode;
struct TypeInfoFuncAttr;

// Do not forget to synchronize runtime.h !
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
    Count,
};

// Do not forget to synchronize runtime.h !
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

static const uint32_t TYPEINFO_ATTRIBUTE_FUNC           = 0x00000001;
static const uint32_t TYPEINFO_ATTRIBUTE_VAR            = 0x00000002;
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
static const uint32_t TYPEINFO_ATTRIBUTE_STRUCT         = 0x00200000;
static const uint32_t TYPEINFO_ATTRIBUTE_ENUM           = 0x00400000;
static const uint32_t TYPEINFO_STRUCT_IS_TUPLE          = 0x00800000;
static const uint32_t TYPEINFO_ATTRIBUTE_ENUMVALUE      = 0x01000000;

static const uint32_t ISSAME_EXACT    = 0x00000001;
static const uint32_t ISSAME_CAST     = 0x00000002;
static const uint32_t ISSAME_CONCRETE = 0x00000004;

struct TypeInfo : public PoolElement
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
        }

        return true;
    }

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

    void reset() override
    {
        flags      = 0;
        nativeType = NativeTypeKind::Void;
        name.clear();
        sizeOf = 0;
    }

    void setConst()
    {
        flags |= TYPEINFO_CONST;
        name = "const " + name;
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
        sizeOf     = from->sizeOf;
    }

    virtual TypeInfo*  clone() = 0;
    static const char* getArticleKindName(TypeInfo* typeInfo);
    static const char* getNakedKindName(TypeInfo* typeInfo);

    uint32_t       flags;
    TypeInfoKind   kind;
    NativeTypeKind nativeType;
    Utf8           name;
    int            sizeOf;
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
    void reset() override
    {
        TypeInfo::reset();
        kind  = TypeInfoKind::Namespace;
        scope = nullptr;
    }

    TypeInfo* clone() override;

    Scope* scope;
};

struct TypeInfoParam : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind = TypeInfoKind::Param;
        namedParam.clear();
        typeInfo = nullptr;
        index    = 0;
        offset   = 0;
        attributes.reset();
    }

    int numRegisters() override
    {
        return typeInfo->numRegisters();
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    Utf8             namedParam;
    TypeInfo*        typeInfo;
    ComputedValue    value;
    int              index;
    int              offset;
    SymbolAttributes attributes;
};

struct TypeInfoEnum : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind    = TypeInfoKind::Enum;
        scope   = nullptr;
        rawType = nullptr;
        values.clear();
        attributes.reset();
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    vector<TypeInfoParam*> values;
    Scope*                 scope;
    TypeInfo*              rawType;
    SymbolAttributes       attributes;
};

enum MatchResult
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
        genericParametersCallValues.clear();
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
        maxGenericParam    = 0;
    }

    int      cptResolved;
    uint32_t maxGenericParam;
    bool     hasNamedParameters;

    uint32_t               flags;
    int                    badSignatureParameterIdx;
    TypeInfo*              badSignatureRequestedType;
    TypeInfo*              badSignatureGivenType;
    MatchResult            result;
    vector<AstNode*>       genericParameters;
    vector<AstNode*>       parameters;
    vector<TypeInfoParam*> solvedParameters;
    vector<bool>           doneParameters;
    vector<ComputedValue>  genericParametersCallValues;
    vector<TypeInfo*>      genericParametersCallTypes;
    vector<TypeInfo*>      genericParametersGenTypes;

    struct MapGenType
    {
        TypeInfo*        toType;
        vector<uint32_t> parameterIndex;
    };

    map<TypeInfo*, MapGenType> mapGenericTypes;
};

struct TypeInfoFuncAttr : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind                 = TypeInfoKind::FuncAttr;
        firstDefaultValueIdx = -1;
        genericParameters.clear();
        parameters.clear();
        returnType = nullptr;
        stackSize  = 0;
        attributes.reset();
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

    int                    firstDefaultValueIdx;
    vector<TypeInfoParam*> genericParameters;
    vector<TypeInfoParam*> parameters;
    TypeInfo*              returnType;
    int                    stackSize;
    SymbolAttributes       attributes;
};

struct TypeInfoPointer : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind        = TypeInfoKind::Pointer;
        finalType   = nullptr;
        pointedType = nullptr;
        ptrCount    = 0;
    }

    void computeName() override
    {
        name.clear();
        if (flags & TYPEINFO_CONST)
            name = "const ";
        for (uint32_t i = 0; i < ptrCount; i++)
            name += "*";
        finalType->computeName();
        name += finalType->name;
    }

    TypeInfo* computePointedType();
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* finalType;
    TypeInfo* pointedType;
    uint32_t  ptrCount;
};

struct TypeInfoArray : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind        = TypeInfoKind::Array;
        pointedType = nullptr;
        finalType   = nullptr;
        count       = 0;
        totalCount  = 0;
    }

    void computeName() override
    {
        pointedType->computeName();
        name.clear();
        if (flags & TYPEINFO_CONST)
            name = "const ";
        if (count == UINT32_MAX)
            name += format("[] %s", pointedType->name.c_str());
        else
            name += format("[%d] %s", count, pointedType->name.c_str());
    }

    int numRegisters() override
    {
        return 1;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType;
    TypeInfo* finalType;
    uint32_t  count;
    uint32_t  totalCount;
};

struct TypeInfoSlice : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind        = TypeInfoKind::Slice;
        pointedType = nullptr;
    }

    void computeName() override
    {
        pointedType->computeName();
        name = format("[..] %s", pointedType->name.c_str());
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType;
};

enum class TypeInfoListKind
{
    Bracket,
    Curly,
};

struct TypeInfoList : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        kind = TypeInfoKind::TypeList;
        childs.clear();
        names.clear();
        scope    = nullptr;
        listKind = TypeInfoListKind::Bracket;
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    int numRegisters() override
    {
        return 1;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfoListKind  listKind;
    vector<TypeInfo*> childs;
    vector<Utf8>      names;
    Scope*            scope;
};

struct TypeInfoVariadic : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        rawType = nullptr;
        kind    = TypeInfoKind::Variadic;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType;
};

struct TypeInfoGeneric : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        rawType = nullptr;
        kind    = TypeInfoKind::Generic;
        flags |= TYPEINFO_GENERIC;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType;
};

struct TypeInfoStruct : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        kind = TypeInfoKind::Struct;
        genericParameters.clear();
        childs.clear();
        scope             = nullptr;
        structNode        = nullptr;
        opInitFct         = nullptr;
        opUserPostCopyFct = nullptr;
        opPostCopy        = nullptr;
        opUserPostMoveFct = nullptr;
        opPostMove        = nullptr;
        opUserDropFct     = nullptr;
        opDrop            = nullptr;
        attributes.reset();
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    int numRegisters() override
    {
        return 1;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;
    void      match(SymbolMatchContext& context);

    vector<TypeInfoParam*> genericParameters;
    vector<TypeInfoParam*> childs;
    Scope*                 scope;
    AstNode*               structNode;
    AstNode*               opInitFct;
    AstNode*               opUserPostCopyFct;
    ByteCode*              opPostCopy;
    AstNode*               opUserPostMoveFct;
    ByteCode*              opPostMove;
    AstNode*               opUserDropFct;
    ByteCode*              opDrop;
    SpinLock               mutex;
    SymbolAttributes       attributes;
};

struct TypeInfoAlias : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind    = TypeInfoKind::Alias;
        rawType = nullptr;
    }

    void computeName() override
    {
        rawType->computeName();
        name = rawType->name;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType;
};

extern Pool<TypeInfoFuncAttr>  g_Pool_typeInfoFuncAttr;
extern Pool<TypeInfoNamespace> g_Pool_typeInfoNamespace;
extern Pool<TypeInfoEnum>      g_Pool_typeInfoEnum;
extern Pool<TypeInfoParam>     g_Pool_typeInfoParam;
extern Pool<TypeInfoPointer>   g_Pool_typeInfoPointer;
extern Pool<TypeInfoArray>     g_Pool_typeInfoArray;
extern Pool<TypeInfoSlice>     g_Pool_typeInfoSlice;
extern Pool<TypeInfoList>      g_Pool_typeInfoList;
extern Pool<TypeInfoNative>    g_Pool_typeInfoNative;
extern Pool<TypeInfoVariadic>  g_Pool_typeInfoVariadic;
extern Pool<TypeInfoGeneric>   g_Pool_typeInfoGeneric;
extern Pool<TypeInfoStruct>    g_Pool_typeInfoStruct;
extern Pool<TypeInfoAlias>     g_Pool_typeInfoAlias;
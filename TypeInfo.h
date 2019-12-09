#pragma once
#include "Pool.h"
#include "Utf8.h"
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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
//static const uint32_t TYPEINFO_INTERFACE                = 0x00000002;
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
        fullname   = from->fullname;
        sizeOf     = from->sizeOf;
    }

    const char* getFullName()
    {
        if (!fullname.empty())
            return fullname.c_str();
        return name.c_str();
    }

    virtual TypeInfo*  clone() = 0;
    static const char* getArticleKindName(TypeInfo* typeInfo);
    static const char* getNakedKindName(TypeInfo* typeInfo);

    Utf8           name;
    Utf8           fullname;
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
    TypeInfo*        typeInfo = nullptr;
    AstNode*         node     = nullptr;
    int              index    = 0;
    int              offset   = 0;
};

struct TypeInfoEnum : public TypeInfo
{
    TypeInfoEnum()
    {
        kind = TypeInfoKind::Enum;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    vector<TypeInfoParam*> values;
    SymbolAttributes       attributes;
    Scope*                 scope   = nullptr;
    TypeInfo*              rawType = nullptr;
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

static const uint32_t TYPEINFO_ATTRIBUTE_FUNC      = 0x00000001;
static const uint32_t TYPEINFO_ATTRIBUTE_VAR       = 0x00000002;
static const uint32_t TYPEINFO_ATTRIBUTE_STRUCT    = 0x00000004;
static const uint32_t TYPEINFO_ATTRIBUTE_ENUM      = 0x00000008;
static const uint32_t TYPEINFO_ATTRIBUTE_ENUMVALUE = 0x00000010;
static const uint32_t TYPEINFO_ATTRIBUTE_STRUCTVAR = 0x00000020;
static const uint32_t TYPEINFO_ATTRIBUTE_INTERFACE = 0x00000040;

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

    vector<TypeInfoParam*> genericParameters;
    vector<TypeInfoParam*> parameters;
    SymbolAttributes       attributes;
    TypeInfo*              returnType           = nullptr;
    int                    firstDefaultValueIdx = -1;
    int                    stackSize            = 0;
    uint32_t               attributeFlags       = 0;
};

struct TypeInfoPointer : public TypeInfo
{
    TypeInfoPointer()
    {
        kind = TypeInfoKind::Pointer;
    }

    void computeName() override
    {
        name.clear();
        if (flags & TYPEINFO_CONST)
            name = "const ";
        for (uint32_t i = 0; i < ptrCount; i++)
        {
            name += "*";
            fullname += "*";
        }

        finalType->computeName();
        name += finalType->name;
        fullname += finalType->getFullName();
    }

    TypeInfo* computePointedType();
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* finalType   = nullptr;
    TypeInfo* pointedType = nullptr;
    uint32_t  ptrCount    = 0;
};

struct TypeInfoArray : public TypeInfo
{
    TypeInfoArray()
    {
        kind = TypeInfoKind::Array;
    }

    void computeName() override
    {
        pointedType->computeName();
        name.clear();
        fullname.clear();
        if (flags & TYPEINFO_CONST)
            name = "const ";
        if (count == UINT32_MAX)
        {
            name += format("[] %s", pointedType->name.c_str());
            fullname += format("[] %s", pointedType->getFullName());
        }
        else
        {
            name += format("[%d] %s", count, pointedType->name.c_str());
            fullname += format("[%d] %s", count, pointedType->getFullName());
        }
    }

    int numRegisters() override
    {
        return 1;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType = nullptr;
    TypeInfo* finalType   = nullptr;
    uint32_t  count       = 0;
    uint32_t  totalCount  = 0;
};

struct TypeInfoSlice : public TypeInfo
{
    TypeInfoSlice()
    {
        kind = TypeInfoKind::Slice;
    }

    void computeName() override
    {
        pointedType->computeName();
        name.clear();
        if (flags & TYPEINFO_CONST)
            name = "const ";
        name += format("[..] %s", pointedType->name.c_str());
        fullname += format("[..] %s", pointedType->getFullName());
    }

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

    vector<TypeInfo*> childs;
    vector<Utf8>      names;
    Scope*            scope    = nullptr;
    TypeInfoListKind  listKind = TypeInfoListKind::Bracket;
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

    vector<TypeInfoParam*> genericParameters;
    vector<TypeInfoParam*> childs;
    vector<TypeInfoParam*> interfaces;
    shared_mutex           mutex;
    SymbolAttributes       attributes;
    TypeInfoStruct*        itable                 = nullptr;
    Scope*                 scope                  = nullptr;
    AstNode*               structNode             = nullptr;
    ByteCode*              opInit                 = nullptr;
    AstNode*               opUserPostCopyFct      = nullptr;
    ByteCode*              opPostCopy             = nullptr;
    AstNode*               opUserPostMoveFct      = nullptr;
    ByteCode*              opPostMove             = nullptr;
    AstNode*               opUserDropFct          = nullptr;
    ByteCode*              opDrop                 = nullptr;
    uint32_t               cptRemainingInterfaces = 0;
    uint32_t               maxPaddingSize         = 0;
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
        name = rawType->name;
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

extern thread_local Pool<TypeInfoFuncAttr>  g_Pool_typeInfoFuncAttr;
extern thread_local Pool<TypeInfoNamespace> g_Pool_typeInfoNamespace;
extern thread_local Pool<TypeInfoEnum>      g_Pool_typeInfoEnum;
extern thread_local Pool<TypeInfoParam>     g_Pool_typeInfoParam;
extern thread_local Pool<TypeInfoPointer>   g_Pool_typeInfoPointer;
extern thread_local Pool<TypeInfoArray>     g_Pool_typeInfoArray;
extern thread_local Pool<TypeInfoSlice>     g_Pool_typeInfoSlice;
extern thread_local Pool<TypeInfoList>      g_Pool_typeInfoList;
extern thread_local Pool<TypeInfoNative>    g_Pool_typeInfoNative;
extern thread_local Pool<TypeInfoVariadic>  g_Pool_typeInfoVariadic;
extern thread_local Pool<TypeInfoGeneric>   g_Pool_typeInfoGeneric;
extern thread_local Pool<TypeInfoStruct>    g_Pool_typeInfoStruct;
extern thread_local Pool<TypeInfoAlias>     g_Pool_typeInfoAlias;
extern thread_local Pool<TypeInfoCode>      g_Pool_typeInfoCode;
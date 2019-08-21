#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SpinLock.h"
#include "Log.h"
#include "Register.h"
struct Scope;
struct TypeInfo;
struct SymbolMatchContext;
struct Job;
enum class Intrisic;
struct AstNode;

enum class TypeInfoKind
{
    Native,
    Namespace,
    Enum,
    EnumValue,
    FuncAttr,
    FuncAttrParam,
    Lambda,
    Pointer,
    Array,
    Slice,
    TypeList,
    Variadic,
    VariadicValue,
    Struct,
    Generic,
};

enum class NativeType
{
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
    Void,
    Count,
};

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted->kind == kind);
    return casted;
}

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted->kind == kind1 || casted->kind == kind2);
    return casted;
}

static const uint64_t TYPEINFO_ATTRIBUTE_FUNC         = 0x00000000'00000001;
static const uint64_t TYPEINFO_ATTRIBUTE_VAR          = 0x00000000'00000002;
static const uint64_t TYPEINFO_INTEGER                = 0x00000000'00000004;
static const uint64_t TYPEINFO_FLOAT                  = 0x00000000'00000008;
static const uint64_t TYPEINFO_UNSIGNED               = 0x00000000'00000010;
static const uint64_t TYPEINFO_CONST                  = 0x00000000'00000020;
static const uint64_t TYPEINFO_IN_MANAGER             = 0x00000000'00000040;
static const uint64_t TYPEINFO_VARIADIC               = 0x00000000'00000080;
static const uint64_t TYPEINFO_STRUCT_HAS_CONSTRUCTOR = 0x00000000'00000100;
static const uint64_t TYPEINFO_GENERIC                = 0x00000000'00000200;
static const uint64_t TYPEINFO_RETURN_BY_COPY         = 0x00000000'00000400;

struct TypeInfo : public PoolElement
{
    virtual bool isSame(TypeInfo* from)
    {
        if (kind != from->kind)
            return false;
        if (isConst() != from->isConst())
            return false;
        return true;
    }

    virtual bool isSameExact(TypeInfo* from)
    {
        return isSame(from);
    }

    bool isNative(NativeType native)
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
        nativeType = NativeType::Void;
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

    virtual TypeInfo* clone() = 0;

    void copyFrom(TypeInfo* from)
    {
        flags      = from->flags;
        kind       = from->kind;
        nativeType = from->nativeType;
        name       = from->name;
        sizeOf     = from->sizeOf;
    }

    static const char* getKindName(TypeInfo* typeInfo);
    static const char* getNakedKindName(TypeInfo* typeInfo);

    uint64_t     flags;
    TypeInfoKind kind;
    NativeType   nativeType;
    Utf8         name;
    int          sizeOf;
};

struct TypeInfoNative : public TypeInfo
{
    TypeInfoNative()
    {
        kind = TypeInfoKind::Native;
    }

    TypeInfoNative(NativeType type, const char* tname, int sof, uint64_t fl)
    {
        kind       = TypeInfoKind::Native;
        nativeType = type;
        name       = tname;
        sizeOf     = sof;
        flags      = fl;
    }

    bool isSame(TypeInfo* from) override
    {
        return this == from;
    }

    TypeInfo* clone() override;
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

struct TypeInfoEnum : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind    = TypeInfoKind::Enum;
        scope   = nullptr;
        rawType = nullptr;
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto castedFrom = static_cast<TypeInfoEnum*>(from);
        return rawType->isSame(castedFrom->rawType);
    }

    TypeInfo* clone() override;

    Scope*    scope;
    TypeInfo* rawType;
};

struct TypeInfoEnumValue : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind      = TypeInfoKind::EnumValue;
        scope     = nullptr;
        enumOwner = nullptr;
    }

    TypeInfo* clone() override;

    Scope*        scope;
    TypeInfoEnum* enumOwner;
};

struct TypeInfoFuncAttrParam : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind = TypeInfoKind::FuncAttrParam;
        namedParam.clear();
        typeInfo = nullptr;
        index    = 0;
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto castedFrom = static_cast<TypeInfoFuncAttrParam*>(from);
        return typeInfo->isSame(castedFrom->typeInfo);
    }

    int numRegisters() override
    {
        return typeInfo->numRegisters();
    }

    TypeInfo* clone() override;

    Utf8          namedParam;
    TypeInfo*     typeInfo;
    ComputedValue genericValue;
    int           index;
};

enum MatchResult
{
    Ok,
    TooManyParameters,
    TooManyGenericParameters,
    NotEnoughParameters,
    NotEnoughGenericParameters,
    BadSignature,
    BadGenericSignature,
    InvalidNamedParameter,
    DuplicatedNamedParameter,
};

struct SymbolMatchContext
{
    int                   badSignatureParameterIdx;
    TypeInfo*             badSignatureRequestedType;
    TypeInfo*             badSignatureGivenType;
    MatchResult           result;
    vector<AstNode*>      genericParameters;
    vector<AstNode*>      parameters;
    vector<bool>          doneParameters;
    bool                  forLambda;
    vector<ComputedValue> genericParametersCallValues;
    vector<TypeInfo*>     genericParametersCallTypes;

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
        forLambda = false;
    }
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

    TypeInfo* clone() override;
    void      computeName();

    void match(SymbolMatchContext& context);
    bool isSame(TypeInfoFuncAttr* from);
    bool isSame(TypeInfo* from) override;
    bool isSameExact(TypeInfo* from) override;

    int                            firstDefaultValueIdx;
    vector<TypeInfoFuncAttrParam*> genericParameters;
    vector<TypeInfoFuncAttrParam*> parameters;
    TypeInfo*                      returnType;
    int                            stackSize;
};

struct TypeInfoPointer : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind        = TypeInfoKind::Pointer;
        pointedType = nullptr;
        ptrCount    = 0;
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto castedFrom = static_cast<TypeInfoPointer*>(from);
        if (ptrCount != castedFrom->ptrCount)
            return false;
        return pointedType->isSame(castedFrom->pointedType);
    }

    TypeInfo* clone() override;

    uint32_t sizeOfPointedBy()
    {
        int size;
        if (ptrCount == 1)
            size = pointedType->sizeOf;
        else
            size = sizeof(void*);
        return size;
    }

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
        count       = 0;
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto castedFrom = static_cast<TypeInfoArray*>(from);
        if (count != castedFrom->count)
            return false;
        return pointedType->isSame(castedFrom->pointedType);
    }

    TypeInfo* clone() override;

    TypeInfo* pointedType;
    uint32_t  count;
};

struct TypeInfoSlice : public TypeInfo
{
    void reset() override
    {
        TypeInfo::reset();
        kind        = TypeInfoKind::Slice;
        pointedType = nullptr;
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto castedFrom = static_cast<TypeInfoSlice*>(from);
        return pointedType->isSame(castedFrom->pointedType);
    }

    TypeInfo* clone() override;

    TypeInfo* pointedType;
};

enum class TypeInfoListKind
{
    Array,
    Tuple,
};

struct TypeInfoList : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        kind = TypeInfoKind::TypeList;
        childs.clear();
        scope    = nullptr;
        listKind = TypeInfoListKind::Array;
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto other = static_cast<TypeInfoList*>(from);
        if (childs.size() != other->childs.size())
            return false;
        if (listKind != other->listKind)
            return false;
        for (int i = 0; i < childs.size(); i++)
        {
            if (!childs[i]->isSame(other->childs[i]))
                return false;
        }

        return true;
    }

    bool isSameExact(TypeInfo* from) override
    {
        if (!isSame(from))
            return false;
        auto other = static_cast<TypeInfoList*>(from);
        if (scope != other->scope)
            return false;
        return true;
    }

    int numRegisters() override
    {
        return 1;
    }

    TypeInfo* clone() override;

    TypeInfoListKind  listKind;
    vector<TypeInfo*> childs;
    Scope*            scope;
};

struct TypeInfoVariadic : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        kind = TypeInfoKind::Variadic;
    }

    TypeInfo* clone() override;
};

struct TypeInfoGeneric : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        kind = TypeInfoKind::Generic;
    }

    bool isSame(TypeInfo* from) override
    {
        if (from->kind == kind)
            return name == from->name;
        return true;
    }

    bool isSameExact(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        return name == from->name;
    }

    TypeInfo* clone() override;
};

struct TypeInfoStruct : public TypeInfo
{
    void reset()
    {
        TypeInfo::reset();
        childs.clear();
        kind       = TypeInfoKind::Struct;
        scope      = nullptr;
        structNode = nullptr;
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto other = static_cast<TypeInfoStruct*>(from);
        if (scope != other->scope)
            return false;
        if (childs.size() != other->childs.size())
            return false;
        for (int i = 0; i < childs.size(); i++)
        {
            if (!childs[i]->isSame(other->childs[i]))
                return false;
        }
        return true;
    }

    TypeInfo* clone() override;

    int numRegisters() override
    {
        return 1;
    }

    Scope*            scope;
    vector<TypeInfo*> childs;
    struct AstNode*   structNode;
};

extern Pool<TypeInfoFuncAttr>      g_Pool_typeInfoFuncAttr;
extern Pool<TypeInfoNamespace>     g_Pool_typeInfoNamespace;
extern Pool<TypeInfoEnum>          g_Pool_typeInfoEnum;
extern Pool<TypeInfoEnumValue>     g_Pool_typeInfoEnumValue;
extern Pool<TypeInfoFuncAttrParam> g_Pool_typeInfoFuncAttrParam;
extern Pool<TypeInfoPointer>       g_Pool_typeInfoPointer;
extern Pool<TypeInfoArray>         g_Pool_typeInfoArray;
extern Pool<TypeInfoSlice>         g_Pool_typeInfoSlice;
extern Pool<TypeInfoList>          g_Pool_typeInfoList;
extern Pool<TypeInfoNative>        g_Pool_typeInfoNative;
extern Pool<TypeInfoVariadic>      g_Pool_typeInfoVariadic;
extern Pool<TypeInfoGeneric>       g_Pool_typeInfoGeneric;
extern Pool<TypeInfoStruct>        g_Pool_typeInfoStruct;
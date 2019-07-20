#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SpinLock.h"
#include "Log.h"
struct Scope;
struct TypeInfo;
struct SymbolMatchContext;
struct Job;
enum class Intrisic;
struct AstFuncCallParam;

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

static const uint64_t TYPEINFO_ATTRIBUTE_FUNC = 0x00000000'00000001;
static const uint64_t TYPEINFO_ATTRIBUTE_VAR  = 0x00000000'00000002;
static const uint64_t TYPEINFO_INTEGER        = 0x00000000'00000004;
static const uint64_t TYPEINFO_FLOAT          = 0x00000000'00000008;
static const uint64_t TYPEINFO_UNSIGNED       = 0x00000000'00000010;
static const uint64_t TYPEINFO_CONST          = 0x00000000'00000020;
static const uint64_t TYPEINFO_IN_MANAGER     = 0x00000000'00000040;

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

    static const char* getNakedName(TypeInfo* typeInfo);

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
    TypeInfoNamespace()
    {
        kind = TypeInfoKind::Namespace;
    }

    void reset() override
    {
        scope = nullptr;
        TypeInfo::reset();
    }

    TypeInfo* clone() override;

    Scope* scope;
};

struct TypeInfoEnum : public TypeInfo
{
    TypeInfoEnum()
    {
        kind = TypeInfoKind::Enum;
    }

    void reset() override
    {
        scope   = nullptr;
        rawType = nullptr;
        TypeInfo::reset();
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
    TypeInfoEnumValue()
    {
        kind = TypeInfoKind::EnumValue;
    }

    void reset() override
    {
        scope     = nullptr;
        enumOwner = nullptr;
        TypeInfo::reset();
    }

    TypeInfo* clone() override;

    Scope*        scope;
    TypeInfoEnum* enumOwner;
};

struct TypeInfoFuncAttrParam : public TypeInfo
{
    TypeInfoFuncAttrParam()
    {
        kind = TypeInfoKind::FuncAttrParam;
    }

    void reset() override
    {
        typeInfo = nullptr;
        index    = 0;
        TypeInfo::reset();
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

    Utf8      namedParam;
    TypeInfo* typeInfo;
    int       index;
};

enum MatchResult
{
    Ok,
    TooManyParameters,
    NotEnoughParameters,
    BadSignature,
    InvalidNamedParameter,
    DuplicatedNamedParameter,
};

struct SymbolMatchContext
{
    int                       badSignatureParameterIdx;
    TypeInfo*                 badSignatureRequestedType;
    TypeInfo*                 badSignatureGivenType;
    MatchResult               result;
    vector<AstFuncCallParam*> parameters;
    vector<bool>              doneParameters;
    bool                      forLambda;

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
        forLambda                 = false;
        parameters.clear();
    }
};

struct TypeInfoFuncAttr : public TypeInfo
{
    TypeInfoFuncAttr()
    {
        kind = TypeInfoKind::FuncAttr;
    }

    void reset() override
    {
        firstDefaultValueIdx = -1;
        parameters.clear();
        returnType = nullptr;
        TypeInfo::reset();
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
        return returnType->numRegisters();
    }

    TypeInfo* clone() override;

    void match(SymbolMatchContext& context);
    bool isSame(TypeInfoFuncAttr* from);
    bool isSame(TypeInfo* from) override;

    int                            firstDefaultValueIdx;
    vector<TypeInfoFuncAttrParam*> parameters;
    TypeInfo*                      returnType;
};

struct TypeInfoPointer : public TypeInfo
{
    TypeInfoPointer()
    {
        kind = TypeInfoKind::Pointer;
    }

    void reset() override
    {
        pointedType = nullptr;
        ptrCount    = 0;
        TypeInfo::reset();
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
    TypeInfoArray()
    {
        kind = TypeInfoKind::Array;
    }

    void reset() override
    {
        pointedType = nullptr;
        count       = 0;
        TypeInfo::reset();
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
    TypeInfoSlice()
    {
        kind = TypeInfoKind::Slice;
    }

    void reset() override
    {
        pointedType = nullptr;
        TypeInfo::reset();
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

struct TypeInfoList : public TypeInfo
{
    TypeInfoList()
    {
        kind = TypeInfoKind::TypeList;
    }

    void reset()
    {
        childs.clear();
        TypeInfo::reset();
    }

    bool isSame(TypeInfo* from) override
    {
        if (!TypeInfo::isSame(from))
            return false;
        auto other = static_cast<TypeInfoList*>(from);
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

    vector<TypeInfo*> childs;
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
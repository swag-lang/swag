#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SpinLock.h"
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
    Pointer,
    Array,
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
    assert(casted->kind == kind);
    return casted;
}

static const uint64_t TYPEINFO_ATTRIBUTE_FUNC = 0x00000000'00000001;
static const uint64_t TYPEINFO_ATTRIBUTE_VAR  = 0x00000000'00000002;
static const uint64_t TYPEINFO_INTEGER        = 0x00000000'00000004;
static const uint64_t TYPEINFO_FLOAT          = 0x00000000'00000008;

struct TypeInfo : public PoolElement
{
    virtual bool isSame(TypeInfo* from)
    {
        return this == from;
    }

    bool isNative(NativeType native)
    {
        return (kind == TypeInfoKind::Native) && (nativeType == native);
    }

    bool isNativeInteger()
    {
        return flags & TYPEINFO_INTEGER;
    }

    static const char* getNakedName(TypeInfo* typeInfo);

    uint64_t     flags = 0;
    TypeInfoKind kind;
    NativeType   nativeType = NativeType::Void;
    Utf8         name;
    int          sizeOf = 0;
};

struct TypeInfoNative : public TypeInfo
{
    TypeInfoNative(NativeType type, const char* tname, int sof, uint64_t fl)
    {
        kind       = TypeInfoKind::Native;
        nativeType = type;
        name       = tname;
        sizeOf     = sof;
        flags      = fl;
    }
};

struct TypeInfoNamespace : public TypeInfo
{
    TypeInfoNamespace()
    {
        kind = TypeInfoKind::Namespace;
    }

    Scope* scope;
};

struct TypeInfoEnum : public TypeInfo
{
    Scope*    scope;
    TypeInfo* rawType = nullptr;

    TypeInfoEnum()
    {
        kind = TypeInfoKind::Enum;
    }
};

struct TypeInfoEnumValue : public TypeInfo
{
    TypeInfoEnumValue()
    {
        kind = TypeInfoKind::EnumValue;
    }

    Scope*        scope;
    TypeInfoEnum* enumOwner = nullptr;
};

struct TypeInfoFuncAttrParam : public TypeInfo
{
    TypeInfoFuncAttrParam()
    {
        kind = TypeInfoKind::FuncAttrParam;
    }

    Utf8      name;
    TypeInfo* typeInfo;
    int       index;
};

enum MatchResult
{
    Ok,
    TooManyParameters,
    NotEnoughParameters,
    BadSignature
};

struct SymbolMatchContext
{
    int                       badSignatureParameterIdx  = 0;
    TypeInfo*                 badSignatureRequestedType = nullptr;
    TypeInfo*                 badSignatureGivenType     = nullptr;
    MatchResult               result                    = MatchResult::Ok;
    vector<AstFuncCallParam*> parameters;
};

struct TypeInfoFuncAttr : public TypeInfo
{
    TypeInfoFuncAttr()
    {
        kind = TypeInfoKind::FuncAttr;
    }

    void match(SymbolMatchContext& context);
    bool isSame(TypeInfoFuncAttr* from);
    bool isSame(TypeInfo* from) override;

    int                            firstDefaultValueIdx = -1;
    vector<TypeInfoFuncAttrParam*> parameters;
    TypeInfo*                      returnType;
};

struct TypeInfoPointer : public TypeInfo
{
    TypeInfoPointer()
    {
        kind = TypeInfoKind::Pointer;
    }

    bool isSame(TypeInfo* from) override
    {
        if (kind != from->kind)
            return false;
        auto castedFrom = static_cast<TypeInfoPointer*>(from);
        if (ptrCount != castedFrom->ptrCount)
            return false;
        return pointedType->isSame(castedFrom->pointedType);
    }

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

    bool isSame(TypeInfo* from) override
    {
        if (kind != from->kind)
            return false;
        auto castedFrom = static_cast<TypeInfoArray*>(from);
        if (size != castedFrom->size)
            return false;
        return pointedType->isSame(castedFrom->pointedType);
    }

    TypeInfo* pointedType;
    uint32_t  size;
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
    }

    bool isSame(TypeInfo* from) override
    {
        if (kind != from->kind)
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

    vector<TypeInfo*> childs;
};

extern Pool<TypeInfoFuncAttr>      g_Pool_typeInfoFuncAttr;
extern Pool<TypeInfoNamespace>     g_Pool_typeInfoNamespace;
extern Pool<TypeInfoEnum>          g_Pool_typeInfoEnum;
extern Pool<TypeInfoEnumValue>     g_Pool_typeInfoEnumValue;
extern Pool<TypeInfoFuncAttrParam> g_Pool_typeInfoFuncAttrParam;
extern Pool<TypeInfoPointer>       g_Pool_typeInfoPointer;
extern Pool<TypeInfoArray>         g_Pool_typeInfoArray;
extern Pool<TypeInfoList>          g_Pool_typeInfoExpressionList;

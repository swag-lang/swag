#pragma once
#include "Pool.h"
#include "Utf8Crc.h"
#include "Log.h"
#include "Register.h"
#include "Attribute.h"
#include "VectorNative.h"
#include "Runtime.h"

struct Scope;
struct TypeInfo;
struct SymbolMatchContext;
struct Job;
enum class Intrisic;
struct AstNode;
struct ByteCode;
struct TypeInfoFuncAttr;
struct AstFuncDecl;
struct JobContext;

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
    SWAG_ASSERT(ptr);
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted->kind == kind);
    return casted;
}

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2));
    return casted;
}

static const uint32_t TYPEINFO_SELF                     = 0x00000001;
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
static const uint32_t TYPEINFO_STRUCT_TYPEINFO          = 0x00100000;
static const uint32_t TYPEINFO_STRUCT_IS_TUPLE          = 0x00200000;
static const uint32_t TYPEINFO_ENUM_FLAGS               = 0x00400000;
static const uint32_t TYPEINFO_SPREAD                   = 0x00800000;
static const uint32_t TYPEINFO_UNDEFINED                = 0x01000000;
static const uint32_t TYPEINFO_ENUM_INDEX               = 0x02000000;

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

    bool isPointer1();
    bool isPointerTo(TypeInfoKind pointerKind);
    bool isPointerTo(TypeInfo* finalType);
    bool isPointerVoid();
    bool isPointerConstVoid();
    bool isPointerToTypeInfo();
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
        if (flags & TYPEINFO_CONST)
            return;
        flags |= TYPEINFO_CONST;
        preName = "const " + preName;
        name    = "const " + name;
    }

    virtual int numRegisters()
    {
        if (sizeOf == 0)
            return 0;
        return max(sizeOf, sizeof(void*)) / sizeof(void*);
    }

    void copyFrom(TypeInfo* from)
    {
        flags      = from->flags & ~TYPEINFO_IN_EXACT_LIST;
        kind       = from->kind;
        nativeType = from->nativeType;
        preName    = from->preName;
        nakedName  = from->nakedName;
        name       = from->name;
        sizeOf     = from->sizeOf;
        declNode   = from->declNode;
    }

    void               forceComputeName();
    virtual TypeInfo*  clone() = 0;
    virtual void       computeName();
    virtual void       computeScopedName();
    void               computeScopedNameNoLock();
    static const char* getArticleKindName(TypeInfo* typeInfo);
    static const char* getNakedKindName(TypeInfo* typeInfo);

    Utf8         preName;    // List const, *, [] ...
    Utf8         nakedName;  // The simple type name
    Utf8         name;       // preName + nakedName
    Utf8Crc      scopedName; // preName + scope name + nakedName
    shared_mutex mutex;
    shared_mutex mutexScopeName;

    TypeInfo* constCopy = nullptr;
    AstNode*  declNode  = nullptr;

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
        nakedName    = name;
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

    int  index    = 0;
    int  offset   = 0;
    bool hasUsing = false;
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
    BadGenericMatch,
    BadGenericSignature,
    InvalidNamedParameter,
    MissingNamedParameter,
    DuplicatedNamedParameter,
};

struct BadSignatureInfos
{
    TypeInfo* badSignatureRequestedType;
    TypeInfo* badSignatureGivenType;
    Utf8      badGenMatch;
    int       badSignatureParameterIdx;

    void clear()
    {
        badGenMatch.clear();
        badSignatureParameterIdx  = -1;
        badSignatureRequestedType = nullptr;
        badSignatureGivenType     = nullptr;
    }
};

struct SymbolMatchContext
{
    static const uint32_t MATCH_ACCEPT_NO_GENERIC = 0x00000001;
    static const uint32_t MATCH_FOR_LAMBDA        = 0x00000002;
    static const uint32_t MATCH_GENERIC_AUTO      = 0x00000004;
    static const uint32_t MATCH_ERROR_VALUE_TYPE  = 0x00000008;
    static const uint32_t MATCH_ERROR_TYPE_VALUE  = 0x00000010;
    static const uint32_t MATCH_UNCONST           = 0x00000020;
    static const uint32_t MATCH_UFCS              = 0x00000040;

    SymbolMatchContext()
    {
        reset();
    }

    void reset()
    {
        result = MatchResult::Ok;
        badSignatureInfos.clear();
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
    BadSignatureInfos            badSignatureInfos;

    uint32_t flags;
    int      cptResolved;

    bool hasNamedParameters;
};

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

    int                     firstDefaultValueIdx = -1;
    int                     stackSize            = 0;
    map<Utf8Crc, TypeInfo*> replaceTypes;
    uint32_t                attributeUsage = 0xFFFFFFFF; // All by default
};

struct TypeInfoPointer : public TypeInfo
{
    TypeInfoPointer()
    {
        kind = TypeInfoKind::Pointer;
    }

    void      computeName() override;
    void      computeScopedName() override;
    void      computePointedType();
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* finalType   = nullptr;
    TypeInfo* pointedType = nullptr;

    uint32_t ptrCount = 0;
};

struct TypeInfoReference : public TypeInfo
{
    TypeInfoReference()
    {
        kind   = TypeInfoKind::Reference;
        sizeOf = sizeof(void*);
    }

    void      computeName() override;
    void      computeScopedName() override;
    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* pointedType  = nullptr;
    TypeInfo* originalType = nullptr;
};

struct TypeInfoArray : public TypeInfo
{
    TypeInfoArray()
    {
        kind = TypeInfoKind::Array;
        flags |= TYPEINFO_RETURN_BY_COPY;
    }

    int numRegisters() override
    {
        return 1;
    }

    void      computeName() override;
    void      computeScopedName() override;
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
        kind   = TypeInfoKind::Slice;
        sizeOf = 2 * sizeof(void*);
    }

    void      computeName() override;
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
    Utf8      computeTupleName(JobContext* context);

    VectorNative<TypeInfoParam*> subTypes;

    Scope* scope = nullptr;
};

struct TypeInfoVariadic : public TypeInfo
{
    TypeInfoVariadic()
    {
        kind = TypeInfoKind::Variadic;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;
    void      computeName() override;

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
    }

    int numRegisters() override
    {
        if (kind == TypeInfoKind::Interface || kind == TypeInfoKind::TypeSet)
            return 2;
        return 1;
    }

    bool           isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo*      clone() override;
    void           computeScopedName() override;
    void           computeName() override;
    void           match(SymbolMatchContext& context);
    TypeInfoParam* findChildByNameNoLock(const Utf8& childName);
    TypeInfoParam* hasInterface(TypeInfoStruct* itf);
    TypeInfoParam* hasInterfaceNoLock(TypeInfoStruct* itf);

    VectorNative<TypeInfoParam*> genericParameters;
    VectorNative<TypeInfoParam*> fields;
    VectorNative<TypeInfoParam*> consts;
    VectorNative<TypeInfoParam*> methods;
    VectorNative<TypeInfoParam*> interfaces;
    SymbolAttributes             attributes;
    Utf8                         structName;

    TypeInfoStruct* itable            = nullptr;
    TypeInfoStruct* fromGeneric       = nullptr;
    Scope*          scope             = nullptr;
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

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    TypeInfo* rawType = nullptr;
};

struct TypeInfoCode : public TypeInfo
{
    TypeInfoCode()
    {
        name      = "code";
        nakedName = "code";
        kind      = TypeInfoKind::Code;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;

    AstNode* content = nullptr;
};

struct TypeInfoNameAlias : public TypeInfo
{
    TypeInfoNameAlias()
    {
        name      = "alias";
        nakedName = "alias";
        kind      = TypeInfoKind::NameAlias;
    }

    bool      isSame(TypeInfo* to, uint32_t isSameFlags) override;
    TypeInfo* clone() override;
};

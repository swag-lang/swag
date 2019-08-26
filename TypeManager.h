#pragma once
#include "TypeInfo.h"
struct AstNode;
struct SourceFile;
struct TypeInfo;
struct TypeInfoNative;
struct TypeInfoVariadic;
struct ErrorContext;

static const uint32_t CASTFLAG_NOERROR       = 0x00000001;
static const uint32_t CASTFLAG_FORCE         = 0x00000002;
static const uint32_t CASTFLAG_CONCRETE_ENUM = 0x00000004;
static const uint32_t CASTFLAG_JUST_CHECK    = 0x00000008;

enum MakeConcrete
{
    FlagAll       = 0xFFFFFFFF,
    FlagEnum      = 0x00000001,
    FlagEnumValue = 0x00000002,
    FlagFunc      = 0x00000004,
    FlagGeneric   = 0x00000008,
};

struct TypeManager
{
    void setup();

    static bool castError(ErrorContext* errorContext, TypeInfo* requestedType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags, bool explicitIsValid = false);

    static bool castToNativeBool(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU8(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU16(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU32(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU64(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS8(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS16(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS32(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS64(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF32(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF64(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);

    static bool castToNative(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToArray(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToTuple(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToSlice(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);

    static bool makeCompatibles(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags = 0);
    static bool makeCompatibles(ErrorContext* errorContext, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags = 0);
    static bool makeCompatibles(ErrorContext* errorContext, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags = 0);

    static void      promote(AstNode* left, AstNode* right);
    static void      promoteOne(AstNode* left, AstNode* right);
    static TypeInfo* concreteType(TypeInfo* typeInfo, MakeConcrete flags = MakeConcrete::FlagAll);

    TypeInfo* registerType(TypeInfo* typeInfo);

    TypeInfoNative*   typeInfoS8;
    TypeInfoNative*   typeInfoS16;
    TypeInfoNative*   typeInfoS32;
    TypeInfoNative*   typeInfoS64;
    TypeInfoNative*   typeInfoU8;
    TypeInfoNative*   typeInfoU16;
    TypeInfoNative*   typeInfoU32;
    TypeInfoNative*   typeInfoU64;
    TypeInfoNative*   typeInfoBool;
    TypeInfoNative*   typeInfoF32;
    TypeInfoNative*   typeInfoF64;
    TypeInfoNative*   typeInfoChar;
    TypeInfoNative*   typeInfoString;
    TypeInfoNative*   typeInfoVoid;
    TypeInfoPointer*  typeInfoNull;
    TypeInfoVariadic* typeInfoVariadic;
    TypeInfoVariadic* typeInfoVariadicValue;

    TypeInfoNative* promoteMatrix[(int) NativeType::Count][(int) NativeType::Count];

    SpinLock          mutexTypes;
    vector<TypeInfo*> allTypes;
};

extern TypeManager g_TypeMgr;
#pragma once
#include "TypeInfo.h"
struct AstNode;
struct SourceFile;
struct TypeInfo;
struct TypeInfoNative;
struct TypeInfoVariadic;
struct SemanticContext;

static const uint32_t CASTFLAG_NOERROR       = 0x00000001;
static const uint32_t CASTFLAG_FORCE         = 0x00000002;
static const uint32_t CASTFLAG_CONCRETE_ENUM = 0x00000004;
static const uint32_t CASTFLAG_JUST_CHECK    = 0x00000008;
static const uint32_t CASTFLAG_UNCONST       = 0x00000010;
static const uint32_t CASTFLAG_AUTO_BOOL     = 0x00000020;

enum MakeConcrete
{
    FlagAll     = 0xFFFFFFFF,
    FlagEnum    = 0x00000001,
    FlagFunc    = 0x00000002,
    FlagGeneric = 0x00000004,
    FlagAlias   = 0x00000008,
};

struct TypeManager
{
    void setup();

    static bool castError(SemanticContext* context, TypeInfo* requestedType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags, bool explicitIsValid = false);

    static bool castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeChar(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);

	static bool castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToTuple(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);

    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags = 0);

    static void      promote(AstNode* left, AstNode* right);
    static void      promoteOne(AstNode* left, AstNode* right);
    static TypeInfo* concreteType(TypeInfo* typeInfo, MakeConcrete flags = MakeConcrete::FlagAll);

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
    TypeInfoNative*   typeInfoAny;
    TypeInfoNative*   typeInfoVoid;
    TypeInfoPointer*  typeInfoNull;
    TypeInfoVariadic* typeInfoVariadic;
    TypeInfoVariadic* typeInfoVariadicValue;

    TypeInfoNative* promoteMatrix[(int) NativeTypeKind::Count][(int) NativeTypeKind::Count];
};

extern TypeManager g_TypeMgr;
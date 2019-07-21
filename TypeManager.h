#pragma once
#include "TypeInfo.h"
struct AstNode;
struct SourceFile;
struct TypeInfo;
struct TypeInfoNative;

static const uint32_t CASTFLAG_NOERROR       = 0x00000001;
static const uint32_t CASTFLAG_FORCE         = 0x00000002;
static const uint32_t CASTFLAG_CONCRETE_ENUM = 0x00000004;
static const uint32_t CASTFLAG_JUST_CHECK    = 0x00000008;

enum MakeConcrete
{
    FlagAll  = 0xFFFFFFFF,
    FlagEnum = 0x00000001,
    FlagFunc = 0x00000002,
};

struct TypeManager
{
    void setup();

    static bool castError(SourceFile* sourceFile, TypeInfo* requestedType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags, bool explicitIsValid = false);

    static bool castToNativeBool(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU8(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU16(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU32(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU64(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS8(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS16(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS32(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS64(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF32(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF64(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);

    static bool castToNative(SourceFile* sourceFile, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToArray(SourceFile* sourceFile, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToSlice(SourceFile* sourceFile, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags);

    static bool makeCompatibles(SourceFile* sourceFile, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags = 0);
    static bool makeCompatibles(SourceFile* sourceFile, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags = 0);
    static bool makeCompatibles(SourceFile* sourceFile, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags = 0);

    static void      promote(AstNode* left, AstNode* right);
    static void      promoteOne(AstNode* left, AstNode* right);
    static TypeInfo* concreteType(TypeInfo* typeInfo, MakeConcrete flags = MakeConcrete::FlagAll);

    TypeInfo* registerType(TypeInfo* typeInfo);

    TypeInfoNative*  typeInfoS8;
    TypeInfoNative*  typeInfoS16;
    TypeInfoNative*  typeInfoS32;
    TypeInfoNative*  typeInfoS64;
    TypeInfoNative*  typeInfoU8;
    TypeInfoNative*  typeInfoU16;
    TypeInfoNative*  typeInfoU32;
    TypeInfoNative*  typeInfoU64;
    TypeInfoNative*  typeInfoBool;
    TypeInfoNative*  typeInfoF32;
    TypeInfoNative*  typeInfoF64;
    TypeInfoNative*  typeInfoChar;
    TypeInfoNative*  typeInfoString;
    TypeInfoNative*  typeInfoVoid;
    TypeInfoPointer* typeInfoNull;

    TypeInfoNative* promoteMatrix[(int) NativeType::Count][(int) NativeType::Count];

    SpinLock          mutexTypes;
    vector<TypeInfo*> allTypes;
};

extern TypeManager g_TypeMgr;
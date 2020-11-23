#pragma once
#include "TypeInfo.h"
struct AstNode;
struct SourceFile;
struct TypeInfo;
struct TypeInfoNative;
struct TypeInfoVariadic;
struct SemanticContext;
enum class LiteralType;
struct Token;

static const uint32_t CASTFLAG_NO_ERROR        = 0x00000001;
static const uint32_t CASTFLAG_EXPLICIT        = 0x00000002;
static const uint32_t CASTFLAG_CONCRETE_ENUM   = 0x00000004;
static const uint32_t CASTFLAG_JUST_CHECK      = 0x00000008;
static const uint32_t CASTFLAG_UNCONST         = 0x00000010;
static const uint32_t CASTFLAG_AUTO_BOOL       = 0x00000020;
static const uint32_t CASTFLAG_NO_COLLECT      = 0x00000040;
static const uint32_t CASTFLAG_BIJECTIF        = 0x00000080;
static const uint32_t CASTFLAG_STRICT          = 0x00000100;
static const uint32_t CASTFLAG_FORCE_UNCONST   = 0x00000200;
static const uint32_t CASTFLAG_COMPARE         = 0x00000400;
static const uint32_t CASTFLAG_UFCS            = 0x00000800;
static const uint32_t CASTFLAG_COERCE          = 0x00001000;
static const uint32_t CASTFLAG_COERCE_SAMESIGN = 0x00002000;
static const uint32_t CASTFLAG_COERCE_FULL     = 0x00004000;

static const uint32_t CONCRETE_ALL     = 0xFFFFFFFF;
static const uint32_t CONCRETE_ENUM    = 0x00000001;
static const uint32_t CONCRETE_FUNC    = 0x00000002;
static const uint32_t CONCRETE_GENERIC = 0x00000004;
static const uint32_t CONCRETE_ALIAS   = 0x00000008;

struct TypeManager
{
    void setup();

    static bool castError(SemanticContext* context, TypeInfo* requestedType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static bool castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeChar(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static bool castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags);
    static bool castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToString(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToFromTypeSet(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags);
    static bool castToFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags);
    static bool castToReference(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castStructToStruct(SemanticContext* context, TypeInfoStruct* toStruct, TypeInfoStruct* fromStruct, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags, bool& ok);
    static bool castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static bool            convertLiteralTupleToStruct(SemanticContext* context, TypeInfo* toType, AstNode* fromNode);
    static TypeInfoArray*  convertTypeListToArray(JobContext* jobContext, TypeInfoList* typeList, bool isCompilerConstant);
    static TypeInfoStruct* convertTypeListToStruct(JobContext* jobContext, TypeInfoList* typeList, bool isCompilerConstant);

    static bool makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags = 0);

    static TypeInfo* solidifyUntyped(TypeInfo* typeInfo);
    static TypeInfo* makeUntypedType(TypeInfo* typeInfo, uint32_t value);
    void             registerTypeType();
    static TypeInfo* literalTypeToType(LiteralType literalType);
    static TypeInfo* literalTypeToType(const Token& token);
    static void      promote(AstNode* left, AstNode* right);
    static void      promoteOne(AstNode* left, AstNode* right);
    static bool      promoteOne(SemanticContext* context, AstNode* right);
    static TypeInfo* promoteUntyped(TypeInfo* typeInfo);
    static void      promoteUntypedInteger(AstNode* left, AstNode* right);
    static TypeInfo* concreteReferenceType(TypeInfo* typeInfo, uint32_t flags = CONCRETE_ALL);
    static TypeInfo* concreteReference(TypeInfo* typeInfo);
    static TypeInfo* concreteType(TypeInfo* typeInfo, uint32_t flags = CONCRETE_ALL);

    TypeInfoNative*    typeInfoS8;
    TypeInfoNative*    typeInfoS16;
    TypeInfoNative*    typeInfoS32;
    TypeInfoNative*    typeInfoS64;
    TypeInfoNative*    typeInfoU8;
    TypeInfoNative*    typeInfoU16;
    TypeInfoNative*    typeInfoU32;
    TypeInfoNative*    typeInfoU64;
    TypeInfoNative*    typeInfoBool;
    TypeInfoNative*    typeInfoF32;
    TypeInfoNative*    typeInfoF64;
    TypeInfoNative*    typeInfoChar;
    TypeInfoNative*    typeInfoString;
    TypeInfoNative*    typeInfoAny;
    TypeInfoNative*    typeInfoVoid;
    TypeInfoNative*    typeInfoUndefined;
    TypeInfoPointer*   typeInfoNull;
    TypeInfoPointer*   typeInfoPVoid;
    TypeInfoPointer*   typeInfoConstPVoid;
    TypeInfoPointer*   typeInfoTypeType;
    TypeInfoVariadic*  typeInfoVariadic;
    TypeInfoNative*    typeInfoUntypedInt;
    TypeInfoNative*    typeInfoUntypedBinHexa;
    TypeInfoNative*    typeInfoUntypedFloat;
    TypeInfoFuncAttr*  typeInfoOpCall;
    TypeInfoCode*      typeInfoCode;
    TypeInfoNameAlias* typeInfoNameAlias;

    TypeInfoNative* promoteMatrix[(int) NativeTypeKind::Count][(int) NativeTypeKind::Count];
};

extern TypeManager g_TypeMgr;
#pragma once
#include "TypeInfo.h"
struct AstNode;
struct SourceFile;
struct TypeInfo;
struct TypeInfoNative;
struct TypeInfoVariadic;
struct SemanticContext;
enum class LiteralType : uint8_t;
struct Token;

static const uint32_t CASTFLAG_DEFAULT        = 0x00000000;
static const uint32_t CASTFLAG_NO_ERROR       = 0x00000001;
static const uint32_t CASTFLAG_EXPLICIT       = 0x00000002;
static const uint32_t CASTFLAG_CONCRETE_ENUM  = 0x00000004;
static const uint32_t CASTFLAG_JUST_CHECK     = 0x00000008;
static const uint32_t CASTFLAG_UNCONST        = 0x00000010;
static const uint32_t CASTFLAG_AUTO_BOOL      = 0x00000020;
static const uint32_t CASTFLAG_NO_COLLECT     = 0x00000040;
static const uint32_t CASTFLAG_COMMUTATIVE    = 0x00000080;
static const uint32_t CASTFLAG_INDEX          = 0x00000100;
static const uint32_t CASTFLAG_FORCE_UNCONST  = 0x00000200;
static const uint32_t CASTFLAG_COMPARE        = 0x00000400;
static const uint32_t CASTFLAG_UFCS           = 0x00000800;
static const uint32_t CASTFLAG_COERCE         = 0x00001000;
static const uint32_t CASTFLAG_TRY_COERCE     = 0x00002000;
static const uint32_t CASTFLAG_AUTO_OPCAST    = 0x00004000;
static const uint32_t CASTFLAG_STRICT         = 0x00008000;
static const uint32_t CASTFLAG_CONST_ERR      = 0x00010000;
static const uint32_t CASTFLAG_FOR_AFFECT     = 0x00020000;
static const uint32_t CASTFLAG_ACCEPT_PENDING = 0x00040000;
static const uint32_t CASTFLAG_LITERAL_SUFFIX = 0x00080000;
static const uint32_t CASTFLAG_NO_LAST_MINUTE = 0x00100000;
static const uint32_t CASTFLAG_NO_ITF         = 0x00200000;
static const uint32_t CASTFLAG_PARAMS         = 0x00400000;

static const uint32_t CONCRETE_ALL        = 0xFFFFFFFF;
static const uint32_t CONCRETE_ENUM       = 0x00000001;
static const uint32_t CONCRETE_FUNC       = 0x00000002;
static const uint32_t CONCRETE_GENERIC    = 0x00000004;
static const uint32_t CONCRETE_ALIAS      = 0x00000008;
static const uint32_t CONCRETE_FORCEALIAS = 0x00000010;

struct InterfaceRef
{
    TypeInfoParam* itf         = nullptr;
    uint32_t       fieldOffset = 0;
    Utf8           fieldRef;
};

struct TypeManager
{
    void setup();

    static bool safetyComputedValue(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castError(SemanticContext* context, TypeInfo* requestedType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool tryOpCast(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool tryOpAffect(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static bool castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeChar(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeUInt(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeInt(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static bool castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags);
    static bool castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToString(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags);
    static bool castToReference(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castStructToStruct(SemanticContext* context, TypeInfoStruct* toStruct, TypeInfoStruct* fromStruct, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags, bool& ok);
    static bool castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToLambda(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static bool            convertLiteralTupleToStructVar(SemanticContext* context, TypeInfo* toType, AstNode* fromNode);
    static TypeInfoArray*  convertTypeListToArray(JobContext* jobContext, TypeInfoList* typeList, bool isCompilerConstant);
    static TypeInfoStruct* convertTypeListToStruct(JobContext* jobContext, TypeInfoList* typeList, bool isCompilerConstant);

    static bool makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags = 0);

    static bool      collectInterface(SemanticContext* context, TypeInfoStruct* fromTypeStruct, TypeInfoStruct* toTypeItf, InterfaceRef& ref, bool skipFirst = false);
    static TypeInfo* solidifyUntyped(TypeInfo* typeInfo);
    static TypeInfo* makeUntypedType(TypeInfo* typeInfo, uint32_t value);
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
    static TypeInfo* makeConst(TypeInfo* typeInfo);
    static uint64_t  align(uint64_t value, uint32_t align);
    static uint32_t  alignOf(TypeInfo* typeInfo);
    static bool      compareConcreteType(const ConcreteTypeInfo* type1, const ConcreteTypeInfo* type2);

    void             registerTypeType();
    TypeInfoPointer* makePointerTo(TypeInfo* toType, bool isConst, uint64_t ptrFlags = 0);
    TypeInfoParam*   makeParam();

    TypeInfoNative* typeInfoS8        = nullptr;
    TypeInfoNative* typeInfoS16       = nullptr;
    TypeInfoNative* typeInfoS32       = nullptr;
    TypeInfoNative* typeInfoS64       = nullptr;
    TypeInfoNative* typeInfoU8        = nullptr;
    TypeInfoNative* typeInfoU16       = nullptr;
    TypeInfoNative* typeInfoU32       = nullptr;
    TypeInfoNative* typeInfoU64       = nullptr;
    TypeInfoNative* typeInfoBool      = nullptr;
    TypeInfoNative* typeInfoF32       = nullptr;
    TypeInfoNative* typeInfoF64       = nullptr;
    TypeInfoNative* typeInfoRune      = nullptr;
    TypeInfoNative* typeInfoString    = nullptr;
    TypeInfoNative* typeInfoAny       = nullptr;
    TypeInfoNative* typeInfoVoid      = nullptr;
    TypeInfoNative* typeInfoUndefined = nullptr;
    TypeInfoNative* typeInfoUInt      = nullptr;
    TypeInfoNative* typeInfoInt       = nullptr;

    TypeInfoPointer* typeInfoNull                                       = nullptr;
    TypeInfoPointer* typeInfoCString                                    = nullptr;
    TypeInfoPointer* typeInfoConstPointers[(int) NativeTypeKind::Count] = {0};
    TypeInfoPointer* typeInfoPointers[(int) NativeTypeKind::Count]      = {0};

    TypeInfoPointer*   typeInfoTypeType       = nullptr;
    TypeInfoVariadic*  typeInfoVariadic       = nullptr;
    TypeInfoVariadic*  typeInfoCVariadic      = nullptr;
    TypeInfoNative*    typeInfoUntypedInt     = nullptr;
    TypeInfoNative*    typeInfoUntypedBinHexa = nullptr;
    TypeInfoNative*    typeInfoUntypedFloat   = nullptr;
    TypeInfoFuncAttr*  typeInfoOpCall         = nullptr;
    TypeInfoFuncAttr*  typeInfoOpCall2        = nullptr;
    TypeInfoCode*      typeInfoCode           = nullptr;
    TypeInfoNameAlias* typeInfoNameAlias      = nullptr;
    TypeInfoSlice*     typeInfoSliceRunes     = nullptr;

    TypeInfoNative* promoteMatrix[(int) NativeTypeKind::Count][(int) NativeTypeKind::Count] = {{0}};
};

extern TypeManager* g_TypeMgr;
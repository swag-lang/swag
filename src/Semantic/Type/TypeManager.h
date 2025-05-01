#pragma once
#include "Main/Statistics.h"
#include "Semantic/Type/TypeInfo.h"

struct AstNode;
struct Diagnostic;
struct SemanticContext;
struct SourceFile;
struct Token;
struct TypeInfo;
struct TypeInfoNative;
struct TypeInfoVariadic;
enum class LiteralType : uint8_t;

using CastFlags       = Flags<uint64_t>;
using CastFlagsResult = Flags<uint32_t>;
using ToConcreteFlags = Flags<uint32_t>;

constexpr CastFlags CAST_FLAG_DEFAULT            = 0x0000000000000000;
constexpr CastFlags CAST_FLAG_INTERFACE          = 0x0000000000000001;
constexpr CastFlags CAST_FLAG_EXPLICIT           = 0x0000000000000002;
constexpr CastFlags CAST_FLAG_CONCRETE_ENUM      = 0x0000000000000004;
constexpr CastFlags CAST_FLAG_JUST_CHECK         = 0x0000000000000008;
constexpr CastFlags CAST_FLAG_UN_CONST           = 0x0000000000000010;
constexpr CastFlags CAST_FLAG_AUTO_BOOL          = 0x0000000000000020;
constexpr CastFlags CAST_FLAG_NO_COLLECT         = 0x0000000000000040;
constexpr CastFlags CAST_FLAG_COMMUTATIVE        = 0x0000000000000080;
constexpr CastFlags CAST_FLAG_INDEX              = 0x0000000000000100;
constexpr CastFlags CAST_FLAG_FORCE_UN_CONST     = 0x0000000000000200;
constexpr CastFlags CAST_FLAG_FOR_COMPARE        = 0x0000000000000400;
constexpr CastFlags CAST_FLAG_UFCS               = 0x0000000000000800;
constexpr CastFlags CAST_FLAG_COERCE             = 0x0000000000001000;
constexpr CastFlags CAST_FLAG_TRY_COERCE         = 0x0000000000002000;
constexpr CastFlags CAST_FLAG_AUTO_OP_CAST       = 0x0000000000004000;
constexpr CastFlags CAST_FLAG_STRICT             = 0x0000000000008000;
constexpr CastFlags CAST_FLAG_CAST               = 0x0000000000010000;
constexpr CastFlags CAST_FLAG_FOR_AFFECT         = 0x0000000000020000;
constexpr CastFlags CAST_FLAG_ACCEPT_PENDING     = 0x0000000000040000;
constexpr CastFlags CAST_FLAG_LITERAL_SUFFIX     = 0x0000000000080000;
constexpr CastFlags CAST_FLAG_NO_LAST_MINUTE     = 0x0000000000100000;
constexpr CastFlags CAST_FLAG_FOR_GENERIC        = 0x0000000000200000;
constexpr CastFlags CAST_FLAG_PARAMS             = 0x0000000000400000;
constexpr CastFlags CAST_FLAG_NO_IMPLICIT        = 0x0000000000800000;
constexpr CastFlags CAST_FLAG_FOR_VAR_INIT       = 0x0000000001000000;
constexpr CastFlags CAST_FLAG_PTR_REF            = 0x0000000002000000;
constexpr CastFlags CAST_FLAG_NO_TUPLE_TO_STRUCT = 0x0000000004000000;
constexpr CastFlags CAST_FLAG_ACCEPT_MOVE_REF    = 0x0000000008000000;
constexpr CastFlags CAST_FLAG_EXACT              = 0x0000000010000000;
constexpr CastFlags CAST_FLAG_CAN_OVERFLOW       = 0x0000000020000000;
constexpr CastFlags CAST_FLAG_EXACT_TUPLE_STRUCT = 0x0000000040000000;

constexpr CastFlagsResult CAST_RESULT_STRUCT_CONVERT      = 0x00000001;
constexpr CastFlagsResult CAST_RESULT_AUTO_OP_CAST        = 0x00000002;
constexpr CastFlagsResult CAST_RESULT_AUTO_OP_AFFECT      = 0x00000004;
constexpr CastFlagsResult CAST_RESULT_AUTO_MOVE_OP_AFFECT = 0x00000008;
constexpr CastFlagsResult CAST_RESULT_GUESS_MOVE          = 0x00000010;
constexpr CastFlagsResult CAST_RESULT_FORCE_REF           = 0x00000020;
constexpr CastFlagsResult CAST_RESULT_UNTYPED_CONVERT     = 0x00000040;
constexpr CastFlagsResult CAST_RESULT_COERCE              = 0x00000080;
constexpr CastFlagsResult CAST_RESULT_GEN_AUTO_OP_CAST    = 0x00000100;
constexpr CastFlagsResult CAST_RESULT_FROM_REF            = 0x00000200;
constexpr CastFlagsResult CAST_RESULT_CONST_COERCE        = 0x00000400;

constexpr ToConcreteFlags CONCRETE_ALL         = 0xFFFFFFFF;
constexpr ToConcreteFlags CONCRETE_ENUM        = 0x00000001;
constexpr ToConcreteFlags CONCRETE_FUNC        = 0x00000002;
constexpr ToConcreteFlags CONCRETE_GENERIC     = 0x00000004;
constexpr ToConcreteFlags CONCRETE_ALIAS       = 0x00000008;
constexpr ToConcreteFlags CONCRETE_FORCE_ALIAS = 0x00000010;

struct InterfaceRef
{
    TypeInfoParam* itf         = nullptr;
    uint32_t       fieldOffset = 0;
    Utf8           fieldRef;
};

struct TypeManager
{
    void setup();

    static bool isOverflowEnabled(const SemanticContext* context, const AstNode* fromNode, CastFlags castFlags);
    static bool safetyErrorOutOfRange(SemanticContext* context, AstNode* fromNode, const TypeInfo* fromType, TypeInfo* toType, bool isNeg = false);
    static bool safetyComputedValue(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static void getCastErrorMsg(Utf8& msg, Utf8& hint, Vector<Utf8>& remarks, TypeInfo* toType, TypeInfo* fromType, CastFlags castFlags, CastErrorType castError, Vector<const Diagnostic*>& notes);
    static bool castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags, CastErrorType castErrorType = CastErrorType::Zero);

    static bool tryOpCast(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool tryOpAffect(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);

    static bool castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeRune(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castSubExpressionList(SemanticContext* context, AstNode* child, TypeInfo* toType, CastFlags castFlags);
    static bool castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, CastFlags castFlags);
    static bool castToEnum(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags);
    static bool castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags);
    static bool castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToString(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags);
    static bool castFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags);
    static bool castToPointerRef(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castStructToStruct(SemanticContext* context, TypeInfoStruct* toStruct, TypeInfoStruct* fromStruct, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags, bool& ok);
    static bool castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToLambda(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToClosure(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToCode(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
    static bool makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, CastFlags castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags = 0);

    static void            convertStructParamToRef(AstNode* node, TypeInfo* typeInfo);
    static TypeInfoArray*  convertTypeListToArray(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant);
    static TypeInfoStruct* convertTypeListToStruct(JobContext* context, TypeInfoList* typeList);

    static bool      collectInterface(SemanticContext* context, TypeInfoStruct* fromTypeStruct, const TypeInfoStruct* toTypeItf, InterfaceRef& ref, bool skipFirst = false);
    static TypeInfo* resolveUntypedType(TypeInfo* typeInfo, uint32_t value);
    static TypeInfo* literalTypeToType(LiteralType literalType);
    static TypeInfo* literalTypeToType(LiteralType literalType, Register literalValue);
    static uint64_t  align(uint64_t value, uint32_t align);
    static uint32_t  alignOf(TypeInfo* typeInfo);

    static bool      promote(SemanticContext* context, AstNode* left, AstNode* right);
    static bool      promoteLeft(SemanticContext* context, AstNode* left, AstNode* right);
    static bool      promote32(SemanticContext* context, AstNode* left);
    static TypeInfo* promoteUntyped(TypeInfo* typeInfo);
    static void      promoteUntypedInteger(AstNode* left, const AstNode* right);

    static bool      compareConcreteType(const ExportedTypeInfo* type1, const ExportedTypeInfo* type2);
    static TypeInfo* concretePtrRefType(TypeInfo* typeInfo, ToConcreteFlags flags = CONCRETE_ALL);
    static TypeInfo* concreteType(const TypeInfo* typeInfo, ToConcreteFlags flags = CONCRETE_ALL);
    static TypeInfo* concretePtrRef(TypeInfo* typeInfo);
    static TypeInfo* concretePtrRefCond(TypeInfo* typeInfo, const AstNode* node);

    TypeInfo*             makeUnConst(TypeInfo* typeInfo);
    TypeInfo*             makeConst(TypeInfo* typeInfo);
    TypeInfo*             makeNullable(TypeInfo* typeInfo);
    TypeInfoPointer*      makePointerTo(TypeInfo* toType, const TypeInfoFlags& ptrFlags = 0);
    static TypeInfoParam* makeParam();
    void                  registerTypeType();

    Mutex mutex;

    struct PointerCache
    {
        TypeInfoFlags    flags;
        TypeInfoPointer* pointerTo;
    };

    Map<TypeInfo*, std::vector<PointerCache>> mapPointers;
    Map<TypeInfo*, TypeInfo*>                 mapConst;
    Map<TypeInfo*, TypeInfo*>                 mapUnConst;
    Map<TypeInfo*, TypeInfo*>                 mapNullable;
    Map<TypeInfo*, TypeInfo*>                 mapNotNullable;

    TypeInfoNative* typeInfoS8          = nullptr;
    TypeInfoNative* typeInfoS16         = nullptr;
    TypeInfoNative* typeInfoS32         = nullptr;
    TypeInfoNative* typeInfoS64         = nullptr;
    TypeInfoNative* typeInfoU8          = nullptr;
    TypeInfoNative* typeInfoU16         = nullptr;
    TypeInfoNative* typeInfoU32         = nullptr;
    TypeInfoNative* typeInfoU64         = nullptr;
    TypeInfoNative* typeInfoBool        = nullptr;
    TypeInfoNative* typeInfoF32         = nullptr;
    TypeInfoNative* typeInfoF64         = nullptr;
    TypeInfoNative* typeInfoRune        = nullptr;
    TypeInfoNative* typeInfoString      = nullptr;
    TypeInfoNative* typeInfoAny         = nullptr;
    TypeInfoNative* typeInfoVoid        = nullptr;
    TypeInfoNative* typeInfoUndefined   = nullptr;
    TypeInfoNative* typeInfoS32Promoted = nullptr;
    TypeInfoNative* typeInfoF32Promoted = nullptr;
    TypeInfoNative* typeInfoU32Promoted = nullptr;

    TypeInfoPointer* typeInfoNull    = nullptr;
    TypeInfoPointer* typeInfoCString = nullptr;

    TypeInfoPointer*  typeInfoTypeType      = nullptr;
    TypeInfoVariadic* typeInfoVariadic      = nullptr;
    TypeInfoVariadic* typeInfoTypedVariadic = nullptr;
    TypeInfoVariadic* typeInfoCVariadic     = nullptr;
    TypeInfoNative*   typeInfoUntypedInt    = nullptr;
    TypeInfoNative*   typeInfoUntypedBinHex = nullptr;
    TypeInfoNative*   typeInfoUntypedFloat  = nullptr;
    TypeInfoFuncAttr* typeInfoOpCall        = nullptr;
    TypeInfoFuncAttr* typeInfoModuleCall    = nullptr;
    TypeInfoFuncAttr* typeInfoOpCall2       = nullptr;
    TypeInfoCodeBlock*     typeInfoCode          = nullptr;
    TypeInfoSlice*    typeInfoSliceRunes    = nullptr;
    TypeInfoNative*   typeInfoCharacter     = nullptr;

    TypeInfoNative* promoteMatrix[static_cast<int>(NativeTypeKind::Count)][static_cast<int>(NativeTypeKind::Count)] = {{nullptr}};
};

extern TypeManager* g_TypeMgr;

template<typename T>
T* makeType(TypeInfoKind k = TypeInfoKind::Invalid)
{
    auto newType = Allocator::alloc<T>();
    if (k != TypeInfoKind::Invalid)
        newType->kind = k;

#ifdef SWAG_STATS
    g_Stats.memTypes += sizeof(T);
#ifdef SWAG_DEV_MODE
    SWAG_ASSERT(newType->kind != TypeInfoKind::Invalid);
    SWAG_ASSERT(static_cast<size_t>(newType->kind) < std::size(g_Stats.countTypesByKind));
    g_Stats.countTypesByKind[static_cast<int>(newType->kind)] += 1;
#endif
#endif

    return newType;
}

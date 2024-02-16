#pragma once
#include "Statistics.h"
#include "TypeInfo.h"
#include "Flags.h"

struct AstNode;
struct SemanticContext;
struct SourceFile;
struct Token;
struct TypeInfo;
struct TypeInfoNative;
struct TypeInfoVariadic;
enum class LiteralType : uint8_t;

using CastFlags = Flags<uint64_t>;
const CastFlags CAST_FLAG_DEFAULT            = 0x0000000000000000;
const CastFlags CAST_FLAG_INTERFACE          = 0x0000000000000001;
const CastFlags CAST_FLAG_EXPLICIT           = 0x0000000000000002;
const CastFlags CAST_FLAG_CONCRETE_ENUM      = 0x0000000000000004;
const CastFlags CAST_FLAG_JUST_CHECK         = 0x0000000000000008;
const CastFlags CAST_FLAG_UN_CONST           = 0x0000000000000010;
const CastFlags CAST_FLAG_AUTO_BOOL          = 0x0000000000000020;
const CastFlags CAST_FLAG_NO_COLLECT         = 0x0000000000000040;
const CastFlags CAST_FLAG_COMMUTATIVE        = 0x0000000000000080;
const CastFlags CAST_FLAG_INDEX              = 0x0000000000000100;
const CastFlags CAST_FLAG_FORCE_UN_CONST     = 0x0000000000000200;
const CastFlags CAST_FLAG_FOR_COMPARE        = 0x0000000000000400;
const CastFlags CAST_FLAG_UFCS               = 0x0000000000000800;
const CastFlags CAST_FLAG_COERCE             = 0x0000000000001000;
const CastFlags CAST_FLAG_TRY_COERCE         = 0x0000000000002000;
const CastFlags CAST_FLAG_AUTO_OP_CAST       = 0x0000000000004000;
const CastFlags CAST_FLAG_STRICT             = 0x0000000000008000;
const CastFlags CAST_FLAG_CAST               = 0x0000000000010000;
const CastFlags CAST_FLAG_FOR_AFFECT         = 0x0000000000020000;
const CastFlags CAST_FLAG_ACCEPT_PENDING     = 0x0000000000040000;
const CastFlags CAST_FLAG_LITERAL_SUFFIX     = 0x0000000000080000;
const CastFlags CAST_FLAG_NO_LAST_MINUTE     = 0x0000000000100000;
const CastFlags CAST_FLAG_FOR_GENERIC        = 0x0000000000200000;
const CastFlags CAST_FLAG_PARAMS             = 0x0000000000400000;
const CastFlags CAST_FLAG_NO_IMPLICIT        = 0x0000000000800000;
const CastFlags CAST_FLAG_FOR_VAR_INIT       = 0x0000000001000000;
const CastFlags CAST_FLAG_PTR_REF            = 0x0000000002000000;
const CastFlags CAST_FLAG_NO_TUPLE_TO_STRUCT = 0x0000000004000000;
const CastFlags CAST_FLAG_ACCEPT_MOVE_REF    = 0x0000000008000000;
const CastFlags CAST_FLAG_EXACT              = 0x0000000010000000;
const CastFlags CAST_FLAG_CAN_OVERFLOW       = 0x0000000020000000;
const CastFlags CAST_FLAG_EXACT_TUPLE_STRUCT = 0x0000000040000000;

using CastFlagsResult = Flags<uint32_t>;
const CastFlagsResult CAST_RESULT_STRUCT_CONVERT      = 0x00000001;
const CastFlagsResult CAST_RESULT_AUTO_OP_CAST        = 0x00000002;
const CastFlagsResult CAST_RESULT_AUTO_OP_AFFECT      = 0x00000004;
const CastFlagsResult CAST_RESULT_AUTO_MOVE_OP_AFFECT = 0x00000008;
const CastFlagsResult CAST_RESULT_GUESS_MOVE          = 0x00000010;
const CastFlagsResult CAST_RESULT_FORCE_REF           = 0x00000020;
const CastFlagsResult CAST_RESULT_UNTYPED_CONVERT     = 0x00000040;
const CastFlagsResult CAST_RESULT_COERCE              = 0x00000080;
const CastFlagsResult CAST_RESULT_GEN_AUTO_OP_CAST    = 0x00000100;
const CastFlagsResult CAST_RESULT_FROM_REF            = 0x00000200;
const CastFlagsResult CAST_RESULT_CONST_COERCE        = 0x00000400;

using ToConcreteFlags = Flags<uint32_t>;
const ToConcreteFlags CONCRETE_ALL         = 0xFFFFFFFF;
const ToConcreteFlags CONCRETE_ENUM        = 0x00000001;
const ToConcreteFlags CONCRETE_FUNC        = 0x00000002;
const ToConcreteFlags CONCRETE_GENERIC     = 0x00000004;
const ToConcreteFlags CONCRETE_ALIAS       = 0x00000008;
const ToConcreteFlags CONCRETE_FORCE_ALIAS = 0x00000010;

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
	static bool errorOutOfRange(SemanticContext* context, AstNode* fromNode, const TypeInfo* fromType, TypeInfo* toType, bool isNeg = false);
	static bool safetyComputedValue(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static void getCastErrorMsg(Utf8& msg, Utf8& hint, Vector<Utf8>& remarks, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags, CastErrorType castError = CastErrorType::Zero, bool forNote = false);
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
	static bool castToFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags);
	static bool castToPointerRef(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static bool castStructToStruct(SemanticContext* context, TypeInfoStruct* toStruct, TypeInfoStruct* fromStruct, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags, bool& ok);
	static bool castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static bool castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static bool castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static bool castToLambda(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static bool castToClosure(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static bool castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags);
	static bool makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, CastFlags castFlags = 0);
	static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags = 0);
	static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags = 0);

	static void            convertStructParamToRef(AstNode* node, TypeInfo* typeInfo);
	static TypeInfoArray*  convertTypeListToArray(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant);
	static TypeInfoStruct* convertTypeListToStruct(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant);

	static bool      collectInterface(SemanticContext* context, TypeInfoStruct* fromTypeStruct, const TypeInfoStruct* toTypeItf, InterfaceRef& ref, bool skipFirst = false);
	static TypeInfo* solidifyUntyped(TypeInfo* typeInfo);
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
	TypeInfoPointer*      makePointerTo(TypeInfo* toType, uint64_t ptrFlags = 0);
	static TypeInfoParam* makeParam();
	void                  registerTypeType();

	Mutex mutex;

	struct PointerCache
	{
		uint64_t         flags;
		TypeInfoPointer* pointerTo;
	};

	Map<TypeInfo*, vector<PointerCache>> mapPointers;
	Map<TypeInfo*, TypeInfo*>            mapConst;
	Map<TypeInfo*, TypeInfo*>            mapUnConst;

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

	TypeInfoPointer* typeInfoNull    = nullptr;
	TypeInfoPointer* typeInfoCString = nullptr;

	TypeInfoPointer*  typeInfoTypeType      = nullptr;
	TypeInfoVariadic* typeInfoVariadic      = nullptr;
	TypeInfoVariadic* typeInfoCVariadic     = nullptr;
	TypeInfoNative*   typeInfoUntypedInt    = nullptr;
	TypeInfoNative*   typeInfoUntypedBinHex = nullptr;
	TypeInfoNative*   typeInfoUntypedFloat  = nullptr;
	TypeInfoFuncAttr* typeInfoOpCall        = nullptr;
	TypeInfoFuncAttr* typeInfoModuleCall    = nullptr;
	TypeInfoFuncAttr* typeInfoOpCall2       = nullptr;
	TypeInfoCode*     typeInfoCode          = nullptr;
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

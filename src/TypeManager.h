#pragma once
#include "TypeInfo.h"
#include "Map.h"
#include "Statistics.h"
struct AstNode;
struct SourceFile;
struct TypeInfo;
struct TypeInfoNative;
struct TypeInfoVariadic;
struct SemanticContext;
enum class LiteralType : uint8_t;
struct Token;

const uint32_t CASTFLAG_DEFAULT            = 0x00000000;
const uint32_t CASTFLAG_INTERFACE          = 0x00000001;
const uint32_t CASTFLAG_EXPLICIT           = 0x00000002;
const uint32_t CASTFLAG_CONCRETE_ENUM      = 0x00000004;
const uint32_t CASTFLAG_JUST_CHECK         = 0x00000008;
const uint32_t CASTFLAG_UNCONST            = 0x00000010;
const uint32_t CASTFLAG_AUTO_BOOL          = 0x00000020;
const uint32_t CASTFLAG_NO_COLLECT         = 0x00000040;
const uint32_t CASTFLAG_COMMUTATIVE        = 0x00000080;
const uint32_t CASTFLAG_INDEX              = 0x00000100;
const uint32_t CASTFLAG_FORCE_UNCONST      = 0x00000200;
const uint32_t CASTFLAG_COMPARE            = 0x00000400;
const uint32_t CASTFLAG_UFCS               = 0x00000800;
const uint32_t CASTFLAG_COERCE             = 0x00001000;
const uint32_t CASTFLAG_TRY_COERCE         = 0x00002000;
const uint32_t CASTFLAG_AUTO_OPCAST        = 0x00004000;
const uint32_t CASTFLAG_STRICT             = 0x00008000;
const uint32_t CASTFLAG_CONST_ERR          = 0x00010000;
const uint32_t CASTFLAG_FOR_AFFECT         = 0x00020000;
const uint32_t CASTFLAG_ACCEPT_PENDING     = 0x00040000;
const uint32_t CASTFLAG_LITERAL_SUFFIX     = 0x00080000;
const uint32_t CASTFLAG_NO_LAST_MINUTE     = 0x00100000;
const uint32_t CASTFLAG_FOR_GENERIC        = 0x00200000;
const uint32_t CASTFLAG_PARAMS             = 0x00400000;
const uint32_t CASTFLAG_NO_IMPLICIT        = 0x00800000;
const uint32_t CASTFLAG_FROM_PROMOTE       = 0x01000000;
const uint32_t CASTFLAG_PTR_REF            = 0x02000000;
const uint32_t CASTFLAG_NO_TUPLE_TO_STRUCT = 0x04000000;
const uint32_t CASTFLAG_ACCEPT_MOVE_REF    = 0x08000000;
const uint32_t CASTFLAG_EXACT              = 0x10000000;
const uint32_t CASTFLAG_CAST               = 0x20000000;
const uint32_t CASTFLAG_FOR_VAR_INIT       = 0x40000000;

// Stored in SymbolMatchContext.flags
const uint32_t CASTFLAG_RESULT_STRUCT_CONVERT     = 0x01000000;
const uint32_t CASTFLAG_RESULT_AUTO_OPCAST        = 0x02000000;
const uint32_t CASTFLAG_RESULT_AUTO_OPAFFECT      = 0x04000000;
const uint32_t CASTFLAG_RESULT_AUTO_MOVE_OPAFFECT = 0x08000000;
const uint32_t CASTFLAG_RESULT_GUESS_MOVE         = 0x10000000;

const uint32_t CONCRETE_ALL        = 0xFFFFFFFF;
const uint32_t CONCRETE_ENUM       = 0x00000001;
const uint32_t CONCRETE_FUNC       = 0x00000002;
const uint32_t CONCRETE_GENERIC    = 0x00000004;
const uint32_t CONCRETE_ALIAS      = 0x00000008;
const uint32_t CONCRETE_FORCEALIAS = 0x00000010;

struct InterfaceRef
{
    TypeInfoParam* itf         = nullptr;
    uint32_t       fieldOffset = 0;
    Utf8           fieldRef;
};

struct TypeManager
{
    void setup();

    static bool errorOutOfRange(SemanticContext* context, AstNode* fromNode, TypeInfo* fromType, TypeInfo* toType, bool isNeg = false);
    static bool safetyComputedValue(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static void getCastErrorMsg(Utf8& msg, Utf8& hint, TypeInfo* toType, TypeInfo* fromType, uint32_t castFlags, bool forNote = false);
    static bool castError(SemanticContext* context, TypeInfo* requestedType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool tryOpCast(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool tryOpAffect(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static bool castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToNativeRune(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
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
    static bool castToFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags);
    static bool castToPointerRef(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castStructToStruct(SemanticContext* context, TypeInfoStruct* toStruct, TypeInfoStruct* fromStruct, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags, bool& ok);
    static bool castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToLambda(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToClosure(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);
    static bool castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags);

    static TypeInfoArray*  convertTypeListToArray(JobContext* jobContext, TypeInfoList* typeList, bool isCompilerConstant);
    static TypeInfoStruct* convertTypeListToStruct(JobContext* jobContext, TypeInfoList* typeList, bool isCompilerConstant);

    static bool makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags = 0);
    static bool makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags = 0);

    static bool      collectInterface(SemanticContext* context, TypeInfoStruct* fromTypeStruct, TypeInfoStruct* toTypeItf, InterfaceRef& ref, bool skipFirst = false);
    static TypeInfo* solidifyUntyped(TypeInfo* typeInfo);
    static TypeInfo* resolveUntypedType(TypeInfo* typeInfo, uint32_t value);
    static TypeInfo* literalTypeToType(LiteralType literalType);
    static TypeInfo* literalTypeToType(LiteralType literalType, Register literalValue);
    static void      promote3264(AstNode* left, AstNode* right);
    static void      promote816(AstNode* left, AstNode* right);
    static void      promoteOne(AstNode* left, AstNode* right, bool is3264);
    static bool      promoteOne(SemanticContext* context, AstNode* right);
    static TypeInfo* promoteUntyped(TypeInfo* typeInfo);
    static void      promoteUntypedInteger(AstNode* left, AstNode* right);
    static uint64_t  align(uint64_t value, uint32_t align);
    static uint32_t  alignOf(TypeInfo* typeInfo);
    static bool      compareConcreteType(const ExportedTypeInfo* type1, const ExportedTypeInfo* type2);
    static void      convertStructParamToRef(AstNode* node, TypeInfo* typeInfo);
    static TypeInfo* concretePtrRefType(TypeInfo* typeInfo, uint32_t flags = CONCRETE_ALL);
    static TypeInfo* concreteType(TypeInfo* typeInfo, uint32_t flags = CONCRETE_ALL);
    static TypeInfo* concretePtrRef(TypeInfo* typeInfo);
    static TypeInfo* concretePtrRefCond(TypeInfo* typeInfo, AstNode* node);
    TypeInfo*        makeUnConst(TypeInfo* typeInfo);
    TypeInfo*        makeConst(TypeInfo* typeInfo);
    TypeInfoPointer* makePointerTo(TypeInfo* toType, uint64_t ptrFlags = 0);
    TypeInfoParam*   makeParam();
    void             registerTypeType();

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

    TypeInfoPointer*  typeInfoTypeType       = nullptr;
    TypeInfoVariadic* typeInfoVariadic       = nullptr;
    TypeInfoVariadic* typeInfoCVariadic      = nullptr;
    TypeInfoNative*   typeInfoUntypedInt     = nullptr;
    TypeInfoNative*   typeInfoUntypedBinHexa = nullptr;
    TypeInfoNative*   typeInfoUntypedFloat   = nullptr;
    TypeInfoFuncAttr* typeInfoOpCall         = nullptr;
    TypeInfoFuncAttr* typeInfoModuleCall     = nullptr;
    TypeInfoFuncAttr* typeInfoOpCall2        = nullptr;
    TypeInfoCode*     typeInfoCode           = nullptr;
    TypeInfoSlice*    typeInfoSliceRunes     = nullptr;

    TypeInfoNative* promoteMatrix3264[(int) NativeTypeKind::Count][(int) NativeTypeKind::Count] = {{0}};
    TypeInfoNative* promoteMatrix816[(int) NativeTypeKind::Count][(int) NativeTypeKind::Count]  = {{0}};
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
    SWAG_ASSERT((size_t) newType->kind < sizeof(g_Stats.countTypesByKind) / sizeof(g_Stats.countTypesByKind[0]));
    g_Stats.countTypesByKind[(int) newType->kind] += 1;
#endif
#endif

    return newType;
}
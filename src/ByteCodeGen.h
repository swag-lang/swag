#pragma once

struct AstArrayPointerSlicing;
struct AstFuncCallParam;
struct AstFuncDecl;
struct AstNode;
struct AstRange;
struct AstVarDecl;
struct ByteCode;
struct ByteCodeInstruction;
struct ByteCodeGenContext;
struct DataSegment;
struct Job;
struct JobContext;
struct Module;
struct RegisterList;
struct Scope;
struct SemanticContext;
struct SourceFile;
struct SourceLocation;
struct SymbolName;
struct SymbolOverload;
struct TypeInfo;
struct TypeInfoArray;
struct TypeInfoFuncAttr;
struct TypeInfoPointer;
struct TypeInfoStruct;
enum class ByteCodeOp : uint16_t;
enum class TokenId : uint16_t;
enum class TypeInfoKind : uint8_t;

#define SAFETY_ZERO_EPSILON 0.00001f

constexpr uint32_t EMIT_CASTFLAG_DEFAULT  = 0x00000000;
constexpr uint32_t EMIT_CASTFLAG_EXPLICIT = 0x00000001;
constexpr uint32_t EMIT_CASTFLAG_AUTO     = 0x00000002;

enum class EmitOpUserKind
{
	Init,
	Drop,
	PostCopy,
	PostMove,
	Max
};

enum class SafetyMsg
{
	CastTruncated,
	CastNeg,
	Plus,
	Minus,
	Mul,
	PlusEq,
	MinusEq,
	MulEq,
	Neg,
	SwitchComplete,
	IndexRange,
	BadSlicingDown,
	BadSlicingUp,
	BadRangeDown,
	NotZero,
	InvalidBool,
	InvalidFloat,
	IntrinsicAbs,
	IntrinsicSqrt,
	IntrinsicLog,
	IntrinsicLog2,
	IntrinsicLog10,
	IntrinsicASin,
	IntrinsicACos,
	NullCheck,
	ErrCheck,
	CastAnyNull,
	CastAny,
	Count,
};

constexpr uint32_t ASKBC_WAIT_SEMANTIC_RESOLVED = 0x00000001;
constexpr uint32_t ASKBC_WAIT_DONE              = 0x00000002;
constexpr uint32_t ASKBC_WAIT_RESOLVED          = 0x00000004;

namespace ByteCodeGen
{
	bool setupRuntime(const ByteCodeGenContext* context, const AstNode* node);
	bool setupByteCodeGenerated(ByteCodeGenContext* context, AstNode* node);
	bool setupByteCodeResolved(const ByteCodeGenContext* context, AstNode* node);
	void askForByteCode(Job* job, AstNode* node, uint32_t flags, ByteCode* caller = nullptr);
	bool makeInline(ByteCodeGenContext* context, AstFuncDecl* funcDecl, AstNode* identifier);

	void getDependantCalls(const AstNode* depNode, VectorNative<AstNode*>& dep);
	void collectLiteralsChildren(AstNode* node, VectorNative<AstNode*>* orderedChildren);
	void computeSourceLocation(const JobContext* context, AstNode* node, uint32_t* storageOffset, DataSegment** storageSegment, bool forceCompiler = false);
	void releaseByteCodeJob(AstNode* node);
	bool checkCatchError(ByteCodeGenContext* context, AstNode* srcNode, const AstNode* callNode, const AstNode* funcNode, AstNode* parent, const TypeInfo* typeInfoFunc);
	bool sameStackFrame(ByteCodeGenContext* context, const SymbolOverload* overload);
	void freeStructParametersRegisters(ByteCodeGenContext* context);
	bool skipNodes(ByteCodeGenContext* context, AstNode* node);
	bool checkEscapedThrow(ByteCodeGenContext* context);

	ByteCodeInstruction* emitMakeSegPointer(const ByteCodeGenContext* context, const DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0);
	ByteCodeInstruction* emitGetFromSeg(const ByteCodeGenContext* context, const DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0, uint32_t numBits);
	ByteCodeInstruction* emitInstruction(const ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, const char* file, uint32_t line);

	bool emitDefaultParamValue(ByteCodeGenContext* context, AstNode* param, RegisterList& regList);
	void emitOpCallUser(const ByteCodeGenContext* context, const TypeInfoStruct* typeStruct, EmitOpUserKind kind, bool pushParam = true, uint32_t offset = 0, uint32_t numParams = 1);
	void emitOpCallUser(const ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc = nullptr, bool pushParam = true, uint32_t offset = 0, uint32_t numParams = 1);
	bool emitExpressionListBefore(ByteCodeGenContext* context);
	bool emitExpressionList(ByteCodeGenContext* context);
	bool emitLiteral(ByteCodeGenContext* context, AstNode* node, const TypeInfo* toType, RegisterList& regList);
	bool emitLiteral(ByteCodeGenContext* context);
	bool emitComputedValue(ByteCodeGenContext* context);
	bool emitBinaryOpPlus(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitBinaryOpMinus(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitBinaryOpMul(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitBinaryOpDiv(ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpPostSpecialFunc(const ByteCodeGenContext* context, TokenId op);
	bool emitCompareOpSpecialFunc(ByteCodeGenContext* context, AstNode* left, AstNode* right, const RegisterList& r0, const RegisterList& r1, TokenId op);
	bool emitInRange(ByteCodeGenContext* context, AstNode* left, AstNode* right, const RegisterList& r0, const RegisterList& r2);
	bool emitBinaryOpModulo(ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitLogicalAndAfterLeft(ByteCodeGenContext* context);
	bool emitLogicalAnd(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitLogicalOrAfterLeft(ByteCodeGenContext* context);
	bool emitLogicalOr(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitBinaryOp(ByteCodeGenContext* context);
	bool emitCompareOpEqual(const ByteCodeGenContext* context, AstNode* left, const AstNode* right, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2);
	bool emitCompareOpNotEqual(const ByteCodeGenContext* context, AstNode* left, const AstNode* right, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2);
	bool emitCompareOpEqual(const ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2);
	bool emitCompareOpNotEqual(const ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2);
	bool emitCompareOp3Way(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpLower(const ByteCodeGenContext* context, const AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpLower(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpLowerEq(const ByteCodeGenContext* context, const AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpLowerEq(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpGreater(const ByteCodeGenContext* context, const AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpGreater(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpGreaterEq(const ByteCodeGenContext* context, const AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOpGreaterEq(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitCompareOp(ByteCodeGenContext* context);
	void emitPostCallUfcs(ByteCodeGenContext* context);
	bool emitCall(ByteCodeGenContext* context);
	void emitPushRAParams(const ByteCodeGenContext* context, VectorNative<uint32_t>& accParams, bool forVariadic);
	bool emitReturnByCopyAddress(const ByteCodeGenContext* context, AstNode* node, const TypeInfoFuncAttr* typeInfoFunc);
	bool emitCall(ByteCodeGenContext* context, AstNode* allParams, AstFuncDecl* funcNode, AstVarDecl* varNode, RegisterList& varNodeRegisters, bool foreign, bool lambda, bool freeRegistersParams);
	bool emitLambdaCall(ByteCodeGenContext* context);
	bool emitForeignCall(ByteCodeGenContext* context);
	bool emitIntrinsicCVaStart(ByteCodeGenContext* context);
	bool emitIntrinsicCVaEnd(ByteCodeGenContext* context);
	bool emitIntrinsicCVaArg(ByteCodeGenContext* context);
	bool emitIntrinsic(ByteCodeGenContext* context);
	bool emitReturn(ByteCodeGenContext* context);
	bool emitIdentifierRef(ByteCodeGenContext* context);
	bool emitTry(ByteCodeGenContext* context);
	bool emitTryCatch(ByteCodeGenContext* context);
	bool emitCatch(ByteCodeGenContext* context);
	bool emitAssume(ByteCodeGenContext* context);
	bool emitInitStackTrace(ByteCodeGenContext* context);
	bool emitTryThrowExit(ByteCodeGenContext* context, AstNode* fromNode);
	bool emitThrow(ByteCodeGenContext* context);
	bool emitIdentifier(ByteCodeGenContext* context);
	bool emitLocalFuncDecl(ByteCodeGenContext* context);
	bool emitUnaryOpMinus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t rt, uint32_t r0);
	bool emitUnaryOpInvert(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t rt, uint32_t r0);
	bool emitUnaryOp(ByteCodeGenContext* context);
	bool emitBitmaskOr(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitBitmaskAnd(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitShiftLeft(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitShiftRight(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitXor(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
	bool emitExplicitCast(ByteCodeGenContext* context);
	bool emitExplicitAutoCast(ByteCodeGenContext* context);
	bool emitCastToNativeBool(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeS8(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeS16(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeS32(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeS64(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeU8(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeU16(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeU32(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeU64(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeF32(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeF64(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo);
	bool emitCastToNativeAny(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* fromTypeInfo);
	bool emitCastToNativeString(const ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* fromTypeInfo);
	bool emitCastToInterface(const ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo);
	bool emitCastToSlice(const ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo);
	bool emitCast(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, uint32_t emitCastFlags = EMIT_CASTFLAG_DEFAULT);
	bool emitFuncCallParam(ByteCodeGenContext* context);
	bool emitFuncDeclParams(ByteCodeGenContext* context);
	bool emitIfAfterExpr(ByteCodeGenContext* context);
	bool emitIfAfterIf(ByteCodeGenContext* context);
	bool emitInlineBefore(ByteCodeGenContext* context);
	bool emitInline(ByteCodeGenContext* context);
	bool emitIf(ByteCodeGenContext* context);
	bool emitWhileBeforeExpr(ByteCodeGenContext* context);
	bool emitWhileAfterExpr(ByteCodeGenContext* context);
	bool emitForBeforeExpr(ByteCodeGenContext* context);
	bool emitForAfterExpr(ByteCodeGenContext* context);
	bool emitForBeforePost(ByteCodeGenContext* context);
	bool emitSwitch(ByteCodeGenContext* context);
	bool emitBeforeSwitch(ByteCodeGenContext* context);
	bool emitSwitchAfterExpr(ByteCodeGenContext* context);
	bool emitSwitchCaseBeforeCase(ByteCodeGenContext* context);
	bool emitSwitchCaseBeforeBlock(ByteCodeGenContext* context);
	bool emitSafetyUnreachable(ByteCodeGenContext* context);
	bool emitSafetySwitchDefault(ByteCodeGenContext* context);
	bool emitSafetyValue(ByteCodeGenContext* context, int r, const TypeInfo* typeInfo);
	bool emitSwitchCaseAfterBlock(ByteCodeGenContext* context);
	bool emitIndex(ByteCodeGenContext* context);
	bool emitLoop(ByteCodeGenContext* context);
	bool emitLoopBeforeBlock(ByteCodeGenContext* context);
	bool emitLoopAfterExpr(ByteCodeGenContext* context);
	bool emitLabelBeforeBlock(ByteCodeGenContext* context);
	bool emitLoopAfterBlock(ByteCodeGenContext* context);
	bool emitCopyArray(ByteCodeGenContext* context, TypeInfo* typeInfo, const RegisterList& dstReg, const RegisterList& srcReg, AstNode* from);
	bool emitAffectEqual(ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, TypeInfo* forcedTypeInfo = nullptr, AstNode* from = nullptr);
	bool emitAffectPlusEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectMinusEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectMulEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectDivEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectPercentEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectAndEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectOrEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectXorEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectShiftLeftEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffectShiftRightEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	bool emitAffect(ByteCodeGenContext* context);
	bool emitBeforeFuncDeclContent(ByteCodeGenContext* context);
	void emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset, bool retVal);
	bool emitLocalVarDeclBefore(ByteCodeGenContext* context);
	bool emitLocalVarDecl(ByteCodeGenContext* context);
	void emitRetValRef(const ByteCodeGenContext* context, SymbolOverload* resolved, const RegisterList& r0, bool retVal, uint32_t stackOffset);
	bool emitStructInit(const ByteCodeGenContext* context, const TypeInfoStruct* typeInfoStruct, uint32_t regOffset, bool retVal);
	bool emitBreak(ByteCodeGenContext* context);
	bool emitFallThrough(ByteCodeGenContext* context);
	bool emitContinue(ByteCodeGenContext* context);
	bool emitMakePointer(ByteCodeGenContext* context);
	bool emitMakeArrayPointerSlicingUpperBound(ByteCodeGenContext* context);
	bool emitMakeArrayPointerSlicing(ByteCodeGenContext* context);
	bool emitMakeLambda(ByteCodeGenContext* context);
	bool emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo);
	bool emitPointerDeRef(ByteCodeGenContext* context);
	bool emitStructDeRef(ByteCodeGenContext* context, TypeInfo* typeInfo);
	bool emitPointerRef(ByteCodeGenContext* context);
	bool emitArrayRef(ByteCodeGenContext* context);
	bool emitStringRef(ByteCodeGenContext* context);
	bool emitSliceRef(ByteCodeGenContext* context);
	bool emitIntrinsicSpread(ByteCodeGenContext* context);
	bool emitIntrinsicCountOf(ByteCodeGenContext* context, AstNode* node, AstNode* expr);
	bool emitIntrinsicCountOf(ByteCodeGenContext* context);
	bool emitIntrinsicDataOf(ByteCodeGenContext* context);
	bool emitKindOf(const ByteCodeGenContext* context, AstNode* node, TypeInfoKind from);
	bool emitImplicitKindOfAny(ByteCodeGenContext* context);
	bool emitImplicitKindOfInterface(ByteCodeGenContext* context);
	bool emitIntrinsicKindOf(ByteCodeGenContext* context);
	bool emitIntrinsicLocationSI(ByteCodeGenContext* context);
	bool emitIntrinsicIsConstExprSI(ByteCodeGenContext* context);
	bool emitIntrinsicMakeAny(ByteCodeGenContext* context);
	bool emitIntrinsicMakeSlice(ByteCodeGenContext* context);
	bool emitIntrinsicMakeCallback(ByteCodeGenContext* context);
	bool emitIntrinsicMakeInterface(ByteCodeGenContext* context);
	bool emitUserOp(ByteCodeGenContext* context, AstNode* allParams = nullptr, AstNode* forNode = nullptr, bool freeRegisterParams = true);
	bool emitLeaveScope(ByteCodeGenContext* context);
	bool emitLeaveScopeDrop(const ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop = nullptr);
	bool emitDeferredStatements(ByteCodeGenContext* context, Scope* scope, bool forError);
	bool emitLeaveScopeReturn(ByteCodeGenContext* context, VectorNative<SymbolOverload*>* forceNoDrop, bool forError);
	bool computeLeaveScope(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop = nullptr, bool forError = false);
	bool emitConditionalOpAfterExpr(ByteCodeGenContext* context);
	bool emitConditionalOpAfterIfTrue(ByteCodeGenContext* context);
	bool emitConditionalOp(ByteCodeGenContext* context);
	bool emitNullConditionalOp(ByteCodeGenContext* context);
	bool emitPassThrough(ByteCodeGenContext* context);
	bool emitUnreachable(ByteCodeGenContext* context);
	void emitDebugLine(ByteCodeGenContext* context, AstNode* node);
	void emitDebugLine(ByteCodeGenContext* context);
	bool emitDebugNop(ByteCodeGenContext* context);
	bool emitInit(ByteCodeGenContext* context);
	bool emitInit(ByteCodeGenContext* context, TypeInfo* pointedType, RegisterList& rExpr, uint64_t numToInit, const AstNode* count, AstNode* parameters);
	bool emitDropCopyMove(ByteCodeGenContext* context);
	bool emitStruct(ByteCodeGenContext* context);
	void emitSetZeroAtPointer(const ByteCodeGenContext* context, uint64_t sizeOf, uint32_t registerIndex);
	void emitSetZeroStack(const ByteCodeGenContext* context, uint32_t offset, uint32_t sizeOf);
	void emitMemCpy(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint64_t sizeOf);
	bool emitDefer(ByteCodeGenContext* context);

	const char* safetyMsg(SafetyMsg msg, TypeInfo* toType = nullptr, TypeInfo* fromType = nullptr);
	void        emitAssert(ByteCodeGenContext* context, uint32_t reg, const char* message = nullptr);
	bool        mustEmitSafety(const ByteCodeGenContext* context, uint16_t what);
	void        emitSafetyNullCheck(ByteCodeGenContext* context, uint32_t r);
	void        emitSafetyErrCheck(ByteCodeGenContext* context, uint32_t r);
	void        emitSafetyNotZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits, const char* message);
	void        emitSafetyDivZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits);
	void        emitSafetyBoundCheckLowerU32(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	void        emitSafetyBoundCheckLowerU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	void        emitSafetyNeg(ByteCodeGenContext* context, uint32_t r0, TypeInfo* typeInfo, bool forAbs = false);
	void        emitSafetyBoundCheckSlice(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	void        emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, const TypeInfoArray* typeInfoArray);
	void        emitSafetyBoundCheckString(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
	void        emitSafetyCastAny(ByteCodeGenContext* context, const AstNode* exprNode, TypeInfo* toType);
	void        emitSafetyCastOverflow(ByteCodeGenContext* context, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, AstNode* exprNode);
	void        emitSafetyRange(ByteCodeGenContext* context, const AstRange* node);
	void        emitSafetyArrayPointerSlicing(ByteCodeGenContext* context, const AstArrayPointerSlicing* node);

	void generateStructAlloc(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct);
	bool generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct);
	void emitOpCallUserArrayOfStruct(const ByteCodeGenContext* context, TypeInfo* typeVar, EmitOpUserKind kind, bool pushParam, uint32_t offset);
	void emitOpCallUserFields(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct, EmitOpUserKind kind);
	bool generateStruct_opDrop(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct);
	bool generateStruct_opPostMove(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct);
	bool generateStruct_opPostCopy(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct);
	bool emitCopyStruct(ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, TypeInfo* typeInfo, AstNode* from);

	void     transformResultToLinear2(const ByteCodeGenContext* context, RegisterList& resultRegisterRC);
	uint32_t reserveRegisterRC(const ByteCodeGenContext* context, const SymbolOverload* overload = nullptr);
	void     reserveRegisterRC(const ByteCodeGenContext* context, RegisterList& rc, int num);
	void     reserveLinearRegisterRC2(const ByteCodeGenContext* context, RegisterList& rc);
	void     freeRegisterRC(const ByteCodeGenContext* context, RegisterList& rc);
	void     freeRegisterRC(const ByteCodeGenContext* context, uint32_t rc);
	void     freeRegisterRC(const ByteCodeGenContext* context, AstNode* node);
	void     truncRegisterRC(const ByteCodeGenContext* context, RegisterList& rc, uint32_t count);
	void     sortRegistersRC(const ByteCodeGenContext* context);
	void     ensureCanBeChangedRC(const ByteCodeGenContext* context, RegisterList& r0);
}

#define EMIT_INST0(__cxt, __op) ByteCodeGen::emitInstruction(__cxt, __op, 0, 0, 0, 0, __FILE__, __LINE__)
#define EMIT_INST1(__cxt, __op, __r0) ByteCodeGen::emitInstruction(__cxt, __op, __r0, 0, 0, 0, __FILE__, __LINE__)
#define EMIT_INST2(__cxt, __op, __r0, __r1) ByteCodeGen::emitInstruction(__cxt, __op, __r0, __r1, 0, 0, __FILE__, __LINE__)
#define EMIT_INST3(__cxt, __op, __r0, __r1, __r2) ByteCodeGen::emitInstruction(__cxt, __op, __r0, __r1, __r2, 0, __FILE__, __LINE__)
#define EMIT_INST4(__cxt, __op, __r0, __r1, __r2, __r3) ByteCodeGen::emitInstruction(__cxt, __op, __r0, __r1, __r2, __r3, __FILE__, __LINE__)

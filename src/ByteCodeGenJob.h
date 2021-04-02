#pragma once
#include "Job.h"
#include "DependentJobs.h"
struct AstNode;
struct SourceFile;
struct SemanticContext;
struct ByteCode;
struct AstNode;
struct ByteCodeGenJob;
struct ByteCodeInstruction;
struct TypeInfo;
struct Module;
struct RegisterList;
struct AstFuncDecl;
struct AstVarDecl;
struct TypeInfoStruct;
struct Scope;
struct SymbolName;
struct SymbolOverload;
struct SourceLocation;
struct TypeInfoArray;
struct AstNode;
struct AstFuncCallParam;
struct AstArrayPointerSlicing;
struct TypeInfoPointer;
enum class ByteCodeOp : uint16_t;

static const uint32_t BCC_FLAG_NOLOCATION = 0x00000001;
static const uint32_t BCC_FLAG_NOSAFETY   = 0x00000002;

struct ByteCodeGenContext : public JobContext
{
    VectorNative<AstNode*>        stackForceNode;
    VectorNative<SourceLocation*> stackForceLocation;
    ByteCode*                     bc;
    ByteCodeGenJob*               job;
    SourceLocation*               forceLocation     = nullptr;
    AstNode*                      forceNode         = nullptr;
    bool                          noLocation        = false;
    uint32_t                      contextFlags      = 0;
    uint16_t                      instructionsFlags = 0;
    uint32_t                      tryCatchScope     = 0;

    void setNoLocation()
    {
        noLocation = true;
    }

    void restoreNoLocation()
    {
        noLocation = false;
    }

    void pushLocation(SourceLocation* loc)
    {
        stackForceLocation.push_back(loc);
        forceLocation = loc;
    }

    void popLocation()
    {
        stackForceLocation.pop_back();
        if (!stackForceLocation.empty())
            forceLocation = stackForceLocation.back();
        else
            forceLocation = nullptr;
    }

    void pushNode(AstNode* pnode)
    {
        stackForceNode.push_back(pnode);
        forceNode = pnode;
    }

    void popNode()
    {
        stackForceNode.pop_back();
        if (!stackForceNode.empty())
            forceNode = stackForceNode.back();
        else
            forceNode = nullptr;
    }
};

struct PushLocation
{
    PushLocation(ByteCodeGenContext* bc, SourceLocation* loc)
    {
        savedBc = bc;
        bc->pushLocation(loc);
    }

    ~PushLocation()
    {
        savedBc->popLocation();
    }

    ByteCodeGenContext* savedBc;
};

struct PushNode
{
    PushNode(ByteCodeGenContext* bc, AstNode* pnode)
    {
        savedBc = bc;
        bc->pushNode(pnode);
    }

    ~PushNode()
    {
        savedBc->popNode();
    }

    ByteCodeGenContext* savedBc;
};

struct PushContextFlags
{
    PushContextFlags(ByteCodeGenContext* bc, uint32_t flags)
    {
        savedBc    = bc;
        savedFlags = bc->contextFlags;
        bc->contextFlags |= flags;
    }

    ~PushContextFlags()
    {
        savedBc->contextFlags = savedFlags;
    }

    ByteCodeGenContext* savedBc;
    uint32_t            savedFlags;
};

struct PushICFlags
{
    PushICFlags(ByteCodeGenContext* bc, uint16_t flags)
    {
        savedBc    = bc;
        savedFlags = bc->instructionsFlags;
        bc->instructionsFlags |= flags;
    }

    ~PushICFlags()
    {
        savedBc->instructionsFlags = savedFlags;
    }

    ByteCodeGenContext* savedBc;
    uint16_t            savedFlags;
};

static const uint32_t ASKBC_WAIT_SEMANTIC_RESOLVED = 0x00000001;
static const uint32_t ASKBC_WAIT_DONE              = 0x00000002;
static const uint32_t ASKBC_WAIT_RESOLVED          = 0x00000004;
static const uint32_t ASKBC_ADD_DEP_NODE           = 0x00000008;

struct ByteCodeGenJob : public Job
{
    JobResult execute() override;

    static bool                 internalError(ByteCodeGenContext* context, const char* msg, AstNode* node = nullptr);
    static ByteCodeInstruction* emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0 = 0, uint32_t r1 = 0, uint32_t r2 = 0, uint32_t r3 = 0);
    static void                 inherhitLocation(ByteCodeInstruction* inst, AstNode* node);
    static void                 askForByteCode(Job* job, AstNode* node, uint32_t flags);
    static void                 collectLiteralsChilds(AstNode* node, VectorNative<AstNode*>* orderedChilds);
    static uint32_t             computeSourceLocation(AstNode* node);
    static bool                 emitDefaultParamValue(ByteCodeGenContext* context, AstNode* param, RegisterList& regList);

    static bool canEmitOpCallUser(ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc = nullptr);
    static void emitOpCallUser(ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc = nullptr, bool pushParam = true, uint32_t offset = 0, uint32_t numParams = 1);
    static bool emitExpressionListBefore(ByteCodeGenContext* context);
    static bool emitExpressionList(ByteCodeGenContext* context);
    static bool emitLiteral(ByteCodeGenContext* context, AstNode* node, TypeInfo* toType, RegisterList& regList);
    static bool emitLiteral(ByteCodeGenContext* context);
    static bool emitBinaryOpPlus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpMinus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpMul(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpDiv(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpModulo(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitLogicalAndAfterLeft(ByteCodeGenContext* context);
    static bool emitLogicalAnd(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitLogicalOrAfterLeft(ByteCodeGenContext* context);
    static bool emitLogicalOr(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOp(ByteCodeGenContext* context);
    static bool emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2);
    static bool emitCompareOpNotEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2);
    static bool emitCompareOpEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2);
    static bool emitCompareOpNotEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2);
    static bool emitCompareOp3Way(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOpLowerEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOpGreater(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOpGreaterEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOp(ByteCodeGenContext* context);
    static bool emitCall(ByteCodeGenContext* context);
    static void emitPushRAParams(ByteCodeGenContext* context, VectorNative<uint32_t>& accParams);
    static bool checkCatchError(ByteCodeGenContext* context, AstNode* callNode, AstNode* funcNode, AstNode* parent, TypeInfo* typeInfoFunc);
    static bool emitCall(ByteCodeGenContext* context, AstNode* allParams, AstFuncDecl* funcNode, AstVarDecl* varNode, RegisterList& varNodeRegisters, bool foreign, bool freeRegistersParams = true);
    static bool emitLambdaCall(ByteCodeGenContext* context);
    static bool emitForeignCall(ByteCodeGenContext* context);
    static bool emitIntrinsic(ByteCodeGenContext* context);
    static bool emitReturn(ByteCodeGenContext* context);
    static bool emitIdentifierRef(ByteCodeGenContext* context);
    static bool emitTry(ByteCodeGenContext* context);
    static bool emitAssume(ByteCodeGenContext* context);
    static bool emitInitStackTrace(ByteCodeGenContext* context);
    static bool emitTryThrowExit(ByteCodeGenContext* context, AstNode* fromNode);
    static bool emitThrow(ByteCodeGenContext* context);
    static bool sameStackFrame(ByteCodeGenContext* context, SymbolOverload* overload);
    static bool emitIdentifier(ByteCodeGenContext* context);
    static bool emitLocalFuncDecl(ByteCodeGenContext* context);
    static bool emitUnaryOpMinus(ByteCodeGenContext* context, uint32_t r0);
    static bool emitUnaryOp(ByteCodeGenContext* context);
    static bool emitBitmaskOr(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBitmaskAnd(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitShiftLeft(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitShiftRight(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitXor(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitUnaryOpInvert(ByteCodeGenContext* context, uint32_t r0);
    static bool emitExplicitCast(ByteCodeGenContext* context);
    static bool emitExplicitAutoCast(ByteCodeGenContext* context);
    static bool emitCastToNativeBool(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeS8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeS16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeS32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeS64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeU8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeU16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeU32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeU64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeF32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeF64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastToNativeAny(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* fromTypeInfo);
    static bool emitCastToNativeString(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* fromTypeInfo);
    static bool emitCastToInterface(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo);
    static bool emitCastToSlice(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo);
    static bool emitCast(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, bool isExplicit = false);
    static bool emitFuncCallParam(ByteCodeGenContext* context);
    static bool emitFuncDeclParams(ByteCodeGenContext* context);
    static bool emitIfAfterExpr(ByteCodeGenContext* context);
    static bool emitIfAfterIf(ByteCodeGenContext* context);
    static bool emitInlineBefore(ByteCodeGenContext* context);
    static bool emitInline(ByteCodeGenContext* context);
    static bool emitIf(ByteCodeGenContext* context);
    static bool emitWhileBeforeExpr(ByteCodeGenContext* context);
    static bool emitWhileAfterExpr(ByteCodeGenContext* context);
    static bool emitForBeforeExpr(ByteCodeGenContext* context);
    static bool emitForAfterExpr(ByteCodeGenContext* context);
    static bool emitForBeforePost(ByteCodeGenContext* context);
    static bool emitSwitch(ByteCodeGenContext* context);
    static bool emitBeforeSwitch(ByteCodeGenContext* context);
    static bool emitSwitchAfterExpr(ByteCodeGenContext* context);
    static bool emitSwitchCaseBeforeCase(ByteCodeGenContext* context);
    static bool emitSwitchCaseBeforeBlock(ByteCodeGenContext* context);
    static bool emitSwitchCaseAfterBlock(ByteCodeGenContext* context);
    static bool emitGetErr(ByteCodeGenContext* context);
    static bool emitIndex(ByteCodeGenContext* context);
    static bool emitLoop(ByteCodeGenContext* context);
    static bool emitLoopAfterExpr(ByteCodeGenContext* context);
    static bool emitLabelBeforeBlock(ByteCodeGenContext* context);
    static bool emitLoopAfterBlock(ByteCodeGenContext* context);
    static bool emitCopyArray(ByteCodeGenContext* context, TypeInfo* typeInfo, RegisterList& dstReg, RegisterList& srcReg, AstNode* from);
    static bool emitAffectEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* forcedTypeInfo = nullptr, AstNode* from = nullptr);
    static bool emitAffectPlusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectMinusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectMulEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectDivEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectPercentEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectAndEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectOrEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectXorEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectShiftLeftEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectShiftRightEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffect(ByteCodeGenContext* context);
    static bool emitBeforeFuncDeclContent(ByteCodeGenContext* context);
    static void emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset, bool retVal);
    static void freeStructParametersRegisters(ByteCodeGenContext* context);
    static bool emitLocalVarDecl(ByteCodeGenContext* context);
    static void emitRetValRef(ByteCodeGenContext* context, RegisterList& r0, bool retVal, uint32_t stackOffset);
    static bool emitStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfo, uint32_t regOffset, bool retVal);
    static bool emitBreak(ByteCodeGenContext* context);
    static bool emitFallThrough(ByteCodeGenContext* context);
    static bool emitContinue(ByteCodeGenContext* context);
    static bool emitMakePointer(ByteCodeGenContext* context);
    static bool emitMakeArrayPointerSlicing(ByteCodeGenContext* context);
    static bool emitMakeLambda(ByteCodeGenContext* context);
    static bool emitWrapRelativePointer(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t sizeOf, TypeInfo* fromTypeInfo);
    static bool emitUnwrapRelativePointer(ByteCodeGenContext* context, uint32_t rr, uint32_t sizeOf);
    static bool emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo, bool safety = true);
    static bool emitPointerDeRef(ByteCodeGenContext* context);
    static bool emitStructDeRef(ByteCodeGenContext* context);
    static bool emitPointerRef(ByteCodeGenContext* context);
    static bool emitArrayRef(ByteCodeGenContext* context);
    static bool emitStringRef(ByteCodeGenContext* context);
    static bool emitSliceRef(ByteCodeGenContext* context);
    static bool emitIntrinsicSpread(ByteCodeGenContext* context);
    static bool emitIntrinsicCountOf(ByteCodeGenContext* context);
    static bool emitIntrinsicDataOf(ByteCodeGenContext* context);
    static bool emitIntrinsicKindOf(ByteCodeGenContext* context);
    static bool emitIntrinsicIsConstExpr(ByteCodeGenContext* context);
    static bool emitIntrinsicMakeAny(ByteCodeGenContext* context);
    static bool emitIntrinsicMakeSlice(ByteCodeGenContext* context);
    static bool emitIntrinsicMakeCallback(ByteCodeGenContext* context);
    static bool emitIntrinsicMakeForeign(ByteCodeGenContext* context);
    static bool emitIntrinsicMakeInterface(ByteCodeGenContext* context);
    static bool makeInline(ByteCodeGenContext* context, AstFuncDecl* funcDecl, AstNode* forNode);
    static bool emitUserOp(ByteCodeGenContext* context, AstNode* allParams = nullptr, AstNode* forNode = nullptr, bool freeRegisterParams = true);
    static bool emitLeaveScope(ByteCodeGenContext* context);
    static bool emitLeaveScopeDrop(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop = nullptr);
    static bool emitDeferredStatements(ByteCodeGenContext* context, Scope* scope, bool errDefer);
    static bool emitLeaveScopeReturn(ByteCodeGenContext* context, VectorNative<SymbolOverload*>* forceNoDrop, bool errDefer);
    static bool emitLeaveScope(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop = nullptr, bool errDefer = false);
    static bool emitConditionalOpAfterExpr(ByteCodeGenContext* context);
    static bool emitConditionalOpAfterIfTrue(ByteCodeGenContext* context);
    static bool emitConditionalOp(ByteCodeGenContext* context);
    static bool emitNullConditionalOp(ByteCodeGenContext* context);
    static bool emitPassThrough(ByteCodeGenContext* context);
    static bool emitDebugNop(ByteCodeGenContext* context);
    static bool emitInit(ByteCodeGenContext* context);
    static bool emitInit(ByteCodeGenContext* context, TypeInfoPointer* typeExpression, RegisterList& rExpr, uint64_t numToInit, AstNode* count, AstNode* parameters);
    static bool emitDropCopyMove(ByteCodeGenContext* context);
    static bool emitReloc(ByteCodeGenContext* context);
    static bool emitStruct(ByteCodeGenContext* context);
    static void emitSetZeroAtPointer(ByteCodeGenContext* context, uint64_t sizeOf, uint32_t registerIndex);
    static void emitMemCpy(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint64_t sizeOf);
    static bool emitDefer(ByteCodeGenContext* context);

    static void emitAssert(ByteCodeGenContext* context, uint32_t reg, const char* msg = nullptr);
    static bool mustEmitSafety(ByteCodeGenContext* context, uint64_t whatOn, uint64_t whatOff);
    static void emitSafetyNotZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits, const char* message);
    static void emitSafetyNullPointer(ByteCodeGenContext* context, uint32_t r, const char* message = "[safety] dereferencing a null pointer", int sizeInBits = 64);
    static void emitSafetyNullLambda(ByteCodeGenContext* context, uint32_t r, const char* message = "[safety] dereferencing a null pointer");
    static void emitSafetyLeftShift(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo);
    static void emitSafetyRightShift(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo);
    static void emitSafetyLeftShiftEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo);
    static void emitSafetyRightShiftEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo);
    static void emitSafetyDivZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits);
    static void emitSafetyBoundCheckLowerU32(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static void emitSafetyBoundCheckLowerU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static void emitSafetyRelativePointerS64(ByteCodeGenContext* context, uint32_t r0, int offsetSize);
    static void emitSafetyBoundCheckLowerEqU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static void emitSafetyBoundCheckSlice(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static void emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, TypeInfoArray* typeInfoArray);
    static void emitSafetyBoundCheckString(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static void emitSafetyCastAny(ByteCodeGenContext* context, AstNode* exprNode);
    static void emitSafetyCast(ByteCodeGenContext* context, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, AstNode* exprNode);
    static void emitSafetyArrayPointerSlicing(ByteCodeGenContext* context, AstArrayPointerSlicing* node);

    static bool generateStruct_opDrop(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool generateStruct_opPostMove(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool generateStruct_opPostCopy(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool generateStruct_opReloc(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool emitCopyStruct(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* typeInfo, AstNode* from);

    static void     transformResultToLinear2(ByteCodeGenContext* context, AstNode* node);
    static void     transformResultToLinear2(ByteCodeGenContext* context, RegisterList& resultRegisterRC);
    static uint32_t reserveRegisterRC(ByteCodeGenContext* context);
    static void     reserveRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num);
    static void     reserveLinearRegisterRC2(ByteCodeGenContext* context, RegisterList& rc);
    static void     freeRegisterRC(ByteCodeGenContext* context, RegisterList& rc);
    static void     freeRegisterRC(ByteCodeGenContext* context, uint32_t rc);
    static void     freeRegisterRC(ByteCodeGenContext* context, AstNode* node);
    static void     truncRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int count);
    static void     sortRegistersRC(ByteCodeGenContext* context);
    static void     ensureCanBeChangedRC(ByteCodeGenContext* context, RegisterList& r0);

    ByteCodeGenContext     context;
    VectorNative<AstNode*> collectChilds;
    VectorNative<Scope*>   collectScopes;

    enum class Pass
    {
        Generate,
        WaitForDependenciesGenerated,
        ComputeDependenciesResolved,
        WaitForDependenciesResolved,
    };

    AstNode*               allParamsTmp = nullptr;
    Pass                   pass         = Pass::Generate;
    VectorNative<AstNode*> dependentNodesTmp;

    void reset() override
    {
        Job::reset();
        context.reset();
        collectChilds.clear();
        collectScopes.clear();
        allParamsTmp = nullptr;
        pass         = Pass::Generate;
        dependentNodesTmp.clear();
    }

    void release() override
    {
        extern thread_local Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;
        g_Pool_byteCodeGenJob.release(this);
    }
};

extern thread_local Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;

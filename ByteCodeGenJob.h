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
enum class ByteCodeOp : uint16_t;

enum class ByteCodeResult
{
    Done,
    Pending,
    NewChilds,
};

struct ByteCodeGenContext
{
    ByteCode*       bc;
    ByteCodeGenJob* job;
    AstNode*        node;
    ByteCodeResult  result;
    SourceFile*     sourceFile = nullptr;
};

struct ByteCodeGenJob : public Job
{
    JobResult execute() override;
    void      setPending() override;

    static bool                 internalError(ByteCodeGenContext* context, const char* msg, AstNode* node = nullptr);
    static ByteCodeInstruction* emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0 = 0, uint32_t r1 = 0, uint32_t r2 = 0);
    static void                 inherhitLocation(ByteCodeInstruction* inst, AstNode* node);
    static void                 setupBC(Module* module, AstNode* node);
    static void                 askForByteCode(Job* job, AstFuncDecl* funcNode);
    static void                 collectLiteralsChilds(AstNode* node, vector<AstNode*>* orderedChilds);
    static bool                 emitDefaultParamValue(ByteCodeGenContext* context, AstNode* param, RegisterList& regList);

    static bool emitExpressionListBefore(ByteCodeGenContext* context);
    static bool emitExpressionList(ByteCodeGenContext* context);
    static bool emitLiteral(ByteCodeGenContext* context, AstNode* node, TypeInfo* toType, RegisterList& regList);
    static bool emitLiteral(ByteCodeGenContext* context);
    static bool emitBinaryOpPlus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpMinus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpMul(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpDiv(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpModulo(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOp(ByteCodeGenContext* context);
    static bool emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2);
    static bool emitCompareOpEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2);
    static bool emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOpGreater(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOp(ByteCodeGenContext* context);
    static bool emitCall(ByteCodeGenContext* context);
    static bool emitCall(ByteCodeGenContext* context, AstNode* allParams, AstFuncDecl* funcNode, AstVarDecl* varNode, bool foreign = false);
    static bool emitLambdaCall(ByteCodeGenContext* context);
    static bool emitForeignCall(ByteCodeGenContext* context);
    static bool emitIntrinsic(ByteCodeGenContext* context);
    static bool emitReturn(ByteCodeGenContext* context);
    static bool emitIdentifierRef(ByteCodeGenContext* context);
    static bool emitIdentifier(ByteCodeGenContext* context);
    static bool emitLocalFuncDecl(ByteCodeGenContext* context);
    static bool emitUnaryOpMinus(ByteCodeGenContext* context, uint32_t r0);
    static bool emitUnaryOp(ByteCodeGenContext* context);
    static bool emitBitmaskOr(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBitmaskAnd(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitShiftLeft(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitShiftRight(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitXor(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
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
    static bool emitCastSlice(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo);
    static bool emitCast(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo);
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
    static bool emitForAfterPost(ByteCodeGenContext* context);
    static bool emitSwitch(ByteCodeGenContext* context);
    static bool emitSwitchAfterExpr(ByteCodeGenContext* context);
    static bool emitSwitchCaseBeforeBlock(ByteCodeGenContext* context);
    static bool emitSwitchCaseAfterBlock(ByteCodeGenContext* context);
    static bool emitIndex(ByteCodeGenContext* context);
    static bool emitLoop(ByteCodeGenContext* context);
    static bool emitLoopAfterExpr(ByteCodeGenContext* context);
    static bool emitLoopAfterBlock(ByteCodeGenContext* context);
    static bool emitAffectEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* forcedTypeInfo = nullptr, AstNode* from = nullptr);
    static bool emitAffectPlusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectMinusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectMulEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectDivEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectPercentEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectAndEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectOrEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectXOrEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectShiftLeftEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffectShiftRightEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffect(ByteCodeGenContext* context);
    static bool emitBeforeFuncDeclContent(ByteCodeGenContext* context);
    static void emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset);
    static bool emitVarDecl(ByteCodeGenContext* context);
    static bool emitStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfo, uint32_t regOffset);
    static bool emitBreak(ByteCodeGenContext* context);
    static bool emitContinue(ByteCodeGenContext* context);
    static bool emitMakePointer(ByteCodeGenContext* context);
    static bool emitMakeLambda(ByteCodeGenContext* context);
    static bool emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo);
    static bool emitPointerDeRef(ByteCodeGenContext* context);
    static bool emitStructDeRef(ByteCodeGenContext* context);
    static bool emitPointerRef(ByteCodeGenContext* context);
    static bool emitArrayRef(ByteCodeGenContext* context);
    static bool emitSliceRef(ByteCodeGenContext* context);
    static bool emitCountOfProperty(ByteCodeGenContext* context);
    static bool emitDataOfProperty(ByteCodeGenContext* context);
    static bool emitKindOfProperty(ByteCodeGenContext* context);
    static bool makeInline(ByteCodeGenContext* context, AstFuncDecl* funcDecl, AstNode* forNode);
    static bool emitUserOp(ByteCodeGenContext* context, AstNode* allParams = nullptr, AstNode* forNode = nullptr);
    static bool emitDeferredStatements(ByteCodeGenContext* context);
    static bool emitLeaveScopeDrop(ByteCodeGenContext* context, Scope* scope);
    static bool emitDeferredStatements(ByteCodeGenContext* context, Scope* scope);
    static bool emitLeaveScope(ByteCodeGenContext* context, Scope* scope);
    static bool emitTrinaryOp(ByteCodeGenContext* context);
    static bool emitPassThrough(ByteCodeGenContext* context);
    static bool emitIs(ByteCodeGenContext* context);
    static bool emitInit(ByteCodeGenContext* context);
    static bool emitDrop(ByteCodeGenContext* context);
    static bool emitClearRefConstantSize(ByteCodeGenContext* context, uint32_t sizeOf, uint32_t registerIndex);

    static bool generateStruct_opDrop(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool generateStruct_opPostMove(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool generateStruct_opPostCopy(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfo);
    static bool prepareEmitStructDrop(ByteCodeGenContext* context, TypeInfo* typeInfo);
    static bool prepareEmitStructCopyMove(ByteCodeGenContext* context, TypeInfo* typeInfo);
    static bool emitStructCopyMoveCall(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* typeInfo, AstNode* from);
    static void emitStructCallFunc(ByteCodeGenContext* context, AstNode* funcNode, RegisterList* r0);

    static uint32_t reserveRegisterRC(ByteCodeGenContext* context);
    static void     reserveRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num);
    static void     reserveLinearRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num);
    static void     freeRegisterRC(ByteCodeGenContext* context, RegisterList& rc);
    static void     freeRegisterRC(ByteCodeGenContext* context, uint32_t rc);
    static void     freeRegisterRC(ByteCodeGenContext* context, AstNode* node);

    void reset() override
    {
        Job::reset();
        originalNode = nullptr;
        nodes.clear();
        collectChilds.clear();
        collectScopes.clear();
        syncToDependentNodes = false;
    }

    ByteCodeGenContext context;
    SourceFile*        sourceFile;
    AstNode*           originalNode;
    vector<AstNode*>   nodes;
    vector<AstNode*>   collectChilds;
    vector<Scope*>     collectScopes;
    bool               syncToDependentNodes;
};

extern Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;
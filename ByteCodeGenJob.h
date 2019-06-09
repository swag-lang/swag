#pragma once
#include "Job.h"
struct AstNode;
struct SourceFile;
struct SemanticContext;
struct ByteCode;
struct AstNode;
struct ByteCodeGenJob;
struct ByteCodeInstruction;
struct TypeInfo;
enum class ByteCodeOp : uint16_t;

enum class ByteCodeResult
{
    Done,
    Pending,
};

struct ByteCodeGenContext
{
    SemanticContext* semantic;
    ByteCode*        bc;
    ByteCodeGenJob*  job;
    AstNode*         node;
    ByteCodeResult   result;
    SourceFile*      sourceFile = nullptr;
};

struct ByteCodeGenJob : public Job
{
    JobResult execute() override;

    void reset() override
    {
        nodes.clear();
        dependentJobs.clear();
    }

    static bool                 internalError(ByteCodeGenContext* context, const char* msg);
    static ByteCodeInstruction* emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0 = 0, uint32_t r1 = 0, uint32_t r2 = 0);

    static bool emitLiteral(ByteCodeGenContext* context);
    static bool emitBinaryOpPlus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpMinus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpMul(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOpDiv(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBinaryOp(ByteCodeGenContext* context);
    static bool emitCompareOpEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOpGreater(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitCompareOp(ByteCodeGenContext* context);
    static bool emitLocalFuncCall(ByteCodeGenContext* context);
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
    static bool emitCast(ByteCodeGenContext* context);
    static bool emitCastNativeS8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeS16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeS32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeS64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeU8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeU16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeU32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeU64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeF32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCastNativeF64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo);
    static bool emitCast(ByteCodeGenContext* context, TypeInfo* typeInfo, AstNode* exprNode, TypeInfo* fromTypeInfo);
    static bool emitFuncCallParam(ByteCodeGenContext* context);
    static bool emitFuncCallParams(ByteCodeGenContext* context);
    static bool emitFuncDeclParams(ByteCodeGenContext* context);
    static bool emitIfAfterExpr(ByteCodeGenContext* context);
    static bool emitIfAfterIf(ByteCodeGenContext* context);
    static bool emitIf(ByteCodeGenContext* context);
    static bool emitAffectEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1);
    static bool emitAffect(ByteCodeGenContext* context);

    AstNode*         originalNode;
    SourceFile*      sourceFile;
    vector<AstNode*> nodes;
    vector<Job*>     dependentJobs;
};

extern Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;
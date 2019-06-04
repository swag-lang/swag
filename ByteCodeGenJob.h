#pragma once
#include "Job.h"
#include "ByteCodeOp.h"
struct AstNode;
struct SourceFile;
struct SemanticContext;
struct ByteCode;
struct AstNode;
struct ByteCodeGenJob;
struct ByteCodeInstruction;

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
    static bool emitIdentifier(ByteCodeGenContext* context);
    static bool emitLocalFuncCall(ByteCodeGenContext* context);
    static bool emitIntrinsic(ByteCodeGenContext* context);
    static bool emitReturn(ByteCodeGenContext* context);
    static bool emitIdentifierRef(ByteCodeGenContext* context);
    static bool emitLocalFuncDecl(ByteCodeGenContext* context);
    static bool emitUnaryOpMinus(ByteCodeGenContext* context, uint32_t r0);
    static bool emitUnaryOp(ByteCodeGenContext* context);
    static bool emitBitmaskOr(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitBitmaskAnd(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitShiftLeft(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);
    static bool emitShiftRight(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2);

    AstNode*         originalNode;
    SourceFile*      sourceFile;
    vector<AstNode*> nodes;
    vector<Job*>     dependentJobs;
};

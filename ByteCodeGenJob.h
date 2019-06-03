#pragma once
#include "Job.h"
#include "ByteCodeOp.h"
struct AstNode;
struct SourceFile;
struct SemanticContext;
struct ByteCode;
struct AstNode;
struct ByteCodeGenJob;

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
    bool             debugInfos = true;
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

    static bool internalError(ByteCodeGenContext* context);
    static void emitInstruction(ByteCodeGenContext* context, ByteCodeOp id);

    static bool emitLiteral(ByteCodeGenContext* context);
    static bool emitBinaryOpPlus(ByteCodeGenContext* context);
    static bool emitBinaryOpMinus(ByteCodeGenContext* context);
    static bool emitBinaryOpMul(ByteCodeGenContext* context);
    static bool emitBinaryOpDiv(ByteCodeGenContext* context);
    static bool emitBinaryOp(ByteCodeGenContext* context);
    static bool emitIdentifier(ByteCodeGenContext* context);
    static bool emitLocalFuncCall(ByteCodeGenContext* context);
    static bool emitIntrinsic(ByteCodeGenContext* context);
    static bool emitReturn(ByteCodeGenContext* context);

    AstNode*         originalNode;
    SourceFile*      sourceFile;
    vector<AstNode*> nodes;
    vector<Job*>     dependentJobs;
};

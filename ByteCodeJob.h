#pragma once
#include "Job.h"
struct AstNode;
struct SourceFile;
struct SemanticContext;
struct ByteCode;
struct AstNode;
struct ByteCodeJob;

enum class ByteCodeResult
{
    Done,
    Pending,
};

struct ByteCodeGenContext
{
    SemanticContext* semantic;
    ByteCode*        bc;
    ByteCodeJob*     job;
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
    }

    static bool internalError(ByteCodeGenContext* context, AstNode* node);
    static void emitInstruction(ByteCodeGenContext* context, AstNode* node, ByteCodeNodeId id);

    static bool emitLiteral(ByteCodeGenContext* context, AstNode* node);
    static bool emitRawNode(ByteCodeGenContext* context, AstNode* node);
    static bool emitNode(ByteCodeGenContext* context, AstNode* node);
    static bool emitBinaryOpPlus(ByteCodeGenContext* context, AstNode* node);
    static bool emitBinaryOpMinus(ByteCodeGenContext* context, AstNode* node);
    static bool emitBinaryOpMul(ByteCodeGenContext* context, AstNode* node);
    static bool emitBinaryOpDiv(ByteCodeGenContext* context, AstNode* node);
    static bool emitBinaryOp(ByteCodeGenContext* context, AstNode* node);
    static bool emitIdentifier(ByteCodeGenContext* context, AstNode* node);

    SourceFile*      sourceFile;
    vector<AstNode*> nodes;
};

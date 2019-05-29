#pragma once
#include "ByteCode.h"
#include "ByteCodeNodeId.h"
struct SemanticContext;
struct ByteCode;

struct ByteCodeGenContext
{
    SemanticContext* semantic;
    ByteCode*        bc;
    bool             debugInfos = true;
};

struct ByteCodeGen
{
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
};

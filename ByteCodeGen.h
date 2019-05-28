#pragma once
#include "ByteCode.h"
#include "ByteCodeNodeId.h"
struct SemanticContext;
struct ByteCode;

struct ByteCodeGenContext
{
    SemanticContext* semantic;
    ByteCode*        bc;
};

struct ByteCodeGen
{
    static bool internalError(ByteCodeGenContext* context, AstNode* node);
    static void addInstruction(ByteCodeGenContext* context, ByteCodeNodeId id);
    static bool emitLiteral(ByteCodeGenContext* context, AstNode* node);
    static bool emitRawNode(ByteCodeGenContext* context, AstNode* node);
    static bool emitNode(ByteCodeGenContext* context, AstNode* node);
};

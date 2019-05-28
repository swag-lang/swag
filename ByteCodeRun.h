#pragma once
struct ByteCode;
struct SemanticContext;
struct AstNode;
struct ConcatBucket;

struct ByteCodeRunContext
{
    ByteCode*     bc;
    ConcatBucket* executionBucket;
    int           executionOffset;
};

struct ByteCodeRun
{
    bool run(ByteCodeRunContext* context);
    bool executeNode(SemanticContext* context, AstNode* node);
};

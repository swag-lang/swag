#pragma once
#include "ByteCodeNodeId.h"
struct ByteCode;
struct SemanticContext;
struct AstNode;
struct ByteCodeRunContext;
struct ConcatBucket;

typedef uint8_t* (*RunNodeFct)(ByteCodeRunContext* context);

struct ByteCodeRun
{
    void setup();
    bool executeNode(ByteCodeRunContext* runContext, SemanticContext* context, AstNode* node);

    bool run(ByteCodeRunContext* context);

    static uint8_t* runPushS32(ByteCodeRunContext* context);

    map<ByteCodeNodeId, RunNodeFct> mapNodes;
};

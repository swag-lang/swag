#pragma once
struct ByteCodeRunContext;
struct ByteCodeInstruction;

struct ByteCodeRun
{
    bool internalError(ByteCodeRunContext* context);
    void runNode(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool run(ByteCodeRunContext* context);
};

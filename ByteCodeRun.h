#pragma once
struct ByteCodeRunContext;
struct ByteCodeInstruction;

struct ByteCodeRun
{
    bool internalError(ByteCodeRunContext* context);
    bool runNode(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool run(ByteCodeRunContext* context);
};

extern ByteCodeRun g_Run;
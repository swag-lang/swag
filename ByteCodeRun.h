#pragma once
struct ByteCodeRunContext;
struct ByteCodeInstruction;

struct ByteCodeRun
{
    bool internalError(ByteCodeRunContext* context);
    void ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool runNode(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool run(ByteCodeRunContext* context);

    condition_variable condVar;
    mutex              mutexDone;
};

extern ByteCodeRun g_Run;
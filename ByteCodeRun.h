#pragma once
#include "ffi.h"
struct ByteCodeRunContext;
struct ByteCodeInstruction;

struct ByteCodeRun
{
    bool  internalError(ByteCodeRunContext* context);
    void* ffiGetFuncAddress(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void  ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool  executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool  run(ByteCodeRunContext* context);

    vector<ffi_type*>  ffiArgs;
    vector<void*>      ffiArgsValues;
    condition_variable condVar;
    mutex              mutexDone;
};

extern ByteCodeRun g_Run;
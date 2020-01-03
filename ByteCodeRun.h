#pragma once
#include "ffi.h"
struct ByteCodeRunContext;
struct ByteCodeInstruction;
struct TypeInfo;
struct AstFuncDecl;

struct ByteCodeRun
{
    void*     ffiGetFuncAddress(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void*     ffiGetFuncAddress(ByteCodeRunContext* context, AstFuncDecl* nodeFunc);
    ffi_type* ffiFromTypeInfo(TypeInfo* typeInfo);
    void      ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip);

    bool executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool runLoop(ByteCodeRunContext* context);
    bool run(ByteCodeRunContext* runContext, bool& exception, int& exceptionCode);
    bool run(ByteCodeRunContext* runContext);

    vector<ffi_type*>  ffiArgs;
    vector<void*>      ffiArgsValues;
    condition_variable condVar;
    mutex              mutexDone;
};

extern ByteCodeRun g_Run;
#pragma once
#include "../ffi/ffi.h"
#include "VectorNative.h"
struct ByteCodeRunContext;
struct ByteCodeInstruction;
struct TypeInfo;
struct AstFuncDecl;
struct TypeInfoFuncAttr;
struct JobContext;
union Register;

struct ByteCodeRun
{
    void*     ffiGetFuncAddress(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void*     ffiGetFuncAddress(ByteCodeRunContext* context, AstFuncDecl* nodeFunc);
    ffi_type* ffiFromTypeInfo(TypeInfo* typeInfo);
    void      ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void      ffiCall(ByteCodeRunContext* context, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc);

    bool executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool runLoop(ByteCodeRunContext* context);
    bool run(ByteCodeRunContext* runContext);

    bool getVariadicSI(ByteCodeRunContext* context, ByteCodeInstruction* ip, Register* regPtr, Register* regCount);
    bool executeIsConstExprSI(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void executeGetFromStackSI(ByteCodeRunContext* context, ByteCodeInstruction* ip);

    static bool executeMathIntrinsic(JobContext* context, ByteCodeInstruction* ip, Register& ra, const Register& rb, const Register& rc);
};

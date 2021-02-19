#pragma once
#include "ffi/ffi.h"
#include "VectorNative.h"
struct ByteCodeRunContext;
struct ByteCodeInstruction;
struct TypeInfo;
struct AstFuncDecl;
struct TypeInfoFuncAttr;
struct JobContext;
struct ByteCode;
union Register;

struct ByteCodeRun
{
    static void* ffiGetFuncAddress(JobContext* context, ByteCodeInstruction* ip);
    static void* ffiGetFuncAddress(JobContext* context, AstFuncDecl* nodeFunc);
    ffi_type*    ffiFromTypeInfo(TypeInfo* typeInfo);
    void         ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void         ffiCall(ByteCodeRunContext* context, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc);

    bool executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    bool runLoop(ByteCodeRunContext* context);
    bool run(ByteCodeRunContext* runContext);

    bool getVariadicSI(ByteCodeRunContext* context, ByteCodeInstruction* ip, Register* regPtr, Register* regCount);
    bool executeIsConstExprSI(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void executeGetFromStackSI(ByteCodeRunContext* context, ByteCodeInstruction* ip);

    void         localCall(ByteCodeRunContext* context, ByteCode* bc, uint32_t popParamsOnRet = 0, uint32_t returnReg = UINT32_MAX);
    void         callInternalPanic(ByteCodeRunContext* context, ByteCodeInstruction* ip, const char* msg);
    static void* makeLambda(JobContext* context, AstFuncDecl* funcNode, ByteCode* bc);
    static bool  executeMathIntrinsic(JobContext* context, ByteCodeInstruction* ip, Register& ra, const Register& rb, const Register& rc);
};

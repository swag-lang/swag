#pragma once

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
    static void* ffiGetFuncAddress(JobContext* context, const ByteCodeInstruction* ip);
    static void* ffiGetFuncAddress(JobContext* context, AstFuncDecl* nodeFunc);
    static void  ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    static void  ffiCall(ByteCodeRunContext* context, const ByteCodeInstruction* ip, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc, int numCVariadicParams = 0);

    bool        executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip);
    void        runLoopNoDbg(ByteCodeRunContext* context);
    bool        runLoop(ByteCodeRunContext* context);
    static bool run(ByteCodeRunContext* runContext);

    static bool  getVariadicSI(const ByteCodeRunContext* context, const ByteCodeInstruction* ip, Register* regPtr, Register* regCount);
    static void* executeLocationSI(const ByteCodeRunContext* context, const ByteCodeInstruction* ip);
    static bool  executeIsConstExprSI(const ByteCodeRunContext* context, const ByteCodeInstruction* ip);
    void         executeGetFromStackSI(ByteCodeRunContext* context, const ByteCodeInstruction* ip);

    static void localCall(ByteCodeRunContext* context, ByteCode* bc, uint32_t popParamsOnRet = 0, uint32_t returnRegOnRet = UINT32_MAX, uint32_t incSPPosCall = 0);
    static void localCallNoTrace(ByteCodeRunContext* context, ByteCode* bc, uint32_t popParamsOnRet = 0, uint32_t returnRegOnRet = UINT32_MAX, uint32_t incSPPosCall = 0);
    static void callInternalCompilerError(ByteCodeRunContext* context, const ByteCodeInstruction* ip, const char* msg);
    static void        callInternalPanic(ByteCodeRunContext* context, const ByteCodeInstruction* ip, const char* msg);
    static void* makeLambda(JobContext* context, AstFuncDecl* funcNode, ByteCode* bc);
    static void  executeLeftShift(Register* rdest, const Register& rleft, const Register& rright, uint32_t numBits, bool isSigned);
    static void  executeRightShift(Register* rdest, const Register& rleft, const Register& rright, uint32_t numBits, bool isSigned);
    static bool  executeMathIntrinsic(JobContext* context, ByteCodeInstruction* ip, Register& ra, const Register& rb, const Register& rc, const Register& rd, bool runtime);
};

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
    bool internalError(ByteCodeRunContext* context);

    bool run(ByteCodeRunContext* context);

    static uint8_t* runPushBool(ByteCodeRunContext* context);
    static uint8_t* runPushS8(ByteCodeRunContext* context);
    static uint8_t* runPushS16(ByteCodeRunContext* context);
    static uint8_t* runPushS32(ByteCodeRunContext* context);
    static uint8_t* runPushS64(ByteCodeRunContext* context);
    static uint8_t* runPushU8(ByteCodeRunContext* context);
    static uint8_t* runPushU16(ByteCodeRunContext* context);
    static uint8_t* runPushU32(ByteCodeRunContext* context);
    static uint8_t* runPushU64(ByteCodeRunContext* context);
    static uint8_t* runPushF32(ByteCodeRunContext* context);
    static uint8_t* runPushF64(ByteCodeRunContext* context);
    static uint8_t* runPushString(ByteCodeRunContext* context);

    static uint8_t* runBinOpPlusS32(ByteCodeRunContext* context);
    static uint8_t* runBinOpPlusS64(ByteCodeRunContext* context);
    static uint8_t* runBinOpPlusU32(ByteCodeRunContext* context);
    static uint8_t* runBinOpPlusU64(ByteCodeRunContext* context);
    static uint8_t* runBinOpPlusF32(ByteCodeRunContext* context);
    static uint8_t* runBinOpPlusF64(ByteCodeRunContext* context);

    static uint8_t* runBinOpMinusS32(ByteCodeRunContext* context);
    static uint8_t* runBinOpMinusS64(ByteCodeRunContext* context);
    static uint8_t* runBinOpMinusU32(ByteCodeRunContext* context);
    static uint8_t* runBinOpMinusU64(ByteCodeRunContext* context);
    static uint8_t* runBinOpMinusF32(ByteCodeRunContext* context);
    static uint8_t* runBinOpMinusF64(ByteCodeRunContext* context);

    static uint8_t* runBinOpMulS32(ByteCodeRunContext* context);
    static uint8_t* runBinOpMulS64(ByteCodeRunContext* context);
    static uint8_t* runBinOpMulU32(ByteCodeRunContext* context);
    static uint8_t* runBinOpMulU64(ByteCodeRunContext* context);
    static uint8_t* runBinOpMulF32(ByteCodeRunContext* context);
    static uint8_t* runBinOpMulF64(ByteCodeRunContext* context);

    static uint8_t* runBinOpDivF32(ByteCodeRunContext* context);
    static uint8_t* runBinOpDivF64(ByteCodeRunContext* context);

    static uint8_t* runRet(ByteCodeRunContext* context);
    static uint8_t* runLocalFuncCall(ByteCodeRunContext* context);
    static uint8_t* runIntrinsicPrintS64(ByteCodeRunContext* context);
    static uint8_t* runIntrinsicPrintF64(ByteCodeRunContext* context);
    static uint8_t* runIntrinsicPrintChar(ByteCodeRunContext* context);
    static uint8_t* runIntrinsicPrintString(ByteCodeRunContext* context);
    static uint8_t* runIntrinsicAssert(ByteCodeRunContext* context);

    map<ByteCodeNodeId, RunNodeFct> mapNodes;
};

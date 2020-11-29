#pragma once
#include "ByteCode.h"
#include "ByteCodeOp.h"
struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Module;
struct Job;

struct ByteCodeOptContext
{
    ByteCode*                          bc;
    VectorNative<ByteCodeInstruction*> jumps;
    VectorNative<ByteCodeInstruction*> nops;
    VectorNative<ByteCodeInstruction*> tmpBufInst;
    VectorNative<uint64_t>             tmpBufU64;
    bool                               allPassesHaveDoneSomething = false;
    bool                               passHasDoneSomething       = false;
    bool                               hasError                   = false;
    ByteCodeGenContext*                semContext                 = nullptr;

    void reset()
    {
        bc = nullptr;
        jumps.clear();
        nops.clear();
        tmpBufInst.clear();
        tmpBufU64.clear();
        allPassesHaveDoneSomething = false;
        passHasDoneSomething       = false;
        hasError                   = false;
        semContext                 = nullptr;
    }
};

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

    VectorNative<ByteCodeInstruction*>                  vecInst;
    VectorNative<uint64_t>                              vecU64;
    map<uint32_t, uint32_t>                             mapU32U32;
    map<uint32_t, ByteCodeInstruction*>                 mapU32InstA;
    map<uint32_t, ByteCodeInstruction*>                 mapU32InstB;
    map<uint32_t, pair<uint64_t, ByteCodeInstruction*>> mapCst;

    bool                allPassesHaveDoneSomething = false;
    bool                passHasDoneSomething       = false;
    bool                hasError                   = false;
    ByteCodeGenContext* semContext                 = nullptr;

    void reset()
    {
        bc = nullptr;
        jumps.clear();
        nops.clear();
        vecInst.clear();
        vecU64.clear();
        mapU32U32.clear();
        mapU32InstA.clear();
        mapU32InstB.clear();
        mapCst.clear();
        allPassesHaveDoneSomething = false;
        passHasDoneSomething       = false;
        hasError                   = false;
        semContext                 = nullptr;
    }
};

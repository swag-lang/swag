#pragma once
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Job.h"

struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Module;
struct Job;

struct ByteCodeOptTreeNode
{
    ByteCodeInstruction* start = nullptr;
    ByteCodeInstruction* end   = nullptr;
    uint32_t             next1 = UINT32_MAX;
    uint32_t             next2 = UINT32_MAX;
    uint32_t             flags = 0;
};

struct ByteCodeOptContext;
struct ByteCodeOptTreeParseContext
{
    uint32_t             curNode;
    ByteCodeInstruction* startIp       = nullptr;
    ByteCodeInstruction* curIp         = nullptr;
    bool                 mustStopAll   = false;
    bool                 mustStopBlock = false;
    uint32_t             doneFlag      = 0;

    function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb;
};

struct ByteCodeOptContext : public JobContext
{
    ByteCode*                          bc;
    VectorNative<ByteCodeInstruction*> jumps;
    VectorNative<ByteCodeInstruction*> nops;

    vector<ByteCodeOptTreeNode>         tree;
    map<ByteCodeInstruction*, uint32_t> mapInstNode;

    VectorNative<ByteCodeInstruction*>                  vecInst;
    VectorNative<uint64_t>                              vecU64;
    map<uint32_t, uint32_t>                             mapU32U32;
    map<uint32_t, ByteCodeInstruction*>                 mapU32InstA;
    map<uint32_t, ByteCodeInstruction*>                 mapU32InstB;
    map<uint64_t, pair<uint64_t, ByteCodeInstruction*>> mapCst;

    bool allPassesHaveDoneSomething = false;
    bool passHasDoneSomething       = false;

    void reset()
    {
        JobContext::reset();
        bc = nullptr;
        jumps.clear();
        nops.clear();
        vecInst.clear();
        vecU64.clear();
        mapU32U32.clear();
        mapU32InstA.clear();
        mapU32InstB.clear();
        mapCst.clear();
        tree.clear();
        mapInstNode.clear();
        allPassesHaveDoneSomething = false;
        passHasDoneSomething       = false;
    }
};

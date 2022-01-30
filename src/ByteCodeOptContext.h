#pragma once
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Job.h"
#include "MapRegTo.h"

struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Module;
struct Job;

static const uint32_t BCOTN_USER1 = 0x00000001;
static const uint32_t BCOTN_USER2 = 0x00000002;

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
    Module*                            module;
    VectorNative<ByteCodeInstruction*> jumps;
    VectorNative<ByteCodeInstruction*> nops;

    vector<ByteCodeOptTreeNode>         tree;
    map<ByteCodeInstruction*, uint32_t> mapInstNode;

    VectorNative<ByteCodeInstruction*>                  vecInst;
    VectorNative<uint64_t>                              vecU64;
    VectorNative<uint32_t>                              vecReg;
    MapRegTo<uint32_t>                                  mapRegReg;
    MapRegTo<ByteCodeInstruction*>                      mapRegInstA;
    MapRegTo<ByteCodeInstruction*>                      mapRegInstB;
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
        mapRegReg.clear();
        mapRegInstA.clear();
        mapRegInstB.clear();
        mapCst.clear();
        tree.clear();
        mapInstNode.clear();
        vecReg.clear();
        allPassesHaveDoneSomething = false;
        passHasDoneSomething       = false;
    }
};

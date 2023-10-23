#pragma once
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Job.h"
#include "MapRegTo.h"
#include "Map.h"

struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Module;
struct Job;

const uint32_t BCOTN_USER1 = 0x00000001;
const uint32_t BCOTN_USER2 = 0x00000002;

struct ByteCodeOptTreeNode
{
    ByteCodeInstruction*   start = nullptr;
    ByteCodeInstruction*   end   = nullptr;
    VectorNative<uint32_t> next;
    VectorNative<uint32_t> parent;
    uint32_t               mark  = 0;
    uint32_t               flags = 0;
    uint32_t               crc   = 0;
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

const uint32_t OCF_HAS_COPY_RBRA = 0x00000001;
const uint32_t OCF_HAS_DUP_COPY  = 0x00000002;
const uint32_t OCF_HAS_COPY_RTRC = 0x00000004;
const uint32_t OCF_HAS_COPY_RCRT = 0x00000008;
const uint32_t OCF_HAS_INLINE    = 0x00000010;
const uint32_t OCF_HAS_PARAM     = 0x00000020;

struct ByteCodeOptContext : public JobContext
{
    VectorNative<ByteCodeInstruction*>                  jumps;
    VectorNative<ByteCodeInstruction*>                  nops;
    VectorNative<ByteCodeInstruction*>                  vecInst;
    VectorNative<uint32_t>                              vecReg;
    MapRegTo<uint32_t>                                  mapRegReg;
    MapRegTo<uint64_t>                                  mapRegU64;
    MapRegTo<ByteCodeInstruction*>                      mapRegInstA;
    MapRegTo<ByteCodeInstruction*>                      mapRegInstB;
    Map<uint64_t, pair<uint64_t, ByteCodeInstruction*>> mapCst;
    Map<uint64_t, uint32_t>                             map6432;
    Map<uint32_t, ByteCodeOptTreeNode*>                 map32Node;
    Vector<ByteCodeOptTreeNode>                         tree;
    Map<ByteCodeInstruction*, uint32_t>                 mapInstNode;
    Map<ByteCodeInstruction*, ByteCodeInstruction*>     mapInstInst;

    ByteCode* bc;
    Module*   module;

    uint32_t mark           = 0;
    uint32_t contextBcFlags = 0;
    uint32_t nextTreeNode   = 0;

    bool allPassesHaveDoneSomething = false;
    bool passHasDoneSomething       = false;
};

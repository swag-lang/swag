#pragma once
#include "Backend/ByteCode/ByteCode.h"
#include "Threading/Job.h"

struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;
struct Job;
struct Module;

constexpr uint32_t BCOTN_USER1 = 0x00000001;
constexpr uint32_t BCOTN_USER2 = 0x00000002;

struct ByteCodeOptTreeNode
{
    VectorNative<uint32_t> next;
    VectorNative<uint32_t> parent;

    ByteCodeInstruction* start = nullptr;
    ByteCodeInstruction* end   = nullptr;

    uint32_t mark     = 0;
    uint32_t flags    = 0;
    uint32_t crcBlock = 0;
};

struct ByteCodeOptContext;

struct ByteCodeOptTreeParseContext
{
    ByteCodeInstruction* startIp = nullptr;
    ByteCodeInstruction* curIp   = nullptr;

    uint32_t curNode  = 0;
    uint32_t doneFlag = 0;

    bool mustStopAll   = false;
    bool mustStopBlock = false;

    std::function<void(ByteCodeOptContext*, ByteCodeOptTreeParseContext&)> cb;
};

constexpr uint32_t OCF_HAS_COPY_RBRA = 0x00000001;
constexpr uint32_t OCF_HAS_DUP_COPY  = 0x00000002;
constexpr uint32_t OCF_HAS_COPY_RTRC = 0x00000004;
constexpr uint32_t OCF_HAS_COPY_RCRT = 0x00000008;
constexpr uint32_t OCF_HAS_INLINE    = 0x00000010;
constexpr uint32_t OCF_HAS_PARAM     = 0x00000020;
constexpr uint32_t OCF_HAS_ERR       = 0x00000040;

struct ByteCodeOptContext : JobContext
{
    void setDirtyPass() { passHasDoneSomething = true; }

    VectorNative<ByteCodeInstruction*>                       jumps;
    VectorNative<ByteCodeInstruction*>                       nops;
    VectorNative<ByteCodeInstruction*>                       vecInst;
    VectorNative<ByteCodeInstruction>                        vecInstCopy;
    VectorNative<uint32_t>                                   vecReg;
    Map<uint32_t, uint32_t>                                  mapRegReg;
    Map<uint32_t, uint64_t>                                  mapRegU64;
    Map<uint32_t, ByteCodeInstruction*>                      mapRegInstA;
    Map<uint32_t, ByteCodeInstruction*>                      mapRegInstB;
    Map<uint64_t, std::pair<uint64_t, ByteCodeInstruction*>> mapCst;
    Map<uint64_t, uint32_t>                                  map6432;
    Map<uint32_t, ByteCodeOptTreeNode*>                      map32Node;
    Vector<ByteCodeOptTreeNode>                              tree;
    Map<ByteCodeInstruction*, uint32_t>                      mapInstNode;
    Map<ByteCodeInstruction*, ByteCodeInstruction*>          mapInstInst;

    ByteCode* bc     = nullptr;
    Module*   module = nullptr;

    uint32_t mark           = 0;
    uint32_t contextBcFlags = 0;
    uint32_t nextTreeNode   = 0;

    bool allPassesHaveDoneSomething = false;
    bool passHasDoneSomething       = false;
};

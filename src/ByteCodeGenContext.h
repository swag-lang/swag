#pragma once
#include "Job.h"
#include "AstNode.h"

static const uint32_t BCC_FLAG_NOLOCATION   = 0x00000001;
static const uint32_t BCC_FLAG_NOSAFETY     = 0x00000002;
static const uint32_t BCC_FLAG_FOR_DEBUGGER = 0x00000004;

struct ByteCodeGenContext : public JobContext
{
    void release();
    void allocateTempCallParams();

    void setNoLocation()
    {
        noLocation = true;
    }

    void restoreNoLocation()
    {
        noLocation = false;
    }

    void pushLocation(SourceLocation* loc)
    {
        stackForceLocation.push_back(loc);
        forceLocation = loc;
    }

    void popLocation()
    {
        stackForceLocation.pop_back();
        if (!stackForceLocation.empty())
            forceLocation = stackForceLocation.back();
        else
            forceLocation = nullptr;
    }

    void pushNode(AstNode* pnode)
    {
        stackForceNode.push_back(pnode);
        forceNode = pnode;
    }

    void popNode()
    {
        stackForceNode.pop_back();
        if (!stackForceNode.empty())
            forceNode = stackForceNode.back();
        else
            forceNode = nullptr;
    }

    VectorNative<AstNode*> collectChilds;
    VectorNative<Scope*>   collectScopes;
    VectorNative<AstNode*> dependentNodesTmp;

    enum class Pass
    {
        Generate,
        WaitForDependenciesGenerated,
        ComputeDependenciesResolved,
    };

    AstNode* allParamsTmp = nullptr;
    Pass     pass         = Pass::Generate;

    VectorNative<AstNode*>        stackForceNode;
    VectorNative<SourceLocation*> stackForceLocation;

    ByteCode*       bc            = nullptr;
    SourceLocation* forceLocation = nullptr;
    AstNode*        forceNode     = nullptr;

    uint32_t contextFlags  = 0;
    uint32_t tryCatchScope = 0;

    uint16_t instructionsFlags = 0;
    bool     noLocation        = false;
};

struct PushLocation
{
    PushLocation(ByteCodeGenContext* bc, SourceLocation* loc)
    {
        savedBc = bc;
        bc->pushLocation(loc);
    }

    PushLocation(ByteCodeGenContext* bc, AstNode* node)
    {
        if (!node)
            return;
        savedBc = bc;
        bc->pushLocation(&node->token.startLocation);
    }

    ~PushLocation()
    {
        if (savedBc)
            savedBc->popLocation();
    }

    ByteCodeGenContext* savedBc = nullptr;
};

struct PushNode
{
    PushNode(ByteCodeGenContext* bc, AstNode* pnode)
    {
        savedBc = bc;
        bc->pushNode(pnode);
    }

    ~PushNode()
    {
        savedBc->popNode();
    }

    ByteCodeGenContext* savedBc;
};

struct PushContextFlags
{
    PushContextFlags(ByteCodeGenContext* bc, uint32_t flags)
    {
        savedBc    = bc;
        savedFlags = bc->contextFlags;
        bc->contextFlags |= flags;
    }

    ~PushContextFlags()
    {
        savedBc->contextFlags = savedFlags;
    }

    ByteCodeGenContext* savedBc;
    uint32_t            savedFlags;
};

struct PushICFlags
{
    PushICFlags(ByteCodeGenContext* bc, uint16_t flags)
    {
        savedBc    = bc;
        savedFlags = bc->instructionsFlags;
        bc->instructionsFlags |= flags;
    }

    ~PushICFlags()
    {
        savedBc->instructionsFlags = savedFlags;
    }

    ByteCodeGenContext* savedBc;
    uint16_t            savedFlags;
};

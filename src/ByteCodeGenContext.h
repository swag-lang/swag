#pragma once
#include "AstNode.h"
#include "Job.h"

static constexpr uint32_t BCC_FLAG_NO_LOCATION  = 0x00000001;
static constexpr uint32_t BCC_FLAG_NO_SAFETY    = 0x00000002;
static constexpr uint32_t BCC_FLAG_FOR_DEBUGGER = 0x00000004;

struct ByteCodeGenContext : JobContext
{
    ByteCodeGenContext()                                = default;
    ByteCodeGenContext(const ByteCodeGenContext& other) = default;

    void release() const;
    void allocateTempCallParams();

    void setNoLocation();
    void restoreNoLocation();
    void pushLocation(SourceLocation* loc);
    void popLocation();
    void pushNode(AstNode* myNode);
    void popNode();

    VectorNative<AstNode*>        collectChildren;
    VectorNative<Scope*>          collectScopes;
    VectorNative<AstNode*>        dependentNodesTmp;
    VectorNative<AstNode*>        stackForceNode;
    VectorNative<SourceLocation*> stackForceLocation;

    AstNode*        allParamsTmp  = nullptr;
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
    PushNode(ByteCodeGenContext* bc, AstNode* node)
    {
        savedBc = bc;
        bc->pushNode(node);
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

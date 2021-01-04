#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "Register.h"
#include "Assert.h"
#include "VectorNative.h"
#include "RaceCondition.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCodeRunContext;
struct ByteCode;

static const uint16_t BCI_JUMP_DEST      = 0x0001;
static const uint16_t BCI_DEBUG          = 0x0002;
static const uint16_t BCI_LOCATION_IS_BC = 0x0004;
static const uint16_t BCI_SAFETY         = 0x0008;
static const uint16_t BCI_IMM_A          = 0x0010;
static const uint16_t BCI_IMM_B          = 0x0020;
static const uint16_t BCI_IMM_C          = 0x0040;
static const uint16_t BCI_IMM_D          = 0x0080;
static const uint16_t BCI_OPT_FLAG       = 0x0100;
static const uint16_t BCI_START_STMT     = 0x0200;
static const uint16_t BCI_POST_COPYMOVE  = 0x0400;

struct ByteCodeInstruction
{
    Register a;
    Register b;
    Register c;
    Register d;
    AstNode* node;

    union
    {
        SourceLocation* location;
        uint32_t        locationBC;
    };

    ByteCodeOp op;
    uint16_t   flags;
};

#define MAX_CACHE_FREE_REG 8
struct ByteCode
{
    void addCallStack(ByteCodeRunContext* context);
    void enterByteCode(ByteCodeRunContext* context);
    void leaveByteCode(ByteCodeRunContext* context, bool popCallStack = true);
    void markLabels();

    void              printInstruction(ByteCodeInstruction* ip);
    void              print();
    Utf8              callName();
    TypeInfoFuncAttr* callType();
    static void       getLocation(ByteCode* bc, ByteCodeInstruction* ip, SourceFile** file, SourceLocation** location, bool force = false);

    static const int ALIGN_RIGHT_OPCODE = 25;

    VectorNative<uint32_t>            availableRegistersRC;
    VectorNative<uint32_t>            availableRegistersRC2;
    uint32_t                          regIsFree[MAX_CACHE_FREE_REG] = {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX};
    VectorNative<pair<void*, size_t>> autoFree;

    Utf8                   name;
    VectorNative<AstNode*> localVars;

    ByteCodeInstruction* out          = nullptr;
    SourceFile*          sourceFile   = nullptr;
    TypeInfoFuncAttr*    typeInfoFunc = nullptr;
    AstNode*             node         = nullptr;

    uint32_t numInstructions       = 0;
    uint32_t maxInstructions       = 0;
    int32_t  maxCallResults        = 0;
    uint32_t maxCallParams         = 0;
    uint32_t maxReservedRegisterRC = 0;
    uint32_t numJumps              = 0;

    bool         compilerGenerated = false;
    bool         isPostMove        = false;
    bool         addedToList       = false;
    atomic<bool> isEmpty;

    SWAG_RACE_CONDITION_INSTANCE(raceCondition);
};

#pragma once
#include "Backend/ByteCode/Register.h"
#include "Threading/Job.h"

struct ByteCode;
struct ByteCodeInstruction;

enum class SanityRefKind
{
    Invalid,
    Register,
    Stack,
};

enum class SanityValueKind : uint8_t
{
    Invalid,
    StackAddr,
    Constant,
    ZeroParam,
    Unknown,
};

struct SanityValue
{
    Register        reg;
    SanityValueKind kind = SanityValueKind::Invalid;

    bool isConstant() const
    {
        return kind == SanityValueKind::Constant || kind == SanityValueKind::ZeroParam;
    }
};

struct SanityState
{
    Vector<uint8_t>      stack;
    Vector<SanityValue>  stackValue;
    Vector<SanityValue>  regs;
    ByteCodeInstruction* branchIp = nullptr;
    ByteCodeInstruction* ip       = nullptr;
    uint32_t             parent   = UINT32_MAX;
};

struct SanityContext : JobContext
{
    ByteCode*                  bc    = nullptr;
    uint32_t                   state = 0;
    Set<ByteCodeInstruction*>  statesHere;
    VectorNative<SanityState*> states;
};

namespace ByteCodeSanity
{
    bool process(ByteCode* bc);
};

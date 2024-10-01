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
    ForceNull,
    Unknown,
};

struct SanityValue
{
    VectorNative<ByteCodeInstruction*> ips;
    Register                           reg;
    SanityValueKind                    kind = SanityValueKind::Invalid;

    bool isConstant() const
    {
        return kind == SanityValueKind::Constant || kind == SanityValueKind::ForceNull;
    }

    void update(ByteCodeInstruction* ip)
    {
        ips.push_back(ip);
    }

    void set(ByteCodeInstruction* ip)
    {
        ips.clear();
        ips.push_back(ip);
    }
};

struct SanityState
{
    Vector<uint8_t>        stack;
    Vector<SanityValue>    stackValue;
    Vector<SanityValue>    regs;
    VectorNative<uint32_t> forceParams0;
    VectorNative<uint32_t> forceParamsU;
    ByteCodeInstruction*   fromIp = nullptr;
    ByteCodeInstruction*   ip     = nullptr;
    uint32_t               parent = UINT32_MAX;
};

struct SanityContext : JobContext
{
    ByteCode*                  bc    = nullptr;
    uint32_t                   state = 0;
    Set<ByteCodeInstruction*>  statesHere;
    VectorNative<SanityState*> states;
};

struct ByteCodeSanity
{
    Diagnostic* raiseError(const Utf8& msg, const SanityValue* locValue = nullptr, AstNode* locNode = nullptr);

    bool checkOverflow(bool isValid, const char* msgKind, TypeInfo* type);
    bool checkDivZero(const SanityValue* value, bool isZero);
    bool checkEscapeFrame(const SanityValue* value);
    bool checkStackOffset(const SanityValue* value, uint64_t stackOffset, uint32_t sizeOf = 0);
    bool checkNotNull(const SanityValue* value);
    bool checkNotNullReturn(uint32_t reg);
    bool checkNotNullArguments(VectorNative<uint32_t> pushParams, const Utf8& intrinsic);
    bool checkStackInitialized(void* addr, uint32_t sizeOf, const SanityValue* locValue = nullptr);

    bool         getImmediateA(SanityValue& result);
    bool         getImmediateB(SanityValue& result);
    bool         getImmediateC(SanityValue& result);
    bool         getImmediateD(SanityValue& result);
    bool         getRegister(SanityValue*& result, uint32_t reg);
    bool         getStackValue(SanityValue* result, void* addr, uint32_t sizeOf);
    bool         getStackAddress(uint8_t*& result, const SanityValue* value, uint64_t stackOffset, uint32_t sizeOf = 0);
    void         setStackValue(void* addr, uint32_t sizeOf, SanityValueKind kind);
    void         updateStackValue(void* addr, uint32_t sizeOf);
    void         invalidateCurStateStack();
    SanityState* newState(ByteCodeInstruction* fromIp, ByteCodeInstruction* startIp);

    bool loop();
    bool process(ByteCode* bc);

    SanityContext context;
};

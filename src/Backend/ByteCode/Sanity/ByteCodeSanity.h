#pragma once
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"
#include "Threading/Job.h"

struct ByteCode;
struct ByteCodeInstruction;

enum class SanityRefKind
{
    Invalid,
    Register,
    Stack,
};

struct SanityState
{
    Vector<uint8_t>                    stack;
    Vector<SanityValue>                stackKind;
    Vector<SanityValue>                regs;
    VectorNative<uint32_t>             forceParams0;
    VectorNative<uint32_t>             forceParamsU;
    VectorNative<ByteCodeInstruction*> ips;
    ByteCodeInstruction*               fromIp = nullptr;
    ByteCodeInstruction*               ip     = nullptr;
    uint32_t                           parent = UINT32_MAX;
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
    bool checkStackOffset(uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue);
    bool checkNotNull(const SanityValue* value);
    bool checkNotNullReturn(uint32_t reg);
    bool checkNotNullArguments(VectorNative<uint32_t> pushParams, const Utf8& intrinsic);
    bool checkStackInitialized(void* addr, uint32_t sizeOf, const SanityValue* locValue = nullptr);

    bool         getImmediateA(SanityValue& result);
    bool         getImmediateB(SanityValue& result);
    bool         getImmediateC(SanityValue& result);
    bool         getImmediateD(SanityValue& result);
    bool         getRegister(SanityValue*& result, uint32_t reg);
    bool         getStackKind(SanityValue* result, void* stackAddr, uint32_t sizeOf);
    bool         getStackAddress(uint8_t*& result, uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue);
    void         setStackKind(void* stackAddr, uint32_t sizeOf, SanityValueKind kind, SanityValueFlags flags = SANITY_VALUE_FLAG_NONE);
    void         updateStackKind(void* addr, uint32_t sizeOf);
    void         invalidateCurStateStack();
    SanityState* newState(ByteCodeInstruction* fromIp, ByteCodeInstruction* startIp);

    static void backTrace(SanityState* state, uint32_t reg);
    bool        loop();
    bool        process(ByteCode* bc);

    SanityContext context;
};

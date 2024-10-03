#pragma once
#include "Backend/ByteCode/Sanity/ByteCodeSanityState.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"
#include "Threading/Job.h"

struct ByteCode;
struct ByteCodeInstruction;

struct SanityContext : JobContext
{
    ByteCode*                          bc    = nullptr;
    uint32_t                           state = 0;
    Set<ByteCodeInstruction*>          statesHere;
    VectorNative<ByteCodeSanityState*> states;
};

struct ByteCodeSanity
{
    static Diagnostic* raiseError(const ByteCodeInstruction* ip, const Utf8& msg, const SanityValue* locValue = nullptr, AstNode* locNode = nullptr);

    bool checkOverflow(bool isValid, const char* msgKind, TypeInfo* type);
    bool checkDivZero(const SanityValue* value, bool isZero);
    bool checkEscapeFrame(const SanityValue* value);
    bool checkNotNull(const SanityValue* value);
    bool checkNotNullReturn(uint32_t reg);
    bool checkNotNullArguments(VectorNative<uint32_t> pushParams, const Utf8& intrinsic);

    ByteCodeSanityState* newState(ByteCodeInstruction* fromIp, ByteCodeInstruction* startIp);

    static bool backTrace(ByteCodeSanityState* state, uint32_t reg);
    bool        loop();
    bool        process(ByteCode* bc);

    SanityContext context;
};

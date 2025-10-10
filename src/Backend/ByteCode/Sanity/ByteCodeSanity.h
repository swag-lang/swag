#pragma once
#include "Backend/ByteCode/Sanity/ByteCodeSanityState.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"
#include "Threading/Job.h"

enum class SafetyMsg;
using SafetyFlags = Flags<uint16_t>;
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
    bool               mustEmitSafety(SafetyFlags what) const;
    static Diagnostic* raiseError(const ByteCodeInstruction* ip, const Utf8& msg, const SanityValue* locValue, AstNode* locNode = nullptr);
    static Diagnostic* raiseError(const ByteCodeInstruction* ip, Diagnostic* err, const SanityValue* locValue = nullptr, AstNode* locNode = nullptr);

    bool checkOverflow(bool isValid, SafetyMsg msgKind, const TypeInfo* type, const void* val0, const void* val1);
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

#pragma once
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"

struct ByteCodeSanity;
struct ByteCodeInstruction;

struct ByteCodeSanityState
{
    bool checkStackOffset(uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue) const;
    bool checkStackInitialized(void* addr, uint32_t sizeOf, const SanityValue* locValue = nullptr) const;

    bool getStackKind(SanityValue* result, void* stackAddr, uint32_t sizeOf) const;
    bool getStackAddress(uint8_t*& result, uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue);
    void setStackKind(void* stackAddr, uint32_t sizeOf, SanityValueKind kind, SanityValueFlags flags = SANITY_VALUE_FLAG_NONE);
    void updateStackKind(void* addr, uint32_t sizeOf);
    void invalidateStack();

    ByteCodeSanity*                    san = nullptr;
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

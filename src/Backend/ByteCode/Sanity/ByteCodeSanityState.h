#pragma once
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"

struct ByteCodeSanity;
struct ByteCodeInstruction;

struct ByteCodeSanityState
{
    bool getImmediateA(SanityValue& result, const ByteCodeInstruction *ipn = nullptr);
    bool getImmediateB(SanityValue& result, const ByteCodeInstruction *ipn = nullptr);
    bool getImmediateC(SanityValue& result, const ByteCodeInstruction *ipn = nullptr);
    bool getImmediateD(SanityValue& result, const ByteCodeInstruction *ipn = nullptr);
    bool getRegister(SanityValue*& result, uint32_t reg);
    
    bool checkStackOffset(uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue) const;
    bool checkStackInitialized(void* addr, uint32_t sizeOf, const SanityValue* locValue = nullptr) const;

    bool getStackKind(SanityValue* result, void* stackAddr, uint32_t sizeOf) const;
    bool getStackAddress(uint8_t*& result, uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue);
    void setStackKind(void* stackAddr, uint32_t sizeOf, SanityValueKind kind, SanityValueFlags flags = SANITY_VALUE_FLAG_NONE);
    void setStackIps(void* addr, uint32_t sizeOf, bool clear);
    void updateStackIps(void* addr, uint32_t sizeOf, const SanityValue* from);
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

#pragma once
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"

struct ByteCodeInstruction;

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

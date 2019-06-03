#include "pch.h"
#include "ByteCodeRunContext.h"

void ByteCodeRunContext::setup(uint32_t numRC, uint32_t numRR, uint32_t stackS)
{
    if (numRegistersRC < numRC)
    {
        numRegistersRC = numRC;
        registersRC    = (Register*) realloc(registersRC, numRC * sizeof(Register));
    }

    if (numRegistersRR < numRR)
    {
        numRegistersRR = numRR;
        registersRR    = (Register*) realloc(registersRR, numRR * sizeof(Register));
    }

    if (stackSize < stackS)
    {
        stackSize = stackS;
        stack     = (uint8_t*) realloc(stack, stackS);
    }

    bp = stack + stackSize;
    sp = bp;
}

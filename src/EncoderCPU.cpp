#include "pch.h"
#include "EncoderCPU.h"

void EncoderCPU::clearInstructionCache()
{
    storageRegCount = UINT32_MAX;
    storageRegStack = 0;
    storageRegBits  = 0;
    storageMemReg   = RAX;
    storageReg      = RAX;
}
